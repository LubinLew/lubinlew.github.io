# PostgreSQL 通信协议

https://www.postgresql.org/docs/current/protocol-overview.html

## 55.1.概述(Overview)

55.1.1. Messaging Overview
55.1.2. Extended Query Overview
55.1.3. Formats and Format Codes

The protocol has separate phases for startup and normal operation. 
In the startup phase, the frontend opens a connection to the server and authenticates itself to the satisfaction of the server. (This might involve a single message, or multiple messages depending on the authentication method being used.) 
If all goes well, the server then sends status information to the frontend, and finally enters normal operation. 
Except for the initial startup-request message, this part of the protocol is driven by the server.

> 协议分离为 `启动`(startup) 和 `正常操作`(normal operation) 两个阶段.
> 在`启动`阶段, 客户端建立链接到服务端(取决于认证方法的不同,这个操作可能包含单条或多条消息).
> 如果一些顺利,服务端会发送状态信息到客户端,并且进入`正常操作`阶段.

During normal operation, 
the frontend sends queries and other commands to the backend, 
and the backend sends back query results and other responses. 
There are a few cases (such as NOTIFY) wherein the backend will send unsolicited messages, 
but for the most part this portion of a session is driven by frontend requests.

> 在`正常操作`阶段, 客户端发送查询(query)和其他命令到服务端,
> 并且服务端发送查询结果和其他响应到客户端.
> 有几种特例(例如 NOTIFY)服务端会主动发送信息,
> 但在大多数情况下,会话的这一部分是由前端请求驱动的。

Termination of the session is normally by frontend choice, but can be forced by the backend in certain cases. 
In any case, when the backend closes the connection, 
it will roll back any open (incomplete) transaction before exiting.

> 通常会话的结束是由前端发起的, 但是在一些特定情况下后端可以强制结束会话.
> 无论如何，当后端关闭连接时，
> 它会在退出之前回滚任何打开的（不完整的）事务。

Within normal operation, SQL commands can be executed through either of two sub-protocols. 
In the “simple query” protocol, the frontend just sends a textual query string, which is parsed and immediately executed by the backend. 
In the “extended query” protocol, processing of queries is separated into multiple steps: parsing, binding of parameter values, and execution. This offers flexibility and performance benefits, at the cost of extra complexity.

> 在`正常操作`时, SQL 命令是通过两种子协议(sub-protocol)来执行的.
> - 简单查询(simple query)子协议: 前端仅发送一个文本查询字符串，该字符串由后端解析并立即执行
> - 扩展查询(extended query)子协议: 查询的处理分为多个步骤, 解析、参数值绑定和执行。这提供了灵活性和性能优势，但代价是额外的复杂性。

Normal operation has additional sub-protocols for special operations such as COPY.

> `正常操作`具有用于特殊操作（例如 COPY）的附加子协议。

### 55.1.1.消息传递概述(Messaging Overview)

All communication is through a stream of messages. 
The first byte of a message identifies the message type, and the next four bytes specify the length of the rest of the message (this length count includes itself, but not the message-type byte). 
The remaining contents of the message are determined by the message type. 
For historical reasons, the very first message sent by the client (the startup message) has no initial message-type byte.

> 所有的通信都是通过消息流进行的。 
> 消息的第一个字节标识消息类型，接下来的四个字节指定消息其余部分的长度（此长度计数包括其本身，但不包括消息类型字节）。
> 消息的其余内容由消息类型决定。由于历史原因，客户端发送的第一条消息（启动消息）没有初始消息类型字节。

To avoid losing synchronization with the message stream, 
both servers and clients typically read an entire message into a buffer (using the byte count) 
before attempting to process its contents. 
This allows easy recovery if an error is detected while processing the contents. 
In extreme situations (such as not having enough memory to buffer the message), 
the receiver can use the byte count to determine how much input to skip before it resumes reading messages.

> 为了避免失去与消息流的同步，服务器和客户端通常在尝试处理其内容之前将整个消息读入缓冲区（使用字节计数）。
> 如果在处理内容时检测到错误，这可以轻松恢复。
> 在极端情况下（例如没有足够的内存来缓冲消息），接收方可以使用字节计数来确定在恢复读取消息之前要跳过多少输入。

Conversely, both servers and clients must take care never to send an incomplete message. 
This is commonly done by marshaling the entire message in a buffer before beginning to send it. 
If a communications failure occurs partway through sending or receiving a message, 
the only sensible response is to abandon the connection, 
since there is little hope of recovering message-boundary synchronization.

> 相反，服务器和客户端都必须注意不要发送不完整的消息。
> 这通常是通过在开始发送之前将整个消息编组到缓冲区中来完成的。
> 如果在发送或接收消息的过程中发生通信故障，唯一明智的操作是放弃连接，因为恢复消息边界同步的希望很小。

### 55.1.2.扩展查询概述(Extended Query Overview)

In the extended-query protocol, execution of SQL commands is divided into multiple steps. 
The state retained between steps is represented by two types of objects: prepared statements and portals. 
A prepared statement represents the result of parsing and semantic analysis of a textual query string. 
A prepared statement is not in itself ready to execute, because it might lack specific values for parameters. 
A portal represents a ready-to-execute or already-partially-executed statement, with any missing parameter values filled in.
(For SELECT statements, a portal is equivalent to an open cursor, but we choose to use a different term since cursors don't handle non-SELECT statements.)

> 在`扩展查询`协议中，SQL命令的执行被分为多个步骤。
> 步骤之间保留的状态由两种类型的对象表示：准备好的语句(prepared statements)和入口(portal)。
> 准备好的语句表示文本查询字符串的解析和语义分析的结果。
> 准备好的语句本身并没有准备好执行，因为它可能缺少参数的特定值。 
> 入口代表一条准备执行或已部分执行的语句，并填充了所有缺失的参数值。
>（对于 SELECT 语句，入口相当于打开的游标，但我们选择使用不同的术语，因为游标不 不处理非 SELECT 语句。）

The overall execution cycle consists of a parse step, which creates a prepared statement from a textual query string;
a bind step, which creates a portal given a prepared statement and values for any needed parameters;
and an execute step that runs a portal's query.
In the case of a query that returns rows (SELECT, SHOW, etc.),
the execute step can be told to fetch only a limited number of rows,
so that multiple execute steps might be needed to complete the operation.

> 整个执行周期包括下面步骤:
> 
> - 解析步骤，该步骤从文本查询字符串创建一个准备好的语句； 
> - 绑定步骤，根据准备好的语句和任何所需参数的值创建一个入口； 
> - 执行步骤
> 
> 对于返回行（SELECT、SHOW 等）的查询，可以告诉执行步骤仅获取有限数量的行，因此可能需要多个执行步骤才能完成操作。

The backend can keep track of multiple prepared statements and portals (but note that these exist only within a session, and are never shared across sessions). 
Existing prepared statements and portals are referenced by names assigned when they were created. 
In addition, an “unnamed” prepared statement and portal exist. 
Although these behave largely the same as named objects, operations on them are optimized for the case of executing a query only once and then discarding it, whereas operations on named objects are optimized on the expectation of multiple uses.

> 后端可以跟踪多个准备好的语句和入口（但请注意，这些仅存在于会话中，并且永远不会在会话之间共享）。 现有准备好的语句和门户通过创建时指定的名称进行引用。 此外，还存在`未命名`的准备好的语句和入口。 尽管它们的行为与命名对象大致相同，但对它们的操作针对仅执行一次查询然后丢弃它的情况进行了优化，而对命名对象的操作针对多次使用的预期进行了优化。

### 55.1.3.格式和格式代码(Formats and Format Codes)

Data of a particular data type might be transmitted in any of several different formats. As of PostgreSQL 7.4 the only supported formats are “text” and “binary”, but the protocol makes provision for future extensions. The desired format for any value is specified by a format code. Clients can specify a format code for each transmitted parameter value and for each column of a query result. Text has format code zero, binary has format code one, and all other format codes are reserved for future definition.

特定数据类型的数据可以以几种不同格式中的任何一种来传输。 从 PostgreSQL 7.4 开始，唯一支持的格式是“文本”和“二进制”，但该协议为未来的扩展做好了准备。 任何值所需的格式都由格式代码指定。 客户端可以为每个传输的参数值和查询结果的每一列指定格式代码。 文本的格式代码为零，二进制的格式代码为一，所有其他格式代码都保留供将来定义。

The text representation of values is whatever strings are produced and accepted by the input/output conversion functions for the particular data type. In the transmitted representation, there is no trailing null character; the frontend must add one to received values if it wants to process them as C strings. (The text format does not allow embedded nulls, by the way.)

值的文本表示是特定数据类型的输入/输出转换函数生成和接受的任何字符串。 在传输的表示中，没有尾随空字符； 如果前端想要将接收到的值作为 C 字符串进行处理，则必须将其加一。 （顺便说一句，文本格式不允许嵌入空值。）

Binary representations for integers use network byte order (most significant byte first). For other data types consult the documentation or source code to learn about the binary representation. Keep in mind that binary representations for complex data types might change across server versions; the text format is usually the more portable choice.

整数的二进制表示使用网络字节顺序（最高有效字节在前）。 对于其他数据类型，请参阅文档或源代码以了解二进制表示形式。 请记住，复杂数据类型的二进制表示可能会因服务器版本而异； 文本格式通常是更便携的选择。

----

## 55.2. Message Flow


