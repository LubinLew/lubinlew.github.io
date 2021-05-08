# File

> [File input plugin | Logstash Reference](https://www.elastic.co/guide/en/logstash/current/plugins-inputs-file.html)



文件流(stream)事件，通常以类似于`tail -0F` 方式读取，但也可以选择从文件头开始读取。

通常，日志记录会在每条记录后添加换行符。默认情况下，假定每一行就是一个事件。

如果要将多行日志行连接到一个事件中，则需要使用多行编解码器(codec)。该插件在发现新文件和处理每个发现的文件之间循环。发现的文件具有生命周期，它们以“监视”或“已忽略”状态开始。生命周期中的其他状态是：“活动”，“关闭”和“未监视”

默认情况下，使用4095个文件的窗口来限制使用中的文件句柄数。处理阶段包括多个阶段：

- 检查“关闭”或“忽略”的文件自上次以来是否已更改大小，如果已更改，则将其置于“已监视”状态。
- 选择足够的“监视”文件以填充窗口中的可用空间，这些文件将变为“活动”状态。
- 将打开并读取活动文件，默认情况下会从最后一个已知位置到当前内容（EOF）的末尾读取每个文件。

在某些情况下，能够控制首先读取哪些文件，排序以及是否完全读取文件或条带/条带化是很有用的。完整的阅读是**所有的**文件中的文件，然后B，则对文件C等。带区卷或带区卷的读取是文件A，文件B，文件C等的依次读取**，**依次类推，直到再次读取所有文件为止。条带化的阅读是通过改变[`file_chunk_count`](https://www.elastic.co/guide/en/logstash/current/plugins-inputs-file.html#plugins-inputs-file-file_chunk_count "file_chunk_count")和也许指定的 [`file_chunk_size`](https://www.elastic.co/guide/en/logstash/current/plugins-inputs-file.html#plugins-inputs-file-file_chunk_size "file_chunk_size")。如果希望所有文件中的某些事件尽可能早地出现在Kibana中，则分条和排序可能会很有用。

该插件有两种操作模式，尾部模式和读取模式。

#### 尾巴模式

在此模式下，该插件旨在跟踪更改的文件并在将其附加到每个文件时发出新的内容。在这种模式下，文件被视为永无止境的内容流，并且EOF没有特殊意义。该插件始终假定将有更多内容。旋转文件时，将检测到较小或为零的大小，将当前位置重置为零，然后继续流式传输。在累积的字符可以作为行发出之前，必须先看到一个定界符。

#### 读取模式

在这种模式下，插件会将每个文件视为内容完整，即有限的行流，现在EOF变得很重要。不需要最后一个定界符，因为EOF表示可以将累积的字符作为一行发出。此外，这里的EOF意味着可以关闭文件并将其置于“未监视”状态-这会自动释放活动窗口中的空间。此模式还可以处理压缩文件，因为它们内容完整。读取模式还允许在完全处理文件后执行操作。

过去尝试模拟读取模式，同时仍然假设无限流并不理想，因此专用读取模式是一种改进。

### 跟踪监视文件中的当前位置

该插件通过将其记录在一个名为sincedb的单独文件中来跟踪每个文件中的当前位置。这样就可以停止并重新启动Logstash，并使它从中断处开始取走，而不会丢失Logstash停止时添加到文件中的行。

默认情况下，sincedb文件位于Logstash的数据目录中，其文件名基于要监视的文件名模式（即`path`选项）。因此，更改文件名模式将导致使用新的sincedb文件，并且任何现有的当前位置状态都将丢失。如果您以任何频率更改模式，则可以通过该`sincedb_path`选项显式选择一个sincedb路径。

`sincedb_path`每个输入必须使用不同的名称。使用相同的路径会导致问题。每个输入的读取检查点必须存储在不同的路径中，因此信息不会被覆盖。

通过标识符跟踪文件。该标识符由索引节点，主要设备号和次要设备号组成。在Windows中，会从`kernel32`API调用中获取其他标识符。

Nowdb记录现在可以过期，这意味着在一定时间后将不会记住较旧文件的读取位置。文件系统可能需要将索引节点重新用于新内容。理想情况下，我们不会使用旧内容的读取位置，但是我们没有可靠的方法来检测是否发生了索引节点重用。这与“读取”模式更为相关，在“读取”模式下，在sincedb中跟踪了很多文件。但是请记住，如果记录已过期，则将再次读取以前看到的文件。

sincedb文件是具有四列（<v5.0.0），五列或六列的文本文件：

1. 索引节点号（或等效值）。
2. 文件系统的主要设备号（或等效文件）。
3. 文件系统的次设备号（或等效文件）。
4. 文件中的当前字节偏移量。
5. 最后一个活动时间戳（浮点数）
6. 该记录与之匹配的最后一个已知路径（对于将旧的sincedb记录转换为新格式而言，该路径为空白。

在非Windows系统上，您可以使用例如获取文件的索引节点号`ls -li`。

### 从远程网络卷读取

文件输入未在远程文件系统（例如NFS，Samba，s3fs-fuse等）上进行全面测试，但是偶尔会对NFS进行测试。远程FS客户端提供的文件大小用于控制在任何给定时间读取多少数据，以防止读取已分配但尚未填充的内存。由于我们在标识符中使用设备主设备和次设备来跟踪文件的“上次读取”位置，并且在重新安装设备主设备和次设备时，它可能会发生变化，因此在重新安装过程中，sincedb记录可能不匹配。读取模式可能不适用于远程文件系统，因为由于远程到客户端复制过程中的延迟，客户端发现时的文件大小可能与远程发现的文件大小不同。

### 尾部模式下的文件旋转

无论是否通过重命名或复制操作来旋转文件，此输入都会检测到并处理文件旋转。为了支持在旋转发生后一段时间内写入旋转文件的程序，请在文件名模式中同时包含原始文件名和旋转文件名（例如/ var / log / syslog和/var/log/syslog.1）。观看（该`path`选项）。对于重命名，将检测到inode已从 `/var/log/syslog`移到。`/var/log/syslog.1`因此“状态”也在内部移动，因此不会重新读取旧内容，但会读取重命名文件上的任何新内容。用于将复制的内容复制/截断到新文件路径中（如果发现），将被视为新发现，并从头开始读取。因此，复制的文件路径不应位于要监视的文件名模式中（该`path`选项）。将检测到截断，并将“上次读取”位置更新为零。

### 文件输入配置选项

该插件支持以下配置选项以及稍后介绍的“[通用选项”](https://www.elastic.co/guide/en/logstash/current/plugins-inputs-file.html#plugins-inputs-file-common-options "常用选项")。

持续时间设置可以以文本形式指定，例如“ 250 ms”，该字符串将转换为十进制秒。有很多受支持的自然和缩写持续时间，有关详细信息，请参见[string_duration](https://www.elastic.co/guide/en/logstash/current/plugins-inputs-file.html#plugins-inputs-file-string_duration "字符串持续时间")。

