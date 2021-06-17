# CSR 证书签名请求


### 生成CSR文件

```bash
openssl genrsa -out mydomain.com.key 2048
openssl req -new -key mydomain.com.key -out mydomain.com.csr
```

生成的 CSR 文件默认为 PEM 格式, 可以使用下面命令查看

```bash
openssl asn1parse -i -in mydomain.com.csr
```
 
### CSR格式转换

将 CSR 文件从 pem 转换为 der 格式

```bash
openssl req -inform pem -outform der -in mydomain.com.csr -out mydomain.com.der.csr
```


可以使用下面命令查看 

```bash
openssl asn1parse -inform der  -i -in mydomain.com.der.csr
```


### References

https://stackoverflow.com/questions/48049491/create-csr-in-java-with-der-format
https://en.wikipedia.org/wiki/Certificate_signing_request