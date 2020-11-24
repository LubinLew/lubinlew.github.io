# offsetof

## NAME

offsetof - 计算一个结构体成员相对于结构体首地址的字节偏移量

## SYNOPSIS

```c
#include <stddef.h>

size_t offsetof(type, member);

/* 宏定义 */
#define offsetof(TYPE, MEMBER) ((size_t)&((TYPE *)0)->MEMBER)
```

## DESCRIPTION

`offsetof()` 是一个宏定义, 他会返回一个结构体成员相对于结构体开头的字节偏移量。
编译器会对结构体进行对齐优化(结构体对齐), 所以结构体成员之间可能被编译器插入了填充(padding)字节；这就导致，结构体成员对于首地址的字节偏移量不是前面几个成员占用的字节数之和。如果结构体成员是位域([bit field](https://en.wikipedia.org/wiki/Bit_field)), 那么编译器会报错。

**宏定义的原理是**：将地址0强转为TYPE类型的结构体指针，那么 MEMBER 的地址(减0,因为结构体首地址为0),就是偏移量。

## RETURN VALUE

`size_t` 类型, 表示字节数

## CONFORMING TO

C89, C99, POSIX.1-2001.

## EXAMPLE

```c
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int
main(void)
{
    struct s {
        int i;
        char c;
        double d;
        char a[];
    };

    /* Output is compiler dependent */
    printf("offsets: i=%ld; c=%ld; d=%ld a=%ld\n",
            (long) offsetof(struct s, i),
            (long) offsetof(struct s, c),
            (long) offsetof(struct s, d),
            (long) offsetof(struct s, a));
    printf("sizeof(struct s)=%ld\n", (long) sizeof(struct s));

    exit(EXIT_SUCCESS);
}
```

## REMARK

在C++中,需要特别小心, 只有符合[POD](https://en.wikipedia.org/wiki/Passive_data_structure)(Plain Old Data)的结构体才能使用`offsetof()`,否则会得到错误的结果。