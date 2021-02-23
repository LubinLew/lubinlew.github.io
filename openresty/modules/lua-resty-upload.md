# lua-resty-upload

> [openresty/lua-resty-upload: Streaming reader and parser for http file uploading](https://github.com/openresty/lua-resty-upload)

这个Lua库是ngx_lua nginx模块的流文件上传API, 支持`multipart/form-data` MIME类型。 该库的API只会逐一返回 `token`。用户只需要重复调​​用`read`方法，直到返回的`token` 值为 `nil`。对于从`read`方法返回的每个token，只需检查当前令牌类型的第一个返回值即可。令牌类型可以是`header`，`body`和`part end`。解析的每个multipart / form-data表单字段都由保存每个字段头的几个头标记，保存每个主体数据块的几个主体标记和指示字段结束的部分结束标志组成。 这就是流阅读的工作方式。即使对于千兆字节的文件数据输入，只要用户自己不累积输入数据块，用于lua的内存也可能很小且恒定。 这个Lua库利用了ngx_lua的cosocket API，该API确保100％的非阻塞行为。


