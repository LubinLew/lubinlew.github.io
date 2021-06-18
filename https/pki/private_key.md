# 私钥

OpenSSL支持 RSA、DSA、ECDSA 算法生成私钥,
- DSA 密钥实际上被限制为 1024 位（Internet Explorer 不支持任何更强的）,TLSv1.3 协议中已不支持DSA证书
- 目前大部分证书都是 RSA 的
- 以后的趋势是 ECDSA 证书逐步替代 RSA 证书

### RSA

使用 RSA 生成私钥前你选择使用什么加密算法、选择密钥的长度和选择是否使用密码(passphrase)保护密钥。
目前认为秘钥长度 2048 位是安全的, 但是越来越多人使用 4096 位。
使用密钥保护私钥是可选的，但是建议使用。受保护的密钥可以安全的存储、传输 和 备份。
但是这样也会造成不便，例如你每次重启web服务器时都可能要求你输入通行码等。
如果服务器被入侵，即使有密码保护，但是在web服务器的内存中是没有密码的，黑客还是可以通过技术手段拿到私钥。

```bash
openssl genrsa -out rsa.key 2048

-----BEGIN RSA PRIVATE KEY-----
MIIEogIBAAKCAQEA1+U3lYeOoQNsu61DsJ8h62/NImVNBuY/C1u0niU3RwiuhkTY
...省略
sZ0yU78qGnV92L6tujVZ2C4BDj8Bl2x7N0/AIOt/BTCmki1Tt/o=
-----END RSA PRIVATE KEY-----

```


### DSA

```bash
openssl dsaparam -genkey -noout -out dsa.key 2048

-----BEGIN DSA PRIVATE KEY-----
MIIDVQIBAAKCAQEAkTL2EtWrC1rXl+mUd43BQiCJC/dhacgnEzTjTdsx8LmcZYDH
...省略
fd/45oY+8wIgNR1Hpfc7318oJFkwtxigjppnqF8rgUA9HSYEsOsyKoA=
-----END DSA PRIVATE KEY-----

```

### ECDSA

椭圆曲线算法的优点是秘钥长度比RSA短

```bash
openssl ecparam -name secp384r1 -genkey -noout -out ecc.key

-----BEGIN EC PRIVATE KEY-----
MIGkAgEBBDC6GYxerP+NxXq1DNXskdOPKJOcO0O8qYvOLkWCLOPddz27979IJB9A
...省略
3W+aXLotwQ/RCVPC+QxSD3vGXeS/nLg=
-----END EC PRIVATE KEY-----

```

使用下面的命令或者OpenSSL支持的曲线列表:

```bash
openssl ecparam -list_curves
  secp256k1 : SECG curve over a 256 bit prime field
  secp384r1 : NIST/SECG curve over a 384 bit prime field
  secp521r1 : NIST/SECG curve over a 521 bit prime field
  prime256v1: X9.62/SECG curve over a 256 bit prime field
```

验证私钥的正确性:

```bash
openssl ecparam -in ecc.key -check -name secp384r1 -text
  ASN1 OID: secp384r1
  NIST CURVE: P-384
  checking elliptic curve parameters: ok
  -----BEGIN EC PARAMETERS-----
  BgUrgQQAIg==
  -----END EC PARAMETERS-----
```
