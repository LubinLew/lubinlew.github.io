# KMS服务部署及windows系统和Office激活

## 部署 KMS 服务

KMS 服务于 TCP 1688 端口

```bash
# https://github.com/Wind4/vlmcsd-docker

docker run -d --rm -p 1688:1688 vlmcsd:latest
```

## 激活

### win10 LTSC 企业版激活

```bash
slmgr /upk
slmgr /ipk M7XTQ-FN8P6-TTKYV-9D4CC-J462D
slmgr /skms 1.1.1.1
slmgr /ato
```

### Office 2016 增强版激活

```bash
cd "C:\Program Files\Microsoft Office\Office16"
cscript ospp.vbs /inpkey:XQNVK-8JYDB-WJ9W3-YJ8YR-WFG99
cscript ospp.vbs /sethst:1.1.1.1
cscript ospp.vbs /act
cscript ospp.vbs /dstatus
```

### 其他版本产品激活

只需要修改对应的 key 即可, 不同版本间 key 不兼容