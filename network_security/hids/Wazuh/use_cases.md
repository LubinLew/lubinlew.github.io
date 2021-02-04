# 用例

Wazuh平台用于以不同方式保护和监视系统。由于其所有功能，它通常用于威胁预防，检测和响应。此外，Wazuh平台用于满足法规遵从性要求（例如PCI DSS或HIPAA）和配置标准（CIS加固指南）。 Wazuh是IaaS用户（例如Amazon AWS，Azure或Google云）中一种流行的安全解决方案，用于监视虚拟机和云实例。这是在系统级别利用Wazuh安全代理完成的，而在基础架构级别则是直接从云提供商的API中提取数据。 此外，Wazuh通常用于保护提供云原生运行时安全性的容器化环境。此功能基于与Docker引擎API和Kubernetes API的集成。此外，为了获得更好的保护，Wazuh安全代理可以在Docker主机上运行，​​从而提供了完整的威胁检测和响应功能。 在文档的此部分中，您将找到Wazuh解决方案的一些更常见用例的简短示例。

| 用例列表                                                     | 用例列表                                               |
| -------------------------------------------------------- | -------------------------------------------------- |
| 1.[Log data analysis](#日志数据分析(Log-data-analysis))        | 2.[File integrity monitoring](#file-integrity)     |
| 3.[Rootkits detection](#rootkits-detection)              | 4.[Active response](#active-response)              |
| 5.[Configuration assessment](#configuration-assessment)  | 6.[System inventory](#system-inventory)            |
| 7.[Vulnerability detection](#vulnerability-detection)    | 8.[Cloud security monitoring](#cloud-security)     |
| 9.[Containers security monitoring](#containers-security) | 10.[Regulatory compliance](#regulatory-compliance) |



## 日志数据分析(Log data analysis)

自动化的日志管理和分析可加速威胁检测。在许多情况下，可以在设备，系统和应用程序的日志消息中找到攻击的证据。 Wazuh可用于自动聚合和分析日志数据。 运行在受监视端点上的Wazuh代理通常负责读取操作系统和应用程序日志消息，并将其转发到进行分析的Wazuh服务器。如果未部署代理，则服务器还可以通过syslog从网络设备或应用程序接收数据。 Wazuh使用解码器来识别日志消息的源应用程序。然后，它使用特定于应用程序的规则分析数据。这是用于检测SSH身份验证失败事件的规则的示例：

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

规则包括一个`match`字段，用于定义规则将要查找的模式。还有一个`level`字段，用于指定结果警报的优先级。此外，规则还利用Mitre ATT＆CK框架中的技术标识符以及映射到法规遵从性控制来丰富事件。 每当代理之一或通过syslog收集的事件与优先级高于预定义阈值（默认值为3）的规则匹配时，管理器都会生成警报。 这是在`/var/ossec/logs/alerts/alerts.json`中找到的示例（为简洁起见，某些字段已删除）：

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

Wazuh服务器一旦生成警报，便会将警报发送到Elastic Stack组件，在其中将其丰富的地理位置信息进行存储和索引。然后，可以使用Kibana搜索，分析和可视化数据。以下是界面中显示的警报：

![报警](_resources/use_case_log_analysis.png)



Wazuh提供了一个默认规则集，该规则集会定期更新，其中包含针对不同系统和应用程序的3,000多个规则。此外，Wazuh允许创建自定义规则。在用户手册中查找更多信息。

---

## 文件完整性监控(File integrity monitoring)

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

## rootkit检测(Rootkits detection)

Wazuh代理会定期扫描受监视的系统，以检测内核和用户空间级别的rootkit。这种恶意软件通常会替换或更改现有操作系统的组件，以更改系统行为。 Rootkit可以隐藏其他进程，文件和网络连接。 Wazuh使用不同的检测机制来搜索系统异常或众所周知的入侵。 Rootcheck组件定期执行此操作：

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

```


