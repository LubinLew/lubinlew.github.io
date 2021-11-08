# cBPF

翻译整理于下面的文档：

> https://www.kernel.org/doc/Documentation/networking/filter.txt
> 
> https://www.freebsd.org/cgi/man.cgi?query=bpf&sektion=4&manpath=FreeBSD+4.7-RELEASE
> 
> https://blog.csdn.net/qq_14978113/article/details/80488711

## 简介

BPF 允许用户空间程序将过滤器附加到任何套接字并允许或禁止某些类型的数据通过套接字。
在 Linux 上，BPF 比在 BSD 上简单得多。人们不必担心设备或类似的东西。

**附加过滤器**

通过选项 `SO_ATTACH_FILTER` 将其发送到内核，过滤器代码通过内核检查，将立即开始过滤该套接字上的数据。

```c
/* SO_ATTACH_FILTER  的调用需要传递 struct sock_fprog 结构的指针 */
struct sock_fprog bpf; //需要给bpf结构赋值(这是最复杂的部分,在下面章节说明)
setsockopt(sockfd, SOL_SOCKET, SO_ATTACH_FILTER, &bpf, sizeof(bpf));
```

> 一种不太常见的情况是在一个运行有过滤器的同一套接字上添加不同的过滤器，假过滤器通过检查，内核会负责移除旧过滤器并将新过滤器放置在其位置，如果失败，旧过滤器将保留在该套接字上

**分离过滤器**

通过选项 `SO_DETACH_FILTER` 从套接字中分离过滤器。 
这个操作可能不常用，因为当您关闭带有过滤器的套接字时，过滤器会被自动删除。

```c
/* SO_DETACH_FILTER 的调用不需要任何参数 */
setsockopt(sockfd, SOL_SOCKET, SO_DETACH_FILTER, NULL, 0);
```

**锁定过滤器**

通过选项 `SO_LOCK_FILTER` 锁定连接到套接字的过滤器。
过滤器一旦设置，就无法移除或更改，确保过滤器将被保留直到套接字关闭。

```c
/* SO_LOCK_FILTER 的调用，需要传递 0 或 1 */
int val = 1; //or 0
setsockopt(sockfd, SOL_SOCKET, SO_LOCK_FILTER,   &val, sizeof(val));
```

以上这些操作最大用户可能就是 libpcap 了。例如发出像 `tcpdump -i eth0 port 22` 这样的高级过滤命令会通过 libpcap 内部编译器生成一个`struct sock_fprog` 结构，通过 `SO_ATTACH_FILTER` 加载到内核中。这个结构可以通过 `tcpdump -i eth0 port 22 -ddd` 显示出来。

> ```bash
>    # tcpdump 显示包匹配的选项
>    -d     导出 packet-matching code 为可读性好的格式.
>    -dd    导出 packet-matching code 为 C 语言片段.
>    -ddd   导出 packet-matching code 为十进制 (在计数之前).
> ```

虽然我们这里只讨论了套接字，但是在 Linux 中 BPF 有很多地方都在使用。有用于 netfilter 的 xt_bpf、内核 qdisc 层中的 cls_bpf、SECCOMP-BPF，以及许多其他使用 BPF 的地方，例如团队驱动程序、PTP 代码等。

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

指令集由 load、store、branch(分支选择)、alu(算术计算)、miscellaneous(杂项)这几个分类组成。

| 指令   | 寻址方式           | 描述                     | 分类            | 代码对照表             |
| ---- | -------------- | ---------------------- | ------------- | ----------------- |
| ld   | 1, 2, 3, 4, 12 | 加载字(32 bit)到 寄存器A      | load          |                   |
| ldi  | 4              | 加载字(32 bit)到 寄存器A      | load          | BPF_LD \| BPF_IMM |
| ldh  | 1, 2           | 加载半字(16 bit)到 寄存器A     | load          |                   |
| ldb  | 1, 2           | 加载字(32 bit)到 寄存器A      | load          |                   |
| ldx  | 3, 4, 5, 12    | 加载字(32 bit)到 寄存器X      | load          |                   |
| ldxi | 4              | 加载字(32 bit)到 寄存器X      | load          |                   |
| ldxb | 5              | 加载字节(8 bit)到 寄存器X      | load          |                   |
| st   | 3              | 存储 寄存器A 的内容到 寄存器M[?] 中 | store         |                   |
| stx  | 3              | 存储 寄存器X 的内容到 寄存器M[?] 中 | store         |                   |
| jmp  | 6              | Jump to label          | branch        |                   |
| ja   | 6              | Jump to label          | branch        |                   |
| jeq  | 7, 8, 9, 10    | Jump on A == <x>       | branch        |                   |
| jneq | 9, 10          | Jump on A != <x>       | branch        |                   |
| jne  | 9, 10          | Jump on A != <x>       | branch        |                   |
| jlt  | 9, 10          | Jump on A <  <x>       | branch        |                   |
| jle  | 9, 10          | Jump on A <= <x>       | branch        |                   |
| jgt  | 7, 8, 9, 10    | Jump on A >  <x>       | branch        |                   |
| jge  | 7, 8, 9, 10    | Jump on A >= <x>       | branch        |                   |
| jset | 7, 8, 9, 10    | Jump on A &  <x>       | branch        |                   |
| add  | 0, 4           | A + <x>                | alu           |                   |
| sub  | 0, 4           | A - <x>                | alu           |                   |
| mul  | 0, 4           | A * <x>                | alu           |                   |
| div  | 0, 4           | A / <x>                | alu           |                   |
| mod  | 0, 4           | A % <x>                | alu           |                   |
| neg  |                | !A                     | alu           |                   |
| and  | 0, 4           | A & <x>                | alu           |                   |
| or   | 0, 4           | A                      | alu           |                   |
| xor  | 0, 4           | A ^ <x>                | alu           |                   |
| lsh  | 0, 4           | 左移 A << <x>            | alu           |                   |
| rsh  | 0, 4           | 右移 A >> <x>            | alu           |                   |
| tax  |                | Copy A into X          | miscellaneous |                   |
| txa  |                | Copy X into A          | miscellaneous |                   |
| ret  | 4, 11          | Return                 | miscellaneous |                   |

下表显示了上面第 2 列中的寻址格式:

| 寻址方式 | 语法          | 描述                                              |
| ---- | ----------- | ----------------------------------------------- |
| 0    | x/%x        | Register X                                      |
| 1    | [k]         | BHW at byte offset k in the packet              |
| 2    | [x + k]     | BHW at the offset X + k in the packet           |
| 3    | M[k]        | Word at offset k in M[]                         |
| 4    | #k          | Literal value stored in k                       |
| 5    | 4*([k]&0xf) | Lower nibble * 4 at byte offset k in the packet |
| 6    | L           | Jump label L                                    |
| 7    | #k,Lt,Lf    | Jump to Lt if true, otherwise jump to Lf        |
| 8    | x/%x,Lt,Lf  | Jump to Lt if true, otherwise jump to Lf        |
| 9    | #k,Lt       | Jump to Lt if predicate is true                 |
| 10   | x/%x,Lt     | Jump to Lt if predicate is true                 |
| 11   | a/%a        | Accumulator A                                   |
| 12   | extension   | BPF extension                                   |

The Linux kernel also has a couple of BPF extensions that are used along
with the class of load instructions by "overloading" the k argument with
a negative offset + a particular extension offset. The result of such BPF extensions are loaded into A.

Linux 内核还有一些 BPF 扩展，它们通过用负偏移量 + 特定扩展偏移量“重载” k 参数来与加载指令类一起使用。 
这种 BPF 扩展的结果被加载到 寄存器A 中.

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
### linux/filter.h

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

#define BPF_STMT(code, k) { (unsigned short)(code), 0, 0, k }
#define BPF_JUMP(code, k, jt, jf) { (unsigned short)(code), jt, jf, k }
```

```c
### linux/bpf_common.h

/* Instruction classes, 指令的类别 */
#define BPF_CLASS(code) ((code) & 0x07)
#define         BPF_LD          0x00     // load data to register A
#define         BPF_LDX         0x01     // load data to register X
#define         BPF_ST          0x02     
#define         BPF_STX         0x03
#define         BPF_ALU         0x04
#define         BPF_JMP         0x05
#define         BPF_RET         0x06
#define         BPF_MISC        0x07

/* ld/ldx fields */
#define BPF_SIZE(code)  ((code) & 0x18)
#define         BPF_W           0x00       //word, 32 bit
#define         BPF_H           0x08       //halfword, 16 bit
#define         BPF_B           0x10       //byte, 8 bit
#define BPF_MODE(code)  ((code) & 0xe0)
#define         BPF_IMM         0x00       //将参数 k(struct sock_filter) 直接加载到寄存器A中
#define         BPF_ABS         0x20       //absolute offset, 将数据向后偏移 k 个字节后,加载到寄存器A中
#define         BPF_IND         0x40       //variable offset, 将数据向后偏移 k + X(寄存器X的值) 个字节后,加载到寄存器A中
#define         BPF_MEM         0x60       //将寄存器M[k]的值加载到寄存器A中
#define         BPF_LEN         0x80       //packet length
#define         BPF_MSH         0xa0

/* alu/jmp fields */
#define BPF_OP(code)    ((code) & 0xf0)
#define         BPF_ADD         0x00
#define         BPF_SUB         0x10
#define         BPF_MUL         0x20
#define         BPF_DIV         0x30
#define         BPF_OR          0x40
#define         BPF_AND         0x50
#define         BPF_LSH         0x60
#define         BPF_RSH         0x70
#define         BPF_NEG         0x80
#define         BPF_MOD         0x90
#define         BPF_XOR         0xa0

#define         BPF_JA          0x00
#define         BPF_JEQ         0x10
#define         BPF_JGT         0x20
#define         BPF_JGE         0x30
#define         BPF_JSET        0x40
#define BPF_SRC(code)   ((code) & 0x08)
#define         BPF_K           0x00
#define         BPF_X           0x08

#define BPF_MAXINSNS 4096
```

P[]  means packet data,
P[i:n] gives the data at byte offset "i" in the packet, interpreted as a word (n=4), unsigned halfword (n=2), or    unsigned byte (n=1)

BPF_LD:
      BPF_LD | BPF_W | BPF_ABS  ->  A <- P[k:4]
      BPF_LD | BPF_H | BPF_ABS  ->  A <- P[k:2]
      BPF_LD | BPF_B | BPF_ABS  ->  A <- P[k:1]
      BPF_LD | BPF_W | BPF_IND  ->  A <- P[X+k:4]
      BPF_LD | BPF_H | BPF_IND  ->  A <- P[X+k:2]
      BPF_LD | BPF_B | BPF_IND  ->  A <- P[X+k:1]
      BPF_LD | BPF_W | BPF_LEN  ->  A <- len
      BPF_LD | BPF_IMM          ->  A <- k
      BPF_LD | BPF_MEM          ->  A <- M[k]

对于套接字过滤，指向该结构的指针通过函数 `setsockopt()` 传递给内核。
如下面例子所示:

```c
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
/* ... */

/* From the example above: tcpdump -i em1 port 22 -dd */
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

上面的示例代码为 `PF_PACKET` 套接字附加了一个套接字过滤器，
以便让所有具有端口 22 的 IPv4/IPv6 数据包通过。 其余的将为此套接字丢弃。

`setsockopt()` 对 `SO_DETACH_FILTER` 的调用不需要任何参数，
而用于防止过滤器被分离的 `SO_LOCK_FILTER` 选项，采用 0 或 1 的整数值。

请注意，套接字过滤器不仅限于 PF_PACKET 套接字，但也可用于其他 socket 系列。

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

```bash
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
  e.g. `tcpdump -iem1 -ddd port 22 | tr '\n' ','`. Note that for JIT
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

BPF kernel internals
--------------------

Internally, for the kernel interpreter, a different instruction set
format with similar underlying principles from BPF described in previous
paragraphs is being used. However, the instruction set format is modelled
closer to the underlying architecture to mimic native instruction sets, so
that a better performance can be achieved (more details later). This new
ISA is called 'eBPF' or 'internal BPF' interchangeably. (Note: eBPF which
originates from [e]xtended BPF is not the same as BPF extensions! While
eBPF is an ISA, BPF extensions date back to classic BPF's 'overloading'
of BPF_LD | BPF_{B,H,W} | BPF_ABS instruction.)

It is designed to be JITed with one to one mapping, which can also open up
the possibility for GCC/LLVM compilers to generate optimized eBPF code through
an eBPF backend that performs almost as fast as natively compiled code.

The new instruction set was originally designed with the possible goal in
mind to write programs in "restricted C" and compile into eBPF with a optional
GCC/LLVM backend, so that it can just-in-time map to modern 64-bit CPUs with
minimal performance overhead over two steps, that is, C -> eBPF -> native code.

Currently, the new format is being used for running user BPF programs, which
includes seccomp BPF, classic socket filters, cls_bpf traffic classifier,
team driver's classifier for its load-balancing mode, netfilter's xt_bpf
extension, PTP dissector/classifier, and much more. They are all internally
converted by the kernel into the new instruction set representation and run
in the eBPF interpreter. For in-kernel handlers, this all works transparently
by using bpf_prog_create() for setting up the filter, resp.
bpf_prog_destroy() for destroying it. The macro
BPF_PROG_RUN(filter, ctx) transparently invokes eBPF interpreter or JITed
code to run the filter. 'filter' is a pointer to struct bpf_prog that we
got from bpf_prog_create(), and 'ctx' the given context (e.g.
skb pointer). All constraints and restrictions from bpf_check_classic() apply
before a conversion to the new layout is being done behind the scenes!

Currently, the classic BPF format is being used for JITing on most
32-bit architectures, whereas x86-64, aarch64, s390x, powerpc64,
sparc64, arm32, riscv64, riscv32 perform JIT compilation from eBPF
instruction set.

Some core changes of the new internal format:

- Number of registers increase from 2 to 10:
  
  The old format had two registers A and X, and a hidden frame pointer. The
  new layout extends this to be 10 internal registers and a read-only frame
  pointer. Since 64-bit CPUs are passing arguments to functions via registers
  the number of args from eBPF program to in-kernel function is restricted
  to 5 and one register is used to accept return value from an in-kernel
  function. Natively, x86_64 passes first 6 arguments in registers, aarch64/
  sparcv9/mips64 have 7 - 8 registers for arguments; x86_64 has 6 callee saved
  registers, and aarch64/sparcv9/mips64 have 11 or more callee saved registers.
  
  Therefore, eBPF calling convention is defined as:
  
  * R0    - return value from in-kernel function, and exit value for eBPF program
  * R1 - R5    - arguments from eBPF program to in-kernel function
  * R6 - R9    - callee saved registers that in-kernel function will preserve
  * R10    - read-only frame pointer to access stack
  
  Thus, all eBPF registers map one to one to HW registers on x86_64, aarch64,
  etc, and eBPF calling convention maps directly to ABIs used by the kernel on
  64-bit architectures.
  
  On 32-bit architectures JIT may map programs that use only 32-bit arithmetic
  and may let more complex programs to be interpreted.
  
  R0 - R5 are scratch registers and eBPF program needs spill/fill them if
  necessary across calls. Note that there is only one eBPF program (== one
  eBPF main routine) and it cannot call other eBPF functions, it can only
  call predefined in-kernel functions, though.

- Register width increases from 32-bit to 64-bit:
  
  Still, the semantics of the original 32-bit ALU operations are preserved
  via 32-bit subregisters. All eBPF registers are 64-bit with 32-bit lower
  subregisters that zero-extend into 64-bit if they are being written to.
  That behavior maps directly to x86_64 and arm64 subregister definition, but
  makes other JITs more difficult.
  
  32-bit architectures run 64-bit internal BPF programs via interpreter.
  Their JITs may convert BPF programs that only use 32-bit subregisters into
  native instruction set and let the rest being interpreted.
  
  Operation is 64-bit, because on 64-bit architectures, pointers are also
  64-bit wide, and we want to pass 64-bit values in/out of kernel functions,
  so 32-bit eBPF registers would otherwise require to define register-pair
  ABI, thus, there won't be able to use a direct eBPF register to HW register
  mapping and JIT would need to do combine/split/move operations for every
  register in and out of the function, which is complex, bug prone and slow.
  Another reason is the use of atomic 64-bit counters.

- Conditional jt/jf targets replaced with jt/fall-through:
  
  While the original design has constructs such as "if (cond) jump_true;
  else jump_false;", they are being replaced into alternative constructs like
  "if (cond) jump_true; /* else fall-through */".

- Introduces bpf_call insn and register passing convention for zero overhead
  calls from/to other kernel functions:
  
  Before an in-kernel function call, the internal BPF program needs to
  place function arguments into R1 to R5 registers to satisfy calling
  convention, then the interpreter will take them from registers and pass
  to in-kernel function. If R1 - R5 registers are mapped to CPU registers
  that are used for argument passing on given architecture, the JIT compiler
  doesn't need to emit extra moves. Function arguments will be in the correct
  registers and BPF_CALL instruction will be JITed as single 'call' HW
  instruction. This calling convention was picked to cover common call
  situations without performance penalty.
  
  After an in-kernel function call, R1 - R5 are reset to unreadable and R0 has
  a return value of the function. Since R6 - R9 are callee saved, their state
  is preserved across the call.
  
  For example, consider three C functions:
  
  u64 f1() { return (*_f2)(1); }
  u64 f2(u64 a) { return f3(a + 1, a); }
  u64 f3(u64 a, u64 b) { return a - b; }
  
  GCC can compile f1, f3 into x86_64:
  
  f1:
    movl $1, %edi
    movq _f2(%rip), %rax
    jmp  *%rax
  f3:
    movq %rdi, %rax
    subq %rsi, %rax
    ret
  
  Function f2 in eBPF may look like:
  
  f2:
    bpf_mov R2, R1
    bpf_add R1, 1
    bpf_call f3
    bpf_exit
  
  If f2 is JITed and the pointer stored to '_f2'. The calls f1 -> f2 -> f3 and
  returns will be seamless. Without JIT, __bpf_prog_run() interpreter needs to
  be used to call into f2.
  
  For practical reasons all eBPF programs have only one argument 'ctx' which is
  already placed into R1 (e.g. on __bpf_prog_run() startup) and the programs
  can call kernel functions with up to 5 arguments. Calls with 6 or more arguments
  are currently not supported, but these restrictions can be lifted if necessary
  in the future.
  
  On 64-bit architectures all register map to HW registers one to one. For
  example, x86_64 JIT compiler can map them as ...
  
    R0 - rax
    R1 - rdi
    R2 - rsi
    R3 - rdx
    R4 - rcx
    R5 - r8
    R6 - rbx
    R7 - r13
    R8 - r14
    R9 - r15
    R10 - rbp
  
  ... since x86_64 ABI mandates rdi, rsi, rdx, rcx, r8, r9 for argument passing
  and rbx, r12 - r15 are callee saved.
  
  Then the following internal BPF pseudo-program:
  
    bpf_mov R6, R1 /* save ctx */
    bpf_mov R2, 2
    bpf_mov R3, 3
    bpf_mov R4, 4
    bpf_mov R5, 5
    bpf_call foo
    bpf_mov R7, R0 /* save foo() return value */
    bpf_mov R1, R6 /* restore ctx for next call */
    bpf_mov R2, 6
    bpf_mov R3, 7
    bpf_mov R4, 8
    bpf_mov R5, 9
    bpf_call bar
    bpf_add R0, R7
    bpf_exit
  
  After JIT to x86_64 may look like:
  
    push %rbp
    mov %rsp,%rbp
    sub $0x228,%rsp
    mov %rbx,-0x228(%rbp)
    mov %r13,-0x220(%rbp)
    mov %rdi,%rbx
    mov $0x2,%esi
    mov $0x3,%edx
    mov $0x4,%ecx
    mov $0x5,%r8d
    callq foo
    mov %rax,%r13
    mov %rbx,%rdi
    mov $0x6,%esi
    mov $0x7,%edx
    mov $0x8,%ecx
    mov $0x9,%r8d
    callq bar
    add %r13,%rax
    mov -0x228(%rbp),%rbx
    mov -0x220(%rbp),%r13
    leaveq
    retq
  
  Which is in this example equivalent in C to:
  
    u64 bpf_filter(u64 ctx)
    {
  
        return foo(ctx, 2, 3, 4, 5) + bar(ctx, 6, 7, 8, 9);
  
    }
  
  In-kernel functions foo() and bar() with prototype: u64 (*)(u64 arg1, u64
  arg2, u64 arg3, u64 arg4, u64 arg5); will receive arguments in proper
  registers and place their return value into '%rax' which is R0 in eBPF.
  Prologue and epilogue are emitted by JIT and are implicit in the
  interpreter. R0-R5 are scratch registers, so eBPF program needs to preserve
  them across the calls as defined by calling convention.
  
  For example the following program is invalid:
  
    bpf_mov R1, 1
    bpf_call foo
    bpf_mov R0, R1
    bpf_exit
  
  After the call the registers R1-R5 contain junk values and cannot be read.
  An in-kernel eBPF verifier is used to validate internal BPF programs.

Also in the new design, eBPF is limited to 4096 insns, which means that any
program will terminate quickly and will only call a fixed number of kernel
functions. Original BPF and the new format are two operand instructions,
which helps to do one-to-one mapping between eBPF insn and x86 insn during JIT.

The input context pointer for invoking the interpreter function is generic,
its content is defined by a specific use case. For seccomp register R1 points
to seccomp_data, for converted BPF filters R1 points to a skb.

A program, that is translated internally consists of the following elements:

  op:16, jt:8, jf:8, k:32    ==>    op:8, dst_reg:4, src_reg:4, off:16, imm:32

So far 87 internal BPF instructions were implemented. 8-bit 'op' opcode field
has room for new instructions. Some of them may use 16/24/32 byte encoding. New
instructions must be multiple of 8 bytes to preserve backward compatibility.

Internal BPF is a general purpose RISC instruction set. Not every register and
every instruction are used during translation from original BPF to new format.
For example, socket filters are not using 'exclusive add' instruction, but
tracing filters may do to maintain counters of events, for example. Register R9
is not used by socket filters either, but more complex filters may be running
out of registers and would have to resort to spill/fill to stack.

Internal BPF can be used as a generic assembler for last step performance
optimizations, socket filters and seccomp are using it as assembler. Tracing
filters may use it as assembler to generate code from kernel. In kernel usage
may not be bounded by security considerations, since generated internal BPF code
may be optimizing internal code path and not being exposed to the user space.
Safety of internal BPF can come from a verifier (TBD). In such use cases as
described, it may be used as safe instruction set.

Just like the original BPF, the new format runs within a controlled environment,
is deterministic and the kernel can easily prove that. The safety of the program
can be determined in two steps: first step does depth-first-search to disallow
loops and other CFG validation; second step starts from the first insn and
descends all possible paths. It simulates execution of every insn and observes
the state change of registers and stack.

eBPF opcode encoding
--------------------

eBPF is reusing most of the opcode encoding from classic to simplify conversion
of classic BPF to eBPF. For arithmetic and jump instructions the 8-bit 'code'
field is divided into three parts:

  +----------------+--------+--------------------+
  |   4 bits       |  1 bit |   3 bits           |
  | operation code | source | instruction class  |
  +----------------+--------+--------------------+
  (MSB)                                      (LSB)

Three LSB bits store instruction class which is one of:

  Classic BPF classes:    eBPF classes:

  BPF_LD    0x00          BPF_LD    0x00
  BPF_LDX   0x01          BPF_LDX   0x01
  BPF_ST    0x02          BPF_ST    0x02
  BPF_STX   0x03          BPF_STX   0x03
  BPF_ALU   0x04          BPF_ALU   0x04
  BPF_JMP   0x05          BPF_JMP   0x05
  BPF_RET   0x06          BPF_JMP32 0x06
  BPF_MISC  0x07          BPF_ALU64 0x07

When BPF_CLASS(code) == BPF_ALU or BPF_JMP, 4th bit encodes source operand ...

  BPF_K     0x00
  BPF_X     0x08

* in classic BPF, this means:
  
  BPF_SRC(code) == BPF_X - use register X as source operand
  BPF_SRC(code) == BPF_K - use 32-bit immediate as source operand

* in eBPF, this means:
  
  BPF_SRC(code) == BPF_X - use 'src_reg' register as source operand
  BPF_SRC(code) == BPF_K - use 32-bit immediate as source operand

... and four MSB bits store operation code.

If BPF_CLASS(code) == BPF_ALU or BPF_ALU64 [ in eBPF ], BPF_OP(code) is one of:

  BPF_ADD   0x00
  BPF_SUB   0x10
  BPF_MUL   0x20
  BPF_DIV   0x30
  BPF_OR    0x40
  BPF_AND   0x50
  BPF_LSH   0x60
  BPF_RSH   0x70
  BPF_NEG   0x80
  BPF_MOD   0x90
  BPF_XOR   0xa0
  BPF_MOV   0xb0  /* eBPF only: mov reg to reg */
  BPF_ARSH  0xc0  /* eBPF only: sign extending shift right */
  BPF_END   0xd0  /* eBPF only: endianness conversion */

If BPF_CLASS(code) == BPF_JMP or BPF_JMP32 [ in eBPF ], BPF_OP(code) is one of:

  BPF_JA    0x00  /* BPF_JMP only */
  BPF_JEQ   0x10
  BPF_JGT   0x20
  BPF_JGE   0x30
  BPF_JSET  0x40
  BPF_JNE   0x50  /* eBPF only: jump != */
  BPF_JSGT  0x60  /* eBPF only: signed '>' */
  BPF_JSGE  0x70  /* eBPF only: signed '>=' */
  BPF_CALL  0x80  /* eBPF BPF_JMP only: function call */
  BPF_EXIT  0x90  /* eBPF BPF_JMP only: function return */
  BPF_JLT   0xa0  /* eBPF only: unsigned '<' */
  BPF_JLE   0xb0  /* eBPF only: unsigned '<=' */
  BPF_JSLT  0xc0  /* eBPF only: signed '<' */
  BPF_JSLE  0xd0  /* eBPF only: signed '<=' */

So BPF_ADD | BPF_X | BPF_ALU means 32-bit addition in both classic BPF
and eBPF. There are only two registers in classic BPF, so it means A += X.
In eBPF it means dst_reg = (u32) dst_reg + (u32) src_reg; similarly,
BPF_XOR | BPF_K | BPF_ALU means A ^= imm32 in classic BPF and analogous
src_reg = (u32) src_reg ^ (u32) imm32 in eBPF.

Classic BPF is using BPF_MISC class to represent A = X and X = A moves.
eBPF is using BPF_MOV | BPF_X | BPF_ALU code instead. Since there are no
BPF_MISC operations in eBPF, the class 7 is used as BPF_ALU64 to mean
exactly the same operations as BPF_ALU, but with 64-bit wide operands
instead. So BPF_ADD | BPF_X | BPF_ALU64 means 64-bit addition, i.e.:
dst_reg = dst_reg + src_reg

Classic BPF wastes the whole BPF_RET class to represent a single 'ret'
operation. Classic BPF_RET | BPF_K means copy imm32 into return register
and perform function exit. eBPF is modeled to match CPU, so BPF_JMP | BPF_EXIT
in eBPF means function exit only. The eBPF program needs to store return
value into register R0 before doing a BPF_EXIT. Class 6 in eBPF is used as
BPF_JMP32 to mean exactly the same operations as BPF_JMP, but with 32-bit wide
operands for the comparisons instead.

For load and store instructions the 8-bit 'code' field is divided as:

  +--------+--------+-------------------+
  | 3 bits | 2 bits |   3 bits          |
  |  mode  |  size  | instruction class |
  +--------+--------+-------------------+
  (MSB)                             (LSB)

Size modifier is one of ...

  BPF_W   0x00    /* word */
  BPF_H   0x08    /* half word */
  BPF_B   0x10    /* byte */
  BPF_DW  0x18    /* eBPF only, double word */

... which encodes size of load/store operation:

 B  - 1 byte
 H  - 2 byte
 W  - 4 byte
 DW - 8 byte (eBPF only)

Mode modifier is one of:

  BPF_IMM  0x00  /* used for 32-bit mov in classic BPF and 64-bit in eBPF */
  BPF_ABS  0x20
  BPF_IND  0x40
  BPF_MEM  0x60
  BPF_LEN  0x80  /* classic BPF only, reserved in eBPF */
  BPF_MSH  0xa0  /* classic BPF only, reserved in eBPF */
  BPF_XADD 0xc0  /* eBPF only, exclusive add */

eBPF has two non-generic instructions: (BPF_ABS | <size> | BPF_LD) and
(BPF_IND | <size> | BPF_LD) which are used to access packet data.

They had to be carried over from classic to have strong performance of
socket filters running in eBPF interpreter. These instructions can only
be used when interpreter context is a pointer to 'struct sk_buff' and
have seven implicit operands. Register R6 is an implicit input that must
contain pointer to sk_buff. Register R0 is an implicit output which contains
the data fetched from the packet. Registers R1-R5 are scratch registers
and must not be used to store the data across BPF_ABS | BPF_LD or
BPF_IND | BPF_LD instructions.

These instructions have implicit program exit condition as well. When
eBPF program is trying to access the data beyond the packet boundary,
the interpreter will abort the execution of the program. JIT compilers
therefore must preserve this property. src_reg and imm32 fields are
explicit inputs to these instructions.

For example:

  BPF_IND | BPF_W | BPF_LD means:

    R0 = ntohl(*(u32 *) (((struct sk_buff *) R6)->data + src_reg + imm32))
    and R1 - R5 were scratched.

Unlike classic BPF instruction set, eBPF has generic load/store operations:

BPF_MEM | <size> | BPF_STX:  *(size *) (dst_reg + off) = src_reg
BPF_MEM | <size> | BPF_ST:   *(size *) (dst_reg + off) = imm32
BPF_MEM | <size> | BPF_LDX:  dst_reg = *(size *) (src_reg + off)
BPF_XADD | BPF_W  | BPF_STX: lock xadd *(u32 *)(dst_reg + off16) += src_reg
BPF_XADD | BPF_DW | BPF_STX: lock xadd *(u64 *)(dst_reg + off16) += src_reg

Where size is one of: BPF_B or BPF_H or BPF_W or BPF_DW. Note that 1 and
2 byte atomic increments are not supported.

eBPF has one 16-byte instruction: BPF_LD | BPF_DW | BPF_IMM which consists
of two consecutive 'struct bpf_insn' 8-byte blocks and interpreted as single
instruction that loads 64-bit immediate value into a dst_reg.
Classic BPF has similar instruction: BPF_LD | BPF_W | BPF_IMM which loads
32-bit immediate value into a register.

eBPF verifier
-------------

The safety of the eBPF program is determined in two steps.

First step does DAG check to disallow loops and other CFG validation.
In particular it will detect programs that have unreachable instructions.
(though classic BPF checker allows them)

Second step starts from the first insn and descends all possible paths.
It simulates execution of every insn and observes the state change of
registers and stack.

At the start of the program the register R1 contains a pointer to context
and has type PTR_TO_CTX.
If verifier sees an insn that does R2=R1, then R2 has now type
PTR_TO_CTX as well and can be used on the right hand side of expression.
If R1=PTR_TO_CTX and insn is R2=R1+R1, then R2=SCALAR_VALUE,
since addition of two valid pointers makes invalid pointer.
(In 'secure' mode verifier will reject any type of pointer arithmetic to make
sure that kernel addresses don't leak to unprivileged users)

If register was never written to, it's not readable:
  bpf_mov R0 = R2
  bpf_exit
will be rejected, since R2 is unreadable at the start of the program.

After kernel function call, R1-R5 are reset to unreadable and
R0 has a return type of the function.

Since R6-R9 are callee saved, their state is preserved across the call.
  bpf_mov R6 = 1
  bpf_call foo
  bpf_mov R0 = R6
  bpf_exit
is a correct program. If there was R1 instead of R6, it would have
been rejected.

load/store instructions are allowed only with registers of valid types, which
are PTR_TO_CTX, PTR_TO_MAP, PTR_TO_STACK. They are bounds and alignment checked.
For example:
 bpf_mov R1 = 1
 bpf_mov R2 = 2
 bpf_xadd *(u32 *)(R1 + 3) += R2
 bpf_exit
will be rejected, since R1 doesn't have a valid pointer type at the time of
execution of instruction bpf_xadd.

At the start R1 type is PTR_TO_CTX (a pointer to generic 'struct bpf_context')
A callback is used to customize verifier to restrict eBPF program access to only
certain fields within ctx structure with specified size and alignment.

For example, the following insn:
  bpf_ld R0 = *(u32 *)(R6 + 8)
intends to load a word from address R6 + 8 and store it into R0
If R6=PTR_TO_CTX, via is_valid_access() callback the verifier will know
that offset 8 of size 4 bytes can be accessed for reading, otherwise
the verifier will reject the program.
If R6=PTR_TO_STACK, then access should be aligned and be within
stack bounds, which are [-MAX_BPF_STACK, 0). In this example offset is 8,
so it will fail verification, since it's out of bounds.

The verifier will allow eBPF program to read data from stack only after
it wrote into it.
Classic BPF verifier does similar check with M[0-15] memory slots.
For example:
  bpf_ld R0 = *(u32 *)(R10 - 4)
  bpf_exit
is invalid program.
Though R10 is correct read-only register and has type PTR_TO_STACK
and R10 - 4 is within stack bounds, there were no stores into that location.

Pointer register spill/fill is tracked as well, since four (R6-R9)
callee saved registers may not be enough for some programs.

Allowed function calls are customized with bpf_verifier_ops->get_func_proto()
The eBPF verifier will check that registers match argument constraints.
After the call register R0 will be set to return type of the function.

Function calls is a main mechanism to extend functionality of eBPF programs.
Socket filters may let programs to call one set of functions, whereas tracing
filters may allow completely different set.

If a function made accessible to eBPF program, it needs to be thought through
from safety point of view. The verifier will guarantee that the function is
called with valid arguments.

seccomp vs socket filters have different security restrictions for classic BPF.
Seccomp solves this by two stage verifier: classic BPF verifier is followed
by seccomp verifier. In case of eBPF one configurable verifier is shared for
all use cases.

See details of eBPF verifier in kernel/bpf/verifier.c

Register value tracking
-----------------------

In order to determine the safety of an eBPF program, the verifier must track
the range of possible values in each register and also in each stack slot.
This is done with 'struct bpf_reg_state', defined in include/linux/
bpf_verifier.h, which unifies tracking of scalar and pointer values.  Each
register state has a type, which is either NOT_INIT (the register has not been
written to), SCALAR_VALUE (some value which is not usable as a pointer), or a
pointer type.  The types of pointers describe their base, as follows:
    PTR_TO_CTX          Pointer to bpf_context.
    CONST_PTR_TO_MAP    Pointer to struct bpf_map.  "Const" because arithmetic
                        on these pointers is forbidden.
    PTR_TO_MAP_VALUE    Pointer to the value stored in a map element.
    PTR_TO_MAP_VALUE_OR_NULL
                        Either a pointer to a map value, or NULL; map accesses
                        (see section 'eBPF maps', below) return this type,
                        which becomes a PTR_TO_MAP_VALUE when checked != NULL.
                        Arithmetic on these pointers is forbidden.
    PTR_TO_STACK        Frame pointer.
    PTR_TO_PACKET       skb->data.
    PTR_TO_PACKET_END   skb->data + headlen; arithmetic forbidden.
    PTR_TO_SOCKET       Pointer to struct bpf_sock_ops, implicitly refcounted.
    PTR_TO_SOCKET_OR_NULL
                        Either a pointer to a socket, or NULL; socket lookup
                        returns this type, which becomes a PTR_TO_SOCKET when
                        checked != NULL. PTR_TO_SOCKET is reference-counted,
                        so programs must release the reference through the
                        socket release function before the end of the program.
                        Arithmetic on these pointers is forbidden.
However, a pointer may be offset from this base (as a result of pointer
arithmetic), and this is tracked in two parts: the 'fixed offset' and 'variable
offset'.  The former is used when an exactly-known value (e.g. an immediate
operand) is added to a pointer, while the latter is used for values which are
not exactly known.  The variable offset is also used in SCALAR_VALUEs, to track
the range of possible values in the register.
The verifier's knowledge about the variable offset consists of:

* minimum and maximum values as unsigned
* minimum and maximum values as signed
* knowledge of the values of individual bits, in the form of a 'tnum': a u64
  'mask' and a u64 'value'.  1s in the mask represent bits whose value is unknown;
  1s in the value represent bits known to be 1.  Bits known to be 0 have 0 in both
  mask and value; no bit should ever be 1 in both.  For example, if a byte is read
  into a register from memory, the register's top 56 bits are known zero, while
  the low 8 are unknown - which is represented as the tnum (0x0; 0xff).  If we
  then OR this with 0x40, we get (0x40; 0xbf), then if we add 1 we get (0x0;
  0x1ff), because of potential carries.

Besides arithmetic, the register state can also be updated by conditional
branches.  For instance, if a SCALAR_VALUE is compared > 8, in the 'true' branch
it will have a umin_value (unsigned minimum value) of 9, whereas in the 'false'
branch it will have a umax_value of 8.  A signed compare (with BPF_JSGT or
BPF_JSGE) would instead update the signed minimum/maximum values.  Information
from the signed and unsigned bounds can be combined; for instance if a value is
first tested < 8 and then tested s> 4, the verifier will conclude that the value
is also > 4 and s< 8, since the bounds prevent crossing the sign boundary.

PTR_TO_PACKETs with a variable offset part have an 'id', which is common to all
pointers sharing that same variable offset.  This is important for packet range
checks: after adding a variable to a packet pointer register A, if you then copy
it to another register B and then add a constant 4 to A, both registers will
share the same 'id' but the A will have a fixed offset of +4.  Then if A is
bounds-checked and found to be less than a PTR_TO_PACKET_END, the register B is
now known to have a safe range of at least 4 bytes.  See 'Direct packet access',
below, for more on PTR_TO_PACKET ranges.

The 'id' field is also used on PTR_TO_MAP_VALUE_OR_NULL, common to all copies of
the pointer returned from a map lookup.  This means that when one copy is
checked and found to be non-NULL, all copies can become PTR_TO_MAP_VALUEs.
As well as range-checking, the tracked information is also used for enforcing
alignment of pointer accesses.  For instance, on most systems the packet pointer
is 2 bytes after a 4-byte alignment.  If a program adds 14 bytes to that to jump
over the Ethernet header, then reads IHL and addes (IHL * 4), the resulting
pointer will have a variable offset known to be 4n+2 for some n, so adding the 2
bytes (NET_IP_ALIGN) gives a 4-byte alignment and so word-sized accesses through
that pointer are safe.
The 'id' field is also used on PTR_TO_SOCKET and PTR_TO_SOCKET_OR_NULL, common
to all copies of the pointer returned from a socket lookup. This has similar
behaviour to the handling for PTR_TO_MAP_VALUE_OR_NULL->PTR_TO_MAP_VALUE, but
it also handles reference tracking for the pointer. PTR_TO_SOCKET implicitly
represents a reference to the corresponding 'struct sock'. To ensure that the
reference is not leaked, it is imperative to NULL-check the reference and in
the non-NULL case, and pass the valid reference to the socket release function.

Direct packet access
--------------------

In cls_bpf and act_bpf programs the verifier allows direct access to the packet
data via skb->data and skb->data_end pointers.
Ex:
1:  r4 = *(u32 *)(r1 +80)  /* load skb->data_end */
2:  r3 = *(u32 *)(r1 +76)  /* load skb->data */
3:  r5 = r3
4:  r5 += 14
5:  if r5 > r4 goto pc+16
R1=ctx R3=pkt(id=0,off=0,r=14) R4=pkt_end R5=pkt(id=0,off=14,r=14) R10=fp
6:  r0 = *(u16 *)(r3 +12) /* access 12 and 13 bytes of the packet */

this 2byte load from the packet is safe to do, since the program author
did check 'if (skb->data + 14 > skb->data_end) goto err' at insn #5 which
means that in the fall-through case the register R3 (which points to skb->data)
has at least 14 directly accessible bytes. The verifier marks it
as R3=pkt(id=0,off=0,r=14).
id=0 means that no additional variables were added to the register.
off=0 means that no additional constants were added.
r=14 is the range of safe access which means that bytes [R3, R3 + 14) are ok.
Note that R5 is marked as R5=pkt(id=0,off=14,r=14). It also points
to the packet data, but constant 14 was added to the register, so
it now points to 'skb->data + 14' and accessible range is [R5, R5 + 14 - 14)
which is zero bytes.

More complex packet access may look like:
 R0=inv1 R1=ctx R3=pkt(id=0,off=0,r=14) R4=pkt_end R5=pkt(id=0,off=14,r=14) R10=fp
 6:  r0 = *(u8 *)(r3 +7) /* load 7th byte from the packet */
 7:  r4 = *(u8 *)(r3 +12)
 8:  r4 *= 14
 9:  r3 = *(u32 *)(r1 +76) /* load skb->data */
10:  r3 += r4
11:  r2 = r1
12:  r2 <<= 48
13:  r2 >>= 48
14:  r3 += r2
15:  r2 = r3
16:  r2 += 8
17:  r1 = *(u32 *)(r1 +80) /* load skb->data_end */
18:  if r2 > r1 goto pc+2
 R0=inv(id=0,umax_value=255,var_off=(0x0; 0xff)) R1=pkt_end R2=pkt(id=2,off=8,r=8) R3=pkt(id=2,off=0,r=8) R4=inv(id=0,umax_value=3570,var_off=(0x0; 0xfffe)) R5=pkt(id=0,off=14,r=14) R10=fp
19:  r1 = *(u8 *)(r3 +4)
The state of the register R3 is R3=pkt(id=2,off=0,r=8)
id=2 means that two 'r3 += rX' instructions were seen, so r3 points to some
offset within a packet and since the program author did
'if (r3 + 8 > r1) goto err' at insn #18, the safe range is [R3, R3 + 8).
The verifier only allows 'add'/'sub' operations on packet registers. Any other
operation will set the register state to 'SCALAR_VALUE' and it won't be
available for direct packet access.
Operation 'r3 += rX' may overflow and become less than original skb->data,
therefore the verifier has to prevent that.  So when it sees 'r3 += rX'
instruction and rX is more than 16-bit value, any subsequent bounds-check of r3
against skb->data_end will not give us 'range' information, so attempts to read
through the pointer will give "invalid access to packet" error.
Ex. after insn 'r4 = *(u8 *)(r3 +12)' (insn #7 above) the state of r4 is
R4=inv(id=0,umax_value=255,var_off=(0x0; 0xff)) which means that upper 56 bits
of the register are guaranteed to be zero, and nothing is known about the lower
8 bits. After insn 'r4 *= 14' the state becomes
R4=inv(id=0,umax_value=3570,var_off=(0x0; 0xfffe)), since multiplying an 8-bit
value by constant 14 will keep upper 52 bits as zero, also the least significant
bit will be zero as 14 is even.  Similarly 'r2 >>= 48' will make
R2=inv(id=0,umax_value=65535,var_off=(0x0; 0xffff)), since the shift is not sign
extending.  This logic is implemented in adjust_reg_min_max_vals() function,
which calls adjust_ptr_min_max_vals() for adding pointer to scalar (or vice
versa) and adjust_scalar_min_max_vals() for operations on two scalars.

The end result is that bpf program author can access packet directly
using normal C code as:
  void *data = (void *)(long)skb->data;
  void *data_end = (void *)(long)skb->data_end;
  struct eth_hdr *eth = data;
  struct iphdr *iph = data + sizeof(*eth);
  struct udphdr *udp = data + sizeof(*eth) + sizeof(*iph);

  if (data + sizeof(*eth) + sizeof(*iph) + sizeof(*udp) > data_end)
          return 0;
  if (eth->h_proto != htons(ETH_P_IP))
          return 0;
  if (iph->protocol != IPPROTO_UDP || iph->ihl != 5)
          return 0;
  if (udp->dest == 53 || udp->source == 9)
          ...;
which makes such programs easier to write comparing to LD_ABS insn
and significantly faster.

eBPF maps
---------

'maps' is a generic storage of different types for sharing data between kernel
and userspace.

The maps are accessed from user space via BPF syscall, which has commands:

- create a map with given type and attributes
  map_fd = bpf(BPF_MAP_CREATE, union bpf_attr *attr, u32 size)
  using attr->map_type, attr->key_size, attr->value_size, attr->max_entries
  returns process-local file descriptor or negative error

- lookup key in a given map
  err = bpf(BPF_MAP_LOOKUP_ELEM, union bpf_attr *attr, u32 size)
  using attr->map_fd, attr->key, attr->value
  returns zero and stores found elem into value or negative error

- create or update key/value pair in a given map
  err = bpf(BPF_MAP_UPDATE_ELEM, union bpf_attr *attr, u32 size)
  using attr->map_fd, attr->key, attr->value
  returns zero or negative error

- find and delete element by key in a given map
  err = bpf(BPF_MAP_DELETE_ELEM, union bpf_attr *attr, u32 size)
  using attr->map_fd, attr->key

- to delete map: close(fd)
  Exiting process will delete maps automatically

userspace programs use this syscall to create/access maps that eBPF programs
are concurrently updating.

maps can have different types: hash, array, bloom filter, radix-tree, etc.

The map is defined by:
  . type
  . max number of elements
  . key size in bytes
  . value size in bytes

Pruning
-------

The verifier does not actually walk all possible paths through the program.  For
each new branch to analyse, the verifier looks at all the states it's previously
been in when at this instruction.  If any of them contain the current state as a
subset, the branch is 'pruned' - that is, the fact that the previous state was
accepted implies the current state would be as well.  For instance, if in the
previous state, r1 held a packet-pointer, and in the current state, r1 holds a
packet-pointer with a range as long or longer and at least as strict an
alignment, then r1 is safe.  Similarly, if r2 was NOT_INIT before then it can't
have been used by any path from that point, so any value in r2 (including
another NOT_INIT) is safe.  The implementation is in the function regsafe().
Pruning considers not only the registers but also the stack (and any spilled
registers it may hold).  They must all be safe for the branch to be pruned.
This is implemented in states_equal().

Understanding eBPF verifier messages
------------------------------------

The following are few examples of invalid eBPF programs and verifier error
messages as seen in the log:

Program with unreachable instructions:
static struct bpf_insn prog[] = {
  BPF_EXIT_INSN(),
  BPF_EXIT_INSN(),
};
Error:
  unreachable insn 1

Program that reads uninitialized register:
  BPF_MOV64_REG(BPF_REG_0, BPF_REG_2),
  BPF_EXIT_INSN(),
Error:
  0: (bf) r0 = r2
  R2 !read_ok

Program that doesn't initialize R0 before exiting:
  BPF_MOV64_REG(BPF_REG_2, BPF_REG_1),
  BPF_EXIT_INSN(),
Error:
  0: (bf) r2 = r1
  1: (95) exit
  R0 !read_ok

Program that accesses stack out of bounds:
  BPF_ST_MEM(BPF_DW, BPF_REG_10, 8, 0),
  BPF_EXIT_INSN(),
Error:
  0: (7a) *(u64 *)(r10 +8) = 0
  invalid stack off=8 size=8

Program that doesn't initialize stack before passing its address into function:
  BPF_MOV64_REG(BPF_REG_2, BPF_REG_10),
  BPF_ALU64_IMM(BPF_ADD, BPF_REG_2, -8),
  BPF_LD_MAP_FD(BPF_REG_1, 0),
  BPF_RAW_INSN(BPF_JMP | BPF_CALL, 0, 0, 0, BPF_FUNC_map_lookup_elem),
  BPF_EXIT_INSN(),
Error:
  0: (bf) r2 = r10
  1: (07) r2 += -8
  2: (b7) r1 = 0x0
  3: (85) call 1
  invalid indirect read from stack off -8+0 size 8

Program that uses invalid map_fd=0 while calling to map_lookup_elem() function:
  BPF_ST_MEM(BPF_DW, BPF_REG_10, -8, 0),
  BPF_MOV64_REG(BPF_REG_2, BPF_REG_10),
  BPF_ALU64_IMM(BPF_ADD, BPF_REG_2, -8),
  BPF_LD_MAP_FD(BPF_REG_1, 0),
  BPF_RAW_INSN(BPF_JMP | BPF_CALL, 0, 0, 0, BPF_FUNC_map_lookup_elem),
  BPF_EXIT_INSN(),
Error:
  0: (7a) *(u64 *)(r10 -8) = 0
  1: (bf) r2 = r10
  2: (07) r2 += -8
  3: (b7) r1 = 0x0
  4: (85) call 1
  fd 0 is not pointing to valid bpf_map

Program that doesn't check return value of map_lookup_elem() before accessing
map element:
  BPF_ST_MEM(BPF_DW, BPF_REG_10, -8, 0),
  BPF_MOV64_REG(BPF_REG_2, BPF_REG_10),
  BPF_ALU64_IMM(BPF_ADD, BPF_REG_2, -8),
  BPF_LD_MAP_FD(BPF_REG_1, 0),
  BPF_RAW_INSN(BPF_JMP | BPF_CALL, 0, 0, 0, BPF_FUNC_map_lookup_elem),
  BPF_ST_MEM(BPF_DW, BPF_REG_0, 0, 0),
  BPF_EXIT_INSN(),
Error:
  0: (7a) *(u64 *)(r10 -8) = 0
  1: (bf) r2 = r10
  2: (07) r2 += -8
  3: (b7) r1 = 0x0
  4: (85) call 1
  5: (7a) *(u64 *)(r0 +0) = 0
  R0 invalid mem access 'map_value_or_null'

Program that correctly checks map_lookup_elem() returned value for NULL, but
accesses the memory with incorrect alignment:
  BPF_ST_MEM(BPF_DW, BPF_REG_10, -8, 0),
  BPF_MOV64_REG(BPF_REG_2, BPF_REG_10),
  BPF_ALU64_IMM(BPF_ADD, BPF_REG_2, -8),
  BPF_LD_MAP_FD(BPF_REG_1, 0),
  BPF_RAW_INSN(BPF_JMP | BPF_CALL, 0, 0, 0, BPF_FUNC_map_lookup_elem),
  BPF_JMP_IMM(BPF_JEQ, BPF_REG_0, 0, 1),
  BPF_ST_MEM(BPF_DW, BPF_REG_0, 4, 0),
  BPF_EXIT_INSN(),
Error:
  0: (7a) *(u64 *)(r10 -8) = 0
  1: (bf) r2 = r10
  2: (07) r2 += -8
  3: (b7) r1 = 1
  4: (85) call 1
  5: (15) if r0 == 0x0 goto pc+1
   R0=map_ptr R10=fp
  6: (7a) *(u64 *)(r0 +4) = 0
  misaligned access off 4 size 8

Program that correctly checks map_lookup_elem() returned value for NULL and
accesses memory with correct alignment in one side of 'if' branch, but fails
to do so in the other side of 'if' branch:
  BPF_ST_MEM(BPF_DW, BPF_REG_10, -8, 0),
  BPF_MOV64_REG(BPF_REG_2, BPF_REG_10),
  BPF_ALU64_IMM(BPF_ADD, BPF_REG_2, -8),
  BPF_LD_MAP_FD(BPF_REG_1, 0),
  BPF_RAW_INSN(BPF_JMP | BPF_CALL, 0, 0, 0, BPF_FUNC_map_lookup_elem),
  BPF_JMP_IMM(BPF_JEQ, BPF_REG_0, 0, 2),
  BPF_ST_MEM(BPF_DW, BPF_REG_0, 0, 0),
  BPF_EXIT_INSN(),
  BPF_ST_MEM(BPF_DW, BPF_REG_0, 0, 1),
  BPF_EXIT_INSN(),
Error:
  0: (7a) *(u64 *)(r10 -8) = 0
  1: (bf) r2 = r10
  2: (07) r2 += -8
  3: (b7) r1 = 1
  4: (85) call 1
  5: (15) if r0 == 0x0 goto pc+2
   R0=map_ptr R10=fp
  6: (7a) *(u64 *)(r0 +0) = 0
  7: (95) exit

  from 5 to 8: R0=imm0 R10=fp
  8: (7a) *(u64 *)(r0 +0) = 1
  R0 invalid mem access 'imm'

Program that performs a socket lookup then sets the pointer to NULL without
checking it:
value:
  BPF_MOV64_IMM(BPF_REG_2, 0),
  BPF_STX_MEM(BPF_W, BPF_REG_10, BPF_REG_2, -8),
  BPF_MOV64_REG(BPF_REG_2, BPF_REG_10),
  BPF_ALU64_IMM(BPF_ADD, BPF_REG_2, -8),
  BPF_MOV64_IMM(BPF_REG_3, 4),
  BPF_MOV64_IMM(BPF_REG_4, 0),
  BPF_MOV64_IMM(BPF_REG_5, 0),
  BPF_EMIT_CALL(BPF_FUNC_sk_lookup_tcp),
  BPF_MOV64_IMM(BPF_REG_0, 0),
  BPF_EXIT_INSN(),
Error:
  0: (b7) r2 = 0
  1: (63) *(u32 *)(r10 -8) = r2
  2: (bf) r2 = r10
  3: (07) r2 += -8
  4: (b7) r3 = 4
  5: (b7) r4 = 0
  6: (b7) r5 = 0
  7: (85) call bpf_sk_lookup_tcp#65
  8: (b7) r0 = 0
  9: (95) exit
  Unreleased reference id=1, alloc_insn=7

Program that performs a socket lookup but does not NULL-check the returned
value:
  BPF_MOV64_IMM(BPF_REG_2, 0),
  BPF_STX_MEM(BPF_W, BPF_REG_10, BPF_REG_2, -8),
  BPF_MOV64_REG(BPF_REG_2, BPF_REG_10),
  BPF_ALU64_IMM(BPF_ADD, BPF_REG_2, -8),
  BPF_MOV64_IMM(BPF_REG_3, 4),
  BPF_MOV64_IMM(BPF_REG_4, 0),
  BPF_MOV64_IMM(BPF_REG_5, 0),
  BPF_EMIT_CALL(BPF_FUNC_sk_lookup_tcp),
  BPF_EXIT_INSN(),
Error:
  0: (b7) r2 = 0
  1: (63) *(u32 *)(r10 -8) = r2
  2: (bf) r2 = r10
  3: (07) r2 += -8
  4: (b7) r3 = 4
  5: (b7) r4 = 0
  6: (b7) r5 = 0
  7: (85) call bpf_sk_lookup_tcp#65
  8: (95) exit
  Unreleased reference id=1, alloc_insn=7

Testing
-------

Next to the BPF toolchain, the kernel also ships a test module that contains
various test cases for classic and internal BPF that can be executed against
the BPF interpreter and JIT compiler. It can be found in lib/test_bpf.c and
enabled via Kconfig:

  CONFIG_TEST_BPF=m

After the module has been built and installed, the test suite can be executed
via insmod or modprobe against 'test_bpf' module. Results of the test cases
including timings in nsec can be found in the kernel log (dmesg).

Misc
----

Also trinity, the Linux syscall fuzzer, has built-in support for BPF and
SECCOMP-BPF kernel fuzzing.
