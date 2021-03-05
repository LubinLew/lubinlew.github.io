# modsecurity.conf

> [ModSecurity/modsecurity.conf-recommended](https://github.com/SpiderLabs/ModSecurity/blob/v3/master/modsecurity.conf-recommended)



## 规则引擎初始化

启用ModSecurity，将其附加到每个事务。 仅从开始就使用检测，因为这样可以最大程度地减少安装后中断的机会。

```bash
SecRuleEngine DetectionOnly
```

## 请求体处理

允许ModSecurity访问请求体。 否则，ModSecurity将无法查看任何POST参数，从而导致巨大的安全漏洞。

```bash
SecRequestBodyAccess On
```

启用XML请求主体解析器。

在xml内容类型的情况下启动XML Processor

```bash
SecRule REQUEST_HEADERS:Content-Type "(?:application(?:/soap\+|/)|text/)xml" \
     "id:'200000',phase:1,t:none,t:lowercase,pass,nolog,ctl:requestBodyProcessor=XML"
```



启用JSON请求主体解析器。 

如果是JSON内容类型，则启动JSON处理程序； 如果您的应用程序不使用“ application / json”，则进行相应的更改

```bash
SecRule REQUEST_HEADERS:Content-Type "application/json" \
     "id:'200001',phase:1,t:none,t:lowercase,pass,nolog,ctl:requestBodyProcessor=JSON"
```

我们将接受的最大请求正文大小。 如果您支持文件上传，则第一行给出的值必须与您愿意接受的最大文件一样大。 第二个值是指数据大小，不包括文件。 您希望使该值尽可能低。

```bash
SecRequestBodyLimit 13107200
SecRequestBodyNoFilesLimit 131072
```

如果请求正文大小超出我们配置的限制，该怎么办。 请记住，此设置将自动设置为 SecRuleEngine设置为DetectionOnly模式时为rocessPartial，以便在最初部署ModSecurity时最大程度地减少中断。

```bash
SecRequestBodyLimitAction Reject
```

验证我们已正确处理了请求正文。
  根据经验，当无法处理请求正文时，您应该拒绝该请求（在以阻止模式部署时）或记录一个高严重性警报（在以仅检测模式部署时）。

```bash
SecRule REQBODY_ERROR "!@eq 0" \
"id:'200002', phase:2,t:none,log,deny,status:400,msg:'Failed to parse request body.',logdata:'%{reqbody_error_msg}',severity:2"

```

默认情况下，严格遵守我们在multipart / form-data中接受的内容
  请求正文。 如果以下规则对您的环境而言过于严格，请考虑将其更改为“仅检测”。 你是
  建议不要将其完全删除。

```bash
SecRule MULTIPART_STRICT_ERROR "!@eq 0" \
"id:'200003',phase:2,t:none,log,deny,status:400, \
msg:'Multipart request body failed strict validation: \
PE %{REQBODY_PROCESSOR_ERROR}, \
BQ %{MULTIPART_BOUNDARY_QUOTED}, \
BW %{MULTIPART_BOUNDARY_WHITESPACE}, \
DB %{MULTIPART_DATA_BEFORE}, \
DA %{MULTIPART_DATA_AFTER}, \
HF %{MULTIPART_HEADER_FOLDING}, \
LF %{MULTIPART_LF_LINE}, \
SM %{MULTIPART_MISSING_SEMICOLON}, \
IQ %{MULTIPART_INVALID_QUOTING}, \
IP %{MULTIPART_INVALID_PART}, \
IH %{MULTIPART_INVALID_HEADER_FOLDING}, \
FL %{MULTIPART_FILE_LIMIT_EXCEEDED}'"
```

我们看到任何可能成为边界的东西了吗？

这是有关ModSecurity Multipart解析器的简短描述：如果所有“类边界”行都与匹配，则解析器返回值为0。
  MIME标头中给出的边界字符串。 在任何其他情况下，其返回的值都不同，例如。 1或2。

RFC 1341描述了多部分内容类型，其语法必须仅包含三行强制性内容（位于内容之上）：

* Content-Type: multipart/mixed; boundary=BOUNDARY_STRING
* --BOUNDARY_STRING
* --BOUNDARY_STRING--

第一行表明这是一个多部分内容，第二行表明从这里开始是多部分内容的一部分，第三个显示了内容的结尾。
如果还有其他以“-”开头的行，则它应该是另一个边界ID-或不是。

在3.0.3之后，边界误差有两种类型：严格误差和允许误差。

 如果多部分内容包含三个顺序正确的必要行，但
 如果有一行或多行带有“-”，则解析器返回的值为2（非零）。

 如果某些必要的行（通常是开始或结束）未命中或命令
 是错误的，则解析器返回的值为1（也是非零）。

 您可以选择所需的那一个。以下示例包含
 “严格”模式，这意味着如果有任何以“-”开头的行，则
 ModSecurity阻止了内容。但是下一个已注释的示例包含
 “宽松”模式，那么您仅检查是否存在必要的行
 正确的顺序。这样，您可以启用上载PEM文件（例如“ ---- BEGIN ..”），
 或其他文本文件，例如包含HTTP标头。

 区别仅在于操作员-在严格模式下（首先），内容被阻止
 在任何非零值的情况下。在宽容模式下（第二个注释）
 仅当值是显式1时，内容才会被阻止。如果值为0或2，则内容将
 允许的。

```bash
 See 1747 and 1924 for further information on the possible values for
 MULTIPART_UNMATCHED_BOUNDARY.

SecRule MULTIPART_UNMATCHED_BOUNDARY "@eq 1" \
    "id:'200004',phase:2,t:none,log,deny,msg:'Multipart parser detected a possible unmatched boundary.'"

```

 PCRE Tuning
 We want to avoid a potential RegEx DoS condition

```bash

SecPcreMatchLimit 1000
SecPcreMatchLimitRecursion 1000

```

一些内部错误将在TX中设置标志，我们将需要查找这些标志。
  所有这些都以“ MSC_”为前缀。 当前存在以下标志：

  MSC_PCRE_LIMITS_EXCEEDED：超出了PCRE匹配限制。

```bash
SecRule TX:/^MSC_/ "!@streq 0" \
        "id:'200005',phase:2,t:none,deny,msg:'ModSecurity internal error flagged: %{MATCHED_VAR_NAME}'"

```

## 响应体处理

允许ModSecurity访问响应主体。
  您应该启用此伪指令以识别错误和数据泄漏问题。

  请记住，启用此指令确实会增加内存消耗和响应延迟。

```bash
SecResponseBodyAccess On
```

您要检查哪些响应MIME类型？ 您应调整以下配置以捕获文档，但避免使用静态文件（例如图像和档案）。

```bash
SecResponseBodyMimeType text/plain text/html text/xml
```

缓冲区响应主体的长度最大为512 KB。

```bash
SecResponseBodyLimit 524288
```

当我们遇到大于配置的限制的响应正文时，会发生什么？ 默认情况下，我们处理所拥有的东西，然后让其余部分通过。 这不太安全，但是不会破坏任何合法页面。

```bash
SecResponseBodyLimitAction ProcessPartial
```

## 文件系统配置

ModSecurity存储临时文件的位置（例如，当它需要处理大于配置的限制的文件上载时）。

  由于所有系统都有/ tmp可用，因此选择了默认设置，但是，这并不理想。 建议您指定一个私有位置。

```bash
SecTmpDir /tmp/
```

ModSecurity将保留其持久数据的位置。 由于所有系统都有/ tmp可用，因此选择了此默认设置，但是，也应将其更新到其他用户无法访问的位置。

```bash
SecDataDir /tmp/
```



## 文件上传处理配置

ModSecurity存放拦截的上传文件的位置。 此位置必须对ModSecurity私有。 您不希望服务器上的其他用户访问文件，是吗？

```bash
SecUploadDir /opt/modsecurity/var/upload/
```

默认情况下，仅保留通过某种方式（通过外部检查脚本）确定为异常的文件。 为此，您还需要至少一个文件检查规则。

```bash
SecUploadKeepFiles RelevantOnly
```

默认情况下，创建的上载文件具有不允许任何其他用户访问它们的权限。 如果要将ModSecurity连接到外部程序（例如，防病毒程序），则可能需要放松一下。

```bash
SecUploadFileMode 0600
```



## 调试日志配置

默认的调试日志配置是复制错误日志中的错误，警告和注意消息。

```bash
SecDebugLog /opt/modsecurity/var/log/debug.log
SecDebugLogLevel 3
```



## 审核日志配置

记录由规则标记的事务以及触发服务器错误的事务（由5xx或4xx确定，不包括404级响应状态代码）。

```bash
SecAuditEngine RelevantOnly
SecAuditLogRelevantStatus "^(?:5|4(?!04))"
```



记录我们所了解的有关交易的所有信息。

```bash
SecAuditLogParts ABIJDEFHZ
```

使用单个文件进行日志记录。 这看起来很容易，但是假定您仅偶尔使用审核日志。

```bash
SecAuditLogType Serial
SecAuditLog /var/log/modsec_audit.log
```

指定并发审核日志记录的路径。

```bash
SecAuditLogStorageDir /opt/modsecurity/var/audit/
```



## 杂项

使用最常用的application / x-www-form-urlencoded参数分隔符。 某个地方可能只有一个应用程序在使用其他应用程序，因此不要期望更改此值。

```bash
SecArgumentSeparator &
```

设置版本0（零）Cookie，因为这是大多数应用程序所使用的。 使用不正确的cookie版本可能会使您的安装遭受规避攻击（违反检查命名cookie的规则）。

```bash
SecCookieFormat 0
```

指定您的Unicode代码点。 t：urlDecodeUni转换功能使用此映射将编码后的数据正确映射到您的语言。 正确设置这些指令有助于减少误报和误报。

```bash
SecUnicodeMapFile unicode.mapping 20127
```

通过共享有关当前ModSecurity版本和依赖项版本的信息来提高ModSecurity的质量。
  将共享以下信息：ModSecurity版本，Web服务器版本，APR版本，PCRE版本，Lua版本，Libxml2版本，主机的匿名唯一ID。

```bash
SecStatusEngine On
```
