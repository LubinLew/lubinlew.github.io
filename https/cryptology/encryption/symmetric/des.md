# DES

DES([Data Encryption Standard](https://en.wikipedia.org/wiki/Data_Encryption_Standard)) 是由 IBM 设计,并在 1977 年被美国国家标准局选为国家标准([FIPS 46](https://csrc.nist.gov/publications/detail/fips/46/archive/1977-01-15))。

DES 应该是最早的现代密码学算法之一。算法本身已完全发布，但设计标准一直保密，直到 1994 年, DES 的设计者之一 Coppersmith 公开发布.

他表示，IBM 团队在算法设计期间就已经知道了称为差分密码分析的攻击，并且它对选择 [S-box](https://en.wikipedia.org/wiki/S-box) 产生了影响。 

S-box 是如何产生的是个谜，很多人都相信，这个 S-box 中存在后门，只要美国政府需要，就能够解密任何 DES 密文。

DES 密钥长度为 56 位,目前基本可以在一天之内完成暴力破解。DES 已经是一个过时的密码学算法，已经不推荐使用。