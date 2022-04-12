# MIME

MIME(Multipurpose Internet Mail Extensions)多用途互联网邮件扩展。


MIME类型又称为媒体类型(media type)，用来标识文档、文件或字节分类的性质(nature)和格式(format)。
MIME类型的标准是 [RFC 6838](https://datatracker.ietf.org/doc/html/rfc6838)。IANA组织负责所有官方的MIME类型，详细见 [MIME类型](https://www.iana.org/assignments/media-types/media-types.xhtml)。

浏览器使用 MIME类型 而不是 文件扩展名 来决定怎么处理 URL。所以服务端正确设置 `Content-Type` 是很重要的，如果配置错误，浏览器可能解析错误，网站无法正常工作，下载的文件也可能被错误的处理。



## MIME类型的结构

通常 MIME类型 由2部分组成: 父类型 和  子类型, 并且是用 `/` 分隔, 中间没有任何空白。

```mime
type/subtype
```

父类型表示的是数据的大分类，如 `video` 或 `text`。
子类型表示的是数据的具体类型，如 `text` 的子类型有  `plain`, `html`, `calendar` 等。

MIME类型还有一个可选的参数

```mime
type/subtype;parameter=value
```

例如，明确指定文本的编码格式，因为默认是 ACSII 编码

```mime
text/plain;charset=UTF-8
```

**MIME 类型不区分大小写，但传统上以小写形式编写。参数值可以区分大小写**。

---

## 父类型

父类型有两个分类 discrete(分离) 和 multipart(多部分)。离散类型是代表单个文件或媒体的类型，例如单个文本或音乐文件或单个视频。多部分类型是一种表示由多个组成部分组成的文档的类型，每个组成部分都可以有自己的独立 MIME 类型;或者，多部分类型可以封装在一个事务中一起发送的多个文件。例如，在将多个文件附加到电子邮件时使用多部分 MIME 类型。

### 离散型

目前的在IANA注册的离散型有 :

- `application` : 任何不明确归入其他类型的二进制数据, 或者是将以某种方式执行或解释的数据或需要特定应用程序或应用程序类别才能使用的二进制数据。 如 `application/zip`, `application/pdf`
- `audio` : 音频或音乐数据。例如, `audio/mpeg`, `audio/vorbis`. 
- `example` : 保留用作显示如何使用 MIME 类型的示例中的占位符
- `font` : 字体/字体数据
- `image` : 图像或图形数据，包括位图和矢量静止图像以及静止图像格式的动画版本，例如动画 GIF 或 APNG
- `model` : 3D 对象或场景的模型数据
- `text` : 纯文本数据，包括任何人类可读的内容、源代码或文本数据，例如逗号分隔值 (CSV) 格式的数据
- `video` : 视频数据或文件，例如 MP4 电影 `video/mp4

对于没有特定子类型的文本文档，应该使用 `text/plain`。同样，对于没有特定或已知子类型的二进制文档，应使用 `application/octet-stream`

### 多部分型

`multipart` 类型表示被分成几部分的文档类别，通常具有不同的 MIME 类型。
它们还可以用于（尤其是在电子邮件场景中）来表示多个单独的文件，这些文件都是同一事务的一部分。 它们代表一个复合文档。

- `multipart/form-data` : 用于使用POST方法上传HTML表单
- `multipart/byteranges` : 与响应 206 Partial Content 一起使用, 表示发送的是文档的一部分, 常见的功能如 HTTP断点续传 或者 多线(进)程下载同一个文件 都是使用这种方式。
- `message` : 封装其他消息的消息, 如果 `message/news`

HTTP 并没有以特殊方式处理除了 `multipart/form-data` 和 `multipart/byteranges` 以外的 multipart 文档的能力：消息被传输到浏览器，如果浏览器不知道如何显示文档，它可能会显示“另存为”窗口）。



