# 数据类型

  [double 和  long double 的区别](https://stackoverflow.com/questions/14221612/difference-between-long-double-and-double-in-c-and-c)

## 定义整形变量

定义有以下 4 种方式

1. 定义一个整形变量，值是随机的垃圾值

```cpp
int num10;
```

2. 定义一个整形变量，使用赋值语句方式

```cpp
int num20 = 2;
int num21 = 3.14; //发生了类型转换，但是编译器不会报警
```

3. 定义一个整形变量，使用对象初始化方式

```cpp
int num30(3);
int num31(3.14); //发生了类型转换，但是编译器不会报警
int num32(num30 + num31); //变量的初始值可以是表达式
int num33(1), num34(2), num35(3); //可以一次定义多个
```

4. 定义一个整形变量，使用初始化列表方式（推荐）

```cpp
int num40 {4};
int num41 {} ; // 等价于 int num10 {0};
int num42 {num40 + num41}; //变量的初始值可以是表达式
int num43{1}, num44{2}, num45{3}; //可以一次定义多个

int num46 {3.14}; //编译报错，初始化列表赋值的类型必需与声明相同
```

> 空花括号有些类似于数字0,使得这种语法很容易被记住。对于任何基本类型，都可以使用零初始化

<mark>推荐使用初始化列表的方式，因为方法2 和 方法3 可以给整形变量赋值浮点型，编译器会自动做类型转化，并且基本不会给出提示，这可能造成精度损失。而初始化列表赋值的类型必需与声明相同，否则会报错。</mark>
这是 C++11 引入的最新语法，对初始化进行了标准化。其主要优势在于，允许以相同的方式初始化所有变量，因而常被称为统一初始化。另外一个优势是，当发生缩窄转换时，初始化列表形式要稍微安全一些。

使用 方法2 和 方法3 给整形变量赋值浮点型，是完全合法的C++语句。它们能够编译，不产生任何警告。
虽然一些编译器确实会对这样公然的缩窄转换发出警告，但不是所有编译器都会这么做。
但是，如果使用初始化列表形式,符合标准的编译器必须至少发出一条诊断消息。一些编译器甚至会发出错误，拒绝编译这种定义。我们认为不应该忽视无意造成的缩窄转换，所以倾向于使用初始化列表形式。

详细可见下面的例子：

```cpp
#include <iostream>

int main(int argc, char* argv[])
{
    int a = 1.4; //编译器会自动做类型转换，可能不会有warning告警
    int b(3.9);  //编译器会自动做类型转换，可能不会有warning告警
    int c {5};
    //ind d {6.1}; Error: Type 'double' cannot be narrowed to 'int' in initializer list

    std::cout << a << std::endl; //1
    std::cout << b << std::endl; //3， 不会四舍五入
    std::cout << c << std::endl; //5

    return 0;
}
```

### 显式类型转化

老式类型转换：

```cpp
(要转换的类型)要转换的变量
int a = (int)num;
```

新式类型转换：

```cpp
static_cast<要转换的类型>(要转换的内容)
int a = static_cast<int>(num);
```

关键字 `static_cast` 表示这个强制转换要进行静态检查，也就是说，在程序编译时进行检查。后面在介绍类的处理时，会遇到动态的的强制转换，这种转换要进行动态检査，即在程序执行时进行检丧。强制转换的结果是把从表达式计算的值转换为尖括号中指定的类型。表达式可以是任何内容，包括从单个变量到包含许多嵌套括号的复杂表达式等所有内容。通过添加显式强制转换，可以告诉编译器缩窄转换是故意执行的操作。如果转换不是缩窄转换，很少添加显式
强制转换。

### 枚举

C++包含两种枚举：限定作用域(`enum class`)的和不限定作用域(`enum`)的。

限定作用域

```cpp
enum class Day {Monday = 1, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday};

std::cout << Day::Monday << std::endl;
```

不限定作用域

```cpp
enum Day {Monday = 1, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday};

std::cout << Monday << std::endl;
```

### 数据类型的别名

旧方式

```cpp
typedef unsigned int uint32_t;
```

新方式

```cpp
using uint32_t = unsigned int;
```

两种方式没有本质上的区别，但是新方式表达方式更直观。只有当为更高级的类型指定别名时，使用 using 方法相比使用typedef语法的其他优势才会展现出来。例如，使用using时，更容易为函数类型指定别名。另外，using 关键字允许指定所谓的类型别名模板或参数化类型别名，这是使用原来的typedef语法所不能实现的。

### for 循环遍历

```cpp
for (声明  ：范围) {
    语句
}
```

```cpp
int arr[] {1,2,3,4,5}
for (int i : arr) {
    std::cout << i << std::endl;
}
```

i 是当前数组一个元素的副本, 

- 所以修改 i 的值不影响原数组的数据
- 副本是一个比较消耗性能的方式

使用引用可以很好的解决上面的问题

```cpp
int arr[] {1,2,3,4,5}
for (int& i : arr) {
    std::cout << i << std::endl;
}
```

如果不想修改数组的内容, 那么加上 const 限定

```cpp
int arr[] {1,2,3,4,5}
for (const int& i : arr) {
    std::cout << i << std::endl;
}
```

### 数组

传统数组的替代品 std::array 和 std::vector.它们可作为C++语言内置的普通数组的直接替代品，但是使用起来更容易、更安全，并且相比低级的内罝数组能提供更大的灵活性。这里的讨论并不全面，但足以像C++语言内置的数组类型那样使用它们。

#### array 容器

```cpp
std::array<TYPE, COUNT> NAME
std::array<TYPE, COUNT> NAME {初始化列表}

//等价于
TYPE NAME[COUNT]; //普通数组
```

如果创建 array 容器，但不指定初始值，则数组也将包含垃圾值，就像普通数组一样。大部分标准库类型，包括vector 和其他所有容器，总是初始化其元素， 通常初始化为0。但是，array 是特殊情况，因为它被专门设计
为尽可能接近内置数组。

array 容器对元素的访问

可以像普通数组一样通过方括号指定下标的方式，这种方式同普通数组一样不会检查下标的合法性，
而 `.at(INDEX)` 则会检查下标的合法性，如果不合法则程序会退出，提示数组越界。

```cpp
#include <iostream>
#include <array>

int main(int argc, char* argv[])
{
    std::array<int, 5> arr {1,2,3,4,5};
    std::cout << arr[0] << std::endl;
    std::cout << arr.at(0) << std::endl;

    std::cout << arr.at(6) << std::endl;

    return 0;
}
```

`.front()`  返回第一个元素

`.back()` 返回最后一个元素

`.fill(VALUE)` 将数组的所有元素都赋值为 VALUE

数组的大小

`.size()` 函数会返回数组的元素个数，类型为 size_t。

### 向量容器 vector

vector 与 array 的区别是不需要指定数组元素的个数，数组的大小是可以自动增加的，以容纳任意数量的元素。并且vector 总会初始化元素为零值。默认值可以指定。

```cpp
#include <iostream>
#include <vector>

int main(int argc, char* argv[])
{
    std::vector<int> a;       //默认 size 为0
    std::vector<int> b(10);   //含有10个元素，值均为0
    std::vector<int> c(10,7); //含有10个元素，值均为7
    std::vector<int> d {0,1,2,3,4,5,6,7,8,9}; //指定10个元素

    //std::cout << a[0] << std::endl; //错误访问非法内存
    std::cout << b[0] << std::endl; // 0
    std::cout << c[0] << std::endl; // 7
    std::cout << d[0] << std::endl; // 0

    return 0;
}
```