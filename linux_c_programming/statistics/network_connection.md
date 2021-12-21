# 网络链接/监听端口

[参考 ss 命令的方式](https://github.com/sivasankariit/iproute2/blob/master/misc/ss.c)， 遍历 **/proc/[pid]/fd** 文件夹下, 获取  socket 对应的 socket inode, 这个 indoe 与 /proc/net/tcp 的 inode 对应。

```bash
$l /proc/1/fd/
lrwx------ 1 root root 64 Dec 10 12:52 0 -> /dev/null
lrwx------ 1 root root 64 Dec 10 12:52 1 -> /dev/null
lrwx------ 1 root root 64 Dec 10 12:52 12 -> socket:[13602]
lrwx------ 1 root root 64 Dec 10 12:52 14 -> socket:[69472617]
lrwx------ 1 root root 64 Dec 10 12:52 18 -> socket:[13607]
lrwx------ 1 root root 64 Dec 14 14:08 22 -> socket:[8868]
lrwx------ 1 root root 64 Dec 10 12:52 37 -> socket:[22235]
lr-x------ 1 root root 64 Dec 10 12:52 11 -> /proc/swaps
lr-x------ 1 root root 64 Dec 10 12:52 10 -> anon_inode:inotify
lrwx------ 1 root root 64 Dec 10 12:52 3 -> anon_inode:[timerfd]
.....
```

/proc/net/tcp

/proc/net/tcp6

/proc/net/udp

/proc/net/udp6

```txt
   46: 010310AC:9C4C 030310AC:1770 01
   |      |      |      |      |   |--> 链接状态(connection state) 详见下方
   |      |      |      |      |------> 远端端口，主机字节序(remote TCP port number)
   |      |      |      |-------------> 远端IP，  网络字节序(remote IPv4 address)
   |      |      |--------------------> 本地端口，主机字节序(local TCP port number)
   |      |---------------------------> 本地IP，  网络字节序(local IPv4 address)
   |----------------------------------> 序号,10进制(number of entry)

   00000150:00000000 01:00000019 00000000
      |        |     |     |       |--> 超时重传次数(number of unrecovered RTO timeouts)
      |        |     |     |----------> 超时时间，单位是jiffies(number of jiffies until timer expires)
      |        |     |----------------> 定时器类型(timer_active) 详见下方
      |        |----------------------> 当状态是ESTABLISHED，表示接收队列中数据长度；状态是LISTEN，表示已经完成连接队列的长度(receive-queue)
      |-------------------------------> 发送队列中数据长度(transmit-queue)

   1000        0 54165785 4 cd1e6040 25 4 27 3 -1
    |          |    |     |    |     |  | |  | |--> slow start size threshold, or -1 if the threshold is >= 0xFFFF
    |          |    |     |    |     |  | |  |    （如果慢启动阈值大于等于0xFFFF则显示-1，否则表示慢启动阈值）
    |          |    |     |    |     |  | |  |----> 当前拥塞窗口大小,sending congestion window
    |          |    |     |    |     |  | |-------> 快速确认数和是否启用的标志位的或运算结果,  (ack.quick<<1)|ack.pingpong
    |          |    |     |    |     |  |---------> 用来计算延时确认的估值,Predicted tick of soft clock (delayed ACK control data)
    |          |    |     |    |     |------------> RTO，单位是clock_t, retransmit timeout
    |          |    |     |    |------------------> socket实例的地址, location of socket in memory
    |          |    |     |-----------------------> socket结构体的引用数, socket reference count
    |          |    |-----------------------------> 套接字对应的inode, inode
    |          |----------------------------------> 持续定时器或保活定时器周期性发送出去但未被确认的TCP段数目，在收到ACK之后清零, unanswered 0-window probes
    |---------------------------------------------> 用户id, uid

==============================================================================================
链接状态(16进制):
  TCP_ESTABLISHED:01   TCP_SYN_SENT:02   TCP_SYN_RECV:03    TCP_FIN_WAIT1:04
  TCP_FIN_WAIT2:05     TCP_TIME_WAIT:06  TCP_CLOSE:07       TCP_CLOSE_WAIT:08
  TCP_LAST_ACL:09      TCP_LISTEN:0A     TCP_CLOSING:0B
==============================================================================================
timer_active:
  0  没有启动定时器,no timer is pending
  1  重传定时器,retransmit-timer is pending
  2  连接定时器、FIN_WAIT_2定时器或TCP保活定时器, another timer (e.g. delayed ack or keepalive) is pending
  3  TIME_WAIT定时器, this is a socket in TIME_WAIT state. Not all fields will contain data (or even exist)
  4  持续定时器, zero window probe timer is pending
```


##　参考链接

https://www.kernel.org/doc/Documentation/networking/proc_net_tcp.rst

https://blog.csdn.net/whatday/article/details/100693051

https://linux.die.net/man/5/proc
