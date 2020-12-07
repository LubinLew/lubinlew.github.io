# 惊群效应

> 转载 [Linux惊群效应详解](https://blog.csdn.net/lyztyycode/article/details/78648798?locationNum=6&fps=1) 有改动
> 
> [Thundering herd problem - Wikipedia](https://en.m.wikipedia.org/wiki/Thundering_herd_problem)

  
惊群效应(现象)就是多进程(多线程)在同时阻塞等待同一个事件的时候（休眠状态），如果等待的这个事件发生，那么他就会唤醒等待的所有进程（或者线程），但是最终却只可能有一个进程（线程）获得这个时间的“控制权”，对该事件进行处理，而其他进程（线程）获取“控制权”失败，只能重新进入休眠状态，这种现象和性能浪费就叫做惊群。举一个很简单的例子，当你往一群鸽子中间扔一粒谷子，所有的鸽子都被惊动前来抢夺这粒食物，但是最终只有一个鸽子抢到食物，没有抢到的鸽子只能等待抢下一粒谷子。

**惊群效应造成的问题**：系统对用户进程/线程频繁地做无效的调度，上下文切换系统性能大打折扣；为了确保只有一个线程得到资源，用户必须对资源操作进行加锁保护，进一步加大了系统开销。

     
 是不是还是觉得不够深入，概念化？看下面：

 1、上下文切换（context  switch）过高会导致cpu像个搬运工，频繁地在寄存器和运行队列之间奔波，更多的时间花在了进程（线程）切换，而不是在真正工作的进程（线程）上面。直接的消耗包括cpu寄存器要保存和加载（例如程序计数器）、系统调度器的代码需要执行。间接的消耗在于多核cache之间的共享数据。

**看一下：** [*wiki上下文切换*](https://en.m.wikipedia.org/wiki/Context_switch)

         
2、通过锁机制解决惊群效应是一种方法，在任意时刻只让一个进程（线程）处理等待的事件。但是锁机制也会造成cpu等资源的消耗和性能损耗。目前一些常见的服务器软件有的是通过锁机制解决的，比如nginx（它的锁机制是默认开启的，可以关闭）；还有些认为惊群对系统性能影响不大，没有去处理，比如lighttpd。

## 各种惊群现象

惊群效应在底层的多进程/多线程编程时，都可能遇到。

### accept() 惊群

在linux2.6版本以后，linux内核已经解决了`accept()`函数的惊群现象，大概的处理方式就是，当内核接收到一个客户连接后，只会唤醒等待队列上的第一个进程（线程）,所以如果服务器采用accept阻塞调用方式，在最新的linux系统中已经没有“惊群效应”了。`accept()`只能被一个进程调用成功，内核很清楚这一点，所以内核可以解决。

### epoll() 惊群

`epoll()`目前还是存在惊群效应。但是Linux 4.5内核版本中的`epoll()`，添加了`EPOLLEXCLUSIVE`标志，因此，几个epoll集（不同的线程或不同的进程）可以在相同的资源上等待，但是只有一个集合会被唤醒。

### 线程惊群

这里指的是 `pthread_cond_broadcast()` 会唤醒所有阻塞在`pthread_cond_wait()` 的线程。所以必需与 `pthread_mutex_t` 一起使用。



## 解决惊群效应

[SO_REUSEPORT](linux_c_programming/network/socket/reuseport.md) 特性。Linux内核的3.9版本带来了SO_REUSEPORT特性，该特性支持多个进程或者线程绑定到同一端口，提高服务器程序的性能，允许多个套接字bind()以及listen()同一个TCP或UDP端口，并且在内核层面实现负载均衡。

nginx中可以使用锁或者链接数限制来解决。详见 [“惊群”，看看nginx是怎么解决它的_陶辉](https://blog.csdn.net/russell_tao/article/details/7204260)。当然最新版本的nginx也支持[SO_REUSEPORT](linux_c_programming/network/socket/reuseport.md) 和 `EPOLLEXCLUSIVE` 特性。


