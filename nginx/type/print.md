# Nginx 中的打印

## 打印函数

```c
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
int vsprintf(char *str, const char *format, va_list ap);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);

u_char * ngx_snprintf(u_char *buf, size_t max, const char *fmt, ...);
u_char * ngx_slprintf(u_char *buf, u_char *last, const char *fmt, ...);
u_char *ngx_vslprintf(u_char *buf, u_char *last, const char *fmt, va_list args);
#define ngx_vsnprintf(buf, max, fmt, args)  ngx_vslprintf(buf, buf + (max), fmt, args)
```

## 打印符号

| 打印    | 数据类型                   | 备注         |
| ----- | ---------------------- | ---------- |
| `%V`  | ngx_str_t *            | 字符串        |
| `%v`  | ngx_variable_value_t * | 变量         |
| `%s`  | u_char *               | 字符串,null结尾 |
| `%O`  | off_t                  |            |
| `%P`  | ngx_pid_t              |            |
| `%T`  | time_t                 |            |
| `%M`  | ngx_msec_t             |            |
| `%uz` | size_t                 |            |
| `%z`  | ssize_t                |            |
| `%i`  | ngx_int_t              |            |
| `%ui` | ngx_uint_t             |            |
| `%d`  | int                    |            |
| `%ud` | u_int                  |            |
| `%l`  | long                   |            |
| `%ul` | u_long                 |            |
| `%D`  | int32_t                |            |
| `%uD` | uint32_t               |            |
| `%L`  | int64_t                |            |
| `%uL` | uint64_t               |            |
| `%A`  | ngx_atomic_int_t       |            |
| `%uA` | ngx_atomic_uint_t      |            |
| `%f`  | float                  | 浮点型        |
| `%r`  | rlim_t                 |            |
| `%p`  | void *                 | 指针地址       |
| `%c`  | int & 0xff             | 字符         |
| `%Z`  | `'\0'`                 | 添加null结尾符  |
| `%N`  | LF                     | 添加换行符      |
| `%%`  | %                      | %号转义       |
