# 表的操作

## MySQL 创建数据表

创建MySQL数据表需要以下信息：

- 表名
- 表字段名
- 定义每个表字段

### 语法

以下为创建MySQL数据表的SQL通用语法：

```sql
CREATE TABLE table_name (column_name column_type);
```

反引号是为了区分 MySQL 关键字与普通字符而引入的符号，一般的，表名与字段名都使用反引号，以下例子中我们将在 RUNOOB 数据库中创建数据表runoob_tbl：

```sql
CREATE TABLE IF NOT EXISTS `runoob_tbl`( 
    `runoob_id` INT UNSIGNED AUTO_INCREMENT, 
    `runoob_title` VARCHAR(100) NOT NULL, 
    `runoob_author` VARCHAR(40) NOT NULL, 
    `submission_date` DATE, 
    PRIMARY KEY ( `runoob_id` ) 
)ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

实例解析：

- 如果你不想字段为 **NULL** 可以设置字段的属性为 **NOT NULL**， 在操作数据库时如果输入该字段的数据为**NULL**，就会报错,除非你有一个很特别的原因去使用 NULL 值，你应该总是让你的字段保持 NOT NULL。。
- AUTO_INCREMENT定义列为自增的属性，一般用于主键，数值会自动加1。
- PRIMARY KEY关键字用于定义列为主键。 您可以使用多列来定义主键，列间以逗号分隔。
- ENGINE 设置存储引擎，存储引擎有多种，CHARSET 设置编码。

### 注意事项

1、首先，我们要搞清楚空值 "" 和  NULL 的概念：

- 1）空值是不占用空间的
- 2）MySQL中的NULL其实是占用空间的

所谓的 NULL 就是什么都没有，连 \0 都没有，\0在字符串中是结束符，但是在物理内存是占空间的，等于一个字节，而 NULL 就是连这一个字节都没有。

2、其次，在数据库里是严格区分的，任何数跟 NULL 进行运算都是 NULL, 判断值是否等于 NULL，不能简单用 =，而要用 IS NULL关键字。

3、数据库的字段 col1 设为 NOT NULL, 仅仅说明该字段不能为 NULL, 也就是说只有在:

```sql
INSERT INTO table1(col1) VALUES(NULL);
```

这种情况下数据库会报错，而:

```sql
INSERT INTO table1(col1) VALUES('');
```

不会报错。

（如果字段是自增ID，第一句不会报错，这不能说明是可以为NULL,而是 数据库系统会根据ID设的缺省值填充，或者如果是自增字段就自动加一等缺省操作。）

4、含有空值的列很难进行查询优化，而且对表索引时不会存储 NULL 值的，所以如果索引的字段可以为 NULL，索引的效率会下降很多。因为它们使得索引、索引的统计信息以及比较运算更加复杂。你应该用 0、一个特殊的值或者一个空串代替空值。

5、联表查询的时候，例如 LEFT JOIN table2，若没有记录，则查找出的 table2 字段都是 null。假如 table2 有些字段本身可以是 null，那么除非把 table2 中 not null 的字段查出来，否则就难以区分到底是没有关联记录还是其他情况。

---

## MySQL 删除数据表

MySQL中删除数据表是非常容易操作的， 但是你再进行删除表操作时要非常小心，因为执行删除命令后所有数据都会消失。

### 语法

以下为删除MySQL数据表的通用语法：

```sql
DROP TABLE table_name;
```

---

## MySQL 向表中插入数据

MySQL 表中使用 `INSERT INTO` SQL语句来插入数据。

### 语法

以下为向MySQL数据表插入数据通用的 `INSERT INTO `SQL语法：

```sql
INSERT INTO table_name ( field1, field2,...fieldN ) 
VALUES ( value1, value2,...valueN );
```

如果数据是字符型，必须使用单引号或者双引号，如："value"。

----

## MySQL 修改表中的数据

如果我们需要修改或更新 MySQL 中的数据，我们可以使用 SQL `UPDATE` 命令来操作。

### 语法

以下是 `UPDATE` 命令修改 MySQL 数据表数据的通用 SQL 语法：

```sql
UPDATE table_name SET field1=new-value1, field2=new-value2 [WHERE Clause]
```

你可以同时更新一个或多个字段。

- 你可以在 WHERE 子句中指定任何条件。
- 你可以在一个单独表中同时更新数据。

当你需要更新数据表中指定行的数据时 WHERE 子句是非常有用的。

---

## MySQL 从表中删除数据

你可以使用 SQL 的 DELETE FROM 命令来删除 MySQL 数据表中的记录。

### 语法

以下是 SQL DELETE 语句从 MySQL 数据表中删除数据的通用语法：

DELETE FROM table_name [WHERE Clause]

- 如果没有指定 WHERE 子句，MySQL 表中的所有记录将被删除。
- 你可以在 WHERE 子句中指定任何条件
- 您可以在单个表中一次性删除记录。

当你想删除数据表中指定的记录时 WHERE 子句是非常有用的。
