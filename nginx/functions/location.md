# location 选项的配置

```nginx
Syntax:  location [ = | ~ | ~* | ^~ ] uri { ... }
         location @name { ... }
Default:  —
Context:  server, location
```

`location` 的功能是根据请求 URI 来配置访问资源。


匹配开始之前, nginx 会对 URL 进行规范化(normalized), 即对 `%XY` 形式的[URL编码](nginx/functions/encode.md)进行解码。

在解码以`%XY`形式编码的文本、解析对相对路径组件`.`的引用后，对规范化(normalized )的 URI 执行匹配。 和`..`，以及可能将两个或多个相邻的斜线压缩成一个斜线。

## 匹配规则

`location` 可以由`前缀(prefix )字符串`或`正则表达式`定义。

正则表达式使用 `~*` 修饰符(用于不区分大小写的匹配)或`~`修饰符(用于区分大小写的匹配)指定。

为了找到与给定请求匹配的 `location`，nginx 首先检查使用前缀字符串定义的位置。
其中，匹配前缀最长的位置被选中并记忆。

然后按照它们在配置文件中出现的顺序检查正则表达式。
正则表达式的搜索在第一次匹配时终止，并使用相应的配置。
如果找不到与正则表达式的匹配项，则使用前面记住的前缀位置的配置。

位置块可以嵌套，下面会提到一些例外情况。

- 对于 macOS 和 Cygwin 等不区分大小写的操作系统，匹配前缀字符串会忽略大小写。 但是，比较仅限于一字节语言环境。
- 正则表达式可以包含稍后可以在其他指令中使用的捕获。
- 如果最长匹配前缀位置具有 `^~` 修饰符，则不检查正则表达式。
- 此外，使用`=`修饰符可以定义 URI 和位置的精确匹配。如果找到完全匹配，则搜索终止。

例如，如果`/`请求频繁发生，定义`location = /`将加速这些请求的处理，因为搜索在第一次比较后立即终止。这样的位置显然不能包含嵌套位置。

### location 配置例子

让我们通过一个例子来说明上面的内容：

```nginx
location = / {
    [ configuration A ]
}

location / {
    [ configuration B ]
}

location /documents/ {
    [ configuration C ]
}

location ^~ /images/ {
    [ configuration D ]
}

location ~* \.(gif|jpg|jpeg)$ {
    [ configuration E ]
}
```

`/`请求将匹配配置 A，
`/index.html`请求将匹配配置 B，
`/documents/document.html`请求将匹配配置 C，
`/images/1.gif`请求将匹配 配置 D，
`/documents/1.jpg`请求将匹配配置 E。

`@`前缀定义了一个命名位置。
这样的位置不用于常规请求处理，而是用于请求重定向。 它们不能嵌套，也不能包含嵌套位置。

### 代理时 location 配置例子

如果位置由以斜杠字符结尾的前缀字符串定义，并且请求由 `proxy_pass` 等代理处理，则执行特殊处理。

为了响应 URI 等于此字符串但没有尾部斜杠的请求，将返回带有代码 301 的永久重定向到附加斜杠的请求 URI。
如果不希望这样，可以像这样定义 URI 和位置的精确匹配：

```nginx
location /user/ {
    proxy_pass http://user.example.com;
}

location = /user {
    proxy_pass http://login.example.com;
}
```
