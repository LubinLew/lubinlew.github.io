# std::string

std::string 主要是用来替换传统的 char 型字符串，使用字符串的操作更加安全便捷。

为了与传统 char 型字符串兼容， std::string 中的字符串同样以 NULL 结尾。

使用时需要包含 `<string>` 头文件。

----

## std::string 对象定义

### 空对象

```cpp
std::string empty; //定义一个空字符串对象，长度为0

std::string empty{}; //和上面相同
```

### 字面量初始化

```cpp
std::string str1{"hello world"}; //使用char型字符串初始化

std::string str2{"hello world", 5}; //使用char型字符串的前5个字符初始化

std::string str3(10, 'A'); //注意这里是小括号，字符串由 10个‘A’组成

std::string str4{str3}; //使用一个字符串初始化另一个字符串

std::string str5{str3, 0, 5}; //使用一个字符串的字串初始化，第三个参数是字串的长度，不是索引
```

使用auto定义变量的时候要注意：

```cpp
auto str1 = "hello"; //str1 是 const char * 类型
auto str2 {"hello"}; //str2 是 const char * 类型
```

---

## std::string 对象操作

### 赋值

可以将 char 型字符串直接赋值给 std::string 对象

```cpp
std::string str1;
str1 = "hello world";
```

也可以用其他 std::string 对象覆盖原值

```cpp
std::string str1;
std::string str2;
str1 = "hello world";
str2 = str1;
```

### 字符串连接

可以使用 + 号操作符直接连接两个或多个字符串。也可以使用 `.append()` 函数连接两个字符串。 

加号可以连接两个std::string 对象，或者 std::string 对象 和 char型字符串。

但是不可以连接两个 char型字符串。也就是说加号的一个操作数必须是std::string对象(操作符重载)。

```cpp
std::string str1 {"hello"};
std::string str2 {"world"};

std::string str3 { str1 + str2 };
std::string str4 { str1 + " " + str2 };

//std::string str5 { "hello" + "world" }; 错误不支持
```

std::string 还可以连接字符

```cpp
std::string str1 {"hello"};
std::string str2 {"world"};

std::string str3 { str1 + ' ' + str2 };
```

另外 `+=` 操作符也可以用于 std::string 对象

```cpp
std::string str1{ "hello" };
str1 += ' ';               //字符
//str1 += 32;              //由于字符可以使用ascii码表示, 所以这样也是可以
str1 += "world";           //字符串
std::cout << str1 << std::endl; // "hello world"

std::string str2{ "str2 " };
str2 += str1;              //std::string 
std::cout << str2 << std::endl; // "str2 hello world"
```

### 字符串连接数字

std::string 不能直接连接数字类型, 所以需要将数字先转换成字符串, C++ 提供了 `std::to_string()`函数来帮助我们完成这一操作。

```cpp
std::string str1{ "num is " };

auto strInt    { std::to_string(1) };
auto strFloat  { std::to_string(2.0F) }; // F : float
auto strDouble { std::to_string(3.0L) }; // L : long double

std::cout << str1 + strInt    << std::endl;
std::cout << str1 + strFloat  << std::endl;
std::cout << str1 + strDouble << std::endl;
```

### std::string 与 字面量的转换

#### 字面量 转 std::string

第一种就是定义一个std::string 对象使用 字面量初始化

```cpp
//方法1
auto str1 {std::string{"hello"}};
```

第二种方式是在字面量后面加 `s` 标记，这和数字后面加 `u`/`lu`/`f` 等方式是相同的，但是需要声明使用`std::string_literals`。

```cpp
//方法2
using namespace std::string_literals;
auto str2 {"hello"s};
```

#### std::string 转 字面量

第一种是使用 `.c_str()` 函数，该函数返回的char指针必定是NULL结尾的。

```cpp
std::string str1 {"hello"};

const char *cstr = str1.c_str();
```

第二种是使用 `.data()` 函数，该函数返回的char指针不一定以NULL结尾，具体依赖编译器的实现。注意在C++17之中，该函数返回的是`char *`类型，C++20 即其他版本都返回`const char *`类型。

```cpp
std::string str1 {"hello"};

auto cstr = str1.data();
```

 ---

## 访问字符串中的字符

std::string 对象可以同 char型数组一样可以使用`[]`方括号来访问其中的字符，下标从0开始, 同时也可以改写其中的内容。

```cpp
std::string str1{ "hello" };

for (size_t i{}; i < str1.length(); i++) {
    str1[i] = std::toupper(str1[i]);  //修改为大写
}

std::cout << str1 << std::endl; //HELLO
```

std::string 对象是一个范围,可以使用基于范围的 for 循环模式

```cpp
std::string str1{ "hello" };

for (auto &ch : str1) {
    ch = std::toupper(ch);
}

std::cout << str1 << std::endl; //HELLO
```

---

## 字符串比较

std::string对象可以与另一个std::string对象或字面量使用 `>`、`>=`、`<`、`<=`、`==`、`!=` 操作符进行比较。

| 比较操作符 | 说明   | 标准说明        |
| ----- | ---- | ----------- |
| >     | 大于   | C++20 移除    |
| >=    | 大于等于 | C++20 移除    |
| <     | 小于   | C++20 移除    |
| <=    | 小于等于 | C++20 移除    |
| ==    | 等于   | Since C++98 |
| <=>   | 不等于  | C++20 新增    |

> 注意:除了 `==` 操作符,其他操作符重载在 C++20中已经移除, 在C++17及以下才能使用。

```cpp
std::string str1{ "hello" };    
std::string str2{ "world" };
auto str3{ "hello" }; //const char*

if (str1 != str2) {
    std::cout << "str1 != str2" << std::endl;
}

if (str1 == str3) {
    std::cout << "str1 == str3" << std::endl;
}
```

也可以使用 `.compare()` 函数进行比较, 注意:两个字符串相同的时候函数返回值为0。

```cpp
std::string str1{ "hello" };    
std::string str2{ "world" };

auto str3{ "hello" }; //const char*

if (str1.compare(str2) != 0) {
    std::cout << "str1 != str2" << std::endl;
}

if (str1.compare(str3) == 0) {
    std::cout << "str1 == str3" << std::endl;
}
```

---

## 函数

| C 函数      | C++ 替代函数     | 说明    | 备注            |
| --------- |:------------ | ----- | ------------- |
| strlen()  | str.length() | 字符串长度 | 都不包含最后的NULL字符 |
| strchr()  | str.find()   |       |               |
| strrchr() | str.rfind()  |       |               |
| strstr()  | str.find()   |       |               |
| strcat()  | str.append() |       |               |
|           |              |       |               |
|           |              |       |               |
|           |              |       |               |