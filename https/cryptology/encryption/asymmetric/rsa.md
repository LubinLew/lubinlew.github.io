# RSA

RSA 加密最大的数据长度等于秘钥的长度 (2048 bits = 256 bytes), 还要减去填充(padding)和数据头(11 bytes for PKCS#1 v1.5 padding).

因此, 通常不会直接使用RSA去加密文件(RSA 也并不是为这种用途设计的)。下面的命令是使用公钥加密的例子:

```bash
# 公钥加密
opopenssl rsautl -encrypt -in data.txt -inkey publickey.pem -pubin -out data.public_encrypt
# 私钥解密
openssl rsautl -decrypt -in data.public_encrypt -inkey privatekey.pem  -out data.decrypt
```

> 公钥加密，私钥解密时，因为加入随机数，每次得到的加密信息不固定。私钥加密，公钥解密时，得到的加密信息固定。

> `openssl rsautl` 只支持公钥加密私钥解密;不支持私钥加密公钥解密, 因为这种方式是不安全的。

> 私钥加密是RSA签名中的一个环节, 但是签名操作中还会包含 hashing 和 填充(padding)操作, 直接使用私钥加密很容易被破解。

如果`data.txt` 文件的内容超过了 245(假设是2048位, 则 245 = 256 - 11)个字节,则会报错

```txt
RSA operation error
140625604695952:error:0406D06E:rsa routines:RSA_padding_add_PKCS1_type_2:data too large for key size:rsa_pk1.c:174:
```

### References

https://en.wikipedia.org/wiki/RSA_(cryptosystem)

https://tls.mbed.org/kb/cryptography/rsa-encryption-maximum-data-size

https://opensource.com/article/21/4/encryption-decryption-openssl

https://gist.github.com/dreikanter/c7e85598664901afae03fedff308736b

https://blog.csdn.net/weixin_34226706/article/details/91576124

https://crypto.stackexchange.com/questions/2123/rsa-encryption-with-private-key-and-decryption-with-a-public-key

https://stackoverflow.com/questions/1181421/is-it-possible-to-encrypt-with-private-key-using-net-rsacryptoserviceprovider

https://stackoverflow.com/questions/5133246/what-is-the-purpose-of-using-separate-key-pairs-for-signing-and-encryption