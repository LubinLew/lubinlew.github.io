# Linux reuseport 特性

> [[SO_REUSEPORT学习笔记](http://www.blogjava.net/yongboy/archive/2015/02/12/422893.html)

服务器网络编程的普遍要求都是高并发/高性能。由于CPU核心数量的增多，高并发都依赖于发挥多核心的优势（当然，高并发/高并发与非常多的事情都相关，这里主要想引出reuseport特性）。

## 惊群效应

`SO_REUSEADDR` 特性允许多个进程监听相同的地址，这样多核对应的多进程模型就可以处理同一个 socket 上的请求。但是这样又产生了 `惊群` 的问题，也就是说当socket 中有新的请求到来的时候，所有的进程都会被唤醒，都想要处理这个请求。有竞争就会有锁的出现。例如 nginx中 ``accept_mutex`` 选项。


