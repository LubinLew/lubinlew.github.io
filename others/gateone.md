# GateOne - WebSSH

> [【webssh】网页上的SSH终端 - K.Takanashi - 博客园](https://www.cnblogs.com/franknihao/p/8963634.html)

GateOne 是一个网页版的ssh工具, github地址 [liftoff/GateOne](https://github.com/liftoff/GateOne), 这个库基本已经停止维护, 不支持Python3, 但是胜在简单易用。

## 安装

> [CentOS安装配置GateOne实现Web终端SSH功能 | 老左笔记](https://www.laozuo.org/10703.html)

```bash
pip install 'tornado==2.4.1'
pip install Pillow

# 下载安装 gateone
wget https://github.com/downloads/liftoff/GateOne/gateone-1.1.tar.gz
tar -zxvf gateone-1.1.tar.gz
cd GateOne
python setup.py install
```

## 配置

配置文件是 `/opt/gateone/server.conf` 可以配置端口(默认端口443), HTTPS证书等

## 运行

```bash
cd /opt/gateone
python2 gateone.py
```

## 访问

https://host/
