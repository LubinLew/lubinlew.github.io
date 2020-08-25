# InfluxDB v1.8 简介

> [InfluxDB 1.8 documentation | InfluxData Documentation](https://docs.influxdata.com/influxdb/v1.8/)

InfluxDB 是一个时序数据库 `TSDB(time series database)` 。它具有处理大量写入和查询的能力。 InfluxDB主要用于大量时间戳数据的备份存储，例如，DevOps 监控、应用 metrics, IoT 传感器数据、real-time analytics.

**下面是InfluxDB的优点**：

- 面向时序数据(`time series data`)的高性能存储， TSM 引擎支持高速数据存入和压缩；
- 使用go语言编写，没有任何其他软件依赖，安装部署容易；
- 提供高性能的HTTP接口用于数据写入和查询；
- 提供插件支持其他类型的数据存储，例如Graphite, collectd,  OpenTSDB；
- 使用类SQL语言，使用更简单；
- `Tags`能为时序做索引，查询更加快速高效；
- 数据保留策略(`Retention policies`) 能够删除旧数据;
- 连续查询(`Continuous queries`) 会自动自动计算统计数据，使常用查询更高效。

## InfluxDB的安装

> [Install InfluxDB OSS | InfluxData Documentation](https://docs.influxdata.com/influxdb/v1.8/introduction/install/)

### 安装权限

使用root安装

### 使用端口

- TCP 端口`8086` 用于客户端使用InfluxDB API通信;
- TCP 端口`8088` 用于RPC服务，用来数据备份和数据恢复；
- 插件使用的端口， 具体见 `/etc/influxdb/influxdb.conf`。

### NTP

InfluxDB 使用运行主机的时间(local time in UTC)作为时间戳，如果数据产生节点和InfluxDB主机的时间不同，那么会导致数据不准确，最好使用NTP服务同步这些主机的时间。

### 安装

> [InfluxDB Downloads Pages](https://portal.influxdata.com/downloads/)

```bash
## CentOS下的安装, 其他环境装参考上面的URL

wget https://dl.influxdata.com/influxdb/releases/influxdb-1.8.2.x86_64.rpm
sudo yum localinstall influxdb-1.8.2.x86_64.rpm

#Created symlink from /etc/systemd/system/influxd.service to /usr/lib/systemd/system/influxdb.service.
#Created symlink from /etc/systemd/system/multi-user.target.wants/influxdb.service to /usr/lib/systemd/system/influxdb.service.

# 开机自动启动
sudo systemctl enable influxdb

# 启动 influxDB
sudo systemctl start influxdb
```

### Service

```bash
# cat /etc/systemd/system/influxd.service

# If you modify this, please also make sure to edit init.sh

[Unit]
Description=InfluxDB is an open-source, distributed, time series database
Documentation=https://docs.influxdata.com/influxdb/
After=network-online.target

[Service]
User=influxdb
Group=influxdb
LimitNOFILE=65536
EnvironmentFile=-/etc/default/influxdb
ExecStart=/usr/bin/influxd -config /etc/influxdb/influxdb.conf $INFLUXD_OPTS
KillMode=control-group
Restart=on-failure

[Install]
WantedBy=multi-user.target
Alias=influxd.service
```

### 配置文件

> [Configure InfluxDB OSS | InfluxData Documentation](https://docs.influxdata.com/influxdb/v1.8/administration/config/)

```bash
# cat /etc/influxdb/influxdb.conf

## 全局设置
reporting-disabled = false # 每隔24小时向官方发送使用情况数据
bind-address = "127.0.0.1:8088" # 用于数据备份和恢复

## metastore-settings
# 这个部分用于控制 InfluxDB metastore。
# metastore 用于存储 users, databases, retention policies, shards, and continuous queries.
[[meta]]
dir = "/var/lib/influxdb/meta"  #The directory where the metadata/raft database is stored
```

http://10.130.12.66:8086/query?q=SHOW%20DATABASES
