# etcdctl工具

用户通常通过命令行工具 `etcdctl` 来与 etcd 进行交互。 本节介绍如何使用 `etcdctl`工具。 此处描述的概念也适用于gRPC API或客户端库API。

可以通过环境变量`ETCDCTL_API`  设定 `etcdctl` 与 etcd 交互的API版本， 默认情况下，从v3.4开始默认使用v3版本API，之前的版本默认v2版本API。

请注意，无法通过v3版本API查询使用v2版本API创建的键，不会返回任何数据，将以exit 0退出。

```bash
export ETCDCTL_API=3
```

[Build, Collaborate & Integrate APIs | SwaggerHub](https://app.swaggerhub.com/apis/LubinLew/etcd-rpc/v3.4.15)

[etcd-server v3.4.15 API doc](https://app.swaggerhub.com/apis-docs/LubinLew/etcd-rpc/v3.4.15)

[OpenAPI-Specification/2.0.md at master · OAI/OpenAPI-Specification · GitHub](https://github.com/OAI/OpenAPI-Specification/blob/master/versions/2.0.md)

## 命令和选项

| 命令                     | 说明                                                                  | API                                 |
| ---------------------- | ------------------------------------------------------------------- | ----------------------------------- |
| alarm disarm           | 解除所有警报(alarm)                                                       | /v3/maintenance/alarm               |
| alarm list             | 列出所有警报                                                              |                                     |
| auth disable           | 禁用身份验证                                                              | /v3/auth/disable                    |
| auth enable            | 启用身份验证                                                              | /v3/auth/enable                     |
| check datascale        | 检查在给定服务器端点上用于不同工作负载的保留数据的内存使用情况                                     |                                     |
| check perf             | 检查etcd集群的性能                                                         |                                     |
| compaction             | 压缩etcd中的事件历史记录                                                      | /v3/kv/compaction                   |
| defrag                 | 对给定端点的etcd成员的存储进行碎片整理                                               | /v3/maintenance/defragment          |
| del                    | 删除指定的键或键范围[key，range_end）                                           |                                     |
| elect                  | 观察并参加领导选举                                                           |                                     |
| endpoint hashkv        | Prints the KV history hash for each endpoint in --endpoints         |                                     |
| endpoint health        | Checks the healthiness of endpoints specified in `--endpoints` flag |                                     |
| endpoint status        | Prints out the status of endpoints specified in `--endpoints` flag  |                                     |
| get                    | Gets the key or a range of keys                                     | /v3/kv/range                        |
| help                   | Help about any command                                              |                                     |
| lease grant            | 创建租约                                                                | /v3/lease/grant                     |
| lease keep-alive       | 更新租约(续约)                                                            | /v3/lease/keepalive                 |
| lease list             | 显示所有活跃(active)的租约                                                   | /v3/lease/leases                    |
| lease revoke           | 撤销租约                                                                | /v3/lease/revoke                    |
| lease timetolive       | 获取租约信息                                                              | /v3/lease/timetolive                |
| lock                   | Acquires a named lock                                               |                                     |
| make-mirror            | 在目标etcd集群上创建镜像                                                      |                                     |
| member add             | 将成员添加到集群                                                            | /v3/cluster/member/add              |
| member list            | 列出集群中的所有成员                                                          | /v3/cluster/member/list             |
| member promote         | 提升集群中的非投票成员                                                         | /v3/cluster/member/promote          |
| member remove          | 从集群中删除成员                                                            | /v3/cluster/member/remove           |
| member update          | 更新集群中的成员                                                            | /v3/cluster/member/update           |
| migrate                | 将v2存储中的键迁移到mvcc存储                                                   |                                     |
| move-leader            | 将领导权移交给另一个etcd集群成员。                                                 | /v3/maintenance/transfer-leadership |
| put                    | Puts the given key into the store                                   | /v3/kv/put                          |
| role add               | 添加新角色                                                               | /v3/auth/role/add                   |
| role delete            | 删除角色                                                                | /v3/auth/role/delete                |
| role get               | 获取角色的详细信息                                                           | /v3/auth/role/get                   |
| role grant-permission  | Grants a key to a role                                              | /v3/auth/role/grant                 |
| role list              | 列出所有角色                                                              | /v3/auth/role/list                  |
| role revoke-permission | Revokes a key from a role                                           | /v3/auth/role/revoke                |
| snapshot restore       | 将etcd成员快照恢复到etcd目录                                                  |                                     |
| snapshot save          | 将etcd节点后端快照存储到给定文件                                                  |                                     |
| snapshot status        | 获取给定文件的后端快照状态                                                       |                                     |
| txn                    | Txn在一次交易中处理所有请求                                                     | /v3/kv/txn                          |
| user add               | 添加一个新用户                                                             | /v3/auth/user/add                   |
| user delete            | 删除一个新用户                                                             | /v3/auth/user/delete                |
| user get               | 获取一个用户的详细信息                                                         | /v3/auth/user/get                   |
| user grant-role        | Grants a role to a user                                             | /v3/auth/user/grant                 |
| user list              | 列出所有用户                                                              | /v3/auth/user/list                  |
| user passwd            | 修改用户密码                                                              | /v3/auth/user/changepw              |
| user revoke-role       | 撤消用户的角色                                                             | /v3/auth/user/revoke                |
| version                | 打印etcdctl的版本                                                        | /version                            |
| watch                  | Watches events stream on keys or prefixes                           | /v3/watch                           |

选项

| 选项                                 | 说明                                                                                                      |
| ---------------------------------- | ------------------------------------------------------------------------------------------------------- |
| --cacert=""                        | 使用此CA证书链验证TLS的安全服务器的证书                                                                                  |
| --cert=""                          | 使用此TLS证书文件识别安全客户端                                                                                       |
| --command-timeout=5s               | timeout for short running command (excluding dial timeout)                                              |
| --debug[=false]                    | enable client-side debug logging                                                                        |
| --dial-timeout=2s                  | dial timeout for client connections                                                                     |
| -d, --discovery-srv=""             | domain name to query for SRV records describing cluster endpoints                                       |
| --discovery-srv-name=""            | service name to query when using DNS discovery                                                          |
| --endpoints=[127.0.0.1:2379]       | gRPC endpoints                                                                                          |
| -h, --help[=false]                 | help for etcdctl                                                                                        |
| --hex[=false]                      | print byte strings as hex encoded strings                                                               |
| --insecure-discovery[=true]        | accept insecure SRV records describing cluster endpoints                                                |
| --insecure-skip-tls-verify[=false] | skip server certificate verification (CAUTION: this option should be enabled only for testing purposes) |
| --insecure-transport[=true]        | disable transport security for client connections                                                       |
| --keepalive-time=2s                | keepalive time for client connections                                                                   |
| --keepalive-timeout=6s             | keepalive timeout for client connections                                                                |
| --key=""                           | 使用此TLS密钥文件识别安全客户端                                                                                       |
| --password=""                      | 密码 (如果使用这个选项那么`--user`选项中就不能包含密码                                                                        |
| --user=""                          | 用户名`username[:password]` 也可以包含密码                                                                        |
| -w, --write-out="simple"           | 设置输出格式 (fields, json, protobuf, simple, table)                                                          |

## 查找版本

使用下面命令查看 `etcdctl` 版本和使用的API版本。

```bash
$ etcdctl version
etcdctl version: 3.4.15
API version: 3.4
```

---

## 数据操作

### 写入数据

应用程序通过写入键值对将其存储到etcd集群中。每个存储的键值对都通过Raft协议复制到所有etcd集群成员，以实现一致性和可靠性。

这是将键`foo`的值设置为`bar`的命令：

```bash
$ etcdctl put foo bar
OK
```

设置的数据可以设置有效时间，在etcd中是使用租约(lease)的概念。创建一个指定时间的租约，然后创建键值对时附加这个这个租约，就完成了有效时间的设置。

```bash
# 创建一个 60秒的租约， 并返回租约ID  
$ etcdctl lease grant 60
lease 32697819aaa3ce0f granted with TTL(60s)

# 使用租约ID创建数据
$ etcdctl put foo1 bar1 --lease=32697819aaa3ce0f 
OK

# 获取数据
$ etcdctl get foo1
foo1
bar1

# 等待租约的60秒过期后，再获取数据
$ etcdctl get foo1
# 不会返回任何数据，租约过期， 租约ID和相关的数据都没了
```

注意: 上面命令中的租约ID，当创建租约的时候就已经开始计时，即使没有人使用它，它自己也会过期，过期后就自动删除了。

### 读取数据

应用程序可以从etcd集群中读取键的值。 查询可以读取单个键或键范围。

假定etcd集群中存储着下列数据：

```bash
foo = bar
foo1 = bar1
foo2 = bar2
foo3 = bar3
```

这是读取键`foo`的值的命令：

```bash
$ etcdctl get foo
foo
bar
```

以下是以十六进制格式读取键`foo`的值的命令：

```bash
$ etcdctl get foo --hex
\x66\x6f\x6f          # Key
\x62\x61\x72          # Value
```

这是只读取键`foo`的值的命令：

```bash
$ etcdctl get foo --print-value-only
bar
```

这是范围从`foo`到`foo3`的键的命令：

```bash
$ etcdctl get foo foo3
foo
bar
foo1
bar1
foo2
bar2
```

注意上面的例子中不包含 `foo3` ，这是因为范围是一个半闭合区间 `[foo, foo3)`, 不包含 `foo3`.

这是覆盖所有以`foo`开头的键的命令：

```bash
$ etcdctl get --prefix foo
foo
bar
foo1
bar1
foo2
bar2
foo3
bar3
```

这是列出所有以`foo`开头的键范围内的命令，将结果数限制为2：

```bash
$ etcdctl get --prefix --limit=2 foo
foo
bar
foo1
bar1
```

### 查询键的过去版本的值

Applications may want to read superseded versions of a key. For example, an application may wish to roll back to an old configuration by accessing an earlier version of a key. Alternatively, an application may want a consistent view over multiple keys through multiple requests by accessing key history.
Since every modification to the etcd cluster key-value store increments the global revision of an etcd cluster, an application can read superseded keys by providing an older etcd revision.

应用程序可能想要读取密钥的替代版本。 例如，应用程序可能希望通过访问密钥的早期版本来回滚到旧配置。 或者，应用程序可能需要通过访问密钥历史记录来通过多个请求对多个密钥进行一致的查看。
由于对etcd集群键值存储的每次修改都会增加etcd集群的全局修订，因此应用程序可以通过提供较旧的etcd修订来读取被取代的密钥。

Suppose an etcd cluster already has the following keys:

```bash
foo = bar         # revision = 2
foo1 = bar1       # revision = 3
foo = bar_new     # revision = 4
foo1 = bar1_new   # revision = 5
```

Here are an example to access the past versions of keys:

```bash
$ etcdctl get --prefix foo # access the most recent versions of keys
foo
bar_new
foo1
bar1_new

$ etcdctl get --prefix --rev=4 foo # access the versions of keys at revision 4
foo
bar_new
foo1
bar1

$ etcdctl get --prefix --rev=3 foo # access the versions of keys at revision 3
foo
bar
foo1
bar1

$ etcdctl get --prefix --rev=2 foo # access the versions of keys at revision 2
foo
bar

$ etcdctl get --prefix --rev=1 foo # access the versions of keys at revision 1
```

### 读取大于或等于指定键的字节值的键

Applications may want to read keys which are greater than or equal to the byte value of the specified key.

Suppose an etcd cluster already has the following keys:

```bash
a = 123
b = 456
z = 789
```

Here is the command to read keys which are greater than or equal to the byte value of key `b` :

```bash
$ etcdctl get --from-key b
b
456
z
789
```

### 删除数据

Applications can delete a key or a range of keys from an etcd cluster.

Suppose an etcd cluster already has the following keys:

```bash
foo = bar
foo1 = bar1
foo3 = bar3
zoo = val
zoo1 = val1
zoo2 = val2
a = 123
b = 456
z = 789
```

Here is the command to delete key `foo`:

```bash
$ etcdctl del foo
1 # one key is deleted
```

Here is the command to delete keys ranging from `foo` to `foo9`:

```bash
$ etcdctl del foo foo9
2 # two keys are deleted
```

Here is the command to delete key `zoo` with the deleted key value pair returned:

```bash
$ etcdctl del --prev-kv zoo
1   # one key is deleted
zoo # deleted key
val # the value of the deleted key
```

Here is the command to delete keys having prefix as `zoo`:

```bash
$ etcdctl del --prefix zoo
2 # two keys are deleted
```

Here is the command to delete keys which are greater than or equal to the byte value of key `b` :

```bash
$ etcdctl del --from-key b
2 # two keys are deleted
```

---

## 监控数据变化

### 监控数据变化

应用程序可以监视一个或多个键的更新。这是一个监视键`foo`值变化的例子：

```bash
$ etcdctl watch foo
# in another terminal: etcdctl put foo bar
PUT
foo
bar
```

这是以十六进制格式监视键`foo`的命令：

```bash
$ etcdctl watch foo --hex
# in another terminal: etcdctl put foo bar
PUT
\x66\x6f\x6f          # Key
\x62\x61\x72          # Value
```

这是监视从`foo`到`foo9`范围键的命令：

```bash
$ etcdctl watch foo foo9
# in another terminal: etcdctl put foo bar
PUT
foo
bar
# in another terminal: etcdctl put foo1 bar1
PUT
foo1
bar1
```

这是监视前缀为`foo`的键的命令：

```bash
$ etcdctl watch --prefix foo
# in another terminal: etcdctl put foo bar
PUT
foo
bar
# in another terminal: etcdctl put fooz1 barz1
PUT
fooz1
barz1
```

这是同时监视多个键`foo`和`zoo`的方法：

```bash
$ etcdctl watch -i
$ watch foo
$ watch zoo
# in another terminal: etcdctl put foo bar
PUT
foo
bar
# in another terminal: etcdctl put zoo val
PUT
zoo
val
```

### 观看键的历史变化

Applications may want to watch for historical changes of keys in etcd. For example, an application may wish to receive all the modifications of a key; if the application stays connected to etcd, then `watch` is good enough. However, if the application or etcd fails, a change may happen during the failure, and the application will not receive the update in real time. To guarantee the update is delivered, the application must be able to watch for historical changes to keys. To do this, an application can specify a historical revision on a watch, just like reading past version of keys.

应用程序可能希望监视etcd中键的更改历史。 例如，应用程序可能希望接收密钥的所有修改记录。 如果应用程序保持与etcd的连接，那么`watch`就足够了。 但是，如果应用程序或etcd失败，则在失败期间可能会发生更改，并且该应用程序将不会实时接收更新。 为了确保更新已交付，应用程序必须能够监视键的更改历史。为此，应用程序可以在`watch`指令上指定历史修订，就像读取键的过去版本一样。

假设我们完成了以下操作序列：

```bash
$ etcdctl put foo bar         # revision = 2
OK
$ etcdctl put foo1 bar1       # revision = 3
OK
$ etcdctl put foo bar_new     # revision = 4
OK
$ etcdctl put foo1 bar1_new   # revision = 5
OK
```

这是观看历史变化的示例：

```bash
# watch for changes on key `foo` since revision 2
$ etcdctl watch --rev=2 foo
PUT
foo
bar
PUT
foo
bar_new
```

```bash
# watch for changes on key `foo` since revision 3
$ etcdctl watch --rev=3 foo
PUT
foo
bar_new
```

以下是仅从最近的历史更改中观看的示例：

```bash
# watch for changes on key `foo` and return last revision value along with modified value
$ etcdctl watch --prev-kv foo
# in another terminal: etcdctl put foo bar_latest
PUT
foo         # key
bar_new     # last value of foo key before modification
foo         # key
bar_latest  # value of foo key after modification
```

### 观看进度

Applications may want to check the progress of a watch to determine how up-to-date the watch stream is. 

应用程序可能需要检查`watch`的进度，以确定手表流的最新状态。

For example, if a watch is used to update a cache, it can be useful to know if the cache is stale compared to the revision from a quorum read. 

例如，如果使用`watch`来更新缓存，则与从仲裁读取的修订版相比，了解缓存是否陈旧可能会很有用。

Progress requests can be issued using the "progress" command in interactive watch session to ask the etcd server to send a progress notify update in the watch stream:

可以在交互式监视会话中使用`progress`命令发出进度请求，以要求etcd服务器在监视流中发送进度通知更新：

```bash
$ etcdctl watch -i
$ watch a
$ progress
progress notify: 1
# in another terminal: etcdctl put x 0
# in another terminal: etcdctl put y 1
$ progress
progress notify: 3
```

Note: The revision number in the progress notify response is the revision from the local etcd server node that the watch stream is connected to. 

If this node is partitioned and not part of quorum, this progress notify revision might be lower than the revision returned by a quorum read against a non-partitioned etcd server node.

注意：进度通知响应中的修订号是监视流连接到的本地etcd服务器节点的修订。 如果此节点已分区且不是仲裁的一部分，则此进度通知修订版可能低于针对未分区的etcd服务器节点读取的仲裁所返回的修订版。

---

## 数据精简

如前所述，etcd会保留修订版本，以便应用程序可以读取`key`的过去版本。 但是，为避免累积无数历史记录，精简过去的修订很重要。 压缩后，etcd删除历史修订版，释放资源以供将来使用。 在压缩修订版之前，所有带有修订的替代数据将不可用。

Here is the command to compact the revisions:

```bash
$ etcdctl compact 5
compacted revision 5

# any revisions before the compacted one are not accessible
$ etcdctl get --rev=4 foo
Error:  rpc error: code = 11 desc = etcdserver: mvcc: required revision has been compacted
```

Note: The current revision of etcd server can be found using get command on any key (existent or non-existent) in json format. Example is shown below for mykey which does not exist in etcd server:

```bash
$ etcdctl get mykey -w=json
{"header":{"cluster_id":14841639068965178418,"member_id":10276657743932975437,"revision":15,"raft_term":4}}
```

----

## 租约

> [etcd租约机制 - 一泽涟漪](https://www.cnblogs.com/ilifeilong/p/11607738.html)

### 新建租约

Applications can grant leases for keys from an etcd cluster. When a key is attached to a lease, its lifetime is bound to the lease's lifetime which in turn is governed by a time-to-live (TTL). Each lease has a minimum time-to-live (TTL) value specified by the application at grant time. The lease's actual TTL value is at least the minimum TTL and is chosen by the etcd cluster. Once a lease's TTL elapses, the lease expires and all attached keys are deleted.

Here is the command to grant a lease:

```bash
# grant a lease with 10 second TTL
$ etcdctl lease grant 10
lease 32695410dcc0ca06 granted with TTL(10s)

# attach key foo to lease 32695410dcc0ca06
$ etcdctl put --lease=32695410dcc0ca06 foo bar
OK
```

### 撤销租约

Applications revoke leases by lease ID. Revoking a lease deletes all of its attached keys.

Suppose we finished the following sequence of operations:

```bash
$ etcdctl lease grant 10
lease 32695410dcc0ca06 granted with TTL(10s)
$ etcdctl put --lease=32695410dcc0ca06 foo bar
OK
```

Here is the command to revoke the same lease:

```bash
$ etcdctl lease revoke 32695410dcc0ca06
lease 32695410dcc0ca06 revoked

$ etcdctl get foo
# empty response since foo is deleted due to lease revocation
```

### 续约

Applications can keep a lease alive by refreshing its TTL so it does not expire.

Suppose we finished the following sequence of operations:

```bash
$ etcdctl lease grant 10
lease 32695410dcc0ca06 granted with TTL(10s)
```

Here is the command to keep the same lease alive:

```bash
$ etcdctl lease keep-alive 32695410dcc0ca06
lease 32695410dcc0ca06 keepalived with TTL(10)
lease 32695410dcc0ca06 keepalived with TTL(10)
lease 32695410dcc0ca06 keepalived with TTL(10)
...
```

### 获取租约信息

Applications may want to know about lease information, so that they can be renewed or to check if the lease still exists or it has expired. Applications may also want to know the keys to which a particular lease is attached.

Suppose we finished the following sequence of operations:

```bash
# grant a lease with 500 second TTL
$ etcdctl lease grant 500
lease 694d5765fc71500b granted with TTL(500s)

# attach key zoo1 to lease 694d5765fc71500b
$ etcdctl put zoo1 val1 --lease=694d5765fc71500b
OK

# attach key zoo2 to lease 694d5765fc71500b
$ etcdctl put zoo2 val2 --lease=694d5765fc71500b
OK
```

Here is the command to get information about the lease:

```bash
$ etcdctl lease timetolive 694d5765fc71500b
lease 694d5765fc71500b granted with TTL(500s), remaining(258s)
```

Here is the command to get information about the lease along with the keys attached with the lease:

```bash
$ etcdctl lease timetolive --keys 694d5765fc71500b
lease 694d5765fc71500b granted with TTL(500s), remaining(132s), attached keys([zoo2 zoo1])

# if the lease has expired or does not exist it will give the below response:
Error:  etcdserver: requested lease not found
```
