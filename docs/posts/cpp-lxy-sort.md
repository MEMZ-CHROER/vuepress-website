---
date: 2026-08-27
category:
  - C++学习
tag:
  - C++
  - 排序
  - 算法
---

# 我写了一个"智能"排序函数：lxySort

前面讲了 STL 的 `sort`，但那毕竟是别人写好的。今天分享一个**我自己写的排序函数**——它会根据数组的实际情况，自动选择最合适的排序算法。我叫它 **lxySort**。

## 一、思路：没有最好的排序，只有最合适的

你可能听过各种排序：快排、归并、计数、基数、冒泡……每个都有自己的适用场景。那么问题来了：**能不能写一个函数，自己判断"该用哪个"？**

这就是 lxySort 干的事。它像个小管家，先看看这堆数据是什么情况，再决定派哪个"打手"上场。

## 二、完整代码

```cpp
#include <algorithm>
#include <vector>
#include <iostream>
#include <cmath>
#include <ctime>
using namespace std;

// 归并核心
void merge(vector<int>& a, vector<int>& temp, int l, int mid, int r) {
    int i = l, j = mid + 1, k = l;
    while (i <= mid && j <= r)
        temp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    while (i <= mid) temp[k++] = a[i++];
    while (j <= r) temp[k++] = a[j++];
    for (int p = l; p <= r; ++p) a[p] = temp[p];
}
void mergeSortRec(vector<int>& a, vector<int>& temp, int l, int r) {
    if (l >= r) return;
    int mid = l + (r - l) / 2;
    mergeSortRec(a, temp, l, mid);
    mergeSortRec(a, temp, mid + 1, r);
    merge(a, temp, l, mid, r);
}
// 对 [l, r] 区间归并排序——也作为快排深度超限时的兜底
void mergeSortRange(vector<int>& a, int l, int r) {
    if (l >= r) return;
    vector<int> temp(a.size());
    mergeSortRec(a, temp, l, r);
}
void mergeSort(vector<int>& a) {
    if (a.empty()) return;
    mergeSortRange(a, 0, (int)a.size() - 1);
}

// 检查区间 [l, r] 是否无序，顺手带回 max/min
bool isUnorderedEx(const vector<int>& a, int l, int r, int& outMax, int& outMin) {
    if (l > r) { outMax = 0; outMin = 0; return false; }
    outMax = outMin = a[l];
    bool unordered = false;
    for (int i = l + 1; i <= r; ++i) {
        if (a[i] < a[i - 1]) unordered = true;
        if (a[i] > outMax) outMax = a[i];
        if (a[i] < outMin) outMin = a[i];
    }
    return unordered;
}

// 鸡尾酒排序（双向冒泡），区间版
void jwjSort(vector<int>& a, int l, int r) {
    while (l < r) {
        bool ok = true;
        for (int i = l; i < r; i++)
            if (a[i] > a[i + 1]) { swap(a[i], a[i + 1]); ok = false; }
        r--;
        for (int i = r; i > l; i--)
            if (a[i] < a[i - 1]) { swap(a[i], a[i - 1]); ok = false; }
        l++;
        if (ok) break;
    }
}

// 快排（带深度超限兜底），区间版
void quickSortInner(vector<int>& a, int l, int r, int depth, int limit) {
    if (depth > limit) {
        // 深度超限说明分得不均衡（快退化到 O(n^2)），改用归并兜底
        mergeSortRange(a, l, r);
        return;
    }
    while (l < r) {
        swap(a[r], a[l + rand() % (r - l + 1)]);  // 随机选 pivot 放右端
        int pivot = a[r];
        int store = l;
        for (int i = l; i < r; ++i)
            if (a[i] <= pivot) swap(a[store++], a[i]);
        swap(a[store], a[r]);
        int pos = store;
        // 小区间递归，大区间交给 while 循环
        if (pos - l < r - pos) { quickSortInner(a, l, pos - 1, depth + 1, limit); l = pos + 1; }
        else                   { quickSortInner(a, pos + 1, r, depth + 1, limit); r = pos - 1; }
    }
}

// 计数排序，区间版
void countingSort(vector<int>& a, int l, int r) {
    if (l >= r) return;
    int mx = a[l], mn = a[l];
    for (int i = l + 1; i <= r; ++i) { mx = max(mx, a[i]); mn = min(mn, a[i]); }
    vector<int> cnt(mx - mn + 1, 0);
    for (int i = l; i <= r; ++i) cnt[a[i] - mn]++;
    int p = l;
    for (int i = 0; i < (int)cnt.size(); i++)
        while (cnt[i]--) a[p++] = i + mn;
}

// 基数排序，区间版
void radixSort(vector<int>& a, int l, int r) {
    if (l >= r) return;
    int mx = a[l];
    for (int i = l + 1; i <= r; ++i) mx = max(mx, a[i]);
    vector<int> temp(r - l + 1);
    for (int exp = 1; mx / exp > 0; exp *= 10) {
        int cnt[10] = {0};
        for (int i = l; i <= r; ++i) cnt[(a[i] / exp) % 10]++;
        for (int i = 1; i < 10; i++) cnt[i] += cnt[i - 1];
        for (int i = r; i >= l; --i) temp[--cnt[(a[i] / exp) % 10]] = a[i];
        for (int i = l; i <= r; ++i) a[i] = temp[i - l];
    }
}

// 区间智能排序（隔离后对中间段使用）
void lxySortRange(vector<int>& a, int l, int r) {
    if (l >= r) return;
    int mx, mn;
    if (!isUnorderedEx(a, l, r, mx, mn)) return;
    if (mx - mn < 20) { countingSort(a, l, r); return; }
    if (a[r] < a[l] && (r - l + 1) <= 64) { swap(a[l], a[r]); jwjSort(a, l, r); return; }
    if (mn > 0 && mx <= 99999) { radixSort(a, l, r); return; }
    int limit = 2 * (int)log2((double)(r - l + 1)) + 1;
    quickSortInner(a, l, r, 0, limit);
}

// 主角：默认把 min 隔离到最左、max 隔离到最右，再对中间段智能排序
void lxySort(vector<int>& a, bool m) {
    if (a.empty()) return;
    int n = (int)a.size();
    int maxv, minv;
    if (!isUnorderedEx(a, 0, n - 1, maxv, minv)) return;  // 全数组已有序
    if (m) { mergeSort(a); return; }                        // 要稳定 → 归并
    if (n <= 2) { if (a[0] > a[1]) swap(a[0], a[1]); return; }

    // 隔离 min 到最左、max 到最右
    int posMax = 0, posMin = 0;
    for (int i = 1; i < n; ++i) {
        if (a[i] > a[posMax]) posMax = i;
        if (a[i] < a[posMin]) posMin = i;
    }
    swap(a[posMax], a[n - 1]);
    if (posMin == n - 1) posMin = posMax;
    swap(a[posMin], a[0]);

    lxySortRange(a, 1, n - 2);   // 中间段 [1, n-2]
}
```

## 三、lxySort 调度逻辑

```cpp
void lxySort(vector<int>& a, bool m) {
    if (a.empty()) return;
    int n = (int)a.size();
    int maxv, minv;
    if (!isUnorderedEx(a, 0, n - 1, maxv, minv)) return;  // 全数组已有序
    if (m) { mergeSort(a); return; }                        // 要稳定 → 归并
    if (n <= 2) { if (a[0] > a[1]) swap(a[0], a[1]); return; }

    // 隔离 min 到最左、max 到最右
    int posMax = 0, posMin = 0;
    for (int i = 1; i < n; ++i) {
        if (a[i] > a[posMax]) posMax = i;
        if (a[i] < a[posMin]) posMin = i;
    }
    swap(a[posMax], a[n - 1]);
    if (posMin == n - 1) posMin = posMax;
    swap(a[posMin], a[0]);

    lxySortRange(a, 1, n - 2);   // 中间段 [1, n-2]
}
```

**核心思路**：先把 `min` 隔离到最左、`max` 隔离到最右，这样**中间段的范围不再被 outlier 撑爆**，计数/基数排序能重新派上用场。然后对中间段 `[1, n-2]` 走一套同样的智能判断（`lxySortRange`）：

- **全数组已有序** → 直接返回，不白费力气
- **要稳定**（`m=true`）→ 归并排序（整段归并，min/max 自然归位）
- **数组 ≤2** → 直接交换
- **否则**：把 min/max 隔离到两端，中间段再走：
  - 范围小（`max-min<20`）→ 计数排序 `O(n)`
  - 递减小数组（≤64）→ 鸡尾酒排序
  - 正整数且 ≤99999 → 基数排序 `O(n)`
  - 兜底 → 快速排序

::: tip 为什么这样更聪明
以 `1 1 2 2 2 4 5 5 6 6 6 8 9 99999999999` 为例：
- 旧版：max 是超大 outlier，`max-min` 巨大 → 计数/基数全被卡死，只能走快排
- 新版：先把 `99999999999` 隔离到最右，中间段全是小整数，范围小 → **直接计数排序 `O(n)` 搞定**
:::

::: tip 一个细节
之前基数分支少写 `return`，导致 `radixSort` 完又被 `quickSort` 覆盖。现在隔离后由 `lxySortRange` 统一调度，不会再出现这种问题。
:::

## 四、快排的"危"：深度超限就换归并

这是整个实现里最值得讲的一环。**快速排序最怕什么？退化到 O(n²)。** 当 pivot 选得不好、数据又很"病态"时，递归会特别深，甚至爆栈。

所以给快排设了一个**深度上限**（introsort 的标准做法，这里以当前区间长度为准）：

```cpp
int limit = 2 * (int)log2((double)(r - l + 1)) + 1;
```

递归深度一旦超过这个值，说明分得极不均衡、有退化成 O(n²) 的风险——这时候**果断切换到归并排序兜底**：

```cpp
void quickSortInner(vector<int>& a, int l, int r, int depth, int limit) {
    if (depth > limit) {
        mergeSortRange(a, l, r);   // 深度超限，归并接手，避免爆栈
        return;
    }
    ...
}
```

`mergeSortRange` 本来就有——既能做全量归并，也能在快排"顶不住"时救场，一举两得。

## 五、为什么这么设计

- **没有银弹**：不同数据适合不同算法，与其赌一个，不如全都要
- **先判断再动手**：有序就省事，小范围就计数，递减就鸡尾酒……
- **隔离极值**：把 min/max 先放两端，中间段的范围更“干净”，线性排序更易命中
- **给快排上保险**：深度超限自动切归并，防止退化爆栈
- **留个兜底**：快排作为通用解，保证任何情况都有结果

## 🚀 在线演示

下面这个是我用 JS 把 lxySort 逻辑翻译过来做的**交互演示**——你可以输入任意整数，它会显示排序结果、实际用了哪个算法、以及耗时。试试预置的几个数据模式，看看它分别会走哪个分支：

<ClientOnly>
  <LxySortDemo />
</ClientOnly>

::: tip 试试这些
- 输入 `1 2 3 4 5` → 已有序，直接返回
- 输入 `9 8 7 6 5 4 3 2 1` → 递减小数组，走鸡尾酒
- 输入一堆个位数 → 范围小，走计数
- 输入大整数 → 走快排
:::

## 小结

lxySort 的核心思想一句话：**根据数组的长度、顺序、数值范围，智能选择合适的排序算法，并给快排套上深度保护的保险。**

排序算法那些我用的都是标准模板，`jwjSort` 是**鸡尾酒排序**。整个函数的意义在于"调度"——把每个算法用在它最擅长的地方，并在"危"的时候及时切换。

---

_写排序不难，难的是知道什么时候该用哪个排序，更难得的是在"危"的时候知道怎么救场。_
