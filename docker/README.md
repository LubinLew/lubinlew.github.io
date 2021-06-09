# Docker

## 简介

Docker 是一个开源的、轻量级的容器引擎,主要运行于 Linux 之上。
和虚拟机相比，Docker 使用容器承载应用程序，而不使用操作系统，所以它的开销很少，性能很高。
但是，Docker 对应用程序的隔离不如虚拟机彻底，所以它并不能完全取代虚拟机。

Docker 的本质是进程。是 linux 使用 cgroup 和 namespace 技术实现的软隔离。
这也是linux上没有 windows 容器的原因。


## docker 的安装

> https://docs.docker.com/engine/install/

```bash
#### centos 7, 其他系统详见上面链接

yum install -y yum-utils
yum-config-manager --add-repo  https://download.docker.com/linux/centos/docker-ce.repo
yum install docker-ce docker-ce-cli containerd.io

systemctl start  docker
systemctl enable docker
```

## 容器编排

k8s