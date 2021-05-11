redis

[python3之redis - Py.qi - 博客园](https://www.cnblogs.com/zhangxinqi/p/8418545.html)

```python
import redis
#创建redis链接对象
r = redis.Redis(host='127.0.0.1',port=6379,decode_responses=True)
#存储键值对
r.set('site','www.qi.cn')
#获取值
print(r.get('site'))
#指定decode_responses为True，表示输出为字符串
red = redis.StrictRedis(host='127.0.0.1',port=6379,decode_responses=True)

#默认redis入库编码是utf-8，如果要修改的话，需要指明 charset 和 decode_responsers 为True
#test = redis.StrictRedis(host='localhost', port=6379, db=0, password=None, socket_timeout=None, connection_pool=None, charset='utf-8', errors='strict', decode_responses=False, unix_socket_path=None)
red.lpush('list1','mongdb','redis','mysql')
print(r.lrange('list1',0,-1))
print(r.llen('list1'))

#output:
# www.qi.cn
# ['mysql', 'redis', 'mongdb']
# 3
```

## 哈希

```python
import redis

pool = redis.ConnectionPool(host='10.130.12.66', port=6379, password='red@6379', db=1, decode_responses=True)
red = redis.Redis(connection_pool=pool)

hkey = 'redis_hkey'

# add data
red.hset(hkey, 'field1', 'value1')
red.hset(hkey, 'field2', 'value2')
red.hset(hkey, 'field3', 'value3')

# get data
data = red.hget(hkey, 'field1')
print("field1: " + data)
data = red.hget(hkey, 'field2')
print("field2: " + data)
data = red.hget(hkey, 'field3')
print("field3: " + data)

# delete
print('=== hdel ===')
red.hdel(hkey, 'field1')
data = red.hget(hkey, 'field1')
print("field1: %s" % type(data))
```
