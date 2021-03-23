# Nginx 中的整型

nginx 中经常可以看到 `u_char` 等类型的使用, 下面是其定义的方法。

### 常用整型定义

nginx 中 字符串指针都是使用 `u_char` 来表示。

```c
// /usr/include/bits/types.h
typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;

// /usr/include/sys/types.h
typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
```

上面结果是在CentOS7下通过GCC预编译的方法获得的。

```bash
$$ cat test.c 
#include <stdlib.h>
int main(void){
    u_char ch = 0;
    return ch;
}

$$ gcc -E test.c > test.i
# 查看 test.i 内容
```

### 保存指针地址的整型定义

关于 `intptr_t` 和 `uintptr_t` 详见 [HERE](linux_c_programming/base/integer_range.md?id=指针)，定义之后，整型就可以用来保存指针了(可移植)。

```c
typedef intptr_t        ngx_int_t;      //有符号整数
typedef uintptr_t       ngx_uint_t;     //无符号整数
typedef intptr_t        ngx_flag_t;     //相当于bool，标志量用
```

### off_t

`off_t` 是一个无符号整型, 用于指示文件偏移量, 数据位数同CPU位数相同.

```c
#ifndef __off_t_defined
# ifndef __USE_FILE_OFFSET64
typedef __off_t   off_t;
# else
typedef __off64_t off_t;
# endif
# define __off_t_defined
#endif
```

### size_t 和 ssize_t

`size_t` 是无符号整型, 用于表示C对象占用的字节数.

 `ssize_t` 基本和 `size_t` 一样区别是`ssize_t`是有符号整型, 一些系统函数如 `read()`  和 `write()` 会使用其作为返回值，可以返回 -1 表示错误。

```c
typedef long unsigned int size_t;
typedef long int ssize_t;
```

### 整型的范围与长度

```c
// 整数的字符串形式长度
#define NGX_INT32_LEN   (sizeof("-2147483648") - 1)
#define NGX_INT64_LEN   (sizeof("-9223372036854775808") - 1)

// 指针长度是4字节，32位cpu
#if (NGX_PTR_SIZE == 4)
#define NGX_INT_T_LEN   NGX_INT32_LEN
#define NGX_MAX_INT_T_VALUE  2147483647

#else
// 指针长度是8字节，64位cpu
#define NGX_INT_T_LEN   NGX_INT64_LEN
#define NGX_MAX_INT_T_VALUE  9223372036854775807
#endif
```
