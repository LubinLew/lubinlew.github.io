# JSON数组排序



使用 `lambda`表达式能够简洁的进行json数组排序

```python
# 按照tag字段进行排序
json_array.sort(key = lambda x:x["tag"])

# 将 tag 字段的值 转换成 整形后排序
json_array.sort(key = lambda x:int(x["tag"]))
```




