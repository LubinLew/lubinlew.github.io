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


