# 

# 整型



## 取值范围

### 基础知识

1.正数在计算机内存中以原码的形式存储，但是负数以补码的形式存储在内存中，-1在内存中是以所有位为1的形式存储的，即下面这种二进制形式(1111,1111,1111,1111,1111,1111,1111,1111)(1111,1111,1111,1111,1111,1111,1111,1111)2进制补码​ ，**负数的补码是在原码的基础上除符号位外其余位取反后+1**。

‌

2.有符号整型进行移位操作结果是与编译器有关的,简而言之,不要对有符号整型数进行位移操作，否则代码不具有移植性.

‌

3.各种数据类型的字节数 

| 数据类型      | 32位   | 64位   |
| --------- | ----- | ----- |
| char      | 1     | 1     |
| short     | 2     | 2     |
| int       | 4     | 4     |
| **long**  | **4** | **8** |
| long long | 8     | 8     |
| float     | 4     | 4     |
| double    | 8     | 8     |

## 

### int 的取值范围‌

int通常为32位，但是在一些嵌入式中有16的情况，这里假定就是32位的情况。int为有符号类型，所以第1位为正负标记位，其他31位为数据位，所以int的取值范围为 [−2<sup>31</sup> ~ 2<sup>31</sup>−1 ]，‌最小值的情况是 32位均为1， 但这是原码，计算补码为 (1000,0000,0000,0000,0000,0000,0000,0000)(1000,0000,0000,0000,0000,0000,0000,0000)2进制补码​, 所以最小值为 1左移31位。

最小值的情况是 32位均为1， 但这是原码，计算补码为 (1000,0000,0000,0000,0000,0000,0000,0000)(1000,0000,0000,0000,0000,0000,0000,0000)2进制补码​, 所以最小值为 1左移31位。‌

最大值情况是首位为0其他31位为1的情况，这正好与上面最小值补码的情况相反。

```c
#define INT_MIN (1U<<31)
#define INT_MAX (～INT_MIN)
```

经过上面的分析，我们还可以得倒以下的结论

```c
INT_MAX == INT_MIN - 1; //true
INT_MIN == INT_MAX + 1; //true
```

### unsigned int 取值范围‌

这个非常好理解，将0U全部位取反，即可得到最大值，而且无论int是几个字节的，这个方法都是可行的，即该方法是可移植的。

```c
#define UINT_MAX (~0U)
#define UINT_MIN (0)
```

### long 和 unsigned long‌

与上面的 int unsigned int 相同，只是在64位机器上偏移位数不同。



## 可移植的整型编程方法

在早年32位向64位移植的时候, 发现很多32位程序中使用int类型保存或者传递指针地址。这在64位机器里是行不通的，因为指针是64位的，32位的int根本不够用，只能将 int 改为 long，这样程序在32位和64位里都能正常运行。

为了保证程序的可移植性，C99标准(ISO C99:7.18 Integer types)增加了`<stdint.h>` 这个头文件。具体文件位置为 `/usr/include/stdint.h` 。



### 整型

直接指定想要使用整型的位数，这样就不用担心移植的问题。

| 数据位数 | 有符号     | 无符号      |
| ---- | ------- | -------- |
| 8位   | int8_t  | uint8_t  |
| 16位  | int16_t | uint16_t |
| 32位  | int32_t | uint32_t |
| 64位  | int64_t | uint64_t |

```c
#ifndef __int8_t_defined
# define __int8_t_defined
typedef signed char		int8_t;
typedef short int		int16_t;
typedef int			    int32_t;
# if __WORDSIZE == 64
typedef long int		int64_t;
# else
__extension__
typedef long long int	int64_t;
# endif
#endif

/* Unsigned.  */
typedef unsigned char		    uint8_t;
typedef unsigned short int	    uint16_t;
#ifndef __uint32_t_defined
typedef unsigned int		    uint32_t;
# define __uint32_t_defined
#endif
#if __WORDSIZE == 64
typedef unsigned long int	     uint64_t;
#else
__extension__
typedef unsigned long long int	 uint64_t;
#endif



```

### 指针

为了整型可以保存指针地址, 特意定义了 2 个 宏 `intptr_t` 和 `uintptr_t`。这2个整型的位数与系统位数即指针位数相同，这样就可以保存指针地址，并且不用担心移植的问题。

代码

```c
/* Types for `void *' pointers.  */
#if __WORDSIZE == 64
# ifndef __intptr_t_defined
typedef long int		    intptr_t;
#  define __intptr_t_defined
# endif
typedef unsigned long int	uintptr_t;
#else
# ifndef __intptr_t_defined
typedef int			        intptr_t;
#  define __intptr_t_defined
# endif
typedef unsigned int		uintptr_t;
#endif



```
