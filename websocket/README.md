# WebSocket

> [WebSocket 教程 - 阮一峰的网络日志](http://www.ruanyifeng.com/blog/2017/05/websocket.html)
> 
> [WebSocket 协议 RFC 文档 全中文翻译](https://segmentfault.com/a/1190000018217630)

## 简介

WebSocket 协议在2008年诞生，2011年成为国际标准 [RFC 6455](https://tools.ietf.org/html/rfc6455)。目前浏览器都已经支持。它的最大特点就是，服务器可以主动向客户端推送信息，客户端也可以主动向服务器发送信息，是真正的双向平等对话，属于[服务器推送技术](https://en.wikipedia.org/wiki/Push_technology)的一种。与 HTTP 一样都是建立 TCP 协议之上，并且与 HTTP 协议有着良好的兼容性。默认端口也是 80 和 443，并且握手阶段采用 HTTP 协议，因此握手时不容易屏蔽，能通过各种 HTTP 代理服务器。协议标识符是`ws`（如果加密，则为`wss`），服务器网址就是 URL。

```bash
ws://www.example.com/uri  # 不用TLS加密,同HTTP
wss://www.example.com/uri # 使用TLS加密,同HTTPS
```