# crs-setup.conf

> [coreruleset/crs-setup.conf.example at v3.4/dev](https://github.com/coreruleset/coreruleset/blob/v3.4/dev/crs-setup.conf.example)

## 配置模式

OWASP V3核心规则集目前支持两种配置模式：<mark>异常评分模式</mark>(默认) 和 <mark>独立控制模式</mark>。

### 异常评分模式(Anomaly Scoring)

也可称为 `协同检测模式`(collaborative detection mode)，这是默认且推荐的模式，因为它提供了最准确的日志信息，并且在设置阻止策略方面提供了最大的灵活性。

此模式的工作原理是：当检测到威胁时，并不会直接阻断此次访问，而是向下继续进行规则匹配，每个匹配成功的规则都会增加 `异常分数`，在对客户端发起的请求数据检测结束时，以及对返回数据检测结束时，都会对 `异常分数` 的总和进行判断，如果大于设置的阈值，才会进行阻断动作，审计日志中也会记录此次访问中所有匹配成功的规则信息。

### 独立控制模式(Self-Contained)

此模式是V2版本规则集的默认模式，它的工作方式是，只要有一条规则匹配成功，便拦截此次访问，审计日志中也只会记录第一次检测到威胁的规则信息。

它可以降低资源使用率，但代价是在阻止策略方面的灵活性较小，而信息日志记录的信息较少（仅记录第一个检测到的威胁）。
规则会继承您指定的破坏性操作（即拒绝，丢弃等）。第一个匹配的规则将执行此操作。 在大多数情况下，这将导致在第一个规则匹配后停止评估。



具体配置见下一个章节 [日志设置](#日志设置) 。

---

## 日志设置

在模式配置中，还必须调整所需的日志记录选项。共有4个日志选项。

| 日志选项         | 说明                      | 备注                        |
| ------------ | ----------------------- | ------------------------- |
| `log`        | 日志记录到web服务器的错误日志中       | 例如会记录到nginx的error.log 日志中 |
| `nolog`      | 日志**不会**记录到web服务器的错误日志中 | -                         |
| `auditlog`   | 启动审计日志                  | 会记录到指令`SecAuditLog`指定的文件中 |
| `noauditlog` | 停用审计日志                  | -                         |

 默认情况下，CRS启用对Web服务器错误日志（或事件查看器）的日志记录以及对ModSecurity审核日志。



### 配置例子

各种模式的示例如下。您必须启用以下选项之一。注意，您必须为`phase:1`和`phase:2`指定。

| 阶段  | 英文说明             | 中文说明         |
| --- | ---------------- | ------------ |
| 1   | Request headers  | 完成读取请求头      |
| 2   | Request body     | 检查请求体        |
| 3   | Response headers | 响应头被发送回客户端之前 |
| 4   | Response body    | 检查响应体        |
| 5   | Logging          | 日志记录发生之前运行   |



#### 默认值

默认值：异常评分模式，记录到错误日志，记录到ModSecurity审核日志

默认情况下，使用错误403响应阻止违规请求。

要更改破坏性行为，请参阅RESPONSE-999-EXCLUSION-RULES-AFTER-CRS.conf.example，并查看“更改异常模式的破坏性行为”部分。

在Apache中，您可以使用ErrorDocument显示友好的错误页面或执行重定向：https://httpd.apache.org/docs/2.4/custom-error.html

```bash
SecDefaultAction "phase:1,log,auditlog,pass"
SecDefaultAction "phase:2,log,auditlog,pass"
```

#### 异常评分模式,只记录审计日志

- 默认情况下，违规请求会被阻止，并显示错误403响应
- 要更改破坏性行为，请参阅RESPONSE-999-EXCLUSION-RULES-AFTER-CRS.conf.example，并查看“更改异常模式的破坏性行为”部分。
- 在Apache中，您可以使用ErrorDocument显示友好的错误页面或执行重定向：[Custom Error Responses - Apache HTTP Server Version 2.4](https://httpd.apache.org/docs/2.4/custom-error.html)

```bash
SecDefaultAction "phase:1,nolog,auditlog,pass"
SecDefaultAction "phase:2,nolog,auditlog,pass"
```

#### 独立控制模式, 阻止后返回403页面

- 在此配置中，默认的破坏性操作变为“deny”。 规则触发后，它将停止处理请求并返回错误403。
- 您还可以使用其他错误状态，例如404、406等。
- 在Apache中，您可以使用ErrorDocument显示友好的错误页面或执行重定向: https://httpd.apache.org/docs/2.4/custom-error.html

```bash
SecDefaultAction "phase:1,log,auditlog,deny,status:403"
SecDefaultAction "phase:2,log,auditlog,deny,status:403"
```

#### 独立控制模式,阻止后返回主页

- 在此配置中，“标记”操作将主机标头数据包括在日志中。 这有助于确定哪个虚拟主机触发了规则（如果有）。
- 请注意，在某些情况下，这可能会导致重定向循环。 例如，如果Cookie或User-Agent标头被阻止，则当客户端随后尝试访问首页时，它也将被阻止。 您还可以重定向到另一个自定义URL。

```bash
SecDefaultAction "phase:1,log,auditlog,redirect:'http://%{request_headers.host}/',tag:'Host: %{request_headers.host}'"
SecDefaultAction "phase:2,log,auditlog,redirect:'http://%{request_headers.host}/',tag:'Host: %{request_headers.host}'"
```



---

## 嫌疑级别设置

嫌疑级别(PL:Paranoia Level)设置使您可以选择所需的规则检查级别，以增加您的异常分数。

随着每一个嫌疑级别的提高，CRS会启用其他规则，从而为您提供更高级别的安全性。 但是，较高的嫌疑级别也增加了由于误报（也称为误报或FP）而阻止某些合法流量的可能性。 如果您使用较高的嫌疑级别，则可能需要为某些请求和接收复杂输入的应用程序添加一些例外规则。

- 嫌疑级别默认为1。 在此级别，启用了大多数核心规则。 建议PL1适用于初学者，涵盖许多不同站点和应用程序的安装以及具有标准安全性要求的设置。 在PL1，您应该很少面对FP。 如果遇到FP，请在CRS GitHub网站上打开问题，并且不要忘记为问题的请求附加完整的审核日志记录。

- 嫌疑级别2包含许多额外的规则，例如，启用许多基于正则表达式的SQL和XSS注入保护，并添加检查代码注入的额外关键字。 建议PL2适用于中级到有经验的用户，他们需要更完整的覆盖范围以及对安全性有更高要求的安装。 PL2随附一些您需要处理的FP。

- 嫌疑级别3级启用了更多规则和关键字列表，并调整了所用特殊字符的限制。 PL3面向经验丰富的FP处理人员和对安全性有较高要求的安装的用户。

- 嫌疑级别4进一步限制了特殊字符。 建议经验丰富的用户以最高级别来保护具有很高安全性要求的安装。 运行PL4可能会产生大量的FP，必须先对其进行处理才能使站点正常工作。

所有规则都会将其PL记录到审核日志中； 例如：`[tag "paranoia-level/2"]`。 这使您可以从审核日志中推论WAF行为如何受到嫌疑级别的影响。

同样重要的是，还要研究下面定义的变量`tx.enforce_bodyproc_urlencoded`（强制主体处理器URLENCODED）。 启用它会关闭CRS的可能旁路。

```bash
SecAction \
  "id:900000,\
   phase:1,\
   nolog,\
   pass,\
   t:none,\
   setvar:tx.paranoia_level=1"
```

可以从较高的嫌疑级别执行规则，但不将其包括在异常评分中。 这样一来，您就可以在1级嫌疑中采用经过良好调整的系统，并从2级嫌疑中添加规则，而不必担心新规则会导致误报，从而使您的分数超过阈值。通过取消注释以下规则并设置`tx.executing_paranoia_level`来启用此可选功能。

从技术上讲，将执行不超过`tx.executing_paranoia_level`中定义的级别的规则，但是只有不超过`tx.paranoia_level`的规则才会影响异常分数。

默认情况下，tx.executing_paranoia_level设置为tx.paranoia_level。
  tx.executing_paranoia_level不得低于tx.paranoia_level。

请注意，将tx.executing_paranoia_level设置为较高的妄想级别会导致对性能的影响，与将tx.paranoia_level设置为该级别一样高。

```bash
SecAction \
  "id:900001,\
   phase:1,\
   nolog,\
   pass,\
   t:none,\
   setvar:tx.executing_paranoia_level=1"
```

---

## Enforce Body Processor URLENCODED

ModSecurity 根据 `Content-Type` 请求标头选择body处理器。 但是客户端并不总是为他们的请求主体有效负载设置`Content-Type`标头。 这将使ModSecurity对有效载荷的视野有限。 使用变量`tx.enforce_bodyproc_urlencoded`可以在这些情况下强制使用URLENCODED主体处理器。 默认情况下，此功能处于关闭状态，因为这意味着ModSecurity的行为已超出CRS（主体处理器不仅适用于CRS，而且适用于所有规则），而且还可能导致偏执级别1的误报。但是，启用此变量 关闭可能的CRS旁路，因此应予以考虑。

```bash
SecAction \
  "id:900010,\
   phase:1,\
   nolog,\
   pass,\
   t:none,\
   setvar:tx.enforce_bodyproc_urlencoded=1"
```

---

## Anomaly Scoring Mode Severity Levels

CRS中的每个规则都有一个关联的严重性级别。 这些是每个严重性级别的默认得分。 如果规则匹配，这些设置将用于增加异常分数。 您可以根据自己的喜好调整这些点，但这通常是不需要的。

| 级别       | 评分  | 说明                                                                   |
| -------- | --- | -------------------------------------------------------------------- |
| CRITICAL | 5   | Mostly generated by the application attack rules (93x and 94x files) |
| ERROR    | 4   | Generated mostly from outbound leakage rules (95x files)             |
| WARNING  | 3   | Generated mostly by malicious client rules (91x files)               |
| NOTICE   | 2   | Generated mostly by the protocol rules (92x files)                   |

在异常模式下，这些分数是累积的。 因此，一个请求可能会遇到多个规则。

注意：在此文件中，我们使用'phase：1'设置CRS配置变量。
通常，使用“ phase：request”。 但是，我们要绝对确保在处理CRS规则之前已设置所有配置变量。

```bash
SecAction \
 "id:900100,\
  phase:1,\
  nolog,\
  pass,\
  t:none,\
  setvar:tx.critical_anomaly_score=5,\
  setvar:tx.error_anomaly_score=4,\
  setvar:tx.warning_anomaly_score=3,\
  setvar:tx.notice_anomaly_score=2"
```

----

## 异常计分模式的阈值设置

在这里，您可以指定阻止入站请求或出站响应的累积异常分数。大多数检测到的入站威胁将给出5的严重评分。较小的违规（例如违反协议/标准）则得分较低。

[ At default value ]

如果将阻止阈值保持为默认值，则CRS的工作方式将与以前的CRS版本相似：单个关键规则匹配将导致请求被阻止并记录。

[ Using higher values ]

如果要降低CRS的敏感度，可以将阻止阈值提高到例如7（在阻止之前需要多个规则匹配）或10（需要至少两个严重警报-或许多其他较小警报的组合） ），甚至更高。 但是，提高阈值可能会导致某些攻击绕过CRS规则或您的策略。

[ New deployment strategy: Starting high and decreasing ]

通常，以较高的异常评分阈值（> 100）开始全新的CRS安装，然后随着对设置的信心增强而降低限制。 您也可以查看下面的“采样百分比”部分，以寻求另一种策略来简化安装新的CRS的过程。

[ Anomaly Threshold / Paranoia Level Quadrant ]

    High Anomaly Limit   |   High Anomaly Limit
    Low Paranoia Level   |   High Paranoia Level
    -> Fresh Site        |   -> Experimental Site

    Low Anomaly Limit    |   Low Anomaly Limit
    Low Paranoia Level   |   High Paranoia Level
    -> Standard Site     |   -> High Security Site

```bash
SecAction \
 "id:900110,\
  phase:1,\
  nolog,\
  pass,\
  t:none,\
  setvar:tx.inbound_anomaly_score_threshold=5,\
  setvar:tx.outbound_anomaly_score_threshold=4"
```

---

## 早期异常计分模式拦截

通常在阶段2结束时和阶段4结束时对请求和响应的异常分数进行汇总和评估。 但是，可以在阶段1结束时和阶段3结束时对这些异常分数进行早期评估。

如果请求（或响应）在此早期评估中达到异常阈值，那么将立即发生阻塞（如果启用了阻塞），并且将不再执行阶段2（和阶段4）。

启用将变量tx.blocking_early设置为1的规则900120，以启用早期阻止。 默认情况下，变量tx.blocking_early设置为0。 因此，默认情况下将禁用提前阻止。

请注意，尽早阻止将对您隐藏潜在的警报。 这意味着如果请求被提前阻止，则不会评估在阶段2（或阶段4）的警报中出现的有效负载。 因此，当您在将来某个时候再次禁用早期阻止功能时，第二阶段的新警报可能会弹出。

```bash
SecAction \
  "id:900120,\
  phase:1,\
  nolog,\
  pass,\
  t:none,\
  setvar:tx.blocking_early=1"
```



---

## Application Specific Rule Exclusions

一些知名的应用程序可能会执行似乎是恶意的操作。 这包括在参数中允许HTML或Javascript之类的操作。 在这种情况下，CRS旨在通过允许管理员逐个应用程序启用预先构建的，特定于应用程序的排除项来防止误报。

这些特定于应用程序的排除项不同于为特定应用程序预先构建的，将放置在REQUEST-900-EXCLUSION-RULES-BEFORE-CRS配置文件中的规则。 “ REQUEST-900”文件旨在让用户添加自己的自定义排除项。 请注意，使用这些特定于应用程序的排除项可能会放宽对CRS的限制，尤其是如果将其与并非针对其设计的应用程序一起使用时。 因此，应谨慎使用它们。

要使用此功能，必须指定受支持的应用程序。 为此，请取消注释规则900130。除了取消注释规则之外，您还需要指定要为其启用排除的应用程序。 当前仅支持（非常）有限的一组应用程序，请使用带有“ REQUEST-903”前缀的文件名来指导您进行选择。 这样的文件名使用以下约定：REQUEST-903.9XXX- {APPNAME} -EXCLUSIONS-RULES.conf

建议您在站点上运行多个Web应用程序，以使用类似于以下示例的规则将排除的影响限制为仅将排除的Web应用程序驻留的路径：SecRule REQUEST_URI“ @beginsWith / wordpress /” setvar：tx.crs_exclusions_wordpress = 1

```bash
SecAction \
 "id:900130,\
  phase:1,\
  nolog,\
  pass,\
  t:none,\
  setvar:tx.crs_exclusions_cpanel=1,\
  setvar:tx.crs_exclusions_dokuwiki=1,\
  setvar:tx.crs_exclusions_drupal=1,\
  setvar:tx.crs_exclusions_nextcloud=1,\
  setvar:tx.crs_exclusions_phpbb=1,\
  setvar:tx.crs_exclusions_phpmyadmin=1,\
  setvar:tx.crs_exclusions_wordpress=1,\
  setvar:tx.crs_exclusions_xenforo=1"
```

---

## HTTP策略设置

本部分定义了HTTP协议的策略，例如：允许的HTTP版本，HTTP方法，允许的请求的 `Content-Type` ,禁止的文件扩展名（例如.bak，.sql）和请求头（例如 Proxy）

These variables are used in the following rule files:

- REQUEST-911-METHOD-ENFORCEMENT.conf
- REQUEST-912-DOS-PROTECTION.conf
- REQUEST-920-PROTOCOL-ENFORCEMENT.conf

| 变量名                                       | 说明                        | 默认值                                                                                                                                                                                                                                                                                                                                                            |
| ----------------------------------------- | ------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `tx.allowed_methods`                      | 允许的请求方法                   | GET HEAD POST OPTIONS                                                                                                                                                                                                                                                                                                                                          |
| `tx.allowed_request_content_type`         | 请求头中允许的`Content-Type`类型   | application/x-www-form-urlencoded<br>multipart/form-data<br>multipart/related<br>text/xml<br>application/xml<br>application/soap+xml<br>application/x-amf<br>application/json<br>application/cloudevents+json<br>application/cloudevents-batch+json<br>application/octet-stream<br>application/csp-report<br>application/xss-auditor-report<br>text/plain<br>  |
| `tx.allowed_http_versions`                | 允许的HTTP版本                 | HTTP/1.0<br> HTTP/1.1<br>HTTP/2<br>HTTP/2.0                                                                                                                                                                                                                                                                                                                    |
| `tx.restricted_extensions`                | 禁止的文件扩展名                  | .asa/ .asax/ .ascx/ .axd/ .backup/ .bak/ .bat/ .cdx/ .cer/ .cfg/ .cmd/ .com/ .config/ .conf/ .cs/ .csproj/ .csr/ .dat/ .db/ .dbf/ .dll/ .dos/ .htr/ .htw/ .ida/ .idc/ .idq/ .inc/ .ini/ .key/ .licx/ .lnk/ .log/ .mdb/ .old/ .pass/ .pdb/ .pol/ .printer/ .pwd/ .rdb/ .resources/ .resx/ .sql/ .swp/ .sys/ .vb/ .vbs/ .vbproj/ .vsdisco/ .webinfo/ .xsd/ .xsx/ |
| `tx.restricted_headers`                   | 禁止的请求标头                   | /proxy/ /lock-token/ /content-range/ /if/                                                                                                                                                                                                                                                                                                                      |
| `tx.static_extensions`                    | 文件扩展名被视为静态文件              | /.jpg/ /.jpeg/ /.png/ /.gif/ /.js/ /.css/ /.ico/ /.svg/ /.webp/                                                                                                                                                                                                                                                                                                |
| `tx.allowed_request_content_type_charset` | 允许客户端发送请求的Content-Type字符集 |                                                                                                                                                                                                                                                                                                                                                                |

---

## HTTP参数与上传限制

在这里，您可以定义HTTP get / post参数和上传的可选限制。 这有助于防止应用程序特定的DoS攻击。这些值在 `REQUEST-920-PROTOCOL-ENFORCEMENT.conf` 中进行检查。 当心使这些限制时阻止合法流量。

| 变量名                      | 说明         | 默认值       |
| ------------------------ | ---------- | --------- |
| `tx.max_num_args`        | 参数数量       | unlimited |
| `tx.arg_name_length`     | 参数名长度      | unlimited |
| `tx.arg_length`          | 参数值长度      | unlimited |
| `tx.total_arg_length`    | 所有参数的总长度   | unlimited |
| `tx.max_file_size`       | 单个上传的文件大小  | unlimited |
| `tx.combined_file_sizes` | 所有上传文件的总大小 | unlimited |

### 例子

设置最大允许参数个数，超过则则阻止请求

```bash
SecAction \
 "id:900300,\
  phase:1,\
  nolog,\
  pass,\
  t:none,\
  setvar:tx.max_num_args=255"
```

---

## 采样率设置

如果直接将核心规则集添加到现有的生产站点可能会导致误报，意外的性能问题和其他不良后果。可以通过将很少一部分请求发送的CRS检测，其他请求不通过CRS检测。然后，在解决问题（如果有）并且对设置有信心的情况下，提高发送到规则集中的请求的比率。

通过设置变量 `tx.sampling_percentage` 来调整被纳入“核心规则”的请求的百分比。 默认值为100，这意味着CRS将检查每个请求。 选择请求是基于ModSecurity生成的伪随机数。

如果允许请求通过而无需CRS进行检查，则审核日志中没有条目（出于性能原因），但是会写入错误日志条目。 如果要禁用错误日志条目，请在包含CRS之后的某个地方发出以下指令（例如 `RESPONSE-999-EXCLUSION-RULES-AFTER-CRS.conf`）。

```bash
SecRuleUpdateActionById 901150 "nolog"
```

注意：如果 `tx.sampling_percentage` 小于100，则某些请求将完全绕过核心规则，并且您将失去使用ModSecurity保护服务的能力。

```bash
SecAction "id:900400,\
  phase:1,\
  pass,\
  nolog,\
  setvar:tx.sampling_percentage=100"
```

---

## Project Honey Pot HTTP Blacklist

（可选）您可以对照 [Project Honey Pot HTTPBL](https://www.projecthoneypot.org/httpbl.php) 检查客户端IP地址。 为了使用此功能，您需要注册以获得免费的API密钥。 使用指令 `SecHttpBlKey` 进行设置。Project Honeypot 返回多个不同的恶意IP类型，您可以通过在下面启用或禁用它们来指定要阻止的对象。

```bash
SecHttpBlKey XXXXXXXXXXXXXXXXX

SecAction "id:900500,\
  phase:1,\
  nolog,\
  pass,\
  t:none,\
  setvar:tx.block_search_ip=1,\
  setvar:tx.block_suspicious_ip=1,\
  setvar:tx.block_harvester_ip=1,\
  setvar:tx.block_spammer_ip=1"
```

---

## 地域访问设置

有一些规则可以检查客户端IP地址的地理位置数据。 CRS可以使用地理位置信息实施对某些国家/地区屏蔽。为了获取地理位置信息，需要使用 [MaxMind](https://www.maxmind.com/) 公司的 GeoIP 数据库。有两个版本的GeoIP数据库。ModSecurity v2 使用GeoLite (.dat 文件，已经不再更新)，ModSecurity v3 使用 GeoLite2 (.mmdb 文件)，这里我们只讨论v3的GeoLite2。GeoLite2需要到MaxMind网站下载(需要注册账号才能下载)。

### 加载GeoIP数据库

下面是加载 GeoLite2 数据库的方法：

```bash
SecGeoLookupDB /usr/share/GeoIP/GeoLite2-Country.mmdb
```

### 地域规则设置

IP信誉文件(`REQUEST-910-IP-REPUTATION.conf`)中的规则可以对照高风险国家/地区代码列表检查客户端。国家/地区的简写代码，详见 [ISO 3166-1 alpha-2 - Wikipedia](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2#Officially_assigned_code_elements) 。如果确定没有从给定国家/地区收到任何合法请求，则可以通过此变量`tx.high_risk_country_codes`禁用来自该国家/地区的所有访问权限。 

```bash
SecAction \
 "id:900600,\
  phase:1,\
  nolog,\
  pass,\
  t:none,\
  setvar:'tx.high_risk_country_codes=UA ID YU LT EG RO BG TR RU PK MY CN'"
```

----

## 反爬虫与DoS防护

可选的DoS保护措施，可防止客户端发出过快的请求。

当客户端在60秒内发出100个以上的请求（不包括静态文件）时，这被视为“突发”。 在两次突发之后，客户端被阻塞600秒。对静态文件的请求不计入DoS； 它们在`tx.static_extensions`设置中列出，您可以在此文件中更改(详见 [HTTP策略设置](HTTP策略设置))。For a detailed description, see rule file `REQUEST-912-DOS-PROTECTION.conf`.

```bash
SecAction \
 "id:900700,\
  phase:1,\
  nolog,\
  pass,\
  t:none,\
  setvar:'tx.dos_burst_time_slice=60',\
  setvar:'tx.dos_counter_threshold=100',\
  setvar:'tx.dos_block_timeout=600'"
```

----

## 检测UTF-8编码

CRS可以选择检查请求内容中是否存在无效的UTF-8编码。 我们只想在网站实际使用UTF-8编码的情况下应用此检查； 否则会导致误报。默认不启用该功能。

```bash
SecAction \
 "id:900950,\
  phase:1,\
  nolog,\
  pass,\
  t:none,\
  setvar:tx.crs_validate_utf8_encoding=1"
```

---

## 基于IP信誉的阻止访问

基于信誉的阻止在CRS中是永久的。 与查看单个请求的其他规则不同，对IP的阻止基于IP集合中的持久记录，该记录在一定时间内保持活动状态。

可以通过两种方式将单个客户端标记为阻止：

- 外部信息 (RBL, GeoIP, etc.) 

- 内部信息 (Core Rules) 

IP集合中的记录带有一个标志，该标志使用名为`ip.reput_block_flag`的标志标记来自各个客户端的请求。 但是仅凭该标志不足以阻止客户。 还有一个名为`tx.do_reput_block`的全局开关。 默认情况下是关闭的。 如果将其设置为1（= On），则带有`IP.reput_block_flag`的客户端请求将被阻止一段时间。

### 变量

 IP信誉相关变量如下:

| 变量名                       | 说明               |
| ------------------------- | ---------------- |
| `ip.reput_block_flag`     | IP收集记录的阻止标志      |
| `ip.reput_block_reason`   | 导致阻断标志的原因（=规则消息） |
| `tx.do_reput_block`       | 决定是否根据标志进行阻止的开关  |
| `tx.reput_block_duration` | IP阻断时间           |

可能很重要的一点是，当很明显其他核心规则带有相关的阻塞标志时，所有其他核心规则都会被跳过。

#### 启用IP信誉检测

```bash
SecAction \
 "id:900960,\
  phase:1,\
  nolog,\
  pass,\
  t:none,\
  setvar:tx.do_reput_block=1"
```

#### 修改阻止时间

默认阻止时间为300秒(5分钟)

```bash
SecAction \
 "id:900970,\
  phase:1,\
  nolog,\
  pass,\
  t:none,\
  setvar:tx.reput_block_duration=300"
```

---

## Collection 超时设置

将 `SecCollectionTimeout` 指令从ModSecurity默认值（1小时）设置为较低的设置，该设置适用于大多数站点。通过清除陈旧的数据来提高性能。

这个值应该大于等于下面2个变量:
`tx.reput_block_duration` (详见 [基于IP信誉的阻止访问](#基于IP信誉的阻止访问)) 和`tx.dos_block_timeout` (详见 [反爬虫与DoS防护](#反爬虫与DoS防护)).

请不要对该指令进行注释，默认值：600秒（10分钟）

```bash
SecCollectionTimeout 600
```

---

## 结束设置

CRS检查 `tx.crs_setup_version` 变量以确保已正确加载设置。如果您不打算使用此设置模板，那么必须在包含CRS规则/ *文件之前手动设置`tx.crs_setup_version`变量。该变量是CRS版本号的数字表示。例如，v3.0.0表示为300。

```bash
SecAction \
 "id:900990,\
  phase:1,\
  nolog,\
  pass,\
  t:none,\
  setvar:tx.crs_setup_version=330"
```
