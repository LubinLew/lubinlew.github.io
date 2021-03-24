# 自定义模块 config 文件的写法

config文件其实是一个可执行的 Shell脚本，如果只想卡法一个HTTP模块，那么config文件中需要定义一下3个变量：

`ngx_addon_name` 仅在configigure 执行时使用, 一般设置为模块名称。

`HTTP_MODULES` 保存所有的HTTP模块名称, 每个HTTP模块间由空格符相连,在重新设置`HTTP_MODULES`变量时,不要直接覆盖它,因为configure调用到自定义的config脚本前,已经将各个HTTP模块设置到`HTTP_MODULES`变量中了,因此,需要如下这设置:

"$HTTP_MODULES ngx_http_mytest_modules"

`NGX_ADDON_SRCS` 用于指定新增模块的源代码,多个待编译的源码间以空格符相连.注意在设置`NGX_ADDON_SRCS`时可以使用 `$ngx_addon_dir` 变量,它等价于configure执行时 --add-module=PATH时的PATH参数

因此,对于mytest模块,可以这样编写config文件

```bash
ngx_addon_name=ngx_http_mytest_module
HTTP_MODULES="$HTTP_MODUELS ngx_http_mytest_module"
NGX_ADDON_SRCS="NGX_ADDON_SRCS $ngx_addon/dir/ngx_http_mytest_module.c"
```


