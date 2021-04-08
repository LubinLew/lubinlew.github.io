# 概述

Wazuh 是一个开源项目，提供安全可见性，合规性和基础设施(infrastructure)监控功能。该项目诞生于OSSEC HIDS的分支，并通过实施新功能并集成了诸如 Elasticsearch 之类的其他工具，已发展成为一个全面的解决方案。

本手册介绍了如何配置和使用每个 Wazuh 组件，其中包括[Wazuh Agent](network_security/hids/wazuh/start?id=wazuh-agent)，[Wazuh Server](network_security/hids/wazuh/start?id=wazuh-server)和 [Elastic Stack](network_security/hids/wazuh/start?id=elastic-stack)。

## Wazuh Server

Wazuh服务器基于一组应用程序，其中每个应用程序或组件都旨在完成特定任务。这些组件共同作用于：

- 分析接收到的各种日志中的数据
- 当日志事件与规则匹配时触发警报
- 注册新的 `Agent`
- 将数据发送到Elastic Stack

### 组件

#### Wazuh Manager

接收并分析来自 `Agent` 的数据，这些数据需要使用解码器(decoder)处理, 并匹配规则来触发安全警报。还用于将配置文件分发给 `Agent`，监视其状态并发送控制消息以触发 `Agent` 级别的自动操作。

#### 注册服务

该注册服务使用安全机制，而 `Server` 不会做任何干预。

#### RESTful API

该 RESTful API 提供了管理和监控 [Wazuh Manager](#wazuh-manger) 和 `Agent` 的配置。它可用于注册 `Agent`，检查 [Wazuh Manager](#wazuh-manger) 的日志消息，解码器和规则，并提供与 `Agent` 相关的有用信息，包括它们的状态，操作系统详细信息以及与文件完整性监视和 rootcheck 有关的警报。

#### Filebeat

Filebeat 用于将警报数据从 Wazuh 管理器转发到 Elasticsearch。该组件具有自己的[官方文档](https://www.elastic.co/guide/en/beats/filebeat/current/filebeat-overview.html)。

----

## Elastic Stack
Elastic Stack 用于索引，浏览和可视化 Wazuh 警报数据。此外，用于 Kibana 的 Wazuh 应用程序可用于可视化配置设置，规则，解码器和 [Wazuh Agent](network_security/hids/wazuh/start?id=wazuh-agent) 状态信息。用于此可视化的仪表板包括但不限于策略，合规性和文件完整性监视。

### 组件

#### Wazuh app

是一个 Kibana 插件, 用来显示 Wazuh 关联信息, 并提供一个RESTful API的 web 接口。该界面使 [Wazuh Manager](#wazuh-manger) 和 [Wazuh Agent](network_security/hids/wazuh/start?id=wazuh-agent) 的管理变得轻松而强大。

#### Elasticsearch

是一个高度可扩展的全文本搜索和分析引擎。它用于索引警报数据和 `Agent` 状态信息的历史数据。更多信息可以在[官方文档](https://www.elastic.co/guide/en/elasticsearch/reference/current/index.html)中找到。

#### Kibana

Kibana 是一个灵活，直观的Web界面，用于挖掘，分析和可视化数据。结合我们的 Wazuh Kibana 应用程序，它可用作 Wazuh Web 用户界面（WUI）。可以在[官方文档](https://www.elastic.co/guide/en/kibana/current/index.html)中找到更多信息。

## Wazuh Agent

运行于受监控的系统上，负责收集日志和事件数据，执行策略监控扫描，检测恶意软件和rootkit以及在修改受监控的文件时触发警报。它通过加密并经过身份验证的通道与 [Wazuh Server](network_security/hids/wazuh/start?id=wazuh-server) 通信。

### 组件

#### Rootcheck

在每个系统上执行rootkit和恶意软件检测。

#### Log monitoring/analysis

收集并分析系统日志，以查找任何可疑活动。

#### Syscheck

Syscheck 定期运行以检查对任何已配置文件（或Windows上的注册表项）的更改。