# 安装

```bash
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

