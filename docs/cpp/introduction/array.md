---
title: 数组
author: Lxy
date: "2023-11-30"
category:
  - C++入门
tag:
  - C++
  - 入门
---

:::tip
在 C++中，数组是一种用于存储相同类型数据元素的数据结构。数组提供了一种有序的方式来存储和访问数据，其中每个元素都通过一个索引（下标）来标识。本章将介绍**一维数组**、**二维数组**、**字符数组**  
为什么使用数组？  
使用数组的主要原因之一是能够在一个变量名下存储多个相同类型的数据，以便更有效地组织和管理数据。数组提供了一个简单而强大的数据结构，使得数据的存储和访问更加方便
:::

## 一维数组

### 1. **一维数组的定义**

当数组中每个元素只带有一个下标时，我们称这样的数组为一维数组。一维数组的定义格式如下：

```cpp
类型标识符 数组名[常量表达式];
```

其中：

- `类型标识符` 表示数组中元素的数据类型，可以是整数、浮点数、字符等。
- `数组名` 是数组的标识符，遵循变量的命名规则。
- `常量表达式` 表示数组元素的个数，可以是常量或符号常量，但不能是变量。

例如：

```cpp
int a[10];   // 合法的一维数组定义，包含10个整数元素
int b[n];    // 非法的一维数组定义，因为 n 是变量而非常量
```

在这个例子中，`a` 是一维数组的数组名，该数组有 10 个元素，分别表示为：`a[0]`, `a[1]`, `a[2]`, ..., `a[9]`。需要注意的是，`a[10]` 不属于该数组的空间范围。

一旦定义了数组变量，C++编译程序会在内存中开辟一串连续的存储单元，存储数组的元素。例如，上述的 `a` 数组在内存中的存储结构如下：

```
a[0] | a[1] | a[2] | a[3] | a[4] | a[5] | a[6] | a[7] | a[8] | a[9]
```

数组 `a` 共有 10 个元素，占据 10 个连续的存储单元。`a` 数组的最小下标是 0，最大下标是 9，按照定义，所有 `a` 数组的元素都是整型变量。

:::warning
如果 `n` 是已经被初始化的变量，那么 `int b[n];` 这样的一维数组定义在 C++ 中是合法的。在 C++11 及之后的标准中，允许使用变量来表示数组的大小。这被称为变长数组（Variable Length Array，VLA）。

所以，下述代码是合法的：

```cpp
int n = 5;  // 已经被初始化的变量
int b[n];  // 合法的一维数组定义，包含5个整数元素
```

这样的数组在编译时会动态地分配相应大小的内存空间。需要注意的是，这种方式在标准 C++ 中是允许的，但并非所有编译器都支持，特别是对于一些嵌入式系统或特殊环境的编译器可能不支持变长数组。因此，在编写可移植的代码时，**建议使用常量表达式作为数组大小**。
:::

### 2. **一组数组的引用**

在 C++中，通过给定的数组名称和元素在数组中的位置编号（下标），程序可以引用数组中的任何一个元素。一维数组元素的引用格式如下：

```cpp
数组名[下标]
```

例如，如果 `i` 和 `j` 都是 `int` 型变量，则以下引用是合法的：

```cpp
a[5]
a[i + j]
a[i++]
```

#### 说明：

1. **下标表达式：**

   - 下标可以是任何整型值的表达式，该表达式可以包含变量和函数调用。
   - 在引用时，下标值应该在数组定义的下标范围内。

2. **下标的灵活性：**

   - 数组的灵活性体现在下标可以是变量，通过对下标变量值的灵活控制，可以实现对数组元素的灵活处理。

3. **逐个引用数组元素：**

   - C++语言只能逐个引用数组元素，而不能一次引用整个数组。
   - 例如，`int a[100], b[100]; a = b;` 这样的写法是非法的。

4. **数组元素的使用：**
   - 数组元素可以像同类型的普通变量一样使用，可以进行赋值和运算的操作，和普通变量完全相同。

#### 例子：

```cpp
int a[100];
int i = 5, j = 3;

a[5] = 42;        // 给数组元素赋值
int result = a[i + j];  // 使用表达式作为下标引用数组元素
```

在这个例子中，`a[5]` 给数组中的第 6 个元素赋值为 42，而 `a[i + j]` 则引用了数组中的一个由变量 `i` 和 `j` 决定的元素。

### 3. **一维数组的初始化**

数组的初始化可以在定义时一并完成。初始化的格式为：

```cpp
类型标识符 数组名[常量表达式] = {值1, 值2, ...};
```

例如：

```cpp
int a[5] = {1, 2, 3, 4, 5};
```

#### 说明：

1. **部分初始化：**

   - 在初始化列表中可以写出全部数组元素的值，也可以只写出部分。例如，对数组进行部分初始化的方式如下：

   ```cpp
   int x[10] = {0, 1, 2, 3, 4};
   ```

   此方法一次仅对数组的前 5 个元素依次进行初始化。

2. **全零初始化：**

   - 若要将数组元素全部初始化为 0，可以简写为 `{0}`。例如：

   ```cpp
   int a[5] = {0};
   ```

   这将把数组 `a` 的所有 5 个元素都初始化为 0。

3. **程序示例：**

   ```cpp
   #include <iostream>
   using namespace std;

   int a[5];

   int main() {
       for (int i = 0; i < 5; i++) {
           cout << a[i] << " ";
       }
       return 0;
   }
   ```

   这个程序中，数组 `a` 定义在 `main` 函数外，其初始值是 0。如果将数组定义在 `main` 函数内，数组的初始值会是随机的。

   ```cpp
   int main() {
       int a[5] = {1, 2};
       for (int i = 0; i < 5; i++) {
           cout << a[i] << " ";
       }
       return 0;
   }
   ```

   这个程序中，数组 `a` 定义在 `main` 函数内，只给 `a[0]`、`a[1]` 赋初值，但后面的 `a[2]` 到 `a[4]` 元素自动赋值为 0。

### 4. **数组越界**

在 C++语言中，使用数组时需要注意以下规定：

1. **数组元素的下标值为正整数。**
2. **在定义元素个数的下标范围内使用。**

然而，C++允许在程序中将下标写成负数或大于数组元素个数的正整数，而在编译时并不会出错。例如：

```cpp
int a[10];
a[-3] = 5;
a[20] = 15;
a[10] = 20;
int k = a[30];
```

这些语句在语法上是正确的，可以通过程序的编译。然而，它们实际上试图访问数组中并不存在的元素，这种现象被称为数组越界。

#### 示例程序：

```cpp
#include <iostream>
using namespace std;

int main() {
    int a[5];
    for (int i = 0; i <= 10; ++i) {
        a[i] = i;
        cout << a[i] << " ";
    }
    return 0;
}
```

#### 说明：

该程序能够通过编译，也能够运行，但存在问题。因为定义了 `a[5]`，在使用时数组下标超过了 4。C++语言中，数组越界访问系统时不一定会给出任何提示，程序可以超出数组边界进行读/写，导致内存混乱。

数组越界是实际编程中常见的错误，难以捕捉。越界语句本身不一定会导致程序立即出错，可能在遇到某些数据时才导致错误，因此在调试过程中可能会出现不按照次序运行的怪异现象。

### 5. **一维数组的应用**

#### 1. 提示输入 n 个数字, 逆序打印输入的 n 个数字，已知可输入数字不超过 100 个

```cpp
#include <iostream>
using namespace std;

int main() {
    int n;
    cout << "请输入数字的数量（不超过100个）: ";
    cin >> n;

    if (n > 100 || n <= 0) {
        cout << "数字的数量必须在1到100之间。" << endl;
        return 1;
    }

    int arr[100]; // 创建一个大小为100的数组

    cout << "请输入 " << n << " 个整数：" << endl;
    for (int i = 0; i < n; ++i) {
        cin >> arr[i];
    }

    cout << "逆序打印输入的数字：" << endl;
    for (int i = n - 1; i >= 0; --i) {
        cout << arr[i] << " ";
    }

    cout << endl;

    return 0;
}
```

#### 说明：

- 语句 `int a[100];` 声明了一个包含 100 个整型变量的数组，它们是 `a[0], a[1], a[2], ..., a[99]`。注意，没有 `a[100]`。
- 在程序中，数组 `a` 被声明在 `main` 函数的外部。这是因为只有放在外部时，数组 `a` 才能够开得很大。如果将数组放在 `main` 函数内，稍大的数组就可能导致程序异常退出。这涉及到内存的分配和生存周期，会在后面的讨论中详细解释。
- 在这个例子中，程序通过循环输入一系列整数，将它们存储在数组 `a` 中，然后按逆序打印这些整数。
- 注意，数组是不能进行赋值操作的，例如 `b = a` 是非法的。如果需要复制数组元素，可以使用 `memcpy` 函数。

  ```cpp
  #include <cstring>

  const int MAXN = 100;

  int a[MAXN];
  int b[MAXN];

  int main() {
      // 复制整个数组 a 到数组 b
      memcpy(b, a, sizeof(a));

      // 复制数组 a 的前 k 个元素到数组 b
      int k = 5;
      memcpy(b, a, sizeof(int) * k);

      // 复制浮点型数组 a 的前 k 个元素到浮点型数组 b
      float c[MAXN];
      float d[MAXN];
      memcpy(d, c, sizeof(float) * k);

      return 0;
  }
  ```

  这里使用了 `memcpy` 函数，需要包含头文件 `<cstring>`。`memcpy` 函数的原型是：

  ```cpp
  void *memcpy(void *dest, const void *src, size_t n);
  ```

  其中，`dest` 是目标地址，`src` 是源地址，`n` 是要复制的字节数。在上述例子中，`sizeof(a)` 表示数组 `a` 的总字节数(**注意非全部复制时需要`sizeof`其正确类型**)。

#### 2. 将数组中第一个元素移到数组末尾，其余数据依次往前平移一个位置

```cpp
#include <iostream>
#include <iomanip>

const int n = 10;

using namespace std;

int a[n];

int main() {
    cout << "Read " << n << " datas:" << endl;
    for (int i = 0; i < n; ++i)
        cin >> a[i];

    int temp = a[0];

    for (int i = 0; i < n - 1; ++i)
        a[i] = a[i + 1];

    a[n - 1] = temp;

    cout << "Result:" << endl;
    for (int i = 0; i < n; ++i)
        cout << setw(3) << a[i];

    return 0;
}
```

#### 运行结果：

```
Read 10 datas:
1 2 3 4 5 6 7 8 9 10
Result:
  2  3  4  5  6  7  8  9 10  1
```

#### 说明：

- 程序通过循环输入数组元素，然后将数组中第一个元素移到数组末尾，其余元素向前平移一个位置。
- 使用了 `setw` 函数来控制输出的宽度。
- `temp` 用来保存第一个元素的值，然后通过循环将数组中的元素依次向前移动，最后将 `temp` 放在数组末尾。

#### 3. 宾馆房间的开关问题详解

问题描述：宾馆里有 100 个房间，从 1 到 100 编了号。第一个服务员把所有的房间门都打开了，第二个服务员把所有编号是 2 的倍数的房间作“相反处理”，第三个服务员把所有编号是 3 的倍数的房间作“相反处理”，以此类推。当第 100 个服务员来过后，哪几扇门是打开的？（所谓“相反处理”是：原来开着的门关上，原来关上的门打开。）

#### 分析与实现：

1. **数组表示门的状态：** 我们使用数组 `a` 来表示 100 扇门的状态，数组下标表示门的编号，数组元素表示门的状态（0 表示关，1 表示开）。初始时，所有门的状态都是 0（关）。

2. **模拟服务员的操作：** 通过两个嵌套的循环，外层循环表示服务员的编号，内层循环表示房间的编号。在内层循环中，如果房间的编号是当前服务员编号的倍数，就执行“相反处理”，即取反该门的状态。

3. **输出打开的门：** 最后通过遍历数组，输出状态为 1 的门的编号，即为最终打开的门。

4. **使用 `memset` 函数初始化数组：** 为了方便，使用 `memset` 函数将数组初始化为全 0(**需头文件`cstring`**)。

```cpp
#include <cstdio>
#include <cstring>

#define MAXN 100 + 10

int a[MAXN];

int main() {
    int n, first = 1;
    memset(a, 0, sizeof(a));

    for (int i = 1; i <= 100; ++i)
        for (int j = 1; j <= 100; ++j)
            if (j % i == 0)
                a[j] = !a[j];

    for (int i = 1; i <= 100; ++i)
        if (a[i]) {
            if (first)
                first = 0;
            else
                printf(" ");
            printf("%d", i);
        }

    printf("\n");

    return 0;
}
```

#### 运行结果：

```
1 4 9 16 25 36 49 64 81 100
```

#### 说明：

- `#define MAXN 100 + 10`使用 C++ 预处理器定义了一个常量 MAXN，它的值为 100 + 10。
- 使用数组 `a` 表示房间的开关状态，初始化为 0。
- 第一个循环模拟了服务员的操作，将相应编号的房间的开关状态取反。
- 第二个循环输出最终开着的房间编号。在输出过程中，使用了 `first` 变量来避免输出多余的空格。

:::warning
`#define` 是 C++ 中的预处理器指令，用于创建宏（macros）。宏是一种简单的文本替换机制，在编译前会对代码中的宏进行替换。`#define` 的一般形式是：

```cpp
#define 标识符 替换文本
```

其中，标识符是宏的名称，替换文本是在编译时用于替换标识符的文本。它的作用包括：

1. **定义常量：** 可以用 `#define` 来定义常量，例如：

   ```cpp
   #define PI 3.14159
   ```

   这样在代码中就可以使用 `PI` 来代表 `3.14159`。

2. **宏函数：** 可以使用 `#define` 来定义简单的宏函数，例如：

   ```cpp
   #define SQUARE(x) ((x) * (x))
   ```

   这样在代码中可以使用 `SQUARE(5)` 来代表 `25`。

3. **条件编译：** 通过 `#define` 可以控制代码的编译，例如：

   ```cpp
   #define DEBUG
   ```

   然后可以通过条件编译语句进行控制：

   ```cpp
   #ifdef DEBUG
       // debug code
   #endif
   ```

   如果定义了 `DEBUG`，则编译器会包含 debug 相关的代码。

请注意，使用宏需要谨慎，因为它是纯文本替换，可能导致意外的行为。在大多数情况下，推荐使用 `const` 和 `inline` 函数来替代宏，因为它们更安全且易于调试。
:::

## 二维数组

### 1. **二维数组的定义**

当一维数组元素的类型也是一维数组时，构成了二维数组。一般格式为：

```cpp
数据类型 数组名[常量表达式1][常量表达式2];
```

例如：

```cpp
int a[4][10];
```

`a` 是一个 4 行、10 列的整数二维数组。

### 2. **二维数组的初始化**

二维数组的初始化方式类似于一维数组。可以将每一行分开写在各自的括号里，也可以将所有数据写在一个括号里。

```cpp
int direct[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

int matrix[3][4] = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12}
};
```

或简写为：

```cpp
int direct[4][2] = {1, 0, 0, 1, -1, 0, 0, -1}; // 尽量不要用
```

```cpp
int matrix[3][3]; // 声明一个3x3的整数矩阵

// 逐个赋值
matrix[0][0] = 1;
matrix[0][1] = 2;
// ...
```

### 3. **二维数组元素的引用**

引用格式为：

```cpp
数组名[下标1][下标2];
```

例如：

```cpp
int element = a[2][5];
```

表示获取第 3 行第 6 列的元素值。

- 二维数组的元素通过行索引和列索引访问：

```cpp
matrix[0][1] = 5; // 赋值=>将第一行第二列的元素设置为 5
int value = matrix[2][3]; // 读取第三行第四列的元素
```

- 你可以使用嵌套循环来遍历二维数组的所有元素：

```cpp
for(int i = 0; i < 3; ++i) {
    for(int j = 0; j < 4; ++j) {
        cout << matrix[i][j] << " ";
    }
    cout << endl;
}
```

### 4. **二维数组的应用**

#### 1. 在 n\*n 方阵里填入 1,2,3,...,n\*n,要求填成蛇形。例如 n = 4 时方阵为：

```cpp
  1  2  3  4
  8  7  6  5
  9 10 11 12
 16 15 14 13
```

```cpp
#include <iostream>
#include <iomanip>
using namespace std;
const int MAXN = 10;
int main() {
int n = 4;
int a[MAXN][MAXN];
    int num = 1;
    for (int i = 0; i < n; ++i) {
        if (i % 2 == 0) {
            for (int j = 0; j < n; ++j) {
                a[i][j] = num++;
            }
        } else {
            for (int j = n - 1; j >= 0; --j) {
                a[i][j] = num++;
            }
        }
    }
    // 输出
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << setw(3) << a[i][j];
        }
        cout << endl;
    }
    return 0;
}
```

## 字符数组

### 1. **字符数组的定义格式**

字符数组是指元素为字符的数组，用于存放字符序列或字符串。与一般数组定义相似，不同之处在于数组类型是字符型。定义格式如下：

```cpp
char 数组名[常量表达式];
```

其中：

- `char` 表示数组中元素的数据类型为字符。
- `数组名` 是数组的标识符，遵循变量的命名规则。
- `常量表达式` 表示字符数组元素的个数，可以是常量或符号常量，但不能是变量。

例如：

```cpp
char ch1[5];    // 合法的一维字符数组定义，包含5个字符元素
char ch2[3][5];  // 合法的二维字符数组定义，包含15个字符元素
```

在字符数组中，第一个元素同样是从下标 `0` 开始，而不是 `1`。

### 2. **字符数组的赋值**

字符数组的赋值分为数组的初始化和数组元素的赋值。初始化的方式包括用字符初始化和用字符串初始化两种，以及使用初始值表进行初始化。

#### (1) 用字符初始化数组

例如：

```cpp
char ch1[5] = {'a', 'b', 'c', 'd'};
```

如果初始值个数少于元素个数，剩余元素默认为空字符(`'\0'`)。字符数组中也可以存放若干个字符，也可以存放字符串。

```cpp
char ch2[5] = {'a', 'b', 'c', '\0'};  // 存放字符串"abc"
//  结束符 '\0'
```

#### **结束符 '\0'**

在 C 语言中，字符串实际上是字符数组。例如，字符串 `"hello"` 在内存中存储为一个包含字符 'h', 'e', 'l', 'l', 'o' 的数组。但是，这还不足以让程序知道字符串在哪里结束。

为了标识字符串的结尾，C 语言使用一个特殊的字符：空字符 `'\0'`（其 ASCII 值为 0）。这意味着上述的 `"hello"` 字符串实际上在内存中存储为六个字符：'h', 'e', 'l', 'l', 'o', '\0'。

#### 为什么需要结束符？

1. **识别字符串结尾**：没有结束符，函数如 `strlen()`（计算字符串长度）或 `printf()`（打印字符串）就无法知道字符串在哪里结束，可能会继续读取内存直到遇到随机的 `'\0'`。

2. **区分字符串和字符数组**：字符数组不一定是字符串，因为它们可能不以 `'\0'` 结束。只有以 `'\0'` 结尾的字符数组才是有效的 C 字符串。

#### 实际例子

如果你有以下的字符数组：

```cpp
char arr[] = {'h', 'e', 'l', 'l', 'o'};
```

这不是一个有效的字符串，因为它没有以 `'\0'` 结尾。要把它变成字符串，你需要：

- 注意：当我们说一个字符数组是一个**有效的字符串**时，我们的意思是它是一个以空字符 '\0' 结尾的字符序列。这是 C 语言处理字符串的标准方式。如果一个字符数组不以空字符结束，许多针对字符串的 C 标准库函数（如 strlen(), strcpy(), printf() 等）将无法正确处理它，因为它们依赖于空字符来确定字符串的结束位置。

```cpp
char str[] = {'h', 'e', 'l', 'l', 'o', '\0'};
```

或者更简单地：

```cpp
char str[] = "hello";
```

在第二种情况下，编译器会自动在字符串的末尾添加 `'\0'`

- 当定义具有足够空间来包含结束符的字符串时要小心。例如，对于 `"hello"`，字符数组的大小至少应该是 6（5 个字符加上 `'\0'`）
- 在处理字符串时，总是要确保最后有一个 `'\0'`，否则可能会导致缓冲区溢出或其他安全问题

#### (2) 用字符串初始化数组

用一个字符串初始化一维字符数组：

```cpp
char ch3[5] = "abcd";
```

注意字符串的长度应小于字符数组的大小或等于字符数组的大小减 1。对于二维字符数组，可以使用由若干个字符串组成的初始值表进行初始化。

```cpp
char ch4[3][4] = {"abc", "mno", "xyz"};
```

```cpp
char words[4][10] = {
    "apple",
    "banana",
    "cherry",
    "date"
};
// 这里，words 是一个有 4 个元素的数组，每个元素都是一个最多可以存储 9 个字符加上一个 null 终止符（\0）的字符串。
// 访问和修改元素的例子：
cout << words[1]; // 输出: banana
strcpy(words[2], "blueberry"); // 修改第三个元素（注意长度限制）
```

#### (3) 数组元素赋值

字符数组的元素赋值是给数组的各个元素赋一个字符值。

```cpp
char ch5[3];
ch5[0] = 'a';
ch5[1] = 'b';
ch5[2] = 'c';
```

对于二维、三维字符数组，也是类似的方式。

#### (4) 字符常量和字符串常量的区别

- 两者的定界符不同，字符常量由单引号括起来，字符串常量由双引号括起来。
- 字符常量只能是单个字符，而字符串常量可以是多个字符。
- 不能把一个字符串常量直接赋给一个字符变量。

例如，字符常量占一个字节，而字符串常量"a"占两个字节（一个字节存放字符，一个字节存放结束标志`\0`）。

### 3. **示例程序**

```cpp
#include <iostream>
using namespace std;

int main() {
    // 初始化一维字符数组
    char charArray1[5] = {'a', 'b', 'c', 'd', 'e'};

    // 初始化二维字符数组
    char charArray2[3][5] = {"abc", "mno", "xyz"};

    // 输出一维字符数组
    cout << "One-dimensional character array: ";
    for (int i = 0; i < 5; i++) {
        cout << charArray1[i] << " ";
    }
    cout << endl;

    // 输出二维字符数组
    cout << "Two-dimensional character array:" << endl;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            cout << charArray2[i][j] << " ";
        }
        cout << endl;
    }

    return 0;
}
```

### 4. **字符串 (String) 类型**

在 C++中，字符串是一种常见的数据类型，用于表示文本信息。C++ 提供了多种处理字符串的方式，其中最常用的是使用标准库中的 `string` 类型。`string` 类型实际上是一个类，它提供了一系列成员函数和操作符，使得字符串的处理更加方便和灵活。

#### 1. **包含头文件**

在使用字符串之前，需要包含 `<string>` 头文件：

```cpp
#include <string>
```

#### 2. **字符串的定义和初始化**

使用 `string` 类型定义和初始化字符串变量非常简便：

```cpp
#include <iostream>
#include <string>
using namespace std;
int main() {
    // 字符串的定义和初始化
    string str1 = "Hello, ";
    string str2 = "world!";

    // 字符串的拼接
    string result = str1 + str2;

    // 输出拼接后的字符串
    cout << result << endl;

    return 0;
}
```

#### 3. **字符串的基本操作**

##### (a) **字符串的长度**

可以使用 `length()` 或 `size()` 成员函数获取字符串的长度：

```cpp
std::string myString = "Hello, World!";
int length = myString.length();  // 或者使用 myString.size();
```

##### (b) **字符串的访问**

可以通过下标访问字符串中的单个字符：

```cpp
#include <string>
string fruits[4] = {
    "apple",
    "banana",
    "cherry",
    "date"
};

char firstChar = fruits[0];  // 获取第一个字符
cout << fruits[1]; // 输出: banana
fruits[2] = "blueberry"; // 修改第三个元素
```

##### (c) **字符串的连接**

使用 `+` 操作符可以将两个字符串连接起来：

```cpp
std::string str1 = "Hello, ";
std::string str2 = "world!";
std::string result = str1 + str2;  // 结果为 "Hello, world!"
```

##### (d) **字符串的比较**

字符串可以通过比较运算符进行比较：

```cpp
std::string str1 = "apple";
std::string str2 = "orange";

if (str1 == str2) {
    // 字符串相等
} else if (str1 < str2) {
    // str1 小于 str2
} else {
    // str1 大于 str2
}
```

#### 4. **动态字符串数组**

如果你不知道数组中将有多少字符串，或者字符串的数量在运行时可能改变，可以使用动态分配的字符串数组。对于 `string`，这通常意味着使用 `vector<string>`

```cpp
#include <vector>
#include <string>
using namespace std;

vector<string> names;
names.push_back("Alice");
names.push_back("Bob");
names.push_back("Charlie");
```

#### 5. **字符串的输入输出**

可以使用标准输入输出流直接进行字符串的输入输出：

```cpp
#include <iostream>
#include <string>
using namespace std;
int main() {
    // 字符串的输入
    string inputString;
    cout << "Enter a string: ";
    cin >> inputString;

    // 字符串的输出
    cout << "You entered: " << inputString << endl;

    return 0;
}
```

#### 6. **其他字符串操作**

`string` 类型还提供了很多其他有用的成员函数，如查找子串、替换字符、提取子串等。详细信息可以查阅相关文档或参考 [C++ 标准库](https://en.cppreference.com/w/cpp/string/basic_string)的使用。

下面是 `string` 类中常用成员函数的表格整理：

| 类别           | 函数                                                           | 描述                             |
| -------------- | -------------------------------------------------------------- | -------------------------------- |
| **构造函数**   | `string()`                                                     | 创建空字符串                     |
|                | `string(const string& str)`                                    | 复制构造函数                     |
|                | `string(const string& str, size_t pos, size_t len)`            | 子字符串构造函数                 |
|                | `string(const char* s)`                                        | 从 C 风格字符串构造              |
|                | `string(const char* s, size_t n)`                              | 从字符数组构造                   |
|                | `string(size_t n, char c)`                                     | 重复字符构造                     |
| **赋值**       | `string& operator=(const string& str)`                         | 赋值运算符                       |
|                | `string& assign(const string& str)`                            | 从另一个字符串赋值               |
|                | `string& assign(const string& str, size_t pos, size_t len)`    | 从另一个字符串的子字符串赋值     |
|                | `string& assign(const char* s)`                                | 从 C 风格字符串赋值              |
|                | `string& assign(const char* s, size_t n)`                      | 从字符数组赋值                   |
|                | `string& assign(size_t n, char c)`                             | 从重复字符赋值                   |
| **访问元素**   | `char& operator[](size_t pos)`                                 | 访问指定位置的字符               |
|                | `char& at(size_t pos)`                                         | 访问指定位置的字符（带范围检查） |
|                | `char& front()`                                                | 访问第一个字符                   |
|                | `char& back()`                                                 | 访问最后一个字符                 |
|                | `const char* c_str() const`                                    | 返回以 null 结尾的字符串指针     |
| **大小和容量** | `size_t size() const`                                          | 返回字符串的长度                 |
|                | `size_t max_size() const`                                      | 返回可能的最大长度               |
|                | `void resize(size_t n, char c)`                                | 改变字符串的长度                 |
|                | `size_t capacity() const`                                      | 返回存储容量                     |
|                | `void shrink_to_fit()`                                         | 减少存储容量以适应大小           |
| **修改**       | `string& append(const string& str)`                            | 追加字符串                       |
|                | `string& insert(size_t pos, const string& str)`                | 在指定位置插入字符串             |
|                | `string& erase(size_t pos, size_t len)`                        | 删除指定位置的字符               |
|                | `void clear()`                                                 | 清除内容                         |
|                | `string& replace(size_t pos, size_t len, const string& str)`   | 替换指定位置的字符               |
| **查找和替换** | `size_t find(const string& str, size_t pos)`                   | 查找字符串                       |
|                | `size_t rfind(const string& str, size_t pos)`                  | 反向查找字符串                   |
|                | `size_t find_first_of(const string& str, size_t pos)`          | 查找第一个匹配字符               |
|                | `size_t find_last_of(const string& str, size_t pos)`           | 查找最后一个匹配字符             |
|                | `size_t find_first_not_of(const string& str, size_t pos)`      | 查找第一个不匹配字符             |
|                | `size_t find_last_not_of(const string& str, size_t pos)`       | 查找最后一个不匹配字符           |
| **比较**       | `int compare(const string& str) const`                         | 比较两个字符串                   |
|                | `int compare(size_t pos, size_t len, const string& str)`       | 比较部分字符串                   |
| **子字符串**   | `string substr(size_t pos, size_t len) const`                  | 返回子字符串                     |
| **其他操作**   | `void swap(string& str)`                                       | 与另一个字符串交换内容           |
|                | `std::getline(std::istream& is, std::string& str, char delim)` | 从输入流读取一行到字符串         |
|                | `size_t copy(char* s, size_t len, size_t pos)`                 | 复制字符串的一部分到字符数组     |

这个表格列出的是 C++ 标准库中 `string` 类的常见成员函数。每个函数都有其特定的格式和调用方式。下面我将解释一些关键部分，以便于理解这些函数的格式和如何使用它们。

1. **函数格式**：

   - 每个函数条目都有一个特定的格式，包括函数名称和它接受的参数。
   - 例如，`string(const char* s, size_t n)` 是一个构造函数，它接受一个 `const char*` 类型的指针 `s` 和一个 `size_t` 类型的 `n` 作为参数。
   - `size_t size() const` 是一个不接受任何参数并返回 `size_t` 类型的成员函数。`const` 表明这个函数不会修改对象的状态。

2. **调用格式**：
   - 成员函数是通过 `std::string` 类的对象调用的。
   - 例如，如果你有一个 `std::string` 对象叫 `myString`，你可以使用 `myString.size()` 来获取字符串的长度。
   - 对于构造函数，它们在创建对象时被调用。例如，`std::string myString("Hello", 5);` 创建了一个包含前五个字符 "Hello" 的字符串。

具体来说，让我们以几个函数为例详细说明：

- `string(const char* s, size_t n)`: 这是一个构造函数，用于创建一个包含前 `n` 个字符的新字符串，这些字符从 `s` 指向的字符串中获取。

  - 使用示例：`std::string str("Hello World", 5);` 这将创建一个字符串 `str`，内容是 "Hello"。

- `size_t size() const`: 这个函数返回调用它的字符串的长度（字符数）。

  - 使用示例：`std::string str = "Hello"; size_t length = str.size();` 这里 `length` 将是 5。

- `string& append(const string& str)`: 这个函数将参数 `str` 中的字符串追加到调用它的字符串后面。

  - 使用示例：`std::string str1 = "Hello"; std::string str2 = " World"; str1.append(str2);` 现在 `str1` 将是 "Hello World"。

- `int compare(const string& str) const`: 用于比较两个字符串。如果调用字符串小于、等于或大于参数字符串，则返回负数、0 或正数。

  - 使用示例：`std::string str1 = "Hello"; std::string str2 = "World"; int result = str1.compare(str2);` 根据 `str1` 和 `str2` 的字典顺序，`result` 将是一个负数，因为 "Hello" 小于 "World"。

- `std::string` 类的 `replace` 成员函数是用于替换字符串中的一部分内容的强大工具。这个函数有几种重载形式，但基本的使用方法是指定要替换的子字符串的起始位置和长度，以及要替换成的新内容。

  - 使用示例：假设我们有一个字符串 `std::string str = "Hello World";`，现在我们想要替换 "World" 为 "C++"。

  ```cpp
  // 原始字符串
  std::string str = "Hello World";

  // 使用 replace 替换字符串中的内容
  // 参数1和2定义了替换的起始位置和长度（"World" 从索引5开始，长度为5）
  // 参数3是新的字符串
  str.replace(6, 5, "C++");

  // 现在 str 是 "Hello C++"
  ```

  在这个例子中，`replace` 函数的调用是 `str.replace(6, 5, "C++");`，其中：

  - `6` 是要开始替换的子字符串的起始索引（注意字符串索引从 0 开始）。
  - `5` 是要替换的子字符串的长度。
  - `"C++"` 是要插入的新字符串。

  经过替换后，`str` 的值变为 `"Hello C++"`。通过调整这些参数，你可以在字符串中的任何位置替换任何长度的子字符串。

### 7. **字符串处理函数**

| 函数格式                                                  | 函数功能                                      | 返回值                                                |
| --------------------------------------------------------- | --------------------------------------------- | ----------------------------------------------------- |
| `strcat(char str1[], const char str2[])`                  | 将 `str2` 连接到 `str1` 的末尾                | 连接后的字符串 `str1` 的值                            |
| `strncat(char str1[], const char str2[], size_t n)`       | 将 `str2` 的前 `n` 个字符连接到 `str1` 的末尾 | 连接后的字符串 `str1` 的值                            |
| `strcpy(char str1[], const char str2[])`                  | 将 `str2` 复制到 `str1`                       | 复制后的字符串 `str1` 的值                            |
| `strncpy(char str1[], const char str2[], size_t n)`       | 将 `str2` 的前 `n` 个字符复制到 `str1`        | 复制后的字符串 `str1` 的值                            |
| `strcmp(const char str1[], const char str2[])`            | 比较 `str1` 和 `str2` 的大小                  | 正整数（str1 大于 str2）、0、负整数（str1 小于 str2） |
| `strncmp(const char str1[], const char str2[], size_t n)` | 比较 `str1` 和 `str2` 的前 `n` 个字符         | 与 `strcmp` 类似的结果                                |
| `strlen(const char str[])`                                | 计算字符串的长度（不包括终止符 `\0`）         | 字符串的长度                                          |
| `strlwr(char str[])`                                      | 将大写字母转换为小写字母                      | 无，直接修改原字符串                                  |
| `strupr(char str[])`                                      | 将小写字母转换为大写字母                      | 无，直接修改原字符串                                  |

以上表格中的函数都是 C/C++标准库中的函数，用于进行字符串的基本操作。需要注意的是，在使用这些函数时，确保传入的字符串数组足够大，以避免溢出。

## C++数组的常见操作总结

1. **声明数组**：指定数组的类型和大小。

   ```cpp
   int arr[10];  // 声明一个整型数组，包含10个元素
   ```

2. **初始化数组**：在声明数组的同时设定初始值。

   ```cpp
   int arr[] = {1, 2, 3, 4, 5}; // 声明并初始化数组
   ```

3. **访问数组元素**：使用索引来访问数组中的元素。

   ```cpp
   int first = arr[0]; // 访问第一个元素
   ```

4. **修改数组元素**：通过索引改变数组中的元素。

   ```cpp
   arr[0] = 10; // 修改第一个元素
   ```

5. **遍历数组**：使用循环遍历数组中的每个元素。

   ```cpp
   for(int i = 0; i < 5; i++) {
       std::cout << arr[i] << " ";
   }
   ```

6. **计算数组长度**：使用 `sizeof` 运算符来计算数组的总大小和单个元素的大小，从而得到数组长度。

   ```cpp
   int length = sizeof(arr) / sizeof(arr[0]);
   ```

7. **多维数组**：声明和使用多维数组。

   ```cpp
   int multi[2][3] = {{1, 2, 3}, {4, 5, 6}}; // 一个 2x3 的二维数组
   ```

8. **数组作为函数参数**：将数组传递给函数。

   ```cpp
   void printArray(int arr[], int size) {
       for(int i = 0; i < size; i++) {
           std::cout << arr[i] << " ";
       }
   }
   ```

9. **数组与指针**：理解数组名作为指针的使用，以及指针与数组的关系。

   ```cpp
   int* ptr = arr; // 数组名 arr 可以作为指向第一个元素的指针
   ```

10. **动态数组**：使用 `new` 和 `delete` 关键字创建和销毁动态数组。
    ```cpp
    int* dynamicArray = new int[10]; // 动态分配一个整型数组
    delete[] dynamicArray; // 释放数组占用的内存
    ```
