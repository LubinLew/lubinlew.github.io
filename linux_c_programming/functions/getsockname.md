# getsockname

NAME
       getsockname - get socket name

## SYNOPSIS

```c
#include <sys/socket.h>

int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

## DESCRIPTION

`getsockname()` 函数返回指定 socket 文件描述符 `sockfd` 绑定的地址。返回的数组会写入参数 `addr` 指向的地址。注意：参数 `addrlen` 指向的地址其值必须初始化为 参数 `addr` 指向buf的大小(字节数)。函数返回时，该参数返回函数实际写入的字节数。如果因为提供的buffer太小，导致返回的地址不完整，那么 `addrlen` 返回的值要大于传入的值。



## RETURN VALUE

成功，函数返回0；失败，函数返回 -1，并设置对应的 `errno`。

| errno    | 描述                           |
| -------- | ---------------------------- |
| EBADF    | 参数 `sockfd` 是无效的文件描述符        |
| EFAULT   | 参数 `addr` 无效(未指向本进程内有效的地址空间) |
| EINVAL   | 参数 `addrlen` 无效, 例如取值后是负值等   |
| ENOBUFS  | 系统资源不足,无法完成此函数的操作            |
| ENOTSOCK | 参数 `sockfd` 绑定的是文件,而不是socket |



## CONFORMING TO

 SVr4, 4.4BSD (the getsockname() function call appeared in 4.2BSD), POSIX.1-2001.



## NOTES

函数的第三个参数类型 `socklen_t`实际上就是 `int`。历史原因见 [HERE](https://baike.baidu.com/item/socklen_t/9602948?fr=aladdin)。



## EXAMPLE


