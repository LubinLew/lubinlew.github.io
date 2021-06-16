# ECDH算法

`DH`算法还有一个变种叫做 `ECDH`(Elliptic-curve Diffie–Hellman)。 是`ECC`算法和`DH`的结合。

使用暴力（即尝试所有可能的密钥）对 `ECDH` 加密的消息进行解密的难度与离散对数问题属同一级别。不过，`ECDH` 算法使用较短的密钥即可达到相同程度的安全性，这是因为它依赖椭圆曲线而不是对数曲线。

----

### ECDH原理

`ECC`是建立在基于椭圆曲线的离散对数问题上的密码体制，给定椭圆曲线上的一个点`P`，一个整数`k`，求解`Q=kP`很容易；给定一个点`P`、`Q`，知道`Q=kP`，求整数`k`确是一个难题。ECDH即建立在此数学难题之上。

密钥磋商过程：

假设密钥交换双方为Alice、Bob，其有共享曲线参数（椭圆曲线E、阶N、基点G）。

1) Alice生成随机整数a，计算A=a*G。 #生成Alice公钥

2) Bob生成随机整数b，计算B=b*G。 #生产Bob公钥

3) Alice将A传递给Bob。A的传递可以公开，即攻击者可以获取A。
   由于椭圆曲线的离散对数问题是难题，所以攻击者不可以通过A、G计算出a。

4) Bob将B传递给Alice。同理，B的传递可以公开。

5) Bob收到Alice传递的A，计算Q =b*A  #Bob通过自己的私钥和Alice的公钥得到对称密钥Q

6) Alice收到Bob传递的B，计算Q`=a*B  #Alice通过自己的私钥和Bob的公钥得到对称密钥Q'

Alice、Bob双方即得Q=b*A=b*(a*G)=(b*a)*G=(a*b)*G=a*(b*G)=a*B=Q' (交换律和结合律)，即双方得到一致的密钥Q。

 目前Openssl里面的ECC算法的套件支持是ECDSA/ECDH。在国密的SSL套件中，可以使用ECDSA/ECC(密钥加密传输)，ECDSA/ECDH(密钥磋商)两种套件

由于 `ECDH` 密钥交换协议不验证公钥发送者的身份，因此无法阻止中间人攻击。如果监听者 Mallory 截获了 Alice 的公钥，就可以替换为他自己的公钥，并将其发送给 Bob。Mallory 还可以截获 Bob 的公钥，替换为他自己的公钥，并将其发送给 Alice。这样，Mallory 就可以轻松地对 Alice 与 Bob 之间发送的任何消息进行解密。他可以更改消息，用他自己的密钥对消息重新加密，然后将消息发送给接收者。

为了解决此问题，Alice 和 Bob 可以在交换公钥之前使用数字签名对公钥进行签名。有两种方法可以实现此目的：

- 使用安全的媒体（例如语音通信或可信载运商）在双方之间传输数字签名密钥。

- 使用公共证书颁发机构 (CA) 向双方提供可信数字签名密钥。

在这两种情况下，都必须使用外部身份验证方案来验证公钥发送者的身份。CNG 示例演示了当未对密钥发送者进行身份验证时会发生的情况。编写该示例旨在启用一个安全漏洞。

如果攻击者拥有大型[量子计算机](https://zh.wikipedia.org/wiki/%E9%87%8F%E5%AD%90%E8%AE%A1%E7%AE%97%E6%9C%BA "量子计算机")，那么他可以使用[秀尔算法](https://zh.wikipedia.org/wiki/%E7%A7%80%E7%88%BE%E6%BC%94%E7%AE%97%E6%B3%95 "秀尔算法")解决离散对数问题，从而破解私钥和共享秘密。目前的估算认为：破解256位素数域上的椭圆曲线，需要2330个[量子比特](https://zh.wikipedia.org/wiki/%E9%87%8F%E5%AD%90%E6%AF%94%E7%89%B9 "量子比特")与1260亿个[托佛利门](https://zh.wikipedia.org/wiki/%E6%89%98%E4%BD%9B%E5%88%A9%E9%96%98)。[[4]](https://zh.wikipedia.org/wiki/%E6%A9%A2%E5%9C%93%E6%9B%B2%E7%B7%9A%E8%BF%AA%E8%8F%B2-%E8%B5%AB%E7%88%BE%E6%9B%BC%E9%87%91%E9%91%B0%E4%BA%A4%E6%8F%9B#cite_note-4)相比之下，使用秀尔算法破解2048位的RSA则需要4098个[量子比特](https://zh.wikipedia.org/wiki/%E9%87%8F%E5%AD%90%E6%AF%94%E7%89%B9 "量子比特")与5.2万亿个[托佛利门](https://zh.wikipedia.org/wiki/%E6%89%98%E4%BD%9B%E5%88%A9%E9%96%98 "托佛利闸")。因此，椭圆曲线会更先遭到量子计算机的破解。目前还不存在建造如此大型量子计算机的科学技术，因此椭圆曲线密码学至少在未来十年（或更久）依然是安全的。但是密码学家已经积极展开了[后量子密码学](https://zh.wikipedia.org/wiki/%E5%90%8E%E9%87%8F%E5%AD%90%E5%AF%86%E7%A0%81%E5%AD%A6 "后量子密码学")的研究。其中，[超奇异椭圆曲线同源密钥交换](https://zh.wikipedia.org/w/index.php?title=%E8%B6%85%E5%A5%87%E5%BC%82%E6%A4%AD%E5%9C%86%E6%9B%B2%E7%BA%BF%E5%90%8C%E6%BA%90%E5%AF%86%E9%92%A5%E4%BA%A4%E6%8D%A2&action=edit&redlink=1)（SIDH）有望取代当前的常规椭圆曲线密钥交换（ECDH）

---

### Reference

https://www.cnblogs.com/HKUI/p/12865277.html

https://www.cnblogs.com/fishou/p/4206451.html

https://blog.csdn.net/mrpre/article/details/52608867

https://blog.csdn.net/xiangguiwang/article/details/74013733

[ECDH 算法概述（CNG 示例） | Microsoft Docs](https://docs.microsoft.com/zh-cn/previous-versions/visualstudio/visual-studio-2008/cc488016(v=vs.90))

https://en.wikipedia.org/wiki/Elliptic-curve_Diffie%E2%80%93Hellman