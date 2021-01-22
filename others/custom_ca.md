# 导入自签发CA证书

## Windows

双击证书 -> 安装证书 -> 当前用户/本地计算机 ->将所有的整证书都放入下面存储 - > 浏览 -> 受信任的根证书颁发机构 -> 确定


## CentOS 7
### 安装证书工具

```bash
yum -y install ca-certificates
```

### 将证书放入下面文件夹中

证书是PEM格式 crt 后缀

```bash
/usr/share/pki/ca-trust-source/anchors/
```

### 更新证书系统

```bash
update-ca-trust
```
