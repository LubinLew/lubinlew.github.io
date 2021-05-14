# Kafka 使用例子

## 消费者

```python
#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from kafka import KafkaConsumer

bootstrap_servers_list="10.130.12.13:9092"
consumer = KafkaConsumer(bootstrap_servers=bootstrap_servers_list, client_id='test', group_id='test')
consumer.subscribe(pattern='^.+$')

# 循环处理数据, 如果没有数据则挂起等待新数据
for msg in consumer:
    print(msg.value.decode())
```

## 生产者

```python
#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import json
from kafka import KafkaProducer

topic_name = 'test'
bootstrap_servers_list="10.130.12.13:9092"

### producer.send only accept BYTE type

######### send string
producer = KafkaProducer(bootstrap_servers=bootstrap_servers_list)
producer.send(topic_name, "string-message".encode(encoding="utf-8"))
producer.flush()
producer.close()

######### send json
producer = KafkaProducer(bootstrap_servers=bootstrap_servers_list, value_serializer=lambda m:json.dumps(m).encode('utf-8'))
json_object = {}
json_object['id'] = 1
json_object['name'] = 'john'
producer.send(topic_name, json_object)
producer.flush()
producer.close()
```