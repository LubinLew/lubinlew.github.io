# LSF/BPF

Linux Socket Filtering(LSF) 又被称为 Berkeley Packet Filter(BPF).

## 介绍

LSF 是从 BPF 衍生而来的。虽然 BSD 和 Linux 内核过滤之间有一些明显的区别，但是基础架构一致。
LSF 内部的 BPF 最早是 cBPF(classic)，后来x86平台首先切换到 eBPF(extended)，
但由于很多上层应用程序仍然使用cBPF(tcpdump、iptables)，并且eBPF还没有支持很多平台，
所以内核提供了从 cBPF 向 eBPF 转换的逻辑，并且 eBPF 在设计的时候也是沿用了很多 cBPF 的指令编码。
但是在指令集合寄存器，还有架构设计上有很大不同, 例如 eBPF 已经可以调用C函数，并且可以跳转到另外的 eBPF 程序。

## 目录

- [cBPF](linux_c_programming/functions/LSF/cBPF.md)
- [eBPF](linux_c_programming/functions/LSF/eBPF.md)

## References

https://blog.csdn.net/ljy1988123/article/details/50444693