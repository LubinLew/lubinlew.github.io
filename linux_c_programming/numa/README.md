# NUMA

在传统的 SMP 架构中，计算机只有一个内存控制器，内存会被所有的 CPU 共享。
当多个 CPU 同时访问内存时，可能会形成瓶颈；同时，对于具有多个 CPU 的大型系统不能很好地进行扩展。
出于这个原因，越来越多的现代系统正在使用 CC/NUMA 架构。

> CC: Cache Coherent, 缓存一致性
> 
> NUMA: Nonuniform Memory Access, 非一致性(不均匀)内存访问

在 SMP 系统上，所有 CPU 都可以平等地访问连接到所有内存芯片 (DIMMs) 的同一个共享内存控制器。
CPU 之间的通信也通过此共享资源进行，这可能会变得拥塞。
单个控制器可以管理的内存芯片数量也是有限的，这就限制了系统可以支持多少内存。 
此外，通过这个单一的流量中心访问内存的延迟相对较高。

NUMA 架构旨在超越 SMP 架构的可扩展性限制。
系统不是每台计算机都有一个内存控制器，而是分成多个节点。 
每个节点都有处理器和自己的内存。
处理器可以非常快速地访问节点中的本地内存。
系统中的所有节点都使用快速互连进行连接。 
添加到系统中的每个新节点都会为系统提供更多的聚合内存带宽和容量，从而提供出色的可扩展性。

理论上，通过忽略软件中本地和远程内存之间的差异，
NUMA系统可以被视为一个SMP系统。事实上，这是经常发生的。
但为了获得最佳性能，应该考虑到这些差异。

NUMA体系结构的一个巨大优势是，即使在具有许多cpu的大系统中，也可以在本地内存上获得非常低的延迟。
因为现代的CPU比内存芯片快得多，所以当CPU从内存中读取数据时，常常要花费相当长的时间等待。
因此，最小化内存延迟可以提高软件性能。

NUMA策略关注在特定节点上分配内存，以让程序尽可能快地访问它们。
实现这一点的主要方法是在本地节点上为线程分配内存，并保持线程在本地节点上运行(节点关联)。
这为内存提供了最佳的延迟，并将通过全局互连的流量最小化。

在SMP系统上，有一种常见的优化称为缓存亲和性，它有点类似。
缓存关联性试图将数据保存在CPU的缓存中，而不是在处理器之间频繁地“跳跃”。
这通常是由操作系统中的调度器来完成的，它尝试在一个CPU上保留线程一段时间，然后将其调度到另一个CPU上。
但是，这与节点关联性有一个重要的区别:当SMP系统上的线程在cpu之间移动时，它的缓存内容最终也会随之移动。
一旦一个内存区域被提交给NUMA系统上的一个特定节点，它就会停留在那里。
在访问它的不同节点上运行的线程总是向互连增加流量，导致更高的延迟。
这就是为什么NUMA系统需要比SMP系统更努力地归档节点关联性。
当然，缓存亲和性本身在NUMA系统上也是一项有价值的优化。这还不足以获得最佳表现。
然而，操作系统中的调度器不能总是纯粹针对节点关联性进行优化。
问题是，在系统中不使用CPU的情况甚至比使用远程内存并看到更高的内存延迟的进程还要糟糕。
在内存性能甚至比系统中所有cpu的使用更重要的情况下，应用程序或系统管理员可以覆盖操作系统的默认决策。
这允许对特定工作负载进行更好的优化。
Linux传统上使用系统调用将线程绑定到特定的cpu(使用sched_set_affinity(2)系统调用和调度)。
NUMA API对此进行了扩展，以允许程序指定应该在哪个节点上分配内存。
为了使用户空间程序更容易针对NUMA配置进行优化，api导出拓扑信息并允许使用用户规格的处理器和内存资源。
还有内部内核api提供NUMA拓扑信息，供内核子系统使用。
这里描述的NUMA API将线程与cpu的位置和内存的位置分开。
它主要与内存的位置有关。此外，应用程序可以单独配置CPU关联性。
NUMA API目前可用于SUSE®LINUX Enterprise Server 9的AMD64和Intel* Itanium*处理器家族。


## 带宽优化

程序的内存访问性能可以根据延迟或带宽进行优化。大多数程序似乎更喜欢较低的延迟，但也有少数例外需要带宽。
使用节点本地内存具有最佳延迟。为了获得更多的带宽，可以并行使用多个节点的内存控制器。
这类似于RAID通过将I/O操作分散到多个硬盘来提高硬盘I/O性能。
NUMA API可以使用CPU中的MMU(内存管理单元)来交错来自不同内存控制器的内存块。
这意味着这种映射中的每个连续page1都来自不同的节点。
当一个应用程序对这样一个交错的区域进行大量的流内存访问时，
多个节点的内存控制器的带宽是合并的。
这取决于NUMA架构，特别是互连的性能和本地和远程内存之间的延迟差异。
在某些系统上，它只在相邻节点的子集上有效工作。
一些NUMA系统，如AMD Opteron，可以通过固件配置，在页面的基础上跨所有节点交错所有内存。这被称为节点交错。
节点交错类似于NUMA API提供的交错模式，但它们在重要方面有所不同。节点交错适用于所有内存。
NUMA API交织可以为每个进程或线程单独配置。如果固件启用了节点交错，则NUMA策略将被禁用。
要使用NUMA策略，必须在BIOS或固件中始终禁用节点交错。
通过NUMA API，每个应用程序都可以单独调整内存区域用于延迟或带宽的策略。

----

## 实现概述

NUMA策略由多个子系统共同提供。内核为进程或特定的内存映射管理内存策略。
这个内核可以由三个新的系统调用控制。
有一个名为libnuma的用户空间共享库，可以从应用程序调用它。libnuma是程序中NUMA策略的推荐API。
它提供了比直接使用系统调用更用户友好和抽象的接口。本文仅描述这种更高层次的接口。
当不应该修改应用程序时，管理员可以使用numactl命令行实用程序设置一些策略。这不如直接从应用程序控制策略灵活。
用户库和应用程序包含在numactl RPM中，它是SUSE LINUX Enterprise Server 92的一部分。
此外，该包还包含一些实用程序，如numastat，用于收集关于内存分配的统计信息，
numademo用于显示不同策略对系统的影响。该包还包含所有函数和程序的手册页。

----

## 策略

NUMA API的主要任务是管理策略。策略可以应用于进程或内存区域。NUMA API目前支持四种策略:

|名称|描述|
|----|----|
|default|在本地节点(线程正在运行的节点)上分配|
|bind|在特定的节点集合上进行分配|
|interleave|在一组节点上交错分配内存|
|preferred|优先尝试在一个节点上分配|


`bind` 和 `preferred` 的区别在于，当不能在指定节点上分配内存时，`bind` 会失败;而 `preferred` 回落到其他节点。
使用 `bind` 可能会导致更早的内存短缺和交换导致的延迟。在libnuma中，`preferred` 和 `bind` 是组合使用的，
可以通过 numa_set_strict libnuma函数对每个线程进行更改。默认是更灵活的首选分配。
可以为每个进程(进程策略)或每个内存区域设置策略。
子进程继承fork上父进程的进程策略。
进程策略应用于进程上下文中所做的所有内存分配。
这包括在系统调用和文件缓存中进行的内部内核分配。
中断总是在当前节点上分配。
当内核分配一页内存时，进程策略总是应用。
每个内存区域设置的策略，也称为VMA策略3，
允许进程在其地址空间中为内存块设置策略。
内存区域策略的优先级高于进程策略。
内存区域策略的主要优点是可以在分配发生之前设置它们。
目前它们只支持匿名进程内存、SYSV共享内存、shmem和tmpfs映射以及hugetlbfs文件。
共享内存的区域策略将一直保持，直到删除共享内存段或文件。


----

## numactl 的使用

numactl 是一个命令行工具，用于运行具有特定NUMA策略的进程。
它对于为无法修改和重新编译的程序设置策略非常有用。



运行指定程序使用 node 0 的CPU, 并且只从 node 0 和 node 1 申请内存

```bash
numactl --cpubind=0 --membind=0,1 program
```

以优先从 node 1 申请内存的方式运行 `numactl --show` 命令

```bash
numactl --preferred=1 numactl --show
```

Run memory bandwidth-intensive number cruncher with memory interleaved over all available nodes.

```bash
numactl --interleave=all numbercruncher
```

Bind the second gigabyte in the tmpfs file /dev/shm/A to node 1.

```bash
numactl --offset=1G --length=1G --membind=1 --file /dev/shm/A --touch
```

Reset the policy for the shared memory file /dev/shm/file. 

```bash
numactl --localalloc /dev/shm/file
```


Print an overview of the available nodes.
 
```bash
numactl --hardware
```



N U M A C T L : I M P O R T A N T C O M M A N D L I N E S W I T C H E S
Here is a quick overview of the important command line switches of numactl.
Many of these switches need a nodemask as argument.
Each node in the system has a unique number.
A nodemask can be a comma-separated list of node numbers, a range of nodes (node1-node2), or all.
See numactl --hardware for the nodes available on the current system.
The most common usage is to set the policy for a process.
The policy is passed as the first argument and is followed by the program name and its argument.
The available policy switches are:
--membind=nodemask:
Allocate memory only on the nodes in nodemask.
--interleave=nodemask:
Interleave all memory allocations over nodes in nodemask.
--cpubind=nodemask:

Execute the process only on the CPUs of the nodes specified in nodemask. It can be specified in addition to other policies because it affects the scheduler separately.
--preferred=node:
Allocate memory preferably on the node node.

Two other options are useful:
--show:
Print the current process NUMA state as inherited from the parent shell.
--hardware:
Give an overview of the available NUMA resources on the system.

For additional details, see the numactl(8) man page.
 

N U M A C T L : S H A R E D M E M O R Y
numactl is also able to change policies in shared memory segments. This is useful for changing the policy of an application.
An example would be a program consisting of multiple processes that uses a common shared memory segment. This is a common setup for database servers. For the individual processes, it is best to use the default policy of allocating memory on their current nodes. This way, they get the best memory latency for their local data structure. The shared memory segment, however, is shared by multiple processes that run on different nodes. To avoid a hot spot on the node that allocated the memory originally, it may be advantageous to set an interleaved policy for the shared memory segment. This way, all access to it are spread evenly over all nodes.
More complex policies are possible. When parts of the shared memory are mostly used by specific processes and are only accessed rarely by others, they could be bound to specific nodes or only interleaved to a subset of nodes near each other.
Shared memory here can be SYSV shared memory (from the shmat system call), mmapped files in tmpfs or shmfs (normally in /dev/shm on a SUSE LINUX system), or a hugetlbfs file. The shared memory policy can be set up before the application starts4. The policy stays assigned to areas in the shared memory segment until it is deleted.
The set policy only applies to new pages that are allocated. Already existing pages in the shared memory segment are not moved to conform to the policy.
Set up a 1GB tmpfs file to interleave its memory over all nodes.

numactl --length=1G --file=/dev/shm/interleaved --interleave=all

A hugetlbfs file can be set up in the same way, but all lengths must be multiples of the huge page size of the system5.
An offset into the shared memory segment or file can be specified with --offset=number. All numeric arguments can have unit prefixes: G for gigabytes, M for megabytes, or K for kilobytes. The mode of the new file can be specified with --mode=mode.
Alternatively, this can be enforced with the --strict option. When --strict is set and an already allocated page does not conform to the new policy, numactl reports an error.
numactl has several more options for controlling the type of the shared memory segment. For details, see the
numactl(8) man page.

4	This assumes the application does not insist on creating the shared memory segment itself.
5	grep Hugepagesize/proc/meminfo gives the huge page size of the current system.
 

L I B N U M A B A S I C S : C H E C K I N G F O R N U M A
So far, we have described numactl, which controls the policy of whole processes. The disadvantage is that the policy always applies to the whole program, not to individual memory areas (except for shared memory) or threads. For some programs, more fine-grained policy control is needed.
This can be done with libnuma. libnuma is a shared library that can be linked to programs and offers a stable API for NUMA policy. It provides a higher level interface than using the NUMA API system calls directly and is the recommended interface for programs. libnuma is part of the numactl RPM.
Applications link with libnuma as follows:

cc ... -lnuma

The NUMA API functions and macros are declared in the numa.h include file.
#include <numa.h>
...
if (numa_available() < 0) {
printf(‘‘Your system does not support NUMA API\n’’);
...
}
...

Before any NUMA API functions can be used, the program must call numa_available(). When this function returns a negative value, there is no NUMA policy support on the system. In this case, the behavior of all other NUMA API functions is undefined and they should not be called.
The next step is usually to call numa_max_node(). This function discovers and returns the number of nodes in the system. The number of nodes is typically needed to set up and verify the memory policy in the program. All programs should discover this dynamically instead of hardcoding a specific system's topology.
All libnuma states are kept locally per thread. Changing a policy in one thread does not affect the other threads in the process.
The following sections give an overview of the various libnuma functions with some examples. Some uncommon functions are not mentioned. For a more detailed reference, see the numa(3) man page.
 

L I B N U M A : N O D E M A S K S
libnuma manages sets of nodes in abstract data types called nodemask_t defined in numa.h. A nodemask_t is a fixed size bit set of node numbers. Each node in the system has a unique number. The highest number is the number returned by numa_max_node(). The highest node is defined for the implementation in the NUMA_NUM_NODES constant. nodemasks are passed by reference to many NUMA API functions.
A nodemask is initialized empty with nodemask_zero().
nodemask_t mask; nodemask_zero(&mask);

A single node can be set with nodemask_set and cleared with nodemask_clr. nodemask_equal compares two nodemasks. nodemask_isset tests if a bit is set in the nodemask.
nodemask_set(&mask, maxnode);	/* set node highest */ if (nodemask_isset(&mask, 1)) {		/* is node 1 set? */
...
}
nodemask_clr(&mask, maxnode);	/* clear highest node again */

There are two predefined nodemasks: numa_all_nodes stands for all nodes in the system and numa_no_nodes is the empty set.
 

L I B N U M A : S I M P L E A L L O C A T I O N
libnuma offers functions to allocate memory with a specified policy. These allocation functions round all allocations to pages (4 KB on AMD64 systems) and are relatively slow. They should be used only for allocating large memory objects that exceed the cache sizes of the CPU and where NUMA policy is likely to help. When no memory can be allocated, they return NULL. All memory allocated with the numa_alloc family of functions should be freed with numa_free.
numa_alloc_onnode allocates memory on a specific node:
void *mem = numa_alloc_onnode(MEMSIZE\_IN\_BYTES, 1); if (mem == NULL)
/* report out of memory error */
... pass mem to a thread bound to node 1 ...

memsize should be smaller than the node size. Remember that other programs may also have memory allocated on that node and allocating the full node may lead to swapping. The numa_node_size() function described below can be used to discover the node size limits of the current system automatically. It is recommended to give administrators a way to override automatic choices of the program and limit the memory consumption.
The thread must eventually free the memory with numa_free.:

numa_free(mem, memsize);

By default, numa_alloc_onnode tries to allocate memory on the specified node first, but falls back to other nodes when there is not enough memory. When numa_set_strict(1) was executed first, it does not fall back and fails the allocation when there is not enough memory on the intended node. Before that, the kernel tries to swap out memory on the node and clear other caches, which can lead to delays.
numa_alloc_interleaved allocates memory interleaved on all nodes in the system.
void *mem = numa_alloc_interleaved(MEMSIZE\_IN\_BYTES); if (mem == NULL)
/* report out of memory error */
... run memory bandwidth intensive algorithm on mem ... numa\_free(mem, MEMSIZE_IN_BYTES);

Using memory interleaved over all nodes is not always a performance win. Depending on the NUMA architecture of the machine, sometimes interleaving the program on only a subset of neighboring nodes gives better bandwidth. The numa_alloc_interleaved_subset function can be used to interleave on a specific set of nodes only.
Another function is numa_alloc_local, which allocates memory on the local node. This is normally the default for all allocations, but useful to specify explicitly when the process has a different process policy. numa_alloc allocates memory with the current process policy.
 

L I B N U M A : P R O C E S S P O L I C Y
Each thread has a default memory policy inherited from its parent. Unless changed with numactl, this policy is normally used to allocate memory preferably on the current node. When existing code in a program cannot be modified to use the numa_alloc functions described in the previous section directly, it is sometimes useful to change the process policy in a program. This way, specific subfunctions can be run with a nondefault policy without actually modifying their code. Process policy can also be used to set a policy for a child process before starting it.
numa_set_interleave_mask enables interleaving for the current thread. All future memory allocations allocate memory round robin interleaved over the nodemask specified. Passing numa_all_nodes interleaves memory to all nodes. Passing numa_no_nodes turns off interleaving again. numa_get_interleave_mask returns the current interleave mask. This can be useful to save a state before changing it in a library so it can be restored later.
numamask_t oldmask = numa_get_interleave_mask(); numa_set_interleave_mask(&numa_all_nodes);
/* run memory bandwidth intensive legacy library that allocates memory */ numa_set_interleave_mask(&oldmask);

numa_set_preferred sets the preferred node of the current thread. The memory allocator tries to allocate memory on that node first. If there is not enough free memory, it reverts to other nodes.
numa_set_membind sets a strict memory-binding mask to a nodemask. “Strict” means that the memory must be allocated on the specified nodes. When there is not enough memory free after swapping, the allocation fails. numa_get_membind returns the current memory binding mask.
numa_set_localalloc sets the process policy to the standard local allocation policy.
 

L I B N U M A : C H A N G I N G T H E P O L I C Y O F E X I S T I N G M E M O R Y A R E A S
When working with shared memory, it is often not possible to use the numa_alloc family of functions to allocate memory. The memory must be gotten from shmat() or mmap instead. To allow libnuma programs to set policy on such areas, there are additional functions for setting memory policy for already existing memory areas.
These functions only affect future allocations in the specified area. Linux uses demand paging and only allocates memory when a page is first touched by the CPU.
numa_interleave_memory sets an interleaving policy with an interleaving mask. Passing numa_all_nodes
interleaves to all nodes in the system.
void *mem = shmat( ... );	/* get shared memory */ numa_interleave_mask(mem, size, numa_all_nodes);

numa_tonode_memory allocates the memory on a specific node. numa_tonodemask_memory puts the memory onto a mask of nodes.
numa_setlocal_memory gives the memory area a policy to allocate on the current node. numa_police_memory
uses the current policy to allocate memory. This can be useful when the memory policy is changed later.

When numa_set_strict(1) was executed previously, these calls call numa_error when any of the already existing pages in the memory area do not conform to the new policy. Otherwise, existing pages are ignored.
 

L I B N U M A : B I N D I N G T O C P U S
The functions discussed so far allocate memory on specific nodes. Another part of NUMA policy is to run the thread on the CPUs of the correct node. This is done by the numa_run_on_node function, which binds the current thread to all CPUs in the node. numa_run_on_node_mask binds the thread to all the CPUs in a nodemask.
Run current thread on node 1 and allocate memory there:
numa_run_on_on_node(1); numa_set_prefered(1);

A simple way to use libnuma is the numa_bind function. It binds both the CPU and the memory of the process allocated in the future to a specific nodemask. It is equivalent to the previous example.
Bind process CPU and memory allocation to node 1 using numa_bind:
nodemask_t mask; nodemask_zero(&mask); nodemask_set(&mask 1); numa_bind(&mask);

The thread can be allowed to execute on all nodes again by binding it to numa_all_nodes:

numa_run_on_node_mask(&numa_all_nodes);

The numa_get_run_node_mask function returns the nodemask of nodes on which the current thread is allowed to run. This can be used to save and restore the scheduler affinity state before running a child process or starting a thread.
 

L I B N U M A : E N Q U I R I N G A B O U T T H E E N V I R O N M E N T
numa_node_size returns the memory size of a node. The return argument is the total size of its memory, which is not necessarily all available to the program. The second argument is a pointer that can be filled with the free memory on the node. The program can allocate node memory somewhere between the free memory (which is normally low because Linux uses free memory for caches) and the maximum memory size. When memory is allocated, Linux frees the cached file data, but allocating too much memory may lead to swapping. This function gives a hint of how much memory is available for allocation on each node, but it should be only taken as a hint, preferably with some way for the administrator to overwrite it. In general, it is recommended never to allocate more than half the total memory of a node by default unless the administrator specified more.
numa_node_to_cpus returns the CPU numbers of all CPUs in a node. This can be used to find out how many CPUs there are in a node. As arguments, it gets the node number and a pointer to an array. The last argument is the length of the array in bytes. The array is filled with a bit mask of CPU numbers. These CPU numbers can later be, for example, passed to the sched_set_affinity system call. When the array is not long enough to contain all CPUs, the function returns -1 and sets errno to ERANGE. It is recommended for applications to handle this error or pass a very big buffer, such as 512 bytes. Otherwise, there may be failures on very big machines. Linux already runs on 1024 CPU machines and is expected to be moved to even bigger machines.
 

L I B N U M A : E R R O R H A N D L I N G
Error handling in libnuma is relatively simple. The main reason for this is that errors in setting NUMA policy can usually be ignored. The worst result of a wrong NUMA policy is that the program runs more slowly than it could.
When an error occurs while setting a policy, the numa_error function is called. By default, it prints an error to stderr. When the numa_exit_on_error global variable is set, it exits the program. The function is declared weak and can be overwritten by defining a replacement function in the main program. For example, a C++ program could throw a C++ exception there.
Memory allocation functions always return NULL when no memory is available.
 

N U M A A L L O C A T I O N S T A T I S T I C S W I T H N U M A S T A T
For each node in the system, the kernel maintains some statistics pertaining to NUMA allocation status as each page is allocated. This information may be useful for testing the effectiveness of a NUMA policy.
The statistics are retrieved with the numastat command. The statistics are collected on a per-node basis. On systems with multiple CPU cores per node, numastat aggregates the results from all cores on a node to form a single result for the entire node. The numastat command reports the following statistics for each node:
When a process requests a page from a particular node and receives a page from the requested node, numa_hit is incremented for that particular node. The process may be running on any node in the system.
When a process requests a page from a particular node and instead receives a page from some other node, numa_miss is incremented at the node where the page was actually allocated. numa_foreign is incremented at the original node from which the page was requested. The process may be running on any node in the system.
interleave_hit is incremented on the node on which a page is allocated when the allocation obeys the interleave policy for the address range. In addition, numa_hit and either local_node or other_node are incremented on the node on which the page is allocated. No statistics are kept for pages allocated according to the interleave policy but not on the requested node because of its lack of free pages.
When a process requests a page and the resulting page is located on the same node as where the process is running, local_node is incremented on that particular node.
When a process requests a page and the resulting page is located on a different node than where the process is running, other_node is incremented for the node on which the page is actually allocated.
The difference between numa_miss and numa_hit and local_node and foreign_node is that the first two count hit or miss for the NUMA policy. The latter count if the allocation was on the same node as the requesting thread.
To better understand the numastat values, consider the following examples.

The following example shows which counters are incremented when a process running on node 0 requests a page on node 0 and it is allocated on node 0.

	node	3	node	2	node	1	node 0
numa_hit							+1
numa_miss							
numa_foreign							
interleave_hit							
local_node							+1
other_node							
 

The following example shows which counters are incremented when a process running on node 0 requests a page on node 0 but it is allocated on node 1 due to a shortage of free pages on node 0.

	node	3	node	2	node 1	node 0
numa_hit						
numa_miss					+1	
numa_foreign						+1
interleave_hit						
local_node						
other_node					+1	


The following example shows which counters are incremented when a process running on node 0 requests and receives a page on node 1. Note the difference between this and the first example.

	node	3	node	2	node 1	node 0
numa_hit					+1	
numa_miss						
numa_foreign						
interleave_hit						
local_node						
other_node					+1	


The following example shows which counters are incremented when a process running on node 0 requests a page on node 1 but it is allocated on node 0 due to a shortage of free pages on node 1.

	node	3	node	2	node 1	node 0
numa_hit						
numa_miss						+1
numa_foreign					+1	
interleave_hit						
local_node						+1
other_node						
 
As a further example, consider a four-node machine with 4 GB of RAM per node. Initially, numastat reports the following statistics for this machine:

	node 3	node 2	node 1	node 0
numa_hit	58956	142758	424386	319127
numa_miss	0	0	0	0
numa_foreign	0	0	0	0
interleave_hit	19204	20238	19675	20576
local_node	43013	126715	409434	305254
other_node	15943	16043	14952	13873


Now suppose that a program called memhog runs on node 1, allocating 8 GB of RAM during execution. After
memhog completes, numastat reports the following statistics:

	node 3	node 2	node 1	node 0
numa_hit	58956	142758	424386	320893
numa_miss	48365	1026046	0	0
numa_foreign	0	0	1074411	0
interleave_hit	19204	20238	19675	20577
local_node	43013	126856	1436403	307019
other_node	64308	1042089	14952	13873


Each column represents a node where an allocation event took place. The memhog program tried to allocate 1,074,411 pages from node 1 but was unable to do so. Instead, the process ended with 1,026,046 pages from node 2 and 48,365 pages from node 3.
 

S Y S T E M C A L L O V E R V I E W
NUMA API adds three new system calls: mbind, set_mempolicy, and get_mempolicy. Normally user applications should use the higher level libnuma interface and not call the system calls directly. The system call interface is declared in numaif.h include file. The system calls are currently not defined in glibc, applications that use them should link to libnuma. They may return -1 and ENOSYS in errno when the kernel does not support NUMA policy.
All of these system calls get nodemasks as arguments, similar to the nodemask_t type of libnuma. In the system call interface, they are defined as arrays of longs, each long containing a string of bits, together with an additional argument that gives the highest node number in the bitmap.
set_mempolicy sets the memory policy of the current thread. The first argument is the policy. Valid policies are MPOL_BIND, MPOL_INTERLEAVE, MPOL_DEFAULT, and MPOL_PREFERRED. These map directly to the numactl policies described earlier. How the nodemask argument is used depends on the policy. For MPOL_INTERLEAVE, it specifies the interleave mask. For MPOL_BIND and MPOL_PREFERRED, it contains the membind mask.
get_mempolicy retrieves the process memory policy of the current thread. In addition to output arguments for policy, nodemask and nodemask size, it has an address and flags arguments. When the MPOL_MF_ADDR bit is set in flags, the VMA policy of the address is returned in the other arguments. When MPOL_F_NODE is additionally set, the current node to which the address pointed is returned.
mbind sets the memory policy for a memory area. The first argument is a memory address and a length. The rest is a policy with mask and length similar to set_mempolicy. In addition, it has a flags argument. When MPOL_MF_STRICT is passed for flags, the call fails when any existing page in the mapping violates the specified policy.
For more detailed information, refer to the man pages get_mempolicy(2), set_mempolicy(2), and mbind(2).
 

L I M I T A T I O N S A N D O U T L O O K
On a bigger system, there is normally a hierarchy in the interconnect. This means that neighboring nodes can be accessed more quickly than remote nodes. NUMA API currently represents the system as a flat set of nodes. Future versions will allow the application to query node distances. Future versions of NUMA API may also allow setting a policy on the file cache.














































