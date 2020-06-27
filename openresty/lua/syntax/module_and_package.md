# 模块

## 基本概念

一个`模块`（module）就是一些代码（Lua或者C）的集合，这些代码可以通过 *require* 函数加载，然后创建和返回一个表。这个表就像是一个命名空间，其中定义的内容是模块中导出的东西，比如函数和常量。模块的意义就是代码的复用。Lua中的标准库都是模块，独立解释器会隐式加载所有的标准库，所以通常不需要*require*函数声明后再使用。**模块是使用表实现的**，这样的好处是我们可以像操作表一样操作模块，因为表是第一类值。

## 函数require

require函数负责加载模块

### 语法

```lua
--普通方法
local m = require('math') --local m = require("math")

--当函数的参数只有一个字符串常量时，括号可以省略，这是常用方法
local m = require 'math'

--用变量指定模块名，再加载
local modename = 'math'
local m = require(modename)

--只引入某个模块的特定函数
local sin = require "math".sin
```

### 工作原理

首先，函数 require 会在表 *package.load* 中检查模块是否已经加载，如果模块已经加载，那么就返回相应的值，不会重复加载；如果没有加载过，那么就会到 *packeage.path* 中的路径里搜素指定模块名称的Lua文件。如果找到了则调用*loadfie*函数加载该文件。如果没有找到该Lua文件，则会到 *package.cpath* 指定的路径中搜索同名的动态库文件，如果找到了则调用*package.loadlib* 函数加载该库文件。

> 为了最终加载模块，函数 require 会带着2个参数调用加载函数，模块名和加载函数所在的文件名（通常所有模块会忽略这两个参数）。如果加载函数有返回值，那么函数require 会返回这个值，然后存储在package.load  表中，以便与以后加载统一模块返回相同的值。如果加载函数没有返回值，且表 package.load[@req{modname}] 为空，则 函数 require 假定模块返回true，如果没有这种补偿，后续有可能重复加载模块。
> 
> 强制重新加载模块的方法如下：
> 
> ```lua
> -- 清空加载记录
> package.loaded.modname = nil
> 
> -- 下一次 require 这个模块的时候，就会重新加载
> ```

由于加载的模块会在后续被复用，所以不能提供模块初始化参数（像类的构造函数的初始化列表），否则会有同名模块的冲突，解决这个问题的最好方法是模块提供初始化函数：

```lua
local mod = require 'somemodule'
mod.init(1,2,3) --由模块提供初始化函数

--如果加载函数返回的是模块本身，还可以写成下面的形式
loca mod = require "somemodule".init(1,2,3)

--实际例子
local xml = require("lib.xmlSimple").newParser()
```

### 模块重命名

TBD

### 模块搜索

TBD

## 编写模块

Lua中创建模块的最简单方法就是创建一个表并将所有需要导出的函数放入其中，最后返回这个表。

```lua
-- easycalc.lua

local _M = {version = "1.0"}  -- 模块

--常量
_M.pi = 3.1415926

--加法
function _M.add(x, y)
    return x + y
end

--加法
function _M.sub(x, y)
    return x - y
end

--乘法
function _M.mul(x, y)
    return x * y
end

--除法
function _M.div(x, y)
    return x / y
end

--返回表
return _M
```

随后就可以使用这个模块了

```lua
local test = require "easycalc"

print(test.pi)
print(test.add(1,2))
print(test.sub(1,2))
print(test.mul(1,2))
print(test.div(1,2))

--[[ 运行结果
3.1415926
3
-1
2
0.5
--]]
```

> 有的人不喜欢最后的return语句，那么可以将模块直接放到package.loaded中
> 
> ```lua
> local _M = {}
> package.loaded[...] = _M
> 
> --函数和常量定义
> 
> --最后不return _M
> ```
> 
> 还有一种模块的写法是，把所有函数都定义为局部变量，然后在最后构造返回的表
> 
> ```lua
> --常量
> local pi = 3.1415926
> 
> --加法
> local function add(x, y)
> 	return x + y
> end
> 
> --加法
> local function sub(x, y)
> 	return x - y
> end
> 
> --乘法
> local function mul(x, y)
> 	return x * y
> end
> 
> --除法
> local function mydiv(x, y)
> 	return x / y
> end
> 
> return {
> 	pi = pi,
> 	add = add,
> 	sub = sub,
> 	mul = mul,
> 	div = mydiv,
> }
> ```

这里只是介绍最简单的模块编写方法，后续会在 元表 和 元方法 章节介绍更复杂功能更加大的模块编写。



# 包（package）

一个包是一颗由模块组成的完整的树，他是Lua语言中用于发行程序的单位。包是模块的集合，包中的模块被称为包的子模块。Lua中的子模块用.号隔开。如mod.sub是mod的子模块。
