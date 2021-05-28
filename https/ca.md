# 自签发CA证书

## 自签发一套证书秘钥作为CA

```bash
openssl genrsa -out CA.key 2048

cat > ./CA.cnf <<EOF
[req]
prompt = no
distinguished_name = distinguished_name

[distinguished_name]
CN = I am the CA
emailAddress = ca@ca.com
O = Organization Name
OU = Organizational Unit Name
ST = State or Province Name 
L = Locality Name
C = CN
EOF

openssl req -new -x509 -days 36500 -config CA.cnf -key CA.key -out CA.crt

# 生成 CA.key(私钥)  和 CA.crt(证书)
```

## 生成网站证书签名请求

```bash
openssl genrsa -out fd.key 2048

cat > ./fd.cnf <<EOF
[req]
prompt = no
distinguished_name = distinguished_name

[distinguished_name]
CN =www.test.com
emailAddress = user@user.com
O = Organization Name
OU = Organizational Unit Name
ST = Beijing
L = Beijing
C = CN
EOF

openssl req -new -config fd.cnf -key fd.key -out fd.csr

# 生成 fd.key(私钥) 和 fd.csr(证书签名请求)
```

## 使用CA签发证书

```bash
openssl x509 -req -days 365 -in wpkcert.csr -CA CA.crt -CAkey CA.key -out fd.crt -CAcreateserial

生成 fd.crt(CA签名的证书)
```