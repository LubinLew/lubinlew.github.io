# C++

## C++ 标准

大的标准可以认为只有三个 C++98，C++11，C++20。
从 C++11 起，被称为现代C++。

| Year | C++ Standard                                                                            | Informal Name                                           | Remark            | Link                                                                       |
| ---- | --------------------------------------------------------------------------------------- | ------------------------------------------------------- | ----------------- | -------------------------------------------------------------------------- |
| 1998 | ISO/IEC 14882:1998[[29]](https://en.wikipedia.org/wiki/C%2B%2B#cite_note-isocpp1998-29) | C++98                                                   | 第一个正式的C++标准       | [下载](https://pan.baidu.com/s/1ntjrXqx)                                     |
| 2003 | ISO/IEC 14882:2003[[30]](https://en.wikipedia.org/wiki/C%2B%2B#cite_note-isocpp2003-30) | [C++03](https://en.wikipedia.org/wiki/C%2B%2B03)        | 在C++98上面进行了小幅度的修订 | 百度不让分享                                                                     |
| 2011 | ISO/IEC 14882:2011[[31]](https://en.wikipedia.org/wiki/C%2B%2B#cite_note-isocpp2011-31) | [C++11](https://en.wikipedia.org/wiki/C%2B%2B11), C++0x | 大变更               | [下载](https://pan.baidu.com/s/1pJnrEiB)                                     |
| 2014 | ISO/IEC 14882:2014[[32]](https://en.wikipedia.org/wiki/C%2B%2B#cite_note-isocpp2014-32) | [C++14](https://en.wikipedia.org/wiki/C%2B%2B14), C++1y | 在C++11上面进行了小幅度的修订 | [下载:草案](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4296.pdf) |
| 2017 | ISO/IEC 14882:2017[[33]](https://en.wikipedia.org/wiki/C%2B%2B#cite_note-isocpp2017-33) | [C++17](https://en.wikipedia.org/wiki/C%2B%2B17), C++1z | 在C++14上面进行了小幅度的修订 | [下载:草案](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4713.pdf) |
| 2020 | ISO/IEC 14882:2020[[12]](https://en.wikipedia.org/wiki/C%2B%2B#cite_note-isocpp2020-12) | [C++20](https://en.wikipedia.org/wiki/C%2B%2B20), C++2a | 大变更               | [下载:草案](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/n4861.pdf) |
| 2023 | -                                                                                       | c++23                                                   | 在C++20上面进行了小幅度的修订 | [计划](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2214r1.html) |

> 由于C++标准的PDF是有版权的，购买需要150刀左右(每一版)，所以新版本标准网上没有资源，但是由于标准的草案是免费的，并且和最终版大体一样，所以找找最新的草案版本也可以。
> 
> 地址为 http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/

---

## 识别C++代码版本

> [How to determine the version of the C++ standard used by the compiler? - Stack Overflow](https://stackoverflow.com/questions/2324658/how-to-determine-the-version-of-the-c-standard-used-by-the-compiler)
> 
> https://gist.github.com/ax3l/53db9fa8a4f4c21ecc5c4100c0d93c94

| 版本        | [GCC](https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html)/[Clang](https://hackingcpp.com/cpp/std/macro_cplusplus.html) | [MSVC](https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus?view=msvc-170) |
| --------- | -------------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------- |
| pre-C++98 | __cplusplus == 1                                                                                                                       | 不支持                                                                                     |
| C++98     | __cplusplus == 199711L                                                                                                                 | 不支持                                                                                     |
| C++03     | __cplusplus == 199711L                                                                                                                 | 不支持                                                                                     |
| C++11     | __cplusplus == 201103L                                                                                                                 | 不支持                                                                                     |
| C++14     | __cplusplus == 201402L                                                                                                                 | _MSVC_LANG == 201402L                                                                   |
| C++17     | __cplusplus == 201703L                                                                                                                 | _MSVC_LANG == 201703L                                                                   |
| C++20     | __cplusplus == 202002L                                                                                                                 | _MSVC_LANG == 202002L                                                                   |

> [C++11 之前版本都定义为`199711L`,很多编译器都不区分C++98 和 C++03](https://en.cppreference.com/w/cpp/preprocessor/replace#Predefined_macros)
> 
> [MSVC 编译器不支持 C++11, C++03, or C++98 标准选择](https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/)
> 
> [MSVC 编译器需要增加开关 `/Zc:__cplusplus` 这样 `__cplusplus` 的值和 `_MSVC_LANG` 相同。](https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/#zc__cplusplus)

---

## 学习资料

https://en.cppreference.com/w/

[C++11 -> C++14 -> C++17](https://zhuanlan.zhihu.com/p/365221113)

[C++ 新特性](https://github.com/0voice/cpp_new_features)

[C++17 完全指南](https://github.com/MeouSker77/Cpp17)

[现代C++特性(英文)](https://github.com/AnthonyCalandra/modern-cpp-features)