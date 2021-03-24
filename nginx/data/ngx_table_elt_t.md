# 键值对 - ngx_table_elt_t

这个键值对数据结构是为HTTP头部量身定做的。

## 数据结构定义

小写化和哈希都是为了更快的检索头部。

```c
typedef struct {
    ngx_uint_t        hash; //key的hash值
    ngx_str_t         key;
    ngx_str_t         value;
    u_char           *lowcase_key; //小写的key
} ngx_table_elt_t;
```

## 例子

nginx 在请求中解析请求头,  解析后的请求头都在`r->headers_in` 中, 即下面的结构体，该结构体中`ngx_table_elt_t` 成员都是RFC2616中定义的标准HTTP头部, 所有的头部信息都能在 第一个成员`headers` (链表)中找到, 所以想要寻找一个非RFC2616标准的头部可以遍历这个链表。

```c
typedef struct {
    //所有解析过得头部都在 headers 这个链表中,链表的每一个元素都是一个 ngx_table_elt_t 结构
    ngx_list_t                        headers;

    //下面的每个 ngx_table_elt_t 成员都是RFC1616中定义的HTTP头部,他们都指向上面 headers 链表中的对应
    ngx_table_elt_t                  *host;
    ngx_table_elt_t                  *connection;
    ngx_table_elt_t                  *if_modified_since;
    ngx_table_elt_t                  *if_unmodified_since;
    ngx_table_elt_t                  *if_match;
    ngx_table_elt_t                  *if_none_match;
    ngx_table_elt_t                  *user_agent;
    ngx_table_elt_t                  *referer;
    ngx_table_elt_t                  *content_length;
    ngx_table_elt_t                  *content_range;
    ngx_table_elt_t                  *content_type;

    ngx_table_elt_t                  *range;

    /*....省略其他.... */

} ngx_http_headers_in_t;
```

链表的遍历可以参考 [这里](nginx/data/ngx_list_t.md)。

```c
ngx_str_t *
ngx_get_spec_headers(u_char *header_key, ngx_http_request_t *r)
{
    ngx_int_t  i;

    ngx_list_part_t *part = &r->headers_in.headers.part;
    ngx_table_elt_t *header = part->elts;

    /* 遍历链表 */
    for (i = 0; /* void */; i++) {
        if (i >= part->nelts) { /* 到达当前节点的尾部 */
            if (part->next == NULL) {/* 没有下一个节点,就结束 */
                break;
            }

            /* 指向下一个节点 */
            part = part->next;
            header = part->elts;
            i = 0; 
        }

        /* 跳过非法的头部 */
        if (header[i].hash == 0) {
            continue;
        }

        if (0 == ngx_strncasecmp(header[i].key.data, header_key, header[i].key.)) {
            return &header[i].value;
        }
    }

    return NULL;
}
```




