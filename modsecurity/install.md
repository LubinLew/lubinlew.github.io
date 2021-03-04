# 部署

编译环境如下:

| 环境                                                                   | 版本                                                                            | 备注      |
| -------------------------------------------------------------------- | ----------------------------------------------------------------------------- | ------- |
| [CentOS7](http://isoredirect.centos.org/centos/7/isos/x86_64/)       | 7.9.2009                                                                      | -       |
| [libmaxminddb](https://github.com/maxmind/libmaxminddb)              | [1.5.2](https://github.com/maxmind/libmaxminddb/releases/tag/1.5.2)           | release |
| [ModSecurity](https://github.com/SpiderLabs/ModSecurity)             | [v3.0.4](https://github.com/SpiderLabs/ModSecurity/releases/tag/v3.0.4)       | release |
| [nginx](http://nginx.org/en/download.html)                           | [1.18.0](http://nginx.org/download/nginx-1.18.0.tar.gz)                       | 稳定版     |
| [Modsecurity-nginx](https://github.com/SpiderLabs/ModSecurity-nginx) | [v1.0.1](https://github.com/SpiderLabs/ModSecurity-nginx/releases/tag/v1.0.1) | release |
| [CRS](https://github.com/coreruleset/coreruleset)                    | [v3.3.0](https://github.com/coreruleset/coreruleset/releases/tag/v3.3.0)      | release |

## 搭建编译环境

```bash
## install tools
yum install -y wget git automake autoconf libtool gcc-c++

## install compile dependencies
yum install -y libxml2-devel pcre-devel libcurl-devel ncurses-devel readline-devel \
               yajl-devel lmdb-devel ssdeep-devel libtermcap-devel openssl-devel
```

## 编译安装

```bash
#!/usr/bin/bash

yum install -y wget git automake autoconf libtool gcc-c++

yum install -y libxml2-devel pcre-devel libcurl-devel ncurses-devel readline-devel \
               yajl-devel lmdb-devel ssdeep-devel libtermcap-devel openssl-devel

rm -rf build
mkdir build
tar xf libmaxminddb-1.5.2.tar.gz -C build
tar xf modsecurity-v3.0.4.tar.gz -C build
tar xf nginx-1.18.0.tar.gz       -C build
tar xf modsecurity-nginx-v1.0.1.tar.gz -C build
tar xf coreruleset-3.3.0.tar.gz  -C build

cd build
ROOT=${PWD}

cd libmaxminddb-1.5.2
mkdir install
MAXMINDDIR=${ROOT}/libmaxminddb-1.5.2/install
./configure --prefix=${MAXMINDDIR} --enable-shared=no CFLAGS="-fPIC"
make
make install

cd ${ROOT}/modsecurity-v3.0.4
./configure --with-maxmind=${MAXMINDDIR}
make
make install

cd ${ROOT}/nginx-1.18.0
./configure \
  --add-dynamic-module=../modsecurity-nginx-v1.0.1 \
  --with-compat             \
  --with-file-aio           \
  --with-threads            \
  --with-http_ssl_module    \
  --with-http_realip_module \
  --with-cc-opt='-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=generic -fPIC' \
  --with-ld-opt='-Wl,-z,relro -Wl,-z,now -pie'
make
make install

mkdir /usr/local/nginx/conf/modsecurity
cd ${ROOT}/modsecurity-v3.0.4
\cp modsecurity.conf-recommended /usr/local/nginx/conf/modsecurity/modsecurity.conf
\cp unicode.mapping /usr/local/nginx/conf/modsecurity/

cd ${ROOT}
\cp -r coreruleset-3.3.0 /usr/local/nginx/conf/modsecurity
cd /usr/local/nginx/conf/modsecurity/coreruleset-3.3.0
\cp cp crs-setup.conf.example crs-setup.conf
echo "Include /usr/local/nginx/conf/modsecurity/coreruleset-3.3.0/crs-setup.conf" >> /usr/local/nginx/conf/modsecurity/modsecurity.conf
echo "Include /usr/local/nginx/conf/modsecurity/coreruleset-3.3.0/rules/*.conf" >>/usr/local/nginx/conf/modsecurity/modsecurity.conf


cat > /usr/local/nginx/conf/nginx.conf <<EOF
user  nobody;
worker_processes auto;
worker_cpu_affinity auto;
worker_rlimit_nofile    65535;
worker_shutdown_timeout 20s;

error_log  logs/error.log;

load_module modules/ngx_http_modsecurity_module.so;

events {
    use epoll;
    worker_connections  65535;
}

http {
    include       mime.types;
    default_type  application/octet-stream;
    server_tokens off;
    client_max_body_size 10m;
    underscores_in_headers on;
    ignore_invalid_headers off;

    server {
        listen 80 reuseport;
        modsecurity on;
        modsecurity_rules_file /usr/local/nginx/conf/modsecurity/modsecurity.conf;
        location / {
            root html;
            index index.html;
        }
    }
}
EOF

ln -s /usr/local/nginx/sbin/nginx /sbin/nginx

nginx -t

```
