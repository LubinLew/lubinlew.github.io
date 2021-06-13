# 对称加密

## 对称加密算法类型

对称加密算法有两种类型，分别是`块密码算法`(block ciphers)和`流密码算法`(stream

ciphers)。

### 流密码算法

| 名称     | 密钥长度           | 说明                                                                      |
| ------ | -------------- | ----------------------------------------------------------------------- |
| RC4    | 可变长密钥，建议2048比特 | 已经证明不安全，详见[RC4](https://en.wikipedia.org/wiki/RC4#Security)             |
| ChaCha | 可变长密钥，建议256比特  | 目前比较安全,详见[ChaCha](https://en.wikipedia.org/wiki/Salsa20#ChaCha_variant) |

流密码算法之所以称为流密码算法， 就在于每次 XOR 运算的时候，是连续对数据流进行运算的一种算法，每次处理的数据流大小一般是一个字节。流密码算法可以并行处理，运算速度非常快。

### 块密码算法

#### 分组模式

##### ECB(Electronic codebook)

![ECB_E](_resources/ECB_encryption.svg)

##### CBC

![CBC_E](_resources/CBC_encryption.svg)

##### CTR

![CTR_E](_resources/CTR_encryption.svg)

##### GCM

##### CCB
