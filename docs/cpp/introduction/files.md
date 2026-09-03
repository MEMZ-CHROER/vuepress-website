---
title: 文件和结构体
author: Lxy
date: 2024-01-25
category:
  - C++入门
tag:
  - C++
  - 入门
---

:::tip
文件是根据特定目的而收集在一起的有关数据的集合。C++把每一个文件都看成是一个有序的字节流，每个文件都以文件结束标志结束。如果要操作某个文件，程序必须首先打开该文件。当一个文件被打开后，该文件就和一个流关联起来，这里的流实际上是一个字节序列

C++将文件分为文本文件和二进制文件。二进制文件一般含有特殊的格式或计算机代码，如图文件和可执行文件等。文本文件则是可以用任何文字处理程序阅读和编辑的简单 ASCII 文件

下面我们学习如何编写 C++代码来实现对文本文件的输入和输出
:::

## 文件操作

> C++语言提供了一批用于文件操作的标准函数。本节不是介绍文件打开函数 fopen，而是介绍另一个函数 freopen，它们都包含于标准库 cstdio 中。文件操作的基本步骤如下：

> 1. 打开文件，将文件指针指向文件，决定打开文件的类型；
> 2. 对文件进行读、写操作；
> 3. 在使用完文件后，关闭文件。

### **重定向版**

【命令格式】

```
FILE *freopen(const char *filename, const char *mode, FILE *stream);
```

【参数说明】

- `filename`：要打开的文件名
- `mode`：文件打开的模式，和 fopen 中的模式（r/w）相同
- `stream`：文件指针，通常使用标准流文件（stdin/stdout/stderr）

其中 stdin 是标准输入流，默认为键盘；stdout 是标准输出流，默认为屏幕；stderr 是标准错误流，一般把屏幕设为默认。通过调用 freopen，就可以修改标准流文件的默认值，实现重定向。

【使用方法】
使用 freopen()函数以只读方式（read）打开输入文件 slyar.in：

```
freopen("slyar.in", "r", stdin);
```

然后使用 freopen()函数以写入方式（write）打开输出文件 slyar.out。

```
freopen("slyar.out", "w", stdout);
```

接下来的事情就是使用 freopen()函数的优点了，就像平常输入输出那样，即便实在文件操作中我们也不再需要修改 scanf、printf、cin 和 cout，而是维持代码的原样就可以了。因为 **freopen()函数重定向了标准流**，使其指向前面指定的文件，省时省力。最后只要使用 fclose 关闭输入文件和输出文件即可。

```
fclose(stdin); fclose(stdout);
```

若要恢复句柄，可以重新打开标准控制台设备文件，只是这个设备文件的名字是与操作系统相关的。

```
freopen("CON", "r", stdin);
```

代码模板：

```cpp
#include <cstdio>  // 使用 freopen 语句，须调用 cstdio 库
int main() {
    freopen("slyar.in", "r", stdin);
    freopen("slyar.out", "w", stdout);
    /* 中间按非文件输入输出原样写代码，什么都不用修改 */
    fclose(stdin); fclose(stdout);
    return 0;
}
```

例: 从 in.txt 文件中读入数据，把它们的和保存到 out.txt 文件中。

```cpp
#include <cstdio>
int main() {
    freopen("in.txt", "r", stdin);  // 定义输入文件名
    freopen("out.txt", "w", stdout);  // 定义输出文件名
    int temp, sum = 0;
    while (scanf("%d", &temp) == 1)  // (cin >> temp) 从输入文件中读入数据
        sum = sum + temp;
    printf("%d\n", sum);
    fclose(stdin); fclose(stdout);
    return 0;
}
```

【说明】`while (cin >> temp)`和`scanf("%d", &temp) == 1`主要是用于判断数据是否已经读完，以便及时终止循环。还可以用成员函数 eof 来判断是否达到数据流的末尾。对 scanf、printf 和 cin、cout 语句都适用。

### **fopen 版**

重定向用起来很方便，但并不是所有算法竞赛都允许读写文件。有的竞赛允许访问文件，但不允许使用 `freopen` 这样的重定向方式读写文件，可以使用 `fopen` 版，对 `scanf` 和 `printf` 语句适用。在 C 和 C++ 中，`fopen` 是一个常用的文件处理函数，用于打开文件并进行读写操作。这种方法相比于重定向（如 `freopen`）更加灵活，尤其是在需要对文件进行更复杂的操作时。

1. **基本概念**

- **fopen:** 用于打开一个文件。如果成功，它返回一个指向 `FILE` 类型的指针，这个指针将用于后续的文件操作。如果失败，它返回 `NULL`。
- **FILE 类型:** 这是一个在 C 标准库中定义的类型，用于代表一个打开的文件。

2. **fopen 函数**

原型：

```cpp
FILE *fopen(const char *filename, const char *mode);
```

- `filename`: 要打开的文件名。
- `mode`: 打开文件的模式。常见模式包括：
  - `"r"`: 仅读取。
  - `"w"`: 写入（如果文件存在则覆盖，不存在则创建）。
  - `"a"`: 追加（写入到文件的末尾）。
  - `"rb"`, `"wb"`, `"ab"`: 分别与上述模式相同，但用于二进制文件。

3. **文件读写**

使用 `fopen` 打开文件后，你可以使用多种函数来进行读写操作，例如 `fprintf`, `fscanf`, `fread`, `fwrite` 等。

- **fprintf 和 fscanf:** 与 `printf` 和 `scanf` 类似，但它们是用于文件操作。
- **fread 和 fwrite:** 用于二进制文件的读写。

4. **关闭文件**

打开的文件应该在操作完成后关闭，这是通过 `fclose` 函数完成的。

5. **示例**

- 简单示例

```cpp
#include <cstdio>
using namespace std;
int main() {
    FILE *fin, *fout;
    fin = fopen("in.txt", "rb");  // 定义输入文件名
    fout = fopen("out.txt", "wb");  // 定义输出文件名
    int temp, sum = 0;
    while (fscanf(fin, "%d", &temp) == 1)  // 从输入文件中读入数据
        sum = sum + temp;
    fprintf(fout, "%d\n", sum);  // cout << sum << endl;
    fclose(fin); fclose(fout);  // 关闭文件，可省略
    return 0;
}
```

- 示例 2:

假设我们要读取一个名为 "in.txt" 的文件，并将其中的整数数据累加后写入到 "out.txt" 文件中。

```c
#include <stdio.h>

int main() {
    FILE *fin, *fout;  // 文件指针
    fin = fopen("in.txt", "r");  // 打开输入文件，用于读取
    if (fin == NULL) {
        perror("Error opening input file");
        return 1;
    }

    fout = fopen("out.txt", "w");  // 打开输出文件，用于写入
    if (fout == NULL) {
        perror("Error opening output file");
        return 1;
    }

    int temp, sum = 0;
    while (fscanf(fin, "%d", &temp) == 1) {  // 从文件中读取整数
        sum += temp;
    }

    fprintf(fout, "%d\n", sum);  // 将结果写入到文件

    fclose(fin);  // 关闭文件
    fclose(fout);
    return 0;
}
```

### **文件输入输出流版**

在 C++中，文件输入输出流是通过 iostream 库中的 ifstream（用于文件读取）和 ofstream（用于文件写入）类实现的。这些类提供了与 cin 和 cout 类似的接口，但是它们是用来读写文件的。

1. **基本概念**

   **ifstream** 类用于从文件读取数据，而 **ofstream** 类用于向文件写入数据。使用这些类的基本步骤如下：

   1. 包含头文件 `<fstream> `
   2. 创建`ifstream`或`ofstream` 对象，并打开相应的文件。
   3. 使用对象进行读取或写入操作。
   4. 关闭文件。
   - **ifstream:** 用于从文件中读取数据。
   - **ofstream:** 用于向文件写入数据。
   - **fstream:** 可以用于同时读写文件。
   - **<< 和 >> 运算符:** 这些是重载的流插入和提取运算符，用于输出和输入。

2. **打开和关闭文件**

在使用文件流之前，您需要打开文件。这可以在构造函数中完成，或者使用 `.open()` 成员函数。

- 使用构造函数：
  ```cpp
  ifstream fin("input.txt");
  ofstream fout("output.txt");
  ```
- 使用 `.open()` 函数：
  ```cpp
  ifstream fin;
  fin.open("input.txt");
  ofstream fout;
  fout.open("output.txt");
  ```

关闭文件通常不是必需的，因为文件流的析构函数会在对象超出作用域时自动关闭文件。但是，您可以在需要时使用 `.close()` 方法手动关闭文件。

3. **读写操作**

- **读取数据：** 使用 `>>` 运算符从 `ifstream` 对象读取数据。
- **写入数据：** 使用 `<<` 运算符向 `ofstream` 对象写入数据。

4. **示例**

- 简单示例

```cpp
#include <iostream>
#include <fstream>
using namespace std;

int main() {
    ifstream fin("in.txt");  // 打开用于读取的文件
    ofstream fout("out.txt");  // 打开用于写入的文件

    int temp, sum = 0;
    while (fin >> temp) {  // 从文件读取数据
        sum += temp;
    }

    fout << sum << endl;  // 将结果写入文件

    fin.close();  // 关闭输入文件
    fout.close();  // 关闭输出文件
    return 0;
}
```

- 示例 2：

```cpp
#include <fstream>
#include <iostream>
using namespace std;

int main() {
    ifstream fin("input.txt");
    if (!fin) {
        cerr << "打开文件错误\n";
        return 1;
    }

    ofstream fout("output.txt");
    if (!fout) {
        cerr << "打开文件错误\n";
        return 1;
    }

    int temp, sum = 0;
    while (fin >> temp) {  // 读取输入文件中的每个整数
        sum += temp;
    }

    fout << sum << endl;  // 将总和写入输出文件

    // 文件将在对象析构时自动关闭
    // fin.close();  // 关闭输入文件
    // fout.close();  // 关闭输出文件
    return 0;
}
```

## 结构体（struct）

> 在实际问题中，一组数据往往具有不同的数据类型。例如，人口大普查时，我们需要记录每一位公民的姓名、年龄、性别、住址、身份证号码。这些信息分别要用整型、字符型、字符串型来记录。为了解决这类问题，C++语言提供了一种构造数据类型——结构体，它在数据存储方面相当于其他高级语言中的记录，但它具有面向对象的优势。

### **基本概念**

C++结构体是一种用户自定义的数据类型，它允许你组合不同类型的数据成员，并将它们作为一个单元来处理。结构体可以包含各种数据类型，例如整数、浮点数、字符、数组、甚至其他结构体等。你可以像使用内置数据类型一样使用结构体，包括声明、初始化、访问其成员等

### **结构体的定义**

结构体变量的定义有两种形式：

**(1) 定义结构体类型的同时定义变量**

```cpp
struct 结构体类型名 { // 其中struct是关键字
    成员表； // 可以有多个成员
    成员函数； // 可以有多个成员函数，也可以没有
} 结构体变量表； // 可以同时定义多个结构体变量，用“，”隔开
```

**(2) 先定义结构体再定义结构体变量**

```cpp
struct 结构体类型名 {
    成员表；
    成员函数；
};
结构体名 结构体变量表 // 同样可以同时定义多个结构体变量
```

> 说明

1. `struct` 是 C++ 中的关键字，用于定义结构体
2. `结构体类型名` 是你为结构体起的名称。在 `struct` 关键字后，紧跟着结构体的名称
3. `{}` 中是结构体的定义体，包括成员表和成员函数。你可以在结构体内部定义多个成员变量和成员函数，也可以省略成员函数
4. `成员表` 指的是结构体中定义的成员变量列表。成员变量是结构体的组成部分，它们表示结构体所包含的不同数据。成员表位于结构体定义的花括号 `{}` 内部，每个成员变量由其类型和名称组成，用分号 `;` 分隔  
   例如，在下面的示例中，`name`、`age` 和 `height` 就是结构体的成员变量：
   ```cpp
   struct Person {
       string name; // 成员变量1 // 注意 string 需要添加 <string> 头文件
       int age;     // 成员变量2
       float height;// 成员变量3
   };
   ```
5. `成员函数` 是指定义在结构体内部的函数。这些函数可以操作结构体的成员变量，并且与结构体的数据或状态相关联。在结构体中声明和定义成员函数与定义类中的成员函数类似，但它们的语法稍有不同  
   例如，下面是一个在结构体中定义的成员函数的示例：

   ```cpp
   struct Rectangle {
       int width;
       int height;

       // 成员函数，计算矩形面积
       int calculateArea() {
           return width * height;
       }
   };
   ```

   在上述示例中，`calculateArea()` 是一个成员函数，用于计算矩形的面积

6. `结构体变量表` 是指在结构体定义之后，通过结构体类型创建的实际变量的列表，你可以在定义结构体的同时创建结构体变量，当同时创建定义多个结构体变量时，用逗号 `,` 隔开，也可以在具体使用时单独创建  
   例如，下面是一个在结构体中定义的结构体变量的示例：

   ```cpp
   #include <iostream>
   using namespace std;
   // 定义结构体
   struct Person {
       string name;
       int age;
       float height;
   };

   // or 定义时同步创建
   // struct Person {
   // string name;
   // int age;
   // float height;
   // } person1 = {"LXY", 23, 1.85},  // 结构体变量1
   //   person2;     // 结构体变量2

   int main() {
       // 结构体变量表
       Person person1 = {"LXY", 23, 1.85}; // 第一个结构体变量
       Person person2; // 第二个结构体变量，需要后续初始化
       return 0;
   }
   ```

   在这个示例中，`person1` 和 `person2` 是结构体变量，它们是根据 `Person` 结构体类型创建的实际变量

   定义结构体变量数组：

   ```cpp
    struct Person {
        string name;
        int age;
        float height;
    } person[100];
   ```

   这种方式定义的结构体 `Person` 创建了一个名为 `person` 的数组，数组的大小为 `100`，每个元素都是一个 `Person` 类型的结构体变量,这时，要访问数组中的第一个元素的姓名，可以这样做：

   ```cpp
   person[0].name;
   ```

   定义结构体数组并进行初始化:

   ```cpp
    struct Person {
        string name;
        int age;
        float height;
    } person1[3] = { {"LXY", 23, 1.85}, {"Bob", 30, 1.80}, {"Charlie", 35, 1.70} },
      person2[2];
   ```

   当然,你也可以先定义结构体，然后在稍后的代码中创建结构体变量表（数组）。示例如下：

   ```cpp
   #include <iostream>
   #include <string>
   using namespace std;
   // 定义结构体
   struct Person {
       string name;
       int age;
       float height;
   };
   int main() {
       // 创建结构体变量表（数组）
       Person personArray[3] = { {"LXY", 23, 1.85}, {"Bob", 30, 1.80}, {"Charlie", 35, 1.70} };
       // 访问结构体数组成员并输出
       for (int i = 0; i < 3; ++i) {
           cout << "Person " << i+1 << ":" << endl;
           cout << "Name: " << personArray[i].name << endl;
           cout << "Age: " << personArray[i].age << endl;
           cout << "Height: " << personArray[i].height << " meters" << endl;
           cout << endl;
       }
       return 0;
   }
   ```

在定义结构体变量时注意，结构体变量名和结构体名不能相同。在定义结构体时，系统不分配实际内存，只有定义结构体变量时，系统才为其分配内存

### **结构体变量的特点**

1. 结构体变量可以整体操作，例如：

```cpp
swap(a[j], a[j + l]);
```

2. 结构体变量的成员访问也很方便、清晰，例如：

```cpp
cin >> a[i].name;
```

3. 结构体变量的初始化和数组的初始化类似，例如：

```cpp
Person person1 = {"LXY", 23, 1.85};
```

### **成员调用**

结构体变量与各个成员之间引用的一般形式为：

```
结构体变量名.成员名
```

对于上面定义的结构体变量，我们可以这样操作：

```cpp
cin >> a[i].name; // 一般情况下不能写 cin >> a[i];
a[i].total = a[i].Chinese + a[i].math; // 就像用整型变量一样，实际上结构体成员的操作与该成员类型所具有的操作是一致的。
```

成员运算符“.”在存取成员数值时使用，其优先级最高，并具有左结合性。在处理包含结构体的结构体时，可记作：

```
strua.strub.memb
```

这说明结构体变量 `strua` 有结构体成员 `strub`；结构体变量 `strub` 有成员 `memb`。

### **成员函数调用**

结构体成员函数调用的一般形式为：

```
结构体变量名.成员函数
```

结构体成员函数默认将结构体变量作为引用参数。

### **示例**

1. 访问结构体成员并输出

```cpp
#include <iostream>
#include <string> // 包含 string 头文件
using namespace std;

// 定义结构体
struct Person {
    string name; // 使用 string 类型表示姓名
    int age;
    float height;
};

int main() {
    // 初始化结构体变量
    Person person1 = {"LXY", 18, 1.85};

    // 访问结构体成员并输出
    cout << "Name: " << person1.name << endl;
    cout << "Age: " << person1.age << endl;
    cout << "Height: " << person1.height << " meters" << endl;

    return 0;
}
```

2. 成绩统计

输入 N 个学生的姓名和语文、数学的得分，按总分从高到低输出，分数相同的按输入先后输出。

**输入格式：**

- 第 1 行，有一个整数 N，N 的范围是[1-100]；
- 下面有 N 行，每行一个姓名，2 个整数。姓名由不超过 10 个的小写字母组成，整数范围是[0-100]。

**输出格式：**

总分排序后的名单。共 N 行，每行格式：姓名 语文 数学 总分。

**输入样例：**

```
4
gaoxiang 78 96
wangxi 70 99
liujia 90 87
zhangjin 78 91
```

**输出样例：**

```
liujia 90 87 177
gaoxiang 78 96 174
wangxi 70 99 169
zhangjin 78 91 169
```

分析：由于姓名是字符串，分数是整数，如果用数组保存，则要两个数组，比如：

```cpp
string name[100];
int score[100][3];
```

这种方法不利于把一个学生的信息当成一个整体处理。下面的程序中通过使用结构(struct)类型的方法来解决这个问题。

```cpp
#include <iostream>
#include <string>
using namespace std;

struct student {
    string name;
    int Chinese, math;
    int total;
};

student a[110];
int n;

int main() {
    cin >> n;
    for (int i = 0; i < n; i++) { // 对结构体中成员的赋值、取值。
        cin >> a[i].name;
        cin >> a[i].Chinese >> a[i].math;
        a[i].total = a[i].Chinese + a[i].math;
    }
    for (int i = n - 1; i > 0; i--) {
        for (int j = 0; j < i; j++) { // 冒泡排序
            if (a[j].total < a[j + 1].total) swap(a[j], a[j + 1]);
        }
    }
    for (int i = 0; i < n; i++) { // 输出
        cout << a[i].name << ' ' << a[i].Chinese << ' ' << a[i].math << ' ' << a[i].total << endl;
    }
    return 0;
}
```

### **结构体的应用**

1. 离散化基础

以后要学习使用的离散化方法编程中，通常要知道每个数排序后的编号(rank 值)。

**输入格式：**

- 第 1 行，一个整数 N，范围在[1-10000]；
- 第 2 行，有 N 个不相同的整数，每个数都是 int 范围的。

**输出格式：**

依次输出每个数的排名。

**输入样例：**

```
5
8 2 6 9 4
```

**输出样例：**

```
4 1 3 5 2
```

分析：排序是必须的，关键是如何把排名写回原来的数序。程序使用了分别对数值和下标不同关键字两次排序的方法来解决这个问题，一个数据“节点”应该包含数值、排名、下标三个元素，用结构体比较好。

```cpp
#include <iostream>
#include <algorithm>
using namespace std;

struct node {
    int data; // 数值
    int rank; // 排名
    int index; // 下标
}; // 定义struct类型

node a[10001];
int n;

bool compl(node x, node y) {
    return x.data < y.data;
} // 自定义比较函数

bool comp2(node x, node y) {
    return x.index < y.index;
} // 同上

int main() {
    cin >> n;
    for (int i = 1; i <= n; i++) {
        cin >> a[i].data;
        a[i].index = i;
    }
    sort(a + 1, a + 1 + n, compl); // 根据值排序，求排名 rank
    for (int i = 1; i <= n; i++) a[i].rank = i;
    sort(a + 1, a + 1 + n, comp2); // 根据下标排序，回到原数据次序
    for (int i = 1; i <= n; i++) {
        cout << a[i].rank << " ";
    }
    return 0;
}
```

2. 模拟链表

在图论题编程中，通常要运用邻接链表数据结构。由于动态指针比静态的数组的存取慢，很多选手就用数组模拟指针。现在就来学习一下这种方法的编程。

有 N 个点，编号从 1 到 N。有 M 条边，每条边用连接的 2 个顶点表示，如：（3,8），表示顶点 3 和 8 之间的边（无向边）。请输出每个顶点通过边相邻的顶点。

**输入格式：**

- 第 1 行，N 和 M 两个整数，N 范围在[1-5000]，M 范围在[1-100000]；
- 下面有 M 行，每行两个整数，表示一条边。

**输出格式：**

N 行，第 i 行的第 1 个数 k 表示有多少边和 i 号顶点相连，后面有 k 个数，表示哪 k 个顶点和 i 连接为一条边。

**输入样例：**

```
5 6
1 3
2 4
1 4
2 3
3 5
2 5
```

**输出样例：**

```
2 4 3
3 5 3 4
3 5 2 1
2 1 2
2 2 3
```

分析：本题中邻接链表的每个节点有 2 个成员：

```cpp
struct node {
    int v; // 定点编号
    int next; // 链表的下一个节点的下标
};
```

一开始我们给足够大的数组，保证可以保存所有边。每读入一条边（a,b），把 a 插入到 b 的链表前，再把 b 插入到 a 的链表前。

```cpp
#include <iostream>
using namespace std;

struct node {
    int v;
    int next;
};

node a[200001];
int n, m, p;
int k[5001], c[5001];

void insert(int u, int v) {
    a[++p].v = v; a[p].next = c[u];
    c[u] = p; k[u]++;
}

int main() {
    cin >> n >> m;
    for (int i = 1; i <= m; i++) {
        int u, v;
        cin >> u >> v;
        insert(u, v);
        insert(v, u);
    }
    for (int i = 1; i <= n; i++) {
        cout << k[i] << " ";
        for (int j = c[i]; j > 0; j = a[j].next) cout << a[j].v << " ";
        cout << endl;
    }
    return 0;
}
```
