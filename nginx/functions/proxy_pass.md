# proxy_pass 配置

> http://nginx.org/en/docs/http/ngx_http_proxy_module.html#proxy_pass

```nginx
Syntax : proxy_pass URL;
Default: —
Context: location, if in location, limit_except
```

proxy_pass 参数组成:

- 协议可以指定`http`或`https`
- 地址可以指定为`域名`, `IP地址`, `Unix Socket`
- 端口是 `可选的`
- URI是 `可选的`

```nginx
proxy_pass http://localhost:8000/uri/;
```

或者 UNIX 套接字路径：

```nginx
proxy_pass http://unix:/tmp/backend.socket:/uri/;
```

如果一个域名解析为多个地址，则所有这些地址都将以循环方式使用。此外，可以将地址指定为服务器组。
参数值可以包含变量。 在这种情况下，如果将地址指定为域名，则在所描述的服务器组中搜索该名称，
如果未找到，则使用解析器确定该名称。

```nginx
upstream dynamic {
    server 192.0.2.1;
    server 192.0.2.2:8080;
    server some.domain.com;
}

server {
    location / {
        proxy_pass http://dynamic;
    }
}
```

请求 URI 被传递给服务器，如下所示：

如果 `proxy_pass` 指令是用一个 URI 指定的，那么当一个请求被传递到服务器时，
与该位置匹配的规范化请求 URI 的部分将被指令中指定的 URI 替换：

```nginx
location /name/ {
    proxy_pass http://127.0.0.1/remote/;
}

# client req: http://10.1.1.10/name/list.html
# server req: http://127.0.0.1/remote/list.html
```

如果 `proxy_pass` 没有指定 URI，
则请求 URI 以与处理原始请求时客户端发送的相同格式传递给服务器，
或者在处理更改的 URI 时传递完整的规范化请求 URI：

```nginx
location /some/path/ {
    proxy_pass http://127.0.0.1;
}

# client req: http://10.1.1.10/some/path/list.html
# server req: http://127.0.0.1/some/path/list.html
```

在某些情况下，无法确定要替换的请求 URI 部分：

当使用正则表达式指定位置时，以及在命名位置内。在这些情况下，应指定不带 URI 的 proxy_pass。

当使用 rewrite 指令在代理位置内更改 URI 时，将使用相同的配置来处理请求（中断）：

```nginx
location /name/ {
    rewrite    /name/([^/]+) /users?name=$1 break;
    proxy_pass http://127.0.0.1;
}

# client req: http://10.1.1.10/name/lubin
# server req: http://127.0.0.1/users?name=lubin
```

在这种情况下，指令中指定的 URI 将被忽略，并将完整更改的请求 URI 传递给服务器。

在 proxy_pass 中使用变量时：

```nginx
location /name/ {
    proxy_pass http://127.0.0.1$request_uri;
}

# client req: http://10.1.1.10/name/lubin
# server req: http://127.0.0.1/name/lubin
```

在这种情况下，如果在指令中指定了 URI，它将按原样传递给服务器，替换原始请求 URI。

## 测试


测试 配置文件如下:

```nginx
server {
    listen       8080;
    server_name  _;

    location /name1/ {
        proxy_pass http://127.0.0.1/remote/;
    }    

    location /some/name2/ {
        proxy_pass http://127.0.0.1;
    }

    location /name3/ {
        rewrite    /name3/([^/]+) /users?name=$1 break;
        proxy_pass http://127.0.0.1;
    }

    location /name4/ {
       proxy_pass http://127.0.0.1$request_uri;
    }
}


server {
    listen       80;
    server_name   _;

    location / {
       add_header Content-Type 'text/html; charset=utf-8';
       return 200 $host$request_uri\n;
    }
}

```


测试 结果如下:


```bash
$ curl localhost:8080/name1/
127.0.0.1/remote/

$ curl localhost:8080/some/name2/
127.0.0.1/some/name2/

$ curl localhost:8080/name3/tom
127.0.0.1/users?name=tom

$ curl localhost:8080/name4/tom
127.0.0.1/name4/tom
```






