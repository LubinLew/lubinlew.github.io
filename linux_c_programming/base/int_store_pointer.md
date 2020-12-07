# 存储指针的整型

通常在32系统中,指针是32位的可以使用 int/long 来保存指针的值，但是在64位系统中只能有long来保存指针的值，因为他们都是64位。不能平台造成指针的大小不同，保存指针值得整型也不同，为了解决这个问题，C99 标准定义了 `intptr_t` 和 `uintptr_t` 类型给一个可以持有一个指针值的整型变量。使用这两个类型，程序就有了可移植性。



linux中这两个类型定义在 `/usr/include/stdint.h`文件中。

```c
/* Types for `void *' pointers.  */
#if __WORDSIZE == 64
    #ifndef __intptr_t_defined
         typedef long int       intptr_t;
         #define __intptr_t_defined
    #endif
    typedef unsigned long int   uintptr_t;
#else
    #ifndef __intptr_t_defined
         typedef int            intptr_t;
         #define __intptr_t_defined
    #endif
    typedef unsigned int        uintptr_t
```

在32位系统中使用 int 类型, 在54位系统中使用 long 类型。
