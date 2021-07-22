公钥加密，私钥解密时，因为加入随机数，每次得到的加密信息不固定。私钥加密，公钥解密时，得到的加密信息固定。 `openssl rsautl` 只支持公钥加密私钥解密;不支持私钥加密公钥解密, 因为这种方式是不安全的。

# RSA

1977年，三位数学家Rivest、Shamir 和 Adleman 设计了一种算法，可以实现非对称加密。这种算法用他们三个人的名字命名，叫做RSA算法。从那时直到现在，RSA算法一直是最广为使用的"非对称加密算法"。这种算法非常可靠，密钥越长，它就越难破解。随着计算机算力提升，目前普遍认为 2048位的密钥是安全的。

> ```bash
> # 生成 RSA 私钥（2048位）
> openssl genrsa -out privatekey.pem 2048
> # 生成 RSA 公钥
> openssl rsa -in privatekey.pem -pubout -out publickey.pem
> ```

---

### 原理

基本原理：对于两个质数相乘容易， 而将其乘积分解很难，即 `n = p1 * P2`, 已知 p1 和 p2 求 n 简单, 但是已知 n 求 p1 和 p2 很难。

RSA加密的原理：<mark>c = m<sup>e</sup> mod n</mark>  c 表示密文，m 表示明文，e 和 n 为公钥。已知 m、e 、n 求 c 简单，但是 已知 e、n、c 求 m 很难。

RSA解密的原理：<mark>m = c<sup>d </sup>mod n</mark>  解密需要 私钥 d 

> n 为随机选取的两个质数p1 和  p2的乘积, e为一个随机整数但是要与 φ(n) (欧拉公式)互为质数.

> - 使用 OpenSSL 命令 从公钥中提取 n 和 e 
> 
> ```bash
> openssl rsa -in publickey.pem -pubin -noout -text
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

### References

https://en.wikipedia.org/wiki/RSA_(cryptosystem)

https://tls.mbed.org/kb/cryptography/rsa-encryption-maximum-data-size

https://opensource.com/article/21/4/encryption-decryption-openssl

https://gist.github.com/dreikanter/c7e85598664901afae03fedff308736b

https://blog.csdn.net/weixin_34226706/article/details/91576124

https://crypto.stackexchange.com/questions/2123/rsa-encryption-with-private-key-and-decryption-with-a-public-key

https://stackoverflow.com/questions/1181421/is-it-possible-to-encrypt-with-private-key-using-net-rsacryptoserviceprovider

https://stackoverflow.com/questions/5133246/what-is-the-purpose-of-using-separate-key-pairs-for-signing-and-encryption