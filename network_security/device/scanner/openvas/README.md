# OpenVAS - Open Vulnerability Assessment Scanner

| 名词   | 缩写展开                               | 备注                        |
| ---- | ---------------------------------- | ------------------------- |
| feed |                                    | 大义是指用户想看的有用信息，能够满足用户需求的信息 |
| NVTs | Network Vulnerability Tests        | 网络漏洞探测                    |
| GVM  | Greenbone Vulnerability Management | 多种服务组成的漏洞管理服务框架           |
| OSP  | Open Scanner Protocol              | 开放扫描器协议                   |
| GMP  | Greenbone Management Protocol      | Greenbone管理协议             |
| GSA  | Greenbone Security Assistant       | GVM的web UI                |

## OpenVAS 简介

OpenVAS(开放式漏洞评估扫描程序)是功能齐全的漏洞扫描程序。它的功能包括 未经身份验证的测试(unauthenticated testing)，经过身份验证的测试(authenticated testing)，各种高级和低级Internet和工业协议，针对大规模扫描的性能调整以及用于实施任何类型的漏洞测试的强大内部编程语言(nasl)。

扫描器(scanner)拥有悠久历史(自2009年开发维护至今)，并且每日更新。社区版本包含80,000多个漏洞的测试能力，使用 GPL 许可。

Greenbone开发OpenVAS作为其<mark>商业漏洞管理产品系列 Greenbone Security Manager(GSM) </mark>的一部分。 OpenVAS是较大体系结构中的一个元素。结合其他开源模块，它构成了Greenbone漏洞管理解决方案。

----

## OpenVAS 发展历史

2005年，漏洞扫描程序 Nessus 的开发人员决定终止在开源许可下的工作，完全商业化。来自 Intevation 和 DN-Systems （两家公司后来创建了Greenbone Networks）的开发人员已经为Nessus 项目做出了许多贡献，这些工作主要由德国联邦信息安全局（BSI）支持。

2006年，Nessus 创建了多个分支，作为对开源解决方案停产的一种反应。在这些分支中只有 OpenVAS 存活了下来，并保持开源。

2008年，Greenbone Networks 成立，全力推动OpenVAS的发展。

本质上，Greenbone的商业计划大约是三个基石：

1. 从简单的漏洞扫描到全面的漏洞管理解决方案。

2. 为企业客户创建整套设备产品。

3. 继续创建透明安全技术的开源概念。

同样在2008年，又有两家公司活跃起来，分别是印度的Secpod和加拿大的Security Space。他们都致力于漏洞测试，并与Greenbone合作开始制作可靠且最新的漏洞测试。首先删除许可不明确或不兼容的所有源代码和漏洞测试，为此删除了了数千个漏洞测试。

2009年，Greenbone添加了第一个附加模块来构建漏洞管理解决方案。 Web界面和中央管理服务是从零开始开发的，将通用协议定义为其API。同时，对OpenVAS扫描仪器行了改进，与原来的祖先 Nessus 变得不兼容。所有开源作品均以 "OpenVAS" 品牌出版。首款  Greenbone Security Manager(GSM) 设备产品于2010年春季进入市场。

从2010年到2016年，商业产品得到了系统性的改进和扩展，开放源代码模块也得到了改进。漏洞管理已扩展为每日更新安全公告，并获得了德国CERT DFN-CERT和CERT-Bund的GPL兼容许可证，该许可证是联邦信息安全局（BSI）的一个部门，曾在各种方面支持OpenVAS。

2017年3月，OpenVAS发 展到版本9。在发行周期中添加了许多新模块和众多功能。

2017年，标志着新时代的开始：首先，Greenbone 成为OpenVAS背后的推动力，减少了品牌混乱。其中包括几项活动，最重要的一项是将“ OpenVAS框架”重命名为“ Greenbone漏洞管理”（GVM），其中，OpenVAS扫描程序是许多模块之一。

2017年的第二个重大变化与 feed 服务有关。除了品牌混淆之外，还有几家公司整合技术并将其作为自己的产品进行推广，他们中只有少数人正确遵守了GPL许可证。他们都没有与Greenbone商业合作。为了获得更好的可见性，减少误解并更好地与其他基于OpenVAS的产品区分开来，将 public feed 重命名为 Greenbone Community Feed，并且对 feed 开发进行了内部化。此外，发布计划从14天的延迟发布改为每日发布。

迈向新时代的第三大变化是向现代基础架构的过渡，即GitHub和社区论坛。整个过渡于2018年完成，并提高了生产力和社区活动。

在2019年，品牌分离完成了。现在，OpenVAS 代表实际的漏洞扫描程序，与以前一样，并且 OpenVAS 中的 S 现在代表的是 扫描器 而不是 系统。这些更改伴随着更新的OpenVAS徽标。嵌入OpenVAS的框架是Greenbone漏洞管理（GVM）。

随GVM-10一起发布的OpenVAS进行了许多性能优化，以应对越来越多的漏洞测试，扫描越来越大的规模和异构性的目标网络的挑战。

与GVM-11一起发布的OpenVAS引入了重大的体系结构更改：以前的服务“ openvassd”变成了命令行工具“ openvas”。 它由服务层ospd-openvas控制。 该概念实质上是通过新的基于XML的无状态，请求响应和通用的OSP（开放式扫描仪协议）代替了旧的有状态，永久和专有的OTP（OpenVAS传输协议）。

-----

## GVM 架构

> [gvm-20-08-architecture](https://community.greenbone.net/t/about-gvm-20-08-architecture/8449)

Greenbone 漏洞管理(GVM)是一个框架， 它由

- Greenbone 漏洞管理器后台程序(gvmd）

- Greenbone 安全助手(GSA）

- Greenbone 安全助手后台程序（gsad）

- 针对目标系统运行漏洞测试的扫描应用程序

![架构](_resources/openvas-arch-20.08.png)

各个组件

![组件](_resources/openvas-modules.svg)



### Greenbone Vulnerability Manager Daemon (gvmd)

Greenbone Vulnerability Manager 是中央服务，可将纯漏洞扫描整合为完整的漏洞管理解决方案。 gvmd 通过OSP协议控制 OpenVAS Scanner。gvmd还控制所有配置和扫描结果数据集中存储的SQL数据库(PostgreSQL)。 此外，gvmd还处理用户管理，包括使用组和角色进行权限控制。 最后，该服务还具有计划任务和处理其他事件的能力。

> [greenbone/gvmd: Greenbone Vulnerability Manager](https://github.com/greenbone/gvmd)

### 

### Greenbone Security Assistant (GSA)

Greenbone Security Assistant（GSA）是GVM的Web界面，用户可以控制扫描和访问漏洞信息。 它是使用GVM的用户的主要联系点。 它通过Web服务器Greenbone Security Assistant守护程序（gsad）连接到gvmd，以提供功能齐全的Web应用程序进行漏洞管理。 使用Greenbone管理协议（GMP）进行通信，用户也可以使用不同的工具直接与之通信。

GSA - 用 React 编写的Web应用程序
GSAD-通过GMP协议与GVMd通信的的HTTP服务器

> [greenbone/gsa: Greenbone Security Assistant](https://github.com/greenbone/gsa)

### 

### OpenVAS Scanner

主扫描程序OpenVAS扫描程序是功能齐全的扫描引擎，它能够执行网络漏洞测试(NVT)的持续更新和扩展的 feed。 有两个 feed 每天都会更新，功能齐全的商业 Greenbone Security Feed(GSF)和免费的 Greenbone Community Feed (GCF)。

OpenVAS扫描仪是通过OSP协议控制的。 OpenVAS扫描程序的OSP守护程序（ospd-openvas）通过OSP与gvmd通信：收集VT数据，启动和停止扫描，并将扫描结果通过ospd传输到gvmd。

> [greenbone/openvas-scanner](https://github.com/greenbone/openvas-scanner)

### 

### OSP Scanner

用户可以使用通用的 ospd 扫描器框架开发和连接自己的OSP扫描器。 可以在[此处](https://github.com/greenbone/ospd-example-scanner)找到可用作OSP扫描器模板的（通用）OSP扫描器示例。

### 

### GMP Clients

Greenbone漏洞管理工具（gvm-tools）是一组工具，可帮助远程控制 GSM设备及其基础的 gvmd。 这些工具有助于访问通信协议GMP（Greenbone管理协议）和OSP（开放式扫描仪协议）。该模块由交互式和非交互式客户端组成。交互式脚本直接支持Python编程语言。但是，也可以在不使用Python进行编程的情况下发出远程GMP/OSP命令。

> [API_documentation](https://docs.greenbone.net/#api_documentation)
> 
> [greenbone/python-gvm: Greenbone Vulnerability Management Python API library](https://github.com/greenbone/python-gvm)
