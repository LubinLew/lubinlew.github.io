# TLS 加速方案

TLS加速方案基本分为 软件加速和硬件加速。

软件加速有 OpenSSL的dasync 异步引擎等。

硬件加速通常都是专用的RSA算法加速卡，例如 Intel QAT、Cavium SSL card 等。



## Tengine 的加速方案

> [Tengine 使用OpenSSL异步引擎](https://tengine.taobao.org/document_cn/ngx_http_ssl_asynchronous_mode_cn.html)
> 
> [Tengine 使用Intel QAT硬件加速卡优化TLS性能](http://tengine.taobao.org/document_cn/tengine_qat_ssl_cn.html)
> 
> [OpenSSL async模块框架分析 - toong ](https://www.cnblogs.com/hugetong/p/14379347.html)

tengine 是阿里基于nginx二次开发的web服务器，其中针对TLS部分新增了`ssl_async`选项用来加速TLS。

##### 软加速：

需要使用OpenSSL 1.1.0之后的版本中在算法层提供的异步的引擎模块`dasync`。 具体使用配置方法参考 [ssl_async](https://tengine.taobao.org/document_cn/ngx_http_ssl_asynchronous_mode_cn.html)。

##### 硬加速：

使用外部专用算法加速硬件如Intel® QuickAssist Technology(QAT)，具体使用配置方法参考 [QAT](http://tengine.taobao.org/document_cn/tengine_qat_ssl_cn.html)。

Intel的QAT卡, 推荐了一个典型的应用场景, 并且开源了代码, 这组代码包含四部分，

- (Tengine不需要)nginx的patch，https://github.com/intel/asynch_mode_nginx

- openssl新增的async mode，已经合并到了openssl的主分支。

- 一个openssl engine叫qatengine 详见: https://github.com/intel/QAT_Engine

- 一个跟随硬件发布的驱动程序，可以在intel网站下载



## Inline-TLS

> [linux kernel inline tls support](https://www.admin-magazine.com/Archive/2018/45/Let-the-hardware-do-the-heavy-lifting/(offset)/3)

Linux 内核在4.13版本引入了 inline TLS 特性，可以在socket层设置相关特性从而让硬件(网卡)来完整计算，从而减少CPU的运算。


