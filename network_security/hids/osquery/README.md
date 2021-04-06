# osquery 简介

> [系统管理利器 osquery](https://cofcool.github.io/tech/2018/02/05/osquery-for-devops)
> 
> [使用OSQUERY作为HIDS检测系统异常-mb5cd8060c9d8e2](https://blog.51cto.com/14328146/2443588)

osquery 是 Facebook 为系统管理，运维开发的一款管理工具，适用于OS X/macOS, Windows, and Linux。可以使用SQL直接查询系统环境变量，运行状况，资源占用等。

osquery 使用 C++ 开发, 代码位置 https://github.com/osquery/osquery/tree/master/osquery .

osquery 经常用到的命令有 `osqueyi` 及 `osqueryd`。  

`osqueryi` 是 osquery 的交互式shell。通过它可以像查询SQL一样查询系统信息。

`osqueryd` 是主机监控的守护进程。生产通常使用这种方式, 向server进行注册, server可以下发配置等。

```bash
ubuntu:~$: file /usr/bin/osqueryi
/usr/bin/osqueryi: symbolic link to osqueryd
```

安装包文件列表, 其中省略了很多 /usr/share/osquery/ 下的文件.

```bash
ubuntu:~$: dpkg-deb -c osquery_4.7.0-1.linux_amd64.deb 
drwxr-xr-x root/root         0 2021-03-12 21:37 ./etc/
drwxr-xr-x root/root         0 2021-03-12 21:37 ./etc/default/
-rw-r--r-- root/root       161 2021-03-12 21:29 ./etc/default/osqueryd
drwxr-xr-x root/root         0 2021-03-12 21:37 ./etc/init.d/
-rwxr-xr-x root/root      4198 2021-03-12 21:37 ./etc/init.d/osqueryd
drwxr-xr-x root/root         0 2021-03-12 21:37 ./etc/osquery/
drwxr-xr-x root/root         0 2021-03-12 21:37 ./usr/
drwxr-xr-x root/root         0 2021-03-12 21:37 ./usr/bin/
-rwxr-xr-x root/root      3636 2021-03-12 21:29 ./usr/bin/osqueryctl
-rwxr-xr-x root/root  45461048 2021-03-12 21:37 ./usr/bin/osqueryd
lrwxrwxrwx root/root         0 2021-03-12 21:37 ./usr/bin/osqueryi -> osqueryd
drwxr-xr-x root/root         0 2021-03-12 21:37 ./usr/lib/
drwxr-xr-x root/root         0 2021-03-12 21:37 ./usr/lib/systemd/
drwxr-xr-x root/root         0 2021-03-12 21:37 ./usr/lib/systemd/system/
-rw-r--r-- root/root       503 2021-03-12 21:37 ./usr/lib/systemd/system/osqueryd.service
drwxr-xr-x root/root         0 2021-03-12 21:37 ./usr/share/
drwxr-xr-x root/root         0 2021-03-12 21:37 ./usr/share/doc/
drwxr-xr-x root/root         0 2021-03-12 21:37 ./usr/share/doc/osquery/
-rw-r--r-- root/root       380 2021-03-12 21:29 ./usr/share/doc/osquery/copyright
drwxr-xr-x root/root         0 2021-03-12 21:37 ./usr/share/osquery/
drwxr-xr-x root/root         0 2021-03-12 21:37 ./usr/share/osquery/certs/
-rw-r--r-- root/root    224085 2021-03-12 21:29 ./usr/share/osquery/certs/certs.pem
-rw-r--r-- root/root      6381 2021-03-12 21:29 ./usr/share/osquery/osquery.example.conf
drwxr-xr-x root/root         0 2021-03-12 21:37 ./usr/share/osquery/packs/
-rw-r--r-- root/root      4968 2021-03-12 21:29 ./usr/share/osquery/packs/hardware-monitoring.conf
-rw-r--r-- root/root     13872 2021-03-12 21:29 ./usr/share/osquery/packs/incident-response.conf
-rw-r--r-- root/root     10142 2021-03-12 21:29 ./usr/share/osquery/packs/it-compliance.conf
-rw-r--r-- root/root      1162 2021-03-12 21:29 ./usr/share/osquery/packs/osquery-monitoring.conf
-rw-r--r-- root/root     17756 2021-03-12 21:29 ./usr/share/osquery/packs/ossec-rootkit.conf
-rw-r--r-- root/root     30695 2021-03-12 21:29 ./usr/share/osquery/packs/osx-attacks.conf
-rw-r--r-- root/root      4172 2021-03-12 21:29 ./usr/share/osquery/packs/unwanted-chrome-extensions.conf
-rw-r--r-- root/root      7474 2021-03-12 21:29 ./usr/share/osquery/packs/vuln-management.conf
-rw-r--r-- root/root      5917 2021-03-12 21:29 ./usr/share/osquery/packs/windows-attacks.conf
-rw-r--r-- root/root     12857 2021-03-12 21:29 ./usr/share/osquery/packs/windows-hardening.conf
drwxr-xr-x root/root         0 2021-03-12 21:37 ./var/
drwxr-xr-x root/root         0 2021-03-12 21:37 ./var/log/
drwxr-xr-x root/root         0 2021-03-12 21:37 ./var/log/osquery/
drwxr-xr-x root/root         0 2021-03-12 21:37 ./var/osquery/
```

**配置文件**

| 配置文件                                    | 说明   | 备注             |
| --------------------------------------- | ---- | -------------- |
| /etc/default/osqueryd                   | 默认配置 | 安装时就有          |
| /etc/osquery/osquery.conf               | 默认配置 | 安装时没有, 需要用户自己写 |
| /usr/share/osquery/osquery.example.conf | 配置例子 |                |

**服务文件**

| 服务文件                                     | 说明                          | 备注                              |
| ---------------------------------------- | --------------------------- | ------------------------------- |
| /etc/init.d/osqueryd                     | 老式(systemd之前)的service服务文件   | 旧Linux版本使用                      |
| /usr/lib/systemd/system/osqueryd.service | systemd服务文件                 |                                 |
| /usr/bin/osqueryctl                      | osquery服务操作(启动/停止/重启/配置检查等) | 命令行工具(shell脚本), 主要还是操作上面2个服务完成的 |

## osquery 表

osquery内置了很多表，通过这些表可以查询到系统信息。通过`.table` 命令可以查看所有表, 基本包含了所有的系统信息.

```sql
osquery> .table
  => acpi_tables
  => apparmor_events
  => apparmor_profiles
  => apt_sources
  => arp_cache
  ...
```

例如查看系统中所有用户的信息, 就可以从 `users` 表中获取

```sql
osquery> select * from users;
+-------+-------+------------+------------+------------------+------------------------------------+--------------------------+-------------------+------+
| uid   | gid   | uid_signed | gid_signed | username         | description                        | directory                | shell             | uuid |
+-------+-------+------------+------------+------------------+------------------------------------+--------------------------+-------------------+------+
| 0     | 0     | 0          | 0          | root             | root                               | /root                    | /bin/bash         |      |
| 1     | 1     | 1          | 1          | daemon           | daemon                             | /usr/sbin                | /usr/sbin/nologin |      |
| 2     | 2     | 2          | 2          | bin              | bin                                | /bin                     | /usr/sbin/nologin |      |
| 3     | 3     | 3          | 3          | sys              | sys                                | /dev                     | /usr/sbin/nologin |      |
| 4     | 65534 | 4          | 65534      | sync             | sync                               | /bin                     | /bin/sync         |      |
| 5     | 60    | 5          | 60         | games            | games                              | /usr/games               | /usr/sbin/nologin |      |
| 41    | 41    | 41         | 41         | gnats            | Gnats Bug-Reporting System (admin) | /var/lib/gnats           | /usr/sbin/nologin |      |
| 100   | 102   | 100        | 102        | systemd-network  | systemd Network Management,,,      | /run/systemd             | /usr/sbin/nologin |      |
| 101   | 103   | 101        | 103        | systemd-resolve  | systemd Resolver,,,                | /run/systemd             | /usr/sbin/nologin |      |
| 102   | 104   | 102        | 104        | systemd-timesync | systemd Time Synchronization,,,    | /run/systemd             | /usr/sbin/nologin |      |
| 103   | 106   | 103        | 106        | messagebus       |                                    | /nonexistent             | /usr/sbin/nologin |      |
| 104   | 110   | 104        | 110        | syslog           |                                    | /home/syslog             | /usr/sbin/nologin |      |
+-------+-------+------------+------------+------------------+------------------------------------+--------------------------+-------------------+------+


```



## osquery 配置

配置选项:

```bash
ubuntu:~$: osqueryd --help
```

**配置文件检查**

检查 `/etc/osquery/osquery.conf` 配置是否正确

```bash
osqueryctl config-check
```