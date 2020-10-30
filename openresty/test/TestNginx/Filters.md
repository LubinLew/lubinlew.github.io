# Filters

我们可以使用过滤器来预处理block的的值。例如我们可以删除掉最后的换行符(如果有的话)，我们可以像下面一样声明一个 `chomp` 过滤器:

```perl
--- response_body chomp
Hello world!
```

如果没有 `chomp` 过滤器, `response_body` 后面可能会有一个换行符。

下面列出了一些常用的过滤器(用户可以自定义过滤器)

## `chomp`

如果最后一个字符是换行符， 那么就删除掉。

## `chop`

不论最后一个字符是什么，都删除掉.

## `eval`

将section的值作为Perl源码片段来对待，将这段代码的执行结果(通常是嘴一个表达式的结果)返回作为这个section的值。这个对于指定无法打印的字符非常有用。

```perl
--- response_body eval
"I don't know what \0 is.\n"
```

# 
