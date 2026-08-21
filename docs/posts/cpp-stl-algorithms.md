---
date: 2026-08-23
category:
  - C++学习
tag:
  - C++
  - STL
  - 算法
---

# STL 三件套：sort / find / lower_bound，算法题救星

刷题的时候，最烦的就是手写排序、查找、二分。其实 STL 早就给你备好了现成的，只要会用，能省一半时间。今天把最常用的三个记下来。

## 一、sort：排序神器

```cpp
#include <algorithm>
#include <vector>

std::vector<int> v = {5, 2, 8, 1, 9};
std::sort(v.begin(), v.end());        // 从小到大
// 结果：1 2 5 8 9

// 从大到小
std::sort(v.begin(), v.end(), std::greater<int>());

// 自定义比较（按绝对值，或按结构体某个字段）
std::sort(v.begin(), v.end(), [](int a, int b) {
  return abs(a) < abs(b);
});
```

`sort` 默认是**快速排序**（内部其实是 introsort），平均 `O(n log n)`，绝大多数情况都够快。

::: tip
对数组也能用：`std::sort(a, a + n)`（数组名就是首指针，还记得上一篇吗）。
:::

## 二、find：找元素

```cpp
#include <algorithm>

std::vector<int> v = {3, 1, 4, 1, 5};
auto it = std::find(v.begin(), v.end(), 4);
if (it != v.end()) {
  std::cout << "找到了，在位置 " << (it - v.begin()) << std::endl;
} else {
  std::cout << "没找到" << std::endl;
}
```

`find` 是**线性查找**（`O(n)`），适合无序数组。如果数组已经排好序，用二分更快。

## 三、lower_bound：有序数组里的二分查找

这是**算法题的隐藏大杀器**。要求：数组必须是**有序的**。

```cpp
#include <algorithm>

std::vector<int> v = {1, 3, 5, 7, 9};
// 找第一个 >= 4 的元素位置
auto it = std::lower_bound(v.begin(), v.end(), 4);
std::cout << *it << std::endl;   // 输出 5（第一个 >=4 的）
// 位置是 it - v.begin()，这里是 2

// 找第一个 > 4 的
auto it2 = std::upper_bound(v.begin(), v.end(), 4);
std::cout << *it2 << std::endl;  // 输出 5
```

`lower_bound` / `upper_bound` 是 `O(log n)` 的二分，**比 find 快太多**。凡是"有序数组里找位置"的题，直接用它，别自己写二分（容易写错边界）。

## 四、实战套路：这三个经常一起用

比如统计"数组里有多少个 4"：

```cpp
#include <algorithm>
#include <vector>

std::vector<int> v = {1, 4, 4, 4, 7};  // 已排序
auto lo = std::lower_bound(v.begin(), v.end(), 4);
auto hi = std::upper_bound(v.begin(), v.end(), 4);
std::cout << "4 的个数：" << (hi - lo) << std::endl;  // 输出 3
```

`hi - lo` 就是区间长度，一条式子统计出出现次数，比手写循环优雅得多。

## 小结口诀

1. 排序用 `sort`（加个 lambda 就能自定义规则）
2. 无序找元素用 `find`
3. 有序找位置用 `lower_bound` / `upper_bound`
4. 计数：`upper_bound - lower_bound`

**先排序，再二分**——这是算法题里最常用的一条组合拳。把这仨用熟，你的刷题速度能快一大截。
