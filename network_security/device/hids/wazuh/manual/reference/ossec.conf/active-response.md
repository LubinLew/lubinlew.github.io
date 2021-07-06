# active-response

XML section name

```xml
<active-response>
</active-response>
```

在主动响应配置中，将已有命令绑定到一个或多个规则或规则类型以及何时执行该命令的附加条件。可以使用的主动响应数量没有限制，但是，每个主动响应都必须在其自己的单独`<active-response>`部分中进行配置。

## 选项

| 选项                                        | 说明                    | Side    |
| ----------------------------------------- | --------------------- | ------- |
| [disabled](#disabled)                     | 功能开关                  | -       |
| [command](#command)                       | 绑定命令的名称               | Manager |
| [location](#location)                     | 命令执行位置                | Manager |
| [agent_id](#agent-id)                     | Agent ID              | Manager |
| [level](#level)                           | 要执行命令所需的最低严重性级别       | Manager |
| [rules_group](#rules-group)               | 触发的规则必需属于某个规则组才执行主动响应 | Manager |
| [rules_id](#rules-id)                     | 允许执行主动响应的规则ID         | Manager |
| [timeout](#timeout)                       | 执行反向命令之前的秒数           | Manager |
| [repeated_offenders](#repeated-offenders) | 多次触发者执行反向命令之前的分钟数     | Agent   |

### disabled

主动响应功能开关。 在一个 `Agent`上将此项设置为 `yes` 那么只有这个`Agent`才禁用主动响应, 

但是在 `Manager` 上设置为`yes` 则是表示在 `Manger` 和 所有 `Agent` 禁用主动响应。

> This option is available on server, local, and agent installations.

| **Default value**  | no      |
| ------------------ | ------- |
| **Allowed values** | yes, no |

### command

绑定命令的名称

| **Default value**  | n/a      |
| ------------------ | -------- |
| **Allowed values** | 已定名命令的名称 |

### location

指定命令在何处才能执行。

| **Default value**  | n/a           | -                          |
| ------------------ | ------------- | -------------------------- |
| **Allowed values** | local         | 只在产生该事件的Agent上运行           |
|                    | server        | 在 Wazuh Manager 上运行        |
|                    | defined-agent | 只在使用`agent_id`指定的的agent上运行 |
|                    | all           | 在所有的Agnet上执行(!小心使用)        |

>  例子:如果一个Agent 运行在一个能够与边缘防火墙(edge firewall)交互的端点上, 那么你就可能在边缘防火墙进行某些IP的阻断等操作。

### agent_id

指定执行主动响应的Agent ID (仅当 `<location>` 字段指定为 `defined-agent` 才有意义).

| **Default value**  | n/a      |
| ------------------ | -------- |
| **Allowed values** | Agent ID |

### level

定义要执行命令所需的最低严重性级别。

| **Default value**  | n/a    |
| ------------------ | ------ |
| **Allowed values** | 1 ~ 16 |

### rules_group

定义命中的规则必需属于某个规则组,才执行主动响应。

| **Default value**  | n/a                          |
| ------------------ | ---------------------------- |
| **Allowed values** | 允许任何规则组。多个组应使用竖线字符 `“\|”` 分隔 |

> 所有的 group都必须以逗号结束

### rules_id

指定允许执行主动响应的规则ID列表.

| **Default value**  | n/a                |
| ------------------ | ------------------ |
| **Allowed values** | rule id,多个ID使用逗号分隔 |

> 当同时设置 `level`，`rules_group`和`rules_id`，主动响应将触发与任何规则匹配一个。换句话说，它们是`或`的关系不是`与`的关系。

### timeout

指定执行反向命令之前的秒数。

当设置了`repeated_offenders` 选项时, `timeout` 只影响第一次触发规则。

| **Default value**  | n/a       |
| ------------------ | --------- |
| **Allowed values** | 正整数 (单位秒) |

### repeated_offenders

当一个`目标`多次触发某个规则时, 这个选项用来设置执行反向命令之前分钟数。

 这是一个以逗号分隔的递增超时列表，最多可以包含5个条目。

这个选项必须直接配置到 `Agent` 中的 `ossec.conf` 文件中, 即使使用了集中配置文件`agent.conf`;

除此之外，它必须在配置文件中位于上方的`<active-response>`部分中定义。

| **Default value**  | n/a        |
| ------------------ | ---------- |
| **Allowed values** | 正整数 (单位分钟) |

----

## 配置示例

```xml
<!-- On the manager side -->

<active-response>
  <disabled>no</disabled>
  <command>host-deny</command>
  <location>defined-agent</location>
  <agent_id>032</agent_id>
  <level>10</level>
  <rules_group>sshd,|pci_dss_11.4,</rules_group>
  <timeout>1</timeout>
</active-response>

<!-- On the agent side -->

<active-response>
  <disabled>no</disabled>
  <repeated_offenders>1,5,10</repeated_offenders>
</active-response>
```
