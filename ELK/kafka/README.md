# Kafka

> [官网](http://kafka.apache.org/)
> 
> [中文文档](http://kafka.apachecn.org/documentation.html) (很久未更新了)
> 
> [Kafka（一）Kafka的简介与架构 - Frankdeng](https://www.cnblogs.com/frankdeng/p/9310684.html)



Apache Kafka® 是 一个分布式流处理平台。



**流处理平台有以下三种特性**

1. 可以让你发布和订阅流式的记录。这一方面与消息队列或者企业消息系统类似。

2. 可以储存流式的记录，并且有较好的容错性。

3. 可以在流式记录产生时就进行处理。




**Kafka适用于两大类别的应用**

1. 构造实时流数据管道，它可以在系统或应用之间可靠地获取数据 (相当于消息队列)。

2. 构建实时流式应用程序，对这些流数据进行转换或者影响 (就是流处理)。



**基本概念**

Kafka作为一个集群，运行在一台或者多台服务器上.Kafka对消息保存时根据Topic进行归类，
发送消息者称为Producer，消息接受者称为Consumer，
此外kafka集群有多个kafka实例组成，每个实例(server)称为broker。
无论是kafka集群，还是consumer都依赖于zookeeper集群保存一些meta信息，来保证系统可用性。





**名词列表**

| 名词             | 释义   | 说明                                                                                                                                                                                    |
| -------------- | ---- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| consumer       | 消费者  | 消息生产者，就是向kafka broker发消息的客户端                                                                                                                                                          |
| producer       | 生产者  | 消息消费者，向kafka broker取消息的客户端                                                                                                                                                            |
| consumer group | 消费者组 | 这是kafka用来实现一个topic消息的广播（发给所有的consumer）和单播（发给任意一个consumer）的手段                                                                                                                          |
| broker         | 经纪人  | 一台kafka服务器就是一个broker。一个集群由多个broker组成                                                                                                                                                  |
| topic          | 主题   | 一个队列,类似数据库的table但是是有序集合                                                                                                                                                               |
| partition      | 分区   | 为了实现扩展性，一个非常大的topic可以分布到多个broker上，一个topic可以分为多个partition，每个partition是一个有序的队列。partition中的每条消息都会被分配一个有序的id（offset）。kafka只保证按一个partition中的顺序将消息发给consumer，不保证一个topic的整体（多个partition间）的顺序 |
| offset         | 偏移量  | kafka的存储文件都是按照offset.kafka来命名，用offset做名字的好处是方便查找。例如你想找位于2049的位置，只要找到2048.kafka的文件即可。当然the first offset就是00000000000.kafka                                                             |
| replication    | 副本   | 在不同broker上的冗余                                                                                                                                                                         |



## 资料

[[尚硅谷]Kafka-5小时从入门到精通](https://www.bilibili.com/video/BV1Xt411g7Fi?p=1)

> [讲义](https://github.com/LubinLew/lubinlew.github.io/tree/master/ELK/kafka/_resources)
