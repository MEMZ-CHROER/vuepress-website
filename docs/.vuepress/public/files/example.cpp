// example.cpp - minimal usage example (pure ASCII)
// Build: g++ -O2 -std=c++17 -fopenmp example.cpp -o example && ./example
// Just include one header, then call lxySort / lxySortUnstable / lxySortByKey directly.

#include "lxy_sort.hpp"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
using namespace std;

int main() {
    // ---- 1) int: stable sort, default comparator ----
    vector<int> a = {5, 3, 9, 1, 4, 3, 7, 2, 8, 6};
    lxySort(a);                       // stable; auto-picks radix/counting/merge/...
    printf("int    : "); for (int x : a) printf("%d ", x); printf("\n");

    // ---- 2) double: supports floats (incl. negatives / denormals) ----
    vector<double> d = {3.5, -1.2, 0.0, 2.7, -0.0, 1e-10, -5.5};
    lxySort(d);
    printf("double : "); for (double x : d) printf("%g ", x); printf("\n");

    // ---- 3) string: generic comparable type ----
    vector<string> s = {"pear", "apple", "fig", "grape", "apple"};
    lxySort(s);
    printf("string : "); for (auto& x : s) printf("%s ", x.c_str()); printf("\n");

    // ---- 4) custom comparator (descending) ----
    vector<int> b = {4, 1, 7, 3, 9};
    lxySort(b, greater<int>());
    printf("desc   : "); for (int x : b) printf("%d ", x); printf("\n");

    // ---- 5) fastest mode when stability not required ----
    vector<string> ss = {"zebra", "apple", "mango", "kiwi", "banana"};
    lxySortUnstable(ss);              // introsort, ~2x faster than std::sort on random strings
    printf("fast   : "); for (auto& x : ss) printf("%s ", x.c_str()); printf("\n");

    // ---- 6) sort a struct by an extracted key (stable; arithmetic key -> linear sort) ----
    struct Rec { int id; string name; };
    vector<Rec> r = {{2,"b"}, {1,"a"}, {2,"d"}, {3,"c"}, {1,"e"}};
    lxySortByKey(r, [](const Rec& x){ return x.id; });   // stable sort by id
    printf("bykey  : "); for (auto& x : r) printf("(%d,%s) ", x.id, x.name.c_str()); printf("\n");

    // ---- 7) multi-key sort: stable sort by (id, name) ----
    struct Rec2 { int id; string name; };
    vector<Rec2> r2 = {{2,"x"}, {1,"b"}, {2,"a"}, {1,"a"}, {2,"m"}};
    lxySortByKey(r2, [](const Rec2& x){ return make_tuple(x.id, x.name); });  // id, then name
    printf("multi  : "); for (auto& x : r2) printf("(%d,%s) ", x.id, x.name.c_str()); printf("\n");

    // ---- 8) parallel sort (comparison-heavy / large; needs -fopenmp) ----
#ifdef _OPENMP
    vector<Rec2> big(500000);
    for (auto& x : big) { x.id = (int)(std::rand()%100); x.name = "n"+std::to_string(std::rand()%1000); }
    lxySortParallel(big, [](const Rec2&x,const Rec2&y){return x.id<y.id;});
    printf("parallel: %zu elems sorted, first=%d\n", big.size(), big[0].id);
#else
    printf("parallel: (compile with -fopenmp to enable)\n");
#endif

    // ---- self-check ----
    bool ok = is_sorted(a.begin(), a.end())
           && is_sorted(d.begin(), d.end())
           && is_sorted(s.begin(), s.end())
           && is_sorted(b.begin(), b.end(), greater<int>());
    printf("\nverification: %s\n", ok ? "ALL PASS" : "FAILED");
    return ok ? 0 : 1;
}
