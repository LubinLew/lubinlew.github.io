# balancer 阶段

> [lua-nginx-module/balancer_by_lua](https://github.com/openresty/lua-nginx-module#balancer_by_lua_block)
> 
> [lua-resty-core/balancer.md](https://github.com/openresty/lua-resty-core/blob/master/lib/ngx/balancer.md)



**设置指令**

| 命令                    | 来源        | 上下文      | 阶段      | 备注                   |
| --------------------- | --------- | -------- | ------- | -------------------- |
| balancer_by_lua_block | OpenResty | upstream | content | not support yielding |
| balancer_by_lua_file  | OpenResty | upstream | content | not support yielding |

**相关指令**

| 命令                          | 来源                                                                                          | 说明                               |
| --------------------------- | ------------------------------------------------------------------------------------------- | -------------------------------- |
| `proxy_next_upstream`       | [nginx](http://nginx.org/en/docs/http/ngx_http_proxy_module.html#proxy_next_upstream)       | 指定重新选择上游服务器的错误种类                 |
| `proxy_next_upstream_tries` | [nginx](http://nginx.org/en/docs/http/ngx_http_proxy_module.html#proxy_next_upstream_tries) | 指定重试次数                           |
| `keepalive`（upstream中）      | [nginx](http://nginx.org/en/docs/http/ngx_http_upstream_module.html#keepalive)              | balancer可以使用nginx原生的上游连接池        |
| `server`（upstream中）         | [nginx](http://nginx.org/en/docs/http/ngx_http_upstream_module.html#server)                 | 使用balancer时，该指令用来防止nginx解析配置文件失败 |

## 简介

balancer 阶段工作于 `upstream{}` 块中，其主要功能是允许用户动态的选择上游服务器。The resulting Lua load balancer can work with any existing Nginx upstream modules like [ngx_proxy](https://nginx.org/en/docs/http/ngx_http_proxy_module.html) and [ngx_fastcgi](https://nginx.org/en/docs/http/ngx_http_fastcgi_module.html).看下面的例子:

```nginx
upstream foo {
     server 0.0.0.0;
     balancer_by_lua_block {
         -- 在这里可以动态的设置服务器地址和端口
     }
     keepalive 16;
 }

 server {
     location / {
         proxy_pass http://foo;
     }
 }
```



balancer 可以使用标准nginx的上游连接池机制(upstream connection pool mechanism), 例如,  [keepalive](https://nginx.org/en/docs/http/ngx_http_upstream_module.html#keepalive) 指令，但是要保证在同一个`upstream{}`块中， [keepalive](https://nginx.org/en/docs/http/ngx_http_upstream_module.html#keepalive) 指令要出现在 `balancer_by_lua_*` 指令后面。

balancer 可以完全无视配置文件中 `upstream {}`块中定义的服务器列表，可以使用  [ngx.balancer](https://github.com/openresty/lua-resty-core/blob/master/lib/ngx/balancer.md) 模块动态设置服务器。

在一次请求中，这个阶段可能被多次调用，原因是请求上游出现了 [proxy_next_upstream](https://nginx.org/en/docs/http/ngx_http_proxy_module.html#proxy_next_upstream) 指令中指定的错误，nginx 需要重试，重试的次数不能超过 [proxy_next_upstream_tries](http://nginx.org/en/docs/http/ngx_http_proxy_module.html#proxy_next_upstream_tries) 中定义的次数。

这个阶段不支持 yield 操作，所以可能导致 yield 的Lua API(像cosockets 和 "light threads") 都不允许在这个阶段使用。

## API

下面是balancer 阶段专用API 的列表

| API 函数           | 上下文              | 说明                  |
| ---------------- | ---------------- | ------------------- |
| set_current_peer | balancer_by_lua* | 设置当前请求的上游服务器IP和端口   |
| set_more_tries   | balancer_by_lua* |                     |
| get_last_failure | balancer_by_lua* | 获取当前请求上一次向上游发送失败的原因 |
| set_timeouts     | balancer_by_lua* | 设置上游连接的超时时间         |
| recreate_request | balancer_by_lua* | 重建要发送到上游的请求         |

例子:





### set_current_peer

设置当前请求的上游服务器IP和端口。

#### 语法

`ok, err = balancer.set_current_peer(host, port)`

| 数据名称 | 数据位置   | 数据类型           | 说明             |
| ---- | ------ | -------------- | -------------- |
| host | 参数[IN] | Lua String     | 必须是IP地址,不可以是域名 |
| port | 参数[IN] | Lua Number     | 端口号            |
| ok   | 返回值    | `true` / `nil` | 函数执行是否成功       |
| err  | 返回值    | Lua String     | 错误描述信息         |

Sets the peer address (host and port) for the current backend query (which may be a retry).

Domain names in `host` do not make sense. You need to use OpenResty libraries like [lua-resty-dns](https://github.com/openresty/lua-resty-dns) to obtain IP address(es) from all the domain names before entering the `balancer_by_lua*` handler (for example, you can perform DNS lookups in an earlier phase like [access_by_lua*](https://github.com/openresty/lua-nginx-module#access_by_lua) and pass the results to the `balancer_by_lua*` handler via [ngx.ctx](https://github.com/openresty/lua-nginx-module#ngxctx).



### set_more_tries



`ok, err = balancer.set_more_tries(count)`

| 数据名称  | 数据位置   | 数据类型           | 说明       |
| ----- | ------ | -------------- | -------- |
| count | 参数[IN] | Lua Number     |          |
| ok    | 返回值    | `true` / `nil` | 函数执行是否成功 |
| err   | 返回值    | Lua String     | 错误描述信息   |

Sets the tries performed when the current attempt (which may be a retry) fails (as determined by directives like [proxy_next_upstream](http://nginx.org/en/docs/http/ngx_http_proxy_module.html#proxy_next_upstream), depending on what particular nginx uptream module you are currently using). Note that the current attempt is *excluded* in the `count` number set here.

Please note that, the total number of tries in a single downstream request cannot exceed the hard limit configured by directives like [proxy_next_upstream_tries](http://nginx.org/en/docs/http/ngx_http_proxy_module.html#proxy_next_upstream_tries), depending on what concrete nginx upstream module you are using. When exceeding this limit, the `count` value will get reduced to meet the limit and the second return value will be the string `"reduced tries due to limit"`, which is a warning, while the first return value is still a `true` value.





### get_last_failure



### set_timeouts

#### 语法

`ok, err = balancer.set_timeouts(connect_timeout, send_timeout, read_timeout)`

| 数据名称            | 数据位置   | 数据类型           | 说明                                                      |
| --------------- | ------ | -------------- | ------------------------------------------------------- |
| connect_timeout | 参数[IN] | Lua Number     | proxy_connect_timeout, 单位为秒(必须大于0)； 设置为nil,表示使用配置文件中的设定 |
| send_timeout    | 参数[IN] | Lua Number     | proxy_send_timeout, 单位为秒(必须大于0)； 设置为nil,表示使用配置文件中的设定    |
| read_timeout    | 参数[IN] | Lua Number     | proxy_read_timeout, 单位为秒(必须大于0)； 设置为nil,表示使用配置文件中的设定    |
| ok              | 返回值    | `true` / `nil` | 函数执行是否成功                                                |
| err             | 返回值    | Lua String     | 错误描述信息                                                  |

该函数的作用域仅限于当前连接(包含后续的重试)，不影响其他连接。

超时参数的单位为秒，但是你可以使用浮点数例如0.001表示1毫秒。

超时参数设为 `nil`，表示继承配置文件中的设定。

在`ngx_stream_proxy_module`模块中，参数 `send_timeout` 和`read_timeout` 是由相同的指令 [`proxy_timeout`](https://nginx.org/en/docs/stream/ngx_stream_proxy_module.html#proxy_timeout)  配置的。为了保持API的兼容性，会使用 `max(send_timeout, read_timeout)` 设置 `proxy_timeout`.
