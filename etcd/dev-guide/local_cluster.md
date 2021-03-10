# 搭建本地集群

对于测试和开发部署，最快，最简单的方法是搭建本地集群。 对于生产部署，请参阅“群集”章节。

## 本地独立(standalone)集群

### 启动集群

运行以下命令，将etcd集群部署为独立集群：

```bash
$ ./etcd
...
```

这时etcd默认监听 `localhost:2379` .

### 与集群互动

使用`etcdctl` 命令与正在运行的集群互动

1. 在集群中存储键/值对
   
   ```bash
     $ ./etcdctl put foo bar
     OK
   ```
   
    如果回显“OK”，则成功存储键值对。

2. 检索键`foo` 的值:
   
   ```bash
   $ ./etcdctl get foo
   bar
   ```
   
   如果返回“ bar”，则表明与etcd集群的交互正在按预期进行。

## 本地多成员(multi-member)集群

### 启动集群

etcd的git仓库提供了一个[`Procfile`](https://github.com/etcd-io/etcd/blob/master/Procfile)文件，可轻松配置本地多成员集群。 要启动多成员集群，请遵循以下操作：

```bash
cat Procfile

# Use goreman to run `go get github.com/mattn/goreman`
# 根据实际情况修改etcd的执行路径

etcd1: /bin/etcd --name infra1 --listen-client-urls http://127.0.0.1:2379 --advertise-client-urls http://127.0.0.1:2379 --listen-peer-urls http://127.0.0.1:12380 --initial-advertise-peer-urls http://127.0.0.1:12380 --initial-cluster-token etcd-cluster-1 --initial-cluster 'infra1=http://127.0.0.1:12380,infra2=http://127.0.0.1:22380,infra3=http://127.0.0.1:32380' --initial-cluster-state new --enable-pprof --logger=zap --log-outputs=stderr
etcd2: /bin/etcd --name infra2 --listen-client-urls http://127.0.0.1:22379 --advertise-client-urls http://127.0.0.1:22379 --listen-peer-urls http://127.0.0.1:22380 --initial-advertise-peer-urls http://127.0.0.1:22380 --initial-cluster-token etcd-cluster-1 --initial-cluster 'infra1=http://127.0.0.1:12380,infra2=http://127.0.0.1:22380,infra3=http://127.0.0.1:32380' --initial-cluster-state new --enable-pprof --logger=zap --log-outputs=stderr
etcd3: /bin/etcd --name infra3 --listen-client-urls http://127.0.0.1:32379 --advertise-client-urls http://127.0.0.1:32379 --listen-peer-urls http://127.0.0.1:32380 --initial-advertise-peer-urls http://127.0.0.1:32380 --initial-cluster-token etcd-cluster-1 --initial-cluster 'infra1=http://127.0.0.1:12380,infra2=http://127.0.0.1:22380,infra3=http://127.0.0.1:32380' --initial-cluster-state new --enable-pprof --logger=zap --log-outputs=stderr
#proxy: bin/etcd grpc-proxy start --endpoints=127.0.0.1:2379,127.0.0.1:22379,127.0.0.1:32379 --listen-addr=127.0.0.1:23790 --advertise-client-url=127.0.0.1:23790 --enable-pprof

# A learner node can be started using Procfile.learner
```

1. 安装`goreman` 来控制基于 `Procfile` 配置文件的应用，下载 [golang](https://golang.google.cn/) 环境，国内还需要配置 [goproxy](https://goproxy.cn/)
   
   ```bash
   $ go get github.com/mattn/goreman
   ```

2. 使用etcd仓库存的 `Procfile`文件利用`goreman`启动集群，注意`goreman`在 `$GOPATH/bin` 下
   
   ```bash
   $ goreman -f Procfile start
   ```
   
   这应就启动三个etcd实例，分别监听 `localhost:2379`, `localhost:22379`, `localhost:32379` ，并形成了一个集群.

### 与集群互动

使用`etcdctl` 与集群交互:

1. 打印成员列表:
   
   ```
   $ etcdctl --write-out=table --endpoints=localhost:2379 member list
   ```
   
    命令结果如下:
   
   ```
   +------------------+---------+--------+------------------------+------------------------+------------+
   |        ID        | STATUS  |  NAME  |       PEER ADDRS       |      CLIENT ADDRS      | IS LEARNER |
   +------------------+---------+--------+------------------------+------------------------+------------+
   | 8211f1d0f64f3269 | started | infra1 | http://127.0.0.1:12380 |  http://127.0.0.1:2379 |      false |
   | 91bc3c398fb3c146 | started | infra2 | http://127.0.0.1:22380 | http://127.0.0.1:22379 |      false |
   | fd422379fda50e48 | started | infra3 | http://127.0.0.1:32380 | http://127.0.0.1:32379 |      false |
   +------------------+---------+--------+------------------------+------------------------+------------+
   ```

2. 将示例键/值对存储在集群中:
   
   ```
   $ etcdctl put foo bar
   OK
   ```
   
   如果打印“OK”，则成功存储键值对。

### 容错测试

要检测 etcd 的容错能力，杀死集群中的成员并尝试获取键值。

1. 标识要停止的成员的进程名称。
   
    Procfile列出了多成员集群的属性。例如每行开始的 `etcd1`,`etcd2`,`etcd3`标识进程实例。

2. 停止其中一个成员`etcd2`:
   
   ```bash
   # kill etcd2
   $ goreman run stop etcd2
   ```

3. 存储一个键值对:
   
   ```bash
   $ etcdctl put key hello
   OK
   ```

4. 获取刚才设置的键值对:
   
   ```
   $ etcdctl get key
   hello
   ```

5. 从刚才停止的`etcd2`获取设置的键值对:
   
   ```
   $ etcdctl --endpoints=localhost:22379 get key
   ```
   
    该命令应显示由连接失败引起的错误:
   
   ```json
   {
   "level":"warn","ts":"2021-03-10T09:19:32.636+0800",
   "caller":"clientv3/retry_interceptor.go:62",
   "msg":"retrying of unary invoker failed",
   "target":"endpoint://client-5f62be90-7e0d-4f41-b35c-53e2c26707e8/localhost:22379",
   "attempt":0,
   "error":"rpc error: code = DeadlineExceeded desc = latest balancer error: all SubConns are in TransientFailure, latest connection error: connection error: desc = \"transport: Error while dialing dial tcp [::1]:22379: connect: connection refused\""
   }
   Error: context deadline exceeded
   
   ```

6. 重新启动已停止的成员:
   
   ```bash
   $ goreman run restart etcd2
   ```

7. 从重新启动的成员那里获取键值对:
   
   ```bash
   $ etcdctl --endpoints=localhost:22379 get key
   hello
   ```
   
    重新启动成员重新建立连接。 `etcdctl` 又可以正确获取键值对。 要了解有关与etcd交互的更多信息，请阅读与etcd交互部分。
