# CentOS7上安装MySQL8

### 基本安装

rpm 包下载地址 https://downloads.mysql.com/archives/community/

```bash
#/usr/bin/bash

# 下面将 mysql的密码修改为 123456,请改为自己需要设置的密码

##############################################
###                安装 MySQL              ###
##############################################
yum remove -y mariadb-libs
rpm -ivh mysql-community-common-8.0.25-1.el7.x86_64.rpm
rpm -ivh mysql-community-client-plugins-8.0.25-1.el7.x86_64.rpm
rpm -ivh mysql-community-libs-8.0.25-1.el7.x86_64.rpm
rpm -ivh mysql-community-client-8.0.25-1.el7.x86_64.rpm
rpm -ivh mysql-community-server-8.0.25-1.el7.x86_64.rpm

systemctl daemon-reload
systemctl enable mysqld
systemctl start  mysqld
sleep 10

##############################################
###    修改 MySQL root用户密码和访问权限   ###
##############################################
PASSLINE=`grep password /var/log/mysqld.log|tail -n 1`
PASSWORD=${PASSLINE#*localhost: }

cat > myql-init.sql << EOF
alter user root@'localhost' identified by '123456';
CREATE USER 'root'@'%' IDENTIFIED BY '123456';
GRANT ALL PRIVILEGES ON *.* TO 'root'@'%' WITH GRANT OPTION;
flush privileges;
EOF


mysql -uroot -p${PASSWORD} --connect-expired-password < myql-init.sql


##############################################
###             导入MySQL 配置             ###
##############################################

```

### 错误

mysqld: File './binlog.index' not found (OS errno 13 - Permission denied)

```bash
chown mysql:mysql -R /var/lib/mysql
chown mysql:mysql -R /var/lib/mysql-files/
chown mysql:mysql -R /var/lib/mysql-keyring/
```