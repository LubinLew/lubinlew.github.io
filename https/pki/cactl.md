# 共享系统证书

> [Using Shared System Certificates on Red Hat Enterprise Linux 7](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/7/html/security_guide/sec-shared-system-certificates)

共享系统证书存储(Shared System Certificates storage) 允许[NSS](https://developer.mozilla.org/en-US/docs/Mozilla/Projects/NSS), [GnuTLS](https://www.gnutls.org/), [OpenSSL](https://www.openssl.org/), 和Java 共享用于检索系统证书锚点和黑名单信息的默认源. 默认情况下，信任库包含Mozilla CA列表(包括可信和不可信)。 该系统允许更新核心Mozilla CA列表或选择另一个证书列表。

> [OpenSSL](https://www.openssl.org/) 应用最广泛的密码学库和TLS实现, 看过其源码的人都知道其代码风格混乱,文档缺乏.
> 
> [NSS](https://developer.mozilla.org/en-US/docs/Mozilla/Projects/NSS)(Network Security Services) 这是Mozilla维护一个类似OpenSSL的开源库. 主要被Mozilla(如Firefox、Thunderbird等) Apache和 Red Hat等公司使用.
> 
> [GnuTLS](https://www.gnutls.org/)这是GNU组织维护的用来取代OpenSSL的开源库,[因为OpenSSL的授权许可问题,GNU组织不能随意使用其代码, 另外GNU认为其代码架构有安全问题](https://lists.gnu.org/archive/html/gnutls-devel/2001-08/msg00015.html). 有意义的是GnuTLS 早期版本有各种高危漏洞, 如今这个库一直在完善, 但是具体的安全性还需要跟踪了解.

## 什么是信任锚(Trust Anchors)

在一个分级密码学(hierarchical cryptographic)系统中,  *trust anchor* 是假定为可信的权威实体. 例如,在 X.509 架构中,根证书就是一个信任锚，可从中获得信任链. 必须事先将信任锚放置在信任存储区中，以便于验证。在DNSSEC的上下文中，信任锚由DNS名称和与该名称关联的公共密钥（或公共密钥的哈希）组成。 它表示为以64为基数的编码密钥。 它与证书相似，因为它是交换信息（包括公钥）的一种方式，可用于验证和认证“ DNS”记录。 [RFC 4033](http://tools.ietf.org/html/rfc4033#page-6)将信任锚定义为DNSKEY RR的已配置DNSKEY RR或DS RR哈希。 验证安全意识的解析器使用此公钥或哈希作为构建身份验证链到签名DNS响应的起点。 通常，验证解析器将必须通过DNS协议之外的某些安全或可信方式来获取其信任锚的初始值。 信任锚的存在还意味着解析程序应该期望信任锚指向的区域被签名。

> [Understanding Trust Anchors](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/7/html/security_guide/sec-Securing_DNS_Traffic_with_DNSSEC#sec-Security_Guide-Understanding_trust_anchors)

## 使用全局(System-wide)信任存储区(Trust Store)

在CentOS 7中, 信任存储区是 `/etc/pki/ca-trust/` 和`/usr/share/pki/ca-trust-source/` 这2个目录. 其中 `/usr/share/pki/ca-trust-source/` 的优先级比 `/etc/pki/ca-trust/`低.

不同子目录下的证书是有区别的不同的 :

- `/usr/share/pki/ca-trust-source/anchors/` or `/etc/pki/ca-trust/source/anchors/` -  trust anchors. 

- `/usr/share/pki/ca-trust-source/blacklist/` or `/etc/pki/ca-trust/source/blacklist/` - 不可信的证书.

- `/usr/share/pki/ca-trust-source/` or `/etc/pki/ca-trust/source/` - for certificates in the extended BEGIN TRUSTED file format.

### 增加新的证书

想要把一个PEM或者DER格式的证书加入的可信CA列表中,只需要将其拷贝到目录 `/usr/share/pki/ca-trust-source/anchors/` 或者`/etc/pki/ca-trust/source/anchors/` 中. 然后执行 `update-ca-trust` 命令进行更新即可

```bash
[cmd]$ cp ~/certificate-trust-examples/Cert-trust-test-ca.pem /usr/share/pki/ca-trust-source/anchors/
[cmd]$ update-ca-trust
```

> 注意事项:当CA变化后, 执行 `update-ca-trust` 命令后. 浏览器需要清空缓存或者重启才能加载最新的CA配置

## 管理 Trusted System Certificates

使用 `trust` 命令可以对 trust anchors 进行增删改查操作. 

```bash
trust --help
usage: trust command <args>...

Common trust commands are:
  list             List trust or certificates
  extract          Extract certificates and trust
  extract-compat   Extract trust compatibility bundles
  anchor           Add, remove, change trust anchors
  dump             Dump trust objects in internal format

See 'trust <command> --help' for more information
```

#### 查看证书列表

产看证书列表

```bash
trust list
pkcs11:id=%d2%87%b4%e3%df%37%27%93%55%f6%56%ea%81%e5%36%cc%8c%1e%3f%bd;type=cert
    type: certificate
    label: ACCVRAIZ1
    trust: anchor
    category: authority

pkcs11:id=%f7%7d%c5%fd%c4%e8%9a%1b%77%64%a7%f5%1d%a0%cc%bf%87%60%9a%6d;type=cert
    type: certificate
    label: AC RAIZ FNMT-RCM
    trust: anchor
    category: authority

pkcs11:id=%d1%09%d0%e9%d7%ce%79%74%54%f9%3a%30%b3%f4%6d%2c%03%03%1b%68;type=cert
    type: certificate
    label: AC Ra\xC3\xADz Certic\xC3\xA1mara S.A.
    trust: anchor
    category: authority
....
```

```bash
trust list --help
usage: trust list --filter=<what>

  --filter=<what>     filter of what to export
                        ca-anchors        certificate anchors
                        blacklist         blacklisted certificates
                        trust-policy      anchors and blacklist (default)
                        certificates      all certificates
                        pkcs11:object=xx  a PKCS#11 URI
  --purpose=<usage>   limit to certificates usable for the purpose
                        server-auth       for authenticating servers
                        client-auth       for authenticating clients
                        email             for email protection
                        code-signing      for authenticating signed code
                        1.2.3.4.5...      an arbitrary object id
  -v, --verbose       show verbose debug output
  -q, --quiet         suppress command output
```

```bash
trust list --filter ca-anchors
pkcs11:id=%1e%0c%f7%b6%67%f2%e1%92%26%09%45%c0%55%39%2e%77%3f%42%4a%a2;type=cert
    type: certificate
    label: ePKI Root Certification Authority
    trust: anchor
    category: authority

pkcs11:id=%7b%5b%45%cf%af%ce%cb%7a%fd%31%92%1a%6a%b6%f3%46%eb%57%48%50;type=cert
    type: certificate
    label: thawte Primary Root CA
    trust: anchor
    category: authority

pkcs11:id=%9a%d8%00%30%00%e7%6b%7f%85%18%ee%8b%b6%ce%8a%0c%f8%11%e1%bb;type=cert
    type: certificate
    label: thawte Primary Root CA - G2
    trust: anchor
    category: authority
....
```

#### 添加证书

给定证书的路径,即可将其加入信任存储区

`trust anchor path.to/certificate.crt`

#### 删除证书

可以用证书的路径或者ID进行删除

`trust anchor --remove path.to/certificate.crt`

`trust anchor --remove "pkcs11:id=%AA%BB%CC%DD%EE;type=cert"`
