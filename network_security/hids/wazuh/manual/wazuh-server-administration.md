# Wazuh Server 管理

Wazuh Manager 是一个分析从所有注册的 `Agent` 接收到的数据，并在事件符合规则时触发警报的系统，还充当本地计算机上的 `Agent`，这意味着它具有 `Agent` 具有的所有功能。此外，管理员可以通过系统日志，电子邮件或集成的外部API转发其触发的警报。

## 远程服务(Remote service)

> https://documentation.wazuh.com/current/user-manual/manager/remote-service.html

可以将 Wazuh Manager 配置为发布代理使用的远程服务，如下所示：

### 配置

远程服务的所有配置在 `ossec.conf` 文件中, 在 xml 的<remote> 段中。可用选项在[远程配置](network_security/hids/wazuh/manual/reference/ossec.conf/remote.md) 中查阅。您可以使用以下配置来更改用于侦听服务的IP地址：

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