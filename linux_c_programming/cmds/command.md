# command

```shell
command [-pVv] command [arg ...]
```

执行一个简单的命令或显示有关命令的信息。

运行带有ARGS的COMMAND可以防止像shell那样查找或显示有关指定命令的信息。 可用于调用命令系统中同名命令。

| 选项   | 释义                         | 例子                                  |
| ---- | -------------------------- | ----------------------------------- |
| `-p` | 使用`PATH`的默认值，以确保找到所有标准实用程序 | 略                                   |
| `-v` | 打印命令的描述，类似于 `type` 命令的结果   | /usr/bin/checkmodule                |
| `-V` | 打印每个命令的更详细的描述              | checkmodule is /usr/bin/checkmodule |
