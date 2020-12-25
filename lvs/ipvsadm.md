# IPVSADM

> [ipvsadm工具介绍](https://blog.csdn.net/developerof/article/details/90214977)

## 常用语法

| 用例                                                                                                                  |     |     |
| ------------------------------------------------------------------------------------------------------------------- | --- | --- |
| ipvsadm -A\|E virtual-service [-s scheduler] [-p [timeout]] [-M netmask] [--pe persistence_engine] [-b sched-flags] |     |     |
| ipvsadm -D virtual-service                                                                                          |     |     |
| ipvsadm -C                                                                                                          |     |     |
| ipvsadm -R                                                                                                          |     |     |
| ipvsadm -S [-n]                                                                                                     |     |     |
| ipvsadm -a\|e virtual-service -r server-address [options]                                                           |     |     |
| ipvsadm -d virtual-service -r server-address                                                                        |     |     |
| ipvsadm -L\|l [virtual-service] [options]                                                                           |     |     |
| ipvsadm -Z [virtual-service]                                                                                        |     |     |
| ipvsadm --set tcp tcpfin udp                                                                                        |     |     |
| ipvsadm --set tcp tcpfin udp                                                                                        |     |     |
| ipvsadm --start-daemon {master\|backup} [daemon-options]                                                            |     |     |
| ipvsadm --stop-daemon {master\|backup}                                                                              |     |     |
| ipvsadm -h                                                                                                          |     |     |

## 命令

| 短选项               | 长选项           | 参数 | 说明                                       | 例子                                                                                            |
| -------------------- | ---------------- | ---  | ------------------------------------------ | ----------------------------------------------------------------------------------------------- |
| -A                   | --add-service    | 无   | 添加一个虚拟服务(with options)             | `ipvsadm -A -t 192.168.1.100:80 -s rr`<br>添加一个TCP虚拟服务192.168.1.100:80，使用轮询算法     |
| -E                   | --edit-service   | 有   | 编辑一个虚拟服务(with options)             | `ipvsadm -E -t 192.168.1.100:80 -s wrr`<br>修改虚拟服务的算法为加权轮询                         |
| -D                   | --delete-service | 有   | 删除虚拟服务                               | `ipvsadm -D -t 192.168.1.100:80`<br>删除虚拟服务                                                |
| -C                   | --clear          | 无   | clear the whole table                      |                                                                                                 |
| -R                   | --restore        | 无   | restore rules from stdin                   |                                                                                                 |
| -S                   | --save           | 无   | save rules to stdout                       |                                                                                                 |
| -a                   | --add-server     |      | add real server with options               | `ipvsadm -a -t 192.168.1.100:80 -r 192.168.1.123 -g -w 2`<br>添加一个真实服务器192.168.1.123，使用DR模式，权重2 |
| -e                   | --edit-server    |      | edit real server with options              | `ipvsadm -a -t 192.168.1.100:80 -r 192.168.1.123 -g -w 5`<br>修改真实服务器的权重               |
| -d                   | --delete-server  |      | delete real server                         | `ipvsadm -d -t 192.168.1.100:80 -r 192.168.1.123`<br>删除真实服务器                             |
| -L  or   -l          | --list           | 无   | list the table                             |                                                                                                 |
| -Z                   | --zero           |      | zero counters in a service or all services |                                                                                                 |
| --set tcp tcpfin udp |                  |      | set connection timeout values              |                                                                                                 |
|                      | --start-daemon   |      | start connection sync daemon               |                                                                                                 |
|                      | --stop-daemon    |      | stop connection sync daemon                |                                                                                                 |
| -h                   | --help           | 无   | display this help message                  |                                                                                                 |

## 

## virtual-service

| 短选项 | 长选项              | 参数  | 说明  | 例子                                                                   |
| --- | ---------------- | --- | --- | -------------------------------------------------------------------- |
| -t  | --tcp-service    | 有   |     | `-t  service-address`   service-address is host[:port]               |
| -u  | --udp-service    | 有   |     | `-u  service-address   service-address` is host[:port]               |
|     | --sctp-service   | 有   |     | `--sctp-service    service-address`   service-address is host[:port] |
| -f  | --fwmark-service | 有   |     | `-f fwmark`   fwmark is an integer greater than zero                 |

## 

## 选项

| 短选项               | 长选项               | 参数  | 说明                                                                                           | 例子  |
| ----------------- | ----------------- | --- | -------------------------------------------------------------------------------------------- | --- |
| -6                | --ipv6            | 无   | fwmark entry uses IPv6                                                                       |     |
| -s scheduler      | --scheduler       | 有   | 配置负载均衡算法, 算法列表如下, `rr\|wrr\|lc\|wlc\|lblc\|lblcr\|dh\|sh\|sed\|nq\|fo\|ovf\|mh`, 默认算法是 `wlc` |     |
|                   | --pe  engine      | 有   | alternate persistence engine may be sip, not set by default.                                 |     |
| -p [timeout]      | --persistent      | 可选  | persistent service                                                                           |     |
| -M netmask        | --netmask         | 有   | persistent granularity mask                                                                  |     |
| -r server-address | --real-server     | 有   | server-address is host (and port)                                                            |     |
| -g                | --gatewaying      |     | gatewaying (direct routing) (default)                                                        |     |
| -i                | --ipip            |     | ipip encapsulation (tunneling)                                                               |     |
| -m                | --masquerading    |     | masquerading (NAT)                                                                           |     |
| type              | --tun-type        |     | one of `ipip\|gue\|gre`, the default tunnel type is ipip.                                    |     |
| port              | --tun-port        |     | tunnel destination port                                                                      |     |
|                   | --tun-nocsum      |     | tunnel encapsulation without checksum                                                        |     |
|                   | --tun-csum        |     | tunnel encapsulation with checksum                                                           |     |
|                   | --tun-remcsum     |     | tunnel encapsulation with remote checksum                                                    |     |
| -w weight         | --weight          |     | capacity of real server                                                                      |     |
| -x uthreshold     | --u-threshold     |     | upper threshold of connections                                                               |     |
| -y lthreshold     | --l-threshold     |     | lower threshold of connections                                                               |     |
| -c                | --connection      |     | output of current IPVS connections                                                           |     |
|                   | --timeout         |     | output of timeout (tcp tcpfin udp)                                                           |     |
|                   | --daemon          |     | output of daemon information                                                                 |     |
|                   | --stats           |     | output of statistics information                                                             |     |
|                   | --rate            |     | output of rate information                                                                   |     |
|                   | --exact           |     | expand numbers (display exact values)                                                        |     |
|                   | --thresholds      |     | output of thresholds information                                                             |     |
|                   | --persistent-conn |     | output of persistent connection info                                                         |     |
|                   | --tun-info        |     | output of tunnel information                                                                 |     |
|                   | --nosort          |     | disable sorting output of service/server entries                                             |     |
|                   | --sort            |     | does nothing, for backwards compatibility                                                    |     |
| -o                | --ops             |     | one-packet scheduling                                                                        |     |
| -n                | --numeric         |     | 数字化显示IP地址和端口(不做解析)                                                                           |     |
| -b                | --sched-flags     | 有   | 调度 scheduler flags (comma-separated)                                                         |     |

## 

## 守护进程选项

| 选项                | 参数        | 说明                                       | 例子  |
| ----------------- | --------- | ---------------------------------------- | --- |
| --syncid          | sid       | syncid for connection sync (default=255) |     |
| --sync-maxlen     | length    | Max sync message length (default=1472)   |     |
| --mcast-interface | interface | multicast interface for connection sync  |     |
| --mcast-group     | address   | IPv4/IPv6 group (default=224.0.0.81)     |     |
| --mcast-port      | port      | UDP port (default=8848)                  |     |
| --mcast-ttl       | ttl       | Multicast TTL (default=1)                |     |
