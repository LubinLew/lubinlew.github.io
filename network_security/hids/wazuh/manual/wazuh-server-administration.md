# Wazuh Server 管理

Wazuh Manager 是一个分析从所有注册的 `Agent` 接收到的数据，并在事件符合规则时触发警报的系统，还充当本地计算机上的 `Agent`，这意味着它具有 `Agent` 具有的所有功能。此外，管理员可以通过系统日志，电子邮件或集成的外部API转发其触发的警报。

## 远程服务(Remote service)

> https://documentation.wazuh.com/current/user-manual/manager/remote-service.html

可以将 Wazuh Manager 配置为发布为 `Agent` 所使用的远程服务，如下所示：

### 配置

远程服务的所有配置在 `ossec.conf` 文件中, 在 xml 的\<remote\> 段中。可用选项在[远程配置](network_security/hids/wazuh/manual/reference/ossec.conf/remote.md) 中查阅。您可以使用以下配置来更改用于侦听服务的IP地址：

```xml
<ossec_config>
  <remote>
    <local_ip>10.0.0.10</local_ip>
  </remote>
</ossec_config>
```

这将设置 Wazuh Manager 监听IP地址 `10.0.0.10`,更改 `ossec.conf` 文件中的任何值时，都必须重新启动服务才会生效。

```bash
## For Systemd
systemctl restart wazuh-manager

## For SysV Init
service wazuh-manager restart
```

----

## 定义警报级别阈值

`Agent` 收集的每个事件(event)都将传输到 `Wazuh Manager`。`Wazuh Manager`将根据事件与规则集中匹配的规则为事件分配严重性级别。默认情况下，它将仅记录严重性级别为3或更高的警报。

### 配置

警报级别阈值是在 `ossec.conf` 文件使用\<alerts\>标记段配置的。详细设置见[alerts](https://documentation.wazuh.com/current/user-manual/reference/ossec-conf/alerts.html#reference-ossec-alerts)。

```xml
<ossec_config>
  <alerts>
      <log_alert_level>6</log_alert_level>
  </alerts>
</ossec_config>
```

这将设置将触发警报的最低严重性级别，警报将存储在`alerts.log` 和/或 `alerts.json`文件中。

如果文件中的任何值发生更改`ossec.conf`，则必须重新启动服务才能使更改生效。

----

## 与外部API集成

该服务(Integrator daemon)允许 Wazuh 连接到外部的API和报警工具，如Slack、PagerDuty、VirusTotal等。

### 配置

与外部API集成是在 `ossec.conf` 文件使用\<integration\>标记段配置的。

```xml
<integration>
  <name> </name>
  <hook_url> </hook_url> <!-- Required for Slack -->
  <api_key> </api_key> <!-- Required for PagerDuty and VirusTotal -->

  <!-- Optional filters -->
  <rule_id> </rule_id>
  <level> </level>
  <group> </group>
  <event_location> </event_location>
</integration>
```

重新启动服务 `wazuh-manager` 使更改生效。


### 外部API示例

可以在此处找到有关继承外部API完整配置参考。

#### Slack

这种集成允许通过[Incoming Webhooks](https://api.slack.com/messaging/webhooks)(这是一种简单的将第三方应用的数据发送的slack中)将警报接收到Slack频道中。
与Slack集成的示例配置：

```xml
<integration>
  <name>slack</name>
  <hook_url>https://hooks.slack.com/services/...</hook_url> <!-- Replace with your Slack hook URL -->
  <alert_format>json</alert_format>
</integration>
```

#### PagerDuty

PagerDuty是适用于IT部门的SaaS事件响应平台。通过此集成，可以使用其官方API创建服务，以便在事件仪表板上接收Wazuh警报。

这是PagerDuty集成的示例配置：

```xml
<integration>
  <name>pagerduty</name>
  <api_key>API_KEY</api_key> <!-- Replace with your PagerDuty API key -->
</integration>
```

#### VirusTotal

> 3.0.0版中的新功能。

这种集成允许使用VirusTotal数据库检查恶意文件。在[VirusTotal集成](https://documentation.wazuh.com/current/user-manual/capabilities/virustotal-scan/index.html#virustotal-scan)页面上找到有关此信息的更多信息。

这是VirusTotal集成的示例配置：

```xml
<integration>
  <name>virustotal</name>
  <api_key>API_KEY</api_key> <!-- Replace with your VirusTotal API key -->
  <group>syscheck</group>
  <alert_format>json</alert_format>
</integration>
```

### 自定义集成

集成器工具能够将wazuh与其他外部软件连接。阅读[如何使用Integrator集成外部软件](https://wazuh.com/blog/how-to-integrate-external-software-using-integrator//)文档以获取更多信息。

这是定制集成的示例配置：

```xml
<!--Custom external Integration -->
<integration>
  <name>custom-integration</name>
  <hook_url>WEBHOOK</hook_url>
  <level>10</level>
  <group>multiple_drops|authentication_failures</group>
  <api_key>APIKEY</api_key> <!-- Replace with your external service API key -->
  <alert_format>json</alert_format>
</integration>
```

----

## 配置syslog输出

Wazuh可以配置为将警报发送到syslog，是在 `ossec.conf` 文件使用\<syslog_output\>标记段配置的,详细配置方式见 [syslog_output](https://documentation.wazuh.com/current/user-manual/reference/ossec-conf/syslog-output.html#reference-ossec-syslog-output)

配置示例如下所示：

```xml
<ossec_config>
  <syslog_output>
    <level>9</level>
    <server>192.168.1.241</server>
  </syslog_output>

  <syslog_output>
    <server>192.168.1.240</server>
  </syslog_output>
</ossec_config>
```

上面的配置将向发`192.168.1.240`送警报，并且，如果警报级别高于9，也会向`192.168.1.241`发送警报。

----

## 配置数据库输出

> https://documentation.wazuh.com/current/user-manual/manager/manual-database-output.html

可以将 Wazuh 配置为将警报输出到数据库中。为此，用户必须指定要使用的数据库类型编译Wazuh。目前支持 MySQL 和 PostgreSQL 数据库。

### 安装数据库开发包

```bash
# centos
yum install mysql-devel
yum install postgresql-devel

# debian
apt-get install libmysqlclient-dev
apt-get install libpq-dev
```

### 编译

```bash
cd wazuh-*/src
make deps && make TARGET=server DATABASE=<mysql/pgsql>
```

----

## 生成自动报告

> https://documentation.wazuh.com/current/user-manual/manager/automatic-reports.html


----

## 配置电子邮件警报

> https://documentation.wazuh.com/current/user-manual/manager/manual-email-report/index.html

