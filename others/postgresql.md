# postgresql 安装

>  https://www.postgresql.org/download/linux/redhat/



```bash
yum install -y https://download.postgresql.org/pub/repos/yum/reporpms/EL-7-x86_64/pgdg-redhat-repo-latest.noarch.rpm
yum install -y postgresql13-server
/usr/pgsql-13/bin/postgresql-13-setup initdb
systemctl enable postgresql-13
systemctl start postgresql-13
```



## 配置文件夹

`PGDATA` = `/var/lib/pgsql/13/data/`



## 远程连接配置

> [connect to PostgreSQL server: FATAL: no pg_hba.conf entry for host](https://dba.stackexchange.com/questions/83984/connect-to-postgresql-server-fatal-no-pg-hba-conf-entry-for-host)