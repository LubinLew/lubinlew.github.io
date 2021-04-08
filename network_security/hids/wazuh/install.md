# 安装

## 安装要求

本节提供有关受支持的操作系统以及Wazuh安装的不同部署类型的建议硬件要求的指南。

<mark>必须使用64位操作系统。</mark>

### 支持的操作系统

azuh服务器和Elastic Stack组件可以安装在以下Linux操作系统中：

- Amazon Linux 1 and 2.

- CentOS 6 or greater.

- Debian 7 or greater.

- Fedora 31 or greater.

- Oracle Linux 6 or greater.

- Red Hat Enterprise Linux 6 or greater.

- Ubuntu 12 or greater.

### 多合一部署

在多合一部署中，Wazuh服务器和Elastic Stack都安装在同一主机上。这种类型的部署适用于测试和小型生产环境。这种环境的典型用例支持大约100个 `Agent`。 <mark>此类部署的最低要求是2核4G，建议使用8核16G配置</mark>。 磁盘空间要求取决于生成的每秒警报数量(APS)。预期的APS取决于受监视端点的数量和类型而有很大差异。下表提供了90天警报所需的每个`Agent`的存储量估计值，具体取决于受监视端点的类型。

| Monitored endpoints | APS  | Storage (GB/90 days) |
| ------------------- | ---- | -------------------- |
| Servers             | 0.25 | 3.8                  |
| Workstations        | 0.1  | 1.5                  |
| Network devices     | 0.5  | 7.6                  |

例如，对于具有80个工作站，10个服务器和10个网络设备的环境，90天警报所需的存储空间约为236 GB。

----

### 分布式部署

在分布式部署中，Wazuh Server 和 Elastic Stack 都安装在单独的主机上。生产环境建使用此配置，因为它可提供服务的高可用性和可伸缩性。 Wazuh Server 和 Elastic Stack 可以分别安装为单节点或多节点群集。 Kibana 可以安装在与 Elasticsearch 相同的节点上，也可以安装在专用主机中。对于每个节点，硬件建议为：

| Minimum       |          |             | Recommended |             |
| ------------- | -------- | ----------- | ----------- | ----------- |
| Component     | RAM (GB) | CPU (cores) | RAM (GB)    | CPU (cores) |
| Wazuh server  | 2        | 2           | 8           | 4           |
| Elastic Stack | 4        | 2           | 16          | 8           |

关于磁盘空间要求，数据量取决于每秒生成的警报（APS）。下表显示了在 Wazuh Server 以及 Elasticsearch 服务器上存储90天警报所需的每个代理所需的磁盘空间的估计，具体取决于受监视端点的类型。

| Monitored endpoints | APS  |    Storage in Wazuh Manager<br>(GB/90 days)  | Storage in Elasticsearch<br>(GB/90 days)     |
| ------------------- | ---- | -------------------------------------------- | -------------------------------------------- |
| Servers             | 0.25 | 0.1                                          | 3.7                                          |
| Workstations        | 0.1  | 0.04                                         | 1.5                                          |
| Network devices     | 0.5  | 0.2                                          | 7.4                                          |

例如，对于具有80个工作站，10个服务器和10个网络设备的环境，90天警报所需的存储在Elasticsearch服务器上约为230 GB，在Wazuh服务器上约为6 GB。

### 弹性伸缩

为了确定 Wazuh server 是否需要更多资源，可以监视以下文件：

- `/var/ossec/var/run/ossec-analysisd.state`
- `/var/ossec/var/run/ossec-remoted.state`

 在 `analysid.state` 文件中，变量 `events_dropped` 指示是否由于缺少资源而删除了事件。

类似地，`ossec-remoted.state` 具有变量 `discarded_count`，该变量指示是否已丢弃来自`Agent`的消息。如果环境运行正常，这两个变量应为零。如果不是这种情况，则可以将其他节点添加到群集中。

要监视 Elastic Stack 环境是否正常运行，可以使用诸如性能分析器之类的工具。 如果需要扩展，以下各节介绍如何使用 Elastic Stack 进行 Wazuh 的分布式部署。



## 部署各个组件

[部署Wazuh Server](network_security/hids/wazuh/install_server.md)

[部署Wazuh Agent](network_security/hids/wazuh/install_agent.md)
