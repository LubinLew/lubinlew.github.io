# std::basic_string

`std::basic_string` 是一个模板类, 定义在 `<string>` 头文件中。

```cpp
template<
    class CharT,
    class Traits = std::char_traits<CharT>,
    class Allocator = std::allocator<CharT>
> class basic_string;

namespace pmr { //since C++17
    template <class CharT, class Traits = std::char_traits<CharT>>
    using basic_string = std::basic_string< CharT, Traits,
                                            std::polymorphic_allocator<CharT> >;
}
```
