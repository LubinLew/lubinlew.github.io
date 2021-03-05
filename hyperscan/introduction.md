# 简介

Hyperscan 是一个软件的正则表达式匹配引擎，特点是高性能和灵活性。用C语言实现，对外暴露C接口。接口主要分为`编译` 和 `扫描` 两大部分。



## 编译(Compilation)

Hyperscan 可以将一组正则表达式编译成一个数据库。编译过程做了许多分析和优化工作，这样会使后续的扫描操作更加高效。

如果一个 `pattern` 不能被编译到数据库中（例如不支持的正则表达式语法，或者超过资源限制）， 编译器会返回错误。

编译好的数据库可以被 `serialized` 和 `relocated`， 这样数据库可以存储在硬盘上，可以在多个机器上移动。 也可以指定特定CPU的指令集。

详细信息见 [模式编译(Compiling Patterns)](hyperscan/getting_started.md) 。

----

## 扫描(Scanning)

当编译好一个 Hyperscan 数据库后，它就可以用来扫面内存中的数据了。Hyperscan 提供了三种扫描模式， 使用哪一种模式取决于数据存储的状态。具体介绍见 [模式扫描](hyperscan/scanning_patterns.md) 章节。

匹配的结果会通过用户自定义的回调函数(Callback)返回。如果一个数据能匹配数据库中的多个 `pattern`，是否返回多个结果的行为由回调函数的返回值决定。如果回调函数返回0 则意味着需要继续进行匹配，如果返回非零值，则意味着扫描结束，扫描函数会返回 `HS_SCAN_TERMINATED` 结束本次扫描。



对于一个给定的数据库， Hyperscan 提供以下几个保证：

- 运行时除了下面2个占用固定空间的结构外，没有任何的内存申请操作:
  
  - **Scratch space**: 用于扫描的临时内存，非持久化数据，每次扫描都可复用。
  
  - **Stream state**: 流模式独有, 这段内存用于能够追踪多块数据的多次匹配。

- 对于指定的数据库，scratch space 和 stream state (流模式) 需要的空间是固定的，并且是在编译时确定的。应用程序对内存的需求可以提前知道。提前申请好这些内存可以提升性能。

- 任何成功编译到数据库中的 `pattern` 都可以被任意数据扫描，内部没有任何限制会导致扫描发生错误。



详细信息见 [模式扫描(Scanning for Patterns)](hyperscan/scanning_patterns.md) 章节。

---

## 工具

Hyperscan库中还提供了一些用于性能测试的工具，详细信息见 [Tools](http://intel.github.io/hyperscan/dev-reference/tools.html#tools) 章节。

----

## 示例代码

 `examples/` 文件夹内有有一些简单的例子用于演示如何使用Hyperscan的API接口。
