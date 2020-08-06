# 代码编译

声明:个人编译是在CentOS7下编译

**编译成果物说明**

Hyperscan 编译可以生成 3 种库 :

- `libhs`  主要的库, 包含 编译 和 扫描 两个部分

- `libhs_runtime`  只包含 扫描 部分, 适用于嵌入式等环境

- `libchimera`  完全支持PCRE语法, 编译比较复杂

可以通过宏配置,生成静态库和动态库

## 编译依赖

编译依赖于 操作系统、硬件 、软件 三大块。

### 操作系统

下面是官方测试过的操作系统和编译器列表:

| 操作系统          | 系统版本                | 编译器版本                        | 备注  |
| ------------- | ------------------- | ---------------------------- | --- |
| Ubuntu        | >= 14.04 LTS        | GCC >= v4.8.1, Clang >= v3.4 |     |
| RedHat/CentOS | >= 7                | GCC >= v4.8.1, Clang >= v3.4 |     |
| FreeBSD       | >= 10.0             | GCC >= v4.8.1, Clang >= v3.4 |     |
| Mac OS X      | >=10.8(XCode/Clang) | Clang >= v3.4                |     |
| Windows       | >= 8                | Visual C++ 2017 Build Tools  |     |

> 编译器 Intel C++ Compiler >= v15 也可以
> 
> Hyperscan 也许可以运行在其他平台上, 但是这是官方不能保证的.

### 硬件依赖

Hyperscan 可以运行于x86 CPU上, IA-32(IA-32 Architecture)架构和 IA-64架构(Intel® 64 Architecture). Hyperscan 之所以性能高, 是因为它利用了因特尔架构的一些特性. CPU最低也需要支持SSSE3(Supplemental Streaming SIMD Extensions 3) 特性, 几乎所有的现代x86 CPU都支持SSSE3特性. 另外, Hyperscan 还可以利用一下特性, 提高性能:

> - Intel Streaming SIMD Extensions 4.2 (SSE4.2)
> 
> - the POPCNT instruction
> 
> - Bit Manipulation Instructions (BMI, BMI2)
> 
> - Intel Advanced Vector Extensions 2 (Intel AVX2)

这些指令都是用于提升CPU性能。在Linux下查看CPU支持的特性可以使用下面的命令:

```bash
cat /proc/cpuinfo | grep flags | uniq
```

### 软件依赖

默认情况下, 编译 Hyperscan 需要以下库, 如果需要编译 `libchimera` 则需要更多的库, 更高的cmake版本, 详细信息见 chimera 章节.

| 依赖软件                                            | 版本       | 是否必需 | 备注                                                    |
| ----------------------------------------------- | -------- | ---- | ----------------------------------------------------- |
| [CMake](http://www.cmake.org/)                  | >=2.8.11 | 必需   | `yum install cmake` <br>`cmake --version` CentOS7默认满足 |
| [Ragel](http://www.colm.net/open-source/ragel/) | 6.9      | 必需   | `yum install ragel`  (下载的是7.0版本)                      |
| [Python](http://www.python.org/)                | 2.7      | 必需   | `python -V` CentOS7默认带python2.7                       |
| [Boost](http://boost.org/)                      | >=1.57   | 必需   | **只需要头文件**, CentOS7官方包只有1.53版本, 所以需要手动下载源码            |
| [pcap](http://tcpdump.org)                      | >=0.8    | 可选   | `yum install libpcap-devel` <br>只是用来编译示例代码            |

#### 关于 Boost Headers 的详细说明

如果你的系统上安装了符合要求版本的Boost库, 那么cmake会找到他们, 编译时就无需其他操作了; 由于Hyperscan只需要Boost库的头文件,所以不需要编译Boost库.如果你的系统上没有安装Boost库, 那么只需要下载Boost源码即可. 有3种方法指定Boost Headers 路径,

- 第一种是利用Cmake `BOOST_ROOT` 变量指定Boost源码路径. 

- 第二种种是拷贝boost的头文件路径到`<hyperscan-source-path>/include/boost`的文件夹中

- 第三种是创建一个连接到 `<hyperscan-source-path>/include/boost` 例如: `ln -s boost_1_59_0/boost /include/boost`

最好选择第一种, 简单方便.

## 编译准备

这里主要是介绍编译 Hyperscan 的一些选项配置。

### 下载 Hyperscan 代码

去github的页面下载指定版本的代码 https://github.com/intel/hyperscan/releases

```bash
wget https://github.com/intel/hyperscan/archive/v5.3.0.tar.gz
tar xf v5.3.0.tar.gz
```

下面是Hyperscan 的编译模板

```bash
cd <where-you-want-to-build-hyperscan>
mkdir <build-dir>
cd <build-dir>
cmake [-G <generator>] [options] <hyperscan-source-path>
```

| generators            | 说明                                                   | 备注                                |
| --------------------- | ---------------------------------------------------- | --------------------------------- |
| Unix Makefiles        | makefile                                             | default on Linux/FreeBSD/Mac OS X |
| Ninja                 | [Ninja](http://martine.github.io/ninja/) build files |                                   |
| Visual Studio 15 2017 | Visual Studio projects                               |                                   |
| Xcode                 | OS X Xcode projects                                  | 可能正常工作                            |

### CMake 选项配置

当CMake执行时，会根据给定的选项生成构建文件，这些选项以  `-D<variable name>=<value>`这种形式声明， 具体的选项和例子如下：

| 选项名称                    | 选项释义                                                                                                                             | 例子                                |
| ----------------------- | -------------------------------------------------------------------------------------------------------------------------------- | --------------------------------- |
| CMAKE_C_COMPILER        | 指定C编译器. <br>Default is /usr/bin/cc.                                                                                              | -DCMAKE_C_COMPILER=/usr/bin/clang |
| CMAKE_CXX_COMPILER      | 指定C++编译器. <br>Default is /usr/bin/c++.                                                                                           |                                   |
| CMAKE_INSTALL_PREFIX    | Install directory for `install` target                                                                                           |                                   |
| CMAKE_BUILD_TYPE        | 指定编译成果类型.取值有以下4个:<br> `Debug`, <br>`Release`, `RelWithDebInfo`(默认值),<br>`MinSizeRel`.<br>                                        | -DCMAKE_BUILD_TYPE=MinSizeRel     |
| BUILD_SHARED_LIBS       | 这个选线默认是off,只会生成静态库, 将这个选择赋值为on 就可以生成动态库,而不生成静态库                                                                                  | -DBUILD_SHARED_LIBS=on            |
| BUILD_STATIC_AND_SHARED | 编译生成静态库和动态库<br>默认值是 off.                                                                                                         | -DBUILD_STATIC_AND_SHARED=on      |
| BOOST_ROOT              | Boost源码路径                                                                                                                        | -DBOOST_ROOT=/path/boost_1_73_0   |
| DEBUG_OUTPUT            | 允许输出详细的debug信息<br>默认值是 off.                                                                                                      | -DDEBUG_OUTPUT=on                 |
| FAT_RUNTIME             | Build the [fat runtime](http://intel.github.io/hyperscan/dev-reference/getting_started.html#fat-runtime).<br>该选项只在Linux下可用, 默认开启 |                                   |

编译举例:

```bash
## 编译 hyperscan v5.3.0 只生成动态库, 使用boost 1.73.0
cmake -DCMAKE_BUILD_TYPE=MinSizeRel   \
      -DBUILD_SHARED_LIBS=on          \
      -DBOOST_ROOT=/root/boost_1_73_0 \
   /path/to/hyperscan-5.3.0 
```

#### 编译类型(Build Type)

Hyperscan 编译类型主要分为`RelWithDebInfo`/`Release`/`MinSizeRel`和 `Debug`两个大类。Release 版本是性能优化的版本，里面没有运行时断言(这些断言很影响性能)。相反 Debug 版带有运行时断言，Debug版本用于开发 Hyperscan，内部还集成了UT测试。

默认编译类型是 `RelWithDebInfo`, 意思是Release版本带有debug信息。`Release`版本则是不带有debug信息，`MinSizeRel` 则是 strip(编译的可执行程序中含有多种symbols，debug是其中占比较大的,详细信息可以man strip查看)过后的 Release 版本，体积最小。hyperscan库如果带debug信息, 库文件大小接近170MB, 去掉debug信息只有8MB左右。

> 题外话：
> 
> 通常像嵌入式等开发，通常会生成带有debug信息的可执行程序，然后通过 `strip -s` 删除其中的符号缩小体积，这样就就有了2个版本的可执行程序，strip的版本用于发布，当有coredump发生时，可以根据问题发生的位置在有debug信息的版本中追查到指定行的代码(借助`objdump`等命令)。

#### 目标结构(Target Architecture)

如果不使用 fat runtime 特性, Hyperscan 会默认使用编译Hyperscan代码的那台机器的CPU指令集去编译目标。这是通过编译器标志位 `-march=native`来实现的。但是这样做的结果是导致在一台机器上编译的库可能无法在另一台拥有不同CPU指令集的机器上运行。为了消除 `-march=native`的影响，可以在`CFLAGS` 和`CXXFLAGS` 中设置正确的编译器标志来指定指令集，这可以通过在cmake的变量 `CMAKE_C_FLAGS` 和 `CMAKE_CXX_FLAGS` 来完成。 例如使用 GCC 4.8 设置 `SSE4.2` 指令集使用下面的方法:

```bash
cmake -DCMAKE_C_FLAGS="-march=corei7"   \
      -DCMAKE_CXX_FLAGS="-march=corei7" \
   /path/to/<hyperscan-source-path>
```

更多信息，请参考 [Instruction Set Specialization](http://intel.github.io/hyperscan/dev-reference/compilation.html#instr-specialization).

#### Fat Runtime

Hyperscan v4.4 版本引入一个新的特性，被称为 “fat runtime”, 即一个单独的 Hyperscan 库可以包含多种CPU指令集的运行时代码。这种特性是Linux独有的。当编译支持fat runtime的目标时，Hyperscan会为不同的指令集，进行多次的编译，并且多次的编译目标都会集成到一个库中。当应用程序执行时，Hyperscan会根据当前运行的CPU指令集自动选择适合的运行时代码。这是通过 `CPUID` 检查当前的CPU指令集, 然后一个间接函数就指向了正确版本的API，这对函数调用的性能（function call performance）没有多大影响，因为这是在二进制加载时，ELF loader时做的一次性工作。当 Hyperscan 库运行在没有 `SSSE3`的CPU上时，运行时API会返回 `HS_ARCH_ERROR` 错误。可以通过API `hs_valid_platform()` 验证Hyperscan是否支持当前的平台。

本次的版本（v5.3.0）支持的运行时种类如下表：

| 种类      | 必需的CPU特性                    | GCC标志                   | 指定Cmake变量         |
| ------- | --------------------------- | ----------------------- | ----------------- |
| Core 2  | `SSSE3`                     | `-march=core2`          |                   |
| Core i7 | `SSE4_2` and `POPCNT`       | `-march=corei7`         |                   |
| AVX 2   | `AVX2`                      | `-march=core-avx2`      |                   |
| AVX 512 | `AVX512BW` (see note below) | `-march=skylake-avx512` | -DBUILD_AVX512=on |

Hyperscan v4.5 增加了 AVX-512 指令的支持。这个指令集成在 Intel “Skylake” Xeon 处理器上。默认情况下， AVX-512 指令没有包括在 fat runtime 中，这是因为不是所有的工具链（toolchains）都支持  AVX-512 指令。开启AVX-512指令需要定义Cmake变量 `-DBUILD_AVX512=on`。

由于 fat runtime 需要编译器、libc 、 binutils 的支持, 目前只有Linux的编译器支持 [indirect function “ifunc” function attribute](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-indirect-functions-3321). 这个特性在所有版本GCC上都支持，最新版本的Clang 和  ICC 也支持。其他非Linux操作系统上目前都不支持。

## 编译 Hyperscan

Depending on the generator used:

- `cmake --build .` — will build everything

- `make -j<jobs>` — use makefiles in parallel

- `ninja` — use Ninja build

- `MsBuild.exe` — use Visual Studio MsBuild

- etc.
