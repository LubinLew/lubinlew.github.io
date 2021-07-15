# OpenSSL

> https://github.com/openssl/openssl

OpenSSL 是用于传输层安全性 (TLS) 和安全套接字层 (SSL) 协议的强大、商业级和功能齐全的工具包。它也是一个通用的密码学库。

### 历史

OpenSSL计划在1998年开始，其目标是发明一套自由的加密工具，在互联网上使用。
OpenSSL以Eric Young以及Tim Hudson两人(加拿大人)开发的SSLeay为基础，随着两人前往RSA公司任职，SSLeay在1998年12月停止开发。
因此在1998年12月，社区另外分支出 OpenSSL，继续开发下去。

OpenSSL管理委员会目前由7人组成，有13个开发人员具有提交权限（其中许多人也是OpenSSL管理委员会的一部分）。只有两名全职员工（研究员），其余的是志愿者。该项目每年的预算不到100万美元，主要依靠捐款。 TLS 1.3 的开发由Akamai赞助。

#### 支持的算法

**加密算法**

- [AES](https://en.wikipedia.org/wiki/Advanced_Encryption_Standard)
- [Blowfish](https://en.wikipedia.org/wiki/Blowfish_(cipher))
- [Camellia](https://en.wikipedia.org/wiki/Camellia_(cipher))
- [Chacha20](https://en.wikipedia.org/wiki/Chacha20)
- [Poly1305](https://en.wikipedia.org/wiki/Poly1305)
- [SEED](https://en.wikipedia.org/wiki/SEED")
- [CAST-128](https://en.wikipedia.org/wiki/CAST-128)
- [DES](https://en.wikipedia.org/wiki/Data_Encryption_Standard)
- [IDEA](https://en.wikipedia.org/wiki/International_Data_Encryption_Algorithm)
- [RC2](https://en.wikipedia.org/wiki/RC2)
- [RC4](https://en.wikipedia.org/wiki/RC4)
- [RC5](https://en.wikipedia.org/wiki/RC5)
- [Triple DES](https://en.wikipedia.org/wiki/Triple_DES)
- [GOST 28147-89](https://en.wikipedia.org/wiki/GOST_(block_cipher))
- [SM4](https://en.wikipedia.org/wiki/SM4_(cipher))

**摘要算法**

- [MD5](https://en.wikipedia.org/wiki/MD5)
- [MD4](https://en.wikipedia.org/wiki/MD4)
- [MD2](https://en.wikipedia.org/wiki/MD2_(cryptography))
- [SHA-1](https://en.wikipedia.org/wiki/SHA-1)
- [SHA-2](https://en.wikipedia.org/wiki/SHA-2)
- [SHA-3](https://en.wikipedia.org/wiki/SHA-3)
- [RIPEMD-160](https://en.wikipedia.org/wiki/RIPEMD-160)
- [MDC-2](https://en.wikipedia.org/wiki/MDC-2)
- [GOST R 34.11-94](https://en.wikipedia.org/wiki/GOST_(hash_function))
- [BLAKE2](https://en.wikipedia.org/wiki/BLAKE_(hash_function)#BLAKE2)
- [Whirlpool](https://en.wikipedia.org/wiki/Whirlpool_(cryptography))
- [SM3](https://en.wikipedia.org/wiki/SM3_(hash_function))

**公钥算法**

- [RSA](https://en.wikipedia.org/wiki/RSA_(algorithm)
- [DSA](https://en.wikipedia.org/wiki/Digital_Signature_Algorithm)
- [Diffie–Hellman key exchange](https://en.wikipedia.org/wiki/Diffie%E2%80%93Hellman_key_exchange)
- [Elliptic curve](https://en.wikipedia.org/wiki/Elliptic_curve_cryptography)
- [X25519](https://en.wikipedia.org/wiki/X25519), 
- [Ed25519](https://en.wikipedia.org/wiki/Ed25519)
- [X448](https://en.wikipedia.org/wiki/Curve448)
- [Ed448](https://en.wikipedia.org/wiki/Curve448)
- [GOST R 34.10-2001](https://en.wikipedia.org/w/index.php?title=GOST_R_34.10-2001&action=edit&redlink=1)
- [SM2](https://en.wikipedia.org/w/index.php?title=SM2_(algorithm)&action=edit&redlink=1)