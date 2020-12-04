# LuaJIT 的 FFI 库

> 翻译自 [FFI Tutorial](https://luajit.org/ext_ffi_tutorial.html)

## 简介

FFI 库允许在纯 Lua 代码中调用外部的 `C 函数` 和 使用 `C 的数据结构`。标准的 Lua 调用 C 函数需要编写额外的绑定函数(绑定函数的编写很复杂)，使用 FFI 就可以省略这个复杂的步骤，因为 FFI 可以解析 C 语言的声明。FFI 库与 LuaJIT 是一体的，不能分离成一个单独的模块。JIT 编译器对访问 C 数据结构生成的代码与 C 编译器生成的代码性能相差无几，性能远高于使用绑定函数的方式。

## 使用说明

下面是对FFI库使用的一些说明

### 加载FFI库

FFI库是内建的LuaJIT中的， 但是默认不会加载和初始化。所以使用之前需要加载它

```lua
local ffi = require("ffi")
```

需要注意的是，这行代码并不是在全局 table 中定义了一个名为`ffi`的变量。你需要local化使用`ffi`变量，`require`函数的作用只是防止重复加载。如果你想要在交互式命令行中尝试使用FFI，那么你需要省略 local 限定，因为在命令行中不会跨行保存局部变量。

### 访问标准系统函数

下面的代码介绍了如何访问标准系统函数。

```lua
local ffi = require("ffi")

--[[ ffi.cdef 区间内指定了我们要使用的C库函数声明(使用标准的C语法), 
 这些声明可以从C头文件或者说明文档中拷贝而来
 Sleep()声明Windows系统上独有的, 但是在Linux下只要不调用它, 
 那么这个不存在的声明是毫无影响的.
--]]
ffi.cdef[[
void Sleep(int ms);
int poll(struct pollfd *fds, unsigned long nfds, int timeout);
]]

local sleep

--[[ 这里需要面对的难题是需要选择不同的标准, 
 例如 Windows系统有一个简单的Sleep()函数,
 在其他系统中有很多函数可以实现毫秒级别的sleep功能,但是都不统一.
 幸运的是 poll()函数在绝大部分非windows系统上都有提供.
 我们需要根据当前的系统, 选择调用不用的接口函数.
--]]
if ffi.os == "Windows" then
    function sleep(s) --定义在Windows系统下使用的 sleep()函数
        ffi.C.Sleep(s*1000)
    end
else
    function sleep(s) --定义在非Windows系统下使用的 sleep()函数
        ffi.C.poll(nil, 0, s*1000)
    end
end

for i=1,160 do
    io.write(".")
    io.flush()
    sleep(0.01)
end

io.write("\n")
```

一些读者可以会注意到`Sleep()`函数是在`KERNEL32.DLL`中, 同时它也一个Windows标准调用函数。 FFI库提供了 `ffi.C` 这个默认的C库命名空间(namespace), 这就允许了FFI 可以像C编译器一样,从默认的库中调用标准函数。另外<u>FFI库会自动检测标准调用，所有你甚至可以不用声明他们</u>。

函数`poll()` 需要多个参数，你可以用 nil 传递 NULL。需要注意的是：数<u>字 0 不会像C/C++一样被转换成指针，你必须传递指针给指针参数， 传递数值给数值参数</u>。

### 访问zlib压缩库

下面的代码展示了如何使用Lua代码调用zlib库的函数。

```lua
local ffi = require("ffi")

--声明zlib中的压缩和解压函数
ffi.cdef[[
unsigned long compressBound(unsigned long sourceLen);

int compress2(uint8_t *dest, unsigned long *destLen,
          const uint8_t *source, unsigned long sourceLen, int level);

int uncompress(uint8_t *dest, unsigned long *destLen,
          const uint8_t *source, unsigned long sourceLen);
]]

--[[ 加载zlib的动态库,在Posix标准的系统中,zlib库都是默认安装的.
 因为 ffi.load() 会自动加载动态库的前缀和后缀, 所有我们可以只写一个z.
 在Windows系统中库名为zlib1.dll(这个需要你自己去zlib网站下载),
 我们检查ffi.os, 根据操作系统的不同,我们加载不用的库名
--]]
local zlib = ffi.load(ffi.os == "Windows" and "zlib1" or "z")

--[[ 压缩函数
  注意可变长数组的使用 buf 
  注意传递指针的方法 buflen 
  ffi.new() 申请的内存由LuaJIT的GC负责释放, 用户无需关心
--]]
local function compress(txt)
    local n = zlib.compressBound(#txt)
    local buf = ffi.new("uint8_t[?]", n)
    local buflen = ffi.new("unsigned long[1]", n)
    local res = zlib.compress2(buf, buflen, txt, #txt, 9)
    assert(res == 0)
    return ffi.string(buf, buflen[0])
end

--解压函数
local function uncompress(comp, n)
    local buf = ffi.new("uint8_t[?]", n)
    local buflen = ffi.new("unsigned long[1]", n)
    local res = zlib.uncompress(buf, buflen, comp, #comp)
    assert(res == 0)
    return ffi.string(buf, buflen[0])
end

-- 测试代码
local txt = string.rep("abcd", 1000)
print("Uncompressed size: ", #txt)
local c = compress(txt)
print("Compressed size: ", #c)
local txt2 = uncompress(c, #txt)
assert(txt2 == txt)
```

### 为C类型定义元方法

下面的代码展示了如何为一个C类型定义元方法。我们定义一个简单的 point 类型(坐标)和一些其相关的操作函数。

```lua
local ffi = require("ffi")

-- 声明 point_t(坐标) 结构体
ffi.cdef[[
   typedef struct { 
       double x;
       double y; 
   } point_t;
]]

-- We have to declare the variable holding the point constructor first, because it's used inside of a metamethod.
-- 必需先定义point变量, 因为下面的元方法中会使用它
local point

-- metatable 定义
local mt = {
  --[[元方法 + 
    将2个坐标相加, 返回一个新的坐标对象
    这里假定 相加的2个对象都是 point_t 类型的, 但是其实可以是任何类型,
    当然需要保证其中一个是 point_t 类型， 例如 point_t 类型加一个数字， 或者 数字加point_t类型。
  --]]
  __add = function(a, b) return point(a.x+b.x, a.y+b.y) end,

  --[[ 元方法 #
    计算该点到原点(0,0)的距离(原理:勾股定理,长方形求对角线长度)
  --]]
  __len = function(a)    return math.sqrt(a.x*a.x + a.y*a.y) end,

  --[[ 自定义名称的元方法 area
    上面的元方法 和C++运算符重载相似,
    当然我们可以定义 自己命名的元方法,
    注意 __index 与 __newindex 的区别
  --]]
  __index = {
    area = function(a)   return a.x*a.x + a.y*a.y end,
  },
}

--[[将元表 与 C类型 绑定, 这个只需要做一次
  注意: 这个绑定是永久的(permanent), 之后不允许修改 这个原表 和 __index 表
For convenience, a constructor is returned by ffi.metatype().
We're not required to use it, though. The original C type can still
be used e.g. to create an array of points. The metamethods automatically
apply to any and all uses of this type.
--]]
point = ffi.metatype("point_t", mt)


--测试
local a = point(3, 4)
print(a.x, a.y)  --> 3  4

-- 元方法: __len
print(#a)        --> 5

-- 元方法: area
-- 注意调用的方式是a:area() 而不是 a.area()
print(a:area())  --> 25

-- 元方法: __add
local b = a + point(0.5, 8)
print(#b)        --> 12.5
```

当C库以面向对象的方式编写，那么C类型元方法的机制会变得非常有用。所谓面向对象的方式就是：Creators 返回一个实例的指针，所以实例方法函数的第一个参数都是这个实例指针。有时你仅需要将  `__index` 指向库的命令空间(namespace) 并且 `__gc` 指向析构函数就足够了。 但是通常你会想要封装成更便捷的接口, 例如需要返回Lua 类型字符串 或者 返回多个值的时候。

一些C库仅声明了实例指针是 `void *` 类型（或者是调用者不需要通过实例指针访问内部成员)。这种情况下你可以使用一个假的类型声明。例如下面这样：

```c
// 不完整结构的指针
typedef struct foo_type *foo_handle;
```

C 库并不知道你在FFI中做了什么样的声明, 只要你传递的实例指针是对的，那么完全没有任何问题。其实，熟悉C的话就知道，这是C语言中一个惯用的技巧。例如你想给其他人提供一套接口函数实现某些功能， 但是具体实现是保密的，不想对外泄露。创建功能返回的实例是一个结构体，但是你不想让调用者知道这个结构体的具体实现。你就可以使用上面这种方式，给调用者提供的接口函数声明如下：

```c
/*  ---给调用者提供的API接口声明---- */

//这样声明就隐藏了结构体的具体定义,即使gdb调试也看不到内部结构
typedef struct instance_s instance_t;

//创建实例
instance_t* create_instance(void);
//销毁实例
void destory_instance(instance_t* ins);
//实例功能
int some_method(instance_t* ins);
```

`struct instance_s` 的定义放到自己内部的头文件中即可。

### 

### 翻译C语法

这是常用的Here's a list of common C idioms and their translation to the LuaJIT FFI:

| 类型                                                              | 声明                          | C 代码                                       | Lua 代码                                                      | 备注         |
| --------------------------------------------------------------- | --------------------------- | ------------------------------------------ | ----------------------------------------------------------- | ---------- |
| 指针引用                                                            | int *p;                     | x = *p;<br>*p = y;                         | x = **p[0]**<br>**p[0]** = y                                | *p => p[0] |
| 指针索引                                                            | int i, *p;                  | x = p[i];<br>p[i+1] = y;                   | x = p[i]<br>p[i+1] = y                                      |            |
| 数组索引                                                            | int i, a[];                 | x = a[i];<br>a[i+1] = y;                   | x = a[i]<br>a[i+1] = y                                      |            |
| 访问`结构体/联合`体成员1                                                  | struct foo s;               | x = s.field;<br>s.field = y;               | x = s.field<br>s.field = y                                  |            |
| 访问`结构体/联合`体成员2                                                  | struct foo *sp;             | x = sp->field;<br>sp->field = y;           | x = **s.field**<br>**s.field** = y                          |            |
| 指针运算1                                                           | int i, *p;                  | x = p + i;<br>y = p - i;                   | x = p + i<br>y = p - i                                      |            |
| 指针运算2                                                           | int *p1, *p2;               | x = p1 - p2;                               | x = p1 - p2                                                 |            |
| 获取数组元素地址                                                        | int i, a[];                 | x = &a[i];                                 | x = **a+i**                                                 |            |
| 指针类型转换                                                          | int *p;                     | x = (intptr_t)p;                           | x = tonumber( ffi.cast("intptr_t", p) )                     |            |
| 有出参数的函数                                                         | void foo(int *inoutlen);    | int len = x;<br>foo(&len);<br>y = len;     | local len =  ffi.new("int[1]", x)<br>foo(len)<br>y = len[0] |            |
| [可变参数](http://luajit.org/ext_ffi_semantics.html#convert_vararg) | int printf(char *fmt, ...); | printf("%g", 1.0);<br>printf("%d", 1);<br> | printf("%g", 1);<br>printf("%d",  ffi.new("int", 1))        |            |

### 是否需要缓存库函数

缓存库函数到局部变量或者upvalues中, 是Lua的惯例, 看下面的例子:

```lua
local byte, char = string.byte, string.char
local function foo(x)
    return char(byte(x)+1)
end
```

这样会节省多次的hash表查找。但是这对LuaJIT来说其实并不是太重要，因为JIT编译器对hash表查找做了大量的优化，甚至能够将大部分优化拓展(hoist)到内部循环之外。那为什还要使用上面这种缓存式的写法呢？这是因为JIT编译器并不能够消除所有的hash表查找。

**但是通过FFI库调用C函数时，情况就有些不同了**。JIT编译器有特殊的逻辑来消除所有的C库命名空间中函数的查找，所以缓存C命名空间中的函数，是反向优化：

```lua
local funca, funcb = ffi.C.funca, ffi.C.funcb -- 错误的方式
local function foo(x, n)
    for i=1,n do funcb(funca(x, i), 1) end
end
```

这样会使他们变成间接调用，生成更大更慢的机器码。你应该只缓存C命令空间，依赖JIT编译器来消除hash表查找 ，像下面的例子一样：

```lua
local C = ffi.C          -- 正确的方式
local function foo(x, n)
    for i=1,n do C.funcb(C.funca(x, i), 1) end
end
```

这样就可以生成更短更快的机器码。所以<u>不要缓存C函数，只缓存C命名空间</u>。 通常来说， 命名空间已经缓存在一个在外部范围(an outer scope)的局部变量中, 像下面这样：

```lua
local lib = ffi.load(...)
```

这时把它拷贝到一个函数作用域中的局部变量，是没有必要的。

## FFI库的API介绍

### 词汇表

- **cdecl** — 抽象的C类型声明 (a Lua string).
- **ctype** — C 类型对象. 这是一种特殊的  **cdata** ，returned by ffi.typeof(). 它相当于 **cdata** 的构造函数。
- **cdata** — C 数据对象. 他持有相对应 **ctype** 的值。
- **ct** — C 类型声明，可以大部分API使用。Either a **cdecl**, a **ctype** or a **cdata** 扮演一个模板类型。
- **cb** — 回调对象. C数据对象中持有一个特殊的函数指针. Calling this function from
  C code runs an associated Lua function.
- **VLA** — 可变长数组。使用 `?` 代替数组元素个数, 例如 `int[?]`。数组元素的个数必须在 [创建](http://luajit.org/ext_ffi_api.html#ffi_new) 的时候指定
- **VLS** — 可变长结构体(C结构体的最后一个成员是 VLA)

### 声明和访问外部符号

#### ffi.cdef(def)

该函数用来声明外部的符号(named variables or functions).  参数 `def` 必须使用Lua string。建议使用下面语法糖的形式：

```lua
ffi.cdef [[
// 声明一个结构体定义
typedef struct foo { 
    int a;
    int b; 
} foo_t; 

/* 声明一个外部的C函数 */
int dofoo(foo_t *f, int n); 
]]
```

参数 `def` 包含的内容必须是 [C声明](http://luajit.org/ext_ffi_semantics.html#clang)(多个声明使用分号分隔，与C头文件一致)。注意这里外部符号仅仅是声明，还没有与任何地址绑定，绑定是在C库命名空间里实现的。

目前不支持C预处理(宏定义展开等)，所以C声明中不能有预处理的内容(如 `#define`等)，`#pragma pack`  例外。将C的头文件中的`#define`定义替换成  `enum`, `static const` 或者`typedef`，最好使用C预处理器处理一下。 

> C语言预处理要做的工作：
> 
> - 将所有的#define删除，并且展开所有的宏定义 
> - 处理所有的条件预编译指令，比如#if #ifdef #elif #else #endif等 
> - 处理#include 预编译指令，将被包含的文件插入到该预编译指令的位置。 
> - 删除所有注释 “//”和”/* */”. 
> - 添加行号和文件标识，以便编译时产生调试用的行号及编译错误警告行号。 
> - 保留所有的#pragma编译器指令，因为编译器需要使用它们
> 
> Linux下预处理命令：
> 
> ```bash
> gcc -E test.c > test.i    # 等价于 cpp test.c >test.i 
> ```

#### ffi.C

这是C库默认的命名空间(C要大写)，它会绑定(bind)目标系统的默认符号(symbol) 或者库，这和C编译器提供默认库有些相似。在 POSIX 系统上, this binds to symbols in the default or global namespace. This includes all exported symbols from the executable and
any libraries loaded into the global namespace. 最少会包含 libc, libm, libdl (on Linux), libgcc (if compiled with GCC), as well as any exported symbols from the Lua/C API provided by LuaJIT itself.

On Windows systems, this binds to symbols exported from the *.exe, the lua51.dll (i.e. the Lua/C API
provided by LuaJIT itself), the C runtime library LuaJIT was linked
with (msvcrt*.dll), kernel32.dll, user32.dll and gdi32.dll.

#### clib = ffi.load(name [,global])

通过`name`加载指定的动态库, 返回一个新的C库命令空间,这个空间绑定了这个库里的符号(symbol). On POSIX
systems, if global is true, the library symbols are loaded into the global namespace, too. If name is a path, the library is loaded from this path.
Otherwise name is canonicalized in a system-dependent way and
searched in the default search path for dynamic libraries:

On POSIX systems, if the name contains no dot, the extension .so is appended. Also, the lib prefix is prepended
if necessary. So ffi.load("z") looks for "libz.so" in the default shared library search path.

On Windows systems, if the name contains no dot, the extension .dll is appended. So ffi.load("ws2_32") looks for "ws2_32.dll" in the default DLL search path.

### 创建 cdata 对象

The following API functions create cdata objects (type() returns "cdata"). All created cdata objects are [garbage collected](http://luajit.org/ext_ffi_semantics.html#gc).

#### cdata = ffi.new(ct [,nelem] [,init...])

根据参数 `ct` 指定的类型创建一个 `cdata` 对象。 如果 `ct` 声明的是 VLA 或者 VLS 类型，那么需要设置参数 `nelem` 来指定数组的长度 。

#### cdata = *ctype*([nelem,] [init...])

这种方式使用 cypte 作为构造函数，其他方面与上面的方式完全等效。

这两个函数都有用来初始化的 `init`  参数。

The cdata object is initialized according to the [rules for initializers](http://luajit.org/ext_ffi_semantics.html#init),
using the optional init arguments. Excess initializers cause
an error.

Performance notice: if you want to create many objects of one kind,
parse the cdecl only once and get its ctype with ffi.typeof(). Then use the ctype as a constructor repeatedly.

Please note that an anonymous struct declaration implicitly
creates a new and distinguished ctype every time you use it for ffi.new(). This is probably **not** what you want,
especially if you create more than one cdata object. Different anonymous structs are not considered assignment-compatible by the
C standard, even though they may have the same fields! Also, they
are considered different types by the JIT-compiler, which may cause an
excessive number of traces. It's strongly suggested to either declare
a named struct or typedef with ffi.cdef() or to create a single ctype object for an anonymous struct with ffi.typeof().

#### ctype = ffi.typeof(ct)

根据参数 `ct` 指定的类型创建一个 C 类型对象。

This function is especially useful to parse a cdecl only once and then
use the resulting ctype object as a [constructor](http://luajit.org/ext_ffi_api.html#ffi_new).

#### cdata = ffi.cast(ct, init)

Creates a scalar cdata object for the given ct. The cdata
object is initialized with init using the "cast" variant of
the [C type conversion rules](http://luajit.org/ext_ffi_semantics.html#convert).

This functions is mainly useful to override the pointer compatibility
checks or to convert pointers to addresses or vice versa.

#### ctype = ffi.metatype(ct, metatable)

Creates a ctype object for the given ct and associates it with
a metatable. Only struct/union types, complex numbers
and vectors are allowed. Other types may be wrapped in a struct, if needed.

The association with a metatable is permanent and cannot be changed
afterwards. Neither the contents of the metatable nor the
contents of an __index table (if any) may be modified
afterwards. The associated metatable automatically applies to all uses
of this type, no matter how the objects are created or where they
originate from. Note that pre-defined operations on types have
precedence (e.g. declared field names cannot be overriden).

All standard Lua metamethods are implemented. These are called directly,
without shortcuts and on any mix of types. For binary operations, the
left operand is checked first for a valid ctype metamethod. The __gc metamethod only applies to struct/union types and performs an implicit [ffi.gc()](http://luajit.org/ext_ffi_api.html#ffi_gc) call during creation of an instance.

#### cdata = ffi.gc(cdata, finalizer)

Associates a finalizer with a pointer or aggregate cdata object. The
cdata object is returned unchanged.

This function allows safe integration of unmanaged resources into the
automatic memory management of the LuaJIT garbage collector. Typical
usage:

```lua
local p = ffi.gc(ffi.C.malloc(n), ffi.C.free)
...
p = nil -- Last reference to p is gone.
-- GC 会自动执行 ffi.C.free(p) 释放内存
```

A cdata finalizer works like the __gc metamethod for userdata
objects: when the last reference to a cdata object is gone, the
associated finalizer is called with the cdata object as an argument. The
finalizer can be a Lua function or a cdata function or cdata function
pointer. An existing finalizer can be removed by setting a nil finalizer, e.g. right before explicitly deleting a resource:

ffi.C.free(ffi.gc(p, nil)) -- Manually free the memory.

### C 类型信息

下面的API用来返回 C 类型的信息。主要用来检测 `cdata` 对象。

#### ffi.sizeof

```lua
size = ffi.sizeof(ct [,nelem])
```

返回参数 `ct` 指向类型的字节数。如果大小未知，则返回`nil`(例如 `void` 或者 函数类型)。对于 VLA / VLS 类型，除了**cdata** 对象，都需要指定参数`nelem`。

```lua
local ffi = require('ffi')

ffi.cdef [[
    typedef struct {
        int a;
        int b;
    }data_t;

    // VLS 结构体
    typedef struct { 
        int data;
        char vla[?]; // VLA
    } vls_t;
]]

local size

size = ffi.sizeof('data_t')
print('size = ', size)  ---> size = 8

size = ffi.sizeof('data_t[5]')
print('size = ', size) ---> size = 40

-- 计算 VLA 的大小
size = ffi.sizeof('data_t[?]', 5)
print('size = ', size) ---> size = 40

-- 计算 cdata 的大小
local array = ffi.new('data_t[?]', 5)
size = ffi.sizeof(array)
print('size = ', size) ---> size = 40

-- 计算 VLS 的大小
size = ffi.sizeof('vls_t', 4)
print('size = ', size)  ---> size = 8
```



#### ffi.alignof

```lua
align = ffi.alignof(ct)
```

返回参数 `ct` 指向类型需要对齐的字节数。`alignof` 是C++11的一个关键字，整体来说就是C语言的结构体对齐问题(不同编译器对结构体对齐要求略有差异，C++的类也有结构体对齐的要求)。通常来说返回值就是结构体中占用字节数最多的那个类型的长度。这个函数对于只关注上层编程的人，基本没有什么用处。

看下面的例子，环境是Linux x64，不同环境类型占用的字节数是有差异的，一些单片机中int只占2个字节。

```lua
local ffi = require('ffi')

ffi.cdef [[
    typedef struct {
        char   a;
        signed b;
        int    c;
        long   d;
        float  e;
        double f;
    }data1_t;

    typedef struct {
        char a;
        int b[10];
    }data2_t;
]]

local size

size = ffi.alignof('data1_t')
print('data1_t align = ', size)  ---> 8

size = ffi.sizeof('data1_t')
print('data1_t size  = ', size) ---> 40


size = ffi.alignof('data2_t')
print('data2_t align = ', size)  ---> 4

size = ffi.sizeof('data2_t')
print('data2_t size  = ', size)  ---> 44
```

#### ffi.offsetof

```lua
ofs [,bpos,bsize] = ffi.offsetof(ct, field)
```

Returns the offset (in bytes) of field relative to the start
of ct, which must be a struct. Additionally returns
the position and the field size (in bits) for bit fields.

#### ffi.istype

```lua
status = ffi.istype(ct, obj)
```

Returns true if obj has the C type given by ct. Returns false otherwise.

C type qualifiers (const etc.) are ignored. Pointers are checked with the standard pointer compatibility rules, but without any special treatment for void *. If ct specifies a struct/union, then a pointer to this type is accepted, too. Otherwise the types must match exactly.

Note: this function accepts all kinds of Lua objects for the obj argument, but always returns false for non-cdata objects.

### Utility Functions

#### ffi.errno

```lua
err = ffi.errno([newerr])
```

Returns the error number set by the last C function call which
indicated an error condition. If the optional newerr argument
is present, the error number is set to the new value and the previous
value is returned.

This function offers a portable and OS-independent way to get and set the
error number. Note that only *some* C functions set the error number. And it's only significant if the function actually indicated an error condition (e.g. with a return value of -1 or NULL). Otherwise, it may or may not contain any previously set value.

You're advised to call this function only when needed and as close as possible after the return of the related C function. The errno value is preserved across hooks, memory allocations, invocations of the JIT compiler and other internal VM activity. The same
applies to the value returned by GetLastError() on Windows, but you need to declare and call it yourself.

#### ffi.string

```lua
str = ffi.string(ptr [,len])
```

根据指针指向的数据，创建一个Lua 字符串。

- 如果省略参数 `len` ，那么 `ptr` 会转换成 `char *` 类型，并且假定 `ptr`指向的数据是 NULL 结尾的，字符串的长度使用`strlen()` 计算。

- 否则，`ptr` 会被转换成 `void *` 类型，使用给定参数 `len`  获取数据，数据中可能内嵌NULL并且不需要面向字节(可能会引起大小端问题)。

这个函数的主要作用是将 C 函数返回的 `const char *` 类型转换成 Lua 字符串。来达到存储或者传递给其他需要Lua字符串参数的函数。 转换后的Lua字符串是原始数据的拷贝，之后与原始数据就没有任何关系了。

性能提示：如果已知字符串的长度，那么声明参数 `len` 会使函数运行更快。



#### ffi.copy

```lua
ffi.copy(dst, src, len)
ffi.copy(dst, str)
```

将数据从 `src` 拷贝到 `dst`。`dst` 被转换成 `void *` 类型，`src` 会被转换成 `const void *` 类型。

第一种方式，参数 `len` 会指定拷贝的字节数量，如果参数 `src` 是一个Lua字符串，那么`len` 的长度不能超过 `#src+1`。

```c
local ffi = require('ffi')

ffi.cdef [[
    void *malloc(size_t size);
    void free(void *ptr);
    int printf(const char *format, ...);
]]

local c_buf = ffi.C.malloc(12)

local lua_str = 'hello world'
ffi.copy(c_buf, lua_str, #lua_str + 1)
ffi.C.printf("%s\n", c_buf)
ffi.C.free(c_buf)
```

第二种方式，`src` 必须是一个Lua字符串。所有Lua字符串的字节加上一个NULL(`#src+1`个字节)被拷贝到 `dst` 。

```c
local ffi = require('ffi')

ffi.cdef [[
    void *malloc(size_t size);
    void free(void *ptr);
    int  printf(const char *format, ...);
]]

local c_buf = ffi.C.malloc(12)

local lua_str = 'hello world'
ffi.copy(c_buf, lua_str)
ffi.C.printf("%s\n", c_buf)
ffi.C.free(c_buf)
```

性能提示：`ffi.copy()` 可以用作C 函数 `memcpy()`, `strcpy()` ,`strncpy()`性能更好的替代。

#### ffi.fill

```lua
ffi.fill(dst, len [,c])
```

填充函数，功能同 C 语言的 `memset()` 函数功能完全相同。使用参数 `c` 的值填充参数`dst`指向的位置 `len` 长度的数据。参数 `c` 默认值为0。该函数可用作 `memset(dst，c，le)`的替代，因为其性能更好。另外请注意参数的顺序不同！

```lua

ffi.cdef [[
   typedef struct {
	   int a;
	   int b;
   } data_t;
]]

local array_ptr  = ffi.new('data_t[10]')

--计算数组长度(字节数)
local array_size = ffi.sizeof('data_t[10]')
print('array size: ', array_size)   --> 80

--使用 1 填充结构体数组,四字节的int型被填充为 0x01010101
ffi.fill(array_ptr, array_size, 1)

print(0x01010101)
for i=0,9 do
	print('array[',i,'] = ', array_ptr[i].a)
end
```



### Target-specific Information

#### status = ffi.abi(param)

Returns true if param (a Lua string) applies for the
target ABI (Application Binary Interface). Returns false otherwise. The following parameters are currently defined:

| Parameter | Description                         |
| --------- | ----------------------------------- |
| 32bit     | 32 bit architecture                 |
| 64bit     | 64 bit architecture                 |
| le        | Little-endian architecture          |
| be        | Big-endian architecture             |
| fpu       | Target has a hardware FPU           |
| softfp    | softfp calling conventions          |
| hardfp    | hardfp calling conventions          |
| eabi      | EABI variant of the standard ABI    |
| win       | Windows variant of the standard ABI |

### ffi.os

[变量非函数]当前操作系统的名称(字符串), 取值范围如下 :

- "Windows",

- "Linux", 

- "OSX", 

- "BSD", 

- "POSIX" 

- "Other"

```lua
local ffi=require('ffi')
print(ffi.os)  --> Linux
```

### ffi.arch

[变量非函数]当前操作系统架构的名称(字符串), 取值范围如下:

- "x86"

- "x64"

- "arm"

- "ppc"

- "ppcspe"

- "mips"

```lua
local ffi=require('ffi')
print(ffi.arch)  --> x64
```

## Methods for Callbacks

The C types for [callbacks](http://luajit.org/ext_ffi_semantics.html#callback) have some extra methods:

### cb:free()

Free the resources associated with a callback. The associated Lua
function is unanchored and may be garbage collected. The callback
function pointer is no longer valid and must not be called anymore
(it may be reused by a subsequently created callback).

### cb:set(func)

Associate a new Lua function with a callback. The C type of the
callback and the callback function pointer are unchanged.

This method is useful to dynamically switch the receiver of callbacks
without creating a new callback each time and registering it again (e.g.
with a GUI library).

## 对标准库函数的扩展

针对于FFI的功能，对标准的Lua库函数进行了扩展，使其能支持cdata对象的操作。

### n = tonumber(cdata)

Converts a number cdata object to a double and returns it as
a Lua number. This is particularly useful for boxed 64 bit
integer values. Caveat: this conversion may incur a precision loss.

### s = tostring(cdata)

Returns a string representation of the value of 64 bit integers
(**"**nnn**LL"** or **"**nnn**ULL"**) or
complex numbers (**"**re±im**i"**). Otherwise
returns a string representation of the C type of a ctype object
(**"ctype<**type**>"**) or a cdata object
(**"cdata<**type**>:** address"), unless you
override it with a __tostring metamethod (see [ffi.metatype()](http://luajit.org/ext_ffi_api.html#ffi_metatype)).

### iter, obj, start = pairs(cdata)

iter, obj, start = ipairs(cdata)  

Calls the __pairs or __ipairs metamethod of the
corresponding ctype.

## 对Lua解释器的扩展

The parser for Lua source code treats numeric literals with the
suffixes LL or ULL as signed or unsigned 64 bit integers. Case doesn't matter, but uppercase is recommended for readability. It handles both decimal (42LL) and hexadecimal(0x2aLL) literals.

The imaginary part of complex numbers can be specified by suffixing
number literals with i or I, e.g. 12.5i.
Caveat: you'll need to use 1i to get an imaginary part with the value one, since i itself still refers to a variable named i.
