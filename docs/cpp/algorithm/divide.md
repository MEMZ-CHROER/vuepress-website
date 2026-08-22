---
title: 分治算法
author: Lxy
date: 2024-09-01
category:
  - 算法
tag:
  - C++
  - 算法
---

## 基本概念

分治算法是一种通过将较大规模的问题分解为较小规模的问题，并对这些较小问题求解，从而解决整个问题的算法。分治算法的核心思想是递归地将问题拆分并解决，这种方法在二分法中尤为明显。

分治算法（Divide and Conquer）是一种重要的算法设计范式。它通过将一个复杂的问题分解成若干个规模较小且相似的子问题，递归地解决这些子问题，再将子问题的解组合起来得到原问题的解。

## 分治算法的基本步骤

1. **分解（Divide）**：
   - 将原问题分解成若干个规模较小的子问题。这些子问题通常是原问题的简化版本。

2. **解决（Conquer）**：
   - 如果子问题规模足够小，直接解决；否则，递归地解决每个子问题。

3. **合并（Combine）**：
   - 将子问题的解合并成原问题的解。

## 分治算法的特点

- 适用于能够自然地分解为若干个相似子问题的问题。
- 递归是分治算法的核心，通常使用递归函数来实现。
- 常见的分治算法有：快速排序（Quick Sort）、归并排序（Merge Sort）、二分查找（Binary Search）等。

## 常见的分治算法例子

1. **二分查找（Binary Search）**

二分查找是一种高效的查找算法，适用于在有序数组中查找元素。

**算法步骤**：

1. **分解**：将数组分成两半。
2. **解决**：检查中间元素是否是目标元素。如果是，返回该元素的索引；否则，根据目标元素与中间元素的大小关系，决定在左半部分还是右半部分递归查找。
3. **合并**：由于二分查找只需要在一侧递归，所以不需要显式的合并步骤。

**C++ 代码实现**：

```cpp
#include <iostream>
using namespace std;

int binarySearch(int arr[], int left, int right, int target) {
    if (right >= left) {
        int mid = left + (right - left) / 2;  // 计算中间索引，防止溢出

        // 检查中间元素是否为目标值
        if (arr[mid] == target)
            return mid;

        // 如果目标值小于中间元素，则在左半部分查找
        if (arr[mid] > target)
            return binarySearch(arr, left, mid - 1, target);

        // 如果目标值大于中间元素，则在右半部分查找
        return binarySearch(arr, mid + 1, right, target);
    }

    // 如果目标值不存在于数组中
    return -1;
}

int main() {
    int arr[] = {2, 3, 4, 10, 40};
    int n = sizeof(arr) / sizeof(arr[0]);
    int target = 10;
    int result = binarySearch(arr, 0, n - 1, target);
    if(result == -1)
        cout << "元素不在数组中" << endl;
    else
        cout << "元素在数组中的索引为: " << result << endl;
    return 0;
}
```

**代码详解**：

- `binarySearch` 函数是递归实现的二分查找。
- `mid` 用于计算数组的中间索引。
- 如果目标元素等于中间元素，则直接返回中间元素的索引。
- 如果目标元素小于中间元素，则递归在左半部分查找。
- 否则，递归在右半部分查找。

2. **归并排序（Merge Sort）**

归并排序是一种基于分治思想的排序算法，时间复杂度为 \(O(n \log n)\)。

**算法步骤**：

1. **分解**：将数组分成两半，直到每个子数组只包含一个元素。
2. **解决**：分别对每个子数组进行排序。
3. **合并**：将两个已排序的子数组合并成一个有序数组。

**C++ 代码实现**：

```cpp
#include <iostream>
using namespace std;

void merge(int arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int L[n1], R[n2];  // 创建临时数组

    for(int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for(int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    // 合并临时数组到原数组
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // 复制剩余元素
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        // 递归分解数组
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        // 合并已排序的子数组
        merge(arr, left, mid, right);
    }
}

void printArray(int arr[], int size) {
    for(int i = 0; i < size; i++)
        cout << arr[i] << " ";
    cout << endl;
}

int main() {
    int arr[] = {12, 11, 13, 5, 6, 7};
    int arr_size = sizeof(arr) / sizeof(arr[0]);

    cout << "排序前的数组: \n";
    printArray(arr, arr_size);

    mergeSort(arr, 0, arr_size - 1);

    cout << "排序后的数组: \n";
    printArray(arr, arr_size);
    return 0;
}
```

**代码详解**：

- `mergeSort` 函数递归地分解数组。
- `merge` 函数合并两个已排序的子数组。
- 递归的终止条件是当数组只有一个元素时，此时不再分解。
- `printArray` 用于打印数组内容，便于观察排序结果。

3. **快速排序（Quick Sort）**

快速排序是另一种重要的排序算法，平均时间复杂度为 \(O(n \log n)\)。

**算法步骤**：

1. **分解**：选择一个基准元素（pivot），将数组分成两部分，一部分比基准元素小，另一部分比基准元素大。
2. **解决**：递归地对两部分进行快速排序。
3. **合并**：快速排序没有显式的合并步骤，因为数组已经在分解阶段被排序。

**C++ 代码实现**：

```cpp
#include <iostream>
using namespace std;

int partition(int arr[], int low, int high) {
    int pivot = arr[high];  // 选择最右边的元素作为基准
    int i = low - 1;  // i 表示已处理部分的最后一个元素

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);  // 交换元素，将较小的元素移到左侧
        }
    }
    swap(arr[i + 1], arr[high]);  // 将基准元素放到正确的位置
    return (i + 1);
}

void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);  // 获取分区索引

        // 递归对分区后的子数组进行排序
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        cout << arr[i] << " ";
    cout << endl;
}

int main() {
    int arr[] = {10, 7, 8, 9, 1, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    quickSort(arr, 0, n - 1);
    cout << "排序后的数组: \n";
    printArray(arr, n);
    return 0;
}
```

**代码详解**：

- `partition` 函数用于将数组分割成两部分，左边小于基准，右边大于基准。
- `quickSort` 函数递归地调用自身来对分区后的子数组进行排序。
- 基准元素的选择和分区过程是快速排序的关键。

## 分治算法的时间复杂度分析

对于分治算法，时间复杂度通常由分解、解决、和合并的时间决定。以归并排序为例：
- **分解**：O(1)，只需找到数组中点。
- **解决**：两个子问题，每个子问题的大小是原问题的一半，总共有 log n 层递归。
- **合并**：O(n)，合并两个大小为 n/2 的数组。

因此，归并排序的时间复杂度是 O(n log n)。

## 分治算法的适用场景

分治算法广泛应用于以下场景：
- **排序问题**：如归并排序、快速排序。
- **查找问题**：如二分查找。
- **矩阵运算**：如矩阵乘法的 Strassen 算法。
- **递归问题**：如汉诺塔问题。

## 例题

1. **猜数字游戏**

在猜数字游戏中，玩家需要在1到1000之间猜测一个整数。通过每次将范围缩小一半，最终在不超过10次的尝试中，玩家可以确定这个数。这种逐步缩小范围的方法就是二分法的基本思想。

**二分法的应用：找数**

给定一个长度为n的单调递增序列，有m次查询，查询要求找出序列中最后一个小于等于某个数x的数。

**代码实现：**

```cpp
#include <iostream>
using namespace std;

int n, m, a[110000];

int main() {
    cin >> n >> m;
    for (int i = 1; i <= n; i++) cin >> a[i];
    a[0] = -1;  // 预设数组a的第一个元素为-1，表示无效值

    for (int i = 1; i <= m; i++) {
        int x;
        int left = 1, right = n, mid;
        cin >> x;

        while (left <= right) {
            mid = (left + right) / 2;  // 取当前区间的中间值
            if (a[mid] <= x) 
                left = mid + 1;  // 若a[mid]小于等于x，移动左边界到mid+1
            else 
                right = mid - 1;  // 若a[mid]大于x，移动右边界到mid-1
        }

        cout << a[right] << endl;  // 输出结果，即最后一个小于等于x的数
    }
    return 0;
}
```

**分析：**
- 代码中，通过二分法不断缩小查询范围，直到找到最后一个小于等于x的数。
- 最终，`right`指向的数就是答案。如果没有满足条件的数，`right`会为0，输出为-1。


2. **一元三次方程求解**

在给定一元三次方程的情况下，使用分治法来找到其三个不同的实根。

**分治法求解：**

1. 对于每个根x，设定搜索区间[x1, x2]。
2. 如果f(x1)与f(x2)的符号相反，则在[x1, x2]内存在一个根。
3. 通过二分法逐步缩小搜索区间，直到找到精确的根。

**代码实现：**

```cpp
double f(double x, double a, double b, double c, double d) {
    return a*x*x*x + b*x*x + c*x + d;
}

void solve(double a, double b, double c, double d) {
    for (int x = -100; x <= 100; x++) {
        double x1 = x, x2 = x + 1;
        if (f(x1, a, b, c, d) == 0) {
            printf("%.2f ", x1);
        } else if (f(x1, a, b, c, d) * f(x2, a, b, c, d) < 0) {
            while (x2 - x1 >= 0.001) {
                double mid = (x1 + x2) / 2;
                if (f(x1, a, b, c, d) * f(mid, a, b, c, d) <= 0) 
                    x2 = mid;
                else 
                    x1 = mid;
            }
            printf("%.2f ", x1);
        }
    }
    cout << endl;
}
```

**分析：**
- 该方法利用了分治思想，通过不断缩小区间，直到找到足够精确的根。
- 通过二分法缩小区间时，能够保证找到的根满足精度要求。

3. **循环比赛日程表 (match)**

设有n个选手（n=2^m）进行循环比赛，每名选手与其他所有选手比赛一次，共进行n-1天。

**分治法生成比赛日程表：**

通过将问题分解为小规模的问题，并通过对称性生成整个比赛日程表。

**代码实现：**

```cpp
#include <cstdio>
const int MAXN = 1025;
int matchlist[MAXN][MAXN];
int m;

int main() {
    printf("Input m: ");
    scanf("%d", &m);
    int n = 1 << m, k = 1, half = 1;  // 1<<m相当于2^m
    matchlist[0][0] = 1;

    while (k <= m) {
        for (int i = 0; i < half; i++)  // 构造右上方方阵
            for (int j = 0; j < half; j++)
                matchlist[i][j + half] = matchlist[i][j] + half;

        for (int i = 0; i < half; i++)  // 对称交换构造下半部分方阵
            for (int j = 0; j < half; j++) {
                matchlist[i + half][j] = matchlist[i][j + half];  // 左下方方阵
                matchlist[i + half][j + half] = matchlist[i][j];  // 右下方方阵
            }

        half *= 2;
        k++;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%d ", matchlist[i][j]);
        printf("\n");
    }

    return 0;
}
```

**分析：**
- 程序通过递归的方式逐步生成比赛表。每次生成一个规模为2^k的方阵，利用对称性填充矩阵的各个部分。
- 通过分治法，n名选手的比赛日程表从1x1的初始表生成到最终的nxn表。