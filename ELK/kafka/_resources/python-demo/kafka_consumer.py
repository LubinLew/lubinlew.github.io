#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from kafka import KafkaConsumer


bootstrap_servers_list="10.130.12.13:9092"
consumer = KafkaConsumer(bootstrap_servers=bootstrap_servers_list, client_id='test', group_id='test')
consumer.subscribe(pattern='^.+$')

# 循环处理数据, 如果没有数据则挂起等待新数据
for msg in consumer:
    print(msg.value.decode())
