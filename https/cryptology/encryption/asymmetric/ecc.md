# ECC

ECC(Elliptic Curve Cryptography)椭圆曲线密码学,是一种基于椭圆曲线数学的公开密钥加密算法。
椭圆曲线在密码学中的使用是在1985年由[Neal Koblitz](https://zh.wikipedia.org/w/index.php?title=Neal_Koblitz&action=edit&redlink=1)和[Victor Miller](https://zh.wikipedia.org/w/index.php?title=Victor_Miller&action=edit&redlink=1)分别独立提出的。

ECC的主要优势是它相比RSA算法使用较小的密钥长度并提供相同等级的安全性,有研究表示160位的椭圆密钥与1024位的RSA密钥安全性相同。
ECC的另一个优势是可以定义群之间的双线性映射，基于Weil对或是Tate对；
双线性映射已经在密码学中发现了大量的应用，例如基于身份的加密。

椭圆曲线密码学的许多形式有稍微的不同，所有的都依赖于被广泛承认的解决椭圆曲线离散对数问题的困难性上，对应有限域上椭圆曲线的群







| 名称                    | 简称    | 简介                                    | 曲线方程                               |
| --------------------- | ----- | ------------------------------------- | ---------------------------------- |
| prime192v1(secp192r1) | P-192 |                                       | y<sup>2</sup> = x<sup>3</sup>+ax+b |
| prime256v1(secp256r1) | P-256 | 256-bit prime field Weierstrass curve | y<sup>2</sup> = x<sup>3</sup>+ax+b |
| secp224r1             | P-224 |                                       |                                    |
| secp384r1             | P-384 |                                       |                                    |
| secp521r1             | P-521 |                                       |                                    |
| secp256k1             | -     | 比特币使用的曲线                              |                                    |
| SM2                   | -     | 国密SM2使用的曲线                            |                                    |



http://safecurves.cr.yp.to/

https://www.cryptosys.net/pki/eccrypto.html

https://en.wikipedia.org/wiki/Elliptic-curve_cryptography