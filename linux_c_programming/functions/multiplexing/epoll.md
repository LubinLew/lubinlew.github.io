# EPOLL

epoll 的功能与 poll 类似,同时监视多个文件描述符以查看是否可以在其中任何一个上进行 I/O 操作。可以用作边缘触发(ET)或水平触发(LT)的接口，并且可以很好地扩展到大量监视的文件描述符。

## SYNOPSIS

```c
  #include <sys/epoll.h>

int epoll_create(int size);
int epoll_create1(int flags);

int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
int epoll_pwait(int epfd, struct epoll_event *events,  int maxevents, int timeout, const sigset_t *sigmask);

#include <unistd.h>
int close(int fd); /* 关闭 epoll 实例的句柄 */
```

## DESCRIPTION

### 核心概念

epoll API 的核心概念是 epoll 实例(instance)，这是一个内核数据结构，从用户空间的角度来看，它可以被视为两个列表的容器：

- 兴趣列表: 进程已注册感兴趣监视的一组文件描述符。

- 就绪列表：是 I/O 为 "ready" 状态的文件描述符集。
  就绪列表是兴趣列表中文件描述符的子集（或更准确地说，是一组引用），由于对这些文件描述符的 I/O 活动，内核会动态填充该文件描述符。

### API

提供以下系统调用来创建和管理 epoll 实例：

#### 创建 epoll 实例

创建一个新的 epoll 实例, 并返回实例句柄。其中 `epoll_create1` 是后加入的新API

```c
int epoll_create(int size);    //kernel >= 2.6,    glibc >=2.3.2
int epoll_create1(int flags);  //kernel >= 2.6.27, glibc >=2.9
```

##### 参数 `size`

从2.6.8开始,参数 `size` 被废弃, 但是为了与旧代码兼容而保留下来,需要大于0。

> 在最初的 epoll_create() 实现中，`size` 参数通知内核调用者希望添加到 epoll 实例中的文件描述符的数量。内核使用此信息作为初始在描述事件的内部数据结构中分配的空间量的提示。 （如有必要，如果调用者的使用量超过 size 给出的提示，内核将分配更多空间。）如今，不再需要此提示（内核动态调整所需数据结构的大小而不需要这个提示），但 size 仍然必须是 大于零，以确保在旧内核上运行新的 epoll 应用程序时向后兼容。

##### 参数 `flags`

取值为 0 或 `EPOLL_CLOEXEC`。为 0 则与 `epoll_create()` 完成相同;为 `EPOLL_CLOEXEC`时, 表示在新文件描述符上设置 close-on-exec (FD_CLOEXEC) 标志。

指定此标志,程序可以避免使用 fcntl(2) F_SETFD 操作来设置 FD_CLOEXEC 标志。

because using a separate fcntl(2) F_SETFD operation to set  the  FD_CLOEXEC flag  does  not  suffice to avoid race conditions where one thread opens a file descriptor and attempts to set its close-on-exec flag using fcntl(2) at the same time as another thread does a fork(2) plus execve(2).  Depending on the order of execution, the race may lead to the file descriptor  returned by  open()  being  unintentionally leaked to the program executed by the child process created by fork(2).  (This kind of race is in principle possible for any system call that creates a file descriptor whose close-on-exec flag should be set, and various other Linux system calls provide  an  equivalent of the O_CLOEXEC flag to deal with this problem.)
请注意，在某些多线程程序中使用此标志是必不可少的，因为使用单独的 fcntl(2) F_SETFD 操作来设置 FD_CLOEXEC 标志不能避免一个线程打开文件描述符并尝试设置其关闭的竞争条件。 使用 fcntl(2) 的 on-exec 标志，同时另一个线程执行 fork(2) 和 execve(2)。 根据执行顺序，竞争可能会导致 open() 返回的文件描述符无意中泄露给由 fork(2) 创建的子进程执行的程序。 （原则上，任何创建文件描述符的系统调用都可能发生这种竞争，该文件描述符应设置 close-on-exec 标志，并且各种其他 Linux 系统调用提供了等效的 O_CLOEXEC 标志来处理此问题。）

#### epoll 实例控制

```c
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
```

此系统调用用于在文件描述符 epfd 引用的 epoll(7) 实例的兴趣列表中添加、修改或删除条目。 它请求对目标文件描述符 fd 执行操作 op。

Valid values for the op argument are:

- EPOLL_CTL_ADD
  Add fd to the interest list and associate the settings specified in event with the internal file linked to fd.

- EPOLL_CTL_MOD
  Change the settings associated with fd in the interest list to the new settings specified in event.

- EPOLL_CTL_DEL
  Remove (deregister) the target file descriptor fd from the interest list.  The event argument is ignored and can be NULL (but see BUGS below).

The events member is a bit mask composed by ORing together zero or more of the following available event types:

- EPOLLIN
    相关的文件描述符可以执行 read(2) 操作.

- EPOLLOUT
    相关的文件描述符可以执行 write(2) 操作.

- EPOLLRDHUP (since Linux 2.6.17)
  
  Stream socket peer closed connection, or shut down writing half of connection.  (This flag is especially useful for writing simple code to detect  peer shutdown when using Edge Triggered monitoring.) Stream socket peer 关闭连接，或者关闭写入一半的连接。 （此标志对于编写简单代码以在使用边缘触发监视时检测对等关闭特别有用。）

- EPOLLPRI
  
  There is an exceptional condition on the file descriptor.  See the discussion of POLLPRI in poll(2). 文件描述符存在异常情况。 请参阅 poll(2) 中对 POLLPRI 的讨论。

- EPOLLERR
  
  Error condition happened on the associated file descriptor.  This event is also reported for the write end of a pipe when the read end has been closed. epoll_wait(2) will always report for this event; it is not necessary to set it in events.
   相关文件描述符上发生错误情况。 当读取端已关闭时，也会为管道的写入端报告此事件。 epoll_wait(2) 将始终报告此事件； 没有必要在事件中设置它。

- EPOLLHUP
  
  Hang up happened on the associated file descriptor.  epoll_wait(2) will always wait for this event; it is not necessary to set it in events. 挂断发生在关联的文件描述符上。 epoll_wait(2) 会一直等待这个事件； 没有必要在事件中设置它。
  Note that when reading from a channel such as a pipe or a stream socket, this event merely indicates that the peer closed its end of the channel.  Subsequent reads from the channel will return 0 (end of file) only after all outstanding data in the channel has been consumed.
  请注意，当从管道或流套接字等通道读取时，此事件仅表示对等方关闭了通道的末端。 只有在通道中所有未完成的数据都被消耗完后，从通道中读取的后续数据才会返回 0（文件结尾）。

- EPOLLET
  
  Sets  the  Edge  Triggered  behavior for the associated file descriptor.  The default behavior for epoll is Level Triggered.  See epoll(7) for more detailed information about Edge and Level Triggered event distribution architectures.
  设置关联文件描述符的边缘触发行为。 epoll 的默认行为是 Level Triggered。 有关 Edge 和 Level Triggered 事件分发架构的更多详细信息，请参阅 epoll(7)。

- EPOLLONESHOT (since Linux 2.6.2)
  
  Sets the one-shot behavior for the associated file descriptor.  This means that after an event is pulled out with epoll_wait(2) the associated file descriptor  is  internally  disabled  and  no other events will be reported by the epoll interface.  The user must call epoll_ctl() with EPOLL_CTL_MOD to rearm the file descriptor with a new event mask.
  设置关联文件描述符的一次性行为。 这意味着在使用 epoll_wait(2) 拉出事件后，关联的文件描述符在内部被禁用，并且 epoll 接口不会报告其他事件。 用户必须使用 EPOLL_CTL_MOD 调用 epoll_ctl() 以使用新的事件掩码重新配置文件描述符。

- EPOLLWAKEUP (since Linux 3.5)
  
  If EPOLLONESHOT and EPOLLET are clear and the process has the CAP_BLOCK_SUSPEND capability, ensure that the system does not enter "suspend" or  "hibernate"  while  this  event  is  pending or being processed.  The event is considered as being "processed" from the time when it is returned by a call to epoll_wait(2) until the next call to epoll_wait(2) on the same epoll(7) file descriptor, the closure of that file descriptor, the removal of the  event file descriptor with EPOLL_CTL_DEL, or the clearing of EPOLLWAKEUP for the event file descriptor with EPOLL_CTL_MOD.  See also BUGS.
   如果 EPOLLONESHOT 和 EPOLLET 被清除并且进程具有 CAP_BLOCK_SUSPEND 能力，请确保在此事件挂起或正在处理时系统不会进入“暂停”或“休眠”。 从调用 epoll_wait(2) 到下一次在同一个 epoll(7) 文件描述符上调用 epoll_wait(2) 时，该事件被认为是“处理”的，该文件描述符的关闭， 使用 EPOLL_CTL_DEL 删除事件文件描述符，或使用 EPOLL_CTL_MOD 清除事件文件描述符的 EPOLLWAKEUP。 另请参阅错误。

- EPOLLEXCLUSIVE (since Linux 4.5)
  
  Sets an exclusive wakeup mode for the epoll file descriptor that is being attached to the target file descriptor, fd.  When a wakeup event occurs and
   multiple epoll file descriptors are attached to the same target file using EPOLLEXCLUSIVE, one or more of the epoll file descriptors  will  receive  an
   event  with  epoll_wait(2).   The  default  in  this  scenario  (when EPOLLEXCLUSIVE is not set) is for all epoll file descriptors to receive an event.
   EPOLLEXCLUSIVE is thus useful for avoiding thundering herd problems in certain scenarios.
  
   If the same file descriptor is in multiple epoll instances, some with the EPOLLEXCLUSIVE flag, and others without, then events will be provided to  all
   epoll instances that did not specify EPOLLEXCLUSIVE, and at least one of the epoll instances that did specify EPOLLEXCLUSIVE.
  
   The  following values may be specified in conjunction with EPOLLEXCLUSIVE: EPOLLIN, EPOLLOUT, EPOLLWAKEUP, and EPOLLET.  EPOLLHUP and EPOLLERR can also
   be specified, but this is not required: as usual, these events are always reported if they occur, regardless of whether they are specified  in  events.
   Attempts to specify other values in events yield the error EINVAL.
  
   EPOLLEXCLUSIVE may be used only in an EPOLL_CTL_ADD operation; attempts to employ it with EPOLL_CTL_MOD yield an error.  If EPOLLEXCLUSIVE has been set
   using epoll_ctl(), then a subsequent EPOLL_CTL_MOD on the same epfd, fd pair yields an error.  A call to epoll_ctl() that specifies  EPOLLEXCLUSIVE  in
   events and specifies the target file descriptor fd as an epoll instance will likewise fail.  The error in all of these cases is EINVAL.

```c
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
```

```c
int epoll_pwait(int epfd, struct epoll_event *events,  int maxevents, int timeout, const sigset_t *sigmask);
```

* Interest in particular file descriptors is then registered via epoll_ctl(2), which adds items to the interest list of the　epoll instance.

* epoll_wait(2) waits for I/O events, blocking the calling thread if no events are currently available. 
  (This system call can be thought of as fetching items from the ready list of the epoll instance.)

水平触发(Level-triggered) 和 边缘触发(edge-triggered)

The epoll event distribution interface is able to behave both as edge-triggered (ET) and as level-triggered (LT). The difference between the two mechanisms can be described as follows. Suppose that this scenario happens:

epoll 事件分发接口既可以作为边缘触发（ET），也可以作为水平触发（LT）。 两种机制之间的区别可以描述如下。 假设发生这种情况：

1. The file descriptor that represents the read side of a pipe (rfd) is registered on the epoll instance.

2. A pipe writer writes 2 kB of data on the write side of the pipe.

3. A call to epoll_wait(2) is done that will return rfd as a ready file descriptor.

4. The pipe reader reads 1 kB of data from rfd.

5. A call to epoll_wait(2) is done.
   If the rfd file descriptor has been added to the epoll interface using the EPOLLET (edge-triggered) flag, the call to epoll_wait(2) done in step 5 will probably hang despite the available data still present in the file input buffer; meanwhile the remote peer might be expecting a response based on the data it already sent.  The reason for this is that edge-triggered mode delivers events only when changes occur on the monitored file descriptor. So, in step 5 the caller might end up waiting for some data that is already present inside the input buffer. In the above example, an event on rfd will be generated because of the write done in 2 and the event is consumed in 3. Since the read operation done in 4 does not consume the whole buffer data, the call to epoll_wait(2) done in step 5 might block indefinitely.

An application that employs the EPOLLET flag should use nonblocking file descriptors to avoid having a blocking read or write starve a task that is handling multiple file descriptors. The suggested way to use epoll as an edge-triggered (EPOLLET) interface is as follows:

   i  with nonblocking file descriptors; and

   ii by waiting for an event only after read(2) or write(2) return EAGAIN.

By contrast, when used as a level-triggered interface (the default, when EPOLLET is not specified), epoll is simply a faster poll(2), and can be used wherever the latter is used since it shares the same semantics.

Since even with edge-triggered epoll, multiple events can be generated upon receipt of multiple chunks of data, the caller has the option to specify the EPOLLONESHOT flag, to tell epoll to disable the associated file descriptor after the receipt of an event with epoll_wait(2). When the EPOLLONESHOT flag is specified, it is the caller's responsibility to rearm the file descriptor using epoll_ctl(2) with EPOLL_CTL_MOD.

If multiple threads (or processes, if child processes have inherited the epoll file descriptor across fork(2)) are blocked in epoll_wait(2) waiting on the same epoll file descriptor and a file descriptor in the interest list that is marked for edge-triggered (EPOLLET) notification becomes ready, just one of the threads (or processes) is awoken from epoll_wait(2). This provides a useful optimization for avoiding "thundering herd" wake-ups in some scenarios.

  Interaction with autosleep
    If the system is in autosleep mode via /sys/power/autosleep and an event happens which wakes the device from sleep, the device driver will keep the device
    awake only until that event is queued. To keep the device awake until the event has been processed, it is necessary to use the epoll_ctl(2) EPOLLWAKEUP flag.

When the EPOLLWAKEUP flag is set in the events field for a struct epoll_event, the system will be kept awake from the moment the event is queued, through the
epoll_wait(2) call which returns the event until the subsequent epoll_wait(2) call. If the event should keep the system awake beyond that time, then a sepa‐
rate wake_lock should be taken before the second epoll_wait(2) call.

  /proc interfaces
    The following interfaces can be used to limit the amount of kernel memory consumed by epoll:

/proc/sys/fs/epoll/max_user_watches (since Linux 2.6.28)
   This specifies a limit on the total number of file descriptors that a user can register across all epoll instances on the system. The limit is per
   real user ID. Each registered file descriptor costs roughly 90 bytes on a 32-bit kernel, and roughly 160 bytes on a 64-bit kernel. Currently, the default value for max_user_watches is 1/25 (4%) of the available low memory, divided by the registration cost in bytes.

  Example for suggested usage While the usage of epoll when employed as a level-triggered interface does have the same semantics as poll(2), the edge-triggered usage requires more clarification to avoid stalls in the application event loop. In this example, listener is a nonblocking socket on which listen(2) has been called.  The function do_use_fd() uses the new ready file descriptor until EAGAIN is returned by either read(2) or write(2). An event-driven state machine application should, after having received EAGAIN, record its current state so that at the next call to do_use_fd() it will continue to read(2) or write(2) from where it stopped before.

```c
#define MAX_EVENTS 10
struct epoll_event ev, events[MAX_EVENTS];
int listen_sock, conn_sock, nfds, epollfd;

/* Code to set up listening socket, 'listen_sock',
  (socket(), bind(), listen()) omitted */

epollfd = epoll_create1(0);
if (epollfd == -1) {
  perror("epoll_create1");
  exit(EXIT_FAILURE);
}

ev.events = EPOLLIN;
ev.data.fd = listen_sock;
if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
  perror("epoll_ctl: listen_sock");
  exit(EXIT_FAILURE);
}

for (;;) {
  nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
  if (nfds == -1) {
    perror("epoll_wait");
    exit(EXIT_FAILURE);
  }

  for (n = 0; n < nfds; ++n) {
    if (events[n].data.fd == listen_sock) {
      conn_sock = accept(listen_sock,
                (struct sockaddr *) &addr, &addrlen);
      if (conn_sock == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
      }
      setnonblocking(conn_sock);
      ev.events = EPOLLIN | EPOLLET;
      ev.data.fd = conn_sock;
      if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
            &ev) == -1) {
        perror("epoll_ctl: conn_sock");
        exit(EXIT_FAILURE);
      }
    } else {
      do_use_fd(events[n].data.fd);
    }
  }
}
```

When used as an edge-triggered interface, for performance reasons, it is possible to add the file descriptor inside the epoll interface (EPOLL_CTL_ADD) once by specifying (EPOLLIN|EPOLLOUT). 
This allows you to avoid continuously switching between EPOLLIN and EPOLLOUT calling epoll_ctl(2) with EPOLL_CTL_MOD.

  Questions and answers
0. What is the key used to distinguish the file descriptors registered in an interest list?

  The key is the combination of the file descriptor number and the open file description (also known as an "open file handle", the kernel's internal repre‐
  sentation of an open file).

1. What happens if you register the same file descriptor on an epoll instance twice?
   
   You will probably get EEXIST. However, it is possible to add a duplicate (dup(2), dup2(2), fcntl(2) F_DUPFD) file descriptor to the same epoll instance.
   This can be a useful technique for filtering events, if the duplicate file descriptors are registered with different events masks.

2. Can two epoll instances wait for the same file descriptor? If so, are events reported to both epoll file descriptors?
   
   Yes, and events would be reported to both. However, careful programming may be needed to do this correctly.

3. Is the epoll file descriptor itself poll/epoll/selectable?
   
   Yes. If an epoll file descriptor has events waiting, then it will indicate as being readable.

4. What happens if one attempts to put an epoll file descriptor into its own file descriptor set?
   
   The epoll_ctl(2) call fails (EINVAL). However, you can add an epoll file descriptor inside another epoll file descriptor set.

5. Can I send an epoll file descriptor over a UNIX domain socket to another process?
   
   Yes, but it does not make sense to do this, since the receiving process would not have copies of the file descriptors in the interest list.

6. Will closing a file descriptor cause it to be removed from all epoll interest lists?
   
   Yes, but be aware of the following point. A file descriptor is a reference to an open file description (see open(2)). Whenever a file descriptor is du‐
   plicated via dup(2), dup2(2), fcntl(2) F_DUPFD, or fork(2), a new file descriptor referring to the same open file description is created. An open file
   description continues to exist until all file descriptors referring to it have been closed.
   
   A file descriptor is removed from an interest list only after all the file descriptors referring to the underlying open file description have been closed.
   This means that even after a file descriptor that is part of an interest list has been closed, events may be reported for that file descriptor if other
   file descriptors referring to the same underlying file description remain open. To prevent this happening, the file descriptor must be explicitly removed
   from the interest list (using epoll_ctl(2) EPOLL_CTL_DEL) before it is duplicated. Alternatively, the application must ensure that all file descriptors
   are closed (which may be difficult if file descriptors were duplicated behind the scenes by library functions that used dup(2) or fork(2)).

7. If more than one event occurs between epoll_wait(2) calls, are they combined or reported separately?
   
   They will be combined.

8. Does an operation on a file descriptor affect the already collected but not yet reported events?
   
   You can do two operations on an existing file descriptor. Remove would be meaningless for this case. Modify will reread available I/O.

9. Do I need to continuously read/write a file descriptor until EAGAIN when using the EPOLLET flag (edge-triggered behavior)?

Receiving an event from epoll_wait(2) should suggest to you that such file descriptor is ready for the requested I/O operation.  You must consider it
ready until the next (nonblocking) read/write yields EAGAIN. When and how you will use the file descriptor is entirely up to you.

For packet/token-oriented files (e.g., datagram socket, terminal in canonical mode), the only way to detect the end of the read/write I/O space is to con‐
tinue to read/write until EAGAIN.

For stream-oriented files (e.g., pipe, FIFO, stream socket), the condition that the read/write I/O space is exhausted can also be detected by checking the
amount of data read from / written to the target file descriptor. For example, if you call read(2) by asking to read a certain amount of data and read(2)
returns a lower number of bytes, you can be sure of having exhausted the read I/O space for the file descriptor. The same is true when writing using
write(2). (Avoid this latter technique if you cannot guarantee that the monitored file descriptor always refers to a stream-oriented file.)

Possible pitfalls and ways to avoid them
 o Starvation (edge-triggered)

 If there is a large amount of I/O space, it is possible that by trying to drain it the other files will not get processed causing starvation. (This problem
 is not specific to epoll.)

 The solution is to maintain a ready list and mark the file descriptor as ready in its associated data structure, thereby allowing the application to remember
 which files need to be processed but still round robin amongst all the ready files. This also supports ignoring subsequent events you receive for file de‐
 scriptors that are already ready.

 o If using an event cache...

 If you use an event cache or store all the file descriptors returned from epoll_wait(2), then make sure to provide a way to mark its closure dynamically
 (i.e., caused by a previous event's processing). Suppose you receive 100 events from epoll_wait(2), and in event #47 a condition causes event #13 to be
 closed. If you remove the structure and close(2) the file descriptor for event #13, then your event cache might still say there are events waiting for that
 file descriptor causing confusion.

 One solution for this is to call, during the processing of event 47, epoll_ctl(EPOLL_CTL_DEL) to delete file descriptor 13 and close(2), then mark its associ‐
 ated data structure as removed and link it to a cleanup list. If you find another event for file descriptor 13 in your batch processing, you will discover
 the file descriptor had been previously removed and there will be no confusion.

## VERSIONS

The epoll API was introduced in Linux kernel 2.5.44. Support was added to glibc in version 2.3.2.

## CONFORMING TO

The epoll API is Linux-specific. Some other systems provide similar mechanisms, for example, FreeBSD has kqueue, and Solaris has /dev/poll.

## NOTES

The set of file descriptors that is being monitored via an epoll file descriptor can be viewed via the entry for the epoll file descriptor in the process's `/proc/[pid]/fdinfo` directory. See proc(5) for further details.
The kcmp(2) KCMP_EPOLL_TFD operation can be used to test whether a file descriptor is present in an epoll instance.

## 版权

This page is part of release 5.05 of the Linux man-pages project. A description of the project, information about reporting bugs, and the latest version of this page, can be found at https://www.kernel.org/doc/man-pages/.
