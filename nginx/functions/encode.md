# URL中编码

在浏览网站时, URL 中经常会看到 `%XY` 形式的字符。

`%XY` 形式的编码在 [RFC 1738](https://www.rfc-editor.org/rfc/rfc1738#section-2.2) 规定。

## 编码方案

URL中一个八位字节(Octet) 可由3个字符即 `%XY` 形式表示, 其中 `X` 和 `Y` 为16进制数字(`0123456789ABCDEF`中的一个,忽略大小写)。

需要的编码的情况主要有个下面三种：
- US-ASCII 编码字符集中没有相应的图形字符
- 如果相应字符的使用不安全
- 相应字符保留用于特定 URL 方案中的某些其他解释。

### 没有对应可打印图形 US-ASCII 字符

URL 仅使用 US-ASCII 编码字符集的中可打印的图形字符**编写**。

`0x80~0xFF` 未在US-ASCII中使用，`0x00~0x1F` 和 `0x7F` 表示控制字符，他们必须进行编码。

### URL中不安全的字符

有多种原因，使用字符可能不安全. 

- 字符 `空格` 是不安全的，带有空格的URL可以会被错误认为是两(或多)个不同的内容

- 字符 `<` 和 `>` 是不安全的，因为它们被用作自由文本中 URL 的分隔符, 如 `<http://www.test.com/>`

- 字符 `"` 是不安全的, 因为在某些系统中，其用于分隔 URL, 如 `"http://www.test.com/"`

- 字符 `#` 是不安全的，应该始终进行编码，因为其用于 URL 的锚, 如`http://www.test.com/page1.html#section2`

- 字符 `%` 是不安全的，因为它用于编码其他字符的

- 其他字符是不安全的，因为已知网关和其他传输代理有时会修改这些字符,他们是 `{`, `}`, `|`, `\`, `^`, `~` ,`[`, `]`, `\``.

所有不安全的字符必须始终在 URL 中进行编码。
例如，即使在通常不处理片段或锚标识符的系统中，字符`#`也必须在 URL 中进行编码，
如果将 URL 复制到使用它们的另一个系统中，也无需更改网址编码。

### URL 中的保留字符

许多 URL 方案将某些字符定义了特殊含义,它们在 URL 的方案特定部分中的出现具有指定的语义。
如果对应于八位字节的字符在方案中被保留，则必须对八位字节进行编码。

字符`;`、`/`、`?`、`:`、`@`、`=`和`&`是可以在方案中为特殊含义保留的字符。不得使用其他字符作为特殊含义。

通常，当八位字节由字符表示时和对其进行编码时，URL 具有相同的解释。
但是，对于保留字符则不是这样：对为特定方案保留的字符进行编码可能会改变 URL 的语义。

因此，只有字母数字、特殊字符`$-_.+!*'()`和用于其保留目的的保留字符可以在 URL 中未编码地使用。

另一方面，不需要编码的字符（包括字母数字）可以在 URL 的特定于方案的部分中编码，只要它们不被用于保留目的。

-----

## 编码攻击

URL编码使用带来便利的同时也引入了潜在的攻击。

使用 URL 编码可以绕过一些 安全产品（或工具）对 URL 的检测，

例如筛选出 SQL 类的关键字,则判定可能是 SQL注入攻击,对访问进行拦截。如果安全产品（或工具）未对URL进行解码，则会放行编码过的访问。

当 web 服务程序在处理这些特殊字符不当的时候，某些攻击会使用 `0x00`, `0x0A`, `0x0D`, `0x7F` 等特殊字符对攻击 payload 进行分隔,达到绕过安全产品的目的。







