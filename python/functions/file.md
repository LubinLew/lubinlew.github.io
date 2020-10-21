# 文件读写

使用with方式读写文件的好处是不用再做close文件操作。



## 读文件

```python
with open('/to/file/path', encoding="utf-8", mode="r") as in_f:
    content = in_f.read()
```

## 写文件

```python
with open('/to/file/path', encoding="utf-8", mode="w") as out_f:
    out_f.write('some strings')
```


