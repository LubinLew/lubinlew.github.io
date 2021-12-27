# CPU使用率计算

## 使用命令查看CPU使用率

```bash
top

iostat -c

mpstat -P ALL

sar -P ALL
```

## 基础知识

上面的命令都是通过解析 **/proc/stat** 文件来获取CPU的信息

```bash
[user]# cat /proc/stat 
cpu  174859 105 201714 893787421 4146 0 1594 23356 0 0
cpu0 18193 10 21644 111744831 344 0 461 2340 0 0
cpu1 23700 12 28238 111695548 662 0 258 3846 0 0
cpu2 21326 10 25134 111725076 701 0 194 2961 0 0
cpu3 19881 20 23611 111725883 377 0 157 2833 0 0
cpu4 21014 17 24709 111729214 323 0 130 2670 0 0
cpu5 22062 21 26171 111718130 869 0 158 3211 0 0
cpu6 18678 8 22038 111740210 259 0 108 2341 0 0
cpu7 30002 5 30167 111708526 607 0 125 3152 0 0
intr 163613326 116 10 0 0 2173 0 3 0 0 0 111813 35 15 0 0 0 0 0 0 0 0 0 0 0 0 159179 0 1834627 3 100631 4 79366 3 599766 3 2233999 3 176678 17 1150171 2 4629842 39 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
ctxt 222790035
btime 1609298736
processes 11217
procs_running 1
procs_blocked 0
softirq 108317383 2 48767867 4 11157861 161058 0 32 32906980 0 15323579
```

可以看到第一行是CPU汇总, 之后每行是各个CPU单独的统计计数。
这些数值的单位都是 `时钟嘀嗒数`(时间单位)。

> jiffies 是Linux内核中的时间子系统，记录了系统启动以来，经过了多少时钟嘀嗒数。
> 
> 1个时钟嘀嗒数是 `sysconf(_SC_CLK_TCK)`(通常是100) 分之1秒。
> 
> https://www.cnblogs.com/arnoldlu/p/7234443.html
> 
> https://stackoverflow.com/questions/17410841/how-does-user-hz-solve-the-jiffy-scaling-issue

### 各字段说明

| 顺序  | 名称         | 说明                                                            | top对应字段 |
| --- | ---------- | ------------------------------------------------------------- | ------- |
| 1   | user       | 从系统启动开始累计到当前时刻，处于用户态的运行时间，不包含 nice 值为负的进程                     | us      |
| 2   | nice       | 从系统启动开始累计到当前时刻，nice值为负的进程所占用的CPU时间                            | ni      |
| 3   | system     | 从系统启动开始累计到当前时刻，处于内核态的运行时间                                     | sy      |
| 4   | idle       | 从系统启动开始累计到当前时刻，除IO等待时间以外的其它等待时间                               | id      |
| 5   | iowait     | 从系统启动开始累计到当前时刻，IO等待时间                                         | wa      |
| 6   | irq        | 从系统启动开始累计到当前时刻，硬中断处理时间                                        | hi      |
| 7   | softirq    | 从系统启动开始累计到当前时刻，软中断处理时间                                        | si      |
| 8   | steal      | 丢失时间,被强制等待（involuntary wait）虚拟CPU的时间，此时hypervisor在为另一个虚拟处理器服务 | st      |
| 9   | guest      | 为在 Linux 内核控制下的客户操作系统运行虚拟 CPU 所花费的时间                          | -       |
| 10  | guest_nice | 运行 niced guest（Linux 内核控制下的guest操作系统的虚拟CPU）所花费的时间             | -       |

可以看出所有字段的值都是从系统启动开始的累计值。

### CPU数量获取

CPU的数量可以从文件中解析出来, 但是没有提前获取来的便捷。实用 sysconf 函数来获取CPU数量

| 参数                   | 说明                                                     | 备注  |
| -------------------- | ------------------------------------------------------ | --- |
| _SC_NPROCESSORS_CONF | The number of processors configured                    |     |
| _SC_NPROCESSORS_ONLN | The number of processors currently online (available). |     |

> GNU C Library 也提供了获取 CPU 个数的 API:
> 
> | API接口                         | 等价于                           |
> | ----------------------------- | ----------------------------- |
> | int **get_nprocs_conf**(void) | sysconf(_SC_NPROCESSORS_CONF) |
> | int **get_nprocs**(void)      | sysconf(_SC_NPROCESSORS_ONLN) |
> 
> ```c
> #include <stdio.h>
> #include <sys/sysinfo.h>
> 
> int main(void) {
>     printf("configured cpu num is %d\n", get_nprocs_conf());
>     printf("currently online num  %d\n", get_nprocs());
>     return 0;
> }
> ```

#### 某些CPU禁用的情况

##### 从 grub 命令行禁用CPU

> [linux - How to disable one CPU - Unix &amp; Linux Stack Exchange](https://unix.stackexchange.com/questions/417672/how-to-disable-one-cpu)
> 
> [boot - How can I tell if my system was booted as EFI/UEFI or BIOS? - Ask Ubuntu](https://askubuntu.com/questions/162564/how-can-i-tell-if-my-system-was-booted-as-efi-uefi-or-bios)
> 
> [How to update Grub on RHEL or CentOS Linux - nixCraft Linux/Unix Forum](https://www.nixcraft.com/t/how-to-update-grub-on-rhel-or-centos-linux/3824/2)

测试机器为 4 核的虚拟机, 修改 `/etc/default/grub`, 在 `GRUB_CMDLINE_LINUX_DEFAULT` 后增加 `isolcpus=3`, 这样就禁用了第四个CPU。

> 这样只是进制用户进程使用第四个CPU, 内核仍然可以使用

之后更新 grub 然后重启

> 首先需要知道系统是 BIOS 还是 UEFI 启动, 在命令行运行下面命令
> 
> ```bash
> [ -d /sys/firmware/efi ] && echo UEFI || echo BIOS
> ```
> 
> BIOS:
> 
> ```bash
> grub2-mkconfig -o /boot/grub2/grub.cfg
> ```
> 
> UEFI:
> 
> ```bash
> grub2-mkconfig -o /boot/efi/EFI/redhat/grub.cfg
> ```

重启后的 `/proc/stat` 文件中CPU部分内容:

```txt
cpu  448 0 768 684187 22 0 34 0 0 0
cpu0 177 0 311 170771 5 0 5 0 0 0
cpu1 133 0 191 171055 6 0 1 0 0 0
cpu2 137 0 263 170937 11 0 26 0 0 0
cpu3 0 0 2 171423 0 0 0 0 0 0
```

发现被禁用的 CPU3 仍然在, 并且是在线的, 这说明这样禁用 CPU 与 online 的设置不匹配

```bash
cat /sys/devices/system/cpu/cpu3/online
1
```

##### 设置 CPU offline

> [linux - Programmatically disable CPU core - Stack Overflow](https://stackoverflow.com/questions/44907731/programmatically-disable-cpu-core)

首先可以查看一下运行在CPU3上的进程

```bash
ps -eo pid,cmd,psr | awk '{if ($3==3) print $1,$2}'
```

> psr 属性可以获取进程所有的CPU
> 
> taskset 命令可以指定进程运行在哪个CPU上

之后设置 CPU3 offline

```bash
echo 0 > /sys/devices/system/cpu/cpu3/online
```

这时查看 `/proc/stat` 文件, 发现CPU3不见了, 这说明 `/proc/stat` 文件中的 CPU 与 `_SC_NPROCESSORS_ONLN` 是一致的。

```txt
cpu  450 0 796 888380 23 0 35 0 0 0
cpu0 178 0 333 226568 6 0 6 0 0 0
cpu1 133 0 192 226882 6 0 1 0 0 0
cpu2 138 0 268 226762 11 0 27 0 0 0
```

但是这个设置只是告诉内核忽略指定的CPU, 但是之前绑定在这个CPU的进程仍然继续在其上运行, 使用上面的 ps 命令发现CPU3上仍然还和原来一样还是有很多进程运行在其上。

> 所以监测 CPU online 数量变化也很重要
> 
> CPU也支持热插拔 [CPU hotplug in the Kernel](https://www.kernel.org/doc/html/latest/core-api/cpu_hotplug.html)



## 使用率计算

10个字段加在一起为 CPU 使用的总时间。选择2个时间点 T1 和 T2（T2 > T1）, 

使用 T2 的CPU总时间 减去 T1 的CPU总时间 等于T1和T2 时间间隔内总CPU使用的时间。

使用 T2 的idle时间 减去 T1 的idle时间 等于T1和T2 时间间隔内CPU空闲的时间。

```bash
CPU的空闲率 = ( (T2.idle - T1.idle) / (T2.sum - T1.sum) ) * 100%
CPU的使用率 = 1 - CPU的空闲率
```





## References

[Linux中计算特定CPU使用率_ibless的博客-CSDN博客_cpu使用率](https://blog.csdn.net/ibless/article/details/85177175)

[Linux CPU利用率计算原理及内核实现 - Q_Quan - 博客园](https://www.cnblogs.com/qquan/articles/4686422.html)

[LINUX CPU利用率计算_浮白-CSDN博客_linux计算cpu使用率](https://blog.csdn.net/turkeyzhou/article/details/6709953)

[Accurate calculation of CPU usage given in percentage in Linux? - Stack Overflow](https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux)

[Get Overall CPU Usage on Linux | Baeldung on Linux](https://www.baeldung.com/linux/get-cpu-usage)
