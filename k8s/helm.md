# Helm

Helm 是 Kubernetes 的软件包管理工具。Helm 由客户端组件 helm 和服务端组件 Tiller 组成, 能够将一组K8S资源打包统一管理, 是查找、共享和使用为Kubernetes构建的软件的最佳方式。

## 安装

> [Installing Helm](https://helm.sh/docs/intro/install/)

官网上安装的方法很多, 这里选择二进制安装(省略了安装golang环境编译等)

```bash
$ wget https://get.helm.sh/helm-canary-linux-amd64.tar.gz
$ tar xf helm-canary-linux-amd64.tar.gz
$ cp linux-amd64/helm /usr/local/bin/

# Initialize a Helm Chart Repository
$ helm repo add stable https://kubernetes-charts.storage.googleapis.com/
$ helm repo update

# search
$ helm search repo stable
```
