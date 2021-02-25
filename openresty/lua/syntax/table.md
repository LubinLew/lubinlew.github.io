# 表

表是Lua语言中唯一的数据结构, 表可以用作表示 包(package)  对象

Lua中的表实际上是一种辅助数组,这种数组不仅可以使用数值作为索引, 也可以使用字符串或其他任意类型作为索引



## 表的标准库

This library provides generic functions for table manipulation. It provides all its functions inside the table `table`.

| 函数             | 否能被JIT编译器编译                                 | 备注             |
| -------------- | ------------------------------------------- | -------------- |
| table.concat   | 2.1                                         |                |
| table.foreach  | no(2.1: builtin compiled, but NYI on ITERN) |                |
| table.foreachi | 2.1                                         |                |
| table.getn     | yes                                         |                |
| table.insert   | partial(Only when pushing)                  |                |
| table.maxn     | no                                          |                |
| table.pack     | no                                          | LuaJIT扩展Lua5.2 |
| table.remove   | 2.1                                         |                |
| table.sort     | no                                          |                |
| table.unpack   | no                                          | LuaJIT扩展Lua5.2 |
| table.new      | unknown, possible yes                       | LuaJIT独有扩展     |
| table.clear    | unknown, possible yes                       | LuaJIT独有扩展     |
| table.nkeys    | yes                                         | openresty独有扩展  |
| table.isempty  | yes                                         | openresty独有扩展  |
| table.isarray  | yes                                         | openresty独有扩展  |
| table.clone    | yes                                         | openresty独有扩展  |

Most functions in the table library assume that the table represents an array or a list. For these functions, when we talk about the "length" of a table we mean the result of the length operator.

### `table.concat (table [, sep [, i [, j]]])`

Given an array where all elements are strings or numbers, returns `table[i]..sep..table[i+1] ··· sep..table[j]`. The default value for `sep` is the empty string, the default for `i` is 1, and the default for `j` is the length of the table. If `i` is greater than `j`, returns the empty string.

### `table.insert (table, [pos,] value)`

Inserts element `value` at position `pos` in `table`, shifting up other elements to open space, if necessary. The default value for `pos` is `n+1`, where `n` is the length of the table (see [§2.5.5](http://www.lua.org/manual/5.1/manual.html#2.5.5)), so that a call `table.insert(t,x)` inserts `x` at the end of table `t`.

### `table.maxn (table)`

Returns the largest positive numerical index of the given table, or zero if the table has no positive numerical indices. (To do its job this function does a linear traversal of the whole table.)

### `table.remove (table [, pos])`

Removes from `table` the element at position `pos`, shifting down other elements to close the space, if necessary. Returns the value of the removed element. The default value for `pos` is `n`, where `n` is the length of the table, so that a call `table.remove(t)` removes the last element of table `t`.

### `table.sort (table [, comp])`

Sorts table elements in a given order, *in-place*, from `table[1]` to `table[n]`, where `n` is the length of the table. If `comp` is given, then it must be a function that receives two table elements, and returns true when the first is less than the second (so that `not comp(a[i+1],a[i])` will be true after the sort). If `comp` is not given, then the standard Lua operator `<` is used instead.

The sort algorithm is not stable; that is, elements considered equal by the given order may have their relative positions changed by the sort.

### table.isempty(table)

**syntax:** *res = isempty(tab)*

Returns `true` when the given Lua table contains neither non-nil array elements nor non-nil key-value pairs, or `false` otherwise.

This API can be JIT compiled.

Usage:

```lua
local isempty = require "table.isempty"

print(isempty({}))  -- true
print(isempty({nil, dog = nil}))  -- true
print(isempty({"a", "b"}))  -- false
print(isempty({nil, 3}))  -- false
print(isempty({cat = 3}))  -- false
```

### table.isarray(table)

**syntax:** *res = isarray(tab)*

Returns `true` when the given Lua table is a pure array-like Lua table, or `false` otherwise.

Empty Lua tables are treated as arrays.

This API can be JIT compiled.

Usage:

```lua
local isarray = require "table.isarray"

print(isarray{"a", true, 3.14})  -- true
print(isarray{"dog" = 3})  -- false
print(isarray{})  -- true
```



### table.nkeys(tab)

**syntax:** *n = nkeys(tab)*

Returns the total number of elements in a given Lua table (i.e. from both the array and hash parts combined).

This API can be JIT compiled.

Usage:

```lua
local nkeys = require "table.nkeys"

print(nkeys({}))  -- 0
print(nkeys({ "a", nil, "b" }))  -- 2
print(nkeys({ dog = 3, cat = 4, bird = nil }))  -- 2
print(nkeys({ "a", dog = 3, cat = 4 }))  -- 3
```



### table.clone(tab)

**syntax:** *t = clone(tab)*

Returns a shallow copy of the given Lua table.

This API can be JIT compiled.

Usage:

```lua
local clone = require "table.clone"

local x = {x=12, y={5, 6, 7}}
local y = clone(x)
... use y ...
```

**Note:** We observe 7% over-all speedup in the edgelang-fan compiler's compiling speed whose Lua is generated by the fanlang compiler.

**Note bis:** Deep cloning is planned to be supported by adding `true` as a second argument.


