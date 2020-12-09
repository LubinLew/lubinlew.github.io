# nginx 如何代理 websocket

> [Nginx WebSocket Proxying](http://nginx.org/en/docs/http/websocket.html)

nginx 在做反向代理的时候, 默认使用 HTTP 1.0 协议，并且不会透传客户端请求中的 `Upgrade` 字段。



```nginx
http {
    map $http_upgrade $connection_upgrade {
        default upgrade;
        ''      close;
    }

    server {
        ...

        location /chat/ {
            proxy_pass http://backend;
            proxy_http_version 1.1;
            proxy_set_header Upgrade    $http_upgrade;
            proxy_set_header Connection $connection_upgrade;
        }
    }
```
