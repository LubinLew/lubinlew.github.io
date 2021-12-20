# 内存使用率计算

> [官方日志](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=34e431b0ae398fc54ea69ff85ec700722c9da773)
> 
> 许多程序检查 `/proc/meminfo` 来估计有多少可用内存。他们通常通过将 "MemFree" 和 "Cached" 相加，这在十年前还是正确的，但是现在(2014)几乎肯定是错的。这是因为 "Cached" 包含不能释放的页缓存(page cache)，例如共享内存段、tmpfs 和 ramfs，并且不包括可回收的 slab 内存，在很多有大量文件的空闲系统中，它可能会占用很大一部分系统内存。

内核为了程序便捷的获取可用内存(无swap), 在 `/proc/meminfo` 中新增了 `MemAvailable` 字段， 在 `Centos/Redhat 7+`, `Ubuntu 16+`中都可用。

但是在老的内核中（例如CentOS6）中没有 `MemAvailable` 字段，这就需要用户自己计算。



## References

[linux - What is the difference between MemFree and MemAvailable in /proc/meminfo](https://superuser.com/questions/980820/what-is-the-difference-between-memfree-and-memavailable-in-proc-meminfo)

[How is the total memory utilization calculated for a Linux server monitor?](https://support.site24x7.com/portal/en/kb/articles/how-is-memory-utilization-calculated-for-a-linux-server-monitor)


