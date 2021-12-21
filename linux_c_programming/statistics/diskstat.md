# 磁盘I/O

> https://www.kernel.org/doc/Documentation/iostats.txt

从2.4.20 和 2.5.45版本开始,引入了更多的磁盘统计信息来帮助我们检测磁盘的活动。一些工具如 `sar` 和 `iostat`等都是使用这些信息完成其功能。

在2.4版本这些信息展示在 `/proc/partitions` 文件中。在2.6+版本这些信息可以在两个地方找到， 一个是 `/proc/diskstats` 文件；另一个是 `sysfs` 文件系统中(sysfs文件系统必需挂载后才能获取信息，挂载的路径是`/sys`)。

如果您正在查看一组已知的少量磁盘信息，则选择 `sysfs` 效果很好。 如果您正在查看大量磁盘，`/proc/diskstats` 是更好的选择，因为这将避免打开大量的磁盘信息快照文件(`/sys/dev/block/*/stat`)。

```txt
   2.4:
      3     0   39082680 hda 446216 784926 9550688 4382310 424847 312726 5922052 19310380 0 3376340 23705160
      3     1    9221278 hda1 35486 0 35496 38030 0 0 0 0 0 38030 38030

   2.6+ sysfs:
      446216 784926 9550688 4382310 424847 312726 5922052 19310380 0 3376340 23705160
      35486    38030    38030    38030

   2.6+ diskstats:
      3    0   hda 446216 784926 9550688 4382310 424847 312726 5922052 19310380 0 3376340 23705160
      3    1   hda1 35486 38030 38030 38030

   4.18+ diskstats:
      3    0   hda 446216 784926 9550688 4382310 424847 312726 5922052 19310380 0 3376340 23705160 0 0 0 0
```

上面例子中, 2.4 版本的统计信息在设备名后,第一个的字段是 446216, 相比之下 2.6+ 的 `sysfs` 直接展示11个统计字段，`/proc/diskstats` 展示了主设备号、次设备号、设备名 之后才是 11个统计字段。每种格式都是11个统计字段，并且意义相同。4.18+ 又在后面增加了4个字段。

除了字段 9 之外的所有字段都是自启动以来累积的。字段 9 应该 当 I/O 完成时归零；所有其他字段只会增加（除非溢出和wrap）。这些是（32 位或 64 位）unsigned long （本机字大小）数字，并且在非常繁忙或长期存在的系统上，它们可能wrap（包裹）。应用程序应该准备好处理这个问题；除非 你的观察是以大量的分钟或小时来衡量的， 在您注意到它们之前，它们不应包裹两次。

每组统计信息仅适用于指定的设备；如果你想系统范围的统计信息，您必须找到所有设备并将它们汇总起来。

| 字段        |                                           |     |
| --------- | ----------------------------------------- | --- |
| Field  1  | reads completed                           |     |
| Field  2  | reads merged                              |     |
| Field  3  | sectors read                              |     |
| Field  4  | milliseconds spent reading                |     |
| Field  5  | writes completed                          |     |
| Field  6  | writes merged                             |     |
| Field  7  | sectors written                           |     |
| Field  8  | milliseconds spent writing                |     |
| Field  9  | I/Os currently in progress                |     |
| Field  10 | milliseconds spent doing I/Os             |     |
| Field  11 | weighted of milliseconds spent doing I/Os |     |
| Field  12 | discards completed                        |     |
| Field  13 | discards merged                           |     |
| Field 14  | sectors discarded                         |     |
| Field 15  | milliseconds spent discarding             |     |

为了避免引入性能瓶颈，修改这些计数器时不会加锁。这意味着当更改发生冲突时可能会引入轻微的不准确，因此（例如）将每个分区发出的所有读取 I/O 相加应该等于对磁盘所做的那些......但由于缺乏锁定，它可能只是非常接近。

在 2.6+ 每个CPU都有计数器，这使不加锁几乎不成问题了。读取统计信息时，对每个 CPU 的计数器求和（可能溢出它们求和的 unsigned long 变量）并将结果提供给用户。 没有方便的用户界面来访问每个 CPU 的计数器本身。


