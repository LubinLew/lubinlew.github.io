# active-response

XML section name

```xml
<active-response>
</active-response>
```

In the active response configuration section, an existing command is bound to one or more rules or rule types along with additional criteria for when to execute the command. There is no limit to the number of active responses that can be used, however, each active response must be configured in its own separate `<active-response>` section.

## 选项

1.[disabled](#disabled)

**Manager side** >

2.[command](#command)
3.[location](#location)
4.[agent_id](#agent-id)
5.[level](#level)
6.[rules_group](#rules-group)
7.[rules_id](#rules-id)
8.[timeout](#timeout)

**Agent side** >

9.[repeated_offenders](#repeated-offenders)

### disabled

Toggles the active-response capability on and off. Setting this option to `yes` on an agent will disable active-response for that agent only, while setting it in the manager’s `ossec.conf` file will disable active-response on the manager and all agents.

> This option is available on server, local, and agent installations.

| **Default value**  | no      |
| ------------------ | ------- |
| **Allowed values** | yes, no |

### command

绑定命令的名称

| **Default value**  | n/a                                      |
| ------------------ | ---------------------------------------- |
| **Allowed values** | Any defined active response command name |

### location

指定命令在何种系统上才能执行。

| **Default value**  | n/a            |                                 |
| ------------------ | -------------- | ------------------------------------- |
| **Allowed values** | local              | 只在产生该事件的agent上运行 |
|                    | server             | 在 Wazuh manager 上运行         |
| defined-agent      | 只在使用`agent_id`指定的的agent上运行 |          |
| all                | 在所有的Agnet上执行(!小心使用).          |                 |

Example:

If the application that interfaces with your edge firewall runs on one of your agents, you might have a firewall-block-edge command that runs a script on that agent to blacklist an offending IP on the edge firewall.

### agent_id

Specifies the ID of the agent on which to execute the active response command (used when defined-agent is set).

| **Default value**  | n/a                                                                                   |
| ------------------ | ------------------------------------------------------------------------------------- |
| **Allowed values** | Any agent id number, as long as **defined-agent** has been specified as the location. |

### level

Defines a minimum severity level required for the command to be executed.

| **Default value**  | n/a                    |
| ------------------ | ---------------------- |
| **Allowed values** | Any level from 1 to 16 |

### rules_group

Defines the rule group that a rule must belong to one for the command to be executed.

| **Default value**  | n/a                                                                                          |
| ------------------ | -------------------------------------------------------------------------------------------- |
| **Allowed values** | Any rule group is allowed. Multiple groups should be separated with a pipe character (“\|”). |

> 所有的 group都必须以逗号结束

### rules_id

Limits the command execution to only when one or more listed rules fire.

| **Default value**  | n/a                       |
| ------------------ | ------------------------- |
| **Allowed values** | rule id,多个ID使用逗号分隔 |

Note

When setting `level`, `rules_group` and `rules_id` together, the active response will be triggered always that any rule matches with **one** of these options. In other words, they are accumulative options, not restrictive.

### timeout

Specifies how long in seconds before the reverse command is executed. When `repeated_offenders` is used, `timeout` only applies to the first offense.

| **Default value**  | n/a                         |
| ------------------ | --------------------------- |
| **Allowed values** | A positive number (seconds) |

### repeated_offenders

Sets timeouts in minutes for repeat offenders. This is a comma-separated list of increasing timeouts that can contain a maximum of 5 entries.

| **Default value**  | n/a                         |
| ------------------ | --------------------------- |
| **Allowed values** | A positive number (minutes) |

Warning

This option must be configured directly in the **ossec.conf** file of the agent, even when using a manager/agent setup with centralized configuration of other settings via **agent.conf**. Apart from that, it has to be defined in the upper `<active-response>` section found in the configuration file.

## Sample Configuration

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
