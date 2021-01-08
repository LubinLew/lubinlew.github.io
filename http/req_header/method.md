# HTTP请求方法

## HTTP请求方法

HTTP1.0 定义了三种请求方法： GET, POST 和 HEAD方法。

HTTP1.1 新增了六种请求方法：OPTIONS、PUT、PATCH、DELETE、TRACE 和 CONNECT 方法。

[WebDAV](https://en.wikipedia.org/wiki/WebDAV)定义了七种请求方法：PROPPATCH、PROPFIND、MKCOL、COPY、MOVE、LOCK、UNLOCK 方法。

| No. | 方法        | 标准       | RFC                                                       | 有无body   | 说明                                                                         |
| --- | --------- | -------- | --------------------------------------------------------- | -------- | -------------------------------------------------------------------------- |
| 1   | GET       | HTTP 1.0 | [7231](https://tools.ietf.org/html/rfc7231#section-4.3.1) | Optional | Gets the contents of the resource.                                         |
| 2   | POST      | HTTP 1.0 | [7231](https://tools.ietf.org/html/rfc7231#section-4.3.3) | Yes      | Action defined by the server.                                              |
| 3   | HEAD      | HTTP 1.0 | [7231](https://tools.ietf.org/html/rfc7231#section-4.3.2) | Optional | Returns the message headers from a message sent to the server.             |
| 4   | OPTIONS   | HTTP 1.1 | [7231](https://tools.ietf.org/html/rfc7231#section-4.3.7) | Optional | Performs an option call to the server.                                     |
| 5   | PUT       | HTTP 1.1 | [7231](https://tools.ietf.org/html/rfc7231#section-4.3.4) | Yes      | Puts the contents of the resource to the server in the specified location. |
| 6   | PATCH     | HTTP 1.1 | [5789](https://tools.ietf.org/html/rfc5789#section-2)     | Yes      | Puts the contents of the resource to the server in the specified location. |
| 7   | DELETE    | HTTP 1.1 | [7231](https://tools.ietf.org/html/rfc7231#section-4.3.5) |          | Deletes the resource.                                                      |
| 8   | TRACE     | HTTP 1.1 | [7231](https://tools.ietf.org/html/rfc7231#section-4.3.8) | No       | Does a trace call to the server.                                           |
| 9   | CONNECT   | HTTP 1.1 | [7231](https://tools.ietf.org/html/rfc7231#section-4.3.6) | Optional | converts the request connection to a transparent TCP/IP tunnel             |
| 10  | PROPPATCH | WebDAV   | [4918](https://tools.ietf.org/html/rfc4918#section-9.2)   | -        | Sets or removes properties on the server.                                  |
| 11  | PROPFIND  | WebDAV   | [4918](https://tools.ietf.org/html/rfc4918#section-9.1)   | -        | Performs a property find on the server.                                    |
| 12  | MKCOL     | WebDAV   | [4918](https://tools.ietf.org/html/rfc4918#section-9.3)   | -        | Creates the collection specified.                                          |
| 13  | COPY      | WebDAV   | [4918](https://tools.ietf.org/html/rfc4918#section-9.8)   | -        | Copies the resource.                                                       |
| 14  | MOVE      | WebDAV   | [4918](https://tools.ietf.org/html/rfc4918#section-9.9)   | -        | Moves the resource.                                                        |
| 15  | LOCK      | WebDAV   | [4918](https://tools.ietf.org/html/rfc4918#section-9.10)  | -        | Locks the resource.                                                        |
| 16  | UNLOCK    | WebDAV   | [4918](https://tools.ietf.org/html/rfc4918#section-9.11)  | -        | Unlocks the resource.                                                      |
