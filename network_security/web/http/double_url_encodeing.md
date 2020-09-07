# 二次URL解码引起的注入攻击

> 参考文章:
> 
> [二次urldecode注入 - My_Dreams - 博客园](https://www.cnblogs.com/zzjdbk/p/12986079.html)

PHP中常用过滤函数如`addslashes()`、`mysql_real_escape_string()`、`mysql_escape_string()`或者使用魔术引号`GPC`开关来防止注入，原理都是给单引号（’）、双引号（”）、反斜杠（\）和NULL等特殊字符前面加上反斜杠来进行转义。

 但是这些函数在遇到`urldecode()`函数时，就会因为二次解码引发注入。`urldecode()`函数是对已编码的URL进行解码。引发注入的原因其实很简单，PHP本身在处理提交的数据之前会进行一次解码，例如`/test.php?id=1`这个URL，我们构造字符串`/test.php?id=1%2527`，PHP第一次解码，`%25`解码成了`%`，于是url变成了`/test.php?id=%27`；然后`urldecode()`函数又进行了一次解码，`%27`解码成了`’`，于是最终URL变成了`/test.php?id=1’`，单引号引发了注入。`rawurldecode()`也会产生同样的问题，因此这两个函数需要慎用。

## 构建测试环境(Centos7)

### 安装配置MySQL

```bash
## 安装
[CMD] yum install -y mariadb-server
[CMD] systemctl start mariadb

## 进入mysql
[CMD] mysql

##设置 root 密码 为 root
mysql> set password for root@localhost = password('root');

## 创建 测试数据库 test
mysql> CREATE DATABASE IF NOT EXISTS test DEFAULT CHARSET utf8 COLLATE utf8_general_ci;
mysql> use test;

## 创建 测试用 table
mysql> CREATE TABLE IF NOT EXISTS `user_email`( 
    `id` INT UNSIGNED AUTO_INCREMENT, 
    `user` VARCHAR(100) NOT NULL, 
    `user_email` VARCHAR(100) NOT NULL, 
    PRIMARY KEY ( `id` ) 
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

## 向table 中添加测试数据
mysql> INSERT INTO user_email (user,email) VALUES ("Kitty Cat","kitty@cat.com");
mysql> INSERT INTO user_email (user,email) VALUES ("Jone Doe","jone@doe.com");


```

### 安装配置PHP

```bash
[CMD] yum install -y php php-fpm php-mysql
[CMD] systemctl start php-fpm
```

安装配置Nginx


