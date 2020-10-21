# JSON数组排序

使用 `lambda`表达式能够简洁的进行json数组排序

```python
# 按照tag字段进行排序
json_array.sort(key = lambda x:x["tag"])

# 将 tag 字段的值 转换成 整形后排序
json_array.sort(key = lambda x:int(x["tag"]))
```

例子

```python
#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import json

json_str = '[ \
    {"id": "1005", "value": "1"}, \
    {"id": "3002", "value": "2"}, \
    {"id": "5120", "value": "3"}, \
    {"id": "4233", "value": "4"}, \
    {"id": "8000", "value": "5"}, \
    {"id": "2020", "value": "6"}  \
]'
json_array = json.loads(json_str)
json_array.sort(key = lambda x:int(x['id']))
new_json_str = json.dumps(json_array, ensure_ascii=False, indent=2, sort_keys=False, )
print(new_json_str)



''' 输出结果
[
  {
    "id": "1005",
    "value": "1"
  },
  {
    "id": "2020",
    "value": "6"
  },
  {
    "id": "3002",
    "value": "2"
  },
  {
    "id": "4233",
    "value": "4"
  },
  {
    "id": "5120",
    "value": "3"
  },
  {
    "id": "8000",
    "value": "5"
  }
]
'''
```
