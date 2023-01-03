# kubeadm 简介

[第193期 | Kubernetes一键部署利器：kubeadm-极客时间](https://time.geekbang.org/column/article/192545)

K8S的部署非常复杂，官方提供了 `kubeadm` 这个工具来帮助使用者快速部署k8s集群。主要是下面两个命令。

```bash
# 创建 Master 节点
kubeadm init

# 将节点加入集群
kubeadm join
```

## kubeadm 工作流程

### 部署前检查

当你执行 kubeadm init 指令后，kubeadm 首先要做的，是一系列的检查工作，以确定这台机器可以用来部署 Kubernetes。这一步检查，我们称为“Preflight Checks”，它可以为你省掉很多后续的麻烦。其实，Preflight Checks 包括了很多方面，比如：

- Linux 内核的版本必须是否是 3.10 以上？
- Linux Cgroups 模块是否可用？
- 机器的 hostname 是否标准？在 Kubernetes 项目里，机器的名字以及一切存储在 Etcd 中的 API 对象，都必须使用标准的 DNS 命名（RFC 1123）。
- 用户安装的 kubeadm 和 kubelet 的版本是否匹配？
- 机器上是不是已经安装了 Kubernetes 的二进制文件？
- Kubernetes 的工作端口 10250/10251/10252 端口是不是已经被占用？
- ip、mount 等 Linux 指令是否存在？
- Docker 是否已经安装？
- ……

### 生成证书和目录

在通过了 Preflight Checks 之后，kubeadm 要为你做的，是生成 Kubernetes 对外提供服务所需的各种证书和对应的目录。

Kubernetes 对外提供服务时，除非专门开启“不安全模式”，否则都要通过 HTTPS 才能访问 kube-apiserver。这就需要为 Kubernetes 集群配置好证书文件。

kubeadm 为 Kubernetes 项目生成的证书文件都放在 Master 节点的 `/etc/kubernetes/pki` 目录下。

证书生成后，kubeadm 接下来会为其他组件生成访问 kube-apiserver 所需的配置文件。这些文件的路径是：/etc/kubernetes/xxx.conf：ls /etc/kubernetes/admin.conf controller-manager.conf kubelet.conf scheduler.conf这些文件里面记录的是，当前这个 Master 节点的服务器地址、监听端口、证书目录等信息。这样，对应的客户端（比如 scheduler，kubelet 等），可以直接加载相应的文件，使用里面的信息与 kube-apiserver 建立安全连接。接下来，kubeadm 会为 Master 组件生成 Pod 配置文件。我已经在上一篇文章中和你介绍过 Kubernetes 有三个 Master 组件 ku
