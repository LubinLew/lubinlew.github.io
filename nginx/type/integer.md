# Nginx 中的整型



## 基本类型定义

关于 `intptr_t` 和 `uintptr_t` [HERE](linux_c_programming/base/integer_range.md)

```c
typedef intptr_t        ngx_int_t;      //有符号整数
typedef uintptr_t       ngx_uint_t;     //无符号整数
typedef intptr_t        ngx_flag_t;     //相当于bool，标志量用
```

## 整型的范围与长度

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
