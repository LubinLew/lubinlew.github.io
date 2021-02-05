# 安装

## 安装要求

本节提供有关受支持的操作系统以及Wazuh安装的不同部署类型的建议硬件要求的指南。

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

在多合一部署中，Wazuh服务器和Elastic Stack都安装在同一主机上。这种类型的部署适用于测试和小型生产环境。这种环境的典型用例支持大约100个代理。 此类部署的最低要求是4 GB的RAM和2个CPU内核，建议的最低要求是16 GB的RAM和8个CPU内核。需要64位操作系统。 磁盘空间要求取决于生成的每秒警报（APS）。预期的APS取决于受监视端点的数量和类型而有很大差异。下表提供了90天警报所需的每个代理的存储量估计值，具体取决于受监视端点的类型。

| Monitored endpoints | APS  | Storage (GB/90 days) |
| ------------------- | ---- | -------------------- |
| Servers             | 0.25 | 3.8                  |
| Workstations        | 0.1  | 1.5                  |
| Network devices     | 0.5  | 7.6                  |

例如，对于具有80个工作站，10个服务器和10个网络设备的环境，90天警报所需的存储空间约为236 GB。

### 分布式部署

在分布式部署中，Wazuh服务器和Elastic Stack都安装在单独的主机上。建议将此配置用于生产环境，因为它可提供服务的高可用性和可伸缩性。 Wazuh服务器和Elastic Stack可以分别安装为单节点或多节点群集。 Kibana可以安装在与Elasticsearch相同的节点上，也可以安装在专用主机中。对于每个节点，硬件建议为：

| Minimum       |          |             | Recommended |             |
| ------------- | -------- | ----------- | ----------- | ----------- |
| Component     | RAM (GB) | CPU (cores) | RAM (GB)    | CPU (cores) |
| Wazuh server  | 2        | 2           | 8           | 4           |
| Elastic Stack | 4        | 2           | 16          | 8           |

<mark>必须使用64位操作系统</mark>。 关于磁盘空间要求，数据量取决于每秒生成的警报（APS）。下表显示了在Wazuh服务器以及Elasticsearch服务器上存储90天警报所需的每个代理所需的磁盘空间的估计，具体取决于受监视端点的类型。

| Monitored endpoints | APS  | Storage in Wazuh Manager<br><br>(GB/90 days) | Storage in Elasticsearch<br><br>(GB/90 days) |
| ------------------- | ---- | -------------------------------------------- | -------------------------------------------- |
| Servers             | 0.25 | 0.1                                          | 3.7                                          |
| Workstations        | 0.1  | 0.04                                         | 1.5                                          |
| Network devices     | 0.5  | 0.2                                          | 7.4                                          |

例如，对于具有80个工作站，10个服务器和10个网络设备的环境，90天警报所需的存储在Elasticsearch服务器上约为230 GB，在Wazuh服务器上约为6 GB。



### Scaling

为了确定Wazuh服务器是否需要更多资源，可以监视以下文件：`/var/ossec/var/run/ossec-analysisd.state`和`/var/ossec/var/run/ossec-remoted.state`。 在`analysid.state`文件中，变量`events_dropped`指示是否由于缺少资源而删除了事件。类似地，`ossec-remoted.state`具有变量`discarded_count`，该变量指示是否已丢弃来自代理的消息。如果环境运行正常，这两个变量应为零。如果不是这种情况，则可以将其他节点添加到群集中。 要监视Elastic Stack环境是否正常运行，可以使用诸如性能分析器之类的工具。 如果需要扩展，以下各节介绍如何使用Elastic Stack进行Wazuh的分布式部署。

----

## Wazuh server 安装

本部分为用户提供了必要的信息，以使用Open Distro for Elasticsearch进行Wazuh的安装，Elasticsearch是Apache 2.0许可的发行版，通过Elastic Security提供了企业安全性，警报，SQL支持，自动索引管理或深度性能分析等增强功能。其他特性。在这里访问我们，以了解有关Wazuh组件的更多信息。

### 部署类型

根据选择的配置，安装指南分为两个独立的部分：多合一部署和分布式部署。 此处提供了有关不同部署类型的最低硬件要求的信息。 

- 多合一部署：Wazuh和Open Distro for Elasticsearch安装在同一主机上。这种类型的部署适用于测试和小型工作环境。 

- 分布式部署：每个组件都作为单节点或多节点群集安装在单独的主机中。这种类型的部署允许产品的高可用性和可伸缩性，并且在大型工作环境中很方便。

### 安装方式

对于每种部署类型，用户可以在两种安装方法之间进行选择： 

无人值守：自动安装。它需要初始输入必要的信息才能通过脚本执行安装过程。

 逐步说明：手动安装。包括安装过程中每个步骤的详细说明。

### 使用Open Distro为Elasticsearch部署Wazuh

#### 多合一部署

本节指导在同一主机上安装和配置Wazuh服务器和Elastic Stack。这种类型的部署适用于测试和小型生产环境。

![1](_resources/all_in_one_no_title4.png)

将安装以下组件： 

- Wazuh服务器，包括作为单节点群集的Wazuh管理器和Wazuh API。

-  Elastic Stack，包括作为单节点群集的OpenDistro for Elasticsearch，Filebeat和Kibana，包括Wazuh Kibana插件。 

通信将使用SSL证书进行加密。这些证书将使用Search Guard脱机TLS工具生成。 另外，为了正确使用Wazuh Kibana插件，将添加额外的Elasticsearch角色和用户。 为了保证Wazuh组件的预期性能，主机必须满足要求部分中描述的硬件要求。 用户可以选择逐步安装，手动执行该过程的方式或无人参与安装，使用脚本的自动方式： 无人值守安装 分步安装

##### 无人值守安装

文档的此部分说明如何使用脚本自动在单个主机上安装Wazuh，该脚本将自动检测操作系统是否使用rpm或deb软件包。该脚本将执行运行状况检查，以验证可用的系统资源是否满足最低要求。有关更多信息，请访问需求部分。 该脚本将安装Java Development Kit和其他软件包，包括Open Distro for Elasticsearch所需的unzip和libcap。此外，Search Guard离线TLS工具将用于生成证书，以保护Elastic Stack中的数据。

###### 安装Wazuh

> Root user privileges are required to run all the commands described below. The `curl` package will be used to download the script.

**1.Download and run the script**

```bash
curl -so ~/all-in-one-installation.sh https://raw.githubusercontent.com/wazuh/wazuh-documentation/4.0/resources/open-distro/unattended-installation/all-in-one-installation.sh && bash ~/all-in-one-installation.sh
```

该脚本将执行运行状况检查，以确保主机具有足够的资源来保证适当的性能。要跳过此步骤，请在运行脚本时添加-i或--i​​gnore-healthcheck选项。 执行脚本后，它将显示以下消息，以确认安装成功：

```bash
Installing all necessary utilities for the installation...
Done
Adding the Wazuh repository...
Done
Installing the Wazuh manager...
Done
Wazuh-manager started
Installing Open Distro for Elasticsearch...
Done
Configuring Elasticsearch...
Certificates created
Elasticsearch started
Initializing Elasticsearch...
Done
Installing Filebeat...
Filebeat started
Done
Installing Open Distro for Kibana...
Kibana started
Done
Checking the installation...
Elasticsearch installation succeeded.
Filebeat installation succeeded.
Initializing Kibana (this may take a while)
########
Installation finished
```

**2.访问Web界面**

```bash
URL: https://<wazuh_server_ip>
user: admin
password: admin
```

首次访问Kibana时，浏览器显示警告消息，指示证书不是由受信任的机构颁发的。可以在浏览器的高级选项中添加例外，或者为了提高安全性，可以将以前生成的root-ca.pem文件导入浏览器的证书管理器中。或者，可以配置来自可信机构的证书。

###### 定制安装

在/etc/kibana/kibana.yml文件中找到的Kibana配置将server.host参数设置为0.0.0.0。这意味着可以从外部访问Kibana，并将接受主机的所有可用IP。如果需要，可以为特定IP更改此值。 强烈建议为/usr/share/elasticsearch/plugins/opendistro_security/securityconfig/internal_users.yml文件中找到的用户更改Elasticsearch的默认密码。有关此过程的更多信息，请参见此处。 Kibana运行之后，有必要为每个用户分配相应的角色。要了解更多信息，请访问“设置Wazuh Kibana插件”部分。 要在一次安装中卸载全部组件的所有组件，请访问卸载部分。

###### 下一步

一旦Wazuh环境准备就绪，就可以在每个要监视的端点中安装Wazuh代理。要了解更多信息，请访问Wazuh代理安装指南，该指南可用于大多数操作系统。
