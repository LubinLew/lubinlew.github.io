# RESPONSE-999-EXCLUSION-RULES-AFTER-CRS

该文件是用来在本地保存站点的例外设置。
您希望在启动过程中无条件禁用规则或修改其操作的规则是该文件中将要使用的规则类型。

请参阅文件[`REQUEST-900-EXCLUSION-RULES-BEFORE-CRS.conf.example`](modsecurity/crs/REQUEST-900-EXCLUSION-RULES-BEFORE-CRS.md)，以获取例外规则机制的说明以及此文件的正确用法。

| 指令                       | 说明                                 |
| ------------------------ | ---------------------------------- |
| SecRuleRemoveById        | 根据规则ID禁用指定的规则                      |
| SecRuleRemoveByMsg       | 根据规则msg禁用指定的规则                     |
| SecRuleRemoveByTag       | 据规则tag禁用指定的规则                      |
| SecRuleUpdateActionById  | 通过ID查询指定规则，并修改其动作列表                |
| SecRuleUpdateTargetById  | 通过ID查询指定规则，并修改其检测目标（变量）列表          |
| SecRuleUpdateTargetByMsg | 根据规则的msg属性，修改msg相同的所有规则的扫描目标（变量）列表 |
| SecRuleUpdateTargetByTag | 根据规则的msg属性，修改msg相同的所有规则的扫描目标（变量）列表 |

----

## 禁用规则

### 禁用某个规则

下面例子的意思是: 无条件禁用ID为 `942100` 规则，该规则是使用 `libinjection`进行SQL注入检测。

```bash
SecRuleRemoveById 942100
```

### 禁用一组规则

下面例子的意思是: 禁用所有含有`attack-injection-php` 标签(tag)的规则，从而禁用所有php注入规则的检测。

```bash
SecRuleRemoveByTag "attack-injection-php"
```

### 禁用特定参数的指定类型检测

下面例子的意思是:对于参数`foo` 不做任何含有标签`attack-sqli`规则的检测。

```bash
SecRuleUpdateTargetByTag "attack-sqli" "!ARGS:foo"
```

----

## 命中动作

OWASP V3核心规则集目前支持两种配置模式：  <mark>异常评分模式</mark>(默认) 和 <mark>独立控制模式</mark>；模式详细介绍见 [modsecurity.conf](modsecurity/modsecurity/modsecurity.conf.md)。

| 动作         | 说明      |     |
| ---------- | ------- | --- |
| `deny`     | 拦截请求    |     |
| `redirect` | 重定向请求   |     |
| `status`   | 返回指定响应码 |     |
| `drop`     | 断开请求连接  |     |

默认的命中动作是 `deny status:403` (拒绝请求，并返回403页面)。

在异常评分模式下，检查累积的攻击得分违反您的政策。 要应用破坏性措施，它们会覆盖默认值 `SecDefaultAction` 中指定的带有 `deny` 操作的操作。

为了将命中行为从 `deny` 更改为其他行为，您必须在CRS规则之后使用`SecRuleUpdateActionByID` 指令更新命中动作。

通常在[`RESPONSE-999-EXCLUSION-RULES-AFTER-CRS.conf`](modsecurity/crs/RESPONSE-999-EXCLUSION-RULES-AFTER-CRS.md)文件中进行配置。这些操作仅适用于异常评分模式。

### 重定向到主页

```bash
SecRuleUpdateActionById 949110 "t:none,redirect:'http://%{request_headers.host}/'"
SecRuleUpdateActionById 959100 "t:none,redirect:'http://%{request_headers.host}/'"
```

### 重定向要其他URL

```bash
SecRuleUpdateActionById 949110 "t:none,redirect:'http://example.com/report_problem'"
SecRuleUpdateActionById 959100 "t:none,redirect:'http://example.com/report_problem'"
```

### 返回404页面

```bash
SecRuleUpdateActionById 949110 "t:none,deny,status:404"
SecRuleUpdateActionById 959100 "t:none,deny,status:404"
```

### 断开连接

```bash
SecRuleUpdateActionById 949110 "t:none,drop"
SecRuleUpdateActionById 959100 "t:none,drop"
```
