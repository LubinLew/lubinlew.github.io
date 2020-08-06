我们常见的各种锁是有层级的，**最底层的两种锁就是互斥锁和自旋锁**，其他锁都是基于它们实现的。互斥锁的加锁成本更高，但它在加锁失败时会释放 CPU 给其他线程；自旋锁则刚好相反。

‌

# 

互斥锁

‌

当你无法判断锁住的代码会执行多久时，应该首选互斥锁，互斥锁是一种独占锁。什么意思呢？当 A 线程取到锁后，互斥锁将被 A 线程独自占有，当 A 没有释放这把锁时，其他线程的取锁代码都会被阻塞。阻塞是怎样进行的呢？对于 99% 的线程级互斥锁而言，阻塞都是由操作系统内核实现的（比如 Linux 下它通常由内核提供的信号量实现）。当获取锁失败时，内核会将线程置为休眠状态，等到锁被释放后，内核会在合适的时机唤醒线程，而这个线程成功拿到锁后才能继续执行。如下图所示：

互斥锁通过内核帮忙切换线程，简化了业务代码使用锁的难度。但是，**线程获取锁失败时，增加了两次上下文切换的成本：从运行中切换为休眠，以及锁释放时从休眠状态切换为运行中。**上下文切换耗时在几十纳秒到几微秒之间，或许这段时间比锁住的代码段执行时间还长。而且，线程主动进入休眠是高并发服务无法容忍的行为，这让其他异步请求都无法执行。

‌

# 

自旋锁

‌

如果你能确定被锁住的代码执行时间很短，就应该用自旋锁取代互斥锁。自旋锁比互斥锁快得多，**因为它通过 CPU 提供的 CAS 函数（全称 Compare And Swap），在用户态代码中完成加锁与解锁操作**。我们知道，加锁流程包括 2 个步骤：第 1 步查看锁的状态，如果锁是空闲的，第 2 步将锁设置为当前线程持有。在没有 CAS 操作前，多个线程同时执行这 2 个步骤是会出错的。比如线程 A 执行第 1 步发现锁是空闲的，但它在执行第 2 步前，线程 B 也执行了第 1 步，B 也发现锁是空闲的，于是线程 A、B 会同时认为它们获得了锁。**CAS 函数把这 2 个步骤合并为一条硬件级指令**。这样，第 1 步比较锁状态和第 2 步锁变量赋值，将变为不可分割的原子指令。于是，设锁为变量 lock，整数 0 表示锁是空闲状态，整数 pid 表示线程 ID，那么 CAS(lock, 0, pid) 就表示自旋锁的加锁操作，CAS(lock, pid, 0) 则表示解锁操作。**多线程竞争锁的时候，加锁失败的线程会“忙等待”，直到它拿到锁。什么叫“忙等待”呢？它并不意味着一直执行 CAS 函数，生产级的自旋锁在“忙等待”时，会与 CPU 紧密配合 ，它通过 CPU 提供的 PAUSE 指令**，减少循环等待时的耗电量；对于单核 CPU，忙等待并没有意义，此时它会主动把线程休眠。

[

CAS原子操作实现无锁及性能分析 - wzjhoutai - 博客园

CAS原子操作实现无锁及性能分析 Author：Echo Chen（陈斌） Email：chenb19870707@gmail.com Blog：Blog.csdn.net/chen19870707

www.cnblogs.com

](https://www.cnblogs.com/wzjhoutai/p/6838104.html)

‌

# 

读写锁

‌

当取不到锁时，互斥锁用“线程切换”来面对，自旋锁则用“忙等待”来面对。这是两种最基本的处理方式，更高级别的锁都会选择其中一种来实现，比如读写锁就既可以基于互斥锁实现，也可以基于自旋锁实现。