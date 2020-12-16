# ssl_session_store 阶段

> [lua-nginx-module/ssl_session_fetch](https://github.com/openresty/lua-nginx-module#ssl_session_fetch_by_lua_block)

**设置指令**

| 命令                             | 来源        | 上下文  | 阶段                        | 备注  |
| ------------------------------ | --------- | ---- | ------------------------- | --- |
| ssl_session_store_by_lua_block | OpenResty | http | right-after-SSL-handshake |     |
| ssl_session_store_by_lua_file  | OpenResty | http | right-after-SSL-handshake |     |

## 简介

This directive runs Lua code to fetch and save the SSL session (if any) according to the session ID provided by the current SSL handshake request for the downstream. The saved or cached SSL session data can be used for future SSL connections to resume SSL sessions without going through the full SSL handshake process (which is very expensive in terms of CPU time).

Lua APIs that may yield, like [ngx.sleep](https://github.com/openresty/lua-nginx-module#ngxsleep) and [cosockets](https://github.com/openresty/lua-nginx-module#ngxsockettcp), are *disabled* in this context. You can still, however, use the [ngx.timer.at](https://github.com/openresty/lua-nginx-module#ngxtimerat) API to create 0-delay timers to save the SSL session data asynchronously to external services (like `redis` or `memcached`).

The Lua API for obtaining the current session ID and the associated session state data is provided in the [ngx.ssl.session](https://github.com/openresty/lua-resty-core/blob/master/lib/ngx/ssl/session.md#readme) Lua module shipped with the [lua-resty-core](https://github.com/openresty/lua-resty-core#readme) library.

To easily test this hook locally with a modern web browser, you can temporarily put the following line in your https server block to disable the TLS session ticket support:

```
ssl_session_tickets off;
```

But do not forget to comment this line out before publishing your site to the world.

This directive was first introduced in the `v0.10.6` release.

Note that: this directive is only allowed to used in **http context** from the `v0.10.7` release (because SSL session resumption happens before server name dispatch).
