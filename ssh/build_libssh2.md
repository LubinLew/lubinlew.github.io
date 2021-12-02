# libssh2 的编译

libssh2 需要使用 Cmake(>=2.8) 或者autoconf 进行编译。

下面是编译时可以在 cmake 命令行中定义的 选项。

> [libssh2/INSTALL_CMAKE](https://github.com/libssh2/libssh2/blob/master/docs/INSTALL_CMAKE.md)

| 选项                      | 说明                                                                                                                                        | 默认值    |
| ----------------------- | ----------------------------------------------------------------------------------------------------------------------------------------- | ------ |
| LINT                    | 编译时是否运行代码检查工具(linter)                                                                                                                     | ON     |
| BUILD_SHARED_LIBS       | 编译静态库还是动态库, `OFF` 编译静态库, `ON` 编译动态库                                                                                                       | OFF    |
| CRYPTO_BACKEND          | 选择密码学库,支持`OpenSSL`、`Libgcrypt`、`WinCNG`、`mbedTLS`                                                                                         | Auto   |
| ENABLE_ZLIB_COMPRESSION | 是否支持负载数据使用`zlib`库压缩                                                                                                                       | OFF    |
| ENABLE_CRYPT_NONE       | 是否支持 "none" 密码组件(明文传输)                                                                                                                    | OFF    |
| ENABLE_MAC_NONE         | 是否放弃MAC(message authentication code)校验                                                                                                    | OFF    |
| ENABLE_GEX_NEW          | diffie-hellman-group-exchange-sha1 (dh-gex) 密钥交换方法最初定义了使用数据包类型 30 的交换协商，以基于单个目标值请求生成对。 后来为范围和目标值提供了 dh-gex 的改进。 默认情况下，libssh2 将使用较新的范围方法。 | ON     |
| ENABLE_DEBUG_LOGGING    | 是否允许输出debug日志                                                                                                                             | ON/OFF |
| CLEAR_MEMORY            | 在释放之前安全地将内存归零(如果后端支持)                                                                                                                     | ON     |

**密码学库**

| 库名                                                                         | 说明                                           | License            |
| -------------------------------------------------------------------------- | -------------------------------------------- | ------------------ |
| [OpenSSL](https://www.openssl.org/)                                        | 老牌的密码学库，基本全平台支持                              | Apache 2.0         |
| [Libgcrypt](https://gnupg.org/software/libgcrypt/index.html)               | 支持大部分 POSIX 系统和许多 pre-POSIX 系统 , 也支持 windows | LGPL               |
| [WinCNG](https://docs.microsoft.com/en-us/windows/win32/seccng/cng-portal) | 从 Windows Vista 开始， windows 官方的 native 密码学库  | -                  |
| [mbedTLS](https://tls.mbed.org/)                                           | 支持 Linux 和 windows                           | Apache 2.0/GPL 2.0 |

## windows cmake

Deps:

install Visual Studio
install cmake for windows

```bash
git clone https://github.com/libssh2/libssh2.git
cd libssh2
mkdir build
cd build
mkdir install
cmake -DCRYPTO_BACKEND=WinCNG -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=install ..
cmake --build .
cmake --build . --target install
```

## linux cmake

```bash
yum install cmake
yum install zlib-devel
yum install openssl-devel

git clone https://github.com/libssh2/libssh2.git
cd libssh2
mkdir build
cd build
mkdir install
cmake -DCRYPTO_BACKEND=OpenSSL -ENABLE_ZLIB_COMPRESSION=ON -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=install ..
cmake --build .
cmake --build . --target install
```

## linux autoconf

> [libssh2/INSTALL_AUTOTOOLS](https://github.com/libssh2/libssh2/blob/master/docs/INSTALL_AUTOTOOLS)

autoconf 的好处是可以配置各种依赖库的自定义路径, 比如 OpenSSL/Zlib 默认只有动态库, 你可以编译一个静态库用于链接。

```bash
yum install autoconf libtool

git clone https://github.com/libssh2/libssh2.git
cd libssh2

autoreconf -fi
```

**configure 配置项** 

这里只列出比较有用的部分选项, 详细选项见 `./configure --help`

| 选项                                                                                          | 说明                                                      |     |
| ------------------------------------------------------------------------------------------- | ------------------------------------------------------- | --- |
| --prefix=PREFIX                                                                             | 安装路径(绝对路径)                                              |     |
| [--build=BUILD](https://github.com/libssh2/libssh2/blob/master/docs/INSTALL_AUTOTOOLS#L156) | configure for building on BUILD [guessed]               |     |
| --host=HOST                                                                                 | cross-compile to build programs to run on HOST [BUILD]  |     |
| --enable-shared[=PKGS]                                                                      | 编译动态库 [default=yes]                                     |     |
| --enable-static[=PKGS]                                                                      | 编译静态库 [default=yes]                                     |     |
| --disable-examples-build                                                                    | 不编译例子程序(默认会编译)                                          |     |
| --with-crypto=auto\|openssl\|libgcrypt\|mbedtls\|wincng                                     | Select crypto backend (default: auto)                   |     |
| --with-libssl-prefix[=DIR]                                                                  | 搜索 libssl 的路径 `DIR/include` 和 `DIR/lib`                 |     |
| --without-libssl-prefix                                                                     | don't search for libssl in includedir and libdir        |     |
| --with-libgcrypt-prefix[=DIR]                                                               | search for libgcrypt in DIR/include and DIR/lib         |     |
| --without-libgcrypt-prefix                                                                  | don't search for libgcrypt in includedir and libdir     |     |
| --with-libmbedcrypto-prefix[=DIR]                                                           | search for libmbedcrypto in DIR/include and DIR/lib     |     |
| --without-libmbedcrypto-prefix                                                              | don't search for libmbedcrypto in includedir and libdir |     |
| --with-libcrypt32-prefix[=DIR]                                                              | search for libcrypt32 in DIR/include and DIR/lib        |     |
| --without-libcrypt32-prefix                                                                 | don't search for libcrypt32 in includedir and libdir    |     |
| --with-libbcrypt-prefix[=DIR]                                                               | search for libbcrypt in DIR/include and DIR/lib         |     |
| --without-libbcrypt-prefix                                                                  | don't search for libbcrypt in includedir and libdir     |     |
| --with-libz                                                                                 | Use libz for compression                                |     |
| --with-libz-prefix[=DIR]                                                                    | search for libz in DIR/include and DIR/lib              |     |
| --without-libz-prefix                                                                       | don't search for libz in includedir and libdir          |     |

```bash
./configure \
   --prefix=`pwd`/install \
    --enable-shared=yes \
    --enable-static=yes \
    --with-crypto=openssl \
    --with-libz

make

make install
```