# X-Content-Type-Options

`X-Content-Type-Options` HTTP响应标头是服务器使用的标记，
用于指示浏览器应遵循在 `Content-Type` 标头中声明的 MIME 类型而不能更改。

此标头由微软在 IE 8 中引入的，作为阻止 MIME 嗅探的一种方式，防止将不可执行的 MIME 类型转换为可执行的 MIME 类型。 
之后其他浏览器也引入了这一个标头。

## MIME 嗅探(sniffing)

在没有 MIME 类型的情况下，或者在浏览器认为它们不正确的情况下，浏览器可能会执行 MIME 嗅探。
浏览器实现 MIME 嗅探的方式可能是不同的，有的通过查看资源的字节来猜测正确的 MIME 类型。
有的如 Safari 会查看 URL 中的文件扩展名来猜测正确的 MIME 类型。
由于某些 MIME 类型代表可执行内容，因此存在安全问题。
服务器可以通过发送 `X-Content-Type-Options` 标头来防止 MIME 嗅探。

## 语法

```mime
X-Content-Type-Options: nosniff
```

用来阻止 类型是 `style` 但是 MIME 不是  `text/css` 和 类型是 `script` 但是MIME类型不是 JavaScript 的情况。
同时它还为 HTML、TXT、JSON 和 XML 文件（不包括 SVG `image/svg+xml`）启用跨源读取阻止 (CORB) 保护。