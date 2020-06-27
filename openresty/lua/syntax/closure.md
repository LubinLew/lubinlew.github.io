# 闭包与函数的关系

在Lua语言中，函数是严格遵循`词法定界`（*lexical scoping*）的`第一类值`（*first-class value*）。这两个特性允许我们在Lua语言中使用很多`函数式语言`（*functional-language*）的强大编程技巧。

`第一类值`意味着函数与其他常见类型的值（例如数值和字符串）具有同等权限：

- 一个程序可以将某个函数保存到变量中（全局变量和局部变量均可）；

- 一个程序可以将某个函数保存到表中（实现面向对象的关键）；

- 也可以将某个函数作为参数传递给其他函数；

- 还可以将某个函数作为其他函数的返回值返回。

`词法定界`意味着函数可以访问包含其自身的外部函数中的变量（Lua支持Lambda演算）。当编写一个被其他函数B包含的函数A时，被包含的函数A可以访问包含其的函数B的所有局部变量，这种特性被称为`词法定界`。函数B中能被函数A访问的局部变量在函数A中被称为`非局部变量`（*non-local variable*），在Lua中也被称为 `上值`（*upvalue*）。

`匿名函数` 在Lua中所有的函数都是匿名函数，向其他所有值一样，函数并没有名字，当我们讨论函数名时，例如 打印函数*print*，实际上*print*只是保存该函数的变量。

`闭包`就是一个函数能够使该函数正确访问非局部变量所需的其他机制。从技术上讲，Lua中其实只有闭包没有函数，函数本身只是闭包的一种原型，不过为了不引起混淆，Lua中仍以函数来指代闭包。

# 第一类值

看一下下面的例子，我们甚至可以随意修改Lua标准库的函数，这就是第一类值的含义。

```lua
--函数赋值给表
a = {p = print}
a.p("hello world!")

--重定义了函数print
print = math.sin
a.p(print(1))

--改写函数 math.sin
math.sin = a.p
math.sin("hello world!")

--[[ 运行结果
hello world!
0.8414709848079
hello world!
--]]
```

## 函数的定义

函数作为第一类值，同样有创建函数的表达式。

```lua
--常见的函数定义方式，但是这其实是下面另一种方式的语法糖（美化形式）
function foo(x) return x+1 end

--通常形式
foo = function(x) return x+1 end
```

## 局部函数

当把一个函数存储到一个局部变量时， 就得到了一个`局部函数`（*local function*），即一个被限定在指定作用域中使用的函数。局部函数对于`包`（*package*）而言尤其又用。由于Lua语言将每个`程序段`（*chunk*）作为一个函数处理，所以在一段程序中声明的函数就是局部函数，这些局部函数只在该程序段中可见。词法定界保证了程序段中的其他函数可以使用这些局部函数。

### 的定义

```lua
--局部函数的定义（语法糖）
local funcion foo(x)
    return x+1
end


--局部函数的定义（语法糖的展开）
local foo

foo = function(x)
    return x+1
end
```

语法糖的这种展开方式对于直接递归调用也是适用的。

```lua
--直接递归函数，求n的阶层
local function foo(n)
    if n > 0 then 
        return n * foo(n-1)
    end
    return 1
end

print(foo(3))
print(foo(5))
print(foo(10))

--[[ 运行结果
6
120
3628800
--]]
```

> 直接递归：函数A的调用函数A
> 
> 间接递归：函数A中调用函数B，函数B中调用函数A

间接递归的时，函数必须向前声明。

```lua
--间接递归调用例子

local funcA  -- 向前声明

local function funcB(n)
    if n > 0 then
        return n + funcA(n-1)
    end
    return 1
end

--[[
这里funcA 不能再用local限定，否则Lua会创建一个全新的局部变量funcA，
从而使先前声明的 funcA（funcB中使用的那个）变为未为定义状态，错误信息如下：
>>> attempt to call upvalue 'funcA' (a nil value)
--]]
function funcA(n)
    if n > 0 then
        return n * funcB(n-1)
    end
    return 1
end


print(funcA(3)) -- 3*(2+1*1)
print(funcB(3)) -- 3+(2*(1+1))

--[[ 运行结果
9
7
--]]
```

# 闭包的使用

函数作为第一类值，能够逃逸出他们变量的原始定界范围。看下面的例子

```lua
local function newCounter()
    local count = 0
    return function () -- 匿名函数
        count = count + 1
        return count
    end
end

--创建针对变量c1的闭包
local c1 = newCounter()
print(c1())
print(c1())
print(c1())

--创建针对变量c2的闭包，c1 和 c2 是不同的闭包
local c2 = newCounter()
print(c2())
print(c2())
print(c2())

--[[ 运行结果
1
2
3
1
2
3
--]]
```

在上面的代码中，匿名函数访问了一个非局部变量（*count*），将其作为计数器。然而由于创建变量的函数 （newCounter）已经返回，当我们调用匿名函数时，变量count似乎已经超出了作用范围，其实不然，由于闭包（*一个函数外加能够使该函数正确访问非局部变量所需的其他机制*）的存在，会创建一个新的局部变量*count*和闭包。上面例子中，c1 和 c2 虽然建立在相同的函数上，但却是不同的闭包，各自拥有独立的局部变量*count*的实例。

闭包函数中的非局部变量也可以是函数的参数，请看下面的例子:

```lua
--闭包函数,这里的非局部变量是参数n
local function displayInput(n)
    return { --返回一个表，表中有匿名函数（相当于回调函数）
        label = n,
        action = function ()
            print("upvalue "..n)
        end
    }
end


local dis = displayInput(2333)
print(dis.label)
dis.action()

dis = displayInput(1234)
print(dis.label)
dis.action()

--[[ 运行结果
2333
upvalue 2333
1234
upvalue 1234
--]]
```

## 沙盒

我们可以使用闭包 加 do 代码段 这来个技巧来实现`沙盒`（*sanbox*）技术，也叫做`安全运行时环境`。先看一下简单的例子，math.sin接受参数为弧度，我们将其重写为接受角度参数。

```lua
--[[
标准库函数 math.sin(x),参数x表示弧度(radian)
弧度和角度的计算公式：1rad=180°/π
--]]
print(math.sin(1))

--重定义 math.sin 函数，参数x表示角度
do --do-end 代码块是用来限制 oldSin 的作用域仅在该代码块内
    local oldSin = math.sin
    local k = math.pi / 180
    math.sin = function (x)
        return oldSin(x * k)
    end
end

print(math.sin(1))

--[[ 运行结果
0.8414709848079
0.017452406437284
--]]
```

do 程序块限制了 oldSin 只在该程序块内有效，也就意味着此后不可能有人会有机会直接调用原版的math.sin 函数了，利用这种特性可以防止某些不受信任的程序调用比较危险的库函数（例如文件的读写等）。

# 函数式编程

函数式编程关心类型（代数结构）之间的关系，命令式编程关心解决问题的步骤。

`面向对象`的世界里我们是把`事物`抽象成`类`和`对象`,然后通过`封装`、`继承`和`多态`来演示他们之间的关系。`函数式`的世界里把世界抽象成`事物`和`事物之间的关系`，用这种方式实现世界的模型。

函数式编程中的lambda可以看成是两个类型之间的关系，一个输入类型和一个输出类型。lambda演算就是给lambda表达式一个输入类型的值，则可以得到一个输出类型的值。

TBD
