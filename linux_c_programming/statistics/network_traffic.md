# 网卡收发数据统计

查看网卡接收发送数据的记录可以使用下面的命令:

```bash
ifconfig

netstat -i

ip -s link
```

这些命令中信息都是从 `/proc/net/dev` 文件中读取的。 

```bash
$ cat /proc/net/dev

Interface | Receive                                                            | Transmit                                                          |
          | bytes          packets   errs drop fifo frame compressed multicast | bytes          packets  errs drop fifo colls carrier compressed   |
  eth0:     24641739937    73278544  0    0    0    0     0         0            11725004304    16899557 0    0    0    0     0       0
    lo:     564187891      8028145   0    0    0    0     0         0            564187891      8028145  0    0    0    0     0       0
docker0:    1198418672     2034021   0    0    0    0     0         0            7410287401     2754797  0    0    0    0     0       0
```

这里的统计信息均来自于内核驱动层，驱动实际上都是通过获取硬件中的状态来得到这些信息的，因此这里统计的值都是网卡上会出现的情况，驱动从网卡寄存器中读取到相应的计数信息，因此这些计数信息不涉及系统软件协议栈中的丢包和错误。

- 1.`errors` 指的是网卡接收异常统计，这个值是从网卡上读取到的，并非内核计数，因此具体含义需要参考网卡的技术说明书，可以认为是接收到异常包，接收异常错误统计的总和。
- 2.`overruns` 指的是 `fifo` 被填满了从而导致的丢包量，当内核申请内存给网卡使用，如果被填满后，内核还没有来得及读取和清空数据，那么就会触发overrun，从而把第一个包丢弃掉。
- 3.`frame` 指的是帧格式错误计数，一般是帧不符合要求，比如长度未进行8字节对齐，2层帧中的crc校验错误等，很可能是网线或者网口异常引起。
- 4.`rx_dropped` 统计了网卡丢包数同时也包括网卡dev层的内核丢包，比如内核发现网卡传递过来的包是不支持的协议类型，那么就会丢弃该包同时增加该计数。
  
  

## 参考链接

https://www.codenong.com/cs110957428/

https://serverfault.com/questions/533513/how-to-get-tx-rx-bytes-without-ifconfig
