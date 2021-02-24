# LuaJIT 简介

LuaJIT已成功用作游戏、设备、网络、图形应用、数值模拟、交易平台 和 许多其他特殊应用程序中的脚本中间件。它可以从嵌入式设备，智能手机，台式机扩展到服务器农场(server farm)。

<mark>它拥有了高灵活性、高性能 和 无与伦比的低内存占用</mark>。自2005年以来，LuaJIT一直在不断发展。它被广泛认为是最快的动态语言实现之一。自首个版本发布以来，它在许多跨语言基准测试中的表现均优于其他动态语言。<mark>对于LuaJIT 2.0，整个虚拟机已被完全重写，并为性能进行了不懈的优化。它结合了用汇编器编写的高速解释器和最新的JIT编译器。创新的跟踪编译器与基于SSA的高级优化和经过高度调整的代码生成后集成在一起</mark>。

LuaJIT完全兼容标准 Lua 5.1。它支持所有 [标准Lua库函数](https://www.lua.org/manual/5.1/manual.html#5)和全套 [Lua / C API函数](https://www.lua.org/manual/5.1/manual.html#3)。LuaJIT在链接器/动态加载程序级别上也与Lua 5.1完全ABI兼容。这意味着您可以针对标准Lua头文件编译C模块，并从Lua或LuaJIT加载相同的共享库。LuaJIT通过新功能扩展了标准Lua VM，并添加了扩展模块。

---

## 扩展模块

LuaJIT 新增了3个内置模块 `bit` 、 `ffi`  和  `jit`。

### bit.* — 位操作模块

LuaJIT支持[Lua BitOp](https://bitop.luajit.org/)定义的所有按位操作。

### ffi.* — FFI 库

强大的 [FFI 库](https://luajit.org/ext_ffi.html) 允许你调用外部的 C 函数(动态库中的) 和 使用Lua代码操作 C 数据结构。

### jit.* — JIT 编译器控件

The functions in this module [control the behavior of the JIT compiler engine](https://luajit.org/ext_jit.html).

----

## C API 扩展

LuaJIT扩展了标准 [Lua / C API](https://luajit.org/ext_c_api.html) 添加了一些 [额外的功能](https://luajit.org/ext_c_api.html)

---

## 标准库增强

### xpcall(f, err [,args...]) 可以传递参数

Unlike the standard implementation in Lua 5.1, xpcall() passes any arguments after the error function to the function which is called in a protected context.

### loadfile() 等可以处理 UTF-8 源码

非ASCII字符由Lua源代码解析器透明地处理。这允许在标识符和字符串中使用UTF-8字符。在源代码的开头会跳过UTF-8 BOM。

### tostring() 等规范化NaN和±Inf

所有数字到字符串的转换始终在所有平台上将非有限数字转换为相同的字符串。NaN导致“ nan”，正无穷大导致“ inf”，负无穷大导致“ -inf”。

### tonumber() ### 等使用内置字符串进行数字转换

在所有平台上，所有字符串到数字的转换都始终以十进制和十六进制转换整数和浮点输入。 关于strtod（）是*不*再使用，这避免了具有差的C库实现许多问题。内置的转换功能根据IEEE-754标准提供了完全的精度，它独立于当前语言环境工作，并且支持十六进制浮点数（例如0x1.5p-3）。

### string.dump(f [,strip]) 生成可移植的字节码

一个额外的参数已添加到string.dump（）。如果设置为 true，则生成没有调试信息的“剥离”字节码。这样可以加快以后字节码的加载速度并减少内存使用量。另请参见 [-b命令行选项](https://luajit.org/running.html#opt_b)。

生成的字节码是可移植的，并且可以加载到LuaJIT支持的任何体系结构上，而与字长或字节序无关。但是，字节码兼容性版本必须匹配。字节码与点发行版（xy0→xy1）保持兼容，但可能随着主要或次要发行版（2.0→2.1）或任何beta版本之间的变化而变化。外部字节码（例如来自Lua 5.1的字节码）不兼容，无法加载。

### 针对math.random（）的增强型PRNG

LuaJIT使用周期为2 ^ 223的Tausworthe PRNG来实现 math.random（）和math.randomseed（）。与使用特定于平台的ANSI rand（）的标准Lua实现相比，PRNG结果的质量要好得多。

PRNG在所有平台上从相同的种子生成相同的序列，并利用种子参数中的所有位。 不带参数的math.random（）为每个调用生成52个伪随机位。结果均匀地分布在0.0和1.0之间。正确放大并四舍五入以保留math.random（n [，m]）的均匀性。

重要说明：此代码或任何其他基于简单math.random（） API的PRNG都不 适合加密用途。

### io.*函数处理64位文件偏移

标准io。*库中的文件I / O函数处理64位文件偏移。特别是，这意味着有可能打开大于2 GB的文件，并重新定位或获取偏移量超过2 GB的当前文件位置（fp：seek（）方法）。

### debug.*函数识别元方法

debug.getinfo（）和lua_getinfo（）也返回有关调用的元方法的信息。所述namewhat字段被设定为 “元方法”和名称字段具有对应元方法的名称（例如“__index”）。

### 完全可恢复的虚拟机

LuaJIT VM是完全可恢复的。这意味着您甚至可以在标准环境Lua 5.1 VM无法实现的情况下，从协程中获得收益：例如，您可以在pcall（） 和xpcall（），迭代器和元方法之间产生收益。

## 增加了 Lua 5.2 的一些特性

Lua 的版本迭代较快，而且不同版本间可能会有语法冲突，LuaJIT 支持了一些不会与Lua 5.1 冲突的 Lua 5.2 特性:

- goto and ::labels::.
- Hex escapes '\x3F' and '\*' escape in strings.
- load(string|reader [, chunkname [,mode [,env]]]).
- loadstring() is an alias for load().
- loadfile(filename [,mode [,env]]).
- math.log(x [,base]).
- string.rep(s, n [,sep]).
- string.format(): %q reversible. %s checks __tostring. %a and "%A added.
- String matching pattern %g added.
- io.read("*L").
- io.lines() and file:lines() process io.read() options.
- os.exit(status|true|false [,close]).
- package.searchpath(name, path [, sep [, rep]]).
- package.loadlib(name, "*").
- debug.getinfo() returns nparams and isvararg for option "u".
- debug.getlocal() accepts function instead of level.
- debug.getlocal() and debug.setlocal() accept negative indexes for varargs.
- debug.getupvalue() and debug.setupvalue() handle C functions.
- debug.upvalueid() and debug.upvaluejoin().
- Command line option -E.
- Command line checks __tostring for errors.

Other features are only enabled, if LuaJIT is built with -DLUAJIT_ENABLE_LUA52COMPAT:

- goto is a keyword and not a valid variable name anymore.
- break can be placed anywhere. Empty statements (;;) are allowed.
- __lt, __le are invoked for mixed types.
- __len for tables. rawlen() library function.
- pairs() and ipairs() check for __pairs and __ipairs.
- coroutine.running() returns two results.
- table.pack() and table.unpack() (same as unpack()).
- io.write() and file:write() return file handle instead of true.
- os.execute() and pipe:close() return detailed exit status.
- debug.setmetatable() returns object.
- debug.getuservalue() and debug.setuservalue().
- Remove math.mod(), string.gfind().

Note: this provides only partial compatibility with Lua 5.2 at the language and Lua library level. LuaJIT is API+ABI-compatible with Lua 5.1, which prevents implementing features that would otherwise break the Lua/C API and ABI (e.g. _ENV).

## C++ Exception Interoperability

LuaJIT has built-in support for interoperating with C++ exceptions. The available range of features depends on the target platform and the toolchain used to compile LuaJIT:

| Platform                          | Compiler        | Interoperability |
| --------------------------------- | --------------- | ---------------- |
| POSIX/x64, DWARF2 unwinding       | GCC 4.3+        | **Full**         |
| Other platforms, DWARF2 unwinding | GCC             | **Limited**      |
| Windows/x64                       | MSVC            | **Full**         |
| Windows/x86                       | Any             | **No**           |
| Other platforms                   | Other compilers | **No**           |

**Full interoperability** means:

- C++ exceptions can be caught on the Lua side with pcall(), lua_pcall() etc.
- C++ exceptions will be converted to the generic Lua error "C++ exception", unless you use the [C call wrapper](https://luajit.org/ext_c_api.html#mode_wrapcfunc) feature.
- It's safe to throw C++ exceptions across non-protected Lua frames on the C stack. The contents of the C++ exception object pass through unmodified.
- Lua errors can be caught on the C++ side with catch(...). The corresponding Lua error message can be retrieved from the Lua stack.
- Throwing Lua errors across C++ frames is safe. C++ destructors will be called.

**Limited interoperability** means:

- C++ exceptions can be caught on the Lua side with pcall(), lua_pcall() etc.
- C++ exceptions will be converted to the generic Lua error "C++ exception", unless you use the [C call wrapper](https://luajit.org/ext_c_api.html#mode_wrapcfunc) feature.
- C++ exceptions will be caught by non-protected Lua frames and are rethrown as a generic Lua error. The C++ exception object will be destroyed.
- Lua errors **cannot** be caught on the C++ side.
- Throwing Lua errors across C++ frames will **not** call C++ destructors.

**No interoperability** means:

- It's **not** safe to throw C++ exceptions across Lua frames.
- C++ exceptions **cannot** be caught on the Lua side.
- Lua errors **cannot** be caught on the C++ side.
- Throwing Lua errors across C++ frames will **not** call C++ destructors.
- Additionally, on Windows/x86 with SEH-based C++ exceptions: it's **not** safe to throw a Lua error across any frames containing a C++ function with any try/catch construct or using variables with (implicit) destructors. This also applies to any functions which may be inlined in such a function. It doesn't matter whether lua_error() is called inside or outside of a try/catch or whether any object actually needs to be destroyed: the SEH chain is corrupted and this will eventually lead to the termination of the process.
