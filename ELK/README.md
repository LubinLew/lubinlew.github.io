# ELK

> [ELK Stack：Elasticsearch、Logstash 和 Kibana | Elastic](https://www.elastic.co/cn/what-is/elk-stack)

**ELK 到底是什么呢？** “ELK”是三个开源项目的首字母缩写，这三个项目分别是：Elasticsearch、Logstash 和 Kibana。

Elasticsearch 是一个搜索和分析引擎。

Logstash 是服务器端数据处理管道，能够同时从多个来源采集数据，转换数据，然后将数据发送到诸如 Elasticsearch 等“存储库”中。

Kibana 则可以让用户在 Elasticsearch 中使用图形和图表对数据进行可视化。

Elastic Stack 是 ELK Stack 的更新换代产品。

---

**一切都起源于 Elasticsearch**

这个开源的分布式搜索引擎基于 JSON 开发而来，具有 RESTful 风格。它使用简单，可缩放规模，十分灵活，因此受到用户的热烈好评，而且如大家所知，围绕这一产品还形成了一家专门致力于搜索的公司。

**引入 Logstash 和 Kibana，产品更强大**

Elasticsearch 的核心是搜索引擎，所以用户开始将其用于日志用例，并希望能够轻松地对日志进行采集和可视化。有鉴于此，我们引入了强大的采集管道 Logstash 和灵活的可视化工具 Kibana。 

**我们向 ELK 中加入了 Beats**

“我只想对某个文件进行 tail 操作，”用户表示。我们用心倾听。在 2015 年，我们向 ELK Stack 中加入了一系列轻量型的单一功能数据采集器，并把它们叫做 Beats。

----

## Elastic 生态圈

![生态](elastic_stack.png)
