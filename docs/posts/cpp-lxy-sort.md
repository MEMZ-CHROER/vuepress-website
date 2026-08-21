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
void lxySort(vector<int>& a, bool m) {
    if (a.empty()) return;
    int maxv = 0, minv = 0;
    bool iud = isUnorderedEx(a, maxv, minv);
    if (!iud)return;
    if (m == true) {
        mergeSort(a);
        return;
    }
    if (maxv - minv < 20) {
        countingSort(a);
        return;
    }
    int sz = (int)a.size();
    if (a.back() < a.front() && sz <= 64) {
        swap(a[0], a[sz - 1]);
        jwjSort(a);
        return;
    }
    if ( minv > 0 && maxv <= 99999)radixSort(a);
    quickSort(a);
    return;
    sort(a.begin(), a.end()); //样品请勿触摸
}
```

## 三、逐层拆解：它到底在判断什么

**第 1 层：空数组直接跑**
```cpp
if (a.empty()) return;
```

**第 2 层：已经有序就偷懒**
```cpp
bool iud = isUnorderedEx(a, maxv, minv);
if (!iud)return;
```
先检查一遍，如果数组已经有序，**直接返回**——不白费力气。顺手还把最大值、最小值带回来了，一石二鸟。

**第 3 层：要稳定排序就用归并**
```cpp
if (m == true) {
    mergeSort(a);
    return;
}
```
`m` 表示是否要求**稳定排序**（相同元素保持原顺序）。需要稳定时派归并。

**第 4 层：数值范围小就用计数排序**
```cpp
if (maxv - minv < 20) {
    countingSort(a);
    return;
}
```
如果最大值和最小值差不到 20，说明数值很"密"，计数排序 `O(n)` 直接起飞。

**第 5 层：小数组递减就用鸡尾酒排序**
```cpp
int sz = (int)a.size();
if (a.back() < a.front() && sz <= 64) {
    swap(a[0], a[sz - 1]);
    jwjSort(a);
    return;
}
```
如果数组是**递减**的、而且很小（≤64），先交换首尾，再用**鸡尾酒排序**（`jwjSort`，就是双向冒泡）——对这种接近有序的小数组特别快。

**第 6 层：正整数且范围合适就用基数排序**
```cpp
if ( minv > 0 && maxv <= 99999)radixSort(a);
quickSort(a);
```
全是正数、而且最大值不超过 99999，用基数排序 `O(n)`。否则就走兜底的**快速排序**。

## 四、最后那行是啥？

```cpp
sort(a.begin(), a.end()); //样品请勿触摸
```

哈哈，这是个"死代码"——因为前面的 `quickSort(a); return;` 已经返回了，这行**永远执行不到**。它是我留着当"样品"的，提醒自己 STL 的 sort 才是终极方案。**请勿触摸** 😄

## 五、为什么这么设计

- **没有银弹**：不同数据适合不同算法，与其赌一个，不如全都要
- **先判断再动手**：有序就省事，小范围就计数，递减就鸡尾酒……
- **留个兜底**：快排作为通用解，保证任何情况都有结果

## 小结

lxySort 的核心思想一句话：**根据数组的长度、顺序、数值范围，智能选择合适的排序算法。**

排序算法那些我用的都是标准模板，`jwjSort` 是**鸡尾酒排序**。整个函数的意义在于"调度"——把每个算法用在它最擅长的地方。

---

_写排序不难，难的是知道什么时候该用哪个排序。_
