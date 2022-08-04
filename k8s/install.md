# K8S的安装(CentOS-7)


```bash
#!/usr/bin/bash
########################################################
# 部署单节点K8S(如果是集群,请注释掉步骤3.3.3)
########################################################

K8SVER=1.24.2
MASTER_NODE=10.130.12.173
# 主机名(名称不能带下划线等特殊字符)
MASTER_NAME=lubin-k8s

#default setting
POD_CIDR=10.244.0.0/16
SERVICE_CIDR=10.96.0.0/16

################## 1.CentOS 7 系统设置 ##################

### 1.1 关闭防火墙
systemctl disable firewalld
systemctl stop firewalld

### 1.2 禁用SELINUX
setenforce 0
sed -i 's/SELINUX=enforcing/SELINUX=disabled/g' /etc/selinux/config

### 1.3 开启桥转发
cat > /etc/sysctl.d/k8s.conf <<EOF
net.bridge.bridge-nf-call-ip6tables = 1
net.bridge.bridge-nf-call-iptables = 1
net.ipv4.ip_forward = 1
EOF
sysctl -q -p

### 1.4 关闭swap分区
swapoff -a
sed -E -i 's/^\/dev\/mapper\/centos-swap.+$//g' /etc/fstab

### 1.5 设置主机名(名称不能带下划线等特殊字符)
hostnamectl set-hostname ${MASTER_NAME}

cat >> /etc/hosts << EOF
${MASTER_NODE}  ${MASTER_NAME}
EOF

################## 2.安装设置Docker ##################
### 2.1 安装Docker
# https://docs.docker.com/engine/install/centos/
yum install -y yum-utils
yum-config-manager --add-repo https://download.docker.com/linux/centos/docker-ce.repo
yum install -y docker-ce docker-ce-cli containerd.io docker-compose-plugin

### 2.2 配置docker选项
#### docker 默认使用 cgroupfs 而 k8s 默认使用 systemd, 也可以使用 --cgroup-driver=cgroupfs 在k8s中设置
mkdir -p /etc/docker
cat > /etc/docker/daemon.json <<EOF
{
  "exec-opts": ["native.cgroupdriver=systemd"]
}
EOF

### 2.3 启动docker
systemctl enable docker
systemctl start docker

################## 3.安装设置K8S ##################

### 3.1 安装Kubernetes
## 3.1.1 设置kubernetes的yum源
cat > /etc/yum.repos.d/kubernetes.repo <<EOF
[kubernetes]
name=Kubernetes Repository
baseurl=http://mirrors.aliyun.com/kubernetes/yum/repos/kubernetes-el7-x86_64/
enable=1
gpgcheck=0
EOF

## 3.1.2 安装Kubernetes程序
yum install -y kubelet-${K8SVER} kubeadm-${K8SVER} kubectl-${K8SVER}

### 3.2 启动 K8S
systemctl daemon-reload
systemctl enable kubelet
systemctl start kubelet

### 3.3 安装 Master 节点
## 3.3.1 初始化master节点
kubeadm init --kubernetes-version=${K8SVER}    \
  --apiserver-advertise-address=${MASTER_NODE} \
  --pod-network-cidr=${POD_CIDR} \
  --service-cidr=${SERVICE_CIDR} \
  --image-repository registry.aliyuncs.com/google_containers

## 3.3.2 拷贝配置文件
mkdir -p $HOME/.kube
sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
sudo chown $(id -u):$(id -g) $HOME/.kube/config

## 3.3.3 设置单节点的K8S
kubectl taint nodes --all node-role.kubernetes.io/master-

#### 4.安装CNI网络插件(flannel 或者 calico)
### 4.1 下载部署 flannel 镜像
kubectl apply -f https://raw.githubusercontent.com/coreos/flannel/master/Documentation/kube-flannel.yml


```