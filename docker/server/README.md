# 搭建私有docker registry

> https://docs.docker.com/registry/deploying/

## 配置 registry 服务

### 无访问密码的 http 服务

```bash
docker run -d \
 -p 5000:5000 \
 -v /data/docker-registry/data:/var/lib/registry \
 --restart=always \
 --name docker-registry \
registry:2
```

### 有访问密码的 http 服务

```bash
# 安装 http 基本认证 密码工具
yum install httpd-tools    # apt-get install apache2-utils

# 配置用户名 和 密码 保存到文件(如果密码中有特殊字符,去掉 -b 选项 和 password, 交互式输入即可)
htpasswd -Bbc /data/docker-registry/auth/htpasswd  username password

# 启动服务
docker run -d \
 -p 5000:5000 \
 -v /data/docker-registry/data:/var/lib/registry \
 -v /data/docker-registry/auth:/auth \
 -e "REGISTRY_AUTH=htpasswd" \
 -e "REGISTRY_AUTH_HTPASSWD_REALM=Registry Realm" \
 -e REGISTRY_AUTH_HTPASSWD_PATH=/auth/htpasswd \
 --restart=always \
 --name docker-registry \
registry:2
```

### 无访问密码的 https 服务

配置 https 需要证书和私钥, 可以自己生成一套。
客户端访问提示证书不可信需要[配置一下即可解决](https://docs.docker.com/registry/insecure/)。

```bash

docker run -d \
 -p 5000:5000 \
 -v /data/docker-registry/data:/var/lib/registry \
 -v /data/docker-registry/certs:/certs \
 -e REGISTRY_HTTP_TLS_CERTIFICATE=/certs/domain.crt \
 -e REGISTRY_HTTP_TLS_KEY=/certs/domain.key \
 --restart=always \
 --name docker-registry \
 registry:2

```


## 配置 nginx 代理 registry

这里配置的https代理, registry 本身还是http的。

```nginx
server {
    listen       443 ssl;
    server_name  registry.sec.navinfo.com;
    charset utf-8;

    client_max_body_size 0; # 不限制上传文件大小
    proxy_buffering off;
    proxy_request_buffering off;

    ssl_certificate "/path/domain.crt";
    ssl_certificate_key "/path/domain.key";
    ssl_session_cache shared:SSL:10m;
    ssl_session_tickets off; 
    ssl_session_timeout  1d;
    ssl_protocols TLSv1.2;
    ssl_ciphers ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384;
    ssl_prefer_server_ciphers off;


    location / {
    proxy_pass http://127.0.0.1:5000/;
    proxy_set_header Host $host:$server_port;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
    proxy_set_header X-Original-URI $request_uri;
    proxy_set_header Docker-Distribution-Api-Version registry/2.0; # 设置HTTP API版本
    }
}
```