# 

# 什么是大小端?

‌

例如一个整形变量 `int var = 0x12345678;` 他的值在内存中是怎样存储的？

‌小端就是指字节序在内存中是如何存储的。大端指的就是把字节序的尾端（0x78）放在高内存地址，而小端指的就是把字节序的尾端（0x78）放在低内存地址**。**

‌

# 何时使用大小端？

‌大小端 **由CPU决定**，与编译器、操作系统这些没有直接关系。

‌我们常用的x86架构(Inter、AMD)的CPU都是小端的；

‌PowerPC架构(IBM、Motorola)的CPU使用的是大端；

‌ARM架构的CPU默认使用小段，但是可以通过配置改为大端。

‌

# 如何判断大小端

```c
int isLittleEndian(void)
{
    union w {
        int  a;
        char b;
    }c;

    c.a = 1;
    return (c.b == 1);
}
```

‌

# 大小端的优缺点

‌

# 大小端转换

‌当使用大端的机器和使用小端的机器进行数据交互时，就需要大小端转化。

‌网络字节序是大端的，主机字节序是不确定的，所以为了撇开主机字节序的干扰编程，Linux提供了一系列 主机字节序 和 网络字节序 转换的函数。

| 函数名称                                            | 说明          |
| ----------------------------------------------- | ----------- |
| uint32_t `htonl`(uint32_t hostlong)             | POSIX       |
| uint16_t `htons`(uint16_t hostshort)            | POSIX       |
| uint32_t `ntohl`(uint32_t netlong)              | POSIX       |
| uint16_t `ntohs`(uint16_t netshort)             | POSIX       |
| uint16_t htobe16(uint16_t host_16bits)          | _BSD_SOURCE |
| uint16_t htole16(uint16_t host_16bits)          | _BSD_SOURCE |
| uint16_t be16toh(uint16_t big_endian_16bits)    | _BSD_SOURCE |
| uint16_t le16toh(uint16_t little_endian_16bits) | _BSD_SOURCE |
| uint32_t htobe32(uint32_t host_32bits)          | _BSD_SOURCE |
| uint32_t be32toh(uint32_t big_endian_32bits)    | _BSD_SOURCE |
| uint32_t le32toh(uint32_t little_endian_32bits) | _BSD_SOURCE |
| uint64_t htobe64(uint64_t host_64bits)          | _BSD_SOURCE |
| uint64_t htole64(uint64_t host_64bits)          | _BSD_SOURCE |
| uint64_t be64toh(uint64_t big_endian_64bits)    | _BSD_SOURCE |
| uint64_t le64toh(uint64_t little_endian_64bits) | _BSD_SOURCE |
