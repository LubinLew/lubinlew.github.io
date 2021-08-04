# Save Wizard Custom Quick Code Formats


大多数的 PS4 存档都是使用小端(Little Endian)方式存储的, 
这里的大端和小端都是针对2个和4个字节来说的，比如说 999 使用十六进制表示为 `0x03E7`,
`03E7` 这种符合人类思考习惯的写入是大端法, 小端写法是 `0xE703`, 即将高低两个字节调换的位置。
目前我们使用的 Intel/AMD/ARM 等CPU都是使用小端的，当然ARM是可以配置大小端的。
同理4个字节的大端数据如 `0x12345678` 转换为小端就是 `0x78563412`。

When a Pointer is set using Codes 8, 9, B, or C, the following write codes 
with the Pointer Offset Type will have the Address in the code offset forwards from the Pointer Address.

PS4 Save Wizard 和 Game Genie 从来未对外公开发布这些作弊码的格式，
下面总结的格式都是在过去多年内被发现总结的，
但是并不是所有的修改器都支持这些格式，但都支持大部分格式。

----

### 标准写入

所谓的写入都是覆盖原地址上的数据, 即覆盖写入。

```txt
0TXXXXXX 000000YY  - 写入 1 个字节
1TXXXXXX 0000YYYY  - 写入 2 个字节 (Auto Swaps, 详见示例3)
2TXXXXXX YYYYYYYY  - 写入 4 个字节 (Auto Swaps, 详见示例4)

T = 偏移类型(Offset Type)
​    0 : 后面的 XXXXXX 表示的是绝对地址(从文件开头的偏移字节数)
    8 : 后面的 XXXXXX 从指针(上面其他语句产生的结果,后面会讲)偏移的字节数
X = 绝对地址 或者 偏移字节数
Y = 需要写入的字节内容

示例:

00000456 00000063 - 将地址为 0x0456 的位置写入字节 63
08000032 00000063 - 将指针偏移地址向后偏移为 0x32(即十进制50) 个字节的位置写入字节 63
10000456 0000E703 - 将地址从 0x0456 开始的位置写入2个字节 E703(大端为03E7,十进制为999)
20000456 FFE0F505 - 将地址从 0x0456 开始的位置写入4个字节 FFE0F505(大端为5F5E0FF,十进制为99999999)
```

### 增加/减少

```txt
3BYYYYYY XXXXXXXX

B = Byte Value & Offset Type
    0 = Add 1 Byte (000000XX)
    1 = Add 2 Bytes (0000XXXX)
    2 = Add 4 Bytes
    3 = Add 8 Bytes
    4 = Sub 1 Byte (000000XX)
    5 = Sub 2 Bytes (0000XXXX)
    6 = Sub 4 Bytes
    7 = Sub 8 Bytes
    8 = Offset from Pointer; Add 1 Byte (000000XX)
    9 = Offset from Pointer; Add 2 Bytes (0000XXXX)
    A = Offset from Pointer; Add 4 Bytes
    B = Offset from Pointer; Add 8 Bytes
    C = Offset from Pointer; Sub 1 Byte (000000XX)
    D = Offset from Pointer; Sub 2 Bytes (0000XXXX)
    E = Offset from Pointer; Sub 4 Bytes
    F = Offset from Pointer; Sub 8 Bytes
Y = Address
X = Bytes to Add/Sub

Example:

31003E3D 0000112A

31003E3D 0000112A - Add 2 Bytes Worth
31003E3D 0000112A - Add to this Address
31003E3D 0000112A - Add These Bytes

Before:

After:

```

### 向后字节搜索(设置指针)

```txt
8TCCXXXX YYYYYYYY
*Other Code Here, Use Specific Offset Type*

T = 偏移类型(Offset Type)
​    0 : 后面的 XXXXXX 表示的是绝对地址(从文件开头的偏移字节数)
    8 : 后面的 XXXXXX 从指针(上面其他语句产生的结果,后面会讲)偏移的字节数

C = Amount of Times to Find until Pointer Set
X = 搜索的字节数(小端, 即安照文件中字节顺序)
Y = 搜索的字节内容, 可以写多行, 最后一行不足16个字节时补 0

Example 1:
80010004 12345678 - 从文件开头搜索4个字节 "12345678", 并设置指针(指针的位置为12所在的字节) 
28000010 7FFFFFFF - 从指针的位置偏移 16(0x10)个字节, 写入四个字节 7FFFFFFF

```


### Reference

https://playersquared.com/forums/81-save-wizard-quick-codes/3012-save-wizard-custom-quick-code-formats.html



