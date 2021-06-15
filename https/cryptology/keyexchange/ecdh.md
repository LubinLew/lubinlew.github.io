ECDH

`DH`算法还有一个变种叫做 `ECDH`(Elliptic-curve Diffie–Hellman)。 是`ECC`算法和`DH`的结合。

----


## ECDH原理

`ECC`是建立在基于椭圆曲线的离散对数问题上的密码体制，给定椭圆曲线上的一个点P，一个整数k，求解Q=kP很容易；给定一个点P、Q，知道Q=kP，求整数k确是一个难题。ECDH即建立在此数学难题之上。密钥磋商过程：

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

---

## Reference

https://www.cnblogs.com/HKUI/p/12865277.html

https://www.cnblogs.com/fishou/p/4206451.html

https://blog.csdn.net/mrpre/article/details/52608867

https://blog.csdn.net/xiangguiwang/article/details/74013733