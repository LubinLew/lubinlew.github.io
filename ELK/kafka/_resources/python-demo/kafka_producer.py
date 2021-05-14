#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import json
from kafka import KafkaProducer

topic_name = 'test'
bootstrap_servers_list="10.130.12.13:9092"


### producer.send only accept BYTE type

# send string
producer = KafkaProducer(bootstrap_servers=bootstrap_servers_list)
producer.send(topic_name, "string-message".encode(encoding="utf-8"))
producer.flush()
producer.close()

#send json
producer = KafkaProducer(bootstrap_servers=bootstrap_servers_list, value_serializer=lambda m:json.dumps(m).encode('utf-8'))
json_object = {}
json_object['id'] = 1
json_object['name'] = 'john'
producer.send(topic_name, json_object)
producer.flush()
producer.close()
