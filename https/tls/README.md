# SSL/TLS

> SSL: Secure Sockets Layer
> 
> TLS: Transport Layer Security

SSL协议是由网景(Netscape)公司发明的 。

网景是最早的互联网浏览器, 但是其依托于windows系统, 

网景浏览器是收费软件, 在其拒绝被微软收购后,被微软免费捆绑的IE浏览器击垮。

IETF组织接手维护SSL协议，TLS这个名字都是微软所起。

| 时间   | 版本      | 说明                      | RFC  | 过期                                                             |
| ---- | ------- | ----------------------- | ---- | -------------------------------------------------------------- |
| 1994 | SSLv1.0 | 未对外发布,有严重的安全漏洞          | -    | -                                                              |
| 1995 | SSLv2.0 | 对外发布,但是有安全缺陷            | -    | 2011,[RFC 6176](https://datatracker.ietf.org/doc/html/rfc6176) |
| 1996 | SSLv3.0 | 解决2.0的设计缺陷              | 6101 | 2015,[RFC 7568](https://datatracker.ietf.org/doc/html/rfc7568) |
| 1998 | -       | 网景公司被收购, SSL协议由IETF组织接手 | -    | -                                                              |
| 1999 | TLSv1.0 | SSL改名为TLS               | 2246 | 2020,被互联网厂商弃用                                                  |
| 2006 | TLSv1.1 | 增加CBC攻击防护等              | 4346 | 2020,被互联网厂商弃用                                                  |
| 2008 | TLSv1.2 | 增加PFR等                  | 5246 | -                                                              |
| 2018 | TLSv1.3 | 握手协议变更/只支持AEAD和HKDF     | 8446 | -                                                              |



### Reference

[TLS Security 3: SSL/TLS History | Acunetix](https://www.acunetix.com/blog/articles/tls-ssl-terminology-basics-part-2/)

[Transport Layer Security - Wikipedia](https://en.wikipedia.org/wiki/Transport_Layer_Security#SSL_1.0,_2.0,_and_3.0)
