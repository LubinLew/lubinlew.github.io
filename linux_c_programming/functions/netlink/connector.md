# Connector

Kernel connector - 基于 netlink 的用户空间与内核空间易于使用的通信模块。

connector 提供了一种统一的用户态与内核态之间通过 netlink socket 协议层进行通信的标准框架，

它实现了一种虚拟设备，通过这种设备内核可以与用户态程序通过 netlink 进行通信，传输固定格式信息的报文。

cn_proc 驱动使用了 connector 驱动，能够将进程事件报告给用户态程序。它会发送诸如进程 fork、exec、id change (uid，gid，suid，etc 等)，以及exit 事件的 netlink 消息。



从用户空间的角度来看，它非常简单:

```c
    socket();
    bind();
    send();
    recv();
```

But if kernelspace wants to use the full power of such connections, the driver writer must create special sockets, must know about struct sk_buff
handling, etc...  The Connector driver allows any kernelspace agents to use netlink based networking for inter-process communication in a significantly easier way:

但是如果内核空间想要使用这种连接的全部功能，驱动程序编写者必须创建特殊的套接字，必须了解 struct sk_buff 处理等......连接器驱动程序允许任何内核空间代理使用基于 netlink 的网络进行进程间通信 一种更简单的方法

```c
int cn_add_callback(struct cb_id *id, char *name, void (*callback) (struct cn_msg *, struct netlink_skb_parms *));
void cn_netlink_send_multi(struct cn_msg *msg, u16 len, u32 portid, u32 __group, int gfp_mask);
void cn_netlink_send(struct cn_msg *msg, u32 portid, u32 __group, int gfp_mask);

struct cb_id
{
    __u32            idx;
    __u32            val;
};
```

idx and val are unique identifiers which must be registered in the connector.h header for in-kernel usage.  void (*callback) (void *) is a
callback function which will be called when a message with above idx.val is received by the connector core.  The argument for that function must be dereferenced to struct cn_msg *.

```c
struct cn_msg
{
    struct cb_id        id;

    __u32            seq;
    __u32            ack;

    __u32            len;        /* Length of the following data */
    __u8            data[0];
};
```

### Connector interfaces.

```c
int cn_add_callback(struct cb_id *id, char *name, void (*callback) (struct cn_msg *, struct netlink_skb_parms *));

 Registers new callback with connector core.

 struct cb_id *id        - unique connector's user identifier.
                  It must be registered in connector.h for legal in-kernel users.
 char *name            - connector's callback symbolic name.
 void (*callback) (struct cn..)    - connector's callback.
                  cn_msg and the sender's credentials


void cn_del_callback(struct cb_id *id);

 Unregisters new callback with connector core.

 struct cb_id *id        - unique connector's user identifier.


int cn_netlink_send_multi(struct cn_msg *msg, u16 len, u32 portid, u32 __groups, int gfp_mask);
int cn_netlink_send(struct cn_msg *msg, u32 portid, u32 __groups, int gfp_mask);

 Sends message to the specified groups.  It can be safely called from
 softirq context, but may silently fail under strong memory pressure.
 If there are no listeners for given group -ESRCH can be returned.

 struct cn_msg *        - message header(with attached data).
 u16 len            - for *_multi multiple cn_msg messages can be sent
 u32 port            - destination port.
                   If non-zero the message will be sent to the
                  given port, which should be set to the
                  original sender.
 u32 __group            - destination group.
                  If port and __group is zero, then appropriate group will
                  be searched through all registered connector users,
                  and message will be delivered to the group which was
                  created for user with the same ID as in msg.
                  If __group is not zero, then message will be delivered
                  to the specified group.
 int gfp_mask            - GFP mask.
```

 Note: When registering new callback user, connector core assigns netlink group to the user which is equal to its id.idx.

### Protocol description.

The current framework offers a transport layer with fixed headers.  The
recommended protocol which uses such a header is as following:

msg->seq and msg->ack are used to determine message genealogy.  When
someone sends a message, they use a locally unique sequence and random
acknowledge number.  The sequence number may be copied into
nlmsghdr->nlmsg_seq too.

The sequence number is incremented with each message sent.

If you expect a reply to the message, then the sequence number in the
received message MUST be the same as in the original message, and the
acknowledge number MUST be the same + 1.

If we receive a message and its sequence number is not equal to one we
are expecting, then it is a new message.  If we receive a message and
its sequence number is the same as one we are expecting, but its
acknowledge is not equal to the sequence number in the original
message + 1, then it is a new message.

Obviously, the protocol header contains the above id.

The connector allows event notification in the following form: kernel
driver or userspace process can ask connector to notify it when
selected ids will be turned on or off (registered or unregistered its
callback).  It is done by sending a special command to the connector
driver (it also registers itself with id={-1, -1}).

As example of this usage can be found in the cn_test.c module which
uses the connector to request notification and to send messages.

### Reliability.

Netlink itself is not a reliable protocol.  That means that messages can
be lost due to memory pressure or process' receiving queue overflowed,
so caller is warned that it must be prepared.  That is why the struct
cn_msg [main connector's message header] contains u32 seq and u32 ack
fields.

### Userspace usage.

2.6.14 has a new netlink socket implementation, which by default does not
allow people to send data to netlink groups other than 1.
So, if you wish to use a netlink socket (for example using connector)
with a different group number, the userspace application must subscribe to
that group first.  It can be achieved by the following pseudocode:

```c
s = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);

l_local.nl_family = AF_NETLINK;
l_local.nl_groups = 12345;
l_local.nl_pid = 0;

if (bind(s, (struct sockaddr *)&l_local, sizeof(struct sockaddr_nl)) == -1) {
    perror("bind");
    close(s);
    return -1;
}

{
    int on = l_local.nl_groups;
    setsockopt(s, 270, 1, &on, sizeof(on));
}
```

Where 270 above is SOL_NETLINK, and 1 is a NETLINK_ADD_MEMBERSHIP socket
option.  To drop a multicast subscription, one should call the above socket
option with the NETLINK_DROP_MEMBERSHIP parameter which is defined as 0.

2.6.14 netlink code only allows to select a group which is less or equal to
the maximum group number, which is used at netlink_kernel_create() time.
In case of connector it is CN_NETLINK_USERS + 0xf, so if you want to use
group number 12345, you must increment CN_NETLINK_USERS to that number.
Additional 0xf numbers are allocated to be used by non-in-kernel users.

Due to this limitation, group 0xffffffff does not work now, so one can
not use add/remove connector's group notifications, but as far as I know, 
only cn_test.c test module used it.

### Code samples

[linux/samples/connector](https://github.com/torvalds/linux/tree/master/samples/connector)



## References

https://blog.csdn.net/Longyu_wlz/article/details/108879110