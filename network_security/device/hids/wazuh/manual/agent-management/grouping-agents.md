# Agent 分组

> 3.0.0版中的新功能。

有两种方法配置已注册的 `Agent`。
可以使用 [ossec.conf](https://documentation.wazuh.com/current/user-manual/reference/ossec-conf/index.html) 文件在本地对其进行配置，也可以使用[集中式配置](https://documentation.wazuh.com/current/user-manual/reference/centralized-configuration.html)对其进行远程配置。
如果使用集中式配置，则可以将Agent分配给组，每个组具有唯一的配置。这大大简化了整个配置过程。

> 阅读 [Agent groups and centralized configuration](https://wazuh.com/blog/agent-groups-and-centralized-configuration/) 一文了解更多信息.

<mark>除非另行分配，否则所有新连接的 Agent 将自动属于`default` 组</mark>。该组是在安装过程中创建的，
配置文件放置在`/var/ossec/etc/shared/default/`文件夹中。这些文件将从Manager推送到属于该组的所有Agent。

分配之前，必须先创建和配置该组。

## 为 Agent 分配一个组

添加Agent并将其连接到Manager后，可以使用 [agent_group](https://documentation.wazuh.com/current/user-manual/reference/tools/agent_groups.html) 工具或 Wazuh API 将其分配给一个组。

以下是将Agent ID 为 002 的Agent分配到一个名为 `dbms` 的组的例子

### 使用 agent_ groups 工具

agent_group 工具 在 Wazuh Server 上, Wazuh Agent 上没有

```bash
/var/ossec/bin/agent_groups -a -i 002 -g dbms
```

### 使用 Wazuh API

使用API接口 [PUT /agents/{agent_id}/group/{group_id}](https://documentation.wazuh.com/current/user-manual/api/reference.html#operation/api.controllers.agent_controller.put_agent_single_group)：

```json
curl -k -X PUT "https://localhost:55000/agents/002/group/dbms?pretty=true" -H  "Authorization: Bearer $TOKEN"

//输出结果:
{
  "data": {
      "affected_items": ["002"],
      "total_affected_items": 1,
      "total_failed_items": 0,
      "failed_items": [],
  },
  "message": "All selected agents were assigned to dbms",
  "error": 0,
}
```

### 检查Agent的组分配

#### 使用 agent_ groups 工具

```bash
/var/ossec/bin/agent_groups -l -g dbms
```

输出结果:

```bash
5 agent(s) in group 'dbms':
 ID: 002 Name: agent-dbms-e1.
 ID: 003 Name: agent-dbms-e2.
 ID: 004 Name: agent-dbms-a1.
 ID: 005 Name: agent-dbms-a2.
 ID: 006 Name: agent-dbms-a3.
```

#### 使用 Wazuh API

API接口 [GET /groups/{group_id}/agents](https://documentation.wazuh.com/current/user-manual/api/reference.html#operation/api.controllers.agent_controller.get_agents_in_group)：

```json
curl -k -X GET "https://localhost:55000/groups/dbms/agents?pretty=true&select=id,name" -H  "Authorization: Bearer $TOKEN"

//输出结果
```json
{
   "data": {

       "affected_items": [
           {"name": "agent-dbms-e1", "id": "002"},
           {"name": "agent-dbms-e2", "id": "003"},
           {"name": "agent-dbms-a1", "id": "004"},
           {"name": "agent-dbms-a2", "id": "005"},
           {"name": "agent-dbms-a3", "id": "006"},
       ],
       "total_affected_items": 5,
       "total_failed_items": 0,
       "failed_items": [],

   },
   "message": "All selected agents information was returned",
   "error": 0,
}
```

一旦组被创建，它的`agent.conf`文件就可以编辑，包含你想分配给这个组的特殊配置。
对于上面的例子，要编辑的文件位于`/var/ossec/etc/shared/dbms/agent.conf`并且属于该组的每个Agent都将接收该文件。

连接到Manager的20分钟内，每一个被分配到该组的Agent都会从Manager接收`dbms` 文件夹中包含的配置文件,包含`agent.conf`文件。
Manager将这些文件发送到Agent所花费的时间长短取决于文件大小，组中Agent的数量以及所使用的连接协议。
例如，取决于网络带宽和性能，使UDP协议在100个Agnet上接收10 MB文件夹（不包括merged.mg文件）可能需要8分钟，
但是，如果使用TCP，则速度可能会更快。

一旦一个Agent属于属于某个组，即使该Agent以其他名称或ID注册过了,该Agent也将自动重新分配到该组。
发生这种情况的原因是，在重新注册Agent时，会将Agent发送的 `merged.mg`文件的校验和(checksum)同其他在Manager中注册的其他Agent进行比较。
然而，这是不是默认的行为，而且，如果需要的话，必须由用户明确激活`local_internal_options.conf`通过添加选项`remoted.guess_agent_group=1`(`remoted`选项详见[internal-options](https://documentation.wazuh.com/current/user-manual/reference/internal-options.html))

-----

## 多组

> 3.7.0版中的新功能。

从Wazuh v3.7.0开始，支持Agent属于多个组。Agent将从每个组接收所有配置文件。从最后分配的组接收的配置具有比其他组更高的优先级。

### 管理多组

- [将一个Agent分配到多个组](#将一个Agent分配到多个组)

- [清单组和配置](https://documentation.wazuh.com/current/user-manual/agents/grouping-agents.html?highlight=group#listing-groups-and-configuration)

- [多进行更改组小号](https://documentation.wazuh.com/current/user-manual/agents/grouping-agents.html?highlight=group#making-changes-on-multiple-groups)

- [共享文件的行为](https://documentation.wazuh.com/current/user-manual/agents/grouping-agents.html?highlight=group#shared-files-behavior)

此功能专注于以更高的粒度自定义Agent的配置。该**Wazuh API**和**agent_组小号** 帮助管理组通过列出他们还通过允许分配/变更/取消分Agent，代理商秒。让我们Agent下在现有代理上管理多个组的三个用例。

### 将一个Agent分配到多个组

将一个Agent分配到多个组很简单，可以通过三种不同的方式：注册，CLI 和 API。

在此示例中，Agent 001 将会被被添加到`webserver`和`apache` 这2个组中。由于每个Agent都有一个默认组`default`组, 所以Agent 001 将会同时属于3各组。

#### 使用 Wazuh API

使用Wazuh API 接口 [PUT /agents/{agent_id}/group/{group_id}](https://documentation.wazuh.com/current/user-manual/api/reference.html#operation/api.controllers.agent_controller.put_agent_single_group)：

##### 先将Agnet 001 注册到 webserver 组

```json
//将Agnet 001 注册到 webserver 组
curl -k -X PUT "https://localhost:55000/agents/001/group/webserver?pretty=true" -H  "Authorization: Bearer $TOKEN"

// 输出结果
{
    "data": {
        "affected_items": ["001"],
        "total_affected_items": 1,
        "total_failed_items": 0,
        "failed_items": [],
    },
    "message": "All selected agents were assigned to webserver",
    "error": 0,
}
```

##### 再将Agnet 001 注册到 apache 组

```json
//将Agnet 001 注册到 apache 组
curl -k -X PUT "https://localhost:55000/agents/001/group/apache?pretty=true" -H  "Authorization: Bearer $TOKEN"

//输出结果
{
    "data": {
        "affected_items": ["001"],
        "total_affected_items": 1,
        "total_failed_items": 0,
        "failed_items": [],
    },
    "message": "All selected agents were assigned to apache",
    "error": 0,
}
```

##### Agent 组查询

之后，我们可以使用**Wazuh API** [GET /agents](https://documentation.wazuh.com/current/user-manual/api/reference.html#operation/api.controllers.agent_controller.get_agents)来查询Agent属于哪个组：

```json
// Agnet 组查询
curl -k -X GET "https://localhost:55000/agents?pretty=true&agents_list=001&select=group" -H  "Authorization: Bearer $TOKEN"

//输出结果
{
    "data": {
        "affected_items": [{"group": ["default", "webserver", "apache"], "id": "001"}],
        "total_affected_items": 1,
        "total_failed_items": 0,
        "failed_items": [],
    },
    "message": "All selected agents information was returned",
    "error": 0,
}
```

在这种情况下，当任何配置参数存在冲突时，apache组的拥有最高优先级。

#### 使用agent_ group 工具

可以以相同的方式将Agent注册到group：

##### 再将Agnet 001 注册到 apache 组

```bash
/var/ossec/bin/agent_groups -a -i 001 -g webserver

# 输出结果
Do you want to add the group 'webserver' to the agent '001'? [y/N]: y
Group 'webserver' added to agent '001'.
```

##### 再将Agnet 001 注册到 apache 组

```bash
/var/ossec/bin/agent_groups -a -i 001 -g apache

# 输出结果
Do you want to add the group 'apache' to the agent '001'? [y/N]: y
Group 'apache' added to agent '001'.
```

##### Agent 组查询

```bash
/var/ossec/bin/agent_groups -s -i 001

# 输出结果
The agent 'ag-windows-12' with ID '001' has the group: '[u'webserver', u'apache']'.
```

#### 使用 agent-auth 工具

要在注册过程中将Agent分配给一个或多个组，请使用-G选Agent册代理以设置Agent其中包含代理的组：

```bash
/var/ossec/bin/agent-auth -m MANAGER_IP -G webserver,apache
```

### 清单组和配置

可以实时了解属于组的Agent，以及根据其所属的组来应用于每个组的配置和共享文件。

例如，要列出当前可用的组，我们可以对**agent_groups**运行以下查询：

```bash
/var/ossec/bin/agent_groups -l -g webserver

# 输出结果
3 agent(s) in group 'webserver':
  ID: 001 Name: ag-windows-12.
  ID: 003 Name: ag-windows-east.
  ID: 004 Name: centos-7-apache
```

### 多组变更

可以为Agent分配多个组的方法相同，也可以还原分配并在可用组之间进行切换。下面显示了Agent为代理001设置组apache：

```bash
/var/ossec/bin/agent_groups -r -i 001 -g apache -q

# 输出结果
Group 'apache' unset for agent '001'.
```

也可以在组之间进行切换，以覆盖现有分配：

```bash
/var/ossec/bin/agent_groups -a -f -i 001 -g apache

# 输出结果
Group 'apache' set to agent '001'.


/var/ossec/bin/agent_groups -s -i 001

# 输出结果
The agent 'ag-windows-12' with ID '001' has the group: '[u'apache']'.
```

该`-f`参数重置分配给Agent的组s并强制其仅属于新组。

最后，要检查的同步状态组配置为单一药剂，无论是以下方法是可用的，Wazuh API [GET /agent/{AGENT_ID}/group/is_sync](https://documentation.wazuh.com/current/user-manual/api/reference.html#operation/api.controllers.agent_controller.get_sync_agent)：

```bash
 /var/ossec/bin/agent_groups -S -i 001

# 输出结果
The agent '001' sync status is: Agent configuration is synced.
```

```json
 curl -k -X GET "https://localhost:55000/agents/001/group/is_sync?pretty=true" -H  "Authorization: Bearer $TOKEN"

//输出结果
{
    "error": 0,
    "data": {
        "affected_items": [
            {
                "id": "001",
                "synced": true
            }
        ],
        "total_affected_items": 1,
        "total_failed_items": 0,
        "failed_items": []
    },
    "message": "Sync info was returned for all selected agents"
}
```

其余agent_group功能可以在[agent_groups](https://documentation.wazuh.com/current/user-manual/reference/tools/agent_groups.html)查看。[Wazuh API](https://documentation.wazuh.com/current/user-manual/api/reference.html)。

### 共享文件行为

如上所述，传统上，Manager根据其Agent所属组共享配置文件。

如果属于多个组，则每个组的配置文件将按照以下条件合并为一个：

- 共享文件例如用于rootkit检测的CIS基准,被放入了共享文件夹，如果共享文件重复，则最后添加的文件将覆盖旧文件。

- 新增加的`agent.conf`文件将追加到现有文件中。当两个组的配置冲突时，分配给Agent的最后一个组优先级高。在[集中式配置手册](https://documentation.wazuh.com/current/user-manual/reference/centralized-configuration.html)了解有关配置优先级的更多信息。

- 从用户设置到特定组的自定义共享文件也将加入以将其发送给Agent。

![多组](images/multigroups1.png)
