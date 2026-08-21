---
date: 2026-08-22
category:
  - C++学习
tag:
  - C++
  - vector
  - STL
---

# C++ vector 入门：别再手动 new 数组了

很多新手学完 `new`，就开始疯狂手动管理数组内存——`new`、`delete[]`、算大小、防越界……结果 bug 一抓一大把。其实 C++ 早就给你准备好了 `std::vector`，方便又安全。

## 一、vector 是什么？

一句话：**一个能自动管理内存、想多长就多长的动态数组**。

```cpp
#include <vector>

int main() {
  std::vector<int> v;          // 空 vector
  v.push_back(10);             // 往尾部加一个
  v.push_back(20);
  v.push_back(30);             // 现在有 3 个元素
  return 0;
}
```

你只管往里塞，**内存分配、释放、扩容**它全包了，不用你操一丝心。

## 二、老写法 vs 新写法

```cpp
// 老写法：手动 new 数组（容易忘 delete，还容易越界）
int n = 100;
int *arr = new int[n];
for (int i = 0; i < n; i++) arr[i] = i * 2;
delete[] arr;  // 忘了这行 = 内存泄漏

// 新写法：vector 自动管理
std::vector<int> arr;
for (int i = 0; i < n; i++) arr.push_back(i * 2);
// 不需要 delete，自动释放
```

看到了吧，vector 连 `delete` 都省了，而且不用担心内存泄漏。

## 三、最常用的几个操作

```cpp
std::vector<int> v = {3, 1, 4, 1, 5};

v.size();        // 元素个数：5
v.push_back(9);  // 尾部加元素
v.pop_back();    // 删尾部元素
v[0];            // 下标访问（不检查越界）
v.at(0);         // 下标访问（越界会抛异常，更安全）

// 遍历
for (size_t i = 0; i < v.size(); i++) std::cout << v[i] << " ";
// 或者用范围 for（推荐，最简洁）
for (int x : v) std::cout << x << " ";
```

## 四、预分配容量，避免频繁扩容

vector 装满后会**自动扩容**（通常是翻倍）。如果你知道大概有多少元素，先 `reserve` 一下能省很多次扩容拷贝：

```cpp
std::vector<int> v;
v.reserve(1000);       // 提前预留 1000 个的空间
for (int i = 0; i < 1000; i++) v.push_back(i);
```

## 五、vector 的坑

**坑 1：下标访问不检查边界**。`v[i]` 越界不报错，会读垃圾内存。要安全就用 `v.at(i)`，它越界会抛异常。

**坑 2：引用别乱存**。`push_back` 扩容后，之前的引用/迭代器可能失效：

```cpp
std::vector<int> v = {1, 2, 3};
int &ref = v[0];
v.push_back(100);  // 扩容了！ref 可能悬空
```

## 小结

1. vector = 自动管理的动态数组
2. 用 `push_back` 加、`size` 查长、范围 for 遍历
3. 知道大小就 `reserve` 一下
4. 要边界检查用 `at`，别迷信 `[]`

学会 vector，你基本可以告别手动 `new` 数组了。写代码瞬间清爽不少。
