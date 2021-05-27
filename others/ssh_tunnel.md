# SSH隧道

> [SSH端口转发详解及实例 - 珂儿吖](https://www.cnblogs.com/keerya/p/7612715.html)



 B 无法直接访问 A, 但是 A 可以直接访问 B, 使用下面方法建立隧道, 则可以在 B 上访问 A。

```bash
# 客户端 A （与 B 建立 SSH连接，并要求 B 监听 2222 端口， 之后 2222端口的流量都转发到建立的SSh连接上）
ssh -NR 2222:localhost:22 root@192.168.12.62 -p 22  -o StrictHostKeyChecking=no

# 客户端 B（连接 2222 端口，就相当于ssh到可客户端A）
ssh root@127.0.0.1 -p 2222
```


