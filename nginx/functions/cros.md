# nginx 跨越配置



```nginx
add_header Access-Control-Allow-Origin: *;
add_header Access-Control-Allow-Credentials: true;
add_header Access-Control-Allow-Methods "OPTIONS, GET, POST";
add_header Access-Control-Allow-Headers "Content-Type, Depth, User-Agent, X-File-Size, X-Requested-With, If-Modified-Since, X-File-Name, Cache-Control";


proxy_set_header Referrer-Policy "";
```


