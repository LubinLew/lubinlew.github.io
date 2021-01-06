# paste 命令

> [Linux paste命令 | 菜鸟教程](https://www.runoob.com/linux/linux-comm-paste.html)
> 
> [paste命令详解 - 马昌伟 - 博客园](https://www.cnblogs.com/machangwei-8/p/9570921.html)
> 
> shell `info coreutils 'paste invocation'`

## NAME

`paste` - 合并多个文件的行

## SYNOPSIS

`paste [OPTION]... [FILE]...`

## DESCRIPTION

`OPTIONS` 只有2个选项 `-d` 和 `-s` ; 

 `FILE` 为文件列表, 多个文件用空白符分隔, `-` 或者没有文件表示将标准输入作为文件.

| 选项                        | 说明                                                                                       |
| ------------------------- | ---------------------------------------------------------------------------------------- |
| `-d`, `--delimiters=LIST` | 程序默认使用TAB分隔合并的内容, 使用该选项可以自定义分隔符号, 注意选项的值可以为一个列表, 表示使用列表中的字符作为分隔符, 详细见下面的[例子](#自定义分隔符列表). |
| `-s`, `--serial`          | 将一个文件的所有行合并成为一行, 行与行之间默认使用TAB分隔                                                          |

## EXAMPLE

下面是示例文件的内容展示

```bash
[user]$ cat num2.txt
1
2

[user]$ cat let3.txt
a
b
c
```

### 将两个文件竖向合并

按照列的方式, 将多个文件的 相同行拼接成一行.

```bash
[usr]$ paste num2.txt let3.txt
1       a
2       b
        c
```

### 将多个文件竖向合并

```bash
$ paste num2.txt let3.txt num2.txt
1       a      1
2       b      2
        c
```

### 将两个文件横向合并

将每个文件中的内容合并成一行, 然后多个文件合并

```bash
[user]$ paste -s num2.txt let3.txt
1       2
a       b       c
```

### 从标准输入混合

下面的命令中, `-` 代表从标准输入读取,  `<num2.txt` 表示将num2.txt的内容作为标准输入.

```bash
[user]$ paste - let3.txt - < num2.txt
1       a      2
        b
        c
```

### 合并连续行

`seq 4` 命令生成1、2、3、4 四个字符, 每个一行, 使用管道符`|`将其作为标准输入传递给`paste` 命令。`-d ' '` 表示使用空格进行分割.

```bash
[user]$ seq 4 | paste -d ' ' - -
1 2
3 4

[user]$ seq 4 | paste -d ' ' - - - -
1 2 3 4

[user]$ seq 4 | paste -d ' ' -s
1 2 3 4
```

### 自定义分隔符列表

依次使用列表中的分隔符进行分隔, 如果分隔的次数大于分隔符的个数, 那么循环使用分隔符列表

```bash
[user]$ paste -d ',.' num2.txt let3.txt num2.txt
1,a.1
2,b.2
,c.


[user]$ paste -d ',.' num2.txt let3.txt num2.txt let3.txt
1,a.1,a
2,b.2,b
,c.,c

[user]$ paste -d ',.' num2.txt let3.txt num2.txt let3.txt num2.txt
1,a.1,a.1
2,b.2,b.2
,c.,c.
```



### 实用例子 : TOP命令只监控nginx进程

`top` 命名可以用使用 `-p` 选项监控指定的进程(通过PID), 多个进程之间使用逗号分隔.

使用 `grep` 和 `awk` 命令筛选出所有nginx进程的PID, 然后使用 `paste` 将其合并成一行,并使用逗号分隔, 拼凑出 `top -p` 需要的格式。

```bash
#!/usr/bin/env bash

PIDARR=`ps -e|grep nginx|awk '{print $1}'|paste -d "," -s`
top -Hp ${PIDARR}
```




