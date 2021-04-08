# command

XML section name

```xml
<command>
</command>
```

该段用来定义一个命令, 这个命令会被一个或多个主动响应所使用。 一个主动响应使用的命令数量是没有限制的。但是每个命令都必须在其独有的 `<command>` 段中。

## 选项

1.[name](#name)
2.[executable](#executable)
3.[expect](#expect)
4.[extra_args](#extra_args)
5.[timeout_allowed](#timeout_allowed)

### name

指定命名的 名称。这个名称用于 [active-response](https://documentation.wazuh.com/current/user-manual/reference/ossec-conf/active-response.html)段.

| **Default value**  | n/a      |
| ------------------ | -------- |
| **Allowed values** | Any name |
| **use**            | Required |

### executable

要运行的脚本的名称,不需要提供路径,<br>因为主动响应脚本在Linux下 `/var/ossec/active-response/bin` 目录中,<br>windows系统在 `C:\Program Files\ossec-agent\active-response\bin` 目录中。

| **Default value**  | n/a           |
| ------------------ | ------------- |
| **Allowed values** | Any file name |
| **use**            | Required      |

### expect

指定提取的字段列表，这些字段将作为参数传递给脚本。如果在特定实例中未声明任何列出的字段，则这些字段值将以破折号`-`而不是空的形式传递。该命令要求在警报中找到期望的字段，否则，主动响应将被跳过。

一个很好的例子是firewall-block命令，该命令期望该`srcip`(source ip)字段以便知道要阻止的IP地址。多个期望的字段名称以逗号分隔。

| **Default value**  | n/a                                             |
| ------------------ | ----------------------------------------------- |
| **Allowed values** |  `srcip` `user` `filename` ,如果有多个用逗号分隔. |
| **use**            | Not required                                    |

### extra_args

> 3.5.0 版本新增

允许用户自定义发送到位于Agent端的主动响应脚本的参数。此设置的内容将附加到发送给Agent的现有参数之后

| **Default value**  | n/a                          |
| ------------------ | ---------------------------- |
| **Allowed values** | 主动响应脚本需要接受的额外参数. |
| **use**            | Not required                 |

### timeout_allowed

指定命令的类型是无状态(*stateful*) 还是 有状态(*stateless*).
如果值为`yes`, 命令的类型是有状态, 意味着经过 `active-response` 中指定的超时时间后, 命令执行的操作会被取消。

| **Default value**  | no      |
| ------------------ | ------- |
| **Allowed values** | yes, no |

## 配置示例

```xml
<!-- For Unix systems -->

<command>
  <name>custom_command</name>
  <executable>custom_script.sh</executable>
  <extra_args>-arg1 --arg2 arg3 ; cat /etc/passwd</extra_args>
  <timeout_allowed>yes</timeout_allowed>
</command>

<!-- For Windows systems -->

<command>
  <name>win_route-null</name>
  <executable>route-null.cmd</executable>
  <expect>srcip</expect>
  <timeout_allowed>yes</timeout_allowed>
</command>
```
