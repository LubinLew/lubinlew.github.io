# 握手规则

websocket 使用 HTTP 协议进行握手，所以握手协议完全遵循 HTTP RFC 2616 的规则。

具体握手的规则：

## 客户端握手规则

- 请求方法必须是 `GET`

- HTTP协议的版本至少是 `1.1`，因为后续 websocket 协议还是使用这条连接，`1.0`版本会关闭连接。

- 请求头中必须有 `Ugrade`字段，其值必须包含 `websocket`

- 请求头中必须有 `Connection`字段，其值必须包含 `Upgrade`

- 如果请求来浏览器，那么请求头中必须有 `Origin`字段。其值是建立连接源地址的ACSII序列化的结果

- 请求头中必须有 `Sec-WebSocket-Key`字段，其值必须是 base64 十六个随机字节得到的，每一个连接都必须随机选择随机数。

- 请求头中必须有 `Sec-WebSocket-Version`字段，其值必须是 `13`

- 请求头中<u>可能</u>有`Sec-WebSocket-Protocol` 字段，声明根据权重排序的子协议(逗号分隔)

- 请求头中<u>可能</u>有`Sec-WebSocket-Extensions` 字段，声明使用的扩展功能

```http
GET /chat HTTP/1.1
Host: server.example.com
Upgrade: websocket
Connection: keep-alive, Upgrade
Origin: http://example.com
Sec-WebSocket-Version: 13
Sec-WebSocket-Key: P47DD5+bQXOtc/bDrMlE7A==
Sec-WebSocket-Extensions: permessage-deflate
Sec-WebSocket-Protocol: chat, superchat
```

## 服务端握手规则

具体规则如下:

- 同意客户端的请求则回复状态码 `101`，否则返回错误码，就不需要下面的其他条件

- 请求头中必须有 `Ugrade`字段，其值必须包含 `websocket`

- 请求头中必须有 `Connection`字段，其值必须包含 `Upgrade`

- 请求头中必须有 `Sec-WebSocket-Accept`字段，其值计算详见 [section-4.2.2](https://www.rfc-editor.org/rfc/rfc6455#section-4.2.2)

- 请求头中<u>可能</u>有`Sec-WebSocket-Protocol` 字段，声明支持的子协议

- 请求头中<u>可能</u>有`Sec-WebSocket-Extensions` 字段，声明同意使用的扩展功能

```http
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: PVV/GgFIJNHYqv8LUyjjFAsG0Is=
Sec-WebSocket-Extensions: permessage-deflate
Sec-WebSocket-Protocol: chat
```
