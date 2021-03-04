# RESPONSE-999-EXCLUSION-RULES-AFTER-CRS

该文件是用来在本地保存站点的例外设置。
您希望在启动过程中无条件禁用规则或修改其操作的规则是该文件中将要使用的规则类型。

请参阅文件`REQUEST-900-EXCLUSION-RULES-BEFORE-CRS.conf.example`，以获取规则排除机制的说明以及此文件的正确用法。

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

下面例子的意思是: 禁用所有含有`attack-injection-php` 标签的规则，从而禁用所有php注入规则的检测。

```bash
SecRuleRemoveByTag "attack-injection-php"
```

### 禁用特定参数的指定类型检测

下面例子的意思是:对于参数`foo` 不做任何含有标签`attack-sqli`规则的检测。

```bash
SecRuleUpdateTargetByTag "attack-sqli" "!ARGS:foo"
```

----

## 拦截动作

OWASP V3核心规则集目前支持两种配置模式：  <mark>异常评分模式</mark>(默认) 和 <mark>独立控制模式</mark>；

**异常评分模式**，也可称为"协同检测模式"，在此模式下，当检测到威胁时，并不会直接阻断此次访问，而是向下继续进行规则匹配，每个匹配成功的规则都会增加"异常分数"，在对客户端发起的请求数据检测结束时，以及对返回数据检测结束时，都会对异常分数的总和进行判断，如果大于设置的阈值，才会进行阻断动作，并向客户端返回403代码，审计日志中也会记录此次访问中所有匹配成功的规则信息。

****独立**控制模式**，此模式是V2版本规则集的默认模式，它的工作方式是，只要有一条规则匹配成功，便拦截此次访问，审计日志中也只会记录第一次检测到威胁的规则信息。

在异常模式下（CRS3中的默认设置），REQUEST-949-BLOCKING-EVALUATION.conf中的规则和RESPONSE-959-BLOCKING-EVALUATION.conf检查累积的攻击得分违反您的政策。 要应用破坏性措施，它们会覆盖默认值 SecDefaultAction（setup.conf）中指定的带有“拒绝”操作的操作。
默认情况下，此“拒绝”与“ status：403”操作配对。

为了将破坏性行为从“拒绝”更改为其他行为，您必须在CRS规则之后使用`SecRuleUpdateActionByID` 指令例如，在`RESPONSE-999-EXCLUSION-RULES-AFTER-CRS.conf`文件中进行配置。这些操作仅在使用异常模式时适用。

拦截的默认动作是返回 403 页面，如果想要使用默认动作，本配置文件不需要任何操作。

可以使用 `redirect` 指令可以完成拦截重定向。

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
