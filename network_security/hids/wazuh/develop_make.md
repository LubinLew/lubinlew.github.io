# Makefile 选项

本节包含在执行安装脚本之前通过编译源代码从Wazuh定制安装的说明。

您还可以在此处找到可用的不同设置`Makefile`。每个设置都有描述，包括您可以使用的默认值和允许值。



## 编译源代码

从源<_installation_agents>按照官方文档安装Wazuh管理器或agent：ref：时，用户将运行`install.sh`脚本。这将在安装源代码之前自动对其进行编译，但是可以在执行脚本之前进行一些自定义。

编译之前必须下载一些依赖项。如果在此之前未执行，则会出现一条错误消息，要求用户执行`make deps` 操作。

要使用编译代码`make`，工作目录必须是`MAKEFILE`驻留目录，在这种情况下，`/src`安装目录必须是该目录：

```bash
cd wazuh/src
make deps
make <OPTIONS>
```

编译源代码之后，现在您可以执行安装脚本：

```bash
cd ../
./install.sh
```

## Makefile

### Available targets

| Target              | Description                   |
| ------------------- | ----------------------------- |
| **deps**            | 下载编译代码所需的外部依赖项。需要Internet连接。  |
| **external**        | 编译外部依赖项。使用时将自动完成`build`。      |
| **build**           | 编译源代码。需要外部依赖关系和`TARGET`标志。    |
| **utils**           | 编译Wazuh二进制文件使用的补充工具。          |
| **test-rules**      | 针对规则和解码器运行测试套件。               |
| **clean**           | 删除所有内容，包括编译的文件，包括外部依赖项，测试和配置。 |
| **clean-deps**      | 删除所有外部依赖项，包括下载的文件。            |
| **clean-external**  | 删除编译的外部依赖项，但不会删除下载的文件。        |
| **clean-internals** | 删除所有已编译的内部依赖项。                |
| **clean-framework** | 删除用于构建Wazuh框架的所有已编译文件。        |
| **clean-windows**   | 删除用于构建Windows代理的所有已编译文件。      |
| **clean-config**    | 删除所有已编译的配置文件。                 |
| **clean-test**      | 删除所有用于测试的已编译文件。               |
| settings            | 显示编译标志的默认值。                   |
| help                | 显示有关Makefile的信息。              |

### Available flags

| Flags                 | Default value                              | Allowed values                         | Description                                                 |
| --------------------- | ------------------------------------------ | -------------------------------------- | ----------------------------------------------------------- |
| **TARGET**            | n/a                                        | server, local, hybrid, agent, winagent | 定义要构建的安装类型。<br/><br/>最常见的是server编译经理和agent/winagent 编译代理。 |
| **V**                 | n/a                                        | 1, yes, YES, y, Y                      | 显示完整的编译器消息。                                                 |
| **DEBUG**             | n/a                                        | 1, yes, YES, y, Y                      | 带有符号的构建而没有优化。                                               |
| **DEBUGAD**           | n/a                                        | 1, yes, YES, y, Y                      | 启用额外的调试登录ossec-analysisd。                                   |
| **PREFIX**            | /var/ossec                                 | Any valid absolute path.               | 将Wazuh安装到指定的绝对路径。                                           |
| **MAXAGENTS**         | 14000                                      | Any number.                            | 置最大代理数。                                                     |
| **ONEWAY**            | n/a                                        | 1, yes, YES, y, Y                      | 禁用经理对代理的ACK。它允许连接代理而无需管理器的向后连接。                             |
| **CLEANFULL**         | n/a                                        | 1, yes, YES, y, Y                      | 使警报邮件主题的格式清晰： <location> - <level> - <description>          |
| **RESOURCES_URL**     | https://packages.wazuh.com/deps/$(VERSION) | Any valid URL string.                  | 设置Wazuh资源URL。                                               |
| **USE_ZEROMQ**        | n/a                                        | 1, yes, YES, y, Y                      | 使用ZeroMQ支持进行构建。                                             |
| **USE_PRELUDE**       | n/a                                        | 1, yes, YES, y, Y                      | 在Prelude支持下进行构建。                                            |
| **USE_INOTIFY**       | n/a                                        | 1, yes, YES, y, Y                      | 借助Inotify支持进行构建。                                            |
| **USE_MSGPACK_OPT**   | n/a                                        | 1, yes, YES, y, Y                      | 使用Msgpack进行全面优化。                                            |
| **BIG_ENDIAN**        | n/a                                        | 1, yes, YES, y, Y                      | 在大端支持下进行构建。                                                 |
| **USE_SELINUX**       | n/a                                        | 1, yes, YES, y, Y                      | 使用SELinux策略进行构建。                                            |
| **USE_AUDIT**         | n/a                                        | 1, yes, YES, y, Y                      | 提供审计服务支持。                                                   |
| **USE_FRAMEWORK_LIB** | n/a                                        | 1, yes, YES, y, Y                      | 对框架使用外部SQLite库。                                             |
| **USE_GEOIP**         | n/a                                        | 1, yes, YES, y, Y                      | 使用GeoIP支持进行构建                                               |
| **OPTIMIZE_CPYTHON**  | n/a                                        | 1, yes, YES, y, Y                      | 当PREFIX指向默认目录以外的其他目录时，将重新构建python解释器，启用此标志可优化过程。            |
| **DATABASE**          | n/a                                        | mysql, pgsql                           | 借助数据库支持进行构建。允许支持MySQL或PostgreSQL。                           |
| **OSSEC_GROUP**       | ossec                                      | Any string.                            |                                                             |
| **OSSEC_USER**        | ossec                                      | Any string.                            |                                                             |
| **OSSEC_USER_MAIL**   | ossecm                                     | Any string.                            |                                                             |
| **OSSEC_USER_REM**    | ossecr                                     | Any string.                            |                                                             |
| **DISABLE_SHARED**    | n/a                                        | 1, yes, YES, y, Y                      | 禁用Wazuh共享库的编译，并使用静态库                                        |
| **DISABLE_SYSC**      | n/a                                        | 1, yes, YES, y, Y                      | 禁用Syscollector模块的编译                                         |
| **DISABLE_CISCAT**    | n/a                                        | 1, yes, YES, y, Y                      | 禁用CIS-CAT模块的编译                                              |
