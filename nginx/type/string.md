# Nginx 中的字符串

> [NGINX 开发指南 - OSCHINA](https://www.oschina.net/translate/nginx-development-guide)

## 字符串的结构体

nginx 使用无符号的char类型指针来表示C字符串 `u_char *`。

```c
typedef struct {
    size_t      len;    //字符串的长度
    u_char     *data;   //字符串的起始地址，注意是u_char，不是char
} ngx_str_t;
```

结构体成员 len 存放字符串的长度，成员 data 指向字符串本身, 他可以是NULL结尾，也可以不是。在大多数情况是不以NULL结尾的。然而，在nginx的某些代码中（例如解析配置的时候），ngx_str_t中的字符串是以NULL结尾的，这种情况会使得字符串比较变得更加简单，也使得使用系统调用的时候更加容易。



## 字符串处理函数

### 内存初始化

```c
#define ngx_memzero(buf, n)       (void) memset(buf, 0, n)
#define ngx_memset(buf, c, n)     (void) memset(buf, c, n)
```



### 内存拷贝

注意 `ngx_cpymem()` 返回的是 `dst + n` 为`dst`最后地址写入地址, 这在需要连续拼接多个字符串的场景下很方便。

```c
/* 原始内存拷贝, 返回值为第一个参数 dest */
void *memcpy(void *dest, const void *src, size_t n);

/* 内存拷贝, 不需要返回值 */
#define ngx_memcpy(dst, src, n)   (void) memcpy(dst, src, n)

/* 内存拷贝, 返回值为 dst + n */
#define ngx_cpymem(dst, src, n)   (((u_char *) memcpy(dst, src, n)) + (n))
#define ngx_copy                  ngx_cpymem


```

### 内存移动

与上面的内存拷贝类似。

```c
/* 原始内存移动, 返回值为第一个参数 dest */
void *memmove(void *dest, const void *src, size_t n);

/* 内存移动, 不需要返回值 */
#define ngx_memmove(dst, src, n)   (void) memmove(dst, src, n)

/* 内存拷贝, 返回值为 dst + n */
#define ngx_movemem(dst, src, n)   (((u_char *) memmove(dst, src, n)) + (n))


```

'

### 字符查找

增加的 `ngx_strlchr()` 可以在指定的内存区域内查找, 这样没有了`\0` 结尾的限制。 

```c
/* 原始字符查找,返回字符 c 第一次出现的位置 */
char *strchr(const char *s, int c);

/* 与 strchr 相同 */
#define ngx_strchr(s1, c)   strchr((const char *) s1, (int) c)

/* 在指定区域内进行字符查找,返回字符 c 第一次出现的位置  */
static ngx_inline u_char *
ngx_strlchr(u_char *p, u_char *last, u_char c)
{
    while (p < last) {
        if (*p == c) {
            return p;
        }
        p++;
    }
    return NULL;
}
```



### 大小写转化

```c
/* 单字符的小写转换 */
#define ngx_tolower(c)  (u_char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)

/* 单字符的大写转换 */
#define ngx_toupper(c)  (u_char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)

/* 字符串的小写转换 */
void ngx_strlow(u_char *dst, u_char *src, size_t n)
{
    while (n) {
        *dst = ngx_tolower(*src);
        dst++;
        src++;
        n--;
    }
}
```

### 字符串比较

```c
/* 只是宏定义 */
#define ngx_strcmp(s1, s2)  strcmp((const char *) s1, (const char *) s2)
#define ngx_strncmp(s1, s2, n)  strncmp((const char *) s1, (const char *) s2, n)


```

### 字符串长度

增加了 `ngx_strnlen()` 函数, 与其他带 n 函数意义一致.

```c
#define ngx_strlen(s)       strlen((const char *) s)

size_t ngx_strnlen(u_char *p, size_t n)
{
    size_t  i;
    for (i = 0; i < n; i++) {
        if (p[i] == '\0') {
            return i;
        }
    }
    return n;
}


```


