# Nginx 动态数组 ngx_array_t

`动态数组`在这里指的是: 数组可扩容, 数组元素占用内存的大小是自定义的。
数组中各元素在内存中是连续的，也就是说数组的存储区域是一块连续的内存。
动态扩容的原理是数组内存储区后方的内存可用则直接申请, 
否则会新申请一块更大的(原来的2倍)内存, 将原来的元素复制到新的内存上, 而原来的内存区则浪费掉了.
所以动态扩容的性能可能是很差的, 所以动态数组适合数组元素个数不会有很大变化的情况. 

## 数据结构定义

```c
typedef struct {
    void        *elts;   /* 数组存储内存的首地址 */
    ngx_uint_t   nelts;  /* 数组中已使用的元素个数 */
    size_t       size;   /* 单个数组元素占用内存的大小 */
    ngx_uint_t   nalloc; /* 当前数组中能够容纳元素的总个数 */
    ngx_pool_t  *pool;   /* 内存池对象,动态数组扩容时使用的内存池 */
} ngx_array_t;
```

## API接口定义

### 初始化动态数组 - ngx_array_init

```c
/** 初始化动态数组
 *  参数 array 不能为空(用户自己申请 ngx_array_t 结构内存)
 *  参数 pool  为后续出现扩容时,申请内存的内存池
 *  参数 n     数组元素的个数
 *  参数 size  每个数组元素占用内存的大小
 *  返回值 NGX_OK/NGX_ERROR
 */

static ngx_inline ngx_int_t
ngx_array_init(ngx_array_t *array, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    array->nelts = 0;
    array->size = size;
    array->nalloc = n;
    array->pool = pool;

    array->elts = ngx_palloc(pool, n * size);
    if (array->elts == NULL) {
        return NGX_ERROR;
    }

    return NGX_OK;
}
```

### 动态数组创建 - ngx_array_create

```c
/* 创建并初始化动态数组
 * 参数 p     申请 ngx_array_t 数据结构内存用的内存池,同时也是后续出现扩容时,申请内存的内存池
 * 参数 n     数组元素的个数
 * 参数 size  每个元素占用内存的大小
 * 返回值 动态数组指针
 * 注意: 其内部调用 ngx_array_init() 完成初始化,这个函数就比 ngx_array_init() 多了 ngx_array_t 的内存申请,
 *      该函数 ngx_array_t 结构 和 数组内存 都是从一个pool中申请的.
 */
ngx_array_t *
ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size)
{
    ngx_array_t *a;

   /* 为 ngx_array_t 申请内存 */
    a = ngx_palloc(p, sizeof(ngx_array_t));
    if (a == NULL) {
        return NULL;
    }

    /* 为数组元素申请内存,并初始化 ngx_array_t 结构  */
    if (ngx_array_init(a, p, n, size) != NGX_OK) {
        return NULL;
    }

    return a;
}
```

### 释放动态数组 - ngx_array_destroy

```c
void
ngx_array_destroy(ngx_array_t *a)
{
    ngx_pool_t  *p;

    p = a->pool;

    /* 动态数组是内存池的最后一个申请者, 
     * 那么直接去掉动态数组的长度就完成了动态数组元素使用内存的释放
     * 这样做可以提高内存的利用率
     */
    if ((u_char *) a->elts + a->size * a->nalloc == p->d.last) {
        p->d.last -= a->size * a->nalloc;
    }

    /* 动态数组的管理结构 ngx_array_t 的内存也是从这个pool中申请的,
     * 并且其内存与数组元素内存是连续的(即使用ngx_array_create的情况),
     * 那么直接去掉动态数组管理结构的长度就完成了内存的释放
     */
    if ((u_char *) a + sizeof(ngx_array_t) == p->d.last) {
        p->d.last = (u_char *) a;
    }
}
```

### 获取动态数组一个新元素的地址 - ngx_array_push

```c
void *
ngx_array_push(ngx_array_t *a)
{
    void        *elt, *new;
    size_t       size;
    ngx_pool_t  *p;

    /* 当前数组已满 */
    if (a->nelts == a->nalloc) {
        size = a->size * a->nalloc;
        p = a->pool;

        /* 动态数组的使用的内存的最后位置与内存池未分配内存的位置相同,
         * 即该内存池上分配了动态数组后,没有再分配其他内存,
         * 这样, 内存池就可以继续为动态数组分配内存
         */
        if ((u_char *) a->elts + size == p->d.last
            && p->d.last + a->size <= p->d.end)
        {
            p->d.last += a->size;
            a->nalloc++;

        } else { /* 新申请一个2倍于原大小的内存块, 并将旧数组拷贝到新的内存中 */
            /* allocate a new array */
            new = ngx_palloc(p, 2 * size);
            if (new == NULL) {
                return NULL;
            }

            ngx_memcpy(new, a->elts, size);
            a->elts = new;
            a->nalloc *= 2;
        }
    }

    elt = (u_char *) a->elts + a->size * a->nelts;
    a->nelts++;

    return elt;
}
```

### 获取动态数组n个新元素的地址 - ngx_array_push_n

```c
/* 原理同 ngx_array_push 相同 */
void *
ngx_array_push_n(ngx_array_t *a, ngx_uint_t n)
{
    void        *elt, *new;
    size_t       size;
    ngx_uint_t   nalloc;
    ngx_pool_t  *p;

    size = n * a->size;

    if (a->nelts + n > a->nalloc) {
        p = a->pool;

        if ((u_char *) a->elts + a->size * a->nalloc == p->d.last
            && p->d.last + size <= p->d.end)
        {
            p->d.last += size;
            a->nalloc += n;

        } else {

            nalloc = 2 * ((n >= a->nalloc) ? n : a->nalloc);

            new = ngx_palloc(p, nalloc * a->size);
            if (new == NULL) {
                return NULL;
            }

            ngx_memcpy(new, a->elts, a->nelts * a->size);
            a->elts = new;
            a->nalloc = nalloc;
        }
    }

    elt = (u_char *) a->elts + a->size * a->nelts;
    a->nelts += n;

    return elt;
}
```
