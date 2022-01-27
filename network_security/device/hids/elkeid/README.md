# Elkeid

## 安装

开源版本只支持 `elkeidup` 工具部署。这里部署一个单机版本的

** 必需有ssh免密登录(私钥)即使是本机 **

** 部署的地址不支持域名 **

** 部署的地址即使是本机也不能使用 127.0.0.1 **

```bash
######下载
wget https://github.com/bytedance/Elkeid/releases/download/v1.7.1/elkeidup
chmod +x ./elkeidup

wget https://github.com/bytedance/Elkeid/releases/download/v1.7.1/package_community.tar.gz
tar xf package_community.tar.gz

## 生成配置模版 elkeid_server.yaml
./elkeidup init
# 将 elkeid_server.yaml 中的 IP 地址都换成本机地址

#后端组件自动化部署
./elkeidup deploy --package package_community/ --config ./elkeid_server.yaml

#检查状态
./elkeidup status

#查看组件地址与密码
cat ~/.elkeidup/elkeid_passwd

#自动化build agent与部分插件
./elkeidup agent build --package package_community/
```

部署结束，根据前端引导进行agent部署, `8082` 端口是UI界面, 登录用户密码在 `~/.elkeidup/elkeid_passwd` 文件中.

## 

## Kafka 手动消费的方式

kafka 有鉴权, 所以访问比较麻烦, 并且kafka 中的数据都是原始 `protobuf` 格式需要解析后才能使用。

```bash
cat > /root/kafka-client.jaas.conf <<EOF
KafkaClient {
  org.apache.kafka.common.security.plain.PlainLoginModule required
  username="admin"
  password="elkeid";
};
EOF

export KAFKA_OPTS=" -Djava.security.auth.login.config=/root/kafka-client.jaas.conf"


/elkeid/kafka/kafka_2.13-2.7.0/bin/kafka-console-consumer.sh \
--bootstrap-server 10.130.12.22:9092 \
--consumer-property security.protocol=SASL_PLAINTEXT \
--consumer-property sasl.mechanism=PLAIN \
--from-beginning \
--topic hids_svr
```

[Elkeid/raw_data_usage_tutorial.md at main · bytedance/Elkeid · GitHub](https://github.com/bytedance/Elkeid/blob/main/elkeidup/raw_data_usage_tutorial.md)