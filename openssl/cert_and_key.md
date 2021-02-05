# 密钥和证书管理

本文翻译整理自[OpenSSL Cookbook](https://www.feistyduck.com/books/openssl-cookbook/) 的【Key and Certificate Management】章节。

**下面主要描述的是怎样使用OpenSSL命令来生成搭建HTTPS Web Server需要的私钥和证书**。环境自然是Linux。

通常分为以下几个步骤：　　

- **生成私钥文件**
- **生成证书签名请求(CSR)文件**
- **CA机构或者自己签发证书**
- **将私钥和证书导入Web Server**

 在开始之前，需要知道的几个基本术语

- 对称加密 与 非对称加密
- PKI
- X509
- CA
- CSR

---

## 秘钥

### 秘钥生成的前提

使用公共加密(pbulic encryption)的第一步是生成一个私钥(private key)，在生成私钥前你要作3个选择，选择使用什么加密算法、选择密钥的长度 和 选择是否使用通行码保护密钥。

#### 密钥算法

OpenSSL支持RSA、DSA 俺的 ECDSA 密钥,但是并不是所有类型的密钥在所有的情景下都是实用的。例如，**所有人都是用RSA算法生成SSL密钥**，这是因为DSA密码的长度限制为1024位(Windows系统支持的最大位数)，并且ECDSA密钥没有被CA机构广泛支持。对于SSH，DSA 和 RSA的使用都很广泛，但是不是所有的客户端都支持ECDSA密钥。

#### 密钥长度

默认的密钥长度可能是不安全的，所以生成密钥时需要明确的配置密钥的长度。例如，RSA密钥的默认长度是512位，这是不安全的。现如今如果你的服务器密钥是512位的，那么入侵者可以使用的你的证书采用暴力破解的方式获取你的私钥，之后他能够冒充你的网站。

**现如今2048位的RSA的密钥被认为是安全的，并且这也是你应该使用的密钥长度。**

同样的2048位的DSA密钥，至少224位的ECDSA密钥是被认为安全的。

#### 通行码(Passphrase)

使用通行码保护私钥是可选的，但是建议使用。受保护的密钥可以安全的存储、传输 和 备份。但是这样也会造成不便，因为没有通行码你就不能使用这些秘钥。例如，你每次重启的web服务器时都可能要求你输入通行码。对于大部分人来说，这样很不方面，或者产生一些不能接受的影响。实际上使用受保护的私钥并不会增强安全性。这是因为一旦使用了该私钥，私钥就会保存在程序的内容中并且不受保护。能够访问服务器的攻击者只需要一些技巧就能够从内存中拿到私钥。通行码应该被视为保护未安装在产品系统中的私钥的一种机制，

从另一方面说，即使把通行码保存在产品系统中也没有关系，这也比使用不受通行码保护的密钥好的多。如果你需要更强的安全性，你应该投资一个硬件解决方案。通行码的保护方式有DES、3DES、SEED、AES-128、AES-192、AES-256，**建议使用AES**。

### 生成密钥

#### 生成不使用通行码保护的RSA私钥

```bash
# 生成不使用通行码保护的私钥
$ openssl genrsa -out fd.key 2048
Generating RSA private key, 2048 bit long modulus
............................................+++
...................+++
e is 65537 (0x10001)

# 显示公钥的内容
$ cat fd.key 
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEA5xT4MQorNkAvyZRZ5pXdejI+tUESWXKvDudG6MXhOHSy2S+d
...省略
WTAvKwaY88M3KawO1AeDiBEN8+HQ8Y9bK6lXb+jWBhUfSoWwhTD3WZU=
-----END RSA PRIVATE KEY-----
$ 
```

#### 生成使用通行码保护的RSA私钥

```bash
# 生成使用通行码保护的私钥
$ openssl genrsa -aes256 -out fd.key 2048
Generating RSA private key, 2048 bit long modulus
..............................................................................+++
..+++
e is 65537 (0x10001)
Enter pass phrase for fd.key: ****
Verifying - Enter pass phrase for fd.key:****

# 显示生成私钥的内容
$ cat fd.key 
-----BEGIN RSA PRIVATE KEY-----
Proc-Type: 4,ENCRYPTED
DEK-Info: AES-256-CBC,DEE8683382BEC12E8D6ADD71DA91C222

uFgnmm9R40heSNzoRKrNrKbwDwfBvDO+KAabn5OVsLAGK3KjcEf2+xtiBo7FiRgg
...省略
CdxlQvDamSBpGM8OqtVX6CXQkPE34mg5fezWTvqwaSZWJtBbtIVeVRUamL50pH/P
-----END RSA PRIVATE KEY-----

# 去除私钥中的通行码保护
$ openssl rsa -in fd.key -out fd.key
```

#### 生成私钥对应的公钥

```bash
$ openss rsa -in fd.key -pubout -out fd-public.key
Enter pass phrase for fd.key:****  # 如果是没有通行码保护的私钥则不需要输入通行码
writing RSA key

$ cat fd-public.key 
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsgw1bgwqlzToi4oC+T3Q
gksuTiPJpNbUBWrpXa2tyCqcDp01u3mgxd/WXcEKoxWrUeAwfRXA0PZQY1QP0sPS
cdjWtPvGxBGEplstN+kOHBh9miMPXhVKr6PjCcI4nWDDXXh9IHnXta8O0zKy5jlk
+IUmTaf2WGen1o9wiscYTeSwlz4FLENNU3s+7N6fEG1jPggE5B6fzWabfUAFhrCY
RIQFEMn1c6fM/NgZSJaGUKXOWPcfRR/+aYJtls2eXpeGz53PBB4JLOH9kTI0+Asa
3+IFDXncFUg724bsFmYxJ57DGau9S+KqEkT2FmdxQR5JoGC2by+xsgMS0i14x3aR
oQIDAQAB
-----END PUBLIC KEY-----
```

----

## 创建证书签名请求(CSR)

拥有了私钥之后, 你就可以创建一个证书签名请求(Certificate Signing Request/CSR)文件。  可以使用这个文件向CA机构请求签名的证书(通常是广受信任的CA机构，需要付费的，当然也有免费的)。  当然也可以也可以自己签发证书。制作CSR文件通常是一个交互的过程，需要用户输入许多相关的信息。其中需要注意的是Common Name字段是网址。 

```bash
# 根据私钥生成csr文件
$ openssl req -new -key fd.key -out fd.csr
You are about to be asked to enter information that will be incorporated
into your certificate request.
What you are about to enter is what is called a Distinguished Name or a DN.
There are quite a few fields but you can leave some blank
For some fields there will be a default value,
If you enter '.', the field will be left blank. 
#接下来会要求你输入几个字段的内容，如果你想让该字段留空，那么你应该输入一个英文句号后回车，
# 如果只敲了回车，那么某些字段会有默认值，这可能跟你想要的不同，所以需要注意这一点。
-----
Country Name (2 letter code) [XX]:CN
State or Province Name (full name) []:Beijing
Locality Name (eg, city) [Default City]:Beijing
Organization Name (eg, company) [Default Company Ltd]:Some Technology Co., Ltd. 
Organizational Unit Name (eg, section) []:Organizational-Unit-Name
Common Name (eg, your name or your server's hostname) []:www.test.com
Email Address []:test@test.com

Please enter the following 'extra' attributes
to be sent with your certificate request
A challenge password []:
An optional company name []:

# 直接查看生成的csr文件内容
$ cat fd.csr
-----BEGIN CERTIFICATE REQUEST-----
MIIC8zCCAdsCAQAwga0xCzAJBgNVBAYTAkNOMRAwDgYDVQQIDAdCZWlqaW5nMRAw
...省略
q1oHo7p6HknUwqTY4dcI0BhVQT6cXBf+PXaSCCwxljXWRNY7ex/H
-----END CERTIFICATE REQUEST-----

# 查看csr文件中输入等信息，可以检查是否有输入错误
$ openssl req -text -in fd.csr -noout
Certificate Request:
    Data:
        Version: 0 (0x0)
        Subject: C=CN, ST=Beijing, L=Beijing, O=Some Technology Co., Ltd. , OU=Organizational-Unit-Name, CN=www.test.com/emailAddress=test@test.com
        Subject Public Key Info:
            Public Key Algorithm: rsaEncryption
                Public-Key: (2048 bit)
                Modulus:
                    00:e7:14:f8:31:0a:2b:36:40:2f:c9:94:59:e6:95:
                    ...省略
                    a8:48:87:b3:ce:c6:92:ee:a6:30:30:cc:11:33:60:
                    37:81
                Exponent: 65537 (0x10001)
        Attributes:
            a0:00
    Signature Algorithm: sha256WithRSAEncryption
         cc:64:64:52:10:7c:09:e9:83:e6:9d:97:82:17:8f:74:74:7d:
         ...省略
         55:41:3e:9c:5c:17:fe:3d:76:92:08:2c:31:96:35:d6:44:d6:
$
```

从上面的例子中我们可以看到生成证书的过程需要填写很多字段，OpenSSL允许我们将这么字段预先写入配置文件，然后就可以引用配置文件生成证书，不用手动输入。

```bash
# cat fd.cnf
[req]
prompt = no
distinguished_name = distinguished_name
[distinguished_name]
CN = www.test.com
emailAddress = test@test.com
O = Some Technology Co., Ltd.
OU = Organizational-Unit-Name
ST = Beijing
L = Beijing
C = CN
```

使用配置文件生成CSR文件

```bash
# 使用配置文件生成CSR文件
$ openssl req -new -config fd.cnf -key fd.key -out fd.csr
# 如果有pass phrase 则需要输入
```

---

## 签发证书

生成CSR文件后，你有2个选择，一个是发送给CA机构请求其为你签发证书，另一个就是自己签发证书。 其实向CA机构申请证书是不用自己生成CSR文件，CA机构提供全套服务，交钱就行了。下面主要讲自己签发证书。

### 自签发单域名证书

```bash
# 根据csr文件自己签发证书
$ openssl x509 -req -days 365 -in fd.csr -signkey fd.key -out fd.crt
Signature ok
subject=/C=CN/ST=Beijing/L=Beijing/O=Some Technology Co., Ltd. /OU=Organizational-Unit-Name/CN=www.test.com/emailAddress=test@test.com
Getting Private key

$ cat fd.crt 
-----BEGIN CERTIFICATE-----
MIID2DCCAsACCQCYrBR/rdwvITANBgkqhkiG9w0BAQUFADCBrTELMAkGA1UEBhMC
...省略
kbIPcNJTtSgFFHG++DORSmgelKqJIXN/Sr+aaBDo2byClpWk6iYsGOejf5QZOTFb
8BIf5p6tCNh8YleVfkUzjtXzcVIBojs66g7EGg==
-----END CERTIFICATE-----
```

### 自签发多域名证书

OpenSSL默认签发单域名的证书，想要签发多域名的证书需要使用x509格式的Subject Alternative Name (SAN)扩展字段。这个字段有2种方法声明多个域名，第一种罗列所有域名，第二种是使用通配符，二者可以混用，使用的时候需要使用配置文件的方式。

```bash
# cat fd.ext
subjectAltName = DNS:*.test.com, DNS:test.com
```

使用扩展字段生成自签发多域名证书时，之前输入的Common Name 字段就会被忽略，可以看作描述性属性。

```bash
# 自签发多域名证书
$ openssl x509 -req -days 365 -in fd.csr -signkey fd.key -out fd.crt -extfile fd.ext
Signature ok
subject=/emailAddress=test@test.com/CN=www.test.com/O=Some Company Ltd/L=Beijing/C=CN
Getting Private key

# 查看证书信息
$ openssl x509 -text -in fd.crt -noout
Certificate:
    Data:
        Version: 3 (0x2)
        Serial Number: 14605685625506933521 (0xcab1d1b1315eab11)
    Signature Algorithm: sha1WithRSAEncryption
        Issuer: emailAddress=test@test.com, CN=www.test.com, O=Some Company Ltd, L=Beijing, C=CN
        Validity
            Not Before: Apr  4 01:22:11 2016 GMT
            Not After : Apr  4 01:22:11 2017 GMT
        Subject: emailAddress=test@test.com, CN=www.test.com, O=Some Company Ltd, L=Beijing, C=CN
        Subject Public Key Info:
            Public Key Algorithm: rsaEncryption
                Public-Key: (2048 bit)
                Modulus:
                    00:e7:14:f8:31:0a:2b:36:40:2f:c9:94:59:e6:95:
　　　　　　　　　　　　...省略
                    a8:48:87:b3:ce:c6:92:ee:a6:30:30:cc:11:33:60:
                    37:81
                Exponent: 65537 (0x10001)
        X509v3 extensions:
            X509v3 Subject Alternative Name: 
                DNS:*.test.com, DNS:test.com
    Signature Algorithm: sha1WithRSAEncryption
         b4:12:f8:6f:3d:6d:30:05:67:12:de:a6:33:65:99:66:8a:75:
 　　　　　...省略
         ce:77:6e:02:50:15:bc:99:af:fc:ca:2b:d3:bd:14:13:f6:5f
$
```

生成的证书可以使用windows系统查看，多域名在证书中存储位置见下图

![证书](_sources/cert.png)

### 快速自签发证书

如果仅仅是生成自己签发的证书，那么可以省略生成CSR的过程，直接使用私钥签发证书

```bash
# 快速自签发证书
$ openssl req -new -x509 -days 365 -key fd.key -out fd.crt
# 可以使用配置文件
$ openssl req -new -x509 -days 365 -config fd.cnf -key fd.key -out fd.crt



```

多域名的配置文件fd.ext可以和普通的配置文件fd.cnf合并起来

```bash
# cat fd.cnf
[req]
prompt = no
distinguished_name = distinguished_name
x509_extensions = extensions

[distinguished_name]
CN = www.test.com
emailAddress = test@test.com
O = Some Technology Co., Ltd.
OU = Organizational-Unit-Name
ST = Beijing
L = Beijing
C = CN

[extensions]
subjectAltName = DNS:*.test.com, DNS:test.com
```

这个配置文件还是和`-config`项一起使用。

----

## 秘钥证书的格式转换

> 在线工具 https://csr.chinassl.net/

### 证书的格式

> [数字证书常见格式整理 - 崔咩咩](https://www.cnblogs.com/cuimiemie/p/6442685.html)
> 
> [证书格式简介及不同格式之间的转换方式 - eshizhan ](https://www.cnblogs.com/eshizhan/archive/2012/10/07/2713557.html)

| 格式          | 扩展名                 | 格式                  | 说明  |
| ----------- | ------------------- | ------------------- | --- |
| PEM         | .pem/.key/.crt/.cer | Base64编码的ASCII码文件存储 |     |
| DER         | .der                | 二进制文件存储             |     |
| PKCS#7/P7B  | .p7b 或 .p7c         | Base64的格式存储         |     |
| PKCS#12/PFX | .pfx 或 .p12         | 加密二进制形式存储           |     |

**PEM to DER**

`openssl x509 -outform der -in certificate.pem -out certificate.der`

**PEM to P7B**

`openssl crl2pkcs7 -nocrl -certfile certificate.cer -out certificate.p7b -certfile CACert.cer`

**PEM to PFX**

`openssl pkcs12 -export -out certificate.pfx -inkey privateKey.key -in certificate.crt -certfile CACert.crt`

**DER to PEM**

`openssl x509 -inform der -in certificate.cer -out certificate.pem`

**P7B to PEM**

`openssl pkcs7 -print_certs -in certificate.p7b -out certificate.cer`

**PFX to PEM**

`openssl pkcs12 -in certificate.pfx -out certificate.cer -nodes`

PXF转PEM后`certificate.cer`文件包含认证证书和私钥，需要把它们分开存储才能使用。


