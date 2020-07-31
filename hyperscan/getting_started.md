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

> 编译器 Intel C++ Compiler >= v15 也可以, 但是过于小众
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

在Linux下查看CPU支持的特性可以使用下面的命令:

```bash
cat /proc/cpuinfo | grep flags | uniq
```

These can be determined at library compile time, see [Target Architecture](http://intel.github.io/hyperscan/dev-reference/getting_started.html#target-arch).

### 软件依赖

默认情况下, 编译 Hyperscan 需要以下库, 如果需要编译 `libchimera` 则需要更多的库, 更高的cmake版本, 详细信息见 chimera 章节.

| Dependency                                      | Version  |     | Notes                                                 |
| ----------------------------------------------- | -------- | --- | ----------------------------------------------------- |
| [CMake](http://www.cmake.org/)                  | >=2.8.11 | 必须  | `yum install cmake` <br>`cmake --version` CentOS7默认满足 |
| [Ragel](http://www.colm.net/open-source/ragel/) | 6.9      | 必须  | `yum install ragel`  (下载的是7.0版本)                      |
| [Python](http://www.python.org/)                | 2.7      | 必须  | `python -V` CentOS7默认带python2.7                       |
| [Boost](http://boost.org/)                      | >=1.57   | 必须  | **只需要头文件**, CentOS7官方包只有1.53版本, 所以需要手动下载源码            |
| [pcap](http://tcpdump.org)                      | >=0.8    | 可选  | `yum install libpcap-devel` <br>只是用来编译示例代码            |

#### 关于 Boost Headers 的详细说明

如果你的系统上安装了符合要求版本的Boost库, 那么cmake会找到他们, 编译时就无需其他操作了; 由于Hyperscan只需要Boost库的头文件,所以不需要编译Boost库.如果你的系统上没有安装Boost库, 那么只需要下载Boost源码即可. 有3种方法指定Boost Headers 路径,

- 第一种是利用Cmake `BOOST_ROOT` 变量指定Boost源码路径. 

- 第二种种是拷贝boost的头文件路径到`<hyperscan-source-path>/include/boost`的文件夹中

- 第三种是创建一个连接到 `<hyperscan-source-path>/include/boost` 例如: `ln -s boost_1_59_0/boost /include/boost`

最好选择第一种, 简单方便.

## 开始编译

hyperscan库如果带debug信息, 库文件大小接近170MB, 去掉debug信息只有8MB左右, 所以如何没有特别需求不要生成带debug信息的库.

### 下载 Hyperscan 代码

去github的页面下载指定版本的代码 https://github.com/intel/hyperscan/releases

```bash
wget https://github.com/intel/hyperscan/archive/v5.3.0.tar.gz
tar xf v5.3.0.tar.gz
```

1. 配置 Hyperscan 编译选项
   
   Ensure that you have the correct [dependencies](http://intel.github.io/hyperscan/dev-reference/getting_started.html#software) present,
   and then:
   
   ```bash
   cd <where-you-want-to-build-hyperscan>
   mkdir <build-dir>
   cd <build-dir>
   cmake [-G <generator>] [options] <hyperscan-source-path>
   ```
   
   Known working generators:
   
   - `Unix Makefiles` — make-compatible makefiles (default on Linux/FreeBSD/Mac OS X)
   
   - `Ninja` — [Ninja](http://martine.github.io/ninja/) build files.
   
   - `Visual Studio 15 2017` — Visual Studio projects
   
   Generators that might work include:
- `Xcode` — OS X Xcode projects.
3. 编译 Hyperscan
   
   Depending on the generator used:
   
   - `cmake --build .` — will build everything
   
   - `make -j<jobs>` — use makefiles in parallel
   
   - `ninja` — use Ninja build
   
   - `MsBuild.exe` — use Visual Studio MsBuild
   
   - etc.

4. Check Hyperscan
   
   Run the Hyperscan unit tests:
   
   bin/unit-hyperscan

### CMake 选项配置

When CMake is invoked, it generates build files using the given options.
Options are passed to CMake in the form `-D<variable name>=<value>`.
Common options for CMake include:

| Variable                | Description                                                                                                                                                  | Example                           |
| ----------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------ | --------------------------------- |
| CMAKE_C_COMPILER        | 指定C编译器. <br>Default is /usr/bin/cc.                                                                                                                          | -DCMAKE_C_COMPILER=/usr/bin/clang |
| CMAKE_CXX_COMPILER      | 指定C++编译器. <br>Default is /usr/bin/c++.                                                                                                                       |                                   |
| CMAKE_INSTALL_PREFIX    | Install directory for `install` target                                                                                                                       |                                   |
| CMAKE_BUILD_TYPE        | 指定编译成果类型.<br>Valid options are Debug, Release, RelWithDebInfo,<br>and MinSizeRel. Default is RelWithDebInfo.                                                 | -DCMAKE_BUILD_TYPE=MinSizeRel     |
| BUILD_SHARED_LIBS       | 这个选线默认是off,只会生成静态库, 将这个选择赋值为on 就可以生成动态库,而不生成静态库                                                                                                              | -DBUILD_SHARED_LIBS=on            |
| BUILD_STATIC_AND_SHARED | Build both static and shared Hyperscan libs.<br>Default off.                                                                                                 | -DBUILD_STATIC_AND_SHARED=on      |
| BOOST_ROOT              | Boost源码路径                                                                                                                                                    | -DBOOST_ROOT=/path/boost_1_73_0   |
| DEBUG_OUTPUT            | Enable very verbose debug output. Default off.                                                                                                               | -DDEBUG_OUTPUT=on                 |
| FAT_RUNTIME             | Build the [fat runtime](http://intel.github.io/hyperscan/dev-reference/getting_started.html#fat-runtime). Default<br>true on Linux, not available elsewhere. |                                   |

For example, to generate a `Debug` build:

cd <build-dir>
cmake -DCMAKE_BUILD_TYPE=Debug <hyperscan-source-path>

#### Build Type

CMake determines a number of features for a build based on the Build Type.
Hyperscan defaults to `RelWithDebInfo`, i.e. “release with debugging
information”. This is a performance optimized build without runtime assertions
but with debug symbols enabled.

The other types of builds are:

> - `Release`: as above, but without debug symbols
> 
> - `MinSizeRel`: a stripped release build
> 
> - `Debug`: used when developing Hyperscan. Includes runtime assertions
>   (which has a large impact on runtime performance), and will also enable
>   some other build features like building internal unit
>   tests.

#### Target Architecture

Unless using the [fat runtime](http://intel.github.io/hyperscan/dev-reference/getting_started.html#fat-runtime), by default Hyperscan will be
compiled to target the instruction set of the processor of the machine that
being used for compilation. This is done via the use of `-march=native`. The
result of this means that a library built on one machine may not work on a
different machine if they differ in supported instruction subsets.

To override the use of `-march=native`, set appropriate flags for the
compiler in `CFLAGS` and `CXXFLAGS` environment variables before invoking
CMake, or `CMAKE_C_FLAGS` and `CMAKE_CXX_FLAGS` on the CMake command line. For
example, to set the instruction subsets up to `SSE4.2` using GCC 4.8:

cmake -DCMAKE_C_FLAGS="-march=corei7" \
  -DCMAKE_CXX_FLAGS="-march=corei7" <hyperscan-source-path>

For more information, refer to [Instruction Set Specialization](http://intel.github.io/hyperscan/dev-reference/compilation.html#instr-specialization).

#### Fat Runtime

A feature introduced in Hyperscan v4.4 is the ability for the Hyperscan
library to dispatch the most appropriate runtime code for the host processor.
This feature is called the “fat runtime”, as a single Hyperscan library
contains multiple copies of the runtime code for different instruction sets.

Note

The fat runtime feature is only available on Linux. Release builds of
Hyperscan will default to having the fat runtime enabled where supported.

When building the library with the fat runtime, the Hyperscan runtime code
will be compiled multiple times for these different instruction sets, and
these compiled objects are combined into one library. There are no changes to
how user applications are built against this library.

When applications are executed, the correct version of the runtime is selected
for the machine that it is running on. This is done using a `CPUID` check
for the presence of the instruction set, and then an indirect function is
resolved so that the right version of each API function is used. There is no
impact on function call performance, as this check and resolution is performed
by the ELF loader once when the binary is loaded.

If the Hyperscan library is used on x86 systems without `SSSE3`, the runtime
API functions will resolve to functions that return [`HS_ARCH_ERROR`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.HS_ARCH_ERROR "HS_ARCH_ERROR") instead of potentially executing illegal instructions. The API function [`hs_valid_platform()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_valid_platform "hs_valid_platform") can be used by application writers to determine if
the current platform is supported by Hyperscan.

As of this release, the variants of the runtime that are built, and the CPU
capability that is required, are the following:

| Variant | CPU Feature Flag(s) Required | gcc arch flag           |
| ------- | ---------------------------- | ----------------------- |
| Core 2  | `SSSE3`                      | `-march=core2`          |
| Core i7 | `SSE4_2` and `POPCNT`        | `-march=corei7`         |
| AVX 2   | `AVX2`                       | `-march=core-avx2`      |
| AVX 512 | `AVX512BW` (see note below)  | `-march=skylake-avx512` |

Note

Hyperscan v4.5 adds support for AVX-512 instructions - in particular the `AVX-512BW` instruction set that was introduced on Intel “Skylake” Xeon
processors - however the AVX-512 runtime variant is **not** enabled by
default in fat runtime builds as not all toolchains support AVX-512
instruction sets. To build an AVX-512 runtime, the CMake variable `BUILD_AVX512` must be enabled manually during configuration. For
example:

cmake -DBUILD_AVX512=on <...>

As the fat runtime requires compiler, libc, and binutils support, at this time
it will only be enabled for Linux builds where the compiler supports the [indirect function “ifunc” function attribute](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-indirect-functions-3321).

This attribute should be available on all supported versions of GCC, and
recent versions of Clang and ICC. There is currently no operating system
support for this feature on non-Linux systems.
