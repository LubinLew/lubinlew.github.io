# 使用 nginx 设置基本认证

> [HTTP Basic Authentication认证](https://www.cnblogs.com/yuqiangli0616/p/9389273.html)
> 
> [HTTP认证之摘要认证](https://www.cnblogs.com/xiaoxiaotank/p/11078571.html)
> 
> [How To Set Up Password Authentication with Nginx](https://www.digitalocean.com/community/tutorials/how-to-set-up-password-authentication-with-nginx-on-ubuntu-14-04)

HTTP认证分为基本认证(HTTP Basic Authentication) 和 摘要认证，基本认证被广泛支持。Nginx中原生支持 基本认证。



## Nginx 命令字段

#### auth_basic

```nginx
Syntax:  auth_basic string | off;
Default: auth_basic off;
Context: http, server, location, limit_except
```

值为 `off` 的时候表示关闭基本认证功能
值为其他时候是 `realm` , 即用户名密码输入的提示语.

#### auth_basic_user_file

```nginx
Syntax:  auth_basic_user_file file;
Default: —
Context: http, server, location, limit_except
```

该指令用于设置基本认证的用户名和密码文件, 文件的格式如下:

```bash
# comment
name1:password1
name2:password2:comment
name3:password3
```

注意上面的密码指的不是明文密码, 是经过加密后的密码.

---

## 密码加密方式

### 1.使用函数 `crypt()` 加密

函数`crypt()`是一种标准的unix密码加密算法, 

```c
#define _XOPEN_SOURCE       /* See feature_test_macros(7) */
#include <unistd.h>

/* 参数 salt 是一个 盐值, 是一个只有2个字符的字符串,字符取值为[a-zA-Z0-9]
 * 输出结果的前两位就是这个盐值, 下面使用命htpasswd或openssl令每次使用相同的密码却生成不同的结果的原因就在此(每次盐值是随机)
 */
char *crypt(const char *key, const char *salt);

#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <crypt.h>

char *crypt_r(const char *key, const char *salt, struct crypt_data *data);
       
/* Link with -lcrypt. */
```

这种加密方式可以使用Apache的 `htpasswd` 命令来完成

```bash
$ yum install httpd-tools    # apt-get install apache2-utils

# htpasswd -c passwdfile username # 会要求你输入密码,需要交互
# htpasswd -bc passwdfile username password # 这种方式会在历史记录里留下明文密码

$ htpasswd -bc /etc/nginx/auth.db myuser mypassword
```

或者使用 `openssl` 命令来完成

```bash
$ echo -n 'usrname:' >> /etc/nginx/auth.db
$ openssl passwd -crypt >> /etc/nginx/auth.db # 需要交互输入密码
```

### 2.使用 `apr1`一种基于MD5变种的密码加密算法

这个可用使用 `openssl` 命令来完成

```bash
$ echo -n 'usrname:' >> /etc/nginx/auth.db
$ openssl passwd -apr1 >> /etc/nginx/auth.db # 需要交互输入密码
```

### 3.使用 [RFC 2307](https://tools.ietf.org/html/rfc2307#section-5.3) 中指定的 “`{scheme}data`” 语法

目前nginx中实现了下面3种方案（scheme）： 

###### 明文方案 `PLAIN`

明文密码, 不应该被使用

```nginx
user1:{PLAIN}1234  # 明文秘密 1234
```

#### 明文SHA1方案 `SHA`

明文 SHA-1 哈希, 不应该被使用, 支持的目的是方便从其他服务器迁移到nginx, 

这里看了源码才搞明白, 比如你输入的密码是 `1234` ,首先要对它做SHA1, nginx中使用函数`ngx_crypt_sha()` 得到一个20个字节的SHA1字节数组, 其值用HEX表示是40个字符的`7110eda4d09e062aa5e4a390b0a572ac0d2c0220`, 但是nginx是对这个20个字节数组做的base64编码, 而不是40个字节的HEX字符串.

通常计算SHA1的命令工具如 `sha1sum` 和 `openssl` 默认得出的结果是40个字符长度的HEX串,

```bash
[user]$ echo -n '1234' | sha1sum
7110eda4d09e062aa5e4a390b0a572ac0d2c0220  -

[user]$ echo -n '1234' | openssl sha1
(stdin)= 7110eda4d09e062aa5e4a390b0a572ac0d2c0220
```

使用 `xxd -r -p` 指令及一将这个40个字符的HEX串 转换成 20个字节的二进制(也就是ngxinx函数`ngx_crypt_sha()`计算得到的结果), 之后使用base64进行转换.

> [Transform hexadecimal information to binary using a Linux command](https://stackoverflow.com/questions/7826526/transform-hexadecimal-information-to-binary-using-a-linux-command)

```bash
# 计算密码 1234 的 nginx SHA方案
# "{SHA}" base64(SHA1(key))
echo -n '1234' | sha1sum | cut -b 1-40 | xxd -r -p | base64
```

密码文件配置:

```nginx
# NGINX配置 user2 密码为 1234
user2:{SHA}cRDtpNCeBiql5KOQsKVyrA0sAiA=
```

#### 加盐SHA1方案  `SSHA`

[加盐的](https://en.wikipedia.org/wiki/Salt_(cryptography)) SHA-1 哈希, 被 [OpenLDAP](https://www.openldap.org/) and [Dovecot](https://www.dovecot.org/) 等软件使用.

计算方式同上面的SHA类似。

需要对`密码+盐值`做 SHA1, `SHA1+盐值` 做base64.

例如 密码是 `1234` 盐值是`ABCD`, 使用下面脚本可以得出最终的SSHA值

```bash
#/usr/bin/bash

# METHOD: "{SSHA}" base64(SHA1(key salt) salt)

# 密码
PASS="1234"
# 盐值
SALT="ABCD"

# 计算加盐密码的 SHA1
SALT_SHA1=`echo -n ${PASS}${SALT} | sha1sum | cut -b 1-40 | xxd -r -p`

# 加盐SHA1 + 盐 做 base64
SSHA=`echo -n ${SALT_SHA1}${SALT} | base64`

echo ${SSHA}
```

密码文件配置:

```bash
# NGINX配置 user3 密码为 1234
user3:{SSHA}vJ6zAXCOeVNNXivEL/mlr5PA2JdBQkNE
```

> Apache 的基本认证密码加密方式见 [Apache Server Password Formats](http://httpd.apache.org/docs/2.4/misc/password_encryptions.html)
> 
> 未加盐的 SHA-1 哈希, 容易被 [rainbow table](http://en.wikipedia.org/wiki/Rainbow_attack) 等方式攻击利用.
