# ssl_session_fetch 阶段

> [lua-nginx-module/ssl_session_fetch](https://github.com/openresty/lua-nginx-module#ssl_session_fetch_by_lua_block)

**设置指令**

| 命令                             | 来源        | 上下文  | 阶段                         | 备注  |
| ------------------------------ | --------- | ---- | -------------------------- | --- |
| ssl_session_fetch_by_lua_block | OpenResty | http | right-before-SSL-handshake |     |
| ssl_session_fetch_by_lua_file  | OpenResty | http | right-before-SSL-handshake |     |



## 简介

通过客户端的`client-hello`消息中`Session iD`来搜索和加载SSL会话。

The Lua API for obtaining the current session ID and loading a cached SSL session data is provided in the [ngx.ssl.session](https://github.com/openresty/lua-resty-core/blob/master/lib/ngx/ssl/session.md) Lua module shipped with the [lua-resty-core](https://github.com/openresty/lua-resty-core#readme) library.

Lua APIs that may yield, like [ngx.sleep](https://github.com/openresty/lua-nginx-module#ngxsleep) and [cosockets](https://github.com/openresty/lua-nginx-module#ngxsockettcp), are enabled in this context.

This hook, together with the [ssl_session_store_by_lua*](https://github.com/openresty/lua-nginx-module#ssl_session_store_by_lua_block) hook, can be used to implement distributed caching mechanisms in pure Lua (based on the [cosocket](https://github.com/openresty/lua-nginx-module#ngxsockettcp) API, for example). If a cached SSL session is found and loaded into the current SSL connection context, SSL session resumption can then get immediately initiated and bypass the full SSL handshake process which is very expensive in terms of CPU time.

Please note that TLS session tickets are very different and it is the clients' responsibility to cache the SSL session state when session tickets are used. SSL session resumptions based on TLS session tickets would happen automatically without going through this hook (nor the [ssl_session_store_by_lua*](https://github.com/openresty/lua-nginx-module#ssl_session_store_by_lua_block) hook). This hook is mainly for older or less capable SSL clients that can only do SSL sessions by session IDs.

When [ssl_certificate_by_lua*](https://github.com/openresty/lua-nginx-module#ssl_certificate_by_lua_block) is specified at the same time, this hook usually runs before [ssl_certificate_by_lua*](https://github.com/openresty/lua-nginx-module#ssl_certificate_by_lua_block). When the SSL session is found and successfully loaded for the current SSL connection, SSL session resumption will happen and thus bypass the [ssl_certificate_by_lua*](https://github.com/openresty/lua-nginx-module#ssl_certificate_by_lua_block) hook completely. In this case, Nginx also bypasses the [ssl_session_store_by_lua*](https://github.com/openresty/lua-nginx-module#ssl_session_store_by_lua_block) hook, for obvious reasons.

To easily test this hook locally with a modern web browser, you can temporarily put the following line in your https server block to disable the TLS session ticket support:

```
ssl_session_tickets off;
```

But do not forget to comment this line out before publishing your site to the world.

If you are using the [official pre-built packages](https://openresty.org/en/linux-packages.html) for [OpenResty](https://openresty.org/) 1.11.2.1 or later, then everything should work out of the box.

If you are not using one of the [OpenSSL packages](https://openresty.org/en/linux-packages.html) provided by [OpenResty](https://openresty.org/), you will need to apply patches to OpenSSL in order to use this directive:

[OpenResty - OpenSSL patches by OpenResty](https://openresty.org/en/openssl-patches.html)

Similarly, if you are not using the Nginx core shipped with [OpenResty](https://openresty.org/) 1.11.2.1 or later, you will need to apply patches to the standard Nginx core:

[OpenResty - Nginx patches by OpenResty for SSL features](https://openresty.org/en/nginx-ssl-patches.html)

This directive was first introduced in the `v0.10.6` release.

Note that this directive can only be used in the **http context** starting with the `v0.10.7` release since SSL session resumption happens before server name dispatch.
