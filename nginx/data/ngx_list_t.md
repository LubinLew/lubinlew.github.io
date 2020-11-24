# 单向链表 - ngx_list_t

nginx的单向链表设计和[ngx_array_t](nginx/data/ngx_array_t.md)很相似。

## 数据结构定义

### 链表元素 - ngx_list_part_t

```c

typedef struct ngx_list_part_s  ngx_list_part_t;

/* 链表(内存元素管理)结构
 * 该结构管理了ngx_list_t.nalloc个内存元素;所有的内存元素在内存中是连续的,与数组相同
 * 每个内存元素的长度不能超过 ngx_list_t.size;超过了则破环了链表结构(存下一个内存元素时就破环了上一个)
 * 当成员nelts等于ngx_list_t.nalloc时,表示当前ngx_list_part_t管理的内存元素耗尽,需要再申请一个 ngx_list_part_t)
*/
struct ngx_list_part_s {
    void             *elts; //该链表管理的内存的首地址,通过 elts + ((n-1) * ngx_list_t.size)即可计算第n个内存元素的首地址
    ngx_uint_t        nelts;//该链表已经使用了多少个内存元素, 最大值为 ngx_list_t.nalloc
    ngx_list_part_t  *next; //下一个链表元素 ngx_list_part_t 的地址
};

```


