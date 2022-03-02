# zookeeper

ZooKeeper 是一个分布式的，开放源码的分布式应用程序协调服务，是 Google 的 Chubby 一个开源的实现，是 Hadoop 和 Hbase 的重要组件。
它是一个为分布式应用提供一致性服务的软件，提供的功能包括：配置维护、域名服务、分布式同步、组服务等。
ZooKeeper 的架构通过冗余服务实现高可用性。
一个典型的分布式数据一致性的解决方案，分布式应用程序可以基于它实现诸如数据发布/订阅、负载均衡、命名服务、分布式协调/通知、集群管理、Master 选举、分布式锁和分布式队列等功能。

## zookeeper 集群搭建

生产环境至少需要三台 zookeeper 服务,来达成高可用, 官方推荐使用奇数个节点来做高可用。

> https://zookeeper.apache.org/doc/current/zookeeperStarted.html

zookeeper 集群常使用三个端口提供服务, 这些端口都可用通过配置修改

| 端口| 功能|
|----|------|
| 2181 | 对 client 端提供服务 |
| 2888 | 集群内机器通信使用 |
| 3888 | 选举 leader 使用 |

集群配置的要点:
 - 通过 `Server.X` 指定集群中各个服务器的地址
 - 通过 `myid` 文件指定当前服务器的 ID

```bash
#!/usr/bin/bash
#set +x

################## 安装配置区 ##########################
#### 不同的机器上 这个ID 不同, 依次是 1,2,3 与 zoo.cfg 中的 server.X 对应
CLUSTER_SERVER_ID=1

# 集群服务器的IP地址
CLUSTER_SERVER_1=10.130.12.66
CLUSTER_SERVER_2=10.130.12.239
CLUSTER_SERVER_3=10.130.12.188

# 数据和日志存储位置
DATA_LOCATION=/data/zookeeper
LOG_LOCATION=/data/logs/zookeeper

# 最新的稳定版本
ZOO_VERSION=3.6.3

#########################################################
TOPDIR="$( cd $(dirname $0) ; pwd -P )"

mkdir -p /opt
mkdir -p ${DATA_LOCATION}
mkdir -p ${LOG_LOCATION}

# 安装 JAVA 运行环境
which java > /dev/null 2>&1
if [ $? -ne 0 ] ; then
    if command -v rpm > /dev/null 2>&1 && rpm -q rpm > /dev/null 2>&1; then
        yum install -y java-latest-openjdk-headless
    else
        apt install -y default-jre
    fi
fi


# 如果安装包不存在则到官网下载
if [ ! -f apache-zookeeper-${ZOO_VERSION}-bin.tar.gz ] ; then
    curl -Lsk https://dlcdn.apache.org/zookeeper/zookeeper-${ZOO_VERSION}/apache-zookeeper-${ZOO_VERSION}-bin.tar.gz -o apache-zookeeper-${ZOO_VERSION}-bin.tar.gz
fi

# 解压安装包到 /opt/zookeeper 文件夹下
tar xf apache-zookeeper-${ZOO_VERSION}-bin.tar.gz -C /opt/
mv /opt/apache-zookeeper-${ZOO_VERSION}-bin /opt/zookeeper
echo ${ZOO_VERSION} > opt/zookeeper/VERSION

# 生成 zookeeper 配置文件
cat > /opt/zookeeper/conf/zoo.cfg << EOF
tickTime=2000
initLimit=10
syncLimit=5
dataDir=${DATA_LOCATION}
dataLogDir=${LOG_LOCATION}
clientPort=2181
server.1=${CLUSTER_SERVER_1}:2888:3888
server.2=${CLUSTER_SERVER_2}:2888:3888
server.3=${CLUSTER_SERVER_3}:2888:3888
EOF

# 写入 zookeeper ID
echo ${CLUSTER_SERVER_ID} > ${DATA_LOCATION}/myid

# 生成 zookeeper systemd 服务文件
cat > /etc/systemd/system/zookeeper.service << EOF
[Unit]
Description=zookeeper.service
After=network.target
 
[Service]
User=root
Group=root
Type=forking

ExecStart=/opt/zookeeper/bin/zkServer.sh start
ExecStop=/opt/zookeeper/bin/zkServer.sh stop
ExecReload=/opt/zookeeper/bin/zkServer.sh restart
 
[Install]
WantedBy=multi-user.target

EOF

# 启动 zookeeper 服务
systemctl daemon-reload
systemctl enable zookeeper
systemctl start  zookeeper
systemctl status zookeeper
/opt/zookeeper/bin/zkServer.sh status
```

上面配置的是最简单的集群, 集群间通信, 客户端和集群通信都是未加密和认证的.

## SASL 认证

https://access.redhat.com/documentation/en-us/red_hat_amq/7.2/html/using_amq_streams_on_red_hat_enterprise_linux_rhel/configuring_zookeeper

## SSL 加密

https://www.ibm.com/docs/en/b2b-integrator/6.0.3?topic=connections-enabling-node-node-encryption-apache-zookeeper





https://www.runoob.com/w3cnote/zookeeper-tutorial.html