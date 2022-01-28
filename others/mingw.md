# MinGW

## 简介

MinGW（Minimalist GNU for Windows），原名 mingw32，是一个免费的开源软件开发环境，用于创建 Microsoft Windows 应用程序。 MinGW 项目的开发与 2005 年至 2008 年创建了一个名为 Mingw-w64 的替代项目分支。

MinGW 包括 GNU 编译器集合 (GCC)、GNU Binutils for Windows（汇编器、链接器、存档管理器）、一组可自由分发的 Windows 特定头文件和静态导入库，这些库支持使用 Windows API、Windows GNU 项目的 GNU 调试器和其他实用程序的本机构建。MinGW 不依赖第三方 C 运行时动态链接库 (DLL) 文件，并且由于运行时库不是使用 GNU 通用公共许可证 (GPL) 分发的，因此无需将源代码与生成的程序一起分发 ，除非在程序的其他地方使用了 GPL 库。MinGW 可以在本机 Microsoft Windows 平台上运行，或在 Linux（或其他 Unix）上交叉编译。 尽管在 MinGW 下生成的程序是 32 位可执行文件，但它们可以在 32 位和 64 位版本的 Windows 中使用。

### 支持的语言

MinGW 支持 GCC 支持的大多数语言。其中包括 C、C++、Objective-C、Objective-C++、Fortran 和 Ada。使用 GCC 运行时库（用于 C++ 的 libstdc++、用于 Fortran 的 libgfortran 等）。MinGW 默认链接到 Windows 操作系统组件库 MSVCRT，这是 Visual C++ 6.0 版链接到的 C 库（最初的目标是 CRTDLL），它于 1998 年发布，因此不包括对 C99 功能的支持，甚至所有C89 的。虽然针对 MSVCRT 生成的程序不需要安装额外的运行时可再发行组件，但缺乏对 C99 的支持导致了移植问题，尤其是在涉及 printf 样式转换说明符的情况下。这些问题已通过实施 C99 兼容性库 libmingwex 得到部分缓解，但所需的大量工作远未完成，可能永远无法完全实现。 Mingw-w64 已经解决了这些问题，并提供了完全符合 POSIX 的 printf 功能。

### 链接兼容性

使用不同 C++ 编译器（如 MinGW 和 Visual Studio）生成的二进制文件（可执行文件或 DLL）通常与链接不兼容。 但是，编译后的 C 代码是链接兼容的。

### 组件

MinGW 项目维护和分发了许多不同的核心组件和补充包，例如 GCC 和 binutils，被翻译成等效的包。这些实用程序可以从 Windows 命令行使用或集成到 IDE 中。可以通过 mingw-get 使用命令行安装软件包。 MinGW 支持根据 `<name>.lib` 和 `<name>.dll` 约定命名的动态库，以及遵循 `lib<name>.a` 在 Unix 和类 Unix 系统上常见的命名约定的静态库。

此外，称为 MSYS（最小系统）的 MinGW 组件提供了轻量级类 Unix shell 环境的 Windows 端口，包括 rxvt 和足以使 autoconf 脚本运行的一系列 POSIX 工具，但它不提供 C 编译器或区分大小写的文件系统。

mingwPORTs 是用户贡献的对 MinGW 软件集合的补充。它们不是以预编译二进制包的形式提供这些“附加组件”，而是以交互式 Bourne shell 脚本的形式提供，引导最终用户完成自动下载和修补原始源代码的过程，然后构建和安装它。希望从 mingwPORT 构建任何应用程序的用户必须首先安装 MinGW 和 MSYS。

Windows 系统头文件和静态导入库的实现是在许可许可证下发布的，而 GNU port 是根据 GNU 通用公共许可证提供的。完整的 MSYS 包和单个 MinGW GNU 实用程序的二进制下载都可以从 MinGW 站点获得。

### 与 Cygwin 的比较

尽管 Cygwin 和 MinGW 都可用于将 Unix 软件移植到 Windows，但它们有不同的方法：Cygwin 旨在提供一个完整的 POSIX 层，包括所有主要 Unix 系统调用和库的完整实现。兼容性被认为比性能更重要。另一方面，MinGW 的优先级是简单性和性能。因此，它不提供某些无法使用 Windows API 轻松实现的 POSIX API，例如 fork()、mmap() 和 ioctl()。 使用本身已移植到 MinGW 的跨平台库编写的应用程序，例如 SDL、wxWidgets、Qt 或 GTK，通常在 MinGW 中编译就像在 Cygwin 中一样容易。

用 Cygwin 编写的 Windows 程序在必须与程序一起分发的 copyleft 兼容性 DLL 之上运行，以及程序的源代码。 MinGW 不需要兼容层，因为基于 MinGW 的程序是通过直接调用 Windows API 进行编译的。

MinGW 和 MSYS 的组合提供了一个小型、独立的环境，可以将其加载到可移动媒体上，而无需在注册表中留下条目或在计算机上留下文件。也可以在 POSIX 系统下使用 MinGW-GCC 交叉编译 Windows 应用程序。这意味着开发人员不需要安装带有 MSYS 的 Windows 来编译将在带有或不带有 Cygwin 的 Windows 上运行的软件。

---

## 安装

环境为 Ubuntu 20.04

```bash
apt install mingw-w64
------------------------------------------------------------
The following additional packages will be installed:
 binutils-common
 binutils-mingw-w64-i686
 binutils-mingw-w64-x86-64
 g++-mingw-w64
 g++-mingw-w64-i686
 g++-mingw-w64-x86-64
 gcc-mingw-w64
 gcc-mingw-w64-base
 gcc-mingw-w64-i686 
 gcc-mingw-w64-x86-64 
 libisl22 
 libmpc3 
 mingw-w64-common 
 mingw-w64-i686-dev 
 mingw-w64-x86-64-dev

Suggested packages:
  gcc-9-locales wine wine64


```

安装的命令(i686)

toolchain builds code for 32-bit Windows

```bash
i686-w64-mingw32-addr2line         i686-w64-mingw32-g++               i686-w64-mingw32-gcc-ranlib-posix  i686-w64-mingw32-ld
i686-w64-mingw32-ar                i686-w64-mingw32-gcc               i686-w64-mingw32-gcc-ranlib-win32  i686-w64-mingw32-ld.bfd
i686-w64-mingw32-as                i686-w64-mingw32-gcc-9             i686-w64-mingw32-gcc-win32         i686-w64-mingw32-nm
i686-w64-mingw32-c++               i686-w64-mingw32-gcc-9.3-posix     i686-w64-mingw32-gcov              i686-w64-mingw32-objcopy
i686-w64-mingw32-c++filt           i686-w64-mingw32-gcc-9.3-win32     i686-w64-mingw32-gcov-dump-posix   i686-w64-mingw32-objdump
i686-w64-mingw32-c++-posix         i686-w64-mingw32-gcc-ar            i686-w64-mingw32-gcov-dump-win32   i686-w64-mingw32-ranlib
i686-w64-mingw32-cpp               i686-w64-mingw32-gcc-ar-posix      i686-w64-mingw32-gcov-posix        i686-w64-mingw32-readelf
i686-w64-mingw32-cpp-posix         i686-w64-mingw32-gcc-ar-win32      i686-w64-mingw32-gcov-tool-posix   i686-w64-mingw32-size
i686-w64-mingw32-cpp-win32         i686-w64-mingw32-gcc-nm            i686-w64-mingw32-gcov-tool-win32   i686-w64-mingw32-strings
i686-w64-mingw32-c++-win32         i686-w64-mingw32-gcc-nm-posix      i686-w64-mingw32-gcov-win32        i686-w64-mingw32-strip
i686-w64-mingw32-dlltool           i686-w64-mingw32-gcc-nm-win32      i686-w64-mingw32-g++-posix         i686-w64-mingw32-windmc
i686-w64-mingw32-dllwrap           i686-w64-mingw32-gcc-posix         i686-w64-mingw32-gprof             i686-w64-mingw32-windres
i686-w64-mingw32-elfedit           i686-w64-mingw32-gcc-ranlib        i686-w64-mingw32-g++-win32         
```

安装的命令(x86_64)

toolchain builds code for 64-bit Windows

```bash
x86_64-w64-mingw32-elfedit           x86_64-w64-mingw32-gcc-ranlib        x86_64-w64-mingw32-g++-win32
x86_64-w64-mingw32-addr2line         x86_64-w64-mingw32-g++               x86_64-w64-mingw32-gcc-ranlib-posix  x86_64-w64-mingw32-ld
x86_64-w64-mingw32-ar                x86_64-w64-mingw32-gcc               x86_64-w64-mingw32-gcc-ranlib-win32  x86_64-w64-mingw32-ld.bfd
x86_64-w64-mingw32-as                x86_64-w64-mingw32-gcc-9             x86_64-w64-mingw32-gcc-win32         x86_64-w64-mingw32-nm
x86_64-w64-mingw32-c++               x86_64-w64-mingw32-gcc-9.3-posix     x86_64-w64-mingw32-gcov              x86_64-w64-mingw32-objcopy
x86_64-w64-mingw32-c++filt           x86_64-w64-mingw32-gcc-9.3-win32     x86_64-w64-mingw32-gcov-dump-posix   x86_64-w64-mingw32-objdump
x86_64-w64-mingw32-c++-posix         x86_64-w64-mingw32-gcc-ar            x86_64-w64-mingw32-gcov-dump-win32   x86_64-w64-mingw32-ranlib
x86_64-w64-mingw32-cpp               x86_64-w64-mingw32-gcc-ar-posix      x86_64-w64-mingw32-gcov-posix        x86_64-w64-mingw32-readelf
x86_64-w64-mingw32-cpp-posix         x86_64-w64-mingw32-gcc-ar-win32      x86_64-w64-mingw32-gcov-tool-posix   x86_64-w64-mingw32-size
x86_64-w64-mingw32-cpp-win32         x86_64-w64-mingw32-gcc-nm            x86_64-w64-mingw32-gcov-tool-win32   x86_64-w64-mingw32-strings
x86_64-w64-mingw32-c++-win32         x86_64-w64-mingw32-gcc-nm-posix      x86_64-w64-mingw32-gcov-win32        x86_64-w64-mingw32-strip
x86_64-w64-mingw32-dlltool           x86_64-w64-mingw32-gcc-nm-win32      x86_64-w64-mingw32-g++-posix         x86_64-w64-mingw32-windmc
x86_64-w64-mingw32-dllwrap           x86_64-w64-mingw32-gcc-posix         x86_64-w64-mingw32-gprof             x86_64-w64-mingw32-windres
```

## 编译的exe增加图标的方法

https://stackoverflow.com/questions/708238/how-do-i-add-an-icon-to-a-mingw-gcc-compiled-executable

