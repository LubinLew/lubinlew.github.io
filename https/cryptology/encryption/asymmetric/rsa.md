# RSA

1977年，三位数学家Rivest、Shamir 和 Adleman 设计了一种算法，可以实现非对称加密。这种算法用他们三个人的名字命名，叫做RSA算法。

从那时直到现在，RSA算法一直是最广为使用的"非对称加密算法"。

这种算法非常可靠，密钥越长，它就越难破解。随着计算机算力提升，目前普遍认为 2048 位的密钥是安全的。

RSA 算法使用密钥长度可以是 1024、2048、4096、...、16384位的。

RSA 还支持更长的密钥（例如 65536 位），但因为性能太差而无法在现实生活中使用（某些操作可能需要几分钟甚至几小时）。

RSA 算法对应标准的是 [RFC8017](https://datatracker.ietf.org/doc/html/rfc8017), 即 PKCS #1: RSA Cryptography Specifications Version 2.2。

#### 常用术语

| 英文               | 中文    | 说明                |
| ---------------- | ----- | ----------------- |
| Modulus          | 模数    |                   |
| Exponent         | 指数    |                   |
| Prime Number     | 素数，质数 | 只能被1和其自身整除        |
| Modulo Operation | 模运算   | 下文中的 mod 即为模运算的简写 |

使用 OpenSSL 生成RSA密钥对

> ```bash
> # 生成 RSA 私钥（2048位）
> openssl genrsa -out privatekey.pem 2048
> # 生成 RSA 公钥
> openssl rsa -in privatekey.pem -pubout -out publickey.pem
> ```

---

## 基础知识

**欧拉定理**是RSA算法的核心, 所以理解了欧拉定理就能理解RSA算法。

### 质数

**质数**(prime)又被称为素数，是指一个大于1的自然数(表示物体个数的数叫自然数,从0开始)，除了1和它自身外，不能被其它自然数整除。

如果两个正整数，除了1以外，没有其他公因子，我们就称这两个数是**互质关系**(coprime)。

#### 质数的性质

- 质数的约数只有 1 和 他本身

- 1 和任意一个自然数是都是互质关系

- 质数的个数是无限的

- 任何一个大于1的自然数,要么本身是质数, 要么可以分解为几个质数之积,而且这种分解是唯一的

- 任意两个质数构成互质关系

- 一个数是质数，另一个数只要不是前者的倍数，两者就构成互质关系

- 如果两个整数之中，较大的那个数是质数，则两者构成互质关系

- p 是大于1的整数，则 p 和 p-1 构成互质关系

- p 是大于1的奇数，则 p 和 p-2 构成互质关系

### 欧拉函数

给定一个正整数 n，在 (0, n) 区间内有多少正整数与 n 构成互质关系, 计算这个解的方法叫做欧拉函数，记作φ(n). 例如 φ(8)=4，因为1,3,5,7均和8互质。

- n=1, 则 φ(1) = 1, 因为1与任何数（包括自身）都构成互质关系。

- n 是质数, 则 φ(n) = n - 1, 因为质数与小于它的每一个数，都构成互质关系。

- n 是质数的某一个次方, 即 n = p<sup>k</sup>(p 为质数,k 为大于等于1的整数), 则φ(n) = φ(p<sup>k</sup>) = p<sup>k</sup> - p<sup>k-1</sup> = p<sup>k</sup>(1 - 1/p), 这是因为只有当一个数不包含质数 p，才可能与 n 互质。而包含质数 p的数一共有 p<sup>k-1</sup> 个，即1×p、2×p、3×p、...、p<sup>k-1</sup>×p，把它们减掉，剩下的就是与 n 互质的数。

- n 是两个互质数 p 和 q 的乘积, 即 n = p × q, 则 φ(n) = φ(p) × φ(q) = (p-1) × (q-1)，这个需要使用中国剩余定理来证明。



### 欧拉定理

如果两个正整数 a 和 n 互质，则 a<sup>φ(n)</sup> ≡ 1 (mod n)

> 欧拉定理（Euler Theorem）是一个关于同余的性质。其他同余定理还有 威尔逊定理、费马小定理、广义欧拉定理。



### 费马小定理

p 为质数, a 为任意自然数, 则 a<sup>p</sup> ≡ a (mod p), 如果 a 不是 p 的倍数则 a<sup>p-1</sup> ≡ 1 (mod p)

> 费马小定理（Fermat's Little Theorem, FLT）

#### 

### 模逆元

模逆元也称为 模倒数、模逆元、模反元素。如果两个正整数 a 和 n 互质，那么一定可以找到整数 b，使得 ab 被 n 除的余数是 1, 即 ab ≡ 1 (mod n), 这时，b就叫做a的模反元素。

---

### 原理

> **基础知识**
> 
> 1.取模运算(mod)
> 
>   又称模除、取模操作、取模运算等，英语：modulo 有时也称作 modulus, 得到的是一个数除以另一个数的余数。
> 
>   给定两个正整数：被除数 *a* 和除数 *n*，*a* modulo *n* (缩写为 *a* mod *n*), 
> 
>   举个例子：计算表达式 "5 mod 2" 得到 1，因为 5÷2=2...1（5 除以 2 商 2 余1）
> 
> 2.**同余**
> 
>   当两个整数除以同一个正整数，若得相同余数，则二整数同余。符号表示为 `≡` 。
> 
>   两个整数 a, b ,若他们除以正整数 m 所得的余数相等, 则称 a,b 对于模 m 同余, 记做 a ≡ b (mod m)

标记说明:

| 标记      | 说明                           |
| ------- | ---------------------------- |
| `p`,`q` | 两个互质的整数                      |
| `n`     | p,q 的乘积, 用二进制表示时的位数即密钥长度     |
| `e`     | 满足 1< e < φ(n)，且 e 与 φ(n) 互质 |
| `d`     | 满足 e * d ≡ 1 (mod φ(n))      |
| `m`     | 明文,整数                        |
| `c`     | 密码,证书                        |
| `h`     | hash 结果                      |
| `s`     | 签名                           |

基本原理：对于两个不同的质数相乘容易，而将其乘积进行分解会很难，即 `n = p * q`, 已知 p 和 q 求 n 简单, 但是已知 n 求 p1 和 p2 很难。

随机选择一个整数 e，满足 1< e < φ(n)，且 e 与φ(n) 互质。找到一个整数 d，满足 <mark>e * d ≡ 1 (mod φ(n))</mark>。

RSA秘钥对包含下面两个部分:

- 公钥 {***n***, ***e***}

- 私钥 {***n***, ***d***}

> 其中 n 为两个质数的乘积, 
> 
> e 也是整数, 当 e 取值较小的时候典型值为 65537

RSA加密的原理：<mark>c ≡ m<sup>e</sup> mod n</mark>  ，c 表示密文，m 表示明文，e 和 n 为公钥。已知 m、e 、n 求 c 简单，但是 已知 e、n、c 求 m 很难。

RSA解密的原理：<mark>m ≡ c<sup>d </sup>mod n</mark>  ，解密需要 整数 d

> - 使用 OpenSSL 命令 从公钥中提取 n 和 e 
> 
> ```bash
> $ openssl rsa -in publickey.pem -pubin -noout -text
> Public-Key: (2048 bit)
> Modulus:
>     00:aa:9b:d4:cb:f7:b8:ee:65:f7:9d:f1:9e:4b:db:
>     11:ed:c5:80:f2:65:17:fc:85:8a:ad:d7:15:f5:ac:
>     82:67:64:32:d3:ce:3a:a3:ef:19:77:e3:f5:96:f5:
>     60:e7:f9:fc:ca:24:d1:15:1e:e8:1f:5e:52:cd:92:
>     3a:0a:43:28:c4:2d:ce:5d:ff:4e:6c:b2:f1:85:c4:
>     b9:ce:b4:ef:3b:22:92:3e:a2:b6:ec:8d:a7:67:43:
>     9f:bd:dd:ea:a0:b9:31:30:ec:0f:02:24:3e:c3:15:
>     0d:df:9f:5d:cd:8a:c1:de:e5:aa:f1:5c:55:e5:fa:
>     65:b2:f6:5e:9d:fb:37:9a:6f:15:50:40:ec:0b:27:
>     46:ed:81:72:ba:9f:33:8f:8b:9e:1f:17:b1:fa:9a:
>     7e:d3:2a:14:5d:ab:31:f1:7d:49:d3:5d:37:bc:de:
>     cb:9b:82:1b:46:c8:5a:c0:53:14:f8:ae:e2:19:3a:
>     8f:e6:9c:c8:7c:65:fc:99:8a:75:dc:5e:3a:81:af:
>     99:44:c2:f5:7a:93:8a:d5:c9:39:71:15:60:1c:e1:
>     39:4a:c5:ae:d1:46:1d:1a:db:16:6e:cd:30:78:27:
>     e7:c8:f7:06:52:a7:16:2f:2a:5d:e2:d7:5e:91:ed:
>     2b:b3:3c:74:2e:52:cd:c7:f1:f6:90:0b:63:a7:81:
>     b1:8b
> Exponent: 65537 (0x10001)
> ```
> 
> - 使用 Python3 从公钥中提取 n 和 e 
> 
> ```python
> #!/usr/bin/env python3
> from Crypto.PublicKey import RSA # pip3 install pycryptodome
> 
> path = 'publickey.pem'
> with open(path) as f:
>     key = RSA.import_key(f.read())
>     print('e = %d' % key.e)
>     print('n = %d' % key.n)
> 
> # 输出结果
> e = 65537
> n = 21537355091329431454230296287616964265156428779857293588345978555507481825161951807416504377433425189284485550210862718422829177717655788086525206254605500918862068767751067941935374733368996480459842687116593872392946926966450485566746705382839034882679958750027060463896238687826044804440805928081473025246289813810950134385198067956193536860272349423290602086876676723314708404668686986850933113920881531008169660461863860700144070533359707784381207670252326748675089033225279728199730551210310633191785419982679774143866927907357332707585582532162321284480119530833108970428908135045774690873511110807887240343947
> ```

---

### 限制

#### 加密内容长度的限制

<mark>RSA 加密最大的数据长度等于秘钥的长度 (2048 bits = 256 bytes), 还要减去填充(padding)和数据头(11 bytes for PKCS#1 v1.5 padding).</mark>
因此, 通常不会直接使用RSA去加密文件(RSA 也并不是为这种用途设计的)。下面的命令是使用公钥加密的例子:

```bash
# 公钥加密
opopenssl rsautl -encrypt -in data.txt -inkey publickey.pem -pubin -out data.public_encrypt
# 私钥解密
openssl rsautl -decrypt -in data.public_encrypt -inkey privatekey.pem  -out data.decrypt
```

如果`data.txt` 文件的内容超过了 245(假设是2048位, 则 245 = 256 - 11)个字节,则会报错

```txt
RSA operation error
140625604695952:error:0406D06E:rsa routines:RSA_padding_add_PKCS1_type_2:data too large for key size:rsa_pk1.c:174:
```

#### 加密方向的限制

> [pkcs#1v1.5_pkcs 1 v1 5有什么特别之处，而这种攻击不会消失_weixin_26636643的博客-CSDN博客](https://blog.csdn.net/weixin_26636643/article/details/108899088)

 公钥加密，私钥解密时，因为加入随机数，每次得到的加密信息不固定。私钥加密，公钥解密时，得到的加密信息固定。 `openssl rsautl` 只支持公钥加密私钥解密;不支持私钥加密公钥解密, 因为这种方式是不安全的。

---

## RSA签名和验签

### RSA签名

1.Calculate the message hash: ***h*** = hash(***m***)

2.Encrypt ***h*** to calculate the signature: s = h<sup>d</sup> (mod n)

The hash ***h*** should be in the range [0...***n***). The obtained **signature** ***s*** is an integer in the range [0...***n***).

### RSA验签

1.Calculate the message hash: ***h*** = hash(***m***)

2.Decrypt the signature: h′ = s<sup>e</sup> (mod n)

3.Compare ***h*** with ***h'*** to find whether the signature is valid or not

If the signature is correct, then the following will be true: h′=s<sup>e</sup> (mod n) = (h<sup>d</sup>)<sup>e</sup>(mod n) = h

---

### References

[RSA算法原理（一） - 阮一峰的网络日志](https://www.ruanyifeng.com/blog/2013/06/rsa_algorithm_part_one.html)

https://en.wikipedia.org/wiki/RSA_(cryptosystem)

[RSA Signatures - Practical Cryptography for Developers](https://cryptobook.nakov.com/digital-signatures/rsa-signatures)

https://tls.mbed.org/kb/cryptography/rsa-encryption-maximum-data-size

https://opensource.com/article/21/4/encryption-decryption-openssl

https://gist.github.com/dreikanter/c7e85598664901afae03fedff308736b

https://blog.csdn.net/weixin_34226706/article/details/91576124

https://crypto.stackexchange.com/questions/2123/rsa-encryption-with-private-key-and-decryption-with-a-public-key

https://stackoverflow.com/questions/1181421/is-it-possible-to-encrypt-with-private-key-using-net-rsacryptoserviceprovider

https://stackoverflow.com/questions/5133246/what-is-the-purpose-of-using-separate-key-pairs-for-signing-and-encryption