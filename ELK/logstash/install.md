# logstash 的安装

Logstash是一个开源数据收集引擎，具有实时管道功能。
Logstash可以动态地将来自不同数据源的数据统一起来，
并将数据标准化到你所选择的目的地。

## 安装

> [logstash的安装](https://www.elastic.co/guide/en/logstash/7.12/installing-logstash.html#_yum)

```bash
rpm --import https://artifacts.elastic.co/GPG-KEY-elasticsearch

cat > /etc/yum.repos.d/elastic.repo << EOF
[logstash-7.x]
name=Elastic repository for 7.x packages
baseurl=https://artifacts.elastic.co/packages/7.x/yum
gpgcheck=1
gpgkey=https://artifacts.elastic.co/GPG-KEY-elasticsearch
enabled=1
autorefresh=1
type=rpm-md
EOF

yum install -y logstash

systemctl start logstash
systemctl status logstash
```

安装包中自带 OpenJDK(/usr/share/logstash/jdk), 没有外部依赖。
配置文件在    `/etc/logstash/` 文件夹下。
执行文件路径  `/usr/share/logstash/bin/logstash`
service 文件路径 `/etc/systemd/system/logstash.service`

> 注意:
> service 文件中使用 logstash 作为执行用户,这会导致某些路径下的文件无法读取, 即使你将文件修改为 777 权限也读取不了,
> 
> 必要的时候修改 service 文件中 `User` 和 `Group` 设置

日志输出到了 `/var/log/messages` 文件中

## 配置
