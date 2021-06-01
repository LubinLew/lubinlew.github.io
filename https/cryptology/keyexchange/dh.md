# DH & ECDH

`DH` 是 Diffie-Hellman 的首字母缩写。是一种密钥协商算法，是 Whitefield 与 Martin Hellman 在1976年提出了一个奇妙的密钥交换协议。它的巧妙之处在于双方最终协商的共同的密钥不会通过网络传输，而是双方各自计算出来的。

`DH`算法还有一个变种叫做 `ECDH`(Elliptic-curve Diffie–Hellman)。 是`ECC`算法和`DH`的结合。

----

## DH原理

- 发送方和接收方设置相同的大素数 `n` 和 `g`，这两个数不是保密的， 他们可以通过非安全通道来协商这两个素数。

- 发送方选择一个大随机整数x，计算 `X = g^x mod n`，发送`X`给接收者；

- 接收方选择一个大随机整数y，计算 `Y = g^y mod n`，发送`Y`给发送方；

 确定最终密钥：

- 发送方密钥为 `k1 = Y^x mod n`，
- 接收方密钥为 `k2 = X^y mod n`。

最终`k1 == k2`, 其他人可以知道`n`、 `g`、 `X`和`Y`， 但是他们不能计算出密钥，除非他们能恢复`x`和`y`。

缺点：不能抵御中间人攻击。
解决方法：使用认证的方法，确保X 、Y可信。

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