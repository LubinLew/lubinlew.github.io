# 主机漏洞扫描(内部)

目前主流的主机漏洞扫描工具都是基于软件包版本而来的。然后使用OVAL数据库对比软件版本,获得漏洞信息。

## 软件包信息收集

| 系统      | 数据位置                        | 数据说明                                                                                            | 数据使用                                                                       |
| ------- | --------------------------- | ----------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------- |
| CentOS  | `/var/lib/rpm/Packages`     | 这是一个5.x 代码版本(无人维护)的 Berkeley DB,<br>由于6.x版本被 Oracel 修改了开源协议, 社区无法使用<br>新版的 rpm 开始切换到 sqlite 数据库 | 使用 [Berkeley DB](https://github.com/berkeleydb/libdb) 库读取信息<br>使用 rpm 命令获取 |
| Ubuntu  | `/var/lib/dpkg/status`      | 这是一个文本文件,格式清晰明了                                                                                 | 按照格式读取                                                                     |
| windows | 从注册表读取(32位/64位/用户安装/hotfix) | -                                                                                               | -                                                                          |


```bash
rpm --querytags
rpm -qa --qf '%{name}\t%{arch}\t%{summary}\t%{size}\t%{epoch}\t%{release}\t%{version}\t%{vendor}\t%{installtime:date}\t%{group}\t\n'
```

## OVAL数据对比

oval 数据各大Linux 发行商都有提供数据

```bash
## centos/redhat
# https://access.redhat.com/labs/securitydataapi/cve.json
# https://www.redhat.com/security/data/oval/
# 具体数据获取参考: https://github.com/wazuh/wazuh/issues/5267

## ubuntu
https://people.canonical.com/~ubuntu-security/oval/com.ubuntu.focal.cve.oval.xml.bz2
https://people.canonical.com/~ubuntu-security/oval/com.ubuntu.bionic.cve.oval.xml.bz2
https://people.canonical.com/~ubuntu-security/oval/com.ubuntu.xenial.cve.oval.xml.bz2
https://people.canonical.com/~ubuntu-security/oval/com.ubuntu.trusty.cve.oval.xml.bz2

## debian
https://www.debian.org/security/oval/oval-definitions-buster.xml
https://www.debian.org/security/oval/oval-definitions-stretch.xml

## windows 可以使用 wazuh 提供的数据
https://feed.wazuh.com/vulnerability-detector/windows/msu-updates.json.gz
```
