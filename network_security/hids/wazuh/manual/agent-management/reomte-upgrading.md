# 远程升级

> New in version 3.0.0.

Contents

- [Upgrading agent](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/upgrading-agent.html)
  - [Using the command line](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/upgrading-agent.html#using-the-command-line)
  - [Using the RESTful API](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/upgrading-agent.html#using-the-restful-api)
- [Agent upgrade module](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/agent-upgrade-module.html)
  - [Upgrade request [API/CLI]](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/agent-upgrade-module.html#upgrade-request-api-cli)
  - [Upgrade result request [API/CLI]](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/agent-upgrade-module.html#upgrade-result-request-api-cli)
- [Adding a custom repository](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/custom-repository.html)
- [Custom WPK packages creation](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/create-custom-wpk/create-wpk-key.html)
  - [Get an X509 certificate and CA](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/create-custom-wpk/create-wpk-key.html#get-an-x509-certificate-and-ca)
  - [Build the WPK package](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/create-custom-wpk/create-wpk-key.html#build-the-wpk-package)
- [Installing a custom WPK package](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/install-custom-wpk.html)
  - [1. Install the root CA into the agent](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/install-custom-wpk.html#install-the-root-ca-into-the-agent)
  - [2. Run the upgrade](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/install-custom-wpk.html#run-the-upgrade)
- [WPK List](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/wpk-list.html)
  - [Linux](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/wpk-list.html#linux)
  - [Windows](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/wpk-list.html#windows)

从3.0.0 版开始支持 Agent 远程升级。此升级由 Manager 执行，该 Manager 向每个注册的 Agent 发送 **WPK**（Wazuh签名软件包）文件，其中包含将 Agent 升级到新版本所需的文件。这样就可以安全简单地升级 Agent，而无需单独访问每个Agent。

Wazuh为每个新版本提供对更新的WPK存储库的访问。所有可用的WPK文件都可以在[此处](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/wpk-list.html)找到。

还可以按照[添加自定义存储库中](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/custom-repository.html)所述的步骤[添加自定义存储库](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/custom-repository.html)。

> 从v4.1.0开始，升级过程由 [Agent升级模块](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/agent-upgrade-module.html#agent-upgrade-module) 负责。

----

## 升级Agent的方式

升级Agent的方式分为 命令行 和 Wazuh API 两种方式。如果是Wazuh集群，建议使用Wazuh API。

### 命令行方式升级

命令行方式需要使用 [`agent_upgrade`](network_security/hids/wazuh/manual/reference/tools/agent_upgrade.md)工具。

1.列出过时的 Agent

```bash
agent_upgrade -l

# output
ID    Name                               Version
002   VM_Debian9                         Wazuh v3.13.2
003   VM_Debian8                         Wazuh v3.13.2
009   VM_WinServ2016                     Wazuh v3.10.1

Total outdated agents: 3
```

2.升级指定的 Agent

```bash
agent_upgrade -a 002

# output
Upgrading...

Upgraded agents:
    Agent 002 upgraded: Wazuh v3.13.2 -> 4.0.0
```

3.升级确认

升级后，Agent 将自动重新启动。检查Agent版本以确保正确升级：

```bash
/var/ossec/bin/agent_control -i 002

# Output
Agent ID:   002
Agent Name: wazuh-agent2
IP address: any/any
Status:     Active

Operating system:    Linux |wazuh-agent2 |5.8.0-7625-generic |Ubuntu SMP Wed Nov 4 01:25:00 UTC 2 |x86_64
Client version:      Wazuh v4.0.0
Configuration hash:  e2f47d482da37c099fa1d6e4c43b523c
Shared file hash:    aabb92f4a8cba49c7c6045c1aa80fbd3
Last keep alive:     1604927114

Syscheck last started at:  Mon Nov  9 13:00:55 2020
Syscheck last ended at:    Mon Nov  9 13:00:56 2020

Rootcheck last started at: Mon Nov  9 13:00:57 2020
```

---

### 使用Wauzh API升级

1.列出过时的 Agent

```json
curl -k -X GET "https://localhost:55000/agents/outdated?pretty=true" -H  "Authorization: Bearer $TOKEN"

{
    "data": {
        "affected_items": [
            {"version": "Wazuh v3.0.0", "id": "002", "name": "VM_Debian9"},
            {"version": "Wazuh v3.0.0", "id": "003", "name": "VM_Debian8"},
            {"version": "Wazuh v3.0.0", "id": "009", "name": "VM_WinServ2016"},
        ],
        "total_affected_items": 3,
        "total_failed_items": 0,
        "failed_items": [],
    },
    "message": "All selected agents information was returned",
    "error": 0,
}
```

2.升级指定的 Agent

```json
curl -k -X PUT "https://localhost:55000/agents/upgrade?agents_list=002,003&pretty=true" -H  "Authorization: Bearer $TOKEN"

{
  "data": {
    "affected_items": [
      {
        "agent": "002",
        "task_id": 1
      },
      {
        "agent": "003",
        "task_id": 2
      }
    ],
    "total_affected_items": 2,
    "total_failed_items": 0,
    "failed_items": []
  },
  "message": "All upgrade tasks have been created",
  "error": 0
}
```

3.升级确认

同时升级 002 和 003 两台 Agent

```bash
curl -k -X GET "https://localhost:55000/agents/upgrade_result?agents_list=002,003&pretty=true" -H  "Authorization: Bearer $TOKEN"

{
  "data": {
    "affected_items": [
      {
        "message": "Success",
        "agent": "002",
        "task_id": 1,
        "node": "worker2",
        "module": "upgrade_module",
        "command": "upgrade",
        "status": "Updated",
        "create_time": "2020/10/21 17:13:45",
        "update_time": "2020/10/21 17:14:07"
      },
      {
        "message": "Success",
        "agent": "003",
        "task_id": 2,
        "node": "worker1",
        "module": "upgrade_module",
        "command": "upgrade",
        "status": "Updated",
        "create_time": "2020/10/21 17:13:45",
        "update_time": "2020/10/21 17:14:11"
      }
    ],
    "total_affected_items": 2,
    "total_failed_items": 0,
    "failed_items": []
  },
  "message": "All agents have been updated",
  "error": 0
}
```

---

## 升级模块

Agent程序升级模块(agent upgrade module)负责远程执行整个代理程序升级过程：

- 在 Manager 端，它负责验证，下载和/或将WPK文件发送给 Agent。

- 在 Agent 端，它处理收到的命令，并在完成升级过程后向 Manager 发送通知。

### 升级请求 [API/CLI][¶](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/agent-upgrade-module.html#upgrade-request-api-cli "Permalink to this headline")

Agent程序升级模块将从以下位置接收升级请求：

`{WAZUH_DIR}/queue/tasks/upgrade`

该模块需要3个参数：

| 选项         | 子选项    | 取值范围                        | 描述               |
| ---------- | ------ | --------------------------- | ---------------- |
| origin     | module | `api`                       | 请求者              |
| command    | -      | `upgrade`, `upgrade_custom` | 指定要执行的命令         |
| parameters | agents | 整型值数组（Agent的ID）             | 指定需要下发命令的Agent列表 |

1. **Upgrade parameters**: Command to upgrade agents from a repository.
   
   | Option            | Suboption    | Values | Required               | Default                                                                          | Description                                                           |
   | ----------------- | ------------ | ------ | ---------------------- | -------------------------------------------------------------------------------- | --------------------------------------------------------------------- |
   | **parameters**    | **wpk_repo** | string | no                     | `<wpk_repo>` configuration                                                       | Parameter to override the default WPK repository set by configuration |
   | **version**       | vX.Y.Z       | no     | Last available version | Overrides the version of the package that will be downloaded from the repository |                                                                       |
   | **use_http**      | true, false  | no     | false                  | Wheter retrieve the WPK file over http or https                                  |                                                                       |
   | **force_upgrade** | true, false  | no     | false                  | Forces the agents to upgrade, ignoring version validations                       |                                                                       |
   
   Example message:
   
   Output
   
   {
      "origin": {
   
          "module": "api"
   
      },
      "command": "upgrade",
      "parameters": {
   
          "agents": [5,6],
          "wpk_repo": "packages.wazuh.com/wpk/",
          "version": "v4.1.0",
          "use_http": false,
          "force_upgrade": false
   
      }
   }

2. **Upgrade custom parameters**: Command to upgrade agents from a local WPK file.
   
   > Note
   > 
   > In case of having a multi-node Wazuh cluster, the custom WPK file has to exist on all nodes in the specified path.
   > 
   > | Option         | Suboption     | Values | Required                                        | Default                               | Description                            |
   > | -------------- | ------------- | ------ | ----------------------------------------------- | ------------------------------------- | -------------------------------------- |
   > | **parameters** | **file_path** | string | yes                                             |                                       | Path to the WPK file that will be sent |
   > | **installer**  | string        | no     | `upgrade.sh` in Linux, `upgrade.bat` in Windows | Override the default installer script |                                        |
   > 
   > Example message:
   > 
   > Output
   > 
   > {
   > 
   >     "origin": {
   >         "module": "api"
   >     },
   >     "command": "upgrade_custom",
   >     "parameters": {
   >         "agents": [20,23],
   >         "file_path": "/home/user/agent.wpk",
   >         "installer": "custom-upgrade-script.sh"
   >     }
   > 
   > }

## Upgrade result request [API/CLI][¶](https://documentation.wazuh.com/current/user-manual/agents/remote-upgrading/agent-upgrade-module.html#upgrade-result-request-api-cli "Permalink to this headline")

The task manager is the one that informs the result of an upgrade task. It will receive the upgrade result requests from a socket located at:

{WAZUH_DIR}/queue/tasks/task

Note

In case of having a multi-node Wazuh cluster, the task manager only runs on the master node. This request should always go to the master node.

The module expects 3 parameters:

| Option         | Suboption  | Values                                 | Description                                                    |
| -------------- | ---------- | -------------------------------------- | -------------------------------------------------------------- |
| **origin**     | **module** | api (only value allowed)               | Emmiter of the request                                         |
| **command**    |            | upgrade, upgrade_custom                | Specifies the command to be executed                           |
| **parameters** | **agents** | Array of int values (id of the agents) | Specifies the list of agents where the command will be applied |

Example message:

Output

{
    "origin": {
        "module": "api"
    },
    "command": "upgrade_result",
    "parameters": {
        "agents": [5,10]
    }
}

The response will contain all the information related to the upgrade task stored in the tasks DB:

| Option      | Values    | Description                                              |
| ----------- | --------- | -------------------------------------------------------- |
| **error**   | int value | Error code: 0 when success, a positive number when error |
| **data**    | array     | Array with the responses for each agent                  |
| **message** | string    | String associated to the error code                      |

The information for each agent will be the following:

| Option          | Values                                                         | Description                                              |
| --------------- | -------------------------------------------------------------- | -------------------------------------------------------- |
| **error**       | int value                                                      | Error code: 0 when success, a positive number when error |
| **message**     | string                                                         | String associated to the error code                      |
| **node**        | string                                                         | Name of the node that executed the task retrieved        |
| **module**      | upgrade_module                                                 | Emmiter of the task retrieved                            |
| **command**     | upgrade, upgrade_custom                                        | Command executed by the task retrieved                   |
| **agent**       | int value (id of the agent)                                    | Id of the agent where the task retrieved was executed    |
| **task_id**     | int value (id of the task)                                     | Id of the task retrieved                                 |
| **create_time** | timestamp                                                      | Creation time of the task retrieved (UTC)                |
| **update_time** | timestamp                                                      | Last update time of the task retrieved (UTC)             |
| **status**      | In queue, Updating, Updated, Error, Cancelled, Timeout, Legacy | Current status of the task retrieved                     |
| **error_msg**   | string                                                         | String associated to the status when the status is Error |

Note

The legacy status is used to indicate that the upgrade is to an old version where the agent does not report the result of the task. The result of these tasks must be checked manually.

Example response:

Output

{
    "error": 0,
    "data": [
        {
            "error": 0,
            "message": "Success",
            "node": "node01",
            "module": "upgrade_module",
            "command": "upgrade",
            "agent": 5,
            "task_id": 15,
            "create_time": "2020/08/11 00:05:18",
            "update_time": "0",
            "status": "Updating"
        },{
            "error": 0,
            "message": "Success",
            "node": "node02",
            "module": "upgrade_module",
            "command": "upgrade",
            "agent": 10,
            "task_id": 16,
            "create_time": "2020/08/11 00:05:30",
            "update_time": "2020/08/11 00:05:52",
            "status": "Error",
            "error_msg": "SHA1 verification error"
        }
    ],
    "message": "Success"
}

---

## 自定义库

可以通过生成存储库来托管定制的代理程序升级软件包，以托管所生成的WPK文件。然后可以将管理器设置为从该存储库向代理发送文件。

WPK文件必须使用以下模式命名：

```bash
wazuh_agent_W_X_Y_Z.wpk
```

释义：

- W是发行版本, 例如 `v3.0.0`

- X 是操作系统的名称, 例如 `centos`

- Y 是操作系统的版本, 例如 `7`

- Z 是机器的体系结构,例如 `x86_64`

例如:

```bash
wazuh_agent_v3.0.0_centos_7_x86_64.wpk
```

存储库的结构应如下所示：

```bash
/
└── centos
    └── 7
        └── x86_64
            ├── versions
            ├── wazuh_agent_v3.0.0_centos_7_x86_64.wpk
            └── wazuh_agent_v3.1.0_centos_7_x86_64.wpk
```

 每个文件夹必须包含一个名为`versions`的文件，该文件列出该文件夹中表示的每个版本以及文件的SHA1哈希。<mark>最新版本必须放在该文件的第一行</mark>。

例如：

```bash
v3.1.0 f835015c6bbf87356a62bdfd513c7f1ffc16e0af
v3.0.0 df5397c8c4a1b29c42726dfa821330fa1bac7058
```

对于管理员来说，此存储库结构对于检查Agent 程序OS，版本和体系结构并寻找正确的升级包是必需的。例如，对于安装在Centos 7 x86_64上的代理，管理器将在*our_wpk_repo / centos / 7 / x86_64 /中*寻找最新的软件包。

----

## 自定义WPK软件包创建

### 获取X509证书和CA

要创建WPK软件包，需要具有X509证书和CA（如果您已经拥有的话），请跳至下[一部分](#生成WPK软件包)；否则，请按照以下步骤操作：

##### 创建根CA:

```bash
openssl req -x509 -new -nodes -newkey rsa:2048 -keyout wpk_root.key -out wpk_root.pem -batch
```

##### 创建证书和密钥:

```bash
openssl req -new -nodes -newkey rsa:2048 -keyout wpkcert.key -out wpkcert.csr -subj '/C=US/ST=CA/O=Wazuh'
```

subject 设置说明：

> - `/C=US` 是国家。
> 
> - `/ST=CA` 是州。
> 
> - `/O=Wazuh` 是组织的名称。

##### 使用根CA签署此证书:

```bash
openssl x509 -req -days 365 -in wpkcert.csr -CA wpk_root.pem -CAkey wpk_root.key -out wpkcert.pem -CAcreateserial
```

### 生成WPK软件包

创建WPK的方式有两种，可以手动进行，也可以通过docker自动化完成。



#### docker自动化生成WPK

Wazuh提供了一种使用docker构建WPK软件包的自动化方法，因此不需要任何其他依赖项。只需要 docker 和 git 工具即可。

从GitHub下载我们的wazuh-packages存储库，然后转到wpk目录:

```bash
git clone https://github.com/wazuh/wazuh-packages && cd wazuh-packages/wpk
```

`generate_wpk_package.sh`使用所需的不同选项执行脚本。该脚本将使用所有必要的工具来构建Docker映像，以创建WPK并运行将其构建的容器：


