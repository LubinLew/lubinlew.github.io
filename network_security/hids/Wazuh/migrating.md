# 从 OSSEC 迁移到 Wazuh

## 为什么要迁移

不幸的是，OSSEC用户在过去十年中没有看到许多新功能。该项目已经处于维护模式很长时间了，很少进行开发工作。没有活动的 roadmap，最新版本主要包含偶尔的贡献者报告的错误修复。 

这就是为什么早在2015年，Wazuh团队决定 fork 该项目的原因。成为是一个更加全面，易于使用，可靠和可扩展的解决方案。 Wazuh在开源社区中得到了广泛的采用，迅速成为企业环境中广泛使用的解决方案。 

关于项目活动和 roadmap，您可以在我们的GitHub存储库中找到Wazuh代码。我们相信值得一提的是，在编写本文档时，该项目已提交了8,500多个PR（比OSSEC多3,000多个）。 

以下是我们为OSSEC项目增加的价值的简短摘要，以及将您的安全监控基础结构升级到Wazuh的充分理由：

### 可扩展性和可靠性

- manager 支持集群, 能够水平扩展。 

- 支持Puppet，Chef，Ansible和Docker部署

- 支持 Agent 和 Manager 间TCP通信

-  防洪功能可防止大量突发事件丢失或对网络性能造成负面影响。 

- Agent 和 Manager 间通信使用AES加密（而不是Blowfish）。 

- Manager 进程支持多线程，大大提高了它们的性能。



### 安装和配置管理

- MSI签名的Windows系统软件包，具有自动注册和配置支持。 

- 统一的RPM和Deb Linux软件包。 支持AIX，Solaris，Mac OS X和HP-UX。 

- RESTful API，用于状态监视，查询和配置管理。 

- 能够从Manager 那里升级 Agent。 

- 使用代理组改进了集中式配置管理。



### 入侵检测

- 改进的日志分析引擎，具有本机JSON解码和动态命名字段的功能。 

- 最大消息大小从6KB增加到64KB（能够分析更大的日志消息）。 

- 使用新的日志分析规则和解码器更新了规则集。 

- Suricata的本地规则，使用JSON解码器。 

- 与OwlH项目集成，以实现统一的NIDS管理。 

- 支持IP信誉数据库（例如AlienVault OTX）。 

- 与Linux审核内核子系统和Windows审核策略的本机集成，以捕获FIM事件的who-data。



### 与云提供商集成

- 用于与Amazon AWS本机集成的模块（从Cloudtrail或Cloudwatch提取数据）。

- Amazon AWS的新规则和解码器。 

- 与Microsoft Azure本机集成的模块。 

- Microsoft Azure的新规则和解码器。



### 监管合规(Regulatory compliance)

- 具有PCI DSS和GPG13要求的警报映射。 

- Wazuh Kibana插件提供的Elastic Stack的合规性仪表板。 

- Wazuh应用程序提供的Splunk合规性仪表板。 

- 使用OwlH项目Suricata映射以实现合规性。 

- SHA256散列用于文件完整性监控（除了MD5和SHA1之外）。



### Elastic Stack 集成

- 提供索引和查询数据的功能。 

- 使用GeoIP Elasticsearch模块进行数据充实。 

- 用于可视化数据的Kibana插件（使用Wazuh REStful API集成）。

- Web用户界面已预先配置了扩展，以使其适应您的用例。



### 事件响应

- 用于收集软件和硬件清单数据的模块。 

- 能够通过RESTful API查询软件和硬件。 

- 与Osquery集成的模块，能够按需运行查询。 

- 日志收集器组件的新输出选项的实现。 

- 与Virustotal集成的模块，用于检测恶意文件的存在。



### 漏洞检测和配置评估

动态创建CVE漏洞数据库，从OVAL存储库收集数据。 

与应用程序清单数据进行互相关以检测易受攻击的软件。

通过Internet安全中心扫描程序集成对CIS-CAT的支持。



## 怎样迁移到Wazuh

以下指南介绍了如何将现有OSSEC安装迁移到Wazuh。根据您的OSSEC安装类型（服务器或代理），执行适当的操作：

| Installation type | Upgrade from          | Upgrade to | Guide                                                                                                           |
| ----------------- | --------------------- | ---------- | --------------------------------------------------------------------------------------------------------------- |
| Server            | OSSEC 2.8.3 or higher | Wazuh 3.x  | [Upgrade OSSEC server](https://documentation.wazuh.com/4.0/migrating-from-ossec/ossec-server.html#ossec-server) |
| Agent             | OSSEC 2.8.3 or higher | Wazuh 3.x  | [Upgrade OSSEC agent](https://documentation.wazuh.com/4.0/migrating-from-ossec/ossec-agent.html#ossec-agent)    |

在已经安装 Elastic stack 的情况下，其迁移超出了Wazuh文档的范围。我们建议您访问我们的安装Elastic Stack指南。

> OSSEC agent 与 Wazuh Server 兼容。您甚至可以使用不同版本的Wazuh和OSSEC代理向集中式Wazuh Server报告。话虽如此，建议将服务器和代理都更新为最新版本。


