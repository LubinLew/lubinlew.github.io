# HTTPS

HTTP的设计目的是数据传输和共享，没有考虑安全问题。

## 为什么HTTP不安全 ?

主要有三点原因，这三点也是安全领域的根本问题，任何基于TCP/IP 的应用都会遇到:

- 无法保护隐私信息(HTTP是明文的)
- 无法验证服务端身份(容易被钓鱼网站攻击)
- 无法鉴别数据篡改(容易被HTTP劫持)

> 运营商劫持:包含 DNS劫持 和 HTTP劫持 两种。
> 
> 路由器劫持: 很多家用路由器也会进行HTTP劫持


## 什么是HTTPS ?

HTTPS = HTTP + SSL/TLS

Hypertext Transfer Protocol Secure

Hypertext Transfer Protocol over Secure Socket Layer

## Reference

BOOK: 深入浅出HTTPS从原理到实践

BOOK: Implementing SSL/TLS Using Cryptography and PKI

BOOK: Network Security withOpenSSL: Cryptography for Secure Communications

BOOK: Bulletproof SSL and TLS: Understanding and Deploying SSL/TLS and PKI to Secure Servers and Web Applications

BOOK: OpenSSL Cookbook