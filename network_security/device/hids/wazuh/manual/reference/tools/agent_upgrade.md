# agent_upgrade

> New in version 3.0.0.

agent_upgrade 程序能够列出过时的 Agent 并进行升级。

> 如果是具有多节点的Wazuh群集，则必须在连接该 Agent 的节点上执行 agent_upgrade。所以推荐使用 Wazuh API 进行升级。
> 
> 从v4.1.0开始，升级过程由[代理升级模块](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/agent-upgrade-module.html#agent-upgrade-module)执行，并且agent_upgrade脚本可以在任何节点上执行。

Agent 完成更新后，它将自动重启动以应用新配置。

## 选项

| 选项                                       | 解释                              |
| ---------------------------------------- | ------------------------------- |
| `-h, --help`                             | 显示帮助信息                          |
| `-l, --list_outdated`                    | 列出所有过时的 Agent                   |
| `-a AGENT_IDs, --agents AGENT_IDs`       | 对指定AgentID 的 Agent 进行升级         |
| `-F, --force`                            | 允许重装相同版本或者降级                    |
| `-s, --silent`                           | 不显示输出信息                         |
| `-v VERSION, --version VERSION`          | 指定要安装的版本, eg `v4.0.0`           |
| `-r REPOSITORY, --repository REPOSITORY` | 指定 库(repository)的URL            |
| `-f FILE, --file FILE`                   | 自定义WPK文件名称                      |
| `-x EXECUTE, --execute EXECUTE`          | WPK中自定义的可执行文件名称,默认是`upgrade.sh` |

> 默认情况下，超时将是代理使用[internal_options.conf中](https://documentation.wazuh.com/current/user-manual/reference/internal-options.html)的`execd.max_restart_lock`选项所允许的最大超时时间。

---

## 例子

#### 列出所有过时的Agent

```bash
agent_upgrade -l

# Output
ID Name Version
002 VM_Debian9 Wazuh v3.13.2
003 VM_Debian8 Wazuh v3.13.2
009 VM_WinServ2016 Wazuh v3.10.1
Total outdated agents: 3
```

#### 升级指定Agent

```bash
agent_upgrade -a 002

# Output
Upgrading...

Upgraded agents:
    Agent 002 upgraded: Wazuh v3.13.2 -> 4.0.0
```

#### 使用自定义库降级Agent

```bash
agent_upgrade -a 002 -dF -v v4.0.0 -r http://mycompany.wpkrepo.com/

# Output
Upgrading...

Upgraded agents:
    Agent 002 upgraded: Wazuh v3.13.2 -> 4.0.0
```

#### 安装自定义WPK文件

```bash
agent_upgrade -a 002 -d -f /root/upgrade_openscap_debian.wpk -x install.sh

# Output
Upgrading...

Upgraded agents:
    Agent 002 upgraded: Wazuh v3.13.2 -> 4.0.0
```


