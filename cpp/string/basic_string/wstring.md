# 国际化

`std::string` 只是用来操作 ASCII 字符集, 对于多个国家的字符(国际化)就需要使用特定的对象了。下面介绍了国际化字符串的相关知识, 由于C++标准没有对国际化的字符串的输入输出做太多工作, 导致其可用性极差, 不建议使用。推荐使用 [ICU](https://icu.unicode.org/design/cpp) 库 或者 [Boost.Locale](https://www.boost.org/doc/libs/1_77_0/libs/locale/doc/html/index.html) 库。

## 基本类型

- `char` `char` 的符号性取决于编译器和目标平台： ARM 和 PowerPC 的默认设置常为无符号，而 x86 与 x64 的默认设置常为有符号。

- `wchar_t` 是 C++ 标准的关键字,用来存储宽字符表示的类型。它需要足够大以表示任何受支持的字符代码, 位数与平台实现有关,在支持 Unicode 的系统上为 32 位。
  
  `std::cin` 和 `std::cout` 将输入和输出看作是 `char` 流，因此不适合用于处理 `wchar_t` 类型，`<iostream>`头文件提供了 `std::wcin` 和 `std::wcout` 用于处理`wchar_t` 类型输入输出流。

> 注意 Windows MSVC 中 `wchar_t` 为 16 位并保存 UTF-16 代码单元,作为整数类型之一，与C++标准是不一样的。

- `char8_t` 这是一个为 UTF-8 设计的类型, 在C语言中 `char8_t` 是 `unsigned char` 的 typedef, 但是在 C++20 中他是完全独立的类型
  
  它与 unsigned char 具有相同的大小、符号性和对齐（从而与 char 和 signed char 具有相同的大小和对齐），但它是独立的类型

- `char16_t` 这是一个为 UTF-16 设计的类型, 它与 std::uint_least16_t 具有相同的大小、符号性和对齐，但它是独立的类型

- `char32_t` 这是一个为 UTF-32 设计的类型, 它与 std::uint_least32_t 具有相同的大小、符号性和对齐，但它是独立的类型

| 类型       | 位数(bit) | 说明                    | 字面量装换       | 标准    |
| -------- | ------- | --------------------- | ----------- | ----- |
| wchar_t  | 16/32   | 宽字符表示的类型              | `L`"hello"  | C++11 |
| char     | 8       | 能在目标系统上最有效地处理的字符表示的类型 | -           | -     |
| char8_t  | 8       | UTF-8 字符表示的类型,无符号整型   | `u8`"hello" | C++20 |
| char16_t | 16      | UTF-16 字符表示的类型,无符号整型  | `u`"hello"  | C++11 |
| char32_t | 32      | UTF-32 字符表示的类型,无符号整型  | `U`"hello"  | C++11 |

```cpp
#include <iostream>

int main(void)
{
#ifdef _MSVC_LANG
    std::cout << "C++ Standard Version: " <<_MSVC_LANG << std::endl;
#else
    std::cout << "C++ Standard Version: " << __cplusplus << std::endl;
#endif

    const char*     str =      "hello";
    const wchar_t*  uwstr =   L"hello";
    const char8_t*  u8str  = u8"hello";
    const char16_t* u16str =  u"hello";
    const char32_t* u32str =  U"hello";

    std::cout <<  "char    : " << str << std::endl;
    std::wcout << "wchar_t : " << uwstr << std::endl;
    //utf系列暂时无法打印, google了一下说是C++23这在完善这方面的内容
}
```

---

## string 类型

| 字符串类型            | 定义                                            | 字符类型     |
| ---------------- | --------------------------------------------- | -------- |
| `std::string`    | typedef `std::basic_string<char>` string;     | char     |
| `std::wstring`   | typedef `std::basic_string<wchar_t>` wstring; | wchar_t  |
| `std::u8string`  | typedef `basic_string<char8_t>` u8string;     | char8_t  |
| `std::u16string` | typedef `basic_string<char16_t>` u16string;   | char16_t |
| `std::u32string` | typedef `basic_string<char32_t>` u32string;   | char32_t |
