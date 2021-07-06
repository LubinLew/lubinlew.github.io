# 使用

> [Usage &#8212; python-gvm 21.1.4.dev1 documentation](https://python-gvm.readthedocs.io/en/latest/usage.html)

API通过GVM协议与远程服务器进行通信， 当前，有两种协议（即GMP和OSP）和三种连接类型（即TLS，SSH和Unix域套接字）可用。



## 使用GMP协议

GMP( Greenbone Management Protocol)是 [gvmd](https://github.com/greenbone/gvmd) 实现的协议。 [Greenbone Security Assistant](https://github.com/greenbone/gsa) 守护程序也使用它来从gvmd请求所有信息。

### Making a Simple Request

要创建请求，必须选择连接类型。 该决定取决于远程gvmd服务器的位置和配置。 对于本地通信，Unix域套接字最适合。 最简单的命令是请求远程gvmd使用的GMP版本。

#### Step by Step

```python
from gvm.connections import UnixSocketConnection
from gvm.protocols.gmp import Gmp

# path to unix socket
path = '/var/run/gvmd.sock'
connection = UnixSocketConnection(path=path)

# using the with statement to automatically connect and disconnect to gvmd
with Gmp(connection=connection) as gmp:
    # get the response message returned as a utf-8 encoded string
    response = gmp.get_version()

    # print the response message
    print(response)
```

## Using OSP

开放扫描程序协议（OSP）是由基本类为扫描程序包装程序开放扫描程序协议守护程序ospd实现的通信协议。 OSP为不同的安全扫描程序创建了一个统一的界面，并使它们的控制流和扫描结果在Greenbone Vulnerability Manager守护程序gvmd下始终可用。 在许多方面，OSP类似于Greenbone管理协议（GMP）：基于XML，无状态且具有永久连接。
