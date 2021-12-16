# 字符串字面量

在 C/C++ 中, 我们将使用双引号括起来的字符串叫做字符串字面量(Literal)。



## 转义字符

字符串字面量中经常会有回车换行(`\r\n`)等特殊字符, 这些特殊字符如下:

| 转义字符   | 意义                    | 备注            |
| ------ | --------------------- | ------------- |
| `\a`   | 响铃(BEL)               | 有蜂鸣器的电脑可以发出声明 |
| `\b`   | 退格(BS)，将当前位置移到前一列     | 008           |
| `\f`   | 换页(FF)，将当前位置移到下页开头    | 012           |
| `\n`   | 换行(LF)，将当前位置移到下一行开头   | 010           |
| `\r`   | 回车(CR)，将当前位置移到本行开头    | 013           |
| `\t`   | 水平制表(HT) （跳到下一个TAB位置） | 009           |
| `\v`   | 垂直制表(VT)              | 011           |
| `\\`   | 代表一个反斜线字符'\'          | 092           |
| `\'`   | 代表一个单引号（撇号）字符         | 039           |
| `\"`   | 代表一个双引号字符             | 034           |
| `\?`   | 代表一个问号                | 三字母词,可以忽略     |
| `\0`   | 空字符(NULL)             | 000           |
| `\ddd` | 1到3位八进制数所代表的任意字符      | 三位八进制         |
| `\xhh` | 1到2位十六进制所代表的任意字符      | 二位十六进制        |

---

## 三字母词

> [c语言中的三字母词以及转义字符](https://blog.csdn.net/lmh10621/article/details/62236570)

为什么上面列表中 `?` 也需要转义呢 ?

在ANSI C 标准中，定义了**9**个**三字母词**(trigraph), 三字母词使C语言环境可以在缺少一些必需字符的字符集(比如,一些非常老式的键盘上没有[]{}#\^~|)上实现。也许是由于这些字符集我们基本上用不到，所以在大多数C语言的书籍中，我们都看不到对三字母词的讲解。

源代码中的 "三字母词"，在编译阶段会被替换为 "对应的字符"。对于以 "?" 开头的字符序列，如果不能与上面9个匹配，编译器将保持原状；一旦匹配，编译器就会做替换。

| 三字母词  | 对应字符 |
| ----- | ---- |
| `??=` | `#`  |
| `??(` | `[`  |
| `??)` | `]`  |
| `??<` | `{`  |
| `??>` | `}`  |
| `??/` | `/`  |
| `??!` | `\|` |
| `??'` | `^`  |
| `??-` | `~`  |

现在的编译器已经默认不支持了, 但是如 g++ 还是会给出提示, 使用相应的选项来支持 三字母词，MSVC则是完全不支持。

```bash
warning: trigraph ??! ignored, use -trigraphs to enable [-Wtrigraphs]
```

---

## 原始字符串字面量

当字符串字面量中有大量转义字符是, 可以很不直观, 所以 C++ 引入了原始字符串字面量的概念,原始字符串字面量可以包含任意字符,例如反斜杠、制表符、双引号 和 换行符等。定义方式如下:

```cpp
R"(...)"
```

例子

```cpp
auto str1_o {  "C:\\Program Files (x86)\\WindowsPowerShell" };
auto str1_r { R"(C:\Program Files (x86)\WindowsPowerShell)" };
std::cout << str1_r << std::endl;

//换行, 注意如果 the 和 world 前面有空格或tab也会被输出
auto str2_o{ "hello\nthe\nworld" };
auto str2_r{ R"(hello
the
world)" };
std::cout << str2_r << std::endl;
```

上面的例子都是 `char` 型的字符串字面量, 原始字符串字面量也支持其他类型的字符串字面量, 只要在`R` 前面加上与原来类型相同的前缀(`L`、`u`、`U`、`u8`)即可.

```cpp
#include <iostream>
#include <string_view>

//用来打印变量类型的模板
template <typename T>
constexpr auto type_name() {
	std::string_view name, prefix, suffix;
#ifdef __clang__
	name = __PRETTY_FUNCTION__;
	prefix = "auto type_name() [T = ";
	suffix = "]";
#elif defined(__GNUC__)
	name = __PRETTY_FUNCTION__;
	prefix = "constexpr auto type_name() [with T = ";
	suffix = "]";
#elif defined(_MSC_VER)
	name = __FUNCSIG__;
	prefix = "auto __cdecl type_name<";
	suffix = ">(void)";
#endif
	name.remove_prefix(prefix.size());
	name.remove_suffix(suffix.size());
	return name;
}

int main(void)
{
	auto str1{  LR"(hello)" }; // wchar_t
	auto str2{  uR"(hello)" }; // char16_t
	auto str3{  UR"(hello)" }; // char32_t
	auto str4{ u8R"(hello)" }; // char8_t

	std::cout << "str1: "  << typeid(str1).name() << std::endl;
	std::cout << "str1: "  << type_name<decltype(str1)>() << std::endl << std::endl;

	std::cout << "str2: "  << typeid(str2).name() << std::endl;
	std::cout << "str2: "  << type_name<decltype(str2)>() << std::endl << std::endl;

	std::cout << "str3: "  << typeid(str3).name() << std::endl;
	std::cout << "str3: "  << type_name<decltype(str3)>() << std::endl << std::endl;

	std::cout << "str4: "  << typeid(str4).name() << std::endl;
	std::cout << "str4: "  << type_name<decltype(str4)>() << std::endl << std::endl;

	return 0;
}
```

> 只有 MSVC 的 `typeid(var).name()` 才能输出正确的类型, 其他编译器输出的都是类型的表意简写
> 
> 所以需要上面定义的 [模板函数](https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c) 用来打印变量类型

### 匹配序列

当字符串中也有双引号时, 需要使用匹配序列的方式, 匹配序列必须前后一致.

```cpp
R"char-sequence(...)char-sequence"
```

`char-sequence` 是一个长度为 1~16 的字符串, 其中不能包含圆括号、空格、控制字符 和 反斜杠字符。

```cpp
#include <iostream>

int main(void)
{
	auto str1{ R"*(my name is "Lubin")*" }; //匹配序列 是 *
	std::cout << "str1: " << str1 << std::endl;

	auto str2{ R"AAAAAAAA(my name is "Lubin")AAAAAAAA" };//匹配序列 是 AAAAAAAA
	std::cout << "str2: " << str1 << std::endl;

	auto str3{ LR"---(my name is "Lubin")---" };//匹配序列 是 ---
	std::wcout << "str3: " << str1 << std::endl;

	return 0;
}
/** OUTPUT:
str1: my name is "Lubin"
str2: my name is "Lubin"
str3: my name is "Lubin"
*/
```
