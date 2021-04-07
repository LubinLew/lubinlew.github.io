# Wazuh 入门

Wazuh 是一个免费的开源安全平台，用于威胁的预防，检测和响应。
它可在本地，虚拟化，容器化和基于云的环境中保护工作负载。
从小型企业到大型企业，Wazuh在全球成千上万的组织中得到广泛使用。
Wazuh 解决方案由部署到需要被监视系统上的 [Wazuh Agent](#wazuh-agent) 和用于收集与分析 `Agent` 上报的数据的 [Wazuh Server](#wazuh-server) 组成。
此外，Wazuh 已与 [Elastic Stack](#elastic-stack) 完全集成，提供了搜索引擎和数据可视化工具，使用户可以浏览各类安全警报。

**目录**
=================

* [社区与支持](#社区与支持)
* [组件](#组件components)
  - [Wazuh Agent](#wazuh-agent)
    - [Agent 架构](#agent-架构)
    - [Agent 模块](#agent-模块)
    - [与 Server 通信](#与-wazuh-server-通信)
  - [Wazuh Server](#wazuh-server)
    - [Server 架构](#server-架构)
    - [Server 服务](#server-服务)
  - [Elastic Stack](#elastic-stack)
* [Wazuh 架构](#wazuh-架构)
  - [Wazuh Agent 和 Wazuh Server 间的通信](#wazuh-agent-和-wazuh-server-间的通信)
  - [Wazuh server 和 Elastic Stack 间的通信](#wazuh-server-和-elastic-stack-间的通信)
  - [使用的端口](#使用的端口)
  - [档案数据存储](#档案数据存储)
* [用例](#用例)
  - [日志数据分析](#日志数据分析log-data-analysis)
  - [文件完整性监控](#文件完整性监控file-integrity-monitoring)
  - [rootkit检测](#rootkit检测rootkits-detection)
  - [主动响应](#主动响应active-response)
  - [配置评估](#配置评估configuration-assessment)
  - [系统盘点](#系统盘点system-inventory)
  - [漏洞检测](#漏洞检测vulnerability-detection)
  - [云安全监控](#云安全监控cloud-security-monitoring)
  - [容器安全监控](#容器安全监控containers-security-monitoring)
  - [监管合规](#监管合规regulatory-compliance)
## 社区与支持

Wazuh 是世界上最大的安全开源社区之一。您可以成为其中的一部分，以向其他用户学习，参与讨论，与我们的开发团队交流并为该项目做出贡献。您可以通过从以下渠道参与：

- [Slack频道](https://wazuh.com/community/join-us-on-slack/)：加入我们的社区频道，与我们的开发人员和技术团队进行实时聊天。

- [Google网上论坛](https://groups.google.com/g/wazuh)：您可以通过电子邮件订阅分享问题并向其他 Wazuh 用户学习。

- [GitHub](https://github.com/wazuh)：获得 Wazuh 源代码，报告问题并为该项目做贡献。

此外，还提供[商业支持，培训，咨询服务](https://wazuh.com/professional-services/)。

----

## 组件(Components)

Wazuh 平台提供的功能可保护您的云，容器和服务器工作负载。其中包括日志数据分析，入侵和恶意软件检测，文件完整性监视，配置评估，漏洞检测以及对法规遵从性的支持。 Wazuh解决方案基于以下三个组件：

- [Wazuh Agent](#wazuh-agent) ：安装在便携式计算机，台式机，服务器，云实例或虚拟机等端点上，它提供预防，检测和响应功能。它支持Windows，Linux，MacOS，HP-UX，Solaris和AIX平台。下面文章中会简写为 `Agent`。

- [Wazuh Server](#wazuh-server)：它用来分析从 `Agent` 接收到的数据，通过解码器(decoders)和规则(rules)对其进行处理，并使用威胁情报(threat intelligence)来查找已知的危害指标(IOC:indicators of compromise)。下面文章中会简写为 `Server`。<mark>一个 Server 可以分析来自成百上千个 `Agent` 的数据，并且能够配置为可以水平扩展的集群</mark>。`Server` 还用于管理 `Agent`，在必要时进行远程配置和升级。

- [Elastic Stack](#elastic-stack) ：它用来存储由 `Server` 生成的警报。此外，Wazuh 和 Kibana 之间深度集成, 为数据的可视化与分析提供了强大的用户界面。此界面还用于管理 Wazuh 配置并监视其状态。 除了基于 `Agent` 的监视功​​能外，<mark>Wazuh 平台还可以监视无 `Agent` 的设备，例如防火墙，交换机，路由器或网络IDS等</mark>。例如，可以通过Syslog收集系统日志数据，并可以通过定期探测其数据（例如，通过SSH或通过API）来监视其配置。 下图显示了 Wazuh 组件和数据流。它显示了解决方案的三个主要组件，分别是  [Wazuh Agent](#wazuh-agent) ， [Wazuh Server](#wazuh-server)  和  [Elastic Stack](#elastic-stack) 。

![Wazuh数据流](_resources/data_flow.png)

### Wazuh Agent

[Wazuh Agent](#wazuh-agent) 可在多种操作系统上运行。支持笔记本电脑、台式机、服务器、云实例、容器和虚拟机上部署。它提供了威胁预防，检测和响应功能。它也用于收集不同类型的系统和应用程序数据，并通过加密并经过身份验证的通道将其转发到 [Wazuh Server](#wazuh-server) 。

#### Agent 架构

[Wazuh Agent](#wazuh-agent) 具有模块化架构，其中不同组件负责各自的任务：监视文件系统，读取日志消息，收集清单数据，扫描系统配置，查找恶意软件等。用户可以通过配置启用或禁用某些 `Agent` 模块。 下图显示了 `Agent` 架构和组件：

![Agent架构](_resources/architecture_agent.png)

#### Agent 模块

所有 `Agent` 模块都有不同的用途和设置。下面各个模块的简略描述：

- **日志收集器**(Log collector)：此模块可以读取日志文件和Windows事件，收集操作系统和应用程序的日志消息。它支持Windows事件的XPath过滤器，并且可以识别多行格式(例如Linux Audit logs)。它还可以使用额外的元数据来丰富JSON事件。

- **命令执行**(Command execution)：此模块可以定期运行授权命令，收集其输出并将其上报给 [Wazuh Server](#wazuh-server) 以进行进一步分析。该模块可用于满足不同目的，例如，监视剩余的硬盘空间，获取上次登录用户的列表等。

- **文件完整性监视**（FIM:File integrity monitoring)：此模块监视文件系统，并在创建，删除或修改文件时进行上报。它持续追踪文件的 属性，权限，所有权和内容。当事件发生时，它将实时捕获事件的详细信息(who/when/what)。此外，该模块使用受监视文件的状态构建和维护了一个数据库，从而允许远程查询。

- **安全配置评估**(SCA:Security configuration assessment)：此组件利用基于Internet安全中心（CIS）基准的即用型检查规则提供连续的配置评估功能。用户还可以创建自己的SCA检查规则来增强安全策略。

- **系统清单**(System inventory)：此模块可以定期运行扫描收集清单数据，例如操作系统版本，网络接口，运行的进程，已安装的应用程序以及打开的端口列表。扫描结果存储在本地SQLite数据库中，可以远程查询。

- **恶意软件检测**(Malware detection)：使用基于非签名(non-signature)的方法，此组件能够检测**异常**(anomaly)和可能存在的rootkit。能够监视系统调用，查找隐藏的进程、文件、端口。

- **主动响应**(Active response)：检测到威胁时，此模块将自动执行某些操作。尤其是它可以阻断网络连接，停止正在运行的进程或删除恶意文件。用户也可以在必要时创建自定义响应（例如，在沙盒中运行二进制文件，捕获网络连接流量，使用防病毒软件扫描文件等）。

- **容器安全监控**(Containers security monitoring)：此模块集成了Docker Engine API，以监视容器化环境中的变更。例如，它检测到容器镜像，网络配置或数据卷(volume)的变更。此外，它还会警告以特权模式运行的容器以及正在运行的容器中执行命令的用户。

- **云安全监控**(Cloud security monitoring)：此组件监视云提供商例如Amazon AWS，Microsoft Azure或Google GCP。它与云平台的API进行本地通信。可以检测到云基础架构的更改（例如，创建新用户，修改安全组，停止云实例等），并收集云服务日志数据（例如，AWS Cloudtrail，AWS Macie，AWS GuardDuty ，Azure Active Directory等）

#### 与 Wazuh Server 通信

[Wazuh Agent](#wazuh-agent) 与 [Wazuh Server](#wazuh-server) 进行通信，以便于将收集的数据和与安全相关的事件发送给 [Wazuh Server](#wazuh-server)。此外，[Wazuh Agent](#wazuh-agent) 还会发送操作数据，报告其配置和状态。
连接后，可以从 [Wazuh Server](#wazuh-server) 远程升级，监视和配置 [Wazuh Agent](#wazuh-agent)。
[Wazuh Agent](#wazuh-agent)与服务器的通信通过安全通道（TCP或UDP）进行，实时提供数据加密和压缩。
此外，它还有流控机制，可避免泛洪，在必要时对事件进行排队并保护网络带宽。
<mark>在第一次将 [Wazuh Agent](#wazuh-agent) 连接到服务器之前，必须先注册。此过程为 [Wazuh Agent](#wazuh-agent) 提供了唯一的预共享密钥，该密钥用于身份验证和数据加密</mark>。

----

### Wazuh Server

 [Wazuh Server](#wazuh-server) 负责分析从 `Agent` 接收的数据，并在检测到威胁或异常时触发警报。它还用于远程管理、配置、监视 `Agent` 的状态。  [Wazuh Server](#wazuh-server) 使用威胁情报源来提高其检测能力。它还利用法规遵从性要求（例如PCI DSS，HIPAA，NIST 800-53…）和Mitre ATT＆CK框架来丰富警报数据，从而提供有关警报的有用上下文。 另外， [Wazuh Server](#wazuh-server) 可以与外部软件集成，例如票务系统（例如Service Now，Jira，PagerDuty）和即时消息传递平台（例如Slack）。这可简化安全操作。

#### Server 架构

 [Wazuh Server](#wazuh-server) 运行分析引擎，Wazuh RESTful API，`Agent` 注册服务，`Agent`连接服务，Wazuh集群守护程序和Filebeat。下图表示服务器体系结构和组件：

![Server架构](_resources/architecture_server.png)

#### Server 服务

服务器通常在独立的物理机，虚拟机，docker容器或云实例上运行。它需要安装在Linux操作系统上。以下是服务的简述：

- **Agent 注册服务**(Agents registration service)：用于通过供应和分配每个 `Agent` 唯一的预共享身份验证密钥来注册新的 `Agent`。此进程作为网络服务运行，并支持通过TLS/SSL证书或固定密码进行身份验证。

- **Agent 连接服务**(Agents connection service)：这是从 `Agent` 接收数据的服务。它利用预共享密钥来验证每个 `Agent` 身份并加密 `Agent` 与 `Server` 之间的通信。此外，此服务用于提供集中的配置管理，能够远程推送新的 `Agent` 设置。

- **分析引擎**(Analysis engine)：这是执行数据分析的进程。它利用解码器来识别正在处理的信息的类型（例如Windows事件，SSHD日志，Web服务器日志等），并从日志消息中提取相关的数据元素（例如源IP地址，事件ID，用户名等）。 接下来，通过使用规则，它可以识别解码事件中的特定模式，这些特定模式可能会触发警报，甚至可能要求采取自动对策（例如，在防火墙上的禁止某些IP）。

- **Wazuh RESTful API**：此服务提供了与 Wazuh 基础结构进行交互的接口。它用于管理 `Agent` 和 `Server` 配置的设置，监视基础结构状态和整体运行状况，管理和编辑Wazuh 解码器和规则，以及查询受监视端点的状态。 Wazuh Web用户界面（Kibana应用程序）也使用它。

- **Wazuh cluster daemon**：此服务用于水平扩展 [Wazuh Server](#wazuh-server) ，将它们部署为群集。这种配置需要与网络负载均衡器(LB)相结合，可提供高可用(HA)和负载均衡。 Wazuh集群守护程序是 [Wazuh Server](#wazuh-server) 用来相互通信并保持同步的工具。

- **Filebeat**：用于将事件和警报发送到 Elasticsearch。它读取 Wazuh 分析引擎的输出并实时发送事件。当连接到多节点 Elasticsearch 集群时，它还提供负载均衡。

----

### Elastic Stack

Elastic Stack 是流行的开源日志管理套件，包括Elasticsearch，Kibana，Filebeat等工具。与Wazuh解决方案特别相关的项目是：

- **Filebeat**：一种轻量级转发器，使用golang开发，用于通过网络传送日志，通常将日志传送到Elasticsearch。在 [Wazuh Server](#wazuh-server) 上使用它来将事件和警报发送到Elasticsearch。它读取 Wazuh 分析引擎的输出，并通过加密通道实时发送事件。当连接到多节点 Elasticsearch 集群时，它还提供负载均衡。

- **Elasticsearch**：高度可扩展的全文本搜索和分析引擎,使用Java开发。 Elasticsearch是分布式的，这意味着数据索引被分为多个分片，每个分片可以具有零个或多个副本。 Wazuh 对警报数据，原始事件和状态监视信息使用不同的索引。

- **Kibana**：灵活直观的Web界面，用于挖掘，分析和可视化数据。它在 Elasticsearch 集群中的索引内容之上运行。 Wazuh Web 用户界面已以插件的形式完全嵌入在 Kibana 中。它包括用于安全事件，法规遵从性（例如PCI DSS，GDPR，CIS，HIPAA，NIST 800-53）的现成仪表板，检测到的易受攻击的应用程序，文件完整性监控数据，配置评估结果，云基础架构监控事件等。

Wazuh 与 [Elastic Stack](#elastic-stack) 集成在一起，提供了已经解码的消息的提要，以供 Elasticsearch 进行索引，还提供了用于警报和日志数据分析的实时 Web 控制台。此外，在 Kibana 之上运行的 Wazuh 用户界面用于管理和监视 Wazuh 基础结构。 Elasticsearch 索引是具有相似特征（例如某些公共字段和共享数据保留要求）的文档的集合。 Wazuh 利用每天创建的多达三个不同的索引来存储不同的事件类型：

- **wazuh-alerts**： [Wazuh Server](#wazuh-server) 生成警报的索引。每次触发的事件具有足够高优先级的规则（此阈值是可配置的）时，就会创建这些规则。

- **wazuh-events**：无论是否执行规则，从 `Agent` 收到的所有事件（存档数据）的索引。

- **wazuh-monitoring**：一段时间内与 [Wazuh Agent](#wazuh-agent) 状态相关的数据的索引。 Web 界面使用它来表示各个 `Agent` 何时处于活动状态，已断开连接或从不连接。

索引由文档(document)组成。对于以上索引，文档是单独的警报，已归档的事件或与 [Wazuh Agent](#wazuh-agent) 状态相关的数据。 Elasticsearch 索引分为一个或多个分片，每个分片可以选择具有一个或多个副本。每个主分片和副本分片都是一个单独的 Lucene 索引。因此，Elasticsearch 索引由许多 Lucene 索引组成。在 Elasticsearch 索引上运行搜索时，将在所有分片上并行执行搜索，并合并结果。在多节点 Elasticsearch 集群中使用 Elasticsearch 索引划分为多个分片和副本，目的是扩大搜索范围并实现高可用性。单节点Elasticsearch 集群通常每个索引只有一个分片，没有副本。

---

## Wazuh 架构

Wazuh 架构基于在受监视的端点上运行的 `Agent`，这些 `Agent` 将安全相关的数据转发到 `Server` 上。此外，Wazuh 还支持无 `Agent` 设备（例如防火墙，交换机，路由器，访问点等），并且可以通过Syslog，SSH或使用其自己的API主动提交日志数据。`Server` 对输入的信息进行解码和分析，并将结果传递给 Elasticsearch 集群以进行索引和存储。

Elasticsearch 集群是一个或多个节点的集合，这些节点相互通信以对索引执行读取和写入操作。不需要处理大量数据的小型 Wazuh 部署可以通过单节点群集轻松处理。当有大量受监视的端点，预期有大量数据或需要高可用性时，建议使用多节点群集。

对于生产环境，建议将 [Wazuh Server](#wazuh-server) 和 Elasticsearch 部署到不同的主机。在这种情况下，Filebeat 用于使用 TLS 加密将 Wazuh 警报和存档的事件安全地转发到 Elasticsearch 群集（单节点或多节点）。

下图显示了 Wazuh 的部署架构。它显示了解决方案组件以及如何将 [Wazuh Server](#wazuh-server) 和 Elasticsearch 配置为群集，从而提供负载均衡和高可用性。

![Wazuh架构](_resources/deployment.png)

### Wazuh Agent 和 Wazuh Server 间的通信

[Wazuh Agent](#wazuh-agent) 将事件发送到 [Wazuh Server](#wazuh-server) 以进行分析和威胁检测。为了开始传送它们，`Agent` 与 `Server` 建立连接，该连接服务监听端口1514（这是可配置的）。然后 [Wazuh Server](#wazuh-server) 使用分析引擎对接收到的事件进行解码和规则检查。触发规则的事件会增加警报数据，例如规则ID和规则名称。可以将事件假脱机到以下一个文件或两个文件中，具体取决于规则是否被执行：

- 文件 `/var/ossec/logs/archives/archives.json`包含所有事件，无论它们是否违反规则。

- 文件`/var/ossec/logs/alerts/alerts.json`仅包含以足够高的优先级（阈值可配置）使规则中断的事件。

Wazuh 消息协议默认情况下使用 AES 加密，每个块具有128位和256位密钥（Blowfish加密也是可选的）。

> 阅读 [Wazuh使用AES加密通信的好处](https://wazuh.com/blog/benefits-of-using-aes-in-our-communications) 来了解更多细节内容。

### Wazuh server 和 Elastic Stack 间的通信

 [Wazuh Server](#wazuh-server) 使用 Filebeat 通过 TLS 加密将警报和事件数据发送到 Elasticsearch 服务器。 Filebeat 读取 [Wazuh Server](#wazuh-server) 的输出数据并将其发送到 Elasticsearch（默认情况下，监听端口9200/TCP）。 Elasticsearch 为数据建立索引后，将使用 Kibana 来挖掘和可视化信息。 Wazuh Web 用户界面作为插件在 Kibana 内部运行。它查询 Wazuh RESTful API（默认情况下，监听 [Wazuh Server](#wazuh-server) 上的端口55000/TCP），以显示 [Wazuh Server](#wazuh-server) 和`Agent` 的配置和与状态有关的信息。如果需要，它还可以通过API调用修改 `Agent` 或 `Server` 配置设置。此通信使用 TLS 加密，并使用用户名和密码进行身份验证。

### 使用的端口

Wazuh组件的通信使用了几种服务。下面是这些服务使用的默认端口的列表。用户可以在必要时修改这些端口号

| Component     | Software      | Port      | Protocol | Purpose                             |
| ------------- | ------------- | --------- | -------- | ----------------------------------- |
| Wazuh server  | Wazuh manager | 1514      | TCP(默认)  | Agent 连接服务                          |
| -             | -             | 1514      | UDP      | Agent 连接服务                          |
| -             | -             | 1515      | TCP      | Agent 连接服务                          |
| -             | -             | 1516      | TCP      | Wazuh cluster daemon                |
| -             | -             | 514       | UDP(默认)  | Wazuh syslog collector (默认不开启)      |
| -             | -             | 514       | TCP      | Wazuh syslog collector (默认不开启)      |
| -             | Wazuh API     | 55000     | TCP      | Wazuh RESTful API                   |
| Elastic Stack | Elasticsearch | 9200      | TCP      | Elasticsearch RESTful API           |
| -             | Elasticsearch | 9300-9400 | TCP      | Elasticsearch cluster communication |
| -             | Kibana        | 5601      | TCP      | Kibana web interface                |

### 档案数据存储

警报事件和非警报事件都存储在 [Wazuh Server](#wazuh-server) 上的文件中，除了发送到 Elasticsearch 外。这些文件可以JSON格式（.json）和/或纯文本格式（.log-无解码字段，但更紧凑）编写。这些文件每天使用MD5，SHA1和SHA256校验和进行压缩和签名。目录和文件名结构如下：

```bash
root@wazuh-manager:/var/ossec/logs/archives/2020/Jan# ls -l
total 176
-rw-r----- 1 ossec ossec 234350 Jan  2 00:00 ossec-archive-01.json.gz
-rw-r----- 1 ossec ossec    350 Jan  2 00:00 ossec-archive-01.json.sum
-rw-r----- 1 ossec ossec 176221 Jan  2 00:00 ossec-archive-01.log.gz
-rw-r----- 1 ossec ossec    346 Jan  2 00:00 ossec-archive-01.log.sum
-rw-r----- 1 ossec ossec 224320 Jan  2 00:00 ossec-archive-02.json.gz
-rw-r----- 1 ossec ossec    350 Jan  2 00:00 ossec-archive-02.json.sum
-rw-r----- 1 ossec ossec 151642 Jan  2 00:00 ossec-archive-02.log.gz
-rw-r----- 1 ossec ossec    346 Jan  2 00:00 ossec-archive-02.log.sum
-rw-r----- 1 ossec ossec 315251 Jan  2 00:00 ossec-archive-03.json.gz
-rw-r----- 1 ossec ossec    350 Jan  2 00:00 ossec-archive-03.json.sum
-rw-r----- 1 ossec ossec 156296 Jan  2 00:00 ossec-archive-03.log.gz
-rw-r----- 1 ossec ossec    346 Jan  2 00:00 ossec-archive-03.log.sum
```

建议根据 [Wazuh Server](#wazuh-server) 的存储容量旋转和备份存档文件。通过使用cron作业，您可以轻松地安排在服务器上仅在本地本地保留存档文件的某个时间窗口（例如，去年或过去三个月）。 <mark>另一方面，您可以选择完全放弃存储归档文件，而仅依靠 Elasticsearch 进行归档存储，尤其是如果您正在运行定期的 Elasticsearch 快照备份和/或具有分片副本的多节点Elasticsearch集群以实现高可用性</mark>。您甚至可以使用cron作业将快照索引移动到最终数据存储服务器，并使用MD5，SHA1和SHA256哈希算法对其进行签名。

----

## 用例

Wazuh 平台可以使用不同方式保护和监视系统。它通常用于威胁预防，检测和响应。此外，Wazuh 平台用于满足法规遵从性要求（例如PCI DSS或HIPAA）和配置标准（CIS加固指南）。 Wazuh 是 IaaS 用户（例如Amazon AWS，Azure或Google云）中一种流行的安全解决方案，用于监视虚拟机和云实例。这是在系统级别利用 [Wazuh Agent](#wazuh-agent) 完成的，而在基础架构级别则是直接从云提供商的API中提取数据。 此外，Wazuh通常用于保护提供云原生运行时安全性的容器化环境。此功能基于与Docker引擎API和Kubernetes API的集成。此外，为了获得更好的保护，[Wazuh Agent](#wazuh-agent) 可以在Docker主机上运行，​​从而提供了完整的威胁检测和响应功能。 在文档的此部分中，您将找到 Wazuh 解决方案的一些常见用例的简短示例。

01.[Log data analysis](#日志数据分析log-data-analysis)
02.[File integrity monitoring](#文件完整性监控file-integrity-monitoring)
03.[Rootkits detection](#rootkit检测rootkits-detection)
04.[Active response](#主动响应active-response)
05.[Configuration assessment](#配置评估configuration-assessment)
06.[System inventory](#系统盘点system-inventory)
07.[Vulnerability detection](#漏洞检测vulnerability-detection)
08.[Cloud security monitoring](#云安全监控cloud-security-monitoring)
09.[Containers security monitoring](#容器安全监控containers-security-monitoring)
10.[Regulatory compliance](#监管合规regulatory-compliance)

### 日志数据分析(Log data analysis)

自动化的日志管理和分析可加速威胁检测。在许多情况下，可以在设备，系统和应用程序的日志消息中找到攻击的证据。 Wazuh 可用于自动聚合和分析日志数据。 运行在受监视端点上的 [Wazuh Agent](#wazuh-agent) 通常负责读取操作系统和应用程序日志消息，并将其转发到 [Wazuh Server](#wazuh-server) 进行分析。如果未部署 `Agent`，则 `Server` 还可以通过syslog从网络设备或应用程序接收数据。 Wazuh 使用解码器来识别日志消息的源应用程序。然后，它使用特定于应用程序的规则分析数据。这是用于检测SSH身份验证失败事件的规则的示例：

```xml
<rule id="5716" level="5">
  <if_sid>5700</if_sid>
  <match>^Failed|^error: PAM: Authentication</match>
  <description>SSHD authentication failed.</description>
  <mitre>
    <id>T1110</id>
  </mitre>
  <group>authentication_failed,pci_dss_10.2.4,pci_dss_10.2.5,gpg13_7.1,gdpr_IV_35.7.d,gdpr_IV_32.2,hipaa_164.312.b,nist_800_53_AU.14,nist_800_53_AC.7,tsc_CC6.1,tsc_CC6.8,tsc_CC7.2,tsc_CC7.3,</group>
</rule>
```

规则包括一个`match`字段，用于定义规则将要查找的模式。
还有一个`level`字段，用于指定结果警报的优先级。
此外，规则还利用Mitre ATT＆CK框架中的技术标识符以及映射到法规遵从性控制来丰富事件。
每当 [Wazuh Agent](#wazuh-agent) 或通过syslog收集的事件与优先级高于预定义阈值（默认值为3）的规则匹配时，管理器都会生成警报。
这是在`/var/ossec/logs/alerts/alerts.json`中找到的示例（为简洁起见，某些字段已删除）：

```json
  "agent": {
      "id": "005",
      "ip": "10.0.1.175",
      "name": "Centos"
  },
  "predecoder": {
      "hostname": "ip-10-0-1-175",
      "program_name": "sshd",
      "timestamp": "Jul 12 15:32:41"
  },
  "decoder": {
      "name": "sshd",
      "parent": "sshd"
  },
  "full_log": "Jul 12 15:32:41 ip-10-0-1-175 sshd[21746]: Failed password for root from 61.177.172.13 port 61658 ssh2",
  "location": "/var/log/secure",
  "data": {
      "dstuser": "root",
      "srcip": "61.177.172.13",
      "srcport": "61658"
  },
  "rule": {
      "description": "sshd: authentication failed.",
      "id": "5716",
      "level": 5,
      "mitre": {
          "id": [
              "T1110"
          ],
          "tactic": [
              "Credential Access"
          ],
          "technique": [
              "Brute Force"
          ]
      },
  },
  "timestamp": "2020-07-12T15:32:41.756+0000"
}
```

 [Wazuh Server](#wazuh-server) 一旦生成警报，便会将警报发送到 [Elastic Stack](#elastic-stack) 组件，在其中增加地理位置信息并进行存储和索引。然后，可以使用Kibana搜索，分析和可视化数据。以下是界面中显示的警报：

![报警](_resources/use_case_log_analysis.png)

Wazuh 提供了一个默认规则集，该规则集会定期更新，其中包含针对不同系统和应用程序的3000多个规则。此外，Wazuh 允许创建自定义规则。在用户手册中查找更多信息。

---

### 文件完整性监控(File integrity monitoring)

修改操作系统或应用程序文件时，文件完整性监视（FIM）组件会检测并发出警报。此功能通常用于检测对敏感数据的访问或更改。如果您的服务器符合PCI DSS，则要求11.5规定必须安装文件完整性监视解决方案才能成功通过审核。 以下是更改受监视文件时生成的警报的示例。元数据包括MD5，SHA1和SHA256校验和，文件大小（更改前后），文件许可权，文件所有者，内容更改以及进行这些更改的用户（谁的数据）。

```json
{
  "agent": {
      "id": "006",
      "ip": "10.0.1.214",
      "name": "RHEL7"
  },
  "decoder": {
      "name": "syscheck_integrity_changed"
  },
  "syscheck": {
      "audit": {
          "effective_user": {
              "id": "0",
              "name": "root"
          },
          "group": {
              "id": "0",
              "name": "root"
          },
          "login_user": {
              "id": "1001",
              "name": "wazuh"
          },
          "process": {
              "cwd": "/home/wazuh",
              "id": "13235",
              "name": "/usr/bin/vim",
              "parent_cwd": "/home/wazuh",
              "parent_name": "/usr/bin/bash",
              "ppid": "10942"
          },
          "user": {
              "id": "0",
              "name": "root"
          }
      },
      "diff": "0a1\n> 8.8.8.8     ads.fastclick.net\n",
      "event": "modified",
      "gid_after": "0",
      "gname_after": "root",
      "inode_after": 198194,
      "inode_before": 55652,
      "md5_after": "feb5cea0deb5925101b642bca97dc7b3",
      "md5_before": "54fb6627dbaa37721048e4549db3224d",
      "mode": "whodata",
      "mtime_after": "2020-07-12T18:07:57",
      "mtime_before": "2020-07-12T18:00:55",
      "path": "/etc/hosts",
      "perm_after": "rw-r--r--",
      "sha1_after": "2aa2079c3972b4bb8f28d69877a7c5e93dacbe6f",
      "sha1_before": "7335999eb54c15c67566186bdfc46f64e0d5a1aa",
      "sha256_after": "48f09f8c313b303ce2ca9365f70ae8d992c5589c56493ac055f0ab129d82c365",
      "sha256_before": "498f494232085ec83303a2bc6f04bea840c2b210fbbeda31a46a6e5674d4fc0e",
      "size_after": "188",
      "size_before": "158",
      "uid_after": "0",
      "uname_after": "root"
  },
  "rule": {
      "description": "Integrity checksum changed.",
      "id": "550",
      "level": 7,
      "mitre": {
          "id": [
              "T1492"
          ],
          "tactic": [
              "Impact"
          ],
          "technique": [
              "Stored Data Manipulation"
          ]
      }
  },
  "timestamp": "2020-07-12T18:07:57.676+0000"
}
```

在FIM仪表板中可以找到文件更改的完整摘要，该仪表板提供了向下追溯功能，可以查看触发的警报的所有详细信息。

![](_resources/use_case_fim.png)

---

### rootkit检测(Rootkits detection)

 [Wazuh Agent](#wazuh-agent) 会定期扫描受监视的系统，以检测内核和用户空间级别的rootkit。这种恶意软件通常会替换或更改现有操作系统的组件，以更改系统行为。 Rootkit可以隐藏其他进程，文件和网络连接。 Wazuh使用不同的检测机制来搜索系统异常或众所周知的入侵。 Rootcheck组件定期执行此操作：

| Action                        | Detection mechanism                                 | Binary  | System call |
| ----------------------------- | --------------------------------------------------- | ------- | ----------- |
| Detection of hidden processes | Comparing output of systembinaries and system calls | ps      | setsid      |
| -                             | -                                                   | -       | getpgid     |
| -                             | -                                                   | -       | kill        |
| Detection of hidden files     | Comparing output of systembinaries and system calls | ls      | stat        |
| -                             | -                                                   | -       | opendir     |
| -                             | -                                                   | -       | readdir     |
| -                             | Scanning /dev                                       | ls      | opendir     |
| Detection of hidden ports     | Comparing output of systembinaries and system calls | netstat | bind        |
| Detection of known rootkits   | Using a malicious file database                     | -       | stat        |
| -                             | -                                                   | -       | fopen       |
| -                             | -                                                   | -       | opendir     |
| -                             | Inspecting files content usingsignatures            | -       | fopen       |
| -                             | Detecting file permission andownership anomalies    | -       | stat        |

以下是发现隐藏进程时生成警报的示例。在这种情况下，受影响的系统正在运行Linux内核级rootkit（名为Diamorphine）：

```json
{
  "agent": {
      "id": "1030",
      "ip": "10.0.0.59",
      "name": "diamorphine-POC"
  },
  "decoder": {
      "name": "rootcheck"
  },
  "full_log": "Process '562' hidden from /proc. Possible kernel level rootkit.",
  "rule": {
      "description": "Host-based anomaly detection event (rootcheck).",
      "id": "510",
      "level": 7
  },
  "timestamp": "2020-07-12T18:07:00-0800"
}
```

[BACK TO TOP](#%E7%94%A8%E4%BE%8B)

----

### 主动响应(Active response)

 [Wazuh Agent](#wazuh-agent) 通过在检测到威胁时运行操作来自动对威胁做出响应。<mark>Agent 具有阻止网络连接，停止运行进程或删除恶意文件等功能</mark>。此外，它还可以运行由用户开发的自定义脚本（例如，Python，Bash或PowerShell）。 为了使用此功能，用户定义了将触发脚本操作的条件，这些条件通常涉及威胁检测和评估。例如，用户可以使用日志分析规则来检测入侵尝试，并使用IP信誉数据库通过查找尝试的连接的源IP地址来评估威胁。 在上述情况下，当源IP地址被识别为恶意（信誉低下）时，通过自动设置防火墙规则以丢弃来自攻击者的所有流量，可以保护受监视系统。根据活动响应，此防火墙规则是临时的或永久的。 在下面，您可以找到用于定义两个用于自动连接阻止的脚本的配置。在Linux系统上， [Wazuh Agent](#wazuh-agent) 通常为此目的与本地Iptables防火墙集成。相反，在Windows系统上，它使用空路由技术（也称为blackholing）：

```xml
<command>
  <name>firewall-drop</name>
  <executable>firewall-drop.sh</executable>
  <expect>srcip</expect>
  <timeout_allowed>yes</timeout_allowed>
</command>
```

```xml
<command>
  <name>win_route-null</name>
  <executable>route-null.cmd</executable>
  <expect>srcip</expect>
  <timeout_allowed>yes</timeout_allowed>
</command>
```

在定义的命令之上，活动响应设置触发它们所需的条件。下面是一个示例，该示例将在日志分析规则`100100`匹配时触发firewall-drop命令。

```xml
<active-response>
  <command>firewall-drop</command>
  <location>local</location>
  <rules_id>100100</rules_id>
  <timeout>60</timeout>
</active-response>
In this case, rule 100100 is used to look for alerts where the source IP address is part of a well-known IP reputation database:
```

```xml
<rule id="100100" level="10">
  <if_group>web|attack|attacks</if_group>
  <list field="srcip" lookup="address_match_key">etc/lists/blacklist-alienvault</list>
  <description>IP address found in AlienVault reputation database.</description>
</rule>
```

以下是带有两个Wazuh警报的屏幕截图：一个是在检测到Web攻击（试图利用PHP服务器漏洞）时触发的，另一个是通知恶意行为者已被阻止的触发。

![1](_resources/use_case_active_response.png)

[BACK TO TOP](#%E7%94%A8%E4%BE%8B)

---

### 配置评估(Configuration assessment)

自动化安全配置评估（SCA）是改善企业安全状况并减少攻击面的一项基本功能。 Wazuh SCA模块有助于通过受监视的端点维护标准配置。这是通过基于Internet安全中心（CIS）强化指南的预定义检查来完成的。 启用S​​CA模块后， [Wazuh Agent](#wazuh-agent) 会定期执行扫描，并报告受监视系统中的错误配置。这些扫描通过策略文件评估系统的配置，其中包含一组要运行的检查。例如，SCA检查可以检查文件系统配置，查找是否存在软件更新或安全补丁，查看是否启用了本地防火墙，识别不必要的运行服务或验证用户的密码策略。 SCA扫描策略以YAML格式编写，使用户可以快速了解它们。使用SCA语法，用户可以扩展现有策略以满足他们的需求或编写新策略。每个策略包含一组检查，并且每个检查都有一个或多个规则。例如，规则可用于查找文件，目录，Windows注册表项或正在运行的进程等的存在。也可以执行命令并根据正则表达式检查其输出。 Linux SCA规则示例：

```yaml
- id: 5546
  title: "Ensure IP forwarding is disabled"
  description: "The net.ipv4.ip_forward flag is used to tell the system whether it can forward packets or not."
  rationale: "Setting the flag to 0 ensures that a system with multiple interfaces (for example, a hard proxy), will never be able to forward packets, and therefore, never serve as a router."
  remediation: "Set the following parameter in /etc/sysctl.conf or a /etc/sysctl.d/* file: net.ipv4.ip_forward = 0 and set the active kernel parameters."
  compliance:
    - cis: ["3.1.1"]
    - cis_csc: ["3", "11"]
    - pci_dss: ["2.2.4"]
    - nist_800_53: ["CM.1"]
  condition: all
  rules:
    - 'c:sysctl net.ipv4.ip_forward -> r:^net.ipv4.ip_forward\s*=\s*0$'
    - 'c:grep -Rh net\.ipv4\.ip_forward /etc/sysctl.conf /etc/sysctl.d -> r:^net.ipv4.ip_forward\s*=\s*0$'
```

Windows SCA规则示例：

```yaml
- id: 14038
  title: "Ensure Microsoft Firewall is enabled"
  compliance:
    - pci_dss: ["10.6.1", "1.4"]
    - hipaa: ["164.312.b", "164.312.a.1"]
    - nist_800_53: ["AU.6", "SC.7"]
    - tsc: ["CC6.1", "CC6.8", "CC7.2", "CC7.3", "CC6.7"]
  condition: all
  rules:
    - 'r:HKEY_LOCAL_MACHINE\software\policies\microsoft\windowsfirewall\domainprofile -> enablefirewall -> 1'
```

macOS SCA规则示例：

```yaml
- id: 8522
  title: "Ensure nfs server is not running"
  description: "macOS can act as an NFS fileserver. NFS sharing could be enabled to allow someone on another computer to mount shares and gain access to information from the user's computer. File sharing from a user endpoint has long been considered questionable and Apple has removed that capability from the GUI. NFSD is still part of the Operating System and can be easily turned on to export shares and provide remote connectivity to an end user computer."
  rationale: "File serving should not be done from a user desktop, dedicated servers should be used.  Open ports make it easier to exploit the computer."
  remediation: "Ensure that the NFS Server is not running and is not set to start at boot Stop the NFS Server: sudo nfsd disable    Remove the exported Directory listing: rm /etc/export"
  compliance:
    - cis: ["4.5"]
  condition: none
  rules:
    - 'p:nfsd'
    - 'p:/sbin/nfsd'
    - 'f:/etc/exports'
```

以下是配置评估评估结果的示例。这些可以通过Web用户界面或直接通过Wazuh RESTful API获得。

![1](_resources/use_case_sca.png)

[BACK TO TOP](#%E7%94%A8%E4%BE%8B)

---

### 系统盘点(System inventory)

 [Wazuh Agent](#wazuh-agent) 系统清单模块从受监视的系统收集硬件和软件信息。此功能有助于识别资产并评估补丁程序管理的有效性。 可以通过Wazuh RESTful API和Web用户界面查询每个受监视端点收集的清单数据。这包括内存使用情况，磁盘空间，CPU规格，网络接口，打开的端口，正在运行的进程以及已安装的应用程序列表。 为了收集数据， [Wazuh Agent](#wazuh-agent) 运行定期扫描（时间间隔是可配置的）。扫描完成后， [Wazuh Agent](#wazuh-agent) 会将新库存数据与以前扫描中的旧库存数据进行比较。这样， [Wazuh Agent](#wazuh-agent) 可以识别系统事件，例如，当打开新端口，已停止进程或已安装新应用程序时。 硬件清单，网络接口，开放端口和网络设置的示例：

![1](_resources/use_case_inventory_1.png)

软件清单示例：

![1](_resources/use_case_inventory_2.png)

运行进程清单示例：

![1](_resources/use_case_inventory_3.png)

[BACK TO TOP](#%E7%94%A8%E4%BE%8B)

---

### 漏洞检测(Vulnerability detection)

攻击者通常会将易受攻击的软件应用程序作为攻击目标，以破坏端点并在目标网络上获得持久存在。 Wazuh平台依靠其软件清单功能，维护安装了 [Wazuh Agent](#wazuh-agent) 的端点上安装的所有应用程序的最新列表。通过将此信息与国家漏洞数据库（NVD）以及从不同操作系统供应商收集的信息进行关联，Wazuh能够识别易受攻击的应用程序并生成风险报告。 为了检测易受攻击的软件，Wazuh使用通过使用从以下来源提取的数据自动创建的常见漏洞和暴露（CVE）数据库：

- [CVEs for Ubuntu Linux distributions](https://canonical.com/)

- [CVEs for Red Hat and CentOS Linux distributions](https://access.redhat.com/)

- [CVEs for Debian Linux distributions](https://www.debian.org/)

- [CVEs from the National Vulnerability Database](https://nvd.nist.gov/)

- [Microsoft Security Response Center](https://www.microsoft.com/msrc)

为了启用漏洞检测，用户需要配置 [Wazuh Agent](#wazuh-agent) 以收集软件清单数据，并配置 [Wazuh Server](#wazuh-server) 从不同的漏洞源中提取CVE信息。这是漏洞检测警报的示例：

```json
{
  "agent": {
    "id": "003",
    "ip": "10.0.1.102",
    "name": "Windows"
  },
  "location": "vulnerability-detector",
  "data": {
    "vulnerability": {
      "assigner": "cve@mitre.org",
      "cve": "CVE-2020-12395",
      "cve_version": "4.0",
      "cvss": {
        "cvss2": {
          "base_score": "10",
          "vector": {
            "access_complexity": "low",
            "attack_vector": "network",
            "authentication": "none",
            "availability": "complete",
            "confidentiality_impact": "complete",
            "integrity_impact": "complete"
          }
        }
      },
      "cwe_reference": "CWE-119",
      "package": {
        "architecture": "x86_64",
        "condition": "less than 68.8.0",
        "generated_cpe": "a:mozilla:thunderbird:68.0::::::x86_64:",
        "name": "Mozilla Thunderbird 68.0 (x64 en-US)",
        "version": "68.0"
      },
      "published": "2020-05-26",
      "references": [
        "https://bugzilla.mozilla.org/buglist.cgi?bug_id=1595886%2C1611482%2C1614704%2C1624098%2C1625749%2C1626382%2C1628076%2C1631508",
        "https://security.gentoo.org/glsa/202005-03",
        "https://security.gentoo.org/glsa/202005-04",
        "https://usn.ubuntu.com/4373-1/",
        "https://www.mozilla.org/security/advisories/mfsa2020-16/",
        "https://www.mozilla.org/security/advisories/mfsa2020-17/",
        "https://www.mozilla.org/security/advisories/mfsa2020-18/",
        "https://nvd.nist.gov/vuln/detail/CVE-2020-12395"
      ],
      "severity": "High",
      "title": "Mozilla developers and community members reported memory safety bugs present in Firefox 75 and Firefox ESR 68.7. Some of these bugs showed evidence of memory corruption and we presume that with enough effort some of these could have been exploited to run arbitrary code. This vulnerability affects Firefox ESR < 68.8, Firefox < 76, and Thunderbird < 68.8.0.",
      "updated": "2020-06-12"
    }
  },
  "rule": {
    "description": "CVE-2020-12395 affects Mozilla Thunderbird 68.0 (x64 en-US)",
    "id": "23505",
    "level": 10
  },
  "timestamp": "2020-07-20T00:41:36.302+0000"
}
```

漏洞检测仪表板：

![1](_resources/use_case_vulnerability.png)

[BACK TO TOP](#%E7%94%A8%E4%BE%8B)

---

### 云安全监控(Cloud security monitoring)

Wazuh安全平台为多云和混合环境提供威胁检测，配置合规性以及连续监视。它通过在两个不同级别上监视基础结构来保护云工作负载： 

- 端点(Endpoint)级别：使用轻量级的Wazuh安全 [Wazuh Agent](#wazuh-agent) 监视云实例或虚拟机。 

- 云基础架构级别：通过收集和分析提供商API的数据来监视云服务和活动。支持Amazon AWS，Microsoft Azure和Google Cloud Platform。

#### Amazon Web Services

 [Wazuh Agent](#wazuh-agent) 还提供了一个模块来监视和保护您的AWS云基础架构。该模块从S3存储桶中收集AWS服务日志数据，并将收集到的日志消息转发到 [Wazuh Server](#wazuh-server) ，在其中使用AWS的即用Wazuh规则分析事件。 下表描述了Wazuh能够监视的AWS服务： 

- Amazon Guardduty：威胁检测服务，可连续监视恶意活动和未经授权的行为，以保护您的AWS账户，工作负载和Amazon S3中存储的数据。 

- Amazon Inspector：自动化安全评估服务，可帮助改善在AWS上部署的应用程序的安全性和合规性。 

- Amazon Key Management Service（KMS）：用于创建和管理加密密钥，并控制它们在各种AWS服务和您的应用程序中的使用。 

- Amazon Macie：完全托管的数据安全性和数据隐私服务。它会自动检测未加密的S3存储桶，可公开访问的存储桶以及与外部AWS账户共享的存储桶。 

- Amazon Virtual Private Cloud（VPC）：设置AWS Cloud的逻辑隔离部分，在该部分中，AWS资源可以在用户定义的虚拟网络上启动。 

- AWS Config：评估，审核和评估您的AWS资源的配置。 Config持续监视和记录AWS资源配置，从而能够根据所需配置自动评估记录的配置。 

- AWS Cloudtrail：对您的AWS账户进行治理，合规性，运营审核和风险审核。借助CloudTrail，您可以记录，持续监控和保留与整个AWS基础架构中的操作相关的帐户活动。 

- AWS Trusted Advisor：通过优化其AWS环境来帮助用户降低成本，提高性能并提高安全性。它提供了实时指导，以帮助用户遵循AWS最佳实践来调配资源。 

- AWS Web Application Firewall（WAF）：帮助保护您的Web应用程序或API免受可能影响可用性，损害安全性或消耗过多资源的常见Web利用。 

删除AWS安全组时的警报示例： 

```json
{
  "agent": {
      "id": "000",
      "name": "wazuh-manager-master"
  },
  "data": {
      "aws": {
          "awsRegion": "us-west-1",
          "aws_account_id": "1234567890",
          "eventID": "12ab34c-1234-abcd-1234-123456789",
          "eventName": "DeleteSecurityGroup",
          "eventSource": "ec2.amazonaws.com",
          "eventTime": "2020-08-06T15:13:07Z",
          "eventType": "AwsApiCall",
          "eventVersion": "1.05",
          "recipientAccountId": "0987654321",
          "requestID": "12345678-abcd-efgh-1234-123456789",
          "requestParameters": {
              "groupId": "sg-12345678901234567"
          },
          "responseElements": {
              "_return": "true",
              "requestId": "12345678-abcd-efgh-1234-123456789"
          },
          "source": "cloudtrail",
          "sourceIPAddress": "cloudformation.amazonaws.com",
          "userAgent": "cloudformation.amazonaws.com",
          "userIdentity": {
              "accountId": "1234567890",
              "arn": "arn:aws:iam::1234567890:user/john.doe",
              "invokedBy": "cloudformation.amazonaws.com",
              "principalId": "ABCDEFGHIJKLMNH",
              "sessionContext": {
                  "attributes": {
                      "creationDate": "2020-08-06T09:08:14Z",
                      "mfaAuthenticated": "false"
                  }
              },
              "type": "IAMUser",
              "userName": "john.doe"
          }
      },
      "integration": "aws"
  },
  "rule": {
      "description": "AWS Cloudtrail: ec2.amazonaws.com - DeleteSecurityGroup.",
      "id": "80202",
      "level": 3
  }
  "timestamp": "2020-08-06T15:47:14.334+0000"
}
```

AWS仪表板示例：

![1](_resources/use_case_cloud.png)

More information on how Wazuh monitors AWS can be found at our [documentation](https://documentation.wazuh.com/4.0/amazon/index.html#amazon).

#### Microsoft Azure

通过Microsoft Azure的 [Wazuh Agent](#wazuh-agent) 模块，可以轻松提取Azure平台日志。特别是，它旨在从以下服务获取数据： 

- 日志分析API：日志分析API是Azure Monitor服务的核心组件，用于聚合和分析日志数据。此类数据的来源是云应用程序，操作系统和Azure资源。用于Azure的Wazuh模块能够查询Log Analytics API，提取由Azure监视器服务收集的日志。 

- Blob存储API：来自Azure服务的日志可以有选择地推送到Azure Blob存储。具体来说，可以配置Azure服务以将日志导出到为此目的而创建的存储帐户中的容器。之后， [Wazuh Agent](#wazuh-agent) 将通过与Blob Storage API集成来下载这些日志。 Active Directory图API：

- Azure Active Directory图API通过REST API端点提供对AZURE AD的访问。 Wazuh使用它来监视Active Directory事件（例如，创建新用户，更新用户属性，禁用用户帐户等）。
  
  这是Azure警报规则的示例。

```json
{
  "agent": {
      "id": "000",
      "name": "wazuh-manager-master-0"
  },
  "data": {
      "Category": "Administrative",
      "ResourceProvider": "Microsoft.Compute",
      "TenantId": "d4cd75e6-7i2e-554d-b604-3811e9914fea",
      "ActivityStatus": "Started",
      "Type": "AzureActivity",
      "OperationId": "d4elf2e7-65d8-2824-gf44-37742d81c00f",
      "ResourceId": "/WazuhGroup/providers/Microsoft.Compute/virtualMachines/Logstash",
      "OperationName": "Microsoft.Compute/virtualMachines/start/action",
      "CorrelationId": "d4elf2e7-65d8-2824-gf44-37742d81c00f",
      "Resource": "Logstash",
      "Level": "Informational",
      "Caller": "john.doe@email.com",
      "TimeGenerated": "2020-05-25T15:43:16.52Z",
      "ResourceGroup": "WazuhGroup",
      "SubscriptionId": "v1153d2d-ugl4-4221-bc88-40365el115gg",
      "EventSubmissionTimestamp": "2020-05-25T15:43:36.109Z",
      "CallerIpAddress": "83.49.98.225",
      "EventDataId": "69db115c-45ds-664b-4275-a684a72b8df2",
      "SourceSystem": "Azure"
  },
  "rule": {
      "description": "Azure: Log analytics: Microsoft.Compute/virtualMachines/start/action",
      "id": "62723",
      "level": 3
  },
  "timestamp": "2020-05-25T15:45:51.432+0000"
}
```

More information about how to use Wazuh to monitor Microsoft Azure can be found at our [documentation](https://documentation.wazuh.com/4.0/azure/index.html#azure).

#### Google Cloud Platform

Wazuh通过从Google Pub / Sub消息传递服务中提取事件来监视Google Cloud服务，该服务用作事件摄取和传递的中间件。这种集成有助于检测针对您的Google Cloud资产的威胁。 

以下示例显示了一个已知的恶意角色（信誉低的源IP地址）试图获取在Google Kubernetes Engine（GKE）中运行的Pod列表时生成的警报：

```json
{
  "agent": {
      "id": "000",
      "name": "wazuh-manager-master"
  },
  "data": {
      "insertId": "b2c2e792-aaa9-4422-82d0-de32940b1234",
      "labels": {
          "authorization": {
              "k8s": {
                  "io/decision": "allow"
              }
          }
      },
      "logName": "projects/gke-audit-logs/logs/cloudaudit.googleapis.com%2Fdata_access",
      "operation": {
          "first": "true",
          "id": "b2c2e792-aaa9-4422-82d0-de32940b1234",
          "last": "true",
          "producer": "k8s.io"
      },
      "protoPayload": {
          "@type": "type.googleapis.com/google.cloud.audit.AuditLog",
          "authenticationInfo": {
              "principalEmail": "john.doe@email.com"
          },
          "authorizationInfo": [{
              "granted": true,
              "permission": "io.k8s.core.v1.pods.list",
              "resource": "core/v1/namespaces/default/pods"
          }],
          "methodName": "io.k8s.core.v1.pods.list",
          "requestMetadata": {
              "callerIp": "35.195.195.195",
              "callerSuppliedUserAgent": "kubectl/v1.18.6 (linux/amd64) kubernetes/dff82dc"
          },
          "resourceName": "core/v1/namespaces/default/pods",
          "serviceName": "k8s.io"
      },
      "receiveTimestamp": "2020-08-17T17:09:19.068723691Z",
      "resource": {
          "labels": {
              "cluster_name": "wazuh",
              "location": "us-central1-c",
              "project_id": "gke-audit-logs"
          },
          "type": "k8s_cluster"
      },
      "timestamp": "2020-08-17T17:09:05.043988Z"
  },
  "rule": {
      "description": "Malicious GKE request origin for io.k8s.core.v1.pods.list operation.",
      "id": "400003",
      "level": 10
  },
  "timestamp": "2020-08-17T17:09:25.832+0000"
}
```

More information on how to use Wazuh to monitor the Google cloud platform can be found in our [documentation](https://documentation.wazuh.com/4.0/gcp/index.html#gcp).

[BACK TO TOP](#%E7%94%A8%E4%BE%8B)

---

### 容器安全监控(Containers security monitoring)

Wazuh用于监视跨容器的安全事件的迹象，并实时发出警报。 Wazuh在两个不同级别上保护容器工作负载：

#### 基础设施级别

Wazuh提供以下机制来监视Docker主机或Kubernetes节点： 

- 与Docker引擎和Kubernetes API集成：在这种情况下，用于Docker的Wazuh模块充当订户。它侦听Docker或Kubernetes事件，能够在检测到异常或安全事件时发出警报。 

- 将 [Wazuh Agent](#wazuh-agent) 部署到Docker主机和Kubernetes节点：对于自我管理的基础架构， [Wazuh Agent](#wazuh-agent) 的部署提供了一套全面的安全功能，例如恶意软件检测，文件完整性监视，配置评估，日志数据分析，漏洞检测和积极回应。 

- 与托管基础架构提供商（例如Google GKE，Amazon EKS等）的集成：在这种情况下，用于云安全监控的Wazuh模块下载托管服务审核日志以进行安全分析。 
  
  Example of security alerts at an infrastructure level:
  
  | 基础结构级别的安全警报示例                                     | 基础结构级别的安全警报示例                                       |
  | ------------------------------------------------- | --------------------------------------------------- |
  | A Docker image is downloaded or updated           | A container is running in privileged mode           |
  | Kubernetes configuration is changed               | Hardening checks fail for the Docker host           |
  | A new container or Pod is created                 | A user runs a command or a shell inside a container |
  | A new application is installed on the Docker host | Vulnerabilities are detected on the Docker host     |

创建Docker容器时的警报示例：

```json
{
  "agent": {
      "id": "006",
      "ip": "10.0.1.214",
      "name": "RHEL7"
  },
  "data": {
      "docker": {
          "Action": "create",
          "Actor": {
              "Attributes": {
                  "image": "nginx",
                  "maintainer": "NGINX Docker Maintainers <docker-maint@nginx.com>",
                  "name": "nginx_container"
              },
              "ID": "e9730949586a38d8ab25990234fb8ccba428e3ec1c8bfbf12fe08ed279aaf11d"
          },
          "Type": "container",
          "from": "nginx",
          "id": "e9730949586a38d8ab25990234fb8ccba428e3ec1c8bfbf12fe08ed279aaf11d",
          "scope": "local",
          "status": "create",
          "time": "1599186260",
          "timeNano": "1599186260265422592.000000"
      }
  },
  "rule": {
      "description": "Docker: Container nginx_container created",
      "level": 3,
      "id": "87901"
  },
  "timestamp": "2020-09-04T02:24:20.266+0000"
}
```

在容器内执行命令时的警报示例：

```json
{
  "agent": {
      "id": "006",
      "ip": "10.0.1.214",
      "name": "RHEL7"
  },
  "data": {
      "docker": {
          "Action": "exec_start: cat /etc/passwd",
          "Actor": {
              "Attributes": {
                  "execID": "363d220ce7a34c521707477d14b7700e4fb26987f9f4e27bc558788ce66570b4",
                  "image": "nginx",
                  "maintainer": "NGINX Docker Maintainers <docker-maint@nginx.com>",
                  "name": "nginx_container"
              },
              "ID": "e9730949586a38d8ab25990234fb8ccba428e3ec1c8bfbf12fe08ed279aaf11d"
          },
          "Type": "container",
          "from": "nginx",
          "id": "e9730949586a38d8ab25990234fb8ccba428e3ec1c8bfbf12fe08ed279aaf11d",
          "scope": "local",
          "status": "exec_start: cat /etc/passwd",
          "time": "1599186799",
          "timeNano": "1599186799425748992.000000"
      }
  },
  "rule": {
      "description": "Docker: Command launched in container nginx_container",
      "level": 3,
      "id": "87907"
  },
  "timestamp": "2020-09-04T02:33:19.431+0000"
}
```

#### 容器级别

为了在容器级别获得可见性，您可以将 [Wazuh Agent](#wazuh-agent) 部署到Kubernetes DaemonSet容器。这种部署可确保 [Wazuh Agent](#wazuh-agent) 将在Kubernetes集群的所有节点中运行。此外，其他Kubernetes Pods将能够将数据（例如应用程序日志消息）发送到DaemonSet容器，以便 [Wazuh Agent](#wazuh-agent) 可以对其进行处理并将其转发到 [Wazuh Server](#wazuh-server) 以进行安全性分析。

| 容器级别的安全警报示例                              | 容器级别的安全警报示例                           |
| ---------------------------------------- | ------------------------------------- |
| New process created in a container       | File integrity monitoring alerts      |
| New application installed in a container | Vulnerability detected in a container |
| Log analysis alert (e.g. Nginx event)    | Hardening check failed in a container |

[BACK TO TOP](#%E7%94%A8%E4%BE%8B)

---

### 监管合规(Regulatory compliance)

合规性 Wazuh平台通常用于满足法规遵从标准的技术方面。 Wazuh不仅提供必要的安全控制（例如入侵检测，配置评估，日志分析，漏洞检测等）来满足合规性要求，而且还使用其SIEM功能集中，分析和丰富安全数据。 为了提供合规性支持，已将Wazuh规则与合规性要求进行了仔细映射。这样，当生成警报（匹配规则条件）时，它会自动包括合规性信息。这是当前支持的标准的列表： 支付卡行业数据安全标准（PCI DSS） 通用数据保护条例（GDPR） NIST特殊出版物800-53（NIST 800-53） 良好做法指南13（GPG13） 信托服务标准（TSC SOC2） 健康保险可移植性和责任法（HIPAA） 此外，Wazuh规则包括使用MITER ATT＆CK框架进行映射，该框架用于警报分类并提供更好的安全上下文。以下是用于检测对Apache Web服务器中禁止目录的访问的检测规则示例：

- Payment Card Industry Data Security Standard (PCI DSS)

- General Data Protection Regulation (GDPR)

- NIST Special Publication 800-53 (NIST 800-53)

- Good Practice Guide 13 (GPG13)

- Trust Services Criteria (TSC SOC2)

- Health Insurance Portability and Accountability Act (HIPAA)

此外，Wazuh规则包括使用MITER ATT＆CK框架进行映射，该框架用于警报分类并提供更好的安全上下文。以下是用于检测对Apache Web服务器中禁止目录的访问的检测规则示例：

```xml
<rule id="30306" level="5">
  <if_sid>30301</if_sid>
  <id>AH01276</id>
  <description>Apache: Attempt to access forbidden directory index.</description>
  <mitre>
    <id>T1190</id>
  </mitre>
  <group>access_denied,pci_dss_6.5.8,pci_dss_10.2.4,gdpr_IV_35.7.d,hipaa_164.312.b,nist_800_53_SA.11,nist_800_53_AU.14,nist_800_53_AC.7,tsc_CC6.6,tsc_CC7.1,tsc_CC6.1,tsc_CC6.8,tsc_CC7.2,tsc_CC7.3,</group>
</rule>
```

规则的示例警报 `Apache: Attempt to access forbidden directory index.`

```json
{
  "agent": {
      "id": "006",
      "ip": "10.0.1.214",
      "name": "RHEL7"
  },
  "data": {
      "id": "AH01276",
      "srcip": "24.4.35.192",
      "srcport": "61844"
  },
  "full_log": "[Fri Sep 04 06:08:51.973988 2020] [autoindex:error] [pid 28811] [client 24.4.35.192:61844] AH01276: Cannot serve directory /var/www/html/: No matching DirectoryIndex (index.html) found, and server-generated directory index forbidden by Options directive",
  "location": "/var/log/httpd/error_log",
  "rule": {
      "description": "Apache: Attempt to access forbidden directory index.",
      "level": 5,
      "id": "30306",
      "mitre": {
          "id": [
              "T1190"
          ],
          "tactic": [
              "Initial Access"
          ],
          "technique": [
              "Exploit Public-Facing Application"
          ]
      },
      "gdpr": [
          "IV_35.7.d"
      ],
      "hipaa": [
          "164.312.b"
      ],
      "nist_800_53": [
          "SA.11",
          "AU.14",
          "AC.7"
      ],
      "pci_dss": [
          "6.5.8",
          "10.2.4"
      ],
      "tsc": [
          "CC6.6",
          "CC7.1",
          "CC6.1",
          "CC6.8",
          "CC7.2",
          "CC7.3"
      ]
  },
  "timestamp": "2020-09-04T06:08:53.878+0000"
}
```

PCI DSS的法规遵从性仪表板示例：

![1](_resources/use_case_regulatory_compliance.png)

[BACK TO TOP](#%E7%94%A8%E4%BE%8B)