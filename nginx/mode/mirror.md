# HTTP请求镜像

> [Module ngx_http_mirror_module](http://nginx.org/en/docs/http/ngx_http_mirror_module.html)

nginx 从 v1.13.4 版本开始支持HTTP请求镜像。实现原理是：当nginx接收到一个HTTP请求后，通过子请求将这个请求发送到指定的服务器，但是会忽略服务器的响应。

注意事项：

> 内部rewrite 会导致一个请求发送多个镜像请求



## 配置例子

```nginx
location / {
    access_log /tmp/access.log;
    root   /usr/share/nginx/html/;
    index  index.html index.htm;

   mirror /mirror;
   mirror_request_body off;
}

location = /mirror {
   internal;
   proxy_pass http://backend$request_uri;
   proxy_set_header Host $host;
   proxy_set_header Content-Length "";
   proxy_set_header X-Real-IP $remote_addr;
   proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
}
```
