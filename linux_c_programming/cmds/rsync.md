# rsync

## 服务端

> Centos 自带 rsync 所以不需要额外下载

```bash
# 设置 rysnc 上传用户
cat > /etc/rsyncd.secrets <<EOF
backup:PASSWORD
EOF
chmod 400 /etc/rsyncd.secrets

# 配置 rysncd 服务
cat > /etc/rsyncd.conf <<EOF
uid = nobody
gid = nobody
use chroot = yes
max connections = 10
pid file = /var/run/rsyncd.pid
log file = /var/log/rsyncd.log

[upload]
    path = /data
    comment = Upload directory
    read only = no
    list = yes
    auth users = backup
    secrets file = /etc/rsyncd.secrets
EOF

systemctl start  rsyncd
systemctl enable rsyncd
```

## 客户端

```bash
RSYNC_PASSWORD="PASSWORD"
rsync -avz FILE backup@192.168.122.240::upload
```

## 排错

- 1. 上传文件的的 owner 需要和服务端 配置中的 uid gid 一致
- 2. Selinux 开启可能会导致权限问题