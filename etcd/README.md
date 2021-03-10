# etcd使用文档

> [etcd/Documentation at release-3.4 · etcd-io/etcd · GitHub](https://github.com/etcd-io/etcd/tree/release-3.4/Documentation)
> 
> [还不了解 etcd？一文带你快速入门（万字长文）_aoho求索](https://blueskykong.blog.csdn.net/article/details/110251835)



2013 年 6 月，CoreOS 发起了 etcd 项目。etcd是一种分布式键值存储，是分布式系统中重要的基础组件。旨在可靠，快速地保持并提供对关键数据的访问。 

它通过分布式锁定，领导者选举(leader elections)和写障碍(write barriers)实现可靠的分布式协调。 etcd集群旨在实现高可用性以及永久性数据存储和检索。

使用 Go 语言实现，支持跨平台，拥有强大的社区，etcd 广泛应用在微服务架构和 Kubernates 集群中，不仅可以作为服务注册与发现，此外还有键值对存储、消息发布与订阅、分布式锁等场景。从业务系统 Web 到 Kubernetes 集群，都可以很方便地从 etcd 中读取、写入数据。

## 优点

- 简单：etcd 的安装简单，且为用户提供了 HTTP API，用户使用起来也很简单

- 存储：etcd 的基本功能，数据分层存储在文件目录中，类似于我们日常使用的文件系统

- Watch 机制：Watch 指定的键、前缀目录的更改，并对更改时间进行通知

- 安全通信：SSL 证书验证

- 高性能：etcd 单实例可以支持 2k/s 读操作，官方也有提供基准测试脚本

- 一致可靠：基于 Raft 共识算法，实现分布式系统数据的高可用性、一致性
  

## Developing with etcd

The easiest way to get started using etcd as a distributed key-value store is to [set up a local cluster][local_cluster].

- [Setting up local clusters][local_cluster]
- [Interacting with etcd][interacting]
- gRPC [etcd core][api_ref] and [etcd concurrency][api_concurrency_ref] API references
- [HTTP JSON API through the gRPC gateway][api_grpc_gateway]
- [gRPC naming and discovery][grpc_naming]
- [Client][namespace_client] and [proxy][namespace_proxy] namespacing
- [Embedding etcd][embed_etcd]
- [Experimental features and APIs][experimental]
- [System limits][system-limit]

## 集群操作

Administrators who need a fault-tolerant etcd cluster for either development or production should begin with a [cluster on multiple machines][clustering].

### Setting up etcd

- [Configuration flags][conf]
- [Multi-member cluster][clustering]
- [gRPC proxy][grpc_proxy]
- [L4 gateway][gateway]

### System configuration

- [Supported systems][supported_platforms]
- [Hardware recommendations][hardware]
- [Performance benchmarking][performance]
- [Tuning][tuning]

### Platform guides

- [Amazon Web Services][aws_platform]
- [Container Linux, systemd][container_linux_platform]
- [FreeBSD][freebsd_platform]
- [Docker container][container_docker]
- [rkt container][container_rkt]

### Security

- [TLS][security]
- [Role-based access control][authentication]

### Maintenance and troubleshooting

- [Frequently asked questions][faq]
- [Monitoring][monitoring]
- [Maintenance][maintenance]
- [Failure modes][failures]
- [Disaster recovery][recovery]
- [Upgrading][upgrading]

## Learning

To learn more about the concepts and internals behind etcd, read the following pages:

- [Why etcd?][why]
- [Understand data model][data_model]
- [Understand APIs][understand_apis]
- [Glossary][glossary]
- Design
  - [Auth subsystem][design-auth-v3]
  - [Client][design-client]
  - [Learner][design-learner]

[api_ref]: dev-guide/api_reference_v3.md
[api_concurrency_ref]: dev-guide/api_concurrency_reference_v3.md
[api_grpc_gateway]: dev-guide/api_grpc_gateway.md
[clustering]: op-guide/clustering.md
[conf]: op-guide/configuration.md
[system-limit]: dev-guide/limit.md
[faq]: faq.md
[why]: learning/why.md
[data_model]: learning/data_model.md
[demo]: demo.md
[download_build]: dl_build.md
[embed_etcd]: https://godoc.org/github.com/etcd-io/etcd/embed
[grpc_naming]: dev-guide/grpc_naming.md
[failures]: op-guide/failures.md
[gateway]: op-guide/gateway.md
[glossary]: learning/glossary.md
[namespace_client]: https://godoc.org/github.com/etcd-io/etcd/clientv3/namespace
[namespace_proxy]: op-guide/grpc_proxy.md#namespacing
[grpc_proxy]: op-guide/grpc_proxy.md
[hardware]: op-guide/hardware.md
[interacting]: dev-guide/interacting_v3.md
[local_cluster]: dev-guide/local_cluster.md
[performance]: op-guide/performance.md
[recovery]: op-guide/recovery.md
[maintenance]: op-guide/maintenance.md
[security]: op-guide/security.md
[monitoring]: op-guide/monitoring.md
[v2_migration]: op-guide/v2-migration.md
[container_rkt]: op-guide/container.md#rkt
[container_docker]: op-guide/container.md#docker
[understand_apis]: learning/api.md
[versioning]: op-guide/versioning.md
[supported_platforms]: op-guide/supported-platform.md
[container_linux_platform]: platforms/container-linux-systemd.md
[freebsd_platform]: platforms/freebsd.md
[aws_platform]: platforms/aws.md
[experimental]: dev-guide/experimental_apis.md
[authentication]: op-guide/authentication.md
[design-auth-v3]: learning/design-auth-v3.md
[design-client]: learning/design-client.md
[design-learner]: learning/design-learner.md
[tuning]: tuning.md
[upgrading]: upgrades/upgrading-etcd.md
