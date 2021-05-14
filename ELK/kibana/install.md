# 安装

```bash
cat > /etc/yum.repos.d/elastic.repo << EOF
[logstash-7.x]
name=Elastic repository for 7.x packages
baseurl=https://artifacts.elastic.co/packages/7.x/yum
gpgcheck=0
enabled=1
autorefresh=1
type=rpm-md
EOF

yum install -y kibana

# 配置 kibana服务地址 和 ES连接方式
cat >> /etc/kibana/kibana.yml << EOF
server.host: "0.0.0.0"
elasticsearch.hosts: ["http://localhost:9200"]
elasticsearch.username: "kibana_system"
elasticsearch.password: "password"
EOF

systemctl daemon-reload
systemctl enable kibana
systemctl start  kibana
```


##### 创建 index pattern

在 Kibana 中  `Stack Management` -> `Index Patterns` -> `Create Index Pattern` -> <NAME>

