# 自签发CA证书

### 自签发一套证书秘钥作为CA

```bash
openssl genrsa -out CA.key 2048

cat > ./CA.cnf <<EOF
[req]
prompt = no
default_md = sha256
default_bits = 2048
req_extensions = req_ext
distinguished_name = dn

[dn]
CN = I am the CA
emailAddress = ca@ca.com
O = Organization Name
OU = OU
ST = Beijing
L = Beijing
C = CN
[req_ext]
basicConstraints = critical,CA:TRUE
subjectKeyIdentifier = hash
authorityKeyIdentifier = keyid:always,issuer:always
EOF

openssl req -new -x509 -days 36500 -config CA.cnf -extensions req_ext -key CA.key -out CA.crt

# 生成 CA.key(私钥)  和 CA.crt(证书)
```

> [x509v3扩展](https://www.openssl.org/docs/manmaster/man5/x509v3_config.html)

### 生成网站证书签名请求

```bash
openssl genrsa -out site.key 2048

cat > ./site.cnf <<EOF
[req]
prompt = no
distinguished_name = dn

[dn]
CN =www.test.com
emailAddress = user@user.com
O = Organization Name
OU = Organizational Unit Name
ST = Beijing
L = Beijing
C = CN
EOF

cat > ./site.ext << EOF
subjectAltName = DNS:*.test.com, DNS:test.com, IP:1.1.1.1, IP:2.2.2.2
EOF

openssl req -new -config site.cnf -key site.key -out site.csr

# 生成 site.key(私钥) 和 site.csr(证书签名请求)
```

### 使用CA签发证书

```bash
openssl x509 -req -days 365 -in site.csr -CA CA.crt -CAkey CA.key -out site.crt -CAcreateserial -extfile site.ext

# 生成 site.crt(CA签名的证书)
```

## Reference

https://gist.github.com/fntlnz/cf14feb5a46b2eda428e000157447309