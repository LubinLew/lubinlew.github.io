# rpm 和 deb 安装包操作

| 功能            | rpm                                | deb                    |
| ------------- | ---------------------------------- | ---------------------- |
| 解压安装包         | `rpm2cpio XXXX.rpm   \| cpio -div` | `dpkg-deb -X YYYY.deb DIR` |
| 查看已安装软件的文件列表  | `rpm -ql bash`                     | `dpkg -L bash`  or `apt-file list bash`       |
| 查看某个文件属于哪个软件包 | `rpm -qf /bin/bash`                | `dpkg -S /bin/bash` or `apt-file search /bin/bash`                       |
| 查看安装包里的所有文件列表 | `rpm -qpl XXXX.rpm`                | `dpkg-deb -c YYYY.deb` |
| 查看软件包里文档列表    | `rpm -qpd XXXX.rpm`                |                        |
| 查看安装包的描述信息    | `rpm -qpi XXXX.rpm`                | `dpkg-deb -I YYYY.deb` |
| 查看软件包的依赖关系    | `rpm -qpR  XXXX.rpm`               | `dpkg-deb -I YYYY.deb` |
