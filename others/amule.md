# amule 下载 ed2k 链接

> Ubuntu 命令行

## 安装 amule

```bash
apt install amule amule-utils amule-daemon
```

## 配置 amule

1.选定EC密码 `<password>`

2.计算EC密码HASH

```bash
echo -n '<password>' | md5sum | cut -d ' ' -f 1
```

3.修改配置文件

```bash
# vim ~/.aMule/amule.conf

ECPassword=<PASSWOD HASH>
```

> 还可以配置 `TempDir` 和 `IncomingDir` 来设置下载路径


4.启动 amule 服务

> 创建 systemd 服务, 原生安装包只有旧的 service 服务

```bash
cat > /lib/systemd/system/amule-daemon.service <<EOF
[Unit]
Description=aMule Daemon

[Service]
ExecStart=/usr/bin/amuled
Restart=always

[Install]
WantedBy=multi-user.target
EOF

systemctl daemon-reload
systemctl start amule-daemon
```

## 使用 amule 下载

```bash
# 首次运行需要设置密码.
$ amulecmd

# 连接到 eD2K 服务器：
$ connect

# 连接到 eD2K 服务器
$ add <ed2k_link>

# 查看下载进度
$ show DL
```

文件默认下载位置 `~/.aMule/Incoming`


