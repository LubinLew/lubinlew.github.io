# NETLINK 7

## NAME

netlink - communication between kernel and user space (AF_NETLINK)

## SYNOPSIS

```c
#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>

netlink_socket = socket(AF_NETLINK, socket_type, netlink_family);
```

## DESCRIPTION

Netlink 用于在内核和用户空间进程之间传输信息。

它由一个用于用户空间进程的基于套接字的标准接口和一个用于内核模块的内部内核 API 组成。本手册页中未记录内部内核接口。 还有一个通过 netlink 字符设备的过时的 netlink 接口； 此接口未在此处记录，仅用于向后兼容。

Netlink 是一种面向数据报的服务。`SOCK_RAW 和` `SOCK_DGRAM` 都是 `socket_type` 的有效值。 但是，netlink 协议不区分数据报和原始套接字。

`netlink_family` 选择要与之通信的内核模块或 netlink 组。 当前分配的 netlink 系列是：

| `netlink_family`参数取值   | 内核版本                    | 说明                                                                                                                                                                                                                                   |
| ---------------------- | ----------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| NETLINK_ROUTE          | 2.6.13 ~ 2.16.17        | Receives routing and link updates and may be used to modify the routing tables (both IPv4 and IPv6), IP addresses, link parameters, neighbor setups, queueing disciplines, traffic classes and packet classifiers (see rtnetlink(7)) |
| NETLINK_W1             | -                       | Messages from 1-wire subsystem                                                                                                                                                                                                       |
| NETLINK_USERSOCK       | -                       | Reserved for user-mode socket protocols                                                                                                                                                                                              |
| NETLINK_FIREWALL       | <=3.4<br>removed in 3.5 | Transport  IPv4  packets  from  netfilter to user space.  Used by ip_queue kernel module.  After a long period of being declared obsolete (in favor of the more advanced                                                             |
| NETLINK_INET_DIAG      | >= 2.6.14               | Query information about sockets of various protocol families from the kernel (see sock_diag(7)).                                                                                                                                     |
| NETLINK_NFLOG          | <= 3.16                 | Netfilter/iptables ULOG                                                                                                                                                                                                              |
| NETLINK_XFRM           | -                       | IPsec                                                                                                                                                                                                                                |
| NETLINK_SELINUX        | >= 2.6.4                | SELinux event notifications                                                                                                                                                                                                          |
| NETLINK_ISCSI          | >= 2.6.15               | Open-iSCSI                                                                                                                                                                                                                           |
| NETLINK_AUDIT          | >= 2.6.6                | Auditing                                                                                                                                                                                                                             |
| NETLINK_FIB_LOOKUP     | >= 2.6.13               | Access to FIB lookup from user space                                                                                                                                                                                                 |
| NETLINK_CONNECTOR      | >= 2.6.14               | Kernel connector.  https://www.kernel.org/doc/Documentation/connector/connector.txt                                                                                                                                                  |
| NETLINK_NETFILTER      | >= 2.6.14               | Netfilter subsystem                                                                                                                                                                                                                  |
| NETLINK_SCSITRANSPORT  | >= 2.6.19               | SCSI Transports.                                                                                                                                                                                                                     |
| NETLINK_RDMA           | >= 3.0                  | Infiniband RDMA                                                                                                                                                                                                                      |
| NETLINK_IP6_FW         | <= 3.4                  | Transport IPv6 packets from netfilter to user space.  Used by `ip6_queue` kernel module                                                                                                                                              |
| NETLINK_DNRTMSG        | -                       | DECnet routing messages                                                                                                                                                                                                              |
| NETLINK_KOBJECT_UEVENT | >= 2.6.10               | Kernel messages to user space.                                                                                                                                                                                                       |
| NETLINK_GENERIC        | >= 2.6.15               | Generic netlink family for simplified netlink usage                                                                                                                                                                                  |
| NETLINK_CRYPTO         | >= 3.2                  | Netlink 接口，用于请求有关向内核加密 API 注册的密码的信息，并允许配置内核加密 API                                                                                                                                                                                    |

Netlink 消息由一个字节流和一个或多个 `nlmsghdr` 标头和相关的有效载荷组成。只能使用标准 NLMSG_* 宏访问字节流。在多部分消息（多个 `nlmsghdr` 标头与一个字节流中的关联有效载荷）中，第一个和所有后续标头设置了 `NLM_F_MULTI` 标志，除了最后一个标头的类型为 `NLMSG_DONE`。在每个 `nlmsghdr` 之后，有效载荷跟随。

```c
struct nlmsghdr {
    __u32 nlmsg_len;    /* 包括标题在内的消息长度 */
    __u16 nlmsg_type;   /* 消息内容类型 */
    __u16 nlmsg_flags;  /* 附加标志 */
    __u32 nlmsg_seq;    /* 序列号 */
    __u32 nlmsg_pid;    /* 发件人端口号 */
};
```

`nlmsg_type` 可以是标准消息类型之一：

- `NLMSG_NOOP` 消息将被忽略;

- `NLMSG_DONE` 消息终止多部分消息;

- `NLMSG_ERROR` 消息发出错误信号并且有效负载包含 `nlmsgerr` 结构。

```c
struct nlmsgerr {
    int error;        /* Negative errno or 0 for acknowledgements */
    struct nlmsghdr msg;  /* Message header that caused the error */
};
```

netlink 系列通常会指定更多的消息类型，请参阅相应的手册页，例如，NETLINK_ROUTE 的 rtnetlink(7)。

标准的 `nlmsg_flags` 取值范围

| 标志位取值                      | 类型           | 说明                                                                                                                               |
| -------------------------- | ------------ | -------------------------------------------------------------------------------------------------------------------------------- |
| NLM_F_REQUEST              | 标准           | Must be set on all request messages                                                                                              |
| NLM_F_MULTI                | 标准           | The  message  is  part  of  a multipart message terminated by NLMSG_DONE.                                                        |
| NLM_F_ACK                  | 标准           | Request for an acknowledgment on success                                                                                         |
| NLM_F_ECHO                 | 标准           | Echo this request                                                                                                                |
| NLM_F_ROOT                 | GET requests | Return the complete table instead of a single entry                                                                              |
| NLM_F_MATCH                | GET requests | Return all entries matching criteria passed in  message  content.  Not implemented yet                                           |
| NLM_F_ATOMIC               | GET requests | Return an atomic snapshot of the table.<br>Note that NLM_F_ATOMIC requires the CAP_NET_ADMIN capability or an effective UID of 0 |
| NLM_F_DUMP                 | GET requests | Convenience macro; equivalent to                                                                                                 |
| (NLM_F_ROOT\|NLM_F_MATCH). |              |                                                                                                                                  |
| NLM_F_REPLACE              | NEW requests | Replace existing matching object                                                                                                 |
| NLM_F_EXCL                 | NEW requests | Don't replace if the object already exists                                                                                       |
| NLM_F_CREATE               | NEW requests | Create object if it doesn't already exist                                                                                        |
| NLM_F_APPEND               | NEW requests | Add to the end of the object list                                                                                                |

`nlmsg_seq` 和 `nlmsg_pid` 用于跟踪消息。 `nlmsg_pid` 显示消息的来源。请注意，如果消息源自 netlink 套接字，则 `nlmsg_pid` 和进程的 PID 之间没有 1:1 的关系。有关详细信息，请参阅地址格式部分。`nlmsg_seq` 和 `nlmsg_pid` 都对 netlink 核心不透明的。

Netlink 不是一个可靠的协议。它会尽最大努力将消息传递到其目的地，但可能会在出现内存不足或其他错误时丢弃消息。为了可靠传输，发送方可以通过设置 NLM_F_ACK 标志来请求接收方的确认。确认是错误字段设置为 0 的 `NLMSG_ERROR` 数据包。应用程序必须自己为接收到的消息生成确认。 内核尝试为每个失败的数据包发送 `NLMSG_ERROR` 消息。 用户进程也应该遵循这个约定。

然而，从内核到用户的可靠传输在任何情况下都是不可能的。如果套接字缓冲区已满，内核将无法发送 netlink 消息：该消息将被丢弃，内核和用户空间进程将不再具有相同的内核状态视图。由应用程序检测何时发生这种情况（通过 recvmsg(2) 返回的 ENOBUFS 错误）并重新同步。

## 地址格式

`sockaddr_nl` 结构描述了用户空间或内核中的 netlink 客户端。 `sockaddr_nl` 可以是单播（仅发送到一个对等方）或发送到 netlink 多播组（`nl_groups` 不等于 0）。

```c
struct sockaddr_nl {
    sa_family_t     nl_family;  /* AF_NETLINK */
    unsigned short  nl_pad;     /* Zero */
    pid_t           nl_pid;     /* Port ID */
    __u32           nl_groups;  /* Multicast groups mask */
};
```

`nl_pid` 是 netlink 套接字的单播地址。 如果目标在内核中，则它始终为 0。 对于用户空间进程，`nl_pid` 通常是拥有目标套接字的进程的 PID。但是，`nl_pid` 标识的是 netlink 套接字，而不是进程。 如果一个进程拥有多个 netlink 套接字，则 `nl_pid` 只能等于最多一个套接字的进程 ID。 有两种方法可以将 `nl_pid` 分配给 netlink 套接字。 如果应用程序在调用 bind(2) 之前设置了 `nl_pid`，则由应用程序来确保 `nl_pid` 是唯一的。 如果应用程序将其设置为 0，则内核会负责分配它。 内核将进程 ID 分配给该进程打开的第一个 netlink 套接字，并为该进程随后创建的每个 netlink 套接字分配一个唯一的 `nl_pid`。

`nl_groups` 是一个位掩码，每一位代表一个 netlink 组号。每个 netlink 系列都有一组 32 个多播组。当在套接字上调用 bind(2) 时，应将 `sockaddr_nl` 中的 `nl_groups` 字段设置为它希望侦听的组的位掩码。此字段的默认值为零，这意味着不会接收多播。套接字可以通过将 `nl_groups` 设置为它在调用 sendmsg(2) 或执行 connect(2) 时希望发送到的组的位掩码来将消息多播到任何多播组。只有有效 UID 为 0 或 `CAP_NET_ADMIN` 功能的进程才能发送或侦听 netlink 多播组。从 Linux 2.6.13 开始，消息不能广播到多个组。对为多播组接收的消息的任何回复都应发送回发送 PID 和多播组。某些 Linux 内核子系统可能还允许其他用户发送和/或接收消息。在 Linux 3.0 中，`NETLINK_KOBJECT_UEVENT`、`NETLINK_GENERIC`、`NETLINK_ROUTE` 和 `NETLINK_SELINUX` 组允许其他用户接收消息。没有组允许其他用户发送消息。

## Socket 选项

要设置或获取 netlink 套接字选项，请调用getsockopt(2) 读取或调用setsockopt(2) 写入选项，并将选项级别参数设置为SOL_NETLINK。 除非另有说明，optval 是一个指向 int 的指针。

| 选项                       | 内核        | 说明                                                                                                                                                                                                                                                                                                                |
| ------------------------ | --------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| NETLINK_PKTINFO          | >= 2.6.14 | Enable nl_pktinfo control messages for received packets to get the extended destination group number.                                                                                                                                                                                                             |
| NETLINK_ADD_MEMBERSHIP   | >= 2.6.14 | Join a group specified by optval                                                                                                                                                                                                                                                                                  |
| NETLINK_DROP_MEMBERSHIP  | >= 2.6.14 | Leave a group specified by optval                                                                                                                                                                                                                                                                                 |
| NETLINK_LIST_MEMBERSHIPS | >= 4.2    | Retrieve all groups a socket is a member of. optval is a pointer to __u32 and optlen is the size of the array. The array is filled with the full membership set of the socket, and the required array size is returned in optlen.                                                                                 |
| NETLINK_BROADCAST_ERROR  | >= 2.6.30 | When not set, netlink_broadcast() only reports ESRCH errors and silently ignore NOBUFS errors                                                                                                                                                                                                                     |
| NETLINK_NO_ENOBUFS       | >= 2.6.30 | This flag can be used by unicast and broadcast listeners to avoid receiving ENOBUFS errors                                                                                                                                                                                                                        |
| NETLINK_LISTEN_ALL_NSID  | >= 4.2    | When set, this socket will receive netlink notifications from all network namespaces that have an nsid assigned into the network namespace where the socket has been opened. The nsid is sent to user space via an ancillary data.                                                                                |
| NETLINK_CAP_ACK          | >= 4.2    | The kernel may fail to allocate the necessary room for the acknowledgment message back to user space. This option trims off the payload of the original netlink message. The netlink message header is still included, so the user can guess from the sequence number which message triggered the acknowledgment. |

## VERSIONS

netlink 的套接字接口首先出现在 Linux 2.2。Linux 2.0 支持更原始的基于设备的 netlink 接口（仍可作为兼容性选项使用）,这里不描述这个过时的接口。

## NOTES

通过 `libnetlink` 或 `libnl` 使用 netlink 通常比通过低级内核接口更好。

## EXAMPLE

下面的示例创建一个 `NETLINK_ROUTE` 网络链路套接字，它将侦听 `RTMGRP_LINK`（网络接口创建/删除/启动/关闭事件）和 `RTMGRP_IPV4_IFADDR`（IPv4 地址添加/删除事件）多播组。

```c
struct sockaddr_nl sa;

memset(&sa, 0, sizeof(sa));
sa.nl_family = AF_NETLINK;
sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;

fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
bind(fd, (struct sockaddr *) &sa, sizeof(sa));
```

下一个示例演示如何将 netlink 消息发送到内核 (pid 0)。 请注意，应用程序必须处理消息序列号以便可靠地跟踪确认。

```c
struct nlmsghdr *nh;    /* The nlmsghdr with payload to send */
struct sockaddr_nl sa;
struct iovec iov = { nh, nh->nlmsg_len };
struct msghdr msg;

msg = { &sa, sizeof(sa), &iov, 1, NULL, 0, 0 };
memset(&sa, 0, sizeof(sa));
sa.nl_family = AF_NETLINK;
nh->nlmsg_pid = 0;
nh->nlmsg_seq = ++sequence_number;
/* Request an ack from kernel by setting NLM_F_ACK */
nh->nlmsg_flags |= NLM_F_ACK;

sendmsg(fd, &msg, 0);
```

最后一个例子是关于读取 netlink 消息。

```c
int len;
char buf[8192];     /* 8192 to avoid message truncation on platforms with page size > 4096 */
struct iovec iov = { buf, sizeof(buf) };
struct sockaddr_nl sa;
struct msghdr msg;
struct nlmsghdr *nh;

msg = { &sa, sizeof(sa), &iov, 1, NULL, 0, 0 };
len = recvmsg(fd, &msg, 0);

for (nh = (struct nlmsghdr *) buf; NLMSG_OK (nh, len);
     nh = NLMSG_NEXT (nh, len)) {
    /* The end of multipart message */
    if (nh->nlmsg_type == NLMSG_DONE)
        return;

    if (nh->nlmsg_type == NLMSG_ERROR)
        /* Do some error handling */
    ...

    /* Continue with parsing payload */
    ...
}
```

## SEE ALSO

cmsg(3), netlink(3), capabilities(7), rtnetlink(7), sock_diag(7)

libnetlink provides a higher level interface to [rtnetlink(7)](https://man7.org/linux/man-pages/man7/rtnetlink.7.html)

information about libnetlink [libnetlink(3) - Linux manual page](https://man7.org/linux/man-pages/man3/libnetlink.3.html)

The libnl suite is a collection of libraries providing APIs to netlink protocol based Linux kernel interfaces.

information about libnl [libnl - Netlink Protocol Library Suite](http://www.infradead.org/~tgr/libnl/)

[libnl: Main Page](https://www.infradead.org/~tgr/libnl/doc/api/index.html#main_intro)

[RFC 3549](https://datatracker.ietf.org/doc/html/rfc3549) "Linux Netlink as an IP Services Protocol"