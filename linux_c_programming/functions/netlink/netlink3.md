# NETLINK 3

## NAME

Netlink 相关宏

## SYNOPSIS

```c
#include <asm/types.h>
#include <linux/netlink.h>

int NLMSG_ALIGN(size_t len);
int NLMSG_LENGTH(size_t len);
int NLMSG_SPACE(size_t len);
void *NLMSG_DATA(struct nlmsghdr *nlh);
struct nlmsghdr *NLMSG_NEXT(struct nlmsghdr *nlh, int len);
int NLMSG_OK(struct nlmsghdr *nlh, int len);
int NLMSG_PAYLOAD(struct nlmsghdr *nlh, int len);
```

## DESCRIPTION

`<linux/netlink.h>`  定义了几个标准宏来访问或创建一个 netlink 数据报(datagram)。 应仅使用这些宏来访问传入和传出 netlink 套接字的缓冲区。

### NLMSG_ALIGN()

将 netlink 消息的长度向上舍入以正确对齐.

### NLMSG_LENGTH()

给定负载长度 len，该宏返回对齐的长度以存储在 nlmsghdr 的 nlmsg_len 字段中.

### NLMSG_SPACE()

返回负载为 len 的 netlink 消息将占用的字节数.

### NLMSG_DATA()

返回指向与传递的 nlmsghdr 关联的有效负载的指针.

### NLMSG_NEXT()

获取多部分消息中的下一个 nlmsghdr。 调用者必须检查当前 nlmsghdr 是否没有设置 NLMSG_DONE 此函数在结束时不会返回 NULL。 len 参数是一个左值，包含消息缓冲区的剩余长度。 这个宏将它递减消息头的长度.

### NLMSG_OK()

如果 netlink 消息没有被截断并且是适合解析的形式，则返回 true.

### NLMSG_PAYLOAD()

返回与 nlmsghdr 关联的有效负载的长度.

## CONFORMING TO

这些宏是非标准的 Linux 扩展。

## NOTES

通过 libnetlink 使用 netlink 通常比通过低级内核接口更好。



## SEE ALSO

libnetlink(3), netlink(7)
