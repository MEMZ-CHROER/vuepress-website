---
---

## 目录

1. [指针](#1-指针)
2. [二维及多维数组](#2-二维及多维数组)
3. [结构体](#3-结构体)
4. [函数](#4-函数)
5. [递推算法](#5-递推算法)
6. [排序算法](#6-排序算法)
7. [算法复杂度](#7-算法复杂度)
8. [文件操作](#8-文件操作)
9. [异常处理](#9-异常处理)
10. [典型例题与编程练习](#10-典型例题与编程练习)

---

## 1. 指针

### 1.1 指针的概念

指针是一个变量，其存储的值是另一个变量的**内存地址**。

```
变量 a 的值：10
变量 a 的地址：0x7fff5fbff8ac
指针 p 存储的值：0x7fff5fbff8ac（a的地址）
```

### 1.2 指针的定义与初始化

```cpp
#include <iostream>
using namespace std;

int main() {
    int a = 10;
    
    // 定义指向 int 类型变量的指针
    int* p = &a;   // & 是取地址运算符，p 保存 a 的地址
    // 说明：
    //   int* p   — p 是"指向 int 的指针"类型的变量
    //   &a       — 取出 a 在内存中的地址
    
    cout << "a的值：" << a << endl;          // 10
    cout << "a的地址：" << &a << endl;        // 类似 0x7fff5fbff8ac
    cout << "p的值：" << p << endl;           // 同上，a的地址
    cout << "p指向的值：" << *p << endl;      // 10，* 是解引用运算符
    
    return 0;
}
```

### 1.3 指针的三种等价的定义方式

```cpp
int* p1;    // 风格1：* 靠近类型
int *p2;    // 风格2：* 靠近变量
int * p3;   // 风格3：* 单独

// 注意：下面这个语句的含义是：p 是一个指针，指向 int；q 是普通的 int 变量
int* p, q;  // p 是指针，q 不是！
```

### 1.4 解引用运算符 `*`

`*` 运算符有两种用途：
1. **定义指针时**：表明变量是指针类型
2. **使用时**：访问指针指向的内存中的值

```cpp
int a = 5;
int* p = &a;

*p = 20;  // 通过指针修改 a 的值
cout << a << endl;  // 20

// 不能解引用未初始化的指针！
int* p2;   // 未初始化，存储垃圾地址
// *p2 = 100;  // 危险！可能崩溃
```

### 1.5 指针的赋值与运算

```cpp
int a = 10, b = 20;
int* p1 = &a;
int* p2 = &b;

// 赋值：让指针指向另一个变量
p1 = &b;  // p1 现在指向 b

// 两个指针指向同一个变量
p2 = p1;  // p2 也指向 b

// 空指针（NULL）
int* p3 = NULL;   // 不指向任何地址
if (p3 == NULL) cout << "空指针" << endl;
```

### 1.6 指针的算术运算

指针可以加上或减去一个整数，移动到相邻的内存单元。

```cpp
int arr[] = {10, 20, 30, 40, 50};
int* p = arr;  // 指向数组首元素，等价于 &arr[0]

cout << *p << endl;       // 10，p 指向 arr[0]
p++;                       // 指针向后移动一个 int 的大小
cout << *p << endl;       // 20，p 现在指向 arr[1]

p += 2;                    // 再向后移动2个 int
cout << *p << endl;       // 40，p 现在指向 arr[3]

// 指针相减（同一个数组中）
int* p1 = &arr[1];
int* p2 = &arr[4];
cout << p2 - p1 << endl;  // 3，两元素下标之差
```

**注意**：指针运算的单位是它所指向类型的大小，不是字节。

### 1.7 指针与数组

数组名在大多数情况下可以当作指针使用（数组名 = 首元素地址）。

```cpp
int arr[] = {10, 20, 30};

// 以下三种方式等价
cout << arr[0] << endl;   // 数组下标
cout << *(arr + 0) << endl;  // 首地址+偏移
cout << *arr << endl;     // 解引用数组名

// 遍历数组
for (int i = 0; i < 3; i++) {
    cout << *(arr + i) << endl;  // 等价于 arr[i]
}

// 指针遍历
for (int* p = arr; p < arr + 3; p++) {
    cout << *p << endl;
}
```

### 1.8 指针与 const

```cpp
int a = 10, b = 20;

// 指向常量的指针：不能通过此指针修改它指向的值
const int* p1 = &a;
// *p1 = 30;  // 错误！不能通过 p1 修改 a
p1 = &b;      // 可以让 p1 指向别的变量

// 常量指针：指针本身的值不能改变（即指向固定地址）
int* const p2 = &a;
*p2 = 30;      // 可以修改 a
// p2 = &b;   // 错误！p2 指向固定地址

// 指向常量的常量指针（都不能改）
const int* const p3 = &a;
// *p3 = 40;   // 错误
// p3 = &b;    // 错误
```

---

## 2. 二维及多维数组

### 2.1 二维数组的概念

二维数组可以看作"数组的数组"，即一维数组的每个元素又是一个一维数组。

```
矩阵视角：
        列0  列1  列2
行0     1    2    3
行1     4    5    6

内存视角（连续存储）：
[1] [2] [3] [4] [5] [6]
 ↑
arr[0][0]      arr[1][2]
```

### 2.2 二维数组的定义与初始化

```cpp
// 定义：类型 数组名[行数][列数];
int matrix[2][3];

// 完全初始化
int a[2][3] = {
    {1, 2, 3},
    {4, 5, 6}
};

// 部分初始化（未指定的元素为0）
int b[2][3] = {{1, 2}, {3}};  // {{1,2,0},{3,0,0}}

// 可以省略第一维的长度（由编译器自动推断）
int c[][3] = {
    {1, 2, 3},
    {4, 5, 6}
};
```

### 2.3 二维数组的访问

```cpp
int arr[2][3] = {
    {1, 2, 3},
    {4, 5, 6}
};

// 访问元素：数组名[行下标][列下标]
cout << arr[0][0] << endl;  // 1
cout << arr[1][2] << endl;  // 6

// 遍历所有元素（行优先）
for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 3; j++) {
        cout << arr[i][j] << " ";
    }
    cout << endl;
}
```

### 2.4 二维数组作为函数参数

```cpp
// 二维数组作为参数，第二维必须指定大小
void printMatrix(int arr[][3], int row) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < 3; j++) {
            cout << arr[i][j] << " ";
        }
        cout << endl;
    }
}

int main() {
    int mat[2][3] = {{1,2,3},{4,5,6}};
    printMatrix(mat, 2);
    return 0;
}
```

### 2.5 多维数组

```cpp
// 三维数组：相当于多个二维数组堆叠
int cube[2][3][4];  // 2个"面"，每个面3行4列

// 初始化
int data[2][2][2] = {
    {{1, 2}, {3, 4}},
    {{5, 6}, {7, 8}}
};

// 访问
cout << data[1][0][1] << endl;  // 6
```

### 2.6 常见应用：矩阵运算

```cpp
// 矩阵转置（行列互换）
int A[3][4], B[4][3];
for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
        B[j][i] = A[i][j];
    }
}
```

---

## 3. 结构体

### 3.1 结构体的定义

结构体是一种用户自定义的数据类型，将不同类型的数据组合在一起。

```cpp
// 定义结构体类型
struct Student {
    string name;   // 姓名
    int age;       // 年龄
    int score;     // 成绩
};  // 注意：定义结束后有分号

// 使用结构体类型定义变量
Student s1;  // C++ 中 struct 关键字可省略
```

### 3.2 结构体变量的使用

```cpp
Student s1;
s1.name = "小明";
s1.age = 16;
s1.score = 95;

// 定义时初始化
Student s2 = {"小红", 15, 88};
```

### 3.3 结构体数组

```cpp
// 结构体数组
Student class1[3] = {
    {"小明", 16, 95},
    {"小红", 15, 88},
    {"小刚", 16, 92}
};

// 遍历
for (int i = 0; i < 3; i++) {
    cout << class1[i].name << ": " << class1[i].score << endl;
}
```

### 3.4 结构体指针

```cpp
Student s = {"小明", 16, 95};
Student* p = &s;

// 通过指针访问成员（两种方式）
cout << (*p).name << endl;  // 方式1
cout << p->name << endl;   // 方式2（更常用）
cout << p->score << endl;
```

### 3.5 结构体嵌套

```cpp
struct Date {
    int year;
    int month;
    int day;
};

struct Person {
    string name;
    Date birthday;  // 结构体中包含另一个结构体
};

Person p = {"小明", {2008, 5, 15}};
cout << p.name << endl;
cout << p.birthday.year << endl;  // 2008
cout << p.birthday.month << endl; // 5
```

### 3.6 结构体作为函数参数

```cpp
// 传递结构体副本（会复制整个结构体）
void printStudent(Student s) {
    cout << s.name << " " << s.score << endl;
}

// 传递指针（推荐，更高效）
void printStudentByPtr(Student* p) {
    cout << p->name << " " << p->score << endl;
}

// 传递引用（推荐，既高效又能修改原值）
void updateScore(Student& s, int newScore) {
    s.score = newScore;
}
```

### 3.7 const 在结构体指针中的应用

```cpp
// 只读取结构体成员，不修改
void printStudentSafe(const Student* p) {
    // p->score = 100;  // 错误！不能修改
    cout << p->name << endl;  // 可以读取
}
```

---

## 4. 函数

### 4.1 模块化编程思想

将一个大程序分解为多个独立的功能模块（函数），每个函数负责完成一个特定任务。

**优点**：
- 代码复用：一次定义，多次调用
- 易于维护：修改一个函数不影响其他部分
- 结构清晰：程序结构一目了然

### 4.2 函数的定义

```cpp
// 返回类型  函数名(参数列表)
// {
//     函数体
//     return 返回值;
// }

int max(int a, int b) {
    if (a > b)
        return a;
    else
        return b;
}

void printHello() {
    cout << "Hello!" << endl;
    // void 函数不需要 return，或写 return;
}
```

### 4.3 函数的声明与调用

```cpp
#include <iostream>
using namespace std;

// 先声明函数
int max(int a, int b);   // 函数原型（声明）
void printMessage();     // 无参函数声明

int main() {
    // 调用函数
    cout << max(3, 5) << endl;  // 5
    printMessage();              // Hello!
    return 0;
}

// 再定义函数
int max(int a, int b) {
    return (a > b) ? a : b;
}

void printMessage() {
    cout << "Hello!" << endl;
}
```

### 4.4 形参与实参

- **形参（形式参数）**：函数定义时的参数，仅在函数内部有效
- **实参（实际参数）**：调用函数时传入的具体值

```cpp
int add(int x, int y) {  // x, y 是形参
    return x + y;
}

int main() {
    int a = 3, b = 4;
    int result = add(a, b);  // a, b 是实参，x=a, y=b 的值拷贝
    cout << result << endl;  // 7
    return 0;
}
```

### 4.5 参数传递方式

#### 值传递（C++ 默认）

将实参的值**拷贝**一份传给形参，函数内部修改形参不影响实参。

```cpp
void swap(int a, int b) {  // 值传递，a、b是副本
    int temp = a;
    a = b;
    b = temp;
}

int main() {
    int x = 3, y = 5;
    swap(x, y);
    cout << x << " " << y << endl;  // 3 5（不变！）
    return 0;
}
```

#### 引用传递（用 `&`）

形参是实参的**别名**，修改形参即修改实参。

```cpp
void swap(int& a, int& b) {  // 引用传递
    int temp = a;
    a = b;
    b = temp;
}

int main() {
    int x = 3, y = 5;
    swap(x, y);
    cout << x << " " << y << endl;  // 5 3（成功交换！）
    return 0;
}
```

#### 指针传递

通过传递指针，在函数内部间接修改实参的值。

```cpp
void swap(int* a, int* b) {  // 指针传递
    int temp = *a;
    *a = *b;
    *b = temp;
}

int main() {
    int x = 3, y = 5;
    swap(&x, &y);
    cout << x << " " << y << endl;  // 5 3
    return 0;
}
```

**三种方式对比**：

| 方式 | 语法 | 能否修改实参 |
|------|------|------------|
| 值传递 | `void f(int a)` | 不能 |
| 引用传递 | `void f(int& a)` | 能 |
| 指针传递 | `void f(int* a)` | 能 |

### 4.6 变量作用域
按 `LEGB` 顺序查找名字：`Local（局部）`→ `Enclosing（外层函数）`→ `Global（模块）`→ `Builtins（内置）`。

```cpp
int global = 100;  // 全局变量（所有函数都能访问）

void test() {
    int local = 50;   // 局部变量（仅在test函数内有效）
    cout << global << endl;  // 可以访问全局变量
    
    // 局部变量与全局变量同名时，局部变量优先
    int global = 200;
    cout << global << endl;  // 200（局部变量）
    cout << ::global << endl; // 100（全局变量，用::访问）
}

int main() {
    cout << global << endl;  // 100
    // cout << local << endl;  // 错误！local 在 main 中不可见
    return 0;
}
```

### 4.7 函数的递归调用

函数可以调用自身，称为递归。

```cpp
// 阶乘的递归实现
int factorial(int n) {
    if (n <= 1) return 1;       // 递归终止条件
    return n * factorial(n - 1); // 递归调用
}

// 递归执行过程（factorial(4)）：
// factorial(4) = 4 * factorial(3)
//            = 4 * 3 * factorial(2)
//            = 4 * 3 * 2 * factorial(1)
//            = 4 * 3 * 2 * 1 = 24

// 斐波那契数列
int fib(int n) {
    if (n <= 2) return 1;
    return fib(n - 1) + fib(n - 2);
}
```

**递归的两个必要条件**：
1. **递归终止条件**：防止无限递归
2. **递归公式**：将大问题分解为小问题的规律

---

## 5. 递推算法

### 5.1 递推的概念

递推是根据已知条件和关系，逐步推导，逐步计算，从初始值出发找到通项公式或最终结果。

**与递归的区别**：
- **递归**：从原问题出发，分解为子问题（自顶向下）
- **递推**：从已知出发，逐步推导到目标（自底向上）

### 5.2 递推的基本思想

利用第 n 项与前若干项之间的关系，从已知项逐步递推。

```cpp
// 已知：a1=1, a2=1, 从第三项起 an = an-1 + an-2（斐波那契）
// 求：第n项

// 方法1：递推（自底向上）
int fibonacci(int n) {
    if (n <= 2) return 1;
    int a = 1, b = 1;  // a = f(n-2), b = f(n-1)
    for (int i = 3; i <= n; i++) {
        int c = a + b;  // f(i) = f(i-2) + f(i-1)
        a = b;
        b = c;
    }
    return b;
}
```

### 5.3 递推关系的推导

**例：一重阶梯问题**
- 题目：小明上楼梯，每次走1步或2步，问上到第n阶有多少种走法？
- 分析：
  - f(1) = 1（只有 {1}）
  - f(2) = 2（{1,1} 或 {2}）
  - f(n) = f(n-1) + f(n-2)（第一步走1步 + 第一步走2步）

```cpp
int climbStairs(int n) {
    if (n <= 2) return n;
    int a = 1, b = 2;
    for (int i = 3; i <= n; i++) {
        int c = a + b;
        a = b;
        b = c;
    }
    return b;
}
```

### 5.4 递推算法解题步骤

1. 找出问题中相邻项之间的关系（递推公式）
2. 确定初始值
3. 确定递推顺序（从前往后或从后往前）
4. 循环或递归实现

---

## 6. 排序算法

### 6.1 排序的概念

**排序**：将一组数据按升序（从小到大）或降序（从大到小）重新排列。

**内部排序**：数据全部加载到内存中排序（如数组排序）
**外部排序**：数据量大，需要借助外部存储排序

### 6.2 冒泡排序（Bubble Sort）

**核心思想**：相邻两个元素比较，若逆序则交换，一趟下来最大的元素"冒泡"到最后。

```cpp
void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {          // 比较 n-1 轮
        for (int j = 0; j < n - 1 - i; j++) { // 每轮比较 n-1-i 次
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}
```

**过程示例**（升序，数组 [5, 3, 8, 4, 2]）：

```
第1轮：[3, 5, 4, 2, 8] — 8冒到最右
第2轮：[3, 4, 2, 5, 8] — 5到倒数第二
第3轮：[3, 2, 4, 5, 8] — 4到倒数第三
第4轮：[2, 3, 4, 5, 8] — 完成
```

**时间复杂度**：`O(n²)` | **空间复杂度**：`O(1)` | **稳定**：是

### 6.3 选择排序（Selection Sort）

**核心思想**：每一轮找出剩余元素中的最小值，与当前位置交换。

```cpp
void selectionSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;  // 假设当前位置是最小值
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[minIdx]) {
                minIdx = j;  // 更新最小值下标
            }
        }
        if (minIdx != i) {
            swap(arr[i], arr[minIdx]);
        }
    }
}
```

**过程示例**（数组 [64, 25, 12, 22, 11]）：
```
第1轮：找最小11，与64交换 → [11, 25, 12, 22, 64]
第2轮：找最小12，与25交换 → [11, 12, 25, 22, 64]
第3轮：找最小22，与25交换 → [11, 12, 22, 25, 64]
第4轮：已是最后一位
```

**时间复杂度**：`O(n²)` | **空间复杂度**：`O(1)` | **稳定**：否（相等时可能改变相对顺序）

### 6.4 插入排序（Insertion Sort）

**核心思想**：将数组分为"已排序区"和"未排序区"，逐个将未排序区元素插入到已排序区的正确位置。

```cpp
void insertionSort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];   // 待插入的元素
        int j = i - 1;
        // 从右往左在已排序区找插入位置
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];  // 元素后移
            j--;
        }
        arr[j + 1] = key;  // 插入
    }
}
```

**过程示例**（数组 [5, 2, 4, 6, 1, 3]）：
```
初始已排序区：[5]，未排序：[2, 4, 6, 1, 3]
插入2：[2, 5, 4, 6, 1, 3]
插入4：[2, 4, 5, 6, 1, 3]
插入6：[2, 4, 5, 6, 1, 3]（不移动）
插入1：[1, 2, 4, 5, 6, 3]
插入3：[1, 2, 3, 4, 5, 6]
```

**时间复杂度**：`O(n²)` | **空间复杂度**：`O(1)` | **稳定**：是

### 6.5 三种排序算法对比

| 算法 | 时间复杂度（平均） | 空间复杂度 | 稳定性 | 特点 |
|------|------------------|-----------|-------|------|
| 冒泡排序 | O(n²) | O(1) | 稳定 | 最简单，适合小数据 |
| 选择排序 | O(n²) | O(1) | 不稳定 | 交换次数最少 |
| 插入排序 | O(n²) | O(1) | 稳定 | 对基本有序数据效率高 |

---

## 7. 算法复杂度

### 7.1 时间复杂度

时间复杂度表示算法运行时间随数据规模增长的趋势，用大 O 记号表示。

**推导规则**：
1. 只保留最高次项
2. 忽略常数系数
3. 忽略低次项

| 复杂度 | 名称 | 常见场景 |
|--------|------|---------|
| O(1) | 常数阶 | 数组随机访问 |
| O(log n) | 对数阶 | 二分查找 |
| O(n) | 线性阶 | 单层循环遍历 |
| O(n log n) | 线性对数阶 | 高效排序（归并、快速） |
| O(n²) | 平方阶 | 双层循环（冒泡、选择、插入） |
| O(n³) | 立方阶 | 三层循环 |
| O(2ⁿ) | 指数阶 | 递归枚举（斐波那契） |
| O(n!) | 阶乘阶 | 全排列 |

### 7.2 空间复杂度

空间复杂度表示算法所需额外内存空间随数据规模增长的趋势。

```cpp
// O(1)：原地算法，只用常数个额外变量
int sum = 0;  // 1个额外空间
for (int i = 0; i < n; i++) sum += a[i];

// O(n)：需要额外数组
int b[1000];  // 额外空间与n成正比
```

### 7.3 复杂度估算示例

```cpp
// 冒泡排序：两重循环，n²
for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
        ...          // 时间：O(n²)，空间：O(1)

// 斐波那契（递归）：指数爆炸
int fib(int n) {      // 时间：O(2ⁿ)，空间：O(n)（递归栈深度）
    if (n <= 1) return n;
    return fib(n-1) + fib(n-2);
}

// 斐波那契（递推）：线性
int fib2(int n) {     // 时间：O(n)，空间：O(1)
    if (n <= 1) return n;
    int a = 1, b = 1;
    for (int i = 3; i <= n; i++) {
        int c = a + b;
        a = b; b = c;
    }
    return b;
}
```

---

## 8. 文件操作

### 8.1 C++ 文件重定向

使用 `freopen` 将标准输入/输出重定向到文件。

```cpp
#include <iostream>
#include <cstdio>
using namespace std;

// 从文件输入（相当于命令行: ./program < input.txt）
freopen("input.txt", "r", stdin);

// 输出到文件（相当于命令行: ./program > output.txt）
freopen("output.txt", "w", stdout);

int main() {
    int a, b;
    cin >> a >> b;
    cout << a + b << endl;
    // 此时输入从 input.txt 读取，输出写入 output.txt
    return 0;
}
```

### 8.2 ifstream / ofstream 文件读写

```cpp
#include <iostream>
#include <fstream>
using namespace std;

// 读取文件
int main() {
    ifstream fin("data.txt");  // 打开文件用于读取
    
    if (!fin) {
        cout << "文件打开失败" << endl;
        return 1;
    }
    
    int n;
    fin >> n;  // 从文件读取
    
    int sum = 0, x;
    for (int i = 0; i < n; i++) {
        fin >> x;
        sum += x;
    }
    fin.close();  // 关闭文件
    
    cout << sum << endl;
    return 0;
}

// 写入文件
int main() {
    ofstream fout("result.txt");  // 打开文件用于写入
    
    fout << "Hello, File!" << endl;
    fout << 123 << endl;
    
    fout.close();  // 关闭文件
    return 0;
}
```

### 8.3 文本文件与二进制文件

- **文本文件**：以 ASCII 或 Unicode 存储，可直接用文本编辑器查看
- **二进制文件**：以字节流存储，效率高但不可直接查看

```cpp
// 追加写入（不覆盖原有内容）
ofstream fout("log.txt", ios::app);
fout << "new log entry" << endl;
fout.close();
```

---

## 9. 异常处理

### 9.1 异常的概念

程序运行时可能发生错误（如除零、数组越界、文件不存在），异常处理提供了一种程序化的错误处理方式。

### 9.2 try-catch 语句

```cpp
#include <iostream>
using namespace std;

int main() {
    int a, b;
    cin >> a >> b;
    
    try {
        if (b == 0) {
            throw "除数不能为零";  // 抛出异常
        }
        cout << a / b << endl;
    }
    catch (const char* msg) {  // 捕获异常
        cout << "错误：" << msg << endl;
    }
    catch (...) {               // 捕获所有异常
        cout << "未知错误" << endl;
    }
    
    cout << "程序继续执行" << endl;
    return 0;
}
```

### 9.3 常见异常类型

```cpp
#include <iostream>
#include <stdexcept>
using namespace std;

int main() {
    try {
        int age = -5;
        if (age < 0) {
            throw invalid_argument("年龄不能为负数");
        }
    }
    catch (const invalid_argument& e) {
        cout << e.what() << endl;  // 输出异常信息
    }
    catch (...) {
        cout << "其他错误" << endl;
    }
    return 0;
}
```

### 9.4 异常处理的意义

- 将正常流程代码与错误处理代码分离
- 错误可以层层传递，由最合适的层级处理
- 避免用 if 判断所有可能错误（代码更简洁）

---

## 10. 典型例题与编程练习

### 例题1：指针与数组

**题目**：用指针实现数组元素的逆序。

```cpp
#include <iostream>
using namespace std;

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    // 双指针逆序
    int* left = arr;           // 指向首
    int* right = arr + n - 1;  // 指向尾
    
    while (left < right) {
        int temp = *left;
        *left = *right;
        *right = temp;
        left++;
        right--;
    }
    
    for (int i = 0; i < n; i++) cout << arr[i] << " ";
    cout << endl;
    
    return 0;
}
// 输出：5 4 3 2 1
```

---

### 例题2：选择排序

**题目**：实现选择排序，对数组升序排列。

```cpp
#include <iostream>
using namespace std;

void selectionSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[minIdx]) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            swap(arr[i], arr[minIdx]);
        }
    }
}

int main() {
    int arr[] = {64, 25, 12, 22, 11};
    int n = 5;
    selectionSort(arr, n);
    for (int i = 0; i < n; i++) cout << arr[i] << " ";
    cout << endl;
    return 0;
}
// 输出：11 12 22 25 64
```

---

### 例题3：结构体与排序

**题目**：有 n 名学生，包含姓名和成绩，按成绩升序排列。

```cpp
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

struct Student {
    string name;
    int score;
};

int main() {
    int n;
    cin >> n;
    Student stu[100];
    
    for (int i = 0; i < n; i++) {
        cin >> stu[i].name >> stu[i].score;
    }
    
    // 按成绩升序排序
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (stu[j].score > stu[j + 1].score) {
                swap(stu[j], stu[j + 1]);
            }
        }
    }
    
    for (int i = 0; i < n; i++) {
        cout << stu[i].name << " " << stu[i].score << endl;
    }
    return 0;
}
```

---

### 练习1：递推——爬台阶

**题目**：有 n 阶台阶，每次走1步或2步，求有多少种走法。

```cpp
#include <iostream>
using namespace std;

int main() {
    int n;
    cin >> n;
    
    if (n <= 2) {
        cout << n << endl;
        return 0;
    }
    
    int a = 1, b = 2;
    for (int i = 3; i <= n; i++) {
        int c = a + b;
        a = b;
        b = c;
    }
    cout << b << endl;
    return 0;
}
```

---

### 练习2：指针——字符串反转

**题目**：用指针将输入的字符串反转后输出。

```cpp
#include <iostream>
#include <cstring>
using namespace std;

int main() {
    char s[100];
    cin.getline(s, 100);
    
    int len = strlen(s);
    char* left = s;
    char* right = s + len - 1;
    
    while (left < right) {
        char temp = *left;
        *left = *right;
        *right = temp;
        left++;
        right--;
    }
    
    cout << s << endl;
    return 0;
}
```

---

### 练习3：函数——进制转换器

**题目**：编写函数，实现十进制转二进制。

```cpp
#include <iostream>
using namespace std;

void toBinary(int n) {
    if (n == 0) {
        cout << 0;
        return;
    }
    
    // 递归版
    if (n > 0) {
        toBinary(n / 2);
        cout << n % 2;
    }
}

int main() {
    int n;
    cin >> n;
    toBinary(n);
    cout << endl;
    return 0;
}
```

---

## 知识导图

```
┌─────────────────────────────────────────────┐
│              GESP C++ 四级知识体系              │
├─────────────────────────────────────────────┤
│  指针 → 定义/解引用/算术/与数组/const指针      │
│  二维数组 → 定义/遍历/作为函数参数            │
│  结构体 → 定义/使用/数组/指针/嵌套            │
│  函数 → 模块化/形参与实参/值传递/引用传递     │
│  变量作用域 → 全局/局部/同名覆盖             │
│  递推算法 → 递推关系推导/自底向上计算        │
│  排序算法 → 冒泡/选择/插入排序原理与代码     │
│  算法复杂度 → 时间/空间复杂度，大O记号        │
│  文件操作 → 重定向/fstream读写                │
│  异常处理 → try-catch-throw                  │
└─────────────────────────────────────────────┘
```

---

> **考试提示**：
> - 指针的 `*` 在定义时表示类型，在使用时表示解引用
> - 数组名是指针常量，不能修改
> - 交换两个数用引用传递或指针传递，值传递无法修改原值
> - 递归必须有终止条件，否则无限递归导致栈溢出
> - 冒泡排序：比较相邻元素；选择排序：找最小值；插入排序：元素插入已排序区
> - 递归实现斐波那契是 `O(2ⁿ)`，递推实现是 `O(n)`
> - 文件操作后记得 `close()` 关闭文件