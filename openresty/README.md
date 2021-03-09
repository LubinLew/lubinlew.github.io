# 学习资料

[《OpenResty从入门到实战》](https://time.geekbang.org/column/intro/186) -- 温铭(OpenResty软件基金会主席) 极客时间(付费)

[《OpenResty 最佳实践》](https://yousali.me/openresty-best-practices/) -- 温铭(OpenResty软件基金会主席) 免费gitbook

[《Promramming OpenResty》](https://openresty.gitbooks.io/programming-openresty/content/)-- 章亦春(OpenResty作者) 免费gitbook(英文)

《OpenResty完全开发指南》-- 罗剑锋（实体书）

《Lua程序设计》 -- Roberto Ierusalimschy (Lua作者) 实体书

****

# OpenResty的优点

## 同步非阻塞编程

‌nginx的模型是的异步非阻塞，这也是其高性能的一个重要因素，但是异步编程的最大问题是编码过程会非常复杂，大部分情况下都是通过回调函数实现的，而在OpenResty中提供大量的同步非阻塞函数，将异步的操作隐藏。提高编程效率。

‌

## 协程和 cosocket

‌

## 动态加载配置

‌nginx想要修改配置文件需要重新加载配置，但是OpenResty可以通过定时脚本加载配置

***

# 软件构成

OpenResty 由以下开源软件构成。

## nginx

‌为了解决一些问题，OpenResty对Nginx做了一些patch

## LuaJIT

[LuaJIT](https://luajit.org/) 是一个非官方的即时([Just-In-Time](https://zh.wikipedia.org/wiki/%E5%8D%B3%E6%99%82%E7%B7%A8%E8%AD%AF))Lua编译器，其执行Lua程序的运行效率比官方的解释器快很多，这也是OpenResty选择LuaJIT的原因。

‌[JIT加速程序运行的原理](https://time.geekbang.org/column/article/100402)是将频繁调用的代码块，先转换成 LuaJIT 自己定义的中间码（IR），然后再生成针对目标体系结构的机器码。所以LuaJIT 的性能优化，本质上就是让尽可能多的 Lua 代码可以被 JIT 编译器生成机器码，而不是回退到 Lua 解释器的解释执行模式。明白了这个道理，你才能理解后面学到的 OpenResty 性能优化的本质。

‌因为LuaJIT是基于Lua 5.1语法开发的，后续又实现了Lua 5.2 和 Lua 5.3部分与5.1语法兼容的新特性，LuaJIT 增加了不少独有的函数，这些函数非常重要。当前LuaJIT作者退休，也没有到新的维护者，所以 OpenResty维护了一个自己的LuaJIT分支 [openresty/luajit2](https://github.com/openresty/luajit2)，并扩展了很多独有的 API，基本都是出于对性能方面的考虑而增加的。

‌LuaJIT扩展介绍 http://luajit.org/extensions.html

**LuaJIT 有两种运行模式：JIT(编译)、interpreter(解释器)**。

‌**JIT模式**：这是luajit高效所在，简单地说就是直接将代码编译成机器码级别执行，效率大大提升（事实上这个机制没有说的那么简单，下面会提到）。然而不幸的是这个模式在ios下是无法开启的，因为ios为了安全，从系统设计上禁止了用户进程自行申请有执行权限的内存空间，因此你没有办法在运行时编译出一段代码到内存然后执行，所以jit模式在ios以及其他有权限管制的平台（例如ps4，xbox）都不能使用。

‌**interpreter模式**：那么没有jit的时候怎么办呢？还有一个interpreter模式。事实上这个模式跟原生lua的原理是一样的，就是并不直接编译成机器码，而是编译成中间态的字节码（bytecode），然后每执行下一条字节码指令，都相当于swtich到一个对应的function中执行，相比之下当然比jit慢。但好处是这个模式不需要运行时生成可执行机器码（字节码是不需要申请可执行内存空间的），所以任何平台任何时候都能用，跟原生lua一样。这个模式可以运行在任何luajit已经支持的平台，而且你可以手动关闭jit，强制运行在interpreter模式下。我们经常说的将**lua编译成bytecode可以防止破解**，这个bytecode是interpreter模式的bytecode，并不是jit编译出的机器码（事实上还有一个在bytecode向机器码转换过程中的中间码SSA IR，有兴趣可以看luajit官方wiki），比较坑的是可供32位版本和64位版本执行的bytecode还不一样，这样才有了著名的2.0.x版本在ios加密不能的坑。 

‌

**2.jit模式一定更快？不一定！**

‌ios不能用jit，那么安卓下应该就可以飞起来用了吧？用脚本语言获得飞一般的性能，让我大红米也能对杠iphone！你开心的太早了。并不是安卓不能开启jit，而是jit的行为极其复杂，对平台高度依赖，导致它在以arm为主的安卓平台下，未必能发挥出在pc上的威力，要知道luajit最初只是考虑pc平台的。首先我们要知道，jit到底怎么运作的。luajit使用了一个很特殊的机制（也是其大坑），叫做trace compiler的方式，来将代码进行jit编译的。什么意思呢？它不是简单的像c++编译器那样直接把整套代码翻译成机器码就完事了，因为这么做有两个问题：1.编译时间长，这点比较好理解；2.更关键的是，作为动态语言，难以优化，例如对于一个function foo(a)，这个a到底是什么类型，并不知道，对这个a的任何操作，都要检查类型，然后根据类型做相应处理，哪怕就是一个简单的a+b都必须这样（a和b完全有可能是两个表，实现的__add元方法），实际上跟interpreter模式就没什么区别了，根本起不到高效运行的作用；3.很多动态类型无法提前知道类型信息，也就很难做链接（知道某个function的地址、知道某个成员变量的地址）那怎么办呢？这个解决方案可以另写一篇文章了。这里只是简单说一下luajit采用的trace compiler方案：首先所有的lua都会被编译成bytecode，在interpreter模式下执行，当interpreter发现某段代码经常被执行，比如for循环代码（是的，大部分性能瓶颈其实都跟循环有关），那么luajit会开启一个记录模式，记录这段代码实际运行每一步的细节（比如里头的变量是什么类型，猜测是数值还是table）。有了这些信息，luajit就可以做优化了：如果a+b发现就是两个数字相加，那就可以优化成数值相加；如果a.xxx就是访问a下面某个固定的字段，那就可以优化成固定的内存访问，不用再走表查询。最后就可以将这段经常执行的代码jit化。这里可以看到，第一，interpreter模式是必须的，无论平台是否允许jit，都必须先使用interpreter执行；第二，并非所有代码都会jit执行，仅仅是部分代码会这样，并且是运行过程中决定的。

[用好lua+unity，让性能飞起来——luajit集成篇/平台相关篇 - UDD_William - 博客园](https://www.cnblogs.com/zwywilliam/p/5999980.html)

## PCRE

编译OpenResty时需要修改PCRE编译选项，使其支持JIT，这样可以提升LuaJIT的效率。

‌另外官方Lua是支持正则的，但是正则的语法与PCRE是完全不同的，使用在OpenResty开发中 建议使用nginx.re 来做PCRE正则，不要使用Lua的string库的正则‌

## OpenSSL

编译OpenSSL时，需要打上特定的布丁。不然就会在处理 SSL session 时，带来功能上的缺失，比如像ngx.sleep这类会导致 yield 的 Lua API 就没法使用。

‌

## Zlib

修改了编译参数