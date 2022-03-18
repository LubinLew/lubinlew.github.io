# python-nmap

> https://pypi.org/project/python-nmap/

python-nmap 是一个便于使用 nmap 进行端口扫描的 python 库。
它可以轻松操作 nmap 扫描结果，对于想要自动执行扫描任务和报告的系统管理员来说，它是一个完美的工具。
它还支持 nmap 脚本输出。它甚至可以异步使用,结果一次返回一个主机到用户定义的回调函数。

安装方法: `pip3 install python-nmap`, 但是系统中需要先安装 nmap 命令。


## nmap 安装

nmap 最新版rpm包下载位置:

```bash
https://nmap.org/download.html#linux-rpm
```

nmap 需要使用 `root` 权限运行。


## 例子

```python
#!/usr/bin/python3

import nmap

nm=nmap.PortScanner()
nm.scan('10.130.12.88', arguments='-O -sV -Pn -p22,80,443')

for host in nm.all_hosts():
    print('----------------------------------------------------')
    print('Host : %s (%s)' % (host, nm[host].hostname()))
    print('State : %s' % nm[host].state())
    for proto in nm[host].all_protocols():
        print('----------')
        print('Protocol : %s' % proto)
        lport = list(nm[host][proto].keys())
        lport.sort()
        for port in lport:
            print ('port : %s\tstate : %s' % (port, nm[host][proto][port]['state']))
```