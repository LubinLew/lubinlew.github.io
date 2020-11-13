# Nginx 动态数组 ngx_array_t

`动态数组`在这里指的是: 数组可扩容, 数组元素的大小自定义。
数组中各元素在内存中是连续的，也就是说数组的存储区域是一块连续的内存。
动态扩容的原理是数组内存储区后方的内存可用则直接申请, 
否则会新申请一块更大的内存, 将原来的元素复制到新的内存上, 而原来的内存区则浪费掉了.
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

```c
/** 初始化动态数组
 *  参数 array 不能为空(用户自己申请 ngx_array_t 结构内存)
 *  参数 pool  为后续出现扩容时,申请内存的内存池
 *  参数 n     数组元素的个数
 *  参数 size  每个元素占用内存的大小
 *  返回值 NGX_OK/NGX_ERROR
 */
 
ngx_int_t ngx_array_init(ngx_array_t *array, ngx_pool_t *pool, ngx_uint_t n, size_t size);

/* 创建并初始化动态数组
 * 参数 p     申请 ngx_array_t 数据结构内存用的内存池,同时也是后续出现扩容时,申请内存的内存池
 * 参数 n     数组元素的个数
 * 参数 size  每个元素占用内存的大小
 * 返回值 动态数组指针
 * 注意: 其内部调用 ngx_array_init() 完成初始化,这个函数就比 ngx_array_init() 多了 ngx_array_t 的内存申请
 */
ngx_array_t *ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size);

/* 销毁动态数组 */
void ngx_array_destroy(ngx_array_t *a);

/** 向数组中添加一个元素,返回这个新元素的地址，
 *  如果数组空间已经用完，数组会自动扩充空间
 */
void *ngx_array_push(ngx_array_t *a);

/*向数组中添加n个元素，返回这n个元素中第一个元素的地址*/
void *ngx_array_push_n(ngx_array_t *a, ngx_uint_t n);

```


