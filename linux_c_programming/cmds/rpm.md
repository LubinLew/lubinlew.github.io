# RPM

> [rpm命令使用_Kasumi_yuki的博客](https://blog.csdn.net/Kasumi_yuki/article/details/89792135)
> 
> [Linux rpm命令详解 - 小a玖拾柒](https://www.cnblogs.com/ftl1012/p/rpm.html)



| 命令                                                                      | 说明           | 备注                                                                                                                               |
| ----------------------------------------------------------------------- | ------------ | -------------------------------------------------------------------------------------------------------------------------------- |
| `rpm -q XXX`                                                            | 已安装包         |                                                                                                                                  |
| `rpm -qp xxx.rpm`                                                       | 本地rpm包文件     | p 是指定本地 package                                                                                                                  |
| `rpm -ql XXX`<br>`rpm -qpl xxx.rpm`                                     | 查看包文件列表      |                                                                                                                                  |
| `rpm -qi XXX`<br>`rpm -qpi xxx.rpm`                                     | 查询包的信息       |                                                                                                                                  |
| `rpm -qR XXX`<br>`rpm -qpR xxx.rpm`                                     | 查询包的依赖       |                                                                                                                                  |
| `rpm -qf FILEPATH`                                                      | 查询文件属于哪个包    |                                                                                                                                  |
| 安装`rpm -ivh xxx.rpm`<br>升级`rpm -Fvh xxx.rpm`<br>升级或安装`rpm -Uvh xxx.rpm` | 本地rpm包的升级与安装 |                                                                                                                                  |
| `rpm -q --scripts xxx`<br>`rpm -qp --scripts xxx`                       | 查看包的脚本       | `preinstall`：安装过程开始之前执行的脚本<br>`postinstall`：安装过程完成之后执行的脚本<br>`preuninstall`：卸载过程开始执行之前执行的脚本<br>`postuninstall`：卸载过程完成之后执行的脚本<br> |

-----

## 查询已安装包

查询主要使用的是 `-q` 选项

### 查看所有安装的软件包

```bash
rpm -qa
```

### 查看已安装包的版本

```bash
rpm -q xxx # xxx 表示软件包的名称 例如 bash gcc 等

## 例子
rpm -q bash
bash-4.2.46-34.el7.x86_64
```

### 查看已安装包的信息

```bash
rpm -qi xxx  # xxx 表示软件包的名称 例如 bash gcc 等

# 例子
rpm -qi bash
Name        : bash
Version     : 4.2.46
Release     : 34.el7
Architecture: x86_64
Install Date: Sat 10 Oct 2020 03:45:46 PM CST
Group       : System Environment/Shells
Size        : 3667724
License     : GPLv3+
Signature   : RSA/SHA256, Sat 04 Apr 2020 04:49:07 AM CST, Key ID 24c6a8a7f4a80eb5
Source RPM  : bash-4.2.46-34.el7.src.rpm
Build Date  : Wed 01 Apr 2020 10:19:58 AM CST
Build Host  : x86-01.bsys.centos.org
Relocations : (not relocatable)
Packager    : CentOS BuildSystem <http://bugs.centos.org>
Vendor      : CentOS
URL         : http://www.gnu.org/software/bash
Summary     : The GNU Bourne Again shell
Description :
The GNU Bourne Again shell (Bash) is a shell or command language
interpreter that is compatible with the Bourne shell (sh). Bash
incorporates useful features from the Korn shell (ksh) and the C shell
(csh). Most sh scripts can be run by bash without modification.
```

### 查看已安装包的安装文件列表

```bash
rpm -ql xxx # xxxx 表示软件包的名称 例如 bash gcc 等

## 例子
rpm -ql bash
/etc/skel/.bash_logout
/etc/skel/.bash_profile
/etc/skel/.bashrc
/usr/bin/alias
....
```

### 查询某个文件来自哪个rpm包

```bash
rpm -qf FILEPATH

# 例子
rpm -qf /usr/bin/bash
bash-4.2.46-34.el7.x86_64
```

---

## 本地rpm包操作

### 查看本地安装包的文件列表

```bash
rpm -qpl xxx.rpm

# 例子
rpm -qpl tcpdump-4.9.2-4.el7_7.1.x86_64.rpm 
/usr/sbin/tcpdump
/usr/sbin/tcpslice
/usr/share/doc/tcpdump-4.9.2
/usr/share/doc/tcpdump-4.9.2/CHANGES
/usr/share/doc/tcpdump-4.9.2/CREDITS
/usr/share/doc/tcpdump-4.9.2/LICENSE
/usr/share/doc/tcpdump-4.9.2/README.md
/usr/share/man/man8/tcpdump.8.gz
/usr/share/man/man8/tcpslice.8.gz
```

### rpm包文件解压

```bash
rpm2cpio xxx.rpm | cpio -div
```

### 安装本地包

```bash
rpm -ivh xxx.rpm
```

### 升级本地包

#### 如果没有安装则安装

```bash
rpm -Uvh xxx.rpm
```

#### 如果没有安装则不安装

```bash
rpm -Fvh xxx.rpm
```
