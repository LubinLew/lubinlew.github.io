# LuaJIT简介

[LuaJIT](https://luajit.org/) 是一个非官方的即时([Just-In-Time](https://zh.wikipedia.org/wiki/%E5%8D%B3%E6%99%82%E7%B7%A8%E8%AD%AF))Lua编译器，其执行Lua程序的运行效率比官方的解释器快很多，这也是OpenResty选择LuaJIT的原因。

‌[JIT加速程序运行的原理](https://time.geekbang.org/column/article/100402)是将频繁调用的代码块（热代码），先转换成 LuaJIT 自己定义的中间码（IR），然后再生成针对目标体系结构的机器码。所以LuaJIT 的性能优化，本质上就是让尽可能多的 Lua 代码可以被 JIT 编译器生成机器码，而不是回退到 Lua 解释器的解释执行模式。明白了这个道理，你才能理解后面学到的 OpenResty 性能优化的本质。



# LuaJIT与标准Lua的语法区别

LuaJIT是基于Lua 5.1语法开发的，后续又实现了Lua 5.2 和 Lua 5.3部分与5.1语法兼容的新特性，LuaJIT 增加了不少独有的函数，这些函数非常重要。当前LuaJIT作者退休，也没有到新的维护者，所以 OpenResty维护了一个自己的LuaJIT分支 [openresty/luajit2](https://github.com/openresty/luajit2)，并扩展了很多独有的 API，基本都是出于对性能方面的考虑而增加的。

‌LuaJIT扩展介绍 [Extensions](http://luajit.org/extensions.html)

OpenResty扩展