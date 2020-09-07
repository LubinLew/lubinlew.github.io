# CentOS7上安装MySQL



```bash
yum install -y mariadb-server
```



设置 数据库 root 密码 为 root

```bash
mysql> set password for root@localhost = password('root');
mysql> flush privileges;
```


