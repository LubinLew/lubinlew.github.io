# 密钥交换算法

密钥协商(key establishment)包括“密钥传输”(key transmission)和“密钥交换”(key exchange)。

在TLS中，会话安全性取决于称为主密钥（master secret）的48字节共享密钥。密钥交换的目的是计算另一个值，即预主密钥（premaster secret）。这个值是组成主密钥的来源。

RSA和DH两种算法都被用于密钥协商。密钥协商(key establishment)包括“密钥传输”(key transmission)和“密钥交换”(key exchange)。TLS是广泛用于日常生活中的https协议的一部分。以下答案针对这两个算法在TLS ciphersuite中的应用，即TLS_RSA_XXX 以及 TLS_DHE_XXX，此处的key指的是接下来在实际传输数据时使用的session key,安全性的结论在答案末尾。区RSA起的作用是Key Transmission，也就是说，用于产生Session Key的“种子”是由客户端决定，用服务器的公钥加密并再次传输到服务器端。（见下图的C,最下方的德语的意思是TLS Record Layer: 用密钥k进行加密）
DH起的作用是key exchange


TLS支持许多密钥交换算法，能够支持各种证书类型、公钥算法和密钥生成协议。它们之中有一些在TLS协议主规格书中定义，但更多的则是在其他规格说明中定义。

| 名称          | 描述                                          | 备注  |
| ----------- | ------------------------------------------- | --- |
| dh_anon     | Diffie-Hellman（DH）密钥交换，未经身份验证               |     |
| dhe_rsa     | 临时DH密钥交换，使用RSA身份验证                          |     |
| ecdh_anon   | 临时椭圆曲线DH（elliptic curve DH，ECDH）密钥交换，未经身份验证 |     |
| ecdhe_rsa   | 临时ECDH密钥交换，使用RSA身份验证                        |     |
| ecdhe_ecdsa | 临时ECDH密钥交换，使用ECDSA身份验证                      |     |
| krb5        | Kerberos密钥交换                                |     |
| rsa         | RSA密钥交换和身份验证                                |     |
| psk         | 预共享密钥（pre-shared key，PSK）密钥交换和身份验证          |     |
| dhe_psk     | 临时DH密钥交换，使用PSK身份验证                          |     |
| rsa_psk     | PSK密钥交换，使用RSA身份验证                           |     |
| srp         | 安全远程密码（secure remote password，SRP）密钥交换和身份验证 |     |

使用哪一种密钥交换由协商的套件所决定。一旦套件决定下来，两端都能了解按照哪种算法继续操作。实际使用的密钥交换算法主要有以下4种。

---

## Reference

https://blog.csdn.net/MEIYOUDAO_JIUSHIDAO/article/details/110285378