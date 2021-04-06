# 自签发CA证书

首先需要自签发一套证书秘钥作为CA。

```bash
# 快速自签发证书
$ openssl req -new -x509 -days 365 -key fd.key -out fd.crt
# 可以使用配置文件
$ openssl req -new -x509 -days 365 -config fd.cnf -key fd.key -out fd.crt
```

使用CA签发证书

```bash
openssl
```
