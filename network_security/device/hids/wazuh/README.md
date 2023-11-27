# Wazuh

## 常见问题

### netstat 占用 CPU过高

脚本和程序中都有调用 netstat 命令的情况.

> 代码: rootcheck/check_rc_ports.c
> 命令: `netstat -an | grep "^tcp" |  grep "[^0-9]32818\" > /dev/null 2>&1`

对于服务器和代理服务器来说其上有大量的链接 这可能导致 netstat 命令很慢或者卡住


### wazuh-modulesd 占用CPU过高

执行syscollector收集服务器网络端口信息，
因服务器上跑了大量的web应用，
wazuh原生收集端口及<network>及<ports>本身会占用大量的CPU，且时间会持续几秒。

不再统计<network>及<ports>。