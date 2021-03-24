# 缓冲区链表 - ngx_chain_t

`ngx_chain_t` 是与`ngx_buf_t`配置使用的链表数据结构。成员`buf` 指向当前的 `ngx_buf_t` 缓冲区, 成员`next`用来指向下一个`ngx_chain_t`,最后一个`ngx_chain_t`的成员`next`指向 NULL。

```c
struct ngx_chain_s {
    ngx_buf_t    *buf;
    ngx_chain_t  *next;
};

```

向用户发送HTTP包体时，就要传入`ngx_chain_t` 链表对象，注意，如果是最后一个`ngx_chain_t`,那么必须将成员`next`置为NULL,否则永远不会发送成功,而这个请求将一直不会结束。
