# QCOW2镜像扩容及分区扩容

1. QCOW2 镜像扩容

> 需要先停止虚机

```bash
qemu-img resize some.qcow2 +40G
```

2.分区扩容

> 启动虚机, 磁盘和分区以实际为准

```bash
growpart  /dev/vda 1
resize2fs /dev/vda1
```

