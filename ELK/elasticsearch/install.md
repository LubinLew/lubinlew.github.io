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

yum install -y elasticsearch

# 配置 es 工作目录
mkdir -p /var/lib/elasticsearch
chown elasticsearch:elasticsearch /var/lib/elasticsearch

# 配置 es 服务地址和集群信息(这里是单机配置)
sed -i 's/#network.host: 192.168.0.1/network.host: 0.0.0.0/g' /etc/elasticsearch/elasticsearch.yml
sed -i 's/#node.name: node-1/node.name: node-1/g' /etc/elasticsearch/elasticsearch.yml
sed -i 's/#cluster.initial_master_nodes: \["node-1", "node-2"\]/cluster.initial_master_nodes: \["node-1"\]/g' /etc/elasticsearch/elasticsearch.yml

# 配置 es 密码
cat >> /etc/elasticsearch/elasticsearch.yml << EOF
xpack.security.enabled: true
xpack.license.self_generated.type: basic
xpack.security.transport.ssl.enabled: true
EOF

systemctl daemon-reload
systemctl enable elasticsearch
systemctl start  elasticsearch

# 交互式 输入密码(H8Sn_!Rd)
/usr/share/elasticsearch/bin/elasticsearch-setup-passwords interactive
```
