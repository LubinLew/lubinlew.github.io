# Lua字符串

Lua 字符串用于表示文本，其由一个或多个字节(1个字节有8个比特位)组成。Lua 语言中字符串可以存储任意格式的二进制数据，可以使用任意的编码方式(UTF8、UTF16、GB2312等)来存储数据。

<mark>Lua 字符串是不可变的</mark>，不能像 C 语言一样修改字符串中某个字符，只能通过创建一个新的字符串的方式进行修改，而原来的字符串不变。

```lua
a = "one string"
b = string.gsub(a, "one", "another") -- 将字符串中的 one 替换成 another

print(a) --> one string
print(b) --> another string
```

<mark>Lua的字符串下标从1开始</mark>。 下标可以是负数,表示逆序, 例如 -1 表示倒数第一个字符, -2表示倒数第二个字符,以此类推。

## 字符串的基本操作

### 字符串声明

Lua 中的字符串可以使用双引号 或者 单引号来界定, `"string"` 和 `'string'` 是完全相同的的字符串。其唯一的区别是使用双引号声明的字符串中出现单引号时，单引号不需要转义；使用单引号声明的字符串中出现双引号时，双引号不需要转义。实际编码过程中，尽量使用相同的声明方式，保持代码风格。

```lua
a = "double ' quote"
b = 'single " quote'
c = "'xxx'"
d = '"xxx"'

e = "double \" quote"
f = 'single \' quote'
```

#### 转义序列(escape sequence)

字符串中可以包含下面与C语言类似的转义序列:

| 转义序列   | 英文释义                                                            | 中文释义       | 补充说明      |
| ------ | --------------------------------------------------------------- | ---------- | --------- |
| `\a`   | bell                                                            | 响铃         | 设备的蜂鸣器    |
| `\b`   | backspace                                                       | 退格         | 回退一个字符    |
| `\f`   | form feed                                                       | 换页         |           |
| `\n`   | newline                                                         | 换行         |           |
| `\r`   | carriage return                                                 | 回车         |           |
| `\t`   | horizontal tab                                                  | 水平制表符      |           |
| `\v`   | vertical tab                                                    | 垂直制表符      |           |
| `\\`   | backslash                                                       | 反斜杠        |           |
| `\"`   | quotation mark [double quote]                                   | 双引号        |           |
| `\'`   | apostrophe [single quote]                                       | 单引号        |           |
| `\ddd` | A character in a string can be specified by its numerical value | 使用数值表示一个字符 | 最多3个十进制数字 |

这里需要注意的是 `\ddd` 是用ASCII码值声明字符的方式。`\ddd` 表示使用十进制数一个字符。例如`1` 的ASCII码值为 49, 那么可以使用 `\49` 或 `\049` 表示。但是如果后面还有其他数字，则会导致解析错误，例如`\492` 想表示`12`但是会导致Lua解析错误，所以需要使用 `\049` 满3位的方式。

```lua
 print("\49")   ---> 1
 print("\049")  ---> 1
 print("\0492") ---> 12
```

> Lua5.2 开始支持 `\xHH` 十六进制, 例如 `\41` 等价于 0x41 即字母 A
> 
> Lua5.3 开始支持 `\u{h...h}` 的方式来声明UTF8字符,例如 `\u{3b1}` 即希腊字符 α (阿尔法)
> 
> Lua5.2 开始支持 `\z`转义符, 用来跳过其后的所有空白符,这样可以将长字符串分多行声明

#### 长字符串声明

对于长字符串可以使用 `[[ ... ]]` 双方括号的方式来声明。

其中的转义字符不再生效。

多行字符串中的缩进、空格、换行等都会被保留。

如果多行字符串中的第一个字符是换行符时，那么这个换行符会被忽略。

```lua
long_str = [[
<html>
<head>
  <title>Long String</title>
<body>
  <h1>BODY</h1>
</head>
<\html>
]]
```

如果字符串中有`[[`或者`]]` 字符时，会破坏声明，这时候需要使用`[=[ ... ]=]` 这种方式声明，注意其中`=`等号的个数是任意的, 但是前后等号的个数必须是相同的。

`[==[ ... ]==]` 或者 `[===[ ... ]===]` 等都是正确的声明方式。

```lua
long_str = [===[
example1:[[A]]
example2:[=[B]=]
example3:[==[C]==]
]===]
```

---

### 字符串长度

像 Lua 语言中其他对象(表、函数等)一样，Lua 的字符串也是自动内存管理的对象之一。这意味着 Lua 语言会负责字符串的分配和释放，开发人员无需关心。可以使用长度操作符 `#` 获取字符串长度，注意这个长度是字节数，而不是字符数。

> Lua 字符串没有类似C语言`\0` 的字符串结尾符号。

```lua
a = "hello"
print(#a)             ---> 5
print(#"hello world") ---> 11
print(#"你好")         ---> 6
```

### 字符串拼接

Lua 中字符串可以使用 连接操作符 `..` 来进行连接。可以同时拼接多个字符串。如果操作数中存在数值，那么 Lua 语言会先把数值转化为字符串。

Lua 语言在运行时提供了数值与字符串之间的自动转化。针对字符串的所有算术操作会尝试将字符串转换为数值。Lua语言不仅仅在算术操作时进行这种强制类型转换，还会在任何需要数值的情况下进行自动转换，例如函数`math.sin()`的参数。

```lua
a = "hello"
print(a .. " world")  ---> hello world

b = " world"
c = a .. b
print(c)               ---> hello world

-- 拼接多个字符串
d = "1" .. "2" .. "3" .. "4" .. "5"
print(d)      ---> 12345

-- 数字拼接的时候注意， .. 和 数字之间必须使用空格分隔
print(1 .. 2 .. 3 .. 4 .. 5) ---> 12345

-- 数字和字符串拼接
print("1" .. 2) ---> 12
```

----

## 字符串标准库

该库提供了用于字符串操作的通用功能，例如查找和提取子字符串以及模式匹配。在Lua中索引字符串时，第一个字符位于位置1（而不是C中的0）。允许索引为负，并解释为从字符串的末尾开始向后索引。因此，最后一个字符位于位置-1，依此类推。

字符串库`string`实质上时一个`table`, 其中提供了其所有功能。它还为`__index`字段指向表的字符串设置一个`string`元表。因此，您可以使用面向对象样式的字符串函数。例如，`string.byte(s, i)` 可以写成`s:byte(i)`。

| 函数                                          | 能否被JIT编译器编译                                               | 释义          | 备注                                                                                           |
| ------------------------------------------- | --------------------------------------------------------- | ----------- | -------------------------------------------------------------------------------------------- |
| string.len (s)                              | yes                                                       | 字符串长度(字节数)  |                                                                                              |
| string.byte (s [, i [, j]])                 | yes                                                       | 返回字符串中字符的数值 |                                                                                              |
| string.char (···)                           | 2.1                                                       | 数值转字符串      |                                                                                              |
| string.dump (function [,strip])             | never                                                     | 函数转换为字符串    | Lua5.1不支持第二个参数<br>Lua5.3支持第三个参数<br>LuaJIT支持第二个参数                                             |
| string.format (formatstring, ···)           | 2.1 partial<br>Not for %p or non-string arguments for %s. | 格式化打印       |                                                                                              |
| string.find (s, pattern [, init [, plain]]) | 2.1 一部分<br>只有查找固定字符串，而不是模式匹配是才能被编译                        | 字符串查找       | OpenResty中使用 [ngx.re.find()](https://github.com/openresty/lua-nginx-module#ngxrefind) 代替     |
| string.match (s, pattern [, init])          | no                                                        | 字符串查找       | OpenResty中使用 [ngx.re.match()](https://github.com/openresty/lua-nginx-module#ngxrematch) 代替   |
| string.gmatch (s, pattern)                  | no                                                        | 字符串查找       | OpenResty中使用 [ngx.re.gmatch()](https://github.com/openresty/lua-nginx-module#ngxregmatch) 代替 |
| string.sub (s, i [, j])                     | yes                                                       | 字符串查找       | OpenResty中使用 [ngx.re.sub()](https://github.com/openresty/lua-nginx-module#ngxresub) 代替       |
| string.gsub (s, pattern, repl [, n])        | no                                                        | 字符串查找       | OpenResty中使用 [ngx.re.gsub()](https://github.com/openresty/lua-nginx-module#ngxregsub) 代替     |
| string.lower (s)                            | 2.1                                                       | 小写转换        |                                                                                              |
| string.upper (s)                            | 2.1                                                       | 大写转换        |                                                                                              |
| string.reverse (s)                          | 2.1                                                       | 字符串逆序       |                                                                                              |
| string.rep(s, n [,sep])                     | 2.1                                                       | 字符串多副本拼接    | Lua5.1不支持第三个参数<br>Lua5.2支持第三个参数<br>LuaJIT支持第三个参数                                             |

### string.len (s)

返回一个字符串的字节长度。

空字符串 `""` 的长度为0；

内嵌的0也算长度,  `"a\000bc\000"` 的长度为5(`\000` 表示 0x00)。

```lua
print(string.len(""))   ---> 0

print(string.len("\0"))   ---> 1
print(string.len("\00"))  ---> 1
print(string.len("\000")) ---> 1

print(string.len("你好")) ---> 6
```

### string.byte (s [, i [, j]])

返回字符的内部`数字代码`(numerical codes，从结果看基本都是ACSII码的值)。

函数返回的是字符串`s`下标从1开始,  `s[i]`  至`s[j]` 的`数字代码`. 

参数 `i` 的默认值是 1; 参数 `j` 的默认值等于参数 `i`。

注意：函数返回的是 Lua Number 类型， 当返回多个的时候，返回的不是一个 table，而是多个返回值，需要多个变量承载，由于Lua栈的限制默认最大返回参数个数为100万个。

```lua
local v1,v2,v3,v4,v5 = string.byte("hello", 1, 5)
print(v1, v2, v3, v4, v5) -- 104 101 108 108 111
```

注意`数字代码` 不一定可以跨平台。

### string.char (···)

接收零个或多个整数。返回一个长度等于参数个数的字符串，其中每个字符的内部数字代码均等于其相应参数。

```lua
local str = string.char(97,98,0x43,0x44)
print(str) ---> abCD
```

注意`数字代码` 不一定可以跨平台。

### string.dump (function[, strip])

返回一个字符串，该字符串表示参数指定的函数的二进制形式，这个字符串之后可以使用`loadstring()`加载，返回该函数的副本。函数必须是不带升值(upvalues)的Lua函数。

可选参数`strip` 为布尔类型, 表示是否携带debug信息，默认值为`false` 表示携带debug信息, 值为`true` 表示不带有debug信息, 返回的字符串体积更小,之后加载会更快更省内存。

> LuaJIT生成的字符串是可移植的, 任意支持LuaJIT的平台都可以使用，甚至不受其CPU位数和大小端的限制。
> 
> LuaJIT不能加载 标准Lua生成的函数字符串, 二者是互不兼容的
> 
> 不同的LuaJIT版本可能会存在兼容性问题,例如 2.0 与 2.1版本

```lua
-- 定义函数
function test(p)
    print(p)
end

-- 将 test 函数导出成字符串形式,并去除debug信息
fn_str = string.dump(test, true)

-- 加载 字符串, 得到一个函数副本
fn_str_copy = loadstring(fn_str)

-- 使用这个函数副本(同使用原来的 test 函数相同)
fn_str_copy("hello")
```

上述的方式可以用来动态升级程序, 或者对关键函数加密.

### string.find (s, pattern [, init [, plain]])

在字符串`s`中查找`pattern`的第一个匹配项。 如果找到匹配项，则返回匹配的开始和结束的索引; 否则，返回`nil`。 

第三个可选参数`init`为整型，指定从何处开始搜索； 其默认值为1，并且可以为负，注意这里的负值不表示倒序匹配，只是指定位置。 

第四个可选参数`plain`为布尔类型，是模式匹配的开关。默认值为`false`，表示使用模式匹配，值为`true`会关闭模式匹配功能，这时`pattern`中的任何字符都不会被视为“ magic”。

#### 模式匹配 - pattern

lua中 `pattern` 与平常使用PCRE正则表达式有所不同, 语法差异比较大。在OpenResty中请一律使用 `ngx.re.*` 系列API代替Lua原生的模式匹配。

| 字符类      | 释义                                 | 备注                                         |
| -------- | ---------------------------------- | ------------------------------------------ |
| `x`      | （ 其中x不是魔术字符之一 ^$()%.[]*+-?）表示字符x本身 | `^$()%.[]*+-?` 为魔术字符                       |
| `.`      | 表示任意一个字符                           | 与PCRE相同                                    |
| `%a`     | 表示任意一个字母                           |                                            |
| `%A`     | `%a`的补集, 表示任意一个非字母的字符              |                                            |
| `%c`     | 表示任意一个控制字符                         |                                            |
| `%C`     | `%c`的补集, 表示任意一个非控制字符的字符            |                                            |
| `%d`     | 表示任意一个数字                           |                                            |
| `%D`     | `%d`的补集, 表示任意一个非数字的字符              |                                            |
| `%l`     | 表示任意一个小写字母                         |                                            |
| `%L`     | `%l`的补集, 表示任意一个非小写字母的字符            |                                            |
| `%g`     | 表示任意一个除空白字符以外的可打印字符                | LuaJIT 和 Lua5.2才支持该该项                      |
| `%G`     | `%g`的补集,表示空白字符和不能打印字符(控制字符)        | LuaJIT 和 Lua5.2才支持该该项                      |
| `%p`     | 表示任意一个标点符号                         | 标点符合集合未确认, TODO                            |
| `%P`     | `%p`补集, 表示任意一个非标点符号                |                                            |
| `%s`     | 表示任意一个空白字符                         | 空格、`\t`、`\v`、`\r`、`\n`、`\f`` 都算空白字符，详见下面例子 |
| `%S`     | `%s`的补集, 表示任意一个非空白字符               | 注意与`%g`的区别(控制字符)                           |
| `%u`     | 表示任意一个大写字母                         |                                            |
| `%U`     | `%u`的补集, 表示任意一个非大写字母的字符            |                                            |
| `%w`     | 表示任意一个字母数字字符                       |                                            |
| `%W`     | `%w`补集,表示任意一个非字母数字字符               |                                            |
| `%x`     | 表示任意一个十六进制数字                       | 例子结果,令人迷惑 TODO                             |
| `%X`     | `%x`的补集, 表示任意一个非小写字母的字符            | 例子结果,令人迷惑 TODO                             |
| `%z`     | 嵌入的零, 即`\0`                        | 模式不能包含嵌入的零。使用%z代替。                         |
| `[set]`  | 字符集, 匹配任何在括号内的字符                   | 允许使用范围 [0-9]、[a-z]<br>允许使用其他字符类 [%w_]      |
| `[^set]` | 负字符集, 匹配任何不在括号内的字符                 |                                            |

模式匹配中的魔术字符为 `^$()%.[]*+-?`, 如果想要使用其本身代表的字符,需要用`%` 转义

| 模式项              | 释义                                            | 备注      |
| ---------------- | --------------------------------------------- | ------- |
| `*`              | 匹配之前的字符或子表达式零次或多次                             | 与PCRE相同 |
| `+`              | 匹配之前的字符或子表达式一次或多次                             | 与PCRE相同 |
| `-`              | 匹配之前的字符或子表达式零次或多次<br>与`*`不同的是，这些重复项将始终匹配最短的序列 | Lua独有   |
| `?`              | 匹配之前的字符或子表达式零次或一次                             | 与PCRE相同 |
| `%n`,  1<= n <=9 | 此类项目匹配等于捕获的第n个字符串的子字符串                        |         |
| `%bxy`           | xy是不同的字符，表示以x开头y结尾的字符串                        | 未深究     |
| `^`              | 匹配字符串的开始                                      |         |
| `$`              | 匹配字符串的结尾                                      |         |
| `()`             | 子表达式，捕获(captures)                             |         |

```lua
-- test on LuaJIT 2.1.0-beta3(OpenResty)
print(string.find("1abc2abc3", "%a"))    ---> 2 2
print(string.find("1abc2abc3", "%a%a"))  ---> 2 3
print(string.find("1abc2abc3", "%a+"))   ---> 2 4

-- 空白字符
print(string.find("1 2", "%s"))     ---> 2 2
print(string.find("1\t2", "%s"))    ---> 2 2
print(string.find("1\v2", "%s"))    -

--> 2 2
print(string.find("1\r2", "%s"))    ---> 2 2
print(string.find("1\n2", "%s"))    ---> 2 2
print(string.find("1\f2", "%s"))    ---> 2 2
print(string.find("1\a2", "%s"))    ---> nil
print(string.find("1\b2", "%s"))    ---> nil

-- 查找非字母
print(string.find("hello-world", "%A"))  ---> 6 6
-- 查找非控制字符
print(string.find("\a\fb", "%C"))        ---> 3 3
-- 查找非数字的字符
print(string.find("123abc456", "%D"))    ---> 4 4
print(string.find("abcAedf", "%L"))      ---> 4 4
print(string.find(" 1 2", "%S"))         ---> 2    2
print(string.find(" \a 2", "%g"))        ---> 4    4
print(string.find("\a", "%G"))           ---> 1 1
print(string.find("\t", "%G"))           ---> 1 1

-- 迷惑
print(string.find("0x1234", "%x"))  ---> 1 1
print(string.find("0x1234", "%X"))  ---> 2 2

-- * 与 - 的区别
print(string.find("12345678", "%d*")) ---> 1 8
print(string.find("12345678", "%d-")) ---> 1 0 令人不解的结果 0表示什么 ?


```

### string.gsub (s, pattern, repl [, n])

返回字符串`s`的副本，其中所有(或前`n`个,如果制定了参数`n`)命中`pattern`的部分替换成参数`repl`指定的内容。第二个返回值是已发生的匹配总数。

参数`repl`可以是字符串，表或函数。

- 如果`repl`为字符串，则将其值用于替换。字符 `%`可以作为转义字符：`repl`如果有`%n`(1<=n<=9)的形式，代表第N个捕获。`%0`表示整体命中结果

- 如果`repl`是表，则使用第一个捕获作为键，查询每个匹配表。如果模式未指定捕获，则将整个匹配项用作键。

- 如果`repl`是函数，则每次匹配时都会调用此函数，所有捕获的子字符串均按顺序传递为参数；如果模式未指定捕获，则将整个匹配作为唯一参数传递。

如果表查询或函数调用返回的值是字符串或数字，则将其用作替换字符串；否则，它将用作替换字符串。否则，如果它为**false**或**nil**，则不进行替换（即，原始匹配项保留在字符串中）。

```lua
 x = string.gsub("hello world", "(%w+)", "%1 %1")
 --> x="hello hello world world"

 x = string.gsub("hello world", "%w+", "%0 %0", 1)
 --> x="hello hello world"

 x = string.gsub("hello world from Lua", "(%w+)%s*(%w+)", "%2 %1")
 --> x="world hello Lua from"

 x = string.gsub("home = $HOME, user = $USER", "%$(%w+)", os.getenv)
 --> x="home = /home/roberto, user = roberto"

 x = string.gsub("4+5 = $return 4+5$", "%$(.-)%$", function (s)
       return loadstring(s)()
     end)
 --> x="4+5 = 9"

 local t = {name="lua", version="5.1"}
 x = string.gsub("$name-$version.tar.gz", "%$(%w+)", t)
 --> x="lua-5.1.tar.gz"
```

### string.match (s, pattern [, init])

查找字符串中的第一个匹配项 。

如果找到一个，则返回模式中的捕获(可以有多个捕获)；否则返回`nil`。

如果未指定捕获，则返回整个匹配项。

第三个可选数字参数指定从何处开始搜索；其默认值为1，并且可以为负。

```lua
print(string.match("123abc78", "%d%a+"))  ---> 3abc

print(string.match("123abc78", "%d(%a+)")) ---> abc

print(string.match("123abc78", "%d(%d(%a+))")) ---> 3abc	abc
```



### string.gmatch (s, pattern)

返回一个迭代器函数, 每次调用该函数将会返回下一个捕获. 

如果`pattern` 没有声明捕获, 那么会返回整个命中目标.

```lua
--- 迭代器方式
s = "hello world from Lua"
for w in string.gmatch(s, "%a+") do
   print(w)
end
 
--- 函数方式
> mfn = string.gmatch("hello the world", "%a+")
print(mfn())> 
hello
> print(mfn())
the
> print(mfn())
world

--- 返回捕获
for w in string.gmatch("hello the world", "%s(%a+)") do
    print(w)
end
```

~~~~下一个示例`key=value`将给定字符串中的所有对收集到一个表中：

```lua
 t = {}
 s = "from=world, to=Lua"
 for k, v in string.gmatch(s, "(%w+)=(%w+)") do
   t[k] = v
 end
```

对于此功能，不能使用`^`模式，因为这会阻止迭代



### string.format (formatstring, ···)

根据第一个参数（必须是字符串）中给出的描述，返回其可变数量的参数的格式版本。 

格式字符串遵循与标准C函数的printf系列相同的规则。 

唯一的区别是不支持选项/修饰符`*`，`l`，`L`，`n`，`p`和`h`，并且还有一个附加选项`q`。 `q`选项以适合Lua解释器安全读取的格式格式化字符串：该字符串写在双引号之间，并且写入时正确地转义了该字符串中的所有双引号，换行符，嵌入的零和反斜杠。

```lua
 string.format('%q', 'a string with "quotes" and \n new line')
```

会生成下面的字符串:

```lua
> print(string.format('%q', 'a string with "quotes" and \n new line'))

> "a string with \"quotes\" and \
 new line"
```

选项`c`，`d`，`E`，`e`，`f`， `g`，`G`，`i`，`o`，`u`，`X`，和`x`所有期望一个数字作为论据，而`q`与`s`期望的字符串。

该函数不接受包含嵌入式零的字符串值，但作为该`q`选项的参数除外。



### string.rep (s, n [, sep])

返回一个字符串，该字符串是字符串`s`的`n`个副本的串联, 副本之间使用字符串`sep`分隔, `sep`默认值为空字符串。

```lua
print(string.rep("abc", 3))   --> abcabcabc

print(string.rep("123", 4, "-"))  --> 123-123-123-123
```

### string.sub (s, i [, j])

返回`s`的子字符串，从`i`开始，一直到`j`； `i`和`j`可以为负数。 如果不存在`j`，则假定它等于-1（与字符串长度相同）。 

调用`string.sub(s,1,j)`返回长度为`j`的`s`的前缀，而`string.sub(s,-i)`返回长度为`i`的`s`的后缀。

```lua
print(string.sub("abcd", 2))    --> bcd
print(string.sub("abcd", 2, 3)) --> bc
print(string.sub("abcd", 3, 3)) --> c

print(string.sub("abcd", -2))   --> cd
```

### string.reverse (s)

返回参数`s`的逆序字符串

```lua
print(string.reverse("12345678"))   --> 87654321
```

### string.lower (s)

接收一个字符串并返回此字符串的副本，并将所有大写字母更改为小写字母。其他所有字符均保持不变。大写字母的定义取决于当前语言环境。

```lua
print(string.lower("a1-B2=c3,D4_END"))  --> a1-b2=c3,d4_end
```

### string.upper (s)

接收一个字符串并返回此字符串的副本，并将所有小写字母都更改为大写。其他所有字符均保持不变。小写字母的定义取决于当前语言环境。

```lua
print(string.upper("a1-B2=c3,D4_end"))  -->  A1-B2=C3,D4_END
```
