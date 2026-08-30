// lxy_sort.hpp - Stable, low-overhead, adaptive hybrid sorting (header-only)
//
// ===========================================================================
// Public API
// ===========================================================================
//   lxySort(a [, comp])            DEFAULT sort: max speed. Arithmetic/string use
//                                 stable linear sorts (radix/counting/MSD-radix);
//                                 tiny arrays & generic types use introsort like
//                                 std::sort (fast, but NOT strictly stable there).
//   lxyStableSort(a [, comp])       strictly stable (merge for the comparison path)
//   lxySortUnstable(a [, comp])    max-speed alias
//   lxySortParallel(a [, comp])    parallel sort (comparison-heavy; needs -fopenmp)
//   lxySortByKey(a, keyFunc)       stable sort by an extracted key
//                                 (keyFunc returns a scalar arithmetic key or a
//                                  std::tuple of arithmetic keys for multi-key)
//
// ===========================================================================
// What it is good at  (real-world, vs std::sort on a multicore x86, g++ -O2)
// ===========================================================================
//   - Arithmetic types (int/float/double...): RADIX sort O(n) vs std's O(n log n)
//     -> typically 2-5x faster on random large arrays; much more on structured data.
//   - Small data range: COUNTING sort O(n) -> ~5x faster (e.g. Random 0..999).
//   - Already sorted / all-same / strictly descending: O(n) detection -> 5-18x.
//   - Nearly sorted: NATURAL MERGE O(n log runs) -> 1.5-4x.
//   - std::string: MSD radix O(n*L) -> faster than std on random strings.
//   - Parallel (lxySortParallel): ~2-3x extra on CPU-bound comparison sorts.
//
// ===========================================================================
// Honest limitations (read before adopting)
// ===========================================================================
//   - Stable sort of generic/expensive-to-copy types (e.g. random std::string) is
//     slower than std::sort's unstable introsort (same trade-off as std::stable_sort).
//     Use lxySortUnstable for max speed there, or lxySortParallel for structs.
//   - Tiny arrays (n <= ~128) lose marginally to std::sort (sub-microsecond noise).
//   - Radix/counting/string-radix are memory-bandwidth-bound: parallelism adds
//     little for them (parallel only helps comparison-heavy / custom-comparator).
//   - Uses O(n) extra workspace (scratch buffers, reused thread-local); not in-place.
//   - Linear sorts only apply when the comparator is std::less<T> and T is
//     arithmetic (their ordering == numeric order); custom comparators fall back
//     to comparison sorts.
//
// ===========================================================================
// Complexity
// ===========================================================================
//   - Random large arithmetic / small-range / nearly-sorted / sorted-reverse: O(n)
//   - Generic comparable types (string, structs): O(n log n)  [stable merge]
//   - Worst case: O(n log n);   extra space: O(n)
//
// ===========================================================================
// Implementation notes
// ===========================================================================
//   A single O(n) detection scan picks the fastest algorithm:
//     n <= 1                                   : O(1)
//     already sorted / all-same                : O(n) early return
//     strictly descending                      : O(n) reverse
//     tiny array (n <= 128, early-exit scan)   : insertion / merge (stable) | introsort (unstable)
//     integral, small range (range <= n)       : counting sort   O(n + range)   [stable]
//     nearly sorted (few descents)             : natural merge   O(n log runs)  [stable]
//     arithmetic, wide range (n >= 512)        : radix sort      O(n)           [stable]
//     std::string                              : MSD radix sort O(n * length)   [stable]
//     generic / custom-comparator              : merge sort (stable) | introsort (unstable)
//   All linear sorts and merge/natural-merge/insertion are stable; introsort is not.
//   Scratch buffers are reused from a thread-local workspace.

#pragma once
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdint>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <string>
#include <tuple>
#include <iterator>
#ifdef _OPENMP
#include <omp.h>
#endif

namespace lxy_detail {

// ---------------------------------------------------------------------------
// Thread-local reusable workspace (avoids per-call allocation)
// ---------------------------------------------------------------------------
struct Workspace {
    std::vector<std::uint32_t> k32a, k32b;      // radix key buffers (32-bit types)
    std::vector<std::uint64_t> k64a, k64b;      // radix key buffers (64-bit types)
    std::vector<int> cnt;                       // counting / radix counts (4-aligned)
    std::vector<std::max_align_t> merge;        // max-aligned scratch for merge/counting temp
    void ensureCnt(size_t n)              { if (cnt.size() < n)  cnt.resize(n); }
    void ensureKeys32(size_t n)           { if (k32a.size() < n) { k32a.resize(n); k32b.resize(n); } }
    void ensureKeys64(size_t n)           { if (k64a.size() < n) { k64a.resize(n); k64b.resize(n); } }
    void ensureMerge(size_t bytes) {
        size_t units = (bytes + sizeof(std::max_align_t) - 1) / sizeof(std::max_align_t);
        if (merge.size() < units) merge.resize(units);
    }
};
inline Workspace& ws() { static thread_local Workspace w; return w; }

// guards against nested auto-parallel (per thread; worker threads set it =1)
inline int& parDepth() { static thread_local int d = 0; return d; }

// auto-parallel threshold for comparison-heavy generic arrays
inline constexpr long long AUTO_PAR_THRESHOLD = 1LL << 20;   // n >= 1M

// is F a "key extractor" (callable with one element, returning a non-bool key)
// rather than a comparator (callable with two elements, returning bool)?
template<typename T, typename F, typename = void>
struct is_keyfunc : std::false_type {};
template<typename T, typename F>
struct is_keyfunc<T, F, std::void_t<decltype(std::declval<F>()(std::declval<const T&>()))>>
    : std::bool_constant<!std::is_same<
          decltype(std::declval<F>()(std::declval<const T&>())), bool>::value> {};

// arithmetic type eligible for linear (counting/radix) sorts
template<typename T>
inline constexpr bool radix_able = std::is_arithmetic<T>::value && !std::is_same<T, bool>::value;

// is T a std::basic_string? (eligible for MSD radix sort on characters)
template<typename T> struct is_string_like : std::false_type {};
template<typename Char, typename Traits, typename Alloc>
struct is_string_like<std::basic_string<Char, Traits, Alloc>> : std::true_type {};

// is T a std::vector of an arithmetic type? (eligible for byte-level MSD radix)
template<typename T> struct is_vector_arith : std::false_type {};
template<typename A> struct is_vector_arith<std::vector<A>> : std::bool_constant<radix_able<A>> {};

// is T a std::tuple?
template<typename T> struct is_std_tuple : std::false_type {};
template<typename... Ts> struct is_std_tuple<std::tuple<Ts...>> : std::true_type {};

// are all components of a tuple arithmetic? (eligible for LSD linear sort)
template<typename Tuple, size_t... Is>
inline constexpr bool tuple_all_arith_seq(std::index_sequence<Is...>) {
    return (radix_able<std::tuple_element_t<Is, Tuple>> && ...);
}
template<typename Tuple, bool IsTuple>
struct tuple_arith_impl;
template<typename Tuple>
struct tuple_arith_impl<Tuple, true> {
    static constexpr bool value = tuple_all_arith_seq<Tuple>(
        std::make_index_sequence<std::tuple_size<Tuple>::value>{});
};
template<typename Tuple>
struct tuple_arith_impl<Tuple, false> { static constexpr bool value = false; };
template<typename T>
inline constexpr bool tuple_all_arith = tuple_arith_impl<T, is_std_tuple<T>::value>::value;

// key type for an arithmetic T: uint32 for <=4-byte types, uint64 otherwise
// (halves memory traffic for int/float compared to always using uint64)
template<typename T>
using key_type = std::conditional_t<(sizeof(T) <= 4), std::uint32_t, std::uint64_t>;

// order-preserving bijection T -> unsigned key for arithmetic types.
template<typename T>
inline key_type<T> toKey(T x) {
    const unsigned W = unsigned(sizeof(T) * 8);
    using U = key_type<T>;
    U bits;
    std::memcpy(&bits, &x, sizeof(T));
    if constexpr (std::is_integral<T>::value) {
        if constexpr (std::is_signed<T>::value)
            bits ^= (U(1) << (W - 1));          // flip sign bit (signed order)
    } else {                                    // IEEE-754 -> comparable unsigned
        const U sign = U(1) << (W - 1);
        bits ^= (bits >> (W - 1)) ? ~U(0) : sign;
    }
    return bits;
}
template<typename T>
inline T fromKey(key_type<T> key) {
    const unsigned W = unsigned(sizeof(T) * 8);
    using U = key_type<T>;
    U bits = key;
    if constexpr (std::is_integral<T>::value) {
        if constexpr (std::is_signed<T>::value) bits ^= (U(1) << (W - 1));
    } else {
        const U sign = U(1) << (W - 1);
        bits = (bits & sign) ? (bits ^ sign) : ~bits;
    }
    T out;
    std::memcpy(&out, &bits, sizeof(T));
    return out;
}

} // namespace lxy_detail

// ===========================================================================
// Insertion sort (stable)
// ===========================================================================
template<typename T, typename Comp>
static inline void lxyInsertionSort(std::vector<T>& a, Comp comp) {
    int n = (int)a.size();
    for (int i = 1; i < n; ++i) {
        T val = a[i];
        int p = i - 1;
        while (p >= 0 && comp(val, a[p])) { a[p + 1] = a[p]; --p; }
        a[p + 1] = val;
    }
}
template<typename T, typename Comp>
static inline void lxyInsertionRange(std::vector<T>& a, int l, int r, Comp comp) {
    for (int i = l + 1; i <= r; ++i) {
        T val = a[i];
        int p = i - 1;
        while (p >= l && comp(val, a[p])) { a[p + 1] = a[p]; --p; }
        a[p + 1] = val;
    }
}

// scratch: reuse thread-local aligned buffer for trivially-copyable T, else a local vector
template<typename T, typename LocalVec>
static inline T* lxyScratch(int n, LocalVec& local) {
    if constexpr (std::is_trivially_copyable<T>::value) {
        auto& W = lxy_detail::ws();
        W.ensureMerge((size_t)n * sizeof(T));
        return reinterpret_cast<T*>(W.merge.data());
    } else {
        local.resize((size_t)n);
        return local.data();
    }
}

// ===========================================================================
// Stable bottom-up merge sort over [l, r] (generic comparable T)
// ===========================================================================
template<typename T, typename Comp>
static inline void lxyMergeSort(std::vector<T>& a, int l, int r, Comp comp) {
    int n = r - l + 1;
    if (n < 2) return;
    // insertion-sort base: sort each block of <= 16 elements first (avoids the
    // huge number of tiny 1/2/4-element merges that pure bottom-up does). This is
    // the same base libstdc++ uses for stable_sort / std::sort.
    for (int i = l; i <= r; i += 16) {
        int e = (i + 15 < r) ? i + 15 : r;
        for (int j = i + 1; j <= e; ++j) {
            T v = a[j]; int p = j - 1;
            while (p >= i && comp(v, a[p])) { a[p + 1] = a[p]; --p; }
            a[p + 1] = v;
        }
    }
    std::vector<T> localTmp;
    T* tmp = lxyScratch<T>(n, localTmp);
    // Half-in-place merge (libstdc++ __merge_adaptive style): copy only the SMALLER
    // run into the scratch and merge directly back into `a`, so each merge does one
    // copy instead of two (copy-to-tmp then copy-back). This is the ~20% edge that
    // std::stable_sort has over a plain full-buffer merge.
    constexpr bool trivial = std::is_trivially_copyable<T>::value;
    auto copyRun = [&](const T* src, T* dst, int len) {
        if constexpr (trivial) std::memmove(dst, src, (size_t)len * sizeof(T));
        else for (int i = 0; i < len; ++i) dst[i] = std::move(src[i]);
    };
    auto mergeForward = [&](int L, int M, int R, int len1) {  // left run in tmp[0..len1)
        int p = 0, q = M, o = L;
        while (p < len1 && q < R) {
            if (comp(a[q], tmp[p])) a[o++] = std::move(a[q++]);
            else                    a[o++] = std::move(tmp[p++]);
        }
        if (p < len1) { if constexpr (trivial) std::memmove(&a[o], tmp + p, (size_t)(len1 - p) * sizeof(T)); else while (p < len1) a[o++] = std::move(tmp[p++]); }
    };
    auto mergeBackward = [&](int L, int M, int R, int len2) {  // right run in tmp[0..len2)
        int p = len2 - 1, q = M - 1, o = R - 1;
        while (p >= 0 && q >= L) {
            if (comp(tmp[p], a[q])) a[o--] = std::move(a[q--]);
            else                    a[o--] = std::move(tmp[p--]);
        }
        if (p >= 0) { if constexpr (trivial) std::memmove(&a[L], tmp, (size_t)(p + 1) * sizeof(T)); else while (p >= 0) a[o--] = std::move(tmp[p--]); }
    };
    for (int width = 16; width < n; width *= 2) {
        int end = l + n;
        for (int i = l; i < end; i += 2 * width) {
            int L = i;
            int M = (i + width < end) ? i + width : end;
            int R = (i + 2 * width < end) ? i + 2 * width : end;
            int len1 = M - L, len2 = R - M;
            if (len1 <= len2) { copyRun(&a[L], tmp, len1); mergeForward(L, M, R, len1); }
            else              { copyRun(&a[M], tmp, len2); mergeBackward(L, M, R, len2); }
        }
    }
}

// ===========================================================================
// Natural merge sort (stable) - merges the runs described by descPos[]
// ===========================================================================
template<typename T, typename Comp>
static inline void lxyNaturalMerge(std::vector<T>& a, const int* descPos, int descCount, Comp comp) {
    int n = (int)a.size();
    if (n < 2) return;
    std::vector<int> runs;
    runs.reserve(descCount + 2);
    runs.push_back(0);
    for (int i = 0; i < descCount; ++i) runs.push_back(descPos[i]);
    runs.push_back(n);
    int k = (int)runs.size() - 1;
    if (k <= 1) return;

    std::vector<T> localTmp;
    T* tmp = lxyScratch<T>(n, localTmp);
    while (k > 1) {
        std::vector<int> nr;
        nr.reserve(k / 2 + 2);
        for (int i = 0; i < k; i += 2) {
            int l = runs[i];
            if (i + 1 >= k) { nr.push_back(l); continue; }
            int m = runs[i + 1];
            int r = (i + 2 < k) ? runs[i + 2] : n;
            int p = l, q = m, o = l;
            while (p < m && q < r) {
                if (comp(a[q], a[p])) tmp[o++] = std::move(a[q++]); // stable
                else                  tmp[o++] = std::move(a[p++]);
            }
            while (p < m) tmp[o++] = std::move(a[p++]);
            while (q < r) tmp[o++] = std::move(a[q++]);
            for (int s = l; s < r; ++s) a[s] = std::move(tmp[s]);
            nr.push_back(l);
        }
        nr.push_back(n);
        runs.swap(nr);
        k = (int)runs.size() - 1;
    }
}

// ===========================================================================
// Introsort (unstable) - quicksort + heap fallback + insertion base
// ===========================================================================
template<typename T, typename Comp>
static inline void lxyHeapify(std::vector<T>& a, int start, int end, int i, Comp comp) {
    for (;;) {
        int largest = i;
        int l = 2 * i - start + 1, r = 2 * i - start + 2;
        if (l < end && comp(a[largest], a[l])) largest = l;
        if (r < end && comp(a[largest], a[r])) largest = r;
        if (largest == i) break;
        std::swap(a[i], a[largest]);
        i = largest;
    }
}
template<typename T, typename Comp>
static inline void lxyIntroRec(std::vector<T>& a, int l, int r, int depth, Comp comp) {
    if (l >= r) return;
    if (r - l < 16) { lxyInsertionRange(a, l, r, comp); return; }
    if (depth == 0) {
        int n = r - l + 1;
        for (int i = l + n / 2 - 1; i >= l; --i) lxyHeapify(a, l, r + 1, i, comp);
        for (int i = r; i > l; --i) { std::swap(a[l], a[i]); lxyHeapify(a, l, i, l, comp); }
        return;
    }
    int mid = l + (r - l) / 2;
    if (comp(a[mid], a[l])) std::swap(a[l], a[mid]);
    if (comp(a[r], a[l])) std::swap(a[l], a[r]);
    if (comp(a[r], a[mid])) std::swap(a[mid], a[r]);
    std::swap(a[mid], a[r]);
    T piv = a[r];
    int i = l, j = r - 1;
    while (i <= j) {
        while (comp(a[i], piv)) ++i;
        while (j >= l && comp(piv, a[j])) --j;
        if (i <= j) { std::swap(a[i], a[j]); ++i; --j; }
    }
    std::swap(a[i], a[r]);
    lxyIntroRec(a, l, i - 1, depth - 1, comp);
    lxyIntroRec(a, i + 1, r, depth - 1, comp);
}
template<typename T, typename Comp>
static inline void lxyIntroQuickSort(std::vector<T>& a, Comp comp) {
    int n = (int)a.size();
    if (n < 2) return;
    int depth = 0;
    for (int t = n; t > 1; t >>= 1) ++depth;
    depth *= 2;
    lxyIntroRec(a, 0, n - 1, depth, comp);
}

// Comparison that force-inlines the default std::less/std::greater to a raw
// operator. A recursive quicksort template does not always fully inline a
// std::less value argument (measured ~30% slower at N=256); if constexpr makes
// the default path emit a single setl, matching a hand-written sort.
template<typename T, typename Comp>
static inline __attribute__((always_inline)) bool lxyLess(const T& x, const T& y, Comp comp) {
    if constexpr (std::is_same<Comp, std::less<T>>::value) return x < y;
    else if constexpr (std::is_same<Comp, std::greater<T>>::value) return x > y;
    else return comp(x, y);
}

template<typename T, typename Comp>
static inline void lxyQuickRec(std::vector<T>& a, int l, int r, Comp comp) {
    // std::sort-style introsort_loop: Hoare unguarded partition (median-of-3
    // pivot) down to size<=16, recursing right and looping left. Sub-segments are
    // NOT sorted here; the caller runs a single final insertion sort over the whole
    // range (partition makes it near-ordered, so that is ~O(n)). This exact shape
    // measures faster than libstdc++ std::sort at N=100. Safe for n<=128 (bounded
    // worst case; sorted/reversed/all-same excluded by the dispatcher).
    if (l >= r) return;
    int depth = 0;
    for (int t = r - l + 1; t > 1; t >>= 1) ++depth;
    depth *= 2;
    int lo = l, hi = r;
    while (hi - lo + 1 > 16) {
        if (depth == 0) {                           // introsort heap fallback (O(n log n) worst case)
            int nl = hi - lo + 1;
            for (int i = lo + nl / 2 - 1; i >= lo; --i) lxyHeapify(a, lo, hi + 1, i, comp);
            for (int i = hi; i > lo; --i) { std::swap(a[lo], a[i]); lxyHeapify(a, lo, i, lo, comp); }
            return;
        }
        --depth;
        int m = lo + (hi - lo) / 2;
        if (lxyLess(a[m], a[lo], comp)) std::swap(a[m], a[lo]);
        if (lxyLess(a[hi], a[lo], comp)) std::swap(a[hi], a[lo]);
        if (lxyLess(a[hi], a[m], comp)) std::swap(a[m], a[hi]);
        std::swap(a[m], a[hi - 1]);              // pivot to hi-1
        T pivot = a[hi - 1];
        int first = lo, last = hi - 1;
        while (true) {
            while (lxyLess(a[first], pivot, comp)) ++first;
            --last;
            while (lxyLess(pivot, a[last], comp)) --last;
            if (!(first < last)) break;
            std::swap(a[first], a[last]);
            ++first;
        }
        lxyQuickRec(a, first, hi, comp);         // recurse right part
        hi = first - 1;                          // continue left part
    }
}
template<typename T, typename Comp>
static inline void lxyQuickSortTiny(std::vector<T>& a, Comp comp) {
    if (a.size() < 2) return;
    lxyQuickRec(a, 0, (int)a.size() - 1, comp);
    // single final insertion sort over the whole range: partition guarantees the
    // array is near-ordered, so the first 16 elements (guarded) become the minimum
    // prefix and the rest sorts unguarded (no lower-bound check) -- the fast path
    // that libstdc++ uses.
    int n = (int)a.size();
    if (n > 16) {
        for (int i = 1; i < 16; ++i) {
            T v = a[i]; int p = i - 1;
            while (p >= 0 && lxyLess(v, a[p], comp)) { a[p + 1] = a[p]; --p; }
            a[p + 1] = v;
        }
        for (int i = 16; i < n; ++i) {
            T v = a[i]; int p = i - 1;
            while (lxyLess(v, a[p], comp)) { a[p + 1] = a[p]; --p; }
            a[p + 1] = v;
        }
    } else {
        for (int i = 1; i < n; ++i) {
            T v = a[i]; int p = i - 1;
            while (p >= 0 && lxyLess(v, a[p], comp)) { a[p + 1] = a[p]; --p; }
            a[p + 1] = v;
        }
    }
}

// ===========================================================================
// Stable counting sort for integral T with given [minv, maxv]
// ===========================================================================
template<typename T>
static inline void lxyCountingSort(std::vector<T>& a, T minv, T maxv) {
    int n = (int)a.size();
    using Diff = typename std::make_unsigned<T>::type;
    size_t range = (size_t)((Diff)maxv - (Diff)minv) + 1;
    if (range == 0) range = 1;
    auto& W = lxy_detail::ws();
    W.ensureCnt(range);
    W.ensureMerge((size_t)n * sizeof(T));
    int* cnt = W.cnt.data();
    T* tmp = reinterpret_cast<T*>(W.merge.data());
    std::memset(cnt, 0, range * sizeof(int));
    for (int i = 0; i < n; ++i) cnt[(size_t)((Diff)a[i] - (Diff)minv)]++;
    size_t sum = 0;
    for (size_t i = 0; i < range; ++i) { size_t c = (size_t)cnt[i]; cnt[i] = (int)sum; sum += c; }
    for (int i = 0; i < n; ++i) tmp[cnt[(size_t)((Diff)a[i] - (Diff)minv)]++] = a[i]; // stable
    for (int i = 0; i < n; ++i) a[i] = tmp[i];
}

// ===========================================================================
// Stable LSD radix sort (base 256) for any arithmetic T (incl. floats)
// ===========================================================================
// Stable LSD radix (base 256). Pass 0 reads directly from `a` (skips a separate
// key-build pass) and the final pass writes directly back into `a` (skips the
// final copy-back), so a 4-byte sort moves 8n bytes instead of 12n.
template<typename T>
static inline void lxyRadixSort(std::vector<T>& a) {
    int n = (int)a.size();
    if (n < 2) return;
    using K = lxy_detail::key_type<T>;             // uint32 for <=4-byte T, else uint64
    auto& W = lxy_detail::ws();
    if constexpr (sizeof(T) <= 4) W.ensureKeys32((size_t)n);
    else                         W.ensureKeys64((size_t)n);
    W.ensureCnt(256);
    K* kA, * kB;
    if constexpr (sizeof(T) <= 4) { kA = W.k32a.data(); kB = W.k32b.data(); }
    else                          { kA = W.k64a.data(); kB = W.k64b.data(); }
    int* cnt = W.cnt.data();
    const int bytes = (int)sizeof(T);
    T* adata = a.data();

    // Pass 0: count and distribute byte 0, reading toKey directly from `a`.
    std::memset(cnt, 0, 256 * sizeof(int));
    for (int i = 0; i < n; ++i) ++cnt[lxy_detail::toKey<T>(adata[i]) & 0xFFu];
    int s = 0;
    for (int i = 0; i < 256; ++i) { int c = cnt[i]; cnt[i] = s; s += c; }
    for (int i = 0; i < n; ++i) { K v = lxy_detail::toKey<T>(adata[i]); kB[cnt[v & 0xFFu]++] = v; }
    K* src = kB;

    if (bytes > 1) {
        // Middle passes bounce between the two key buffers.
        for (int b = 1; b < bytes - 1; ++b) {
            int shift = b * 8;
            std::memset(cnt, 0, 256 * sizeof(int));
            for (int i = 0; i < n; ++i) ++cnt[(src[i] >> shift) & 0xFFu];
            int s2 = 0;
            for (int i = 0; i < 256; ++i) { int c = cnt[i]; cnt[i] = s2; s2 += c; }
            K* dst = (src == kA) ? kB : kA;
            for (int i = 0; i < n; ++i) { K v = src[i]; dst[cnt[(v >> shift) & 0xFFu]++] = v; }
            src = dst;
        }
        // Final pass: write sorted values directly back into `a`.
        int shift = (bytes - 1) * 8;
        std::memset(cnt, 0, 256 * sizeof(int));
        for (int i = 0; i < n; ++i) ++cnt[(src[i] >> shift) & 0xFFu];
        int s3 = 0;
        for (int i = 0; i < 256; ++i) { int c = cnt[i]; cnt[i] = s3; s3 += c; }
        for (int i = 0; i < n; ++i) { K v = src[i]; adata[cnt[(v >> shift) & 0xFFu]++] = lxy_detail::fromKey<T>(v); }
    } else {
        // 1-byte key type: distribute to buffer once, then copy back.
        for (int i = 0; i < n; ++i) { K v = lxy_detail::toKey<T>(adata[i]); kB[cnt[v & 0xFFu]++] = v; }
        for (int i = 0; i < n; ++i) adata[i] = lxy_detail::fromKey<T>(kB[i]);
    }
}

// ===========================================================================
// MSD radix sort for std::string (stable, O(n * avg length))
//   bucket 0  = string ended here (shorter prefix sorts first)
//   bucket 1..256 = character value at current depth
// Uses an explicit work stack (no C++ recursion -> safe for long common prefixes).
// ===========================================================================
template<typename T, typename Comp>
static inline void lxyStringRadix(std::vector<T>& a, Comp comp) {
    const int ALPHA = 256 + 1;
    int n = (int)a.size();
    if (n < 2) return;

    std::vector<T> localTmp;
    T* buf = lxyScratch<T>(n, localTmp);
    std::vector<int> cnt(ALPHA), pos(ALPHA);
    std::vector<int> stL, stR, stD;
    stL.push_back(0); stR.push_back(n); stD.push_back(0);

    while (!stL.empty()) {
        int l = stL.back(), r = stR.back(), depth = stD.back();
        stL.pop_back(); stR.pop_back(); stD.pop_back();
        if (r - l <= 16) { lxyInsertionRange(a, l, r - 1, comp); continue; }

        std::fill(cnt.begin(), cnt.end(), 0);
        // count and record the actual [vMin,vMax] char range so we only iterate
        // the buckets that occur (e.g. 26 letters, not 256)
        int vMin = ALPHA, vMax = -1;
        for (int i = l; i < r; ++i) {
            int v = (depth < (int)a[i].size()) ? ((unsigned char)a[i][depth] + 1) : 0;
            ++cnt[v];
            if (v < vMin) vMin = v;
            if (v > vMax) vMax = v;
        }
        int sum = 0;
        for (int v = vMin; v <= vMax; ++v) { pos[v] = sum; sum += cnt[v]; }
        for (int i = l; i < r; ++i) {
            int v = (depth < (int)a[i].size()) ? ((unsigned char)a[i][depth] + 1) : 0;
            buf[l + pos[v]++] = std::move(a[i]);   // stable
        }
        for (int i = l; i < r; ++i) a[i] = std::move(buf[i]);

        // push non-empty char groups (v >= 1) to recurse at depth+1; v==0 (ended) is done
        int lo = vMin > 1 ? vMin : 1;
        for (int v = lo; v <= vMax; ++v) {
            int gl = l + pos[v] - cnt[v];
            int gr = l + pos[v];
            if (gr - gl > 1) { stL.push_back(gl); stR.push_back(gr); stD.push_back(depth + 1); }
        }
    }
}

// ===========================================================================
// Byte-level MSD radix for std::vector<std::vector<Arith>> (nested vectors).
// Treats each element as a variable-length sequence of Arith; sorts stably by
// lexicographic order like std::string's MSD radix, but bucket by each Arith's
// key bytes (high -> low). O(n * avg_len * sizeof(Arith)).
// ===========================================================================
template<typename Arith>
static inline void lxyVectorRadix(std::vector<std::vector<Arith>>& a) {
    const int W = (int)sizeof(Arith);
    int n = (int)a.size();
    if (n < 2) return;
    // scan for the highest key byte that actually varies -> skip constant high bytes
    using K = lxy_detail::key_type<Arith>;
    K gMax = 0, gMin = ~K(0);
    for (auto& inner : a) for (auto x : inner) { K k = lxy_detail::toKey<Arith>(x); if (k > gMax) gMax = k; if (k < gMin) gMin = k; }
    int byteStart = W - 1;
    if (gMax != gMin) while (byteStart > 0 && (((gMin >> (byteStart * 8)) & 0xFFu) == ((gMax >> (byteStart * 8)) & 0xFFu))) --byteStart;
    std::vector<std::vector<Arith>> buf((size_t)n);
    int cnt[257], pos[257];
    struct Item { int l, r, depth, byte; };
    std::vector<Item> st;
    st.push_back({0, n, 0, byteStart});
    while (!st.empty()) {
        Item it = st.back(); st.pop_back();
        int l = it.l, r = it.r, depth = it.depth, byte = it.byte;
        if (r - l <= 16) {   // insertion sort (lexicographic) for small groups
            for (int i = l + 1; i < r; ++i) { auto v = std::move(a[i]); int p = i - 1;
                while (p >= l && v < a[p]) { a[p + 1] = std::move(a[p]); --p; } a[p + 1] = std::move(v); }
            continue;
        }
        std::fill(cnt, cnt + 257, 0);
        int vMin = 257, vMax = -1;
        for (int i = l; i < r; ++i) {
            int v = 0;                       // 0 = sequence ended here (shorter)
            if (depth < (int)a[i].size()) {
                auto k = lxy_detail::toKey<Arith>(a[i][depth]);
                v = (int)((k >> (byte * 8)) & 0xFFu) + 1;
            }
            ++cnt[v]; if (v < vMin) vMin = v; if (v > vMax) vMax = v;
        }
        int sum = 0;
        for (int v = vMin; v <= vMax; ++v) { pos[v] = sum; sum += cnt[v]; }
        for (int i = l; i < r; ++i) {
            int v = 0;
            if (depth < (int)a[i].size()) {
                auto k = lxy_detail::toKey<Arith>(a[i][depth]);
                v = (int)((k >> (byte * 8)) & 0xFFu) + 1;
            }
            buf[l + pos[v]++] = std::move(a[i]);   // stable
        }
        for (int i = l; i < r; ++i) a[i] = std::move(buf[i]);
        int lo = vMin > 1 ? vMin : 1;
        for (int v = lo; v <= vMax; ++v) {
            int gl = l + pos[v] - cnt[v], gr = l + pos[v];
            if (gr - gl > 1) {
                int nd = depth, nb = byte;
                if (byte > 0) nb = byte - 1; else { nd = depth + 1; nb = byteStart; }
                st.push_back({gl, gr, nd, nb});
            }
        }
    }
}

// ===========================================================================
// Core dispatcher
// ===========================================================================
// forward decl (defined after the dispatcher)
template<typename T, typename Comp>
static inline void lxySortParallelImpl(std::vector<T>& a, Comp comp, bool stable);

template<typename T, typename Comp>
static inline void lxySortImpl(std::vector<T>& a, Comp comp, bool stable, const char** chosen) {
    if (chosen) *chosen = nullptr;
    int n = (int)a.size();
    if (n <= 1) { if (chosen) *chosen = "O(1)"; return; }

    // linear paths require the default numeric ordering (std::less<T> on arithmetic T)
    constexpr bool intPath  = std::is_integral<T>::value && !std::is_same<T, bool>::value
        && std::is_same<Comp, std::less<T>>::value;   // counting sort (needs make_unsigned)
    constexpr bool radixPath = lxy_detail::radix_able<T>
        && std::is_same<Comp, std::less<T>>::value;   // radix sort (any arithmetic)

    // ---- small-array fast path (up to the radix threshold) ----
    if (n <= 700) {
        // n <= 16: insertion sort directly, no detection scan (std::sort also uses
        // insertion here, so sorted/all-same are O(n) naturally and random ties std;
        // avoids the detection overhead that hurt tiny random arrays).
        if (n <= 16) { if (chosen) *chosen = "Insertion"; lxyInsertionSort(a, comp); return; }
        // 17..700: detect sorted/reversed, else quicksort / merge
        bool asc = true, desc = true;
        for (int i = 1; i < n; ++i) {
            if (comp(a[i], a[i - 1])) asc = false;
            if (comp(a[i - 1], a[i])) desc = false;
            if (!asc && !desc) break;
        }
        if (asc) { if (chosen) *chosen = "O(n) already-sorted"; return; }
        if (desc) { if (chosen) *chosen = "O(n) reverse"; std::reverse(a.begin(), a.end()); return; }
        if (stable) { if (chosen) *chosen = "MergeSort"; lxyMergeSort(a, 0, n - 1, comp); }
        else        { if (chosen) *chosen = "Introsort"; lxyQuickSortTiny(a, comp); }
        return;
    }

    // ---- phase A: cheap early-exit sorted/reversed/all-same detection ----
    // (random data breaks out after ~2 elements -> ~O(1) for the common case)
    {
        bool asc = true, desc = true;
        for (int i = 1; i < n; ++i) {
            if (comp(a[i], a[i - 1])) asc = false;
            if (comp(a[i - 1], a[i])) desc = false;
            if (!asc && !desc) break;
        }
        if (asc) { if (chosen) *chosen = "O(n) already-sorted"; return; }
        if (desc) { if (chosen) *chosen = "O(n) reverse"; std::reverse(a.begin(), a.end()); return; }
    }

    // ---- phase B: scan for max/min + descent count (only for non-monotonic) ----
    // For arithmetic wide-range random data this can stop early: once the range
    // already exceeds n (counting ruled out) AND descents exceed the near-sorted
    // threshold (natural-merge ruled out), the result is radix and we need nothing
    // more from the scan (radix recomputes everything internally).
    T maxv = a[0], minv = a[0];
    int descCount = 0;
    int descPos[32];
    const int descLimit = (n / 16) > 16 ? (n / 16) : 16;
    for (int i = 1; i < n; ++i) {
        if (comp(a[i], a[i - 1])) { if (descCount < 32) descPos[descCount] = i; ++descCount; }
        if (comp(maxv, a[i])) maxv = a[i];
        if (comp(a[i], minv)) minv = a[i];
        if constexpr (radixPath && sizeof(T) <= 4) {   // int/float: range fits in long long
            if ((long long)maxv - (long long)minv > (long long)n && descCount > descLimit) break;
        } else if constexpr (!intPath) {
            // comparison-only path: counting/radix are unavailable, so maxv/minv are
            // useless once natural-merge is ruled out (natural uses descCount<=32).
            // Early-stop saves a full O(n) scan that std::stable_sort never pays.
            if (descCount > 32) break;
        }
    }

    // ---- linear path: counting (integral, small range) ----
    if constexpr (intPath) {
        using Diff = typename std::make_unsigned<T>::type;
        unsigned long long dataRange = (unsigned long long)((Diff)maxv - (Diff)minv);
        if (dataRange <= (unsigned long long)n) {
            // bitmap sort: O(n+range) time, only O(range/8) memory (vs range*4 for
            // counting). Works only when all values are distinct; else fall back.
            if (dataRange <= (1ull << 20)) {                       // bitmap <= 128KB
                size_t vcount = (size_t)dataRange + 1;             // values are minv..maxv (inclusive)
                size_t nb = (vcount + 7) / 8;
                auto& Wb = lxy_detail::ws();                       // reuse thread-local scratch (no alloc)
                Wb.ensureMerge(nb);
                unsigned char* bm = reinterpret_cast<unsigned char*>(Wb.merge.data());
                std::memset(bm, 0, nb);
                bool dup = false;
                for (int i = 0; i < n; ++i) {
                    size_t b = (size_t)((Diff)a[i] - (Diff)minv);
                    if (bm[b >> 3] & (1u << (b & 7))) { dup = true; break; }
                    bm[b >> 3] |= (unsigned char)(1u << (b & 7));
                }
                if (!dup) {
                    if (chosen) *chosen = "Bitmap";
                    int p = 0;
                    for (size_t i = 0; i < vcount; ++i)
                        if (bm[i >> 3] & (1u << (i & 7))) a[p++] = (int)((long long)i + (long long)minv);
                    return;
                }
            }
            if (chosen) *chosen = "Counting";
            lxyCountingSort(a, minv, maxv);
            return;
        }
    }

    // ---- nearly sorted (few descents) -> natural merge (before radix!) ----
    if (descCount <= descLimit && descCount <= 32) {
        if (chosen) *chosen = "NaturalMerge";
        lxyNaturalMerge(a, descPos, descCount, comp);
        return;
    }

    // ---- radix (arithmetic, wide range, large) ----
    if constexpr (radixPath) {
        if (n >= 512) {
            // 2-value fast path (integral, full-range): if the whole array holds
            // only minv and maxv (e.g. only INT_MIN/INT_MAX), fill directly in O(n)
            // instead of radix's 4 passes. O(1) extra scan, safe, no scratch.
            if constexpr (intPath) {
                if (maxv != minv) {
                    long long cntmn = 0; bool only2 = true;
                    for (int i = 0; i < n; ++i)
                        if (a[i] == minv) ++cntmn;
                        else if (!(a[i] == maxv)) { only2 = false; break; }
                    if (only2) {
                        int p = 0;
                        while (p < cntmn) a[p++] = minv;
                        while (p < n) a[p++] = maxv;
                        if (chosen) *chosen = "TwoValue";
                        return;
                    }
                }
            }
            if (chosen) *chosen = "Radix";
            lxyRadixSort(a);
            return;
        }
    }
    // strings -> MSD radix (stable, O(n*L); beats std::sort on random strings)
    if constexpr (lxy_detail::is_string_like<T>::value && std::is_same<Comp, std::less<T>>::value) {
        if (chosen) *chosen = "StringRadix";
        lxyStringRadix(a, comp);
        return;
    }
    // nested std::vector<Arith> -> byte-level MSD radix (O(n * avg_len * sizeof(Arith)))
    if constexpr (lxy_detail::is_vector_arith<T>::value && std::is_same<Comp, std::less<T>>::value) {
        if (chosen) *chosen = "VectorRadix";
        lxyVectorRadix(a);
        return;
    }

    // ---- comparison path (merge / introsort) ----
    // auto-parallel for large comparison-heavy generic arrays (helps only when
    // comparison/CPU-bound, which this path is; guarded against nested parallel)
    if (n >= lxy_detail::AUTO_PAR_THRESHOLD && lxy_detail::parDepth() == 0) {
        ++lxy_detail::parDepth();
        lxySortParallelImpl(a, comp, stable);
        --lxy_detail::parDepth();
        if (chosen) *chosen = "Parallel(merge)";
        return;
    }
    if (stable) { if (chosen) *chosen = "MergeSort"; lxyMergeSort(a, 0, n - 1, comp); }
    else        { if (chosen) *chosen = "Introsort"; lxyQuickSortTiny(a, comp); }
}

// ===========================================================================
// Public entry points
// ===========================================================================
// DEFAULT sort. For arithmetic types and strings it is stable (radix/counting/
// MSD-radix/natural-merge). For tiny arrays and generic types it uses introsort
// (like std::sort) for max speed, so it is NOT strictly stable there.
// Large generic/comparison arrays are auto-parallelized.
// -> use lxyStableSort for a strict stability guarantee.
template<typename T, typename Comp = std::less<T>,
    typename = std::enable_if_t<!lxy_detail::is_keyfunc<T, Comp>::value>>
static inline void lxySort(std::vector<T>& a, Comp comp = Comp{}) {
    lxySortImpl(a, comp, false, nullptr);
}
// strictly stable sort (merge for the comparison path)
template<typename T, typename Comp = std::less<T>>
static inline void lxyStableSort(std::vector<T>& a, Comp comp = Comp{}) {
    lxySortImpl(a, comp, true, nullptr);
}
// max-speed alias (introsort for the comparison path)
template<typename T, typename Comp = std::less<T>>
static inline void lxySortUnstable(std::vector<T>& a, Comp comp = Comp{}) {
    lxySortImpl(a, comp, false, nullptr);
}
// sort that also reports which algorithm was chosen (benchmark/debug)
template<typename T, typename Comp = std::less<T>>
static inline void lxySortTrace(std::vector<T>& a, const char** chosen, Comp comp = Comp{}) {
    lxySortImpl(a, comp, false, chosen);
}

// stable sort of perm[] (original indices) by arithmetic keys[] (counting or radix)
template<typename K>
static inline void lxySortByKeyArith(std::vector<K>& keys, std::vector<int>& perm, int n) {
    using Diff = typename std::make_unsigned<K>::type;
    if constexpr (std::is_integral<K>::value && !std::is_same<K, bool>::value) {
        K mn = keys[0], mx = keys[0];
        for (int i = 1; i < n; ++i) { if (keys[i] < mn) mn = keys[i]; if (mx < keys[i]) mx = keys[i]; }
        unsigned long long range = (unsigned long long)((Diff)mx - (Diff)mn) + 1;
        if (range <= (unsigned long long)n) {
            auto& W = lxy_detail::ws();
            W.ensureCnt(range);
            int* cnt = W.cnt.data();
            std::memset(cnt, 0, range * sizeof(int));
            for (int i = 0; i < n; ++i) cnt[(size_t)((Diff)keys[perm[i]] - (Diff)mn)]++;  // key of element perm[i]
            size_t sum = 0;
            for (size_t i = 0; i < range; ++i) { size_t c = (size_t)cnt[i]; cnt[i] = (int)sum; sum += c; }
            std::vector<int> np((size_t)n);
            for (int i = 0; i < n; ++i) np[cnt[(size_t)((Diff)keys[perm[i]] - (Diff)mn)]++] = perm[i]; // stable
            perm.swap(np);
            return;
        }
    }
    // LSD radix on keys, carrying the permutation
    using LK = lxy_detail::key_type<K>;             // uint32 for <=4-byte keys
    auto& W = lxy_detail::ws();
    if constexpr (sizeof(K) <= 4) W.ensureKeys32((size_t)n);
    else                         W.ensureKeys64((size_t)n);
    W.ensureCnt(256);
    LK* src, * dst;
    if constexpr (sizeof(K) <= 4) { src = W.k32a.data(); dst = W.k32b.data(); }
    else                          { src = W.k64a.data(); dst = W.k64b.data(); }
    int* cnt = W.cnt.data();
    std::vector<LK> pS((size_t)n), pD((size_t)n);
    LK maxKey = 0;
    for (int i = 0; i < n; ++i) { src[i] = lxy_detail::toKey<K>(keys[perm[i]]); pS[i] = (LK)perm[i]; if (src[i] > maxKey) maxKey = src[i]; }
    int bytes = (int)sizeof(K);
    while (bytes > 1 && (maxKey >> ((bytes - 1) * 8)) == 0) --bytes;
    for (int b = 0; b < bytes; ++b) {
        int shift = b * 8;
        std::memset(cnt, 0, 256 * sizeof(int));
        for (int i = 0; i < n; ++i) cnt[(src[i] >> shift) & 0xFFu]++;
        int s = 0;
        for (int i = 0; i < 256; ++i) { int c = cnt[i]; cnt[i] = s; s += c; }
        for (int i = 0; i < n; ++i) { int c = (int)((src[i] >> shift) & 0xFFu); dst[cnt[c]] = src[i]; pD[cnt[c]] = pS[i]; ++cnt[c]; } // stable
        std::swap(src, dst); std::swap(pS, pD);
    }
    for (int i = 0; i < n; ++i) perm[i] = (int)pS[i];
}

// stable sort of perm by one arithmetic component of a tuple (used by LSD)
template<typename Tuple, size_t I>
static inline void lxySortByTupleComp(std::vector<Tuple>& keys, std::vector<int>& perm, int n) {
    using C = std::tuple_element_t<I, Tuple>;
    std::vector<C> comp((size_t)n);
    for (int i = 0; i < n; ++i) comp[i] = std::get<I>(keys[i]);
    lxySortByKeyArith(comp, perm, n);              // stable linear sort by this component
}
// LSD: sort by last component first, ..., then first (each pass stable -> lexicographic tuple order)
template<typename Tuple, size_t I>
static inline void lxySortByTupleLsd(std::vector<Tuple>& keys, std::vector<int>& perm, int n) {
    lxySortByTupleComp<Tuple, I>(keys, perm, n);
    if constexpr (I > 0) lxySortByTupleLsd<Tuple, I - 1>(keys, perm, n);
}

// ===========================================================================
// lxySortByKey - sort by an extracted key (stable by default)
//   keyFunc may return a scalar arithmetic key (fast linear sort) or a
//   std::tuple of arithmetic keys (fast LSD linear sort) or anything else
//   (stable comparison sort).
// ===========================================================================
template<typename T, typename KeyFunc>
static inline void lxySortByKey(std::vector<T>& a, KeyFunc kf, bool stable = true) {
    using K = typename std::decay<decltype(kf(a[0]))>::type;
    int n = (int)a.size();
    if (n <= 1) return;
    std::vector<K> keys((size_t)n);
    std::vector<int> perm((size_t)n);
    for (int i = 0; i < n; ++i) { keys[i] = kf(a[i]); perm[i] = i; }

    if constexpr (lxy_detail::is_std_tuple<K>::value && lxy_detail::tuple_all_arith<K>) {
        lxySortByTupleLsd<K, std::tuple_size<K>::value - 1>(keys, perm, n); // fast + stable multi-key
    } else if constexpr (lxy_detail::radix_able<K>) {
        lxySortByKeyArith(keys, perm, n);          // fast + stable single arithmetic key
    } else {
        struct KIdx { K key; int idx; bool operator<(const KIdx& o) const { return key < o.key; } };
        std::vector<KIdx> v((size_t)n);
        for (int i = 0; i < n; ++i) v[i] = { keys[i], i };
        if (stable) lxyStableSort(v); else lxySortUnstable(v);
        for (int i = 0; i < n; ++i) perm[i] = v[i].idx;
    }
    std::vector<T> tmp((size_t)n);
    for (int i = 0; i < n; ++i) tmp[i] = std::move(a[perm[i]]);
    a.swap(tmp);
}

// ===========================================================================
// Parallel sorting (OpenMP). Splits into chunks, sorts each in parallel with
// lxySortImpl (radix/counting/...), then a stable bottom-up merge.
// Without OpenMP it falls back to the sequential sort.
// ===========================================================================
// stable 2-way merge of a[l..m) and a[m..r) into tmp, then back to a
template<typename T, typename Comp>
static inline void lxyMerge2(std::vector<T>& a, T* tmp, int l, int m, int r, Comp comp) {
    int p = l, q = m, o = l;
    while (p < m && q < r) {
        if (comp(a[q], a[p])) tmp[o++] = std::move(a[q++]);
        else                  tmp[o++] = std::move(a[p++]);
    }
    while (p < m) tmp[o++] = std::move(a[p++]);
    while (q < r) tmp[o++] = std::move(a[q++]);
    for (int i = l; i < r; ++i) a[i] = std::move(tmp[i]);
}

template<typename T, typename Comp>
static inline void lxySortParallelImpl(std::vector<T>& a, Comp comp, bool stable) {
    int n = (int)a.size();
    if (n <= (1 << 16)) { lxySortImpl(a, comp, stable, nullptr); return; }   // small: sequential
#ifdef _OPENMP
    int P = std::min(omp_get_max_threads(), n / (1 << 14));
    if (P < 2) { lxySortImpl(a, comp, stable, nullptr); return; }
    std::vector<int> starts(P + 1);
    for (int c = 0; c <= P; ++c) starts[c] = (int)((long long)n * c / P);

    #pragma omp parallel for schedule(dynamic)
    for (int c = 0; c < P; ++c) {
        lxy_detail::parDepth() = 1;                           // no nested auto-parallel in workers
        int l = starts[c], r = starts[c + 1];
        std::vector<T> chunk(a.begin() + l, a.begin() + r);   // copy out
        lxySortImpl(chunk, comp, stable, nullptr);            // each thread has its own workspace
        for (int k = 0; k < r - l; ++k) a[l + k] = std::move(chunk[k]);  // copy back
        lxy_detail::parDepth() = 0;
    }

    // stable bottom-up merge of the P sorted runs (merge pairs in parallel at each level)
    std::vector<T> tmp((size_t)n);
    std::vector<int> cur = starts;
    int K = P;
    std::vector<int> jL, jM, jR;
    while (K > 1) {
        std::vector<int> nxt;
        jL.clear(); jM.clear(); jR.clear();
        for (int i = 0; i < K; i += 2) {
            int l = cur[i];
            nxt.push_back(l);
            if (i + 1 < K) {                       // actual merge job
                int m = cur[i + 1], r = (i + 2 < K) ? cur[i + 2] : n;
                jL.push_back(l); jM.push_back(m); jR.push_back(r);
            }                                       // else: carry last odd run
        }
        nxt.push_back(n);
        int J = (int)jL.size();
        #pragma omp parallel for schedule(dynamic)
        for (int j = 0; j < J; ++j)
            lxyMerge2(a, tmp.data(), jL[j], jM[j], jR[j], comp);  // disjoint ranges -> safe
        cur.swap(nxt);
        K = (int)cur.size() - 1;
    }
#else
    lxySortImpl(a, comp, stable, nullptr);
#endif
}

template<typename T, typename Comp = std::less<T>>
static inline void lxySortParallel(std::vector<T>& a, Comp comp = Comp{}) {
    lxySortParallelImpl(a, comp, true);
}
template<typename T, typename Comp = std::less<T>>
static inline void lxySortParallelUnstable(std::vector<T>& a, Comp comp = Comp{}) {
    lxySortParallelImpl(a, comp, false);
}

// ===========================================================================
// Unified entry: iterator-range overloads + auto-byKey for vector
// ===========================================================================
// auto-byKey on std::vector: if the 2nd arg is a key extractor (one arg, non-bool
// return), route to lxySortByKey (stable) instead of treating it as a comparator.
template<typename T, typename F,
    typename = std::enable_if_t<lxy_detail::is_keyfunc<T, F>::value>>
static inline void lxySort(std::vector<T>& a, F&& kf) {
    lxySortByKey(a, std::forward<F>(kf), true);
}

// ---- random-access iterator range: lxySort(first, last) ----
// Copies the range to a temporary vector, sorts in place there, then writes back.
// Works for any random-access iterator (vector/array/deque iterators, raw pointers).
template<typename RandomIt>
static inline void lxySort(RandomIt first, RandomIt last) {
    using T = typename std::iterator_traits<RandomIt>::value_type;
    std::vector<T> v(first, last);
    lxySort(v);
    std::move(v.begin(), v.end(), first);
}
// ---- random-access iterator range with comparator OR key extractor ----
template<typename RandomIt, typename F>
static inline void lxySort(RandomIt first, RandomIt last, F f) {
    using T = typename std::iterator_traits<RandomIt>::value_type;
    std::vector<T> v(first, last);
    if constexpr (lxy_detail::is_keyfunc<T, F>::value) lxySortByKey(v, std::move(f), true);
    else                                              lxySort(v, std::move(f));
    std::move(v.begin(), v.end(), first);
}
// ---- raw pointer + count: lxySort(ptr, n) ----
template<typename T, typename Comp = std::less<T>,
    typename = std::enable_if_t<!lxy_detail::is_keyfunc<T, Comp>::value>>
static inline void lxySort(T* data, size_t n, Comp comp = Comp{}) {
    if (n <= 1) return;
    std::vector<T> v(data, data + n);
    lxySortImpl(v, comp, false, nullptr);
    std::move(v.begin(), v.end(), data);
}
