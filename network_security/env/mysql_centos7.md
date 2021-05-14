# CentOS7上安装MySQL





mysqld: File './binlog.index' not found (OS errno 13 - Permission denied)

```bash
chown mysql:mysql -R /var/lib/mysql
chown mysql:mysql -R /var/lib/mysql-files/
chown mysql:mysql -R /var/lib/mysql-keyring/
```