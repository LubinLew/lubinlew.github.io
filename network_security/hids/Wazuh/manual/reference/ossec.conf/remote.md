# Remote

XML section name

```xml
<remote>
</remote>
```

配置Wazuh Manager 监听来自 `Agent` 的事件。

## 选项

- [connection](#connection)
- [port](#port)
- [protocol](#protocol)
- [allowed-ips](#allowed-ips)
- [denied-ips](#denied-ips)
- [local_ip](#local_ip)
- [ipv6](#ipv6)
- [queue_size](#queue_size)
- [rids_closing_time](#rids_closing_time)

### connection

指定接受的传入连接类型：`secure` 或 `syslog`。

| 默认值  | secure         |
| ---- | -------------- |
| 取值范围 | secure, syslog |

### port

指定用于侦听事件的端口。

| 默认值  | 1514 if secure, 514 if syslog |
| ---- | ----------------------------- |
| 取值范围 | 1 ~ 65535                     |


### protocol

Specifies the protocol to use. It is available for secure connections and syslog events.

| **Default value**  | tcp      |
| ------------------ | -------- |
| **Allowed values** | udp, tcp |

>  不能同时使用两种协议。


### allowed-ips

允许向服务器发送系统日志消息的IP地址列表（每行一个）.

| **Default value**  | n/a                       |
| ------------------ | ------------------------- |
| **Allowed values** | Any IP address or network |

> 使用syslog连接类型时，必须列出至少一个IP地址。

### denied-ips

不允许向服务器发送系统日志消息的IP地址列表（每行一个）

| **Default value**  | n/a                       |
| ------------------ | ------------------------- |
| **Allowed values** | Any IP address or network |


### local_ip

用于侦听连接的本地IP地址。

| **Default value**  | All interfaces          |
| ------------------ | ----------------------- |
| **Allowed values** | Any internal ip address |

### ipv6

本地IP地址是否为IPv6

| **Default value**  | no      |
| ------------------ | ------- |
| **Allowed values** | yes, no |

>  目前无法同时设置 local_ip 和 ipv6

### queue_size

设置 Remote 服务进程 `Agent`事件的队列大小。

| **Default value**  | 131072     |
| ------------------ | ---------- |
| **Allowed values** | 1 ~ 262144 |

>  远程队列仅适用于`Agent`事件，而不适用于syslog事件。仅当**连接**类型为`secure` 时此选项才有效。


### rids_closing_time

Sets the time to close the RIDS files for agents that don’t report new events in that time interval.

设置关闭在该时间间隔内未报告新事件的代理的RIDS文件的时间。

| **Default value**  | 5m                                           |
| ------------------ | -------------------------------------------- |
| **Allowed values** | 一个正数，应包含表示时间单位的后缀字符，例如s（秒），m（分钟），h（小时），d（天）。 |

## 配置示例

```xml
<remote>
  <connection>syslog</connection>
  <port>514</port>
  <protocol>tcp</protocol>
  <allowed-ips>192.168.1.0/24</allowed-ips>
  <local_ip>192.168.1.5</local_ip>
</remote>

<remote>
  <connection>secure</connection>
  <port>1514</port>
  <protocol>tcp</protocol>
  <queue_size>16384</queue_size>
  <rids_closing_time>5m</rids_closing_time>
</remote>
```