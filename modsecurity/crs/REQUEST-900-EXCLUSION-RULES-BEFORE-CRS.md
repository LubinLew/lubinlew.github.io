# REQUEST-900-EXCLUSION-RULES-BEFORE-CRS.conf

该文件是用来在本地保存站点的例外设置。该文件包含的规则类型有以下几种:

- 规避检测

- 允许某些事务通过检测

- 更改已经应用的规则



下载CRS规则集后，可以发现rules文件夹下一个名为 `REQUEST-900-EXCLUSION-RULES-BEFORE-CRS.conf.example`文件。带有`.example`扩展名的文件是用来包含用户创建或修改的数据。`.example`扩展应该被重命名以`.conf`结尾的文件。这样做的好处是，OWASP CRS的更新后将不会覆盖用户生成的配置文件。由于这种设计范例，鼓励用户不要直接修改规则文件，而是修改
`REQUEST-900-EXCLUSION-RULES-BEFORE-CRS.conf`和`RESPONSE-999-EXCLUSION-RULES-AFTER-CRS.conf`这两个文件来修改OWASP规则。



`REQUEST-900-EXCLUSION-RULES-BEFORE-CRS` 和 `RESPONSE-999-EXCLUSION-RULES-AFTER-CRS` 文件的用途是不同的。 详细的区分如下:

ModSecurity 维护两个不同的上下文(Context)：启动(startup)和每个事务(per transaction)。

通常，指令是在启动上下文中处理的，它们会影响每个事务的上下文，通常在ModSecurity 执行期间保持不变。因此如果要在启动时禁用某个规则，则必须在对应的规则声明后放置 `SecRuleRemoveById`指令(或其他具有类似功能的指令)，否则它将不知道该规则的存在(因为这些规则是在同时读入的)。 这意味着，当使用影响 `SecRules` 的指令时，应在所有现有规则之后放置这些例外。 这就是为什么将`RESPONSE-999-EXCLUSION-RULES-AFTER-CRS.conf`设计为最后加载的原因。

相反，ModSecurity支持几种动作，这些动作可以在每个事务上下文期间（即正在处理规则时）更改基础配置的状态，通常是通过使用 `ctl` 动作来完成的。 由于这些是规则的一部分，因此将按照应用规则的顺序对它们进行评估（通过物理位置，考虑阶段）。 作为此命令的结果，应考虑何时执行 `ctl` 动作。 这与涉及修改ID的`ctl` 选项（例如`ruleRemoveById`）特别相关。 在这些情况下，重要的是将此类规则放置在它们将影响的规则ID之前。 与设置上下文不同，当我们在每个事务上下文中处理规则时，我们已经知道所有规则ID。 正是基于这种逻辑，我们在所有其余规则之前包含了诸如此类的规则。 结果，`REQUEST-900-EXCLUSION-RULES-BEFORE-CRS.conf`设计为首先加载。

| 指令                          | 使用位置                                                |
| --------------------------- | --------------------------------------------------- |
| `ctl:ruleEngine`            | <mark>REQUEST-900-EXCLUSION-RULES-BEFORE-CRS</mark> |
| `ctl:ruleRemoveById`        | <mark>REQUEST-900-EXCLUSION-RULES-BEFORE-CRS</mark> |
| `ctl:ruleRemoveByMsg`       | <mark>REQUEST-900-EXCLUSION-RULES-BEFORE-CRS</mark> |
| `ctl:ruleRemoveByTag`       | <mark>REQUEST-900-EXCLUSION-RULES-BEFORE-CRS</mark> |
| `ctl:ruleRemoveTargetById`  | <mark>REQUEST-900-EXCLUSION-RULES-BEFORE-CRS</mark> |
| `ctl:ruleRemoveTargetByMsg` | <mark>REQUEST-900-EXCLUSION-RULES-BEFORE-CRS</mark> |
| `ctl:ruleRemoveTargetByTag` | <mark>REQUEST-900-EXCLUSION-RULES-BEFORE-CRS</mark> |
| `SecRuleRemoveById`         | RESPONSE-999-EXCLUSION-RULES-AFTER-CRS              |
| `SecRuleRemoveByMsg`        | RESPONSE-999-EXCLUSION-RULES-AFTER-CRS              |
| `SecRuleRemoveByTag`        | RESPONSE-999-EXCLUSION-RULES-AFTER-CRS              |
| `SecRuleUpdateActionById`   | RESPONSE-999-EXCLUSION-RULES-AFTER-CRS              |
| `SecRuleUpdateTargetById`   | RESPONSE-999-EXCLUSION-RULES-AFTER-CRS              |
| `SecRuleUpdateTargetByMsg`  | RESPONSE-999-EXCLUSION-RULES-AFTER-CRS              |
| `SecRuleUpdateTargetByTag`  | RESPONSE-999-EXCLUSION-RULES-AFTER-CRS              |

## 示例

以下是一组示例，向您展示如何执行规则例外。

### 不检测可信IP的访问

通过此规则集，您可以控制 ModSecurity 如何处理源自授权漏洞扫描(AVS)源的流量。相关文章[HERE](http://blog.spiderlabs.com/2010/12/advanced-topic-of-the-week-handling-authorized-scanning-traffic.html)。白名单ASV网络块（不阻止或记录AVS流量）根据您的AVS流量更新IP网络块

```modsecurity
SecRule REMOTE_ADDR "@ipMatch 192.168.1.100" \
     "id:1000,\
     phase:1,\
     pass,\
     nolog,\
     ctl:ruleEngine=Off"
```

### 绕过某个规则中特定参数的检测

下面规则的意思是:当 `REQUEST_URI`(URL带参数) 为 `/index.php` 时，规则 `942100` 执行时不对参数名`password` 的参数值进行检测。

```modsecurity
SecRule REQUEST_URI "@beginsWith /index.php" \
     "id:1001,\
     phase:1,\
     pass,\
     nolog,\
     ctl:ruleRemoveTargetById=942100;ARGS:password"
```

### 特定参数不进某些攻击类型的检测

CRS中的攻击规则带有诸如 `attack-lfi`，`attack-sqli`，`attack-xss`，`attack-injection-php` 等标签。

ModSecurity规则排除：对于标记为Attack-sqli的所有规则，禁用对ARGS：pwd的检查

下面规则的意思是:当 `REQUEST_FILENAME`(URL不带参数) 以字符串`/wp-login.php` 结尾时，任何规则中有`attack-sqli` 的规则，不对参数名为`pwd`的参数值做检测

```modsecurity
SecRule REQUEST_FILENAME "@endsWith /wp-login.php" \
     "id:1002,\
     phase:2,\
     pass,\
     nolog,\
     ctl:ruleRemoveTargetByTag=attack-sqli;ARGS:pwd"
```

### 特定参数不做任何CRS规则检测

此规则说明，我们可以非常有效地使用标记将整个ModSecurity实例中的常见误报列入白名单。 可以这样做是因为OWASP_CRS中的每个规则都标记有OWASP_CRS。

 这不会影响自定义规则。

```modsecurity
SecRule REQUEST_FILENAME "@endsWith /wp-login.php" \
     "id:1003,\
     phase:2,\
     pass,\
     nolog,\
     ctl:ruleRemoveTargetByTag=OWASP_CRS;ARGS:pwd"
```

### 禁用一个区间内所有规则

下面规则的意思是:当 `REQUEST_FILENAME`(URL不带参数) 以字符串`/admin` 开头时，规则ID在`941000-942999`区间内的所有规则禁用。

```modsecurity
SecRule REQUEST_FILENAME "@beginsWith /admin" \
     "id:1004,\
     phase:2,\
     pass,\
     nolog,\
     ctl:ruleRemoveById=941000-942999"
```

The application specific rule exclusion files
REQUEST-903.9001-DRUPAL-EXCLUSION-RULES.conf

REQUEST-903.9002-WORDPRESS-EXCLUSION-RULES.conf
bring additional examples which can be useful then tuning a service.
