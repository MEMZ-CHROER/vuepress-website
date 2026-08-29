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

## 二、完整代码（新版 lxy_sort.hpp）

lxySort 已经迭代成了**一个完整的多算法混合排序头文件** `lxy_sort.hpp`（纯模板、单头文件、header-only）。它不再是文章里的简单版本，而是包含：

- **多种排序策略**：插入 / 归并 / 自然归并 / 计数 / 基数（LSD）/ 基数（字符串）/ 基数（vector）/ Introsort（快排+堆+插入）
- **稳定与不稳定**：`lxySort`（默认）、`lxyStableSort`（严格稳定）、`lxySortByKey`（按键排序）
- **自动并行**：大规模比较型数组自动 OpenMP 并行
- **类型支持**：整数、浮点数、字符串、嵌套 vector、自定义比较器

📥 **源码下载**：[lxy_sort.hpp](/files/lxy_sort.hpp)

配套文件：
- 📥 [bench.cpp（性能对比 std::sort）](/files/bench.cpp)
- 📥 [example.cpp（使用示例）](/files/example.cpp)

编译测试：
```bash
g++ -O2 -std=c++17 -fopenmp bench.cpp -o bench   # Windows 用 g++，Linux 用 g++ -fopenmp
```

## 三、新版调度逻辑（lxySortImpl）

新版不再用"隔离 min/max"那一套，而是**按数据特征逐级分类**。整个调度器 `lxySortImpl` 的决策树如下：

```text
n ≤ 1      → O(1)
n ≤ 16     → 插入排序（小数组直接用，免检测开销）
17..128    → 检测升/降序，否则 Introsort / 归并
n > 128:
  Phase A   → 检测已排序/反转（随机数据 ~2 次即退出，近 O(1)）
  Phase B   → 扫描 max/min/降序次数（宽范围+乱序可提前终止）
  counting  → 整数小范围（range ≤ n）
  natural   → 近有序（降序次数少）→ 自然归并
  radix     → 宽范围大数组（n≥512）→ LSD 基数
  introsort → 通用比较路径（快排+堆回退+插入基）
```

伪代码（整数，稳定开关 `stable`）：

```cpp
if (n <= 16) { insertionSort(a); return; }        // 小数组
asc/desc 检测 → 已有序 O(n) 直接返回 / 反转 O(n)
扫 max/min/descCount（可提前 break）
if (maxv - minv <= n) { countingSort(a); return; } // 计数
if (descCount 少)      { naturalMerge(a); return; } // 自然归并
if (n >= 512)          { radixSort(a); return; }    // 基数
stable ? mergeSort : introsort                      // 通用路径
```

::: tip 为什么去掉"隔离 min/max"
旧版把 min/max 放两端是为了"清理中间段范围"。但新版通过 **Phase B 的提前终止** + 更精细的 range 判断（`range ≤ n` 才算小范围），加上 **naturalMerge 处理近有序**，已经能覆盖那些场景，而且**调度更简单、边界更少**。
:::

## 四、Introsort：快排的"保险"

新版通用路径用的是 **Introsort**（内省排序），它给快排上了双重保险：

```cpp
void lxyIntroRec(vector<int>& a, int l, int r, int depth) {
    if (r - l <= 16) { insertionSort(a, l, r); return; }  // 小段插入
    if (depth <= 0)  { heapSortRange(a, l, r); return; } // 深度超限→堆排
    // 三分取中选 pivot，标准快排分区...
}
```

1. **三分取中**：从 l/mid/r 取中位数当 pivot，降低退化概率
2. **深度上限**：递归深度超过 `2*log2(n)` 就切到**堆排序**（保证最坏 O(n log n)）
3. **小段插入**：区间 ≤16 直接插入排序（小数组插入最快）

相比旧版的"深度超限换归并"，Introsort 用堆排兜底——**不需要额外 O(n) 空间，且稳定保证最坏 O(n log n)**。

## 五、为什么这么设计

- **没有银弹**：不同数据适合不同算法，与其赌一个，不如全都要
- **先判断再动手**：有序就省事，小范围就计数，近有序就自然归并……
- **逐级分类**：按长度/顺序/数值范围逐级走决策树，线性排序优先命中
- **给快排上保险**：Introsort 深度超限自动切堆排，防止退化 O(n²)
- **留个兜底**：比较排序作为通用解，保证任何情况都有结果

## 🚀 在线演示

下面这个是我用 JS 把 lxySort **新版调度逻辑**翻译过来的**交互演示**——你可以输入**任意整数或浮点数**，它会显示排序结果、实际用了哪个算法、以及耗时。试试预置的几个数据模式，看看它分别会走哪个分支：

<ClientOnly>
  <LxySortDemo />
</ClientOnly>

::: tip 试试这些
- 输入 `1 2 3 4 5` → 已有序，直接返回
- 输入 `9 8 7 6 5 4 3 2 1` → 反转，直接翻转
- 输入 `5.5 2.1 8.9 1.5 9.7` → 浮点数，走比较排序
- 输入一堆个位数 → 范围小，走计数
- 输入大整数 → 走基数 / Introsort
:::

## 小结

lxySort 的核心思想一句话：**根据数组的长度、顺序、数值范围，逐级选择合适的排序算法，并给快排套上 Introsort 的保险。**

整个函数的意义在于"调度"——把每个算法用在它最擅长的地方，并在"危"的时候及时切换。

---

_写排序不难，难的是知道什么时候该用哪个排序，更难得的是在"危"的时候知道怎么救场。_
