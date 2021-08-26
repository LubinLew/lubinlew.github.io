

### struct sockaddr_nl 结构体

结构体 `sockaddr_nl`  描述了一个(用户空间或内核) 的 netlink 客户端。它可以是单播(unicast) 或发送到netlink多播(multicast)组(nl_group不为0)。

```c
struct sockaddr_nl {
    sa_family_t     nl_family;  /* AF_NETLINK */
    unsigned short  nl_pad;     /* Zero */
    pid_t           nl_pid;     /* Port ID */
    __u32           nl_groups;  /* Multicast groups mask */
};
```

- `nl_pid` 是 netlink 套接字的单播地址, 如果目的地在内核中,则值为0。对于一个用户空间的进程，该成员的值通常为拥有目的socket的进程的PID，注意 `nl_pid` 成员指定的是 netlink 套接字而不是进程。如果一个进程拥有多个 netlink 套接字，则 nl_pid 最多只能等于一个套接字的进程 PID。有两种方法可以将 nl_pid 分配给 netlink 套接字。如果 应用程序在调用bind(2)之前设置nl_pid，那么应用程序需要确保 nl_pid 是唯一的。如果 应用程序将它设置为 0，则内核负责分配它。 内核将进程 PID 分配给第一个 netlink 套接字,并且为该进程随后创建的 netlink 套接字分配一个唯一的 nl_pid。

- `nl_groups` 是一个位掩码，每一位代表一个 netlink 组号。每个 netlink 系列都有一组 32 个多播组。当在套接字上调用 bind(2) 时，应将 sockaddr_nl 中的 nl_groups 字段设置为它希望侦听的组的位掩码。此字段的默认值为零，这意味着不会接收多播。套接字可以通过将 nl_groups 设置为它在调用 sendmsg(2) 或执行 connect(2) 时希望发送到的组的位掩码来将消息多播到任何多播组。只有有效 UID 为 0 或 CAP_NET_ADMIN 功能的进程才能发送或侦听 netlink 多播组。从 Linux 2.6.13 开始，消息不能广播到多个组。对为多播组接收的消息的任何回复都应发送回发送 PID 和多播组。某些 Linux 内核子系统可能还允许其他用户发送和/或接收消息。在 Linux 3.0 中，NETLINK_KOBJECT_UEVENT、NETLINK_GENERIC、NETLINK_ROUTE 和 NETLINK_SELINUX 组允许其他用户接收消息。没有组允许其他用户发送消息。
