# 

# 值类别

> [Value categories - cppreference.com](https://en.cppreference.com/w/cpp/language/value_category)

每个 C++ 表达式（一个运算符及其操作数、文字、变量名等）都具有两个独立的属性：类型(type)和值类别(value category)。 每个表达式都有一些非引用类型，并且每个表达式都属于三个主要值类别之一：prvalue、xvalue 和 lvalue。

- glvalue（“广义的” lvalue）是一个表达式，其计算确定对象或函数的身份；

- prvalue（“纯粹的” rvalue）是一个表达式，其评估
  
  - 计算内置运算符的操作数的值（此类纯右值没有结果对象），
  
  - 或者初始化一个对象（这样的纯右值被称为有一个结果对象）。结果对象可以是变量、由 new-expression 创建的对象、由临时物化创建的临时对象或其成员。 请注意，非 void 废弃表达式有一个结果对象（物化临时对象）。 此外，每个类和数组纯右值都有一个结果对象，除非它是 decltype 的操作数；

- xvalue（“eXpiring”值）是一个 glvalue，表示其资源可以重用的对象；

- lvalue（历史上所谓左值，因为左值可能出现在赋值表达式的左侧）是 glvalue 不是 xvalue；

- rvalue（历史上所谓右值，因为右值可能出现在赋值表达式的右侧）是 prvalue 或者 xvalue。

## 主要类别

### lvalue

以下表达式是左值表达式：

- `变量`/`函数`/[模板参数对象](https://en.cppreference.com/w/cpp/language/template_parameters#Non-type_template_parameter) 的名字，或者`数据成员`(不论类型),例如 `std::cin` 或者 `std::endl`。即使变量的类型是一个右值引用,由其名称组成的表达式是左值表达式（但请参阅符[Move-eligible expressions](https://en.cppreference.com/w/cpp/language/value_category#Move-eligible_expressions)）

- 返回类型为左值引用的`函数调用`或`重载运算符表达式`。例如 `std::getline(std::cin, str)`, `std::cout << 1`, `str1 = str2`, or `++it`;

- `a = b`, `a += b`, `a %= b` 和 其他所有内建 [assignment and compound assignment](https://en.cppreference.com/w/cpp/language/operator_assignment "cpp/language/operator assignment") 表达式

- `++a` 和`--a`, 内建 [pre-increment and pre-decrement](https://en.cppreference.com/w/cpp/language/operator_incdec#Built-in_prefix_operators "cpp/language/operator incdec") 表达式;

- `*p`, 内建 [indirection](https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_indirection_operator "cpp/language/operator member access") 表达式;

- a[n] and p[n]

- a.m

- p->m

- a.*mp

- p->*mp

- `a,b`

- `a ? b : c`

- 字符串,  [string literal](https://en.cppreference.com/w/cpp/language/string_literal "cpp/language/string literal"), 例如 as "Hello, world!";

- 将表达式强制转换为左值引用类型, 例如 `static_cast<int&>(x)` or `static_cast<void(&)(int)>(x)`;

- 左值引用类型的非类型模板参数

- 函数调用或重载运算符表达式，其返回类型是对函数的右值引用；(C++11)

- 将表达式转换为对函数类型的右值引用, 例如 `static_cast<void(&&)(int)>(x)`,(C++11)