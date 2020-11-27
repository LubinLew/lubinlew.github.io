# 简介

FFI 库允许在纯 Lua 代码中调用外部的 `C 函数` 和 使用 `C 的数据结构`。标准的 Lua 调用 C 函数需要编写额外的绑定函数(绑定函数的编写很复杂)，使用 FFI 就可以省略这个复杂的步骤，因为 FFI 可以解析 C 语言的声明。FFI 库与 LuaJIT 是一体的，不能分离成一个单独的模块。JIT 编译器对访问 C 数据结构生成的代码与 C 编译器生成的代码性能相差无几，性能远高于使用绑定函数的方式。



## 例子 : 调用外部 C 函数

调用外部的 C 函数非常简单, 看下面的例子:

```lua
-- 加载 FFI 库
local ffi = require("ffi")

-- 外部 C 函数的声明(完全是 C 语法)
ffi.cdef[[
    int printf(const char *fmt, ...);
]]

-- 调用 声明的 外部函数
ffi.C.printf("Hello %s!", "world")
```

看似简单的使用，其背后隐藏了许多细节:

- 标准 C 库命令空间的使用 `ffi.C`

- 在命令空间中绑定 `printf`函数

- 函数的参数自动从 Lua 类型转换成 C 类型



下面是一个在Windows系统中弹出消息对话框(message box)的例子:

```lua
local ffi = require("ffi")

ffi.cdef[[
    int MessageBoxA(void *w, const char *txt, const char *cap, int type);
]]

ffi.C.MessageBoxA(nil, "Hello world!", "Test", 0)
```

和使用经典 Lua/C API 编写绑定函数而言，这实在是太简单了。



## 例子 : 使用 C 数据结构

FFI 库允许你创建和使用 C 数据结构(结构体/联合体)。使用 C 数据结构主要是用来和 C 函数交互的，但是也可以独立使用。Lua 使用高级的数据类型，这些类型拥有 灵活、可扩展和动态的优点。这些优点在一些特殊情况下可能导致性能低下，有时你需要一个低级的数据类型，例如一个固定结构的大数组，如果使用 Lua 实现，就是一个大 table 包含许多小 table，这会带来大量的内存开销和性能开销。

Here's a sketch of a library that operates on color images plus a simple benchmark. First, the plain Lua version:

```lua
local floor = math.floor

local function image_ramp_green(n)
  local img = {}
  local f = 255/(n-1)
  for i=1,n do
    img[i] = { red = 0, green = floor((i-1)*f), blue = 0, alpha = 255 }
  end
  return img
end

local function image_to_grey(img, n)
  for i=1,n do
    local y = floor(0.3*img[i].red + 0.59*img[i].green + 0.11*img[i].blue)
    img[i].red = y; img[i].green = y; img[i].blue = y
  end
end

local N = 400*400
local img = image_ramp_green(N)
for i=1,1000 do
  image_to_grey(img, N)
end
```

上面创建了一个拥有16万像素的 table，每一个像素是含有4个数值(取值范围0-255)的 table。首先使用一维创建一个图片First an image with a green ramp is created (1D for simplicity), then the image is converted to 灰度(greyscale) 1000 times. Yes, that's silly, but I was in need of a simple example ...

And here's the FFI version. The modified parts have been marked in bold:

```lua
-- 加载 FFI 库
local ffi = require("ffi")

-- 声明 数据结构
ffi.cdef[[
   typedef struct { 
       uint8_t red; 
       uint8_t green;
       uint8_t blue;
       uint8_t alpha; 
} rgba_pixel;
]]

local function image_ramp_green(n)
  local img = ffi.new("rgba_pixel[?]", n)
  local f = 255/(n-1)
  for i=0,n-1 do
    img[i].green = i*f
    img[i].alpha = 255
  end
  return img
end

local function image_to_grey(img, n)
  for i=0,n-1 do
    local y = 0.3*img[i].red + 0.59*img[i].green + 0.11*img[i].blue
    img[i].red = y; img[i].green = y; img[i].blue = y
  end
end

local N = 400*400
local img = image_ramp_green(N)
for i=1,1000 do
  image_to_grey(img, N)
end
```

Ok, so that wasn't too difficult:

① First, load the FFI library and declare the low-level data type. Here we choose a struct which holds four byte fields, one for each component of a 4x8 bit RGBA pixel.

② Creating the data structure with ffi.new() is straightforward — the '?' is a placeholder for the number of elements of a variable-length array.

③ C arrays are zero-based, so the indexes have to run from 0 to n-1. One might want to allocate one more element instead to simplify converting legacy code.

④ Since ffi.new() zero-fills the array by default, we only need to set the green and the alpha fields.

⑤ The calls to math.floor() can be omitted here, because floating-point numbers are already truncated towards zero when converting them to an integer. This happens implicitly when the number is stored in the fields of each pixel.

Now let's have a look at the impact of the changes: first, memory consumption for the image is down from 22 Megabytes to 640 Kilobytes (400*400*4 bytes). That's a factor of 35x less! So, yes, tables do have a noticeable overhead. BTW: The original program would consume 40 Megabytes in plain Lua (on x64).

Next, performance: the pure Lua version runs in 9.57 seconds (52.9 seconds with the Lua interpreter) and the FFI version runs in 0.48 seconds on my machine (YMMV). That's a factor of 20x faster (110x faster than the Lua interpreter).

The avid reader may notice that converting the pure Lua version over to use array indexes for the colors ([1] instead of .red, [2] instead of .green etc.) ought to be more compact and faster. This is certainly true (by a factor of ~1.7x). Switching to a struct-of-arrays would help, too.

However the resulting code would be less idiomatic and rather error-prone. And it still doesn't get even close to the performance of the FFI version of the code. Also, high-level data structures cannot be easily passed to other C functions, especially I/O functions, without undue conversion penalties.
