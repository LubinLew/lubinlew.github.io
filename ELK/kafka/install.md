# Kafka 安装

安装的是单机版, 不是集群部署, 集群部署参考[kafka集群环境搭建](https://www.cnblogs.com/luzhanshi/p/13369834.html)

#### 安装 jdk

```bash
yum install -y java-11-openjdk-headless
```

#### 安装 zookeeper

```bash
wget https://downloads.apache.org/zookeeper/stable/apache-zookeeper-3.6.3-bin.tar.gz
tar xf apache-zookeeper-3.6.3-bin.tar.gz -C /opt/
mv /opt/apache-zookeeper-3.6.3-bin /opt/zookeeper
# 配置 zookeeper
cat > /opt/zookeeper/conf/zoo.cfg << EOF
tickTime=2000
initLimit=10
syncLimit=5
dataDir=/var/lib/zookeeper
dataLogDir=/var/log/zookeeper
clientPort=2181
EOF
# 启动 zookeeper(后台运行)
/opt/zookeeper/bin/zkServer.sh start
```

#### 安装 Kafka

```bash
wget https://downloads.apache.org/kafka/2.8.0/kafka_2.13-2.8.0.tgz
tar xf kafka_2.13-2.8.0.tgz -C /opt/
mv /opt/kafka_2.13-2.8.0 /opt/kafka
# 启动 kafka(后台运行)
/opt/kafka/bin/kafka-server-start.sh -daemon /opt/kafka/config/server.properties
```
