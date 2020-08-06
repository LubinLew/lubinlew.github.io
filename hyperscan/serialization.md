# Serialization

对于一些应用, 使用Hyperscan编译后在扫描的方式并符合作者的设计. 一些用户希望

- 在其他主机上编译模式数据库;

- 将编译好的模式数据库持久化存储起来;

- 控制模式数据库所在的内存区域

由于Hyperscan 的模式数据库在内存中不是完全连续的, 其中包含了指针和特定的对齐需要.因此,模式数据库不能直接拷贝. 为了满足模式数据库能够拷贝迁移,  Hyperscan 提供了
serializing 和deserializing 模式数据库 用的API接口.

```c
hs_error_t HS_CDECL hs_serialize_database(
        const hs_database_t *db, 
        char **bytes,
        size_t *length);


hs_error_t HS_CDECL hs_deserialize_database(
        const char *bytes,
        const size_t length,
        hs_database_t **db);


hs_error_t HS_CDECL hs_deserialize_database_at(
        const char *bytes,
        const size_t length,
        hs_database_t *db);


hs_error_t HS_CDECL hs_serialized_database_size(
        const char *bytes,
        const size_t length,
        size_t *deserialized_size);

hs_error_t HS_CDECL hs_database_info(
        const hs_database_t *database,
        char **info);
```

1. [`hs_serialize_database()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_serialize_database "hs_serialize_database"): serializes a pattern database into a
   flat relocatable buffer of bytes.

2. [`hs_deserialize_database()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_deserialize_database "hs_deserialize_database"): reconstructs a newly allocated pattern
   database from the output of [`hs_serialize_database()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_serialize_database "hs_serialize_database").

3. [`hs_deserialize_database_at()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_deserialize_database_at "hs_deserialize_database_at"): reconstructs a pattern
   database at a given memory location from the output of [`hs_serialize_database()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_serialize_database "hs_serialize_database").

4. [`hs_serialized_database_size()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_serialized_database_size "hs_serialized_database_size"): given a serialized pattern database,
   returns the size of the memory block required by the database when
   deserialized.

5. [`hs_serialized_database_info()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_serialized_database_info "hs_serialized_database_info"): given a serialized pattern database,
   returns a string containing information about the database. This call is
   analogous to [`hs_database_info()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_database_info "hs_database_info").

Note

Hyperscan performs both version and platform compatibility checks
upon deserialization. The [`hs_deserialize_database()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_deserialize_database "hs_deserialize_database") and [`hs_deserialize_database_at()`](http://intel.github.io/hyperscan/dev-reference/api_files.html#c.hs_deserialize_database_at "hs_deserialize_database_at") functions will only permit the
deserialization of databases compiled with (a) the same version of Hyperscan
and (b) platform features supported by the current host platform. See [Instruction Set Specialization](http://intel.github.io/hyperscan/dev-reference/compilation.html#instr-specialization) for more information on platform specialization.

## The Runtime Library

Hyperscan主要的库 (`libhs`) 包含了编译(compiler)和运行(runtime/scanning)的部分. This means that in order to support the Hyperscan
compiler, which is written in C++, it requires C++ linkage and has a
dependency on the C++ standard library.

许多嵌入式应用只需要进行扫描的部分. 在这种情况下,模式编译通常在其他主机上进行, 嵌入式应用只是使用序列化的模式数据库.为了支持这种应用模式, `libhs_runtime`这个库就被发布了, 这个库不依赖C++标准库,这解决了应用对C++的依赖. and
provides all Hyperscan functions other that those used to compile databases.
