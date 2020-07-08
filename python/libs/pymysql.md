# 资料

[Welcome to PyMySQL’s documentation! &mdash; PyMySQL 0.7.2 documentation](https://pymysql.readthedocs.io/en/latest/index.html)

[GitHub - PyMySQL/PyMySQL: Pure Python MySQL Client](https://github.com/PyMySQL/PyMySQL)



## 安装

```bash
pip3 install pymysql
```

## 基本使用

```python
# 导入pymysql模块
import pymysql
 
# 连接 MySQL 数据库,
# 例子: conn = pymysql.connect("10.130.12.66","root","123","test")
conn = pymysql.connect(
    host=“你的数据库地址”,
    user=“用户名”,
    password=“密码”,
    database=“数据库名”,
    charset=“utf8”)
 
# 得到一个可以执行SQL语句的光标对象
cursor = conn.cursor()  # 执行完毕返回的结果集默认以元组显示
# 得到一个可以执行SQL语句并且将结果作为字典返回的游标
#cursor = conn.cursor(cursor=pymysql.cursors.DictCursor)
 
# 定义要执行的SQL语句
sql = """
CREATE TABLE USER1 (
id INT auto_increment PRIMARY KEY ,
name CHAR(10) NOT NULL UNIQUE,
age TINYINT NOT NULL
)ENGINE=innodb DEFAULT CHARSET=utf8;
"""
 
# 执行SQL语句
cursor.execute(sql)
 
# 关闭光标对象
cursor.close()
 
# 关闭数据库连接
conn.close()
```
