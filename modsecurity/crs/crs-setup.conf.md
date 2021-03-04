# crs-setup.conf



## 配置模式

OWASP V3核心规则集目前支持两种配置模式：<mark>异常评分模式</mark>(默认) 和 <mark>独立控制模式</mark>。

### 异常评分模式(Anomaly Scoring)

也可称为 `协同检测模式`(collaborative detection mode)，这是默认且推荐的模式，因为它提供了最准确的日志信息，并且在设置阻止策略方面提供了最大的灵活性。

此模式的工作原理是：当检测到威胁时，并不会直接阻断此次访问，而是向下继续进行规则匹配，每个匹配成功的规则都会增加 `异常分数`，在对客户端发起的请求数据检测结束时，以及对返回数据检测结束时，都会对 `异常分数` 的总和进行判断，如果大于设置的阈值，才会进行阻断动作，审计日志中也会记录此次访问中所有匹配成功的规则信息。

### 独立控制模式(Self-Contained)

此模式是V2版本规则集的默认模式，它的工作方式是，只要有一条规则匹配成功，便拦截此次访问，审计日志中也只会记录第一次检测到威胁的规则信息。

它可以降低资源使用率，但代价是在阻止策略方面的灵活性较小，而信息日志记录的信息较少（仅记录第一个检测到的威胁）。
规则会继承您指定的破坏性操作（即拒绝，丢弃等）。第一个匹配的规则将执行此操作。 在大多数情况下，这将导致在第一个规则匹配后停止评估。

---

## Alert Logging Control

---

## Paranoia Level Initialization

---

## Enforce Body Processor URLENCODED

---

## Anomaly Scoring Mode Severity Levels

----

## Anomaly Scoring Mode Blocking Threshold Levels

---

## Early Anomaly Scoring Mode Blocking

---

## Application Specific Rule Exclusions





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
| `tx.allowed_request_content_type_charset` | 允许客户端发送请求的Content-Type字符集 | |utf-8\| \|iso-8859-1\| \|iso-8859-15\| \|windows-1252\|                                                                                                                                                                                                                                                                                                       |

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
