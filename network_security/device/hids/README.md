# HIDS

HIDS全称是Host-based Intrusion Detection System，即基于主机的入侵检测系统。
作为计算机系统的监视器和分析器，它并不作用于外部接口，而是专注于系统内部，监视系统全部或部分的动态的行为以及整个计算机系统的状态。

## Wazuh

[Wazuh](https://github.com/wazuh/wazuh) 是一个安全检测，可见性和合规性开源项目。它诞生于OSSEC HIDS的分支，后来与Elastic Stack和OpenSCAP集成，演变成更全面的解决方案。以下是这些工具及其功能的简要说明：

### OSSEC-HIDS

[ossec-hids](https://github.com/ossec/ossec-hids) 是一种基于主机的入侵检测系统（HIDS），用于安全检测，可见性和合规性监控。它基于多平台代理，将系统数据（例如，日志消息，文件哈希值和检测到的异常）转发给中央管理器，在中央管理器中进行进一步分析和处理，从而产生安全警报。代理通过安全且经过身份验证的渠道将事件数据传送到中央管理器进行分析。

此外，OSSEC HIDS还提供集中式系统日志服务器和无代理配置监控系统，可提供对无代理设备（如防火墙，交换机，路由器，接入点，网络设备等）上的事件和更改的安全洞察。

### Elkeid

字节跳动开源的HIDS, https://github.com/bytedance/Elkeid

### OpenSCAP

[OpenSCAP](https://github.com/OpenSCAP/openscap) 是一种OVAL（开放漏洞评估语言）和XCCDF（可扩展配置清单描述格式）解释器，用于检查系统配置和检测易受攻击的应用程序。

它是一个众所周知的工具，旨在使用行业标准的企业环境安全基准来检查系统的安全合规性和强化。

### Elkeid

[Elkeid](https://github.com/bytedance/Elkeid) 是一个字节跳动开源的云原生的主机的入侵检测解决方案。但是未开源 规则引擎和检测功能。

## Tripwire

[Tripwire](https://github.com/Tripwire/tripwire-open-source) 是一个数据完整性工具，用于监视和警告文件和目录更改。

## osquery

[osquery](https://github.com/osquery/osquery)

## CIS-CAT

Center for Internet Security Configuration Assessment Tool

https://documentation.its.umich.edu/node/941

https://www.cisecurity.org/cybersecurity-tools/cis-cat-pro/