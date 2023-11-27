# 病毒库

| 数据源 | 数据类型 | 备注 |
| --- | --- | --- |
| https://github.com/CYB3RMX/MalwareHashDB | SQLite | 只有 md5 和 名称 两个字段 |
| https://botvrij.eu/data/ | TXT | 数据量非常小,只维护最新数据 |
| https://virusshare.com/hashes | TXT | 只有HASH没有其他任何信息 |
| https://www.clamav.net/ | ClamAV  | 拥有300w+ HASH |

## ClamAV

下载 http://database.clamav.net/main.cvd

### 格式说明 [CVD format](https://opensource.apple.com/source/SpamAssassin/SpamAssassin-124/clamav/docs/html/node45.html)

> CVD (ClamAV Virus Database) is a digitally signed tarball file that contains one or more databases. 
> The header is a 512 bytes long string with colon separated fields.


```bash
# 去除签名信息(删除前512字节)
dd if=main.cvd of=main.tar bs=512 skip=1

# 解压数据库
tar xf main.tar

# HASH数据在 main.hsb 文件中, 冒号分割, 第一部分为 md5  第三部分为 名称
tail -n 10 main.hsb
7873be8fc5e052caa70fdb8f76205892:293376:Win.Trojan.Sality-93158:73
f2131b29d845add67250ca360100fc52:93696:Doc.Dropper.Agent-1451506:73
```