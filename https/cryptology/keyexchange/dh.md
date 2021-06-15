# DH

`DH` 是 Diffie-Hellman 的首字母缩写。是一种密钥协商算法，是 Whitefield Diffie 与 Martin Hellman 在1976年提出了一个奇妙的密钥交换协议。它的巧妙之处在于双方最终协商的共同的密钥不会通过网络传输，而是双方各自计算出来的。


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



---

## Reference

https://www.cnblogs.com/HKUI/p/12865277.html

https://www.cnblogs.com/fishou/p/4206451.html

https://blog.csdn.net/mrpre/article/details/52608867

https://blog.csdn.net/xiangguiwang/article/details/74013733