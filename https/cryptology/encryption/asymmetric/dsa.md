# DSA

DSA（Digital Signature Algorithm，数字签名算法），它是另一种公开密钥算法，它不能用作加密，只用作数字签名。DSA使用公开密钥，为接受者验证数据的完整性和数据发送者的身份。它也可用于由第三方去确定签名和所签数据的真实性。

### 算法原理

The DSA 算法包含了4个部分：

- 密钥生成(key generation)
- 密钥发布(key distribution)
- 生成签名(signing)
- 验证签名(signature verification)

#### 密钥生成

密钥生成分为2个阶段， 第一个阶段是选择共享的算法参数，第二个阶段是生成

##### 选择共享的算法参数

- 选择一个合适的密码学哈希函数 `H`(SHA-1/SHA-2等)
- 选择密钥长度 `L`
- 选择模量 `N`
- 选择一个 `N` 比特的质数 `q`
- 选择一个 `L` 比特的质数 `p`
- 从 `{2 .. q-2}` 随机选择一个整形 `h`
- 计算 <mark>g := h<sup>(h-1)/q</sup> mod p</mark>

算法的公共参就是 `(p, q, g)`

##### 计算公私钥

- 从 `{1 ... q-1}` 中随机选择一个整形 `x` 作为私钥
- 计算公钥 <mark>y := g<sup>x</sup> mod p</mark>

#### 密钥发布

签名的实施者应该公开其公钥 `y`，并且保证其私钥 `x` 仅自己可见。

#### 生成签名

下面是对一个消息 `m` 进行签名的过程：

- 从 `{1 ... q-1} `范围内随机选择一个整数 `k`
- 计算 <mark>`r` := (g<sup>k</sup>mod p) mod q</mark>; 如果 `r` 等于0 则重新选择 `k`
- 计算 <mark>`s` := (k<sup>-1</sup>(H(m) + xr)) mod q</mark>;如果 `s` 等于 0 则重新选择 `k`

那么我们就得到了签名 `(r, s)`

#### 验证签名

使用下面步骤签证签名 `(r, s)`：

- 验证 `0 < r < q`  并且 `0 < s < q`
- 计算 <mark>w := s<sup>-1</sup> mod q</mark>
- 计算 <mark>u<sub>1</sub> := H(m) . w mod q</mark>
- 计算 <mark>u<sub>2</sub> := r . w mod q</mark>
- 计算 <mark>v := (g<sup>u<sub>1</sub></sup>y<sup>u<sub>2</sub></sup> mod p) mod q</mark>

如果 `v` 与 `r` 相等，则验证成功

DSA算法的安全性基于解离散对数的困难性，这类签字标准具有较大的兼容性和适用性，成为网络安全体系的基本构件之一。DSA 的一个重要特点是两个素数公开，这样，当使用别人的`p`和`q`时，即使不知道私钥，你也能确认它们是否是随机产生的，还是作了手脚。RSA 算法却作不到。

### References

https://en.wikipedia.org/wiki/Digital_Signature_Algorithm

https://blog.csdn.net/kotonohaparty/article/details/6977180

https://blog.csdn.net/aaqian1/article/details/89299520