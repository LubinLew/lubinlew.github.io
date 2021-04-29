# Redis 通知机制

> [Redis Keyspace Notifications](https://redis.io/topics/notifications)
> 
> [Redis 事件通知 keyspace & keyevent notification](https://www.cnblogs.com/yeyusheng/p/8533362.html)



## 功能概述

键空间通知(Keyspace notifications)允许客户端订阅发布/订阅通道(channel)，以便接收以某种方式影响Redis数据集的事件。

可以接收的事件的示例是：

- 所有影响给定键的命令。
- 所有键都接受LPUSH操作。
- 所有密钥都在数据库0中过期。

事件是使用Redis的常规Pub/Sub层传递的，因此实现Pub / Sub的客户端可以使用此功能，而无需进行修改。

需要注意的是由于Redis的发布订阅机制是不可靠的，也就是说当一个客户端断开连接并在之后重新连接的话, 那么在该时间段内传递的所有事件将会丢失。

## 事件类型

通过为影响Redis数据空间的每个操作发送两种不同类型的事件来实现键空间通知。例如，针对数据库中命名的键的[DEL](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/del)操作将触发两条消息的传递，这完全等同于以下两个[PUBLISH](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/publish)命令：`mykey``0`[](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/publish)

```bash
PUBLISH __keyspace@0__:mykey del
PUBLISH __keyevent@0__:del mykey
```

很容易看出一个通道如何使我们能够收听所有针对键的事件`mykey`，而另一个通道如何获得与作为`del`操作目标的所有键有关的信息。

`keyspace`在通道中带有前缀的第一种事件称为**Key-space通知**，而在通道中带有前缀的第二种**事件**`keyevent`称为**Key-event通知**。

在上面的示例中`del`，为密钥生成了一个事件`mykey`。发生的事情是：

- 密钥空间通道将事件名称作为消息接收。
- 密钥事件通道接收密钥名称作为消息。

可以仅启用一种通知，以便仅传递我们感兴趣的事件的子集。

## 配置

默认情况下，键空间事件通知是禁用的，因为该功能虽然不太明智，但会占用一些CPU资源。使用`notify-keyspace-events`redis.conf或通过**CONFIG SET**启用通知。

将参数设置为空字符串将禁用通知。为了启用此功能，使用了一个非空字符串，该字符串由多个字符组成，其中每个字符根据下表具有特殊含义：

```
K     Keyspace events, published with __keyspace@<db>__ prefix.
E     Keyevent events, published with __keyevent@<db>__ prefix.
g     Generic commands (non-type specific) like DEL, EXPIRE, RENAME, ...
$     String commands
l     List commands
s     Set commands
h     Hash commands
z     Sorted set commands
t     Stream commands
d     Module key type events
x     Expired events (events generated every time a key expires)
e     Evicted events (events generated when a key is evicted for maxmemory)
m     Key miss events (events generated when a key that doesn't exist is accessed)
A     Alias for "g$lshztxed", so that the "AKE" string means all the events except "m".
```

字符串中至少应存在`K`或`E`应该存在，否则无论字符串的其余部分如何都不会传递任何事件。

例如，仅启用列表的键空间事件，必须将配置参数设置为`Kl`，依此类推。

该字符串`KEA`可用于启用每个可能的事件。

## 由不同命令生成的事件

根据以下列表，不同的命令会生成不同类型的事件。

- [DEL](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/del)`del`为每个删除的键生成一个事件。
- [RENAME](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/rename)生成两个事件，一个`rename_from`是源密钥的`rename_to`事件，一个是目的密钥的事件。
- [MOVE](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/move)生成两个事件，一个`move_from`是源密钥的`move_to`事件，一个是目的密钥的事件。
- [COPY](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/copy)生成一个`copy_to`事件。
- [](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/migrate)`del`如果删除了源密钥，则[MIGRATE会](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/migrate)生成一个事件。
- [RESTORE](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/restore)`restore`为密钥生成一个事件。
- [EXPIRE](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/expire)及其所有变体（[PEXPIRE](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/pexpire)，[EXPIREAT](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/expireat)，[PEXPIREAT](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/pexpireat)）`expire`在以正超时（或未来时间戳记）调用时会生成事件。请注意，当过去使用负超时值或时间戳来调用这些命令时，该键将被删除，而只会`del`生成一个事件。
- [SORT](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/sort)用于设置新密钥`sortstore`时`STORE`会生成一个事件。如果结果列表为空，并且使用了该`STORE`选项，并且已经存在具有该名称的键，则结果是该键被删除，因此`del`在这种情况下会生成一个事件。
- [SET](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/set)及其所有变体（[SETEX](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/setex)，[SETNX](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/setnx)，[GETSET](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/getset)）都会生成`set`事件。但是[SETEX](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/setex)也会生成一个`expire`事件。
- [MSET](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/mset)`set`为每个键生成一个单独的事件。
- [SETRANGE](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/setrange)生成一个`setrange`事件。
- [INCR](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/incr)，[DECR](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/decr)，[INCRBY](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/incrby)，[DECRBY](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/decrby)命令均会生成`incrby`事件。
- [INCRBYFLOAT](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/incrbyfloat)生成一个`incrbyfloat`事件。
- [APPEND](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/append)生成一个`append`事件。
- [](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/lpush)即使在可变情况下，[LPUSH](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/lpush)和[LPUSHX也会](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/lpushx)生成一个`lpush`事件。
- [](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/rpush)即使在可变情况下，[RPUSH](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/rpush)和[RPUSHX也会](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/rpushx)生成一个`rpush`事件。
- [RPOP会](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/rpop)生成一个`rpop`事件。此外，`del`如果因为弹出列表中的最后一个元素而删除了键，则会生成一个事件。
- [LPOP](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/lpop)生成一个`lpop`事件。此外，`del`如果因为弹出列表中的最后一个元素而删除了键，则会生成一个事件。
- [LINSERT](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/linsert)生成一个`linsert`事件。
- [LSET](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/lset)生成一个`lset`事件。
- [LREM会](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/lrem)生成一个`lrem`事件，`del`如果结果列表为空并且已删除密钥，则[还会](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/lrem)生成一个事件。
- [LTRIM会](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/ltrim)生成一个`ltrim`事件，`del`如果结果列表为空并且已删除密钥，则还会生成一个事件。
- [RPOPLPUSH](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/rpoplpush)和[BRPOPLPUSH](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/brpoplpush)生成一个`rpop`事件和一个`lpush`事件。在这两种情况下，订单都是有保证的（`lpush`事件将始终在`rpop`事件发生后交付）。此外，`del`如果结果列表的长度为零且密钥被删除，则将生成一个事件。
- [LMOVE](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/lmove)和[BLMOVE](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/blmove)生成一个`lpop`/`rpop`事件（取决于[wherefrom](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/blmove)参数）和一个`lpush`/`rpush`事件（取决于whereto参数）。在这两种情况下，顺序都得到保证（`lpush`/`rpush`事件将始终在`lpop`/`rpop`事件之后交付）。此外，`del`如果结果列表的长度为零且密钥被删除，则将生成一个事件。
- [HSET](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/hset)，[HSETNX](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/hsetnx)和[HMSET](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/hmset)都产生一个`hset`事件。
- [HINCRBY](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/hincrby)生成一个`hincrby`事件。
- [HINCRBYFLOAT](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/hincrbyfloat)生成一个`hincrbyfloat`事件。
- [](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/hdel)如果生成的哈希为空并且密钥被删除`hdel`，则[HDEL会](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/hdel)生成一个事件，并生成一个附加`del`事件。
- [](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/sadd)`sadd`即使在可变情况下，[SADD](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/sadd)也会生成一个事件。
- [](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/srem)如果结果集为空并且密钥被删除`srem`，则[SREM会](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/srem)生成一个事件和一个附加`del`事件。
- [SMOVE](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/smove)`srem`为源密钥生成一个事件，`sadd`为目标密钥生成一个事件。
- [SPOP会](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/spop)生成一个`spop`事件，`del`如果结果集为空并且已删除密钥，则还会生成一个附加事件。
- [SINTERSTORE](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/sinterstore)，[SUNIONSTORE](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/sunionstore)，[SDIFFSTORE](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/sdiffstore)产生`sinterstore`，`sunionstore`，`sdiffstore`分别事件。在特殊情况下，结果集为空，并且存储结果的键已经存在，`del`由于移除了键，因此会生成一个事件。
- `ZINCR`产生一个`zincr`事件。
- [](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/zadd)`zadd`即使添加了多个元素，[ZADD也会](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/zadd)生成一个事件。
- [](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/zrem)`zrem`即使删除了多个元素，[ZREM也会](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/zrem)生成一个事件。当结果排序后的集合为空并生成密钥时，将生成一个附加`del`事件。
- `ZREMBYSCORE`生成一个`zrembyscore`事件。当结果排序后的集合为空并生成密钥时，将生成一个附加`del`事件。
- `ZREMBYRANK`生成一个`zrembyrank`事件。当结果排序后的集合为空并生成密钥时，将生成一个附加`del`事件。
- [ZDIFFSTORE](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/zdiffstore)，[ZINTERSTORE](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/zinterstore)和[ZUNIONSTORE](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/zunionstore)分别生成`zdiffstore`，`zinterstore`和`zunionstore`事件。在特殊情况下，排序后的结果集为空，并且存储结果的键已经存在，`del`由于移除了键，因此会生成一个事件。
- [XADD](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/xadd)生成一个`xadd`事件，`xtrim`与`MAXLEN`子命令一起使用时，可能会在事件之后。
- [](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/xdel)`xdel`即使删除了多个条目，[XDEL也会](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/xdel)生成一个事件。
- `XGROUP CREATE`产生一个`xgroup-create`事件。
- `XGROUP CREATECONSUMER`产生一个`xgroup-createconsumer`事件。
- `XGROUP DELCONSUMER`产生一个`xgroup-delconsumer`事件。
- `XGROUP DESTROY`产生一个`xgroup-destroy`事件。
- `XGROUP SETID`产生一个`xgroup-setid`事件。
- `XSETID`产生一个`xsetid`事件。
- [XTRIM](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/xtrim)生成一个`xtrim`事件。
- [](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/persist)`persist`如果已成功删除与密钥关联的到期时间，则[PERSIST会](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/persist)生成一个事件。
- 每次由于具有过期时间而将具有生存时间的键从数据集中删除时，`expired`都会生成一个事件。
- 每当由于`maxmemory`策略而从数据集中收回密钥以释放内存时，`evicted`都会生成一个事件。

**重要**说明：只有真正修改了目标键后，所有命令才会生成事件。例如，从[集合中](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/srem)删除不存在的元素的[SREM](https://jwotwh5u7dp2edg6zam3kd6mry-ac4c6men2g7xr2a-redis-io.translate.goog/commands/srem)实际上不会更改键的值，因此不会生成任何事件。

如果不确定给定命令如何生成事件，那么最简单的方法就是观察自己：

```
$ redis-cli config set notify-keyspace-events KEA
$ redis-cli --csv psubscribe '__key*__:*'
Reading messages... (press Ctrl-C to quit)
"psubscribe","__key*__:*",1
```

此时，`redis-cli`在另一个终端中使用可将命令发送到Redis服务器并观察生成的事件：

```
"pmessage","__key*__:*","__keyspace@0__:foo","set"
"pmessage","__key*__:*","__keyevent@0__:set","foo"
...
```

## 过期事件的时间

Redis通过两种方式使具有生存时间的密钥失效：

- 当命令访问密钥并且发现密钥已过期时。
- 通过后台系统，该系统逐步在后台查找过期的密钥，以便能够收集从未访问过的密钥。

这些`expired`事件是在访问密钥时发现的，并且发现上述系统之一已过期，因此，不能保证Redis服务器将`expired`在密钥生存时间达到该时间时生成事件。值为零。

如果没有命令始终以该键为目标，并且有许多键与TTL相关联，则在有效键时间下降到零的时间与`expired`生成事件的时间之间可能会有很大的延迟。

基本上，`expired`事件**是在Redis服务器删除密钥时生成的，**而不是在理论上达到零的生存时间时生成的。

## 集群中的事件

如上所述，Redis群集的每个节点都会生成有关其自己的键空间子集的事件。但是，与群集中的常规发布/订阅通信不同，事件的通知**不会**广播到所有节点。换句话说，键空间事件是特定于节点的。这意味着，要接收集群的所有键空间事件，客户端需要订阅每个节点。

## 历史

- `>= 6.0`：添加了关键错过事件。
