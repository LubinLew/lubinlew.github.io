# 字符串处理 - string

Lua 字符串用于表示文本，其由一个或多个字节(1个字节有8个比特位)组成。Lua 语言中字符串可以存储任意格式的二进制数据，可以使用任意的编码方式(UTF8、UTF16、GB2312等)来存储数据。

Lua 字符串是不可变的，不能像 C 语言一样修改字符串中某个字符，只能通过创建一个新的字符串的方式进行修改，而原来的字符串不变。

```lua
a = "one string"
b = string.gsub(a, "one", "another") -- 将字符串中的 one 替换成 another

print(a) --> one string
print(b) --> another string
```

Lua的字符串下标从1开始。 下标可以是负数,表示逆序, 例如 -1 表示倒数第一个字符, -2表示倒数第二个字符,以此类推。

## 字符串的基本操作

### 字符串声明

Lua 字符串的声明可以使用单引号和 双引号。二者的区别是：

- 双引号声明的字符串中间出现单引号时， 单引号不需要转义

- 单引号声明的字符串中间出现双引号时， 双引号不需要转义

```lua
a = "double ' quote"
b = 'single " quote'
c = "'xxx'"
d = '"xxx"'

e = "double \" quote"
f = 'single \' quote'
```

#### 声明长字符串



### 转义字符

Lua中支持很多 C 风格的转义字符:

| 转义字符 | 中文释义  | 英文释义                     |
| ---- | ----- | ------------------------ |
| \a   | 响铃    | bell                     |
| \b   | 退格    | back space               |
| \f   | 换页    | from feed                |
| \r   | 回车    | carriage return          |
| \n   | 换行    | newline                  |
| \t   | 水平制表符 | horizontal tab           |
| \v   | 垂直制表符 | vertical tab             |
| \\\  | 反斜杠   | backslash                |
| \"   | 双引号   | double quote             |
| \'   | 单引号   | single quote             |
| \ddd | 十进制   | decimal escape character |
| \xhh | 十六进制  | hex escape character     |

这里需要注意的是 `\ddd`  和 `\xhh` 这两个用ASCII码值声明字符的方式。`\ddd` 表示使用十进制数一个字符。例如`1` 的ASCII码值为 49, 那么可以使用  `\49` 或 `\049` 表示。但是如果后面还有其他数字，则会导致解析错误，例如`\492` 想表示`12`但是会导致Lua解析错误，所以最好使用 `\049` 满3位的方式。

```lua
 print("\49")   ---> 1
 print("\049")  ---> 1
 print("\0492") ---> 12
```

`\xhh`是使用十六进制的方式声明一个字符。

```lua
print("\x31")  ---> 1
```

另外 Lua 5.3 还支持 使用 `\u{hhhh}`的方式声明UTF8字符，但是目前LuaJIT不支持这个特性。

### 字符串长度

像 Lua 语言中其他对象(表、函数等)一样，Lua 的字符串也是自动内存管理的对象之一。这意味着 Lua 语言会负责字符串的分配和释放，开发人员无需关心。可以使用长度操作符 `#` 获取字符串长度，注意这个长度是字节数，而不是字符数。

```lua
a = "hello"
print(#a)             ---> 5
print(#"hello world") ---> 11
```

### 字符串拼接

Lua 中字符串可以使用 连接操作符 `..` 来进行连接。可以同时拼接多个字符串。如果操作数中存在数值，那么 Lua 语言会先把数值转化为字符串。

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

The string library provides all its functions inside the table `string`. It also sets a metatable for strings where the `__index` field points to the `string` table. Therefore, you can use the string functions in object-oriented style. For instance, `string.byte(s, i)` can be written as `s:byte(i)`.

The string library assumes one-byte character encodings.



## 字符串标准库

### string.byte (s [, i [, j]])

返回字符的内部`数字代码`(numerical codes，从结果看基本都是ACSII码的值)。

函数返回的是字符串`s`下标从1开始,  `s[i]`  至`s[j]` 的`数字代码`. 

参数 `i` 的默认值是 1; 参数 `j` 的默认值等于参数 `i`。

注意：函数返回的是 Lua Number 类型， 当返回多个的时候，返回的不是一个 table，而是多个返回值，需要多个变量承载：

```lua
local v1,v2,v3,v4,v5 = string.byte("hello", 1, 5)
print(v1, v2, v3, v4, v5) -- 104 101 108 108 111
```

注意`数字代码` 不一定可以跨平台。

### string.char (···)

Receives zero or more integers. Returns a string with length equal to the number of arguments, in which each character has the internal numerical code equal to its corresponding argument.

Note that numerical codes are not necessarily portable across platforms.

### `string.dump (function)`

Returns a string containing a binary representation of the given function, so that a later [`loadstring`](http://www.lua.org/manual/5.1/manual.html#pdf-loadstring) on this string returns a copy of the function. `function`must be a Lua function without upvalues.

### `string.find (s, pattern [, init [, plain]])`

Looks for the first match of `pattern` in the string `s`. If it finds a match, then `find` returns the indices of `s` where this occurrence starts and ends; otherwise, it returns **nil**. A third, optional numerical argument `init` specifies where to start the search; its default value is 1 and can be negative. A value of **true** as a fourth, optional argument `plain` turns off the pattern matching facilities, so the function does a plain "find substring" operation, with no characters in `pattern`being considered "magic". Note that if `plain` is given, then `init` must be given as well.

If the pattern has captures, then in a successful match the captured values are also returned, after the two indices.

### `string.format (formatstring, ···)`

Returns a formatted version of its variable number of arguments following the description given in its first argument (which must be a string). The format string follows the same rules as the `printf` family of standard C functions. The only differences are that the options/modifiers `*`, `l`, `L`, `n`, `p`, and `h` are not supported and that there is an extra option, `q`. The `q` option formats a string in a form suitable to be safely read back by the Lua interpreter: the string is written between double quotes, and all double quotes, newlines, embedded zeros, and backslashes in the string are correctly escaped when written. For instance, the call

     string.format('%q', 'a string with "quotes" and \n new line')

will produce the string:

     "a string with \"quotes\" and \
      new line"

The options `c`, `d`, `E`, `e`, `f`, `g`, `G`, `i`, `o`, `u`, `X`, and `x` all expect a number as argument, whereas `q` and `s` expect a string.

This function does not accept string values containing embedded zeros, except as arguments to the `q` option.

### `string.gmatch (s, pattern)`

Returns an iterator function that, each time it is called, returns the next captures from `pattern` over string `s`. If `pattern` specifies no captures, then the whole match is produced in each call.

As an example, the following loop

     s = "hello world from Lua"
     for w in string.gmatch(s, "%a+") do
       print(w)
     end

will iterate over all the words from string `s`, printing one per line. The next example collects all pairs `key=value` from the given string into a table:

     t = {}
     s = "from=world, to=Lua"
     for k, v in string.gmatch(s, "(%w+)=(%w+)") do
       t[k] = v
     end

For this function, a '`^`' at the start of a pattern does not work as an anchor, as this would prevent the iteration.

### `string.gsub (s, pattern, repl [, n])`

Returns a copy of `s` in which all (or the first `n`, if given) occurrences of the `pattern` have been replaced by a replacement string specified by `repl`, which can be a string, a table, or a function. `gsub` also returns, as its second value, the total number of matches that occurred.

If `repl` is a string, then its value is used for replacement. The character `%` works as an escape character: any sequence in `repl` of the form `%*n*`, with *n*between 1 and 9, stands for the value of the *n*-th captured substring (see below). The sequence `%0` stands for the whole match. The sequence `%%` stands for a single `%`.

If `repl` is a table, then the table is queried for every match, using the first capture as the key; if the pattern specifies no captures, then the whole match is used as the key.

If `repl` is a function, then this function is called every time a match occurs, with all captured substrings passed as arguments, in order; if the pattern specifies no captures, then the whole match is passed as a sole argument.

If the value returned by the table query or by the function call is a string or a number, then it is used as the replacement string; otherwise, if it is **false** or **nil**, then there is no replacement (that is, the original match is kept in the string).

Here are some examples:

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

### `string.len (s)`

Receives a string and returns its length. The empty string `""` has length 0. Embedded zeros are counted, so `"a\000bc\000"` has length 5.

### `string.lower (s)`

Receives a string and returns a copy of this string with all uppercase letters changed to lowercase. All other characters are left unchanged. The definition of what an uppercase letter is depends on the current locale.

### `string.match (s, pattern [, init])`

Looks for the first *match* of `pattern` in the string `s`. If it finds one, then `match` returns the captures from the pattern; otherwise it returns **nil**. If `pattern`specifies no captures, then the whole match is returned. A third, optional numerical argument `init` specifies where to start the search; its default value is 1 and can be negative.

### `string.rep (s, n)`

Returns a string that is the concatenation of `n` copies of the string `s`.

### `string.reverse (s)`

Returns a string that is the string `s` reversed.

### `string.sub (s, i [, j])`

Returns the substring of `s` that starts at `i` and continues until `j`; `i` and `j` can be negative. If `j` is absent, then it is assumed to be equal to -1 (which is the same as the string length). In particular, the call `string.sub(s,1,j)` returns a prefix of `s` with length `j`, and `string.sub(s, -i)` returns a suffix of `s` with length `i`.

### `string.upper (s)`

Receives a string and returns a copy of this string with all lowercase letters changed to uppercase. All other characters are left unchanged. The definition of what a lowercase letter is depends on the current locale.
