# C++ 字符串

字符串库支持3种类型的字符串:

- [std::basic_string](cpp/string/basic_string/) - 设计用于操作任何字符类型的字符串的模板化类.
- [std::basic_string_view](cpp/string/basic_string_view/) (C++17) - 一个轻量级的非拥有只读视图到一个字符串的子序列中
- [NULL结尾的字符串](cpp/string/null_string/) - arrays of characters terminated by a special *null* character.

## std::basic_string

| Type           | Definition                  | Spec  |
| -------------- | --------------------------- | ----- |
| std::string    | std::basic_string<char>     | C++98 |
| std::wstring   | std::basic_string<wchar_t>  | C++98 |
| std::u8string  | std::basic_string<char8_t>  | C++20 |
| std::u16string | std::basic_string<char16_t> | C++11 |
| std::u32string | std::basic_string<char32_t> | C++11 |

## std::basic_string_view

| Type           | Definition                  | Spec  |
| -------------- | --------------------------- | ----- |
| std::string    | std::basic_string<char>     | C++17 |
| std::wstring   | std::basic_string<wchar_t>  | C++17 |
| std::u8string  | std::basic_string<char8_t>  | C++20 |
| std::u16string | std::basic_string<char16_t> | C++17 |
| std::u32string | std::basic_string<char32_t> | C++17 |

## Null-terminated strings

Null-terminated strings are arrays of characters that are terminated by a special *null* character. C++ provides functions to create, inspect, and modify null-terminated strings.

There are three types of null-terminated strings:

- [null-terminated byte strings](https://en.cppreference.com/w/cpp/string/byte "cpp/string/byte")
- [null-terminated multibyte strings](https://en.cppreference.com/w/cpp/string/multibyte "cpp/string/multibyte")
- [null-terminated wide strings](https://en.cppreference.com/w/cpp/string/wide "cpp/string/wide")

### Additional support

#### [std::char_traits](https://en.cppreference.com/w/cpp/string/char_traits)

The string library also provides class template [std::char_traits](https://en.cppreference.com/w/cpp/string/char_traits "cpp/string/char traits") that defines types and functions for [std::basic_string](https://en.cppreference.com/w/cpp/string/basic_string "cpp/string/basic string") and [std::basic_string_view](https://en.cppreference.com/w/cpp/string/basic_string_view "cpp/string/basic string view") (since C++17). The following specializations are defined:

| Defined in header [<string>](https://en.cppreference.com/w/cpp/header/string) |               |
| ----------------------------------------------------------------------------- | ------------- |
| template<> class char_traits<char>;                                           |               |
| template<> class char_traits<wchar_t>;                                        |               |
| template<> class char_traits<char8_t>;                                        | (since C++20) |
| template<> class char_traits<char16_t>;                                       | (since C++11) |
| template<> class char_traits<char32_t>;                                       | (since C++11) |
|                                                                               |               |

#### Conversions and classification

The [localizations library](https://en.cppreference.com/w/cpp/locale "cpp/locale") provides support for string conversions (e.g. [std::wstring_convert](https://en.cppreference.com/w/cpp/locale/wstring_convert "cpp/locale/wstring convert") or std::toupper) as well as functions that classify characters (e.g. std::isspace or std::isdigit).

### See also

| [C++ documentation](https://en.cppreference.com/w/cpp/locale "cpp/locale") for Localizations library |
| ---------------------------------------------------------------------------------------------------- |
| [C documentation](https://en.cppreference.com/w/c/string "c/string") for Strings library             |
