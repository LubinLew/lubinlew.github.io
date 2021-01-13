# ngx.req.get_uri_args

返回一个包含当前请求URL请求参数的 Lua table.

<mark>键和值已经按照URL编码规则进行了解码</mark>.

## 语法

  `args, err = ngx.req.get_uri_args(max_args?)`

| 值        | 位置     | 类型         | 说明                           |
| -------- | ------ | ---------- | ---------------------------- |
| args     | 返回值    | Lua Table  | URL参数键值table, 可能为空,不可能为`nil` |
| err      | 返回值    | Lua String | `"truncated"` or `nil`       |
| max_args | 参数(可选) | Lua Number | [详细见下面说明](#参数说明)             |

## 上下文

set_by_lua*

rewrite_by_lua*

access_by_lua*

content_by_lua*

header_filter_by_lua*

body_filter_by_lua*

log_by_lua*

balancer_by_lua*

## 例子

```lua
location = /test {
     content_by_lua_block {
         local args, err = ngx.req.get_uri_args()

         if err == "truncated" then
             -- one can choose to ignore or reject the current request here
         end

         for key, val in pairs(args) do
             if type(val) == "table" then
                 ngx.say(key, ": ", table.concat(val, ", "))
             else
                 ngx.say(key, ": ", val)
             end
         end
     }
 }
```

Then `GET /test?foo=bar&bar=baz&bar=blah` will yield the response body

```shell
 foo: bar bar: baz, blah
```

## 注意事项

### 如果一个请求没有URL参数, 那么返回一个空的 table

`GET /test`  请求时, `ngx.req.get_uri_args()` 返回一个空的 table 。



### 如果一个参数出现多次, 那么这个参数对应的值会被按顺序放到一个 Lua table 中

. In the settings above, `GET /test?a%20b=1%61+2` will yield:

```shell
 a b: 1a 2
```

### 参数键后没有 `=value` 部分, 值被认为是布尔类型

That is, they will take Lua boolean values `true`.

`GET /test?foo&bar` will yield:

```shell
 foo: true
 bar: true
```

### 参数键后没有`value` 部分,值被认为是空字符串

`GET /test?foo=&bar=` will give something like

```shell
 foo: bar:
```

### 没有参数键只有`=value`部分的情况, 会被丢弃

`GET /test?=hello&=world` will yield an empty output for instance.



Updating query arguments via the Nginx variable `$args` (or `ngx.var.args` in Lua) at runtime is also supported:

```lua
 ngx.var.args = "a=3&b=42"
 local args, err = ngx.req.get_uri_args()
```

Here the `args` table will always look like

```lua
 {a = 3, b = 42}
```

regardless of the actual request query string.



## 参数说明

默认情况下，只会解析前100个请求参数，之后的参数被丢弃，这样做的原因是防止DoS攻击。但是有的攻击者利用这个特性，在100个之后的参数注入 payload 来绕过基于 OpenResty 开发的WAF，所以这个在实际使用上需要特别注意。

从 `v0.10.13` 版本开始,  当超过限制的数量后， 第二个返回值 `err` 会返回字符串 `"truncated"`。 可以通过指定参数 `max_args` 的方式覆盖默认的100限制。

```lua
 local args, err = ngx.req.get_uri_args(10) if err == "truncated" then
     -- one can choose to ignore or reject the current request here
 end
```

参数 `max_args` 设置为0, 表示取消数量限制,处理所有请求参数, 这种做法是不推荐的。

```lua
 local args, err = ngx.req.get_uri_args(0)
```
