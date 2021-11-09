# cBPF

翻译整理于下面的文档：

> https://www.kernel.org/doc/html/latest/networking/filter.html#networking-filter
> 
> https://www.freebsd.org/cgi/man.cgi?query=bpf&sektion=4&manpath=FreeBSD+4.7-RELEASE
> 
> https://nick-black.com/dankwiki/index.php/The_Proc_Connector_and_Socket_Filters
> 
> https://blog.csdn.net/qq_14978113/article/details/80488711

## 简介

BPF 允许用户空间程序将过滤器附加到任何套接字并允许或禁止某些类型的数据通过套接字。
虽然我们这里只讨论套接字，但是在 Linux 中 BPF 有很多地方都在使用。
有用于 netfilter 的 xt_bpf、内核 qdisc 层中的 cls_bpf、SECCOMP-BPF，
以及许多其他使用 BPF 的地方，例如团队驱动程序、PTP 代码等。

**附加过滤器**

通过选项 `SO_ATTACH_FILTER` 将其发送到内核，过滤器代码通过内核检查，将立即开始过滤该套接字上的数据。

```c
/* SO_ATTACH_FILTER  的调用需要传递 struct sock_fprog 结构的指针 */
struct sock_fprog bpf; //需要给bpf结构赋值(这是最复杂的部分,在下面章节说明)
setsockopt(sockfd, SOL_SOCKET, SO_ATTACH_FILTER, &bpf, sizeof(bpf));
```

> 如果在一个运行有过滤器的套接字上添加不同的过滤器，假如过滤器通过检查，内核会负责移除旧过滤器并将新过滤器放置在其位置，如果失败，旧过滤器将保留在该套接字上。

**分离过滤器**

通过选项 `SO_DETACH_FILTER` 从套接字中分离过滤器。 
这个操作可能不常用，因为当您关闭带有过滤器的套接字时，过滤器会被自动删除。

```c
/* SO_DETACH_FILTER 的调用不需要任何参数 */
setsockopt(sockfd, SOL_SOCKET, SO_DETACH_FILTER, NULL, 0);
```

**锁定过滤器**

通过选项 `SO_LOCK_FILTER` 锁定连接到套接字的过滤器。过滤器一旦设置，就无法移除或更改，确保过滤器将被保留直到套接字关闭。

```c
/* SO_LOCK_FILTER 的调用，需要传递 0 或 1 */
int val = 1; //or 0
setsockopt(sockfd, SOL_SOCKET, SO_LOCK_FILTER, &val, sizeof(val));
```

以上这些操作最大用户可能就是 libpcap 了。例如发出像 `tcpdump -i eth0 port 22` 这样的高级过滤命令会通过 libpcap 内部编译器生成一个`struct sock_fprog` 结构，通过 `SO_ATTACH_FILTER` 加载到内核中。这个结构可以通过 `tcpdump -i eth0 port 22 -ddd` 显示出来。

> ```bash
>    # tcpdump 显示包匹配的选项
>    -d     导出 packet-matching code 为可读性好的格式.
>    -dd    导出 packet-matching code 为 C 语言片段.
>    -ddd   导出 packet-matching code 为十进制 (在计数之前).
> ```

## 架构

BPF 在内核中实际上是一个虚拟机(与java虚拟机的原理一致)，有自己定义的虚拟寄存器组和指令集。

### 虚拟寄存器

虚拟寄存器有如下, 总计18个 :

| 元素   | 位数  | 说明                        |
| ---- | --- | ------------------------- |
| A    | 32位 | 所有加载指令的目的地址和所有指令运算结果的存储地址 |
| X    | 32位 | 辅助寄存器(例如移位的位数，除法的除数)      |
| M[i] | 32位 | i=0~15 共16个寄存器，可以自由使用     |

### 指令集

指令集由 load、store、branch(分支选择)、return、alu(算术计算)、miscellaneous(杂项)这几个分类组成。

| 指令   | 寻址方式           | 描述                     | 分类            |
| ---- | -------------- | ---------------------- | ------------- |
| ld   | 1, 2, 3, 4, 12 | 加载字(32 bit)到 寄存器A      | load          |
| ldi  | 4              | 加载字(32 bit)到 寄存器A      | load          |
| ldh  | 1, 2           | 加载半字(16 bit)到 寄存器A     | load          |
| ldb  | 1, 2           | 加载字节(8 bit)到 寄存器A      | load          |
| ldx  | 3, 4, 5, 12    | 加载字(32 bit)到 寄存器X      | load          |
| ldxi | 4              | 加载字(32 bit)到 寄存器X      | load          |
| ldxb | 5              | 加载字节(8 bit)到 寄存器X      | load          |
| st   | 3              | 存储 寄存器A 的内容到 寄存器M[?] 中 | store         |
| stx  | 3              | 存储 寄存器X 的内容到 寄存器M[?] 中 | store         |
| jmp  | 6              | Jump to label          | branch        |
| ja   | 6              | Jump to label          | branch        |
| jeq  | 7, 8, 9, 10    | Jump on A == <x>       | branch        |
| jneq | 9, 10          | Jump on A != <x>       | branch        |
| jne  | 9, 10          | Jump on A != <x>       | branch        |
| jlt  | 9, 10          | Jump on A <  <x>       | branch        |
| jle  | 9, 10          | Jump on A <= <x>       | branch        |
| jgt  | 7, 8, 9, 10    | Jump on A >  <x>       | branch        |
| jge  | 7, 8, 9, 10    | Jump on A >= <x>       | branch        |
| jset | 7, 8, 9, 10    | Jump on A &  <x>       | branch        |
| add  | 0, 4           | A + <x>                | alu           |
| sub  | 0, 4           | A - <x>                | alu           |
| mul  | 0, 4           | A * <x>                | alu           |
| div  | 0, 4           | A / <x>                | alu           |
| mod  | 0, 4           | A % <x>                | alu           |
| neg  | -              | !A                     | alu           |
| and  | 0, 4           | A & <x>                | alu           |
| or   | 0, 4           | A                      | alu           |
| xor  | 0, 4           | A ^ <x>                | alu           |
| lsh  | 0, 4           | 左移 A << <x>            | alu           |
| rsh  | 0, 4           | 右移 A >> <x>            | alu           |
| tax  | -              | 将 寄存器A 的值拷贝到 寄存器X 中    | miscellaneous |
| txa  | -              | 将 寄存器X 的值拷贝到 寄存器A 中    | miscellaneous |
| ret  | 4, 11          | 返回允许发送的数据包长度           | return        |

下表显示了上面第 2 列中的寻址格式:

| 寻址方式 | 语法          | 描述                                              |
| ---- | ----------- | ----------------------------------------------- |
| 0    | x/%x        | 寄存器X 的值                                         |
| 1    | [k]         | BHW at byte offset k in the packet              |
| 2    | [x + k]     | BHW at the offset X + k in the packet           |
| 3    | M[k]        | 寄存器M[k] 的值                                      |
| 4    | #k          | 参数 k 的值, 例如 `#0x806`                            |
| 5    | 4*([k]&0xf) | Lower nibble * 4 at byte offset k in the packet |
| 6    | L           | Jump label L                                    |
| 7    | #k,Lt,Lf    | Jump to Lt if true, otherwise jump to Lf        |
| 8    | x/%x,Lt,Lf  | Jump to Lt if true, otherwise jump to Lf        |
| 9    | #k,Lt       | Jump to Lt if predicate is true                 |
| 10   | x/%x,Lt     | Jump to Lt if predicate is true                 |
| 11   | a/%a        | 寄存器A 的值                                         |
| 12   | extension   | BPF 扩展                                          |

Linux 内核还有一些 BPF 扩展，它们通过用负偏移量 + 特定扩展偏移量“重载” k 参数来与加载指令类一起使用。 这种 BPF 扩展的结果被加载到 寄存器A 中.

> 注意:
> 
> eBPF(extended BPF) 与 BPF extensions 是完全不同的概念
> 
> BPF extensions 是 Linnx 在 cBPF 基础上增加了一些扩展内容
> 
> eBPF 又称为 internal BPF, 是指令集接近CPU指令集,性能更好的架构的过滤系统

可能的 BPF 扩展如下表所示:

| 扩展         | 描述                                               |
| ---------- | ------------------------------------------------ |
| len        | skb->len                                         |
| proto      | skb->protocol                                    |
| type       | skb->pkt_type                                    |
| poff       | Payload start offset                             |
| ifidx      | skb->dev->ifindex                                |
| nla        | Netlink attribute of type X with offset A        |
| nlan       | Nested Netlink attribute of type X with offset A |
| mark       | skb->mark                                        |
| queue      | skb->queue_mapping                               |
| hatype     | skb->dev->type                                   |
| rxhash     | skb->hash                                        |
| cpu        | raw_smp_processor_id()                           |
| vlan_tci   | skb_vlan_tag_get(skb)                            |
| vlan_avail | skb_vlan_tag_present(skb)                        |
| vlan_tpid  | skb->vlan_proto                                  |
| rand       | prandom_u32()                                    |

### 代码定义

用户态应用程序需要包含头文件 `<linux/filter.h>` 该头文件中还包含了 `<linux/bpf_common.h>` 头文件,
基本囊括了所有 BPF 相关的定义。特别需要注意的在指令集的定义上，并非按照上文的指令集逐个定义，而是通过组合的方式定义。

其中包含了下列相关的结构:

```c
// linux/filter.h

struct sock_filter { /* Filter block */
    __u16   code;   /* 真实的汇编指令 */
    __u8    jt;     /* 指令结果为 true  的跳转 */
    __u8    jf;     /* 指令结果为 false 的跳转 */
    __u32   k;      /* 指令的参数(多用途通用字段) */
};

struct sock_fprog {  /* Required for SO_ATTACH_FILTER. */
    unsigned short    len;  /* Number of filter blocks */
    struct sock_filter __user *filter;
};

/* ret - BPF_K and BPF_X also apply */
#define BPF_RVAL(code)  ((code) & 0x18)
#define         BPF_A           0x10     //返回指令用, 返回 寄存器A 中的值   

/* misc */
#define BPF_MISCOP(code) ((code) & 0xf8)
#define         BPF_TAX         0x00     //杂项指令用, 将 寄存器A 的值拷贝到 寄存器X 中
#define         BPF_TXA         0x80     //杂项指令用, 将 寄存器X 的值拷贝到 寄存器A 中


#define BPF_STMT(code, k)         { (unsigned short)(code), 0,  0,  k }
#define BPF_JUMP(code, k, jt, jf) { (unsigned short)(code), jt, jf, k }
```

```c
// linux/bpf_common.h

/* Instruction classes, 指令的类别, 详细的使用方法见下面内容 */
#define BPF_CLASS(code) ((code) & 0x07)
#define         BPF_LD          0x00     // load data to register A
#define         BPF_LDX         0x01     // load data to register X
#define         BPF_ST          0x02     // 将寄存器A 的值存储到 寄存器M[k] 中
#define         BPF_STX         0x03     // 将寄存器X 的值存储到 寄存器M[k] 中
#define         BPF_ALU         0x04     // 算术运算, 最终结果保存到 寄存器A 中
#define         BPF_JMP         0x05     // 条件跳转
#define         BPF_RET         0x06     // 返回数据包的长度, 0表述忽略当前数据包
#define         BPF_MISC        0x07     // 杂项指令, 

/* ld/ldx fields */
#define BPF_SIZE(code)  ((code) & 0x18)
#define         BPF_W           0x00       //word, 32 bit
#define         BPF_H           0x08       //halfword, 16 bit
#define         BPF_B           0x10       //byte, 8 bit
#define BPF_MODE(code)  ((code) & 0xe0)
#define         BPF_IMM         0x00       //将参数 k(struct sock_filter) 直接加载到寄存器A中
#define         BPF_ABS         0x20       //absolute offset, 将数据包从头(0)向后偏移 k 个字节后,加载到寄存器A中
#define         BPF_IND         0x40       //variable offset, 将数据包从头(0)向后偏移 k + X(寄存器X的值) 个字节后,加载到寄存器A中
#define         BPF_MEM         0x60       //将寄存器M[k]的值加载到寄存器A中
#define         BPF_LEN         0x80       //packet length, 数据包长度
#define         BPF_MSH         0xa0       //

/* alu/jmp fields */
#define BPF_OP(code)    ((code) & 0xf0)
#define         BPF_ADD         0x00       //算术运算, 加
#define         BPF_SUB         0x10       //算术运算, 减
#define         BPF_MUL         0x20       //算术运算, 乘
#define         BPF_DIV         0x30       //算术运算, 除
#define         BPF_OR          0x40       //算术运算, 或
#define         BPF_AND         0x50       //算术运算, 与
#define         BPF_LSH         0x60       //算术运算, 左移
#define         BPF_RSH         0x70       //算术运算, 右移
#define         BPF_NEG         0x80       //算术运算, 非
#define         BPF_MOD         0x90       //算术运算, 取余
#define         BPF_XOR         0xa0       //算术运算, 异或

#define         BPF_JA          0x00       //跳转偏移量使用32位的参数k, 基本可以任意跳转
#define         BPF_JEQ         0x10       //相等判断( == ), 跳转使用8位的 jt 或 jf, 最大只能跳256
#define         BPF_JGT         0x20       //大于判断( >  ), 跳转使用8位的 jt 或 jf, 最大只能跳256
#define         BPF_JGE         0x30       //大于等于( >= ), 跳转使用8位的 jt 或 jf, 最大只能跳256
#define         BPF_JSET        0x40       //与操作  ( &  ), 跳转使用8位的 jt 或 jf, 最大只能跳256
#define BPF_SRC(code)   ((code) & 0x08)
#define         BPF_K           0x00       //操作数来自 参数k
#define         BPF_X           0x08       //操作数来自 寄存器X

#define BPF_MAXINSNS 4096
```

### 代码使用

P[]  means packet data,
P[i:n] gives the data at byte offset "i" in the packet, interpreted as a word (n=4), unsigned halfword (n=2), or    unsigned byte (n=1)

#### BPF_LD

加载值到 存器A 中。

| 指令                         | 说明           |
| -------------------------- | ------------ |
| BPF_LD \| BPF_W \| BPF_ABS | A = P[k:4]   |
| BPF_LD \| BPF_H \| BPF_ABS | A = P[k:2]   |
| BPF_LD \| BPF_B \| BPF_ABS | A = P[k:1]   |
| BPF_LD \| BPF_W \| BPF_IND | A = P[X+k:4] |
| BPF_LD \| BPF_H \| BPF_IND | A = P[X+k:2] |
| BPF_LD \| BPF_B \| BPF_IND | A = P[X+k:1] |
| BPF_LD \| BPF_W \| BPF_LEN | A = len      |
| BPF_LD \| BPF_IMM          | A = k        |
| BPF_LD \| BPF_MEM          | A = M[k]     |

#### BPF_LDX

加载值到 存器X 中。请注意 BPF_MSH，这是一种有效加载 IP 标头长度的技巧。

| 指令                          | 说明                 |
| --------------------------- | ------------------ |
| BPF_LDX \| BPF_W \| BPF_IMM | X = k              |
| BPF_LDX \| BPF_W \| BPF_MEM | X = M[k]           |
| BPF_LDX \| BPF_W \| BPF_LEN | X = len            |
| BPF_LDX \| BPF_B \| BPF_MSH | X = 4*(P[k:1]&0xf) |

#### BPF_ST

将 寄存器A 的值存储到 寄存器M[k] 中。

| 指令     | 说明       |
| ------ | -------- |
| BPF_ST | M[k] = A |

#### BPF_STX

将 寄存器X 的值存储到 寄存器M[k] 中。

| 指令      | 说明       |
| ------- | -------- |
| BPF_STX | M[k] = X |

#### BPF_ALU

alu算术指令执行 寄存器A 和 寄存器X 或 常量 之间的操作，并将结果存储回 寄存器A。 
对于二进制操作，需要指定源模式（BPF_K 或 BPF_X）。

| 指令                          | 说明         |
| --------------------------- | ---------- |
| BPF_ALU \| BPF_ADD \| BPF_K | A = A + k  |
| BPF_ALU \| BPF_SUB \| BPF_K | A = A - k  |
| BPF_ALU \| BPF_MUL \| BPF_K | A = A * k  |
| BPF_ALU \| BPF_DIV \| BPF_K | A = A / k  |
| BPF_ALU \| BPF_AND \| BPF_K | A = A & k  |
| BPF_ALU \| BPF_OR  \| BPF_K | A = A      |
| BPF_ALU \| BPF_LSH \| BPF_K | A = A << k |
| BPF_ALU \| BPF_RSH \| BPF_K | A = A >> k |
| BPF_ALU \| BPF_ADD \| BPF_X | A = A + X  |
| BPF_ALU \| BPF_SUB \| BPF_X | A = A - X  |
| BPF_ALU \| BPF_MUL \| BPF_X | A = A * X  |
| BPF_ALU \| BPF_DIV \| BPF_X | A = A / X  |
| BPF_ALU \| BPF_AND \| BPF_X | A = A & X  |
| BPF_ALU \| BPF_OR  \| BPF_X | A = A      |
| BPF_ALU \| BPF_LSH \| BPF_X | A = A << X |
| BPF_ALU \| BPF_RSH \| BPF_X | A = A >> X |
| BPF_ALU \| BPF_NEG          | A = !A     |

#### BPF_JMP

跳转指令改变控制流。
条件跳转将 寄存器A 与常量(BPF_K) 或寄存器X (BPF_X) 进行比较。
如果结果为真(或非零)，则采用真分支，否则采用假分支。
跳转偏移量以 8 位编码，因此最长跳转是 256 条指令。
但是，始终跳转(BPF_JA) 操作码使用 32 位 k 字段作为偏移量，允许任意远的目的地。
所有条件都使用无符号比较约定。

| 指令                           | 说明                       |
| ---------------------------- | ------------------------ |
| BPF_JMP \| BPF_JA            | pc += k                  |
| BPF_JMP \| BPF_JGT  \| BPF_K | pc += (A > k)  ? jt : jf |
| BPF_JMP \| BPF_JGE  \| BPF_K | pc += (A >= k) ? jt : jf |
| BPF_JMP \| BPF_JEQ  \| BPF_K | pc += (A == k) ? jt : jf |
| BPF_JMP \| BPF_JSET \| BPF_K | pc += (A & k)  ? jt : jf |
| BPF_JMP \| BPF_JGT  \| BPF_X | pc += (A > X)  ? jt : jf |
| BPF_JMP \| BPF_JGE  \| BPF_X | pc += (A >= X) ? jt : jf |
| BPF_JMP \| BPF_JEQ  \| BPF_X | pc += (A == X) ? jt : jf |
| BPF_JMP \| BPF_JSET \| BPF_X | pc += (A & X)  ? jt : jf |

#### BPF_RET

返回指令终止过滤器程序并指定要接受的数据包数量(即，它们返回截断数量)。 
返回值为零表示应忽略该数据包。 
返回值是常量 (BPF_K) 寄存器A (BPF_A)。

| 指令               | 说明             |
| ---------------- | -------------- |
| BPF_RET \| BPF_A | accept A bytes |
| BPF_RET \| BPF_K | accept k bytes |

#### BPF_MISC

杂项类别是为不属于上述类别的任何内容以及可能需要添加的任何新说明创建的。
目前，这些是将索引寄存器复制到累加器或反之亦然的寄存器传输指令。

| 指令                  | 说明    |
| ------------------- | ----- |
| BPF_MISC \| BPF_TAX | X = A |
| BPF_MISC \| BPF_TXA | A = X |

如下面例子所示:

```c
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
/* ... */

/* From the example above: tcpdump -i eth0 port 22 -dd */
struct sock_filter code[] = {
    { 0x28,  0,  0, 0x0000000c },
    { 0x15,  0,  8, 0x000086dd },
    { 0x30,  0,  0, 0x00000014 },
    { 0x15,  2,  0, 0x00000084 },
    { 0x15,  1,  0, 0x00000006 },
    { 0x15,  0, 17, 0x00000011 },
    { 0x28,  0,  0, 0x00000036 },
    { 0x15, 14,  0, 0x00000016 },
    { 0x28,  0,  0, 0x00000038 },
    { 0x15, 12, 13, 0x00000016 },
    { 0x15,  0, 12, 0x00000800 },
    { 0x30,  0,  0, 0x00000017 },
    { 0x15,  2,  0, 0x00000084 },
    { 0x15,  1,  0, 0x00000006 },
    { 0x15,  0,  8, 0x00000011 },
    { 0x28,  0,  0, 0x00000014 },
    { 0x45,  6,  0, 0x00001fff },
    { 0xb1,  0,  0, 0x0000000e },
    { 0x48,  0,  0, 0x0000000e },
    { 0x15,  2,  0, 0x00000016 },
    { 0x48,  0,  0, 0x00000010 },
    { 0x15,  0,  1, 0x00000016 },
    { 0x06,  0,  0, 0x0000ffff },
    { 0x06,  0,  0, 0x00000000 },
};

struct sock_fprog bpf = {
    .len = ARRAY_SIZE(code),
    .filter = code,
};

sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
if (sock < 0)
    /* ... bail out ... */

ret = setsockopt(sock, SOL_SOCKET, SO_ATTACH_FILTER, &bpf, sizeof(bpf));
if (ret < 0)
    /* ... bail out ... */

/* ... */
close(sock);
```

上面的示例代码为 `PF_PACKET` 套接字附加了一个套接字过滤器，以便让所有具有端口 22 的 IPv4/IPv6 数据包通过。 其余的将为此套接字丢弃。套接字过滤器不仅限于 PF_PACKET 套接字，但也可用于其他 socket 系列。

通常，对数据包套接字进行套接字过滤的大多数用例将由 libpcap 以高级语法涵盖，
因此作为应用程序开发人员，您应该坚持这一点。 libpcap 将它自己的层包裹在所有这些之上。

除非:

1) 不能使用/链接到 libpcap 库;
2) 所需的 BPF 过滤器使用 libpcap 编译器不支持的 Linux 扩展;
3) 过滤器可能更复杂，并且无法使用 libpcap 编译器很好的地实现;
4) 特定过滤器代码的优化应与 libpcap 的内部编译器不同;

那么在上面这些情况下，需要手工编写过滤器。例如，xt_bpf 和 cls_bpf 用户的需求可能会导致更复杂的过滤器代码，
或者无法用 libpcap 表达的需求（例如，不同代码路径的不同返回代码）。 
此外，BPF JIT 实现者可能希望手动编写测试用例，因此也需要对 BPF 代码进行低级访问。

BPF引擎和指令集

在 tools/bpf/ 下有一个名为 bpf_asm 的辅助工具，它可以用于编写上一节中提到的示例场景的低级过滤器。
这里提到的类似汇编的语法已经在 bpf_asm 中实现了，后面会用到(不再直接处理可读性较差的操作码）。 

bpf_asm 工具可以将程序翻译成 `操作码` 是一个由以下元素组成的数组（`struct sock_filter`）：

```txt
  op:16, jt:8, jf:8, k:32
```

元素 op 是一个 16 位宽的操作码，它是具有编码的特定指令。
jt 和 jf 是两个 8 位宽的跳转目标，一个用于条件“如果为真则跳转”，另一个用于“如果为假则跳转”。
最终，元素 k 包含一个杂项参数，可以根据 op 中给定的指令以不同方式解释。

These extensions can also be prefixed with '#'.
Examples for low-level BPF:

** ARP(地址解析协议) packets:

```asm
# tcpdump arp -d
(000) ldh      [12]                # 数据包的第12和13个字节(半字2个字节)的值存储到寄存器A中
(001) jeq      #0x806   jt 2  jf 3 # 比较寄存器A中的值是否为 0x0806(这个是ARP协议的标识号)
(002) ret      #262144             # jt 2, 如果判断为真,则跳转到本步骤, 返回 0x4000
(003) ret      #0                  # jf 3, 如果判断为假,则跳转到本步骤, 返回 0
```

** IPv4 TCP packets:

```asm
  ldh [12]
  jne #0x800, drop
  ldb [23]
  jneq #6, drop
  ret #-1
  drop: ret #0
```

** (Accelerated) VLAN w/ id 10:

```asm
  ld vlan_tci
  jneq #10, drop
  ret #-1
  drop: ret #0
```

** icmp random packet sampling, 1 in 4

```asm
  ldh [12]
  jne #0x800, drop
  ldb [23]
  jneq #1, drop
  # get a random uint32 number
  ld rand
  mod #4
  jneq #1, drop
  ret #-1
  drop: ret #0
```

** SECCOMP filter example:

```asm
  ld [4]                  /* offsetof(struct seccomp_data, arch) */
  jne #0xc000003e, bad    /* AUDIT_ARCH_X86_64 */
  ld [0]                  /* offsetof(struct seccomp_data, nr) */
  jeq #15, good           /* __NR_rt_sigreturn */
  jeq #231, good          /* __NR_exit_group */
  jeq #60, good           /* __NR_exit */
  jeq #0, good            /* __NR_read */
  jeq #1, good            /* __NR_write */
  jeq #5, good            /* __NR_fstat */
  jeq #9, good            /* __NR_mmap */
  jeq #14, good           /* __NR_rt_sigprocmask */
  jeq #13, good           /* __NR_rt_sigaction */
  jeq #35, good           /* __NR_nanosleep */
  bad: ret #0             /* SECCOMP_RET_KILL_THREAD */
  good: ret #0x7fff0000   /* SECCOMP_RET_ALLOW */
```

The above example code can be placed into a file (here called "foo"), and
then be passed to the bpf_asm tool for generating opcodes, output that xt_bpf
and cls_bpf understands and can directly be loaded with. Example with above
ARP code:

```bash
$ ./bpf_asm foo
4,40 0 0 12,21 0 1 2054,6 0 0 4294967295,6 0 0 0,
```

In copy and paste C-like output:

```bash
$ ./bpf_asm -c foo
{ 0x28,  0,  0, 0x0000000c },
{ 0x15,  0,  1, 0x00000806 },
{ 0x06,  0,  0, 0xffffffff },
{ 0x06,  0,  0, 0000000000 },
```

In particular, as usage with xt_bpf or cls_bpf can result in more complex BPF
filters that might not be obvious at first, it's good to test filters before
attaching to a live system. For that purpose, there's a small tool called
bpf_dbg under tools/bpf/ in the kernel source directory. This debugger allows
for testing BPF filters against given pcap files, single stepping through the
BPF code on the pcap's packets and to do BPF machine register dumps.

Starting bpf_dbg is trivial and just requires issuing:

```bash
./bpf_dbg
```

In case input and output do not equal stdin/stdout, bpf_dbg takes an
alternative stdin source as a first argument, and an alternative stdout
sink as a second one, e.g. `./bpf_dbg test_in.txt test_out.txt`.

Other than that, a particular libreadline configuration can be set via
file "~/.bpf_dbg_init" and the command history is stored in the file
"~/.bpf_dbg_history".

Interaction in bpf_dbg happens through a shell that also has auto-completion
support (follow-up example commands starting with '>' denote bpf_dbg shell).
The usual workflow would be to ...

load bpf 6,40 0 0 12,21 0 3 2048,48 0 0 23,21 0 1 1,6 0 0 65535,6 0 0 0
  Loads a BPF filter from standard output of bpf_asm, or transformed via
  e.g. `tcpdump -i eth0 -ddd port 22 | tr '\n' ','`. Note that for JIT
  debugging (next section), this command creates a temporary socket and
  loads the BPF code into the kernel. Thus, this will also be useful for
  JIT developers.

load pcap foo.pcap
  Loads standard tcpdump pcap file.

run [<n>]
bpf passes:1 fails:9
  Runs through all packets from a pcap to account how many passes and fails
  the filter will generate. A limit of packets to traverse can be given.

disassemble
l0:    ldh [12]
l1:    jeq #0x800, l2, l5
l2:    ldb [23]
l3:    jeq #0x1, l4, l5
l4:    ret #0xffff
l5:    ret #0
  Prints out BPF code disassembly.

dump
/* { op, jt, jf, k }, */
{ 0x28,  0,  0, 0x0000000c },
{ 0x15,  0,  3, 0x00000800 },
{ 0x30,  0,  0, 0x00000017 },
{ 0x15,  0,  1, 0x00000001 },
{ 0x06,  0,  0, 0x0000ffff },
{ 0x06,  0,  0, 0000000000 },
  Prints out C-style BPF code dump.

breakpoint 0
breakpoint at: l0:    ldh [12]
breakpoint 1
breakpoint at: l1:    jeq #0x800, l2, l5
  ...
  Sets breakpoints at particular BPF instructions. Issuing a `run` command
  will walk through the pcap file continuing from the current packet and
  break when a breakpoint is being hit (another `run` will continue from
  the currently active breakpoint executing next instructions):

run
  -- register dump --
  pc:       [0]                       <-- program counter
  code:     [40] jt[0] jf[0] k[12]    <-- plain BPF code of current instruction
  curr:     l0:    ldh [12]           <-- disassembly of current instruction
  A:        [00000000][0]             <-- content of A (hex, decimal)
  X:        [00000000][0]             <-- content of X (hex, decimal)
  M[0,15]:  [00000000][0]             <-- folded content of M (hex, decimal)
  -- packet dump --                   <-- Current packet from pcap (hex)
  len: 42
    0: 00 19 cb 55 55 a4 00 14 a4 43 78 69 08 06 00 01
   16: 08 00 06 04 00 01 00 14 a4 43 78 69 0a 3b 01 26
   32: 00 00 00 00 00 00 0a 3b 01 01
  (breakpoint)

breakpoint
breakpoints: 0 1
  Prints currently set breakpoints.

step [-<n>, +<n>]
  Performs single stepping through the BPF program from the current pc
  offset. Thus, on each step invocation, above register dump is issued.
  This can go forwards and backwards in time, a plain `step` will break
  on the next BPF instruction, thus +1. (No `run` needs to be issued here.)

select <n>
  Selects a given packet from the pcap file to continue from. Thus, on
  the next `run` or `step`, the BPF program is being evaluated against
  the user pre-selected packet. Numbering starts just as in Wireshark
  with index 1.

quit

  Exits bpf_dbg.

JIT compiler
------------

The Linux kernel has a built-in BPF JIT compiler for x86_64, SPARC,
PowerPC, ARM, ARM64, MIPS, RISC-V and s390 and can be enabled through
CONFIG_BPF_JIT. The JIT compiler is transparently invoked for each
attached filter from user space or for internal kernel users if it has
been previously enabled by root:

```bash
  echo 1 > /proc/sys/net/core/bpf_jit_enable
```

For JIT developers, doing audits etc, each compile run can output the generated
opcode image into the kernel log via:

```bash
  echo 2 > /proc/sys/net/core/bpf_jit_enable
```

Example output from dmesg:

```txt
[ 3389.935842] flen=6 proglen=70 pass=3 image=ffffffffa0069c8f
[ 3389.935847] JIT code: 00000000: 55 48 89 e5 48 83 ec 60 48 89 5d f8 44 8b 4f 68
[ 3389.935849] JIT code: 00000010: 44 2b 4f 6c 4c 8b 87 d8 00 00 00 be 0c 00 00 00
[ 3389.935850] JIT code: 00000020: e8 1d 94 ff e0 3d 00 08 00 00 75 16 be 17 00 00
[ 3389.935851] JIT code: 00000030: 00 e8 28 94 ff e0 83 f8 01 75 07 b8 ff ff 00 00
[ 3389.935852] JIT code: 00000040: eb 02 31 c0 c9 c3
```

When CONFIG_BPF_JIT_ALWAYS_ON is enabled, bpf_jit_enable is permanently set to 1 and
setting any other value than that will return in failure. This is even the case for
setting bpf_jit_enable to 2, since dumping the final JIT image into the kernel log
is discouraged and introspection through bpftool (under tools/bpf/bpftool/) is the
generally recommended approach instead.

In the kernel source tree under tools/bpf/, there's bpf_jit_disasm for
generating disassembly out of the kernel log's hexdump:

```bash
./bpf_jit_disasm
```

70 bytes emitted from JIT compiler (pass:3, flen:6)

```asm
ffffffffa0069c8f + <x>:
   0:    push   %rbp
   1:    mov    %rsp,%rbp
   4:    sub    $0x60,%rsp
   8:    mov    %rbx,-0x8(%rbp)
   c:    mov    0x68(%rdi),%r9d
  10:    sub    0x6c(%rdi),%r9d
  14:    mov    0xd8(%rdi),%r8
  1b:    mov    $0xc,%esi
  20:    callq  0xffffffffe0ff9442
  25:    cmp    $0x800,%eax
  2a:    jne    0x0000000000000042
  2c:    mov    $0x17,%esi
  31:    callq  0xffffffffe0ff945e
  36:    cmp    $0x1,%eax
  39:    jne    0x0000000000000042
  3b:    mov    $0xffff,%eax
  40:    jmp    0x0000000000000044
  42:    xor    %eax,%eax
  44:    leaveq
  45:    retq
```

Issuing option `-o` will "annotate" opcodes to resulting assembler
instructions, which can be very useful for JIT developers:

```bash
 ./bpf_jit_disasm -o
```

70 bytes emitted from JIT compiler (pass:3, flen:6)

```asm
ffffffffa0069c8f + <x>:
   0:    push   %rbp
    55
   1:    mov    %rsp,%rbp
    48 89 e5
   4:    sub    $0x60,%rsp
    48 83 ec 60
   8:    mov    %rbx,-0x8(%rbp)
    48 89 5d f8
   c:    mov    0x68(%rdi),%r9d
    44 8b 4f 68
  10:    sub    0x6c(%rdi),%r9d
    44 2b 4f 6c
  14:    mov    0xd8(%rdi),%r8
    4c 8b 87 d8 00 00 00
  1b:    mov    $0xc,%esi
    be 0c 00 00 00
  20:    callq  0xffffffffe0ff9442
    e8 1d 94 ff e0
  25:    cmp    $0x800,%eax
    3d 00 08 00 00
  2a:    jne    0x0000000000000042
    75 16
  2c:    mov    $0x17,%esi
    be 17 00 00 00
  31:    callq  0xffffffffe0ff945e
    e8 28 94 ff e0
  36:    cmp    $0x1,%eax
    83 f8 01
  39:    jne    0x0000000000000042
    75 07
  3b:    mov    $0xffff,%eax
    b8 ff ff 00 00
  40:    jmp    0x0000000000000044
    eb 02
  42:    xor    %eax,%eax
    31 c0
  44:    leaveq
    c9
  45:    retq
    c3
```

For BPF JIT developers, bpf_jit_disasm, bpf_asm and bpf_dbg provides a useful
toolchain for developing and testing the kernel's JIT compiler.
