# Linux Traps

因为项目需要一个通用的x64程序(可以在 centos/ubuntu 等多个发行版上运行)，所以在 alpine 容器中编译了一个全静态链接的程序，
在很多机器上测试都是没有问题的，直到在一台机器上出现指令错误。

```bash
[user@linux ~]$ ./myapp
Illegal instruction
```

## 问题调查

查看 /var/log/messages 日志，发现了 kernel traps 日志，问题是 `invalid opcode`。

```messages
Sep  6 09:07:10 rhel7 kernel: traps: myapp[23024] trap invalid opcode ip:419fc2 sp:7fff9deed300 error:0 in myapp[401000+5f0000]
```

通过 gdb 定位到 问题出现在一条 `vmovdqu` 指令上。

```gdb
(gdb) r
Program received signal SIGILL, Illegal instruction.
0x0000000000419fc2 in _GLOBAL__sub_I__ZN3ue214resolveAssertsERNS_13ReportManagerERNS_8NGHolderERKNS_14ExpressionInfoE ()

(gdb) bt
#0  0x0000000000419fc2 in _GLOBAL__sub_I__ZN3ue214resolveAssertsERNS_13ReportManagerERNS_8NGHolderERKNS_14ExpressionInfoE ()
#1  0x00000000009dd20a in libc_start_init () at src/env/__libc_start_main.c:64
#2  0x00000000009dd22f in libc_start_main_stage2 (main=0x41f398 <main(int, char**)>, argc=2, argv=0x7fffffffe318) at src/env/__libc_start_main.c:92
#3  0x000000000041f22e in _start ()

(gdb) disassemble _GLOBAL__sub_I__ZN3ue214resolveAssertsERNS_13ReportManagerERNS_8NGHolderERKNS_14ExpressionInfoE
Dump of assembler code for function _GLOBAL__sub_I__ZN3ue214resolveAssertsERNS_13ReportManagerERNS_8NGHolderERKNS_14ExpressionInfoE:
   0x0000000000419f44 <+0>:     push   %rbp
   0x0000000000419f45 <+1>:     mov    $0x8,%ecx
   0x0000000000419f4a <+6>:     mov    $0x39,%edx
   0x0000000000419f4f <+11>:    mov    %rsp,%rbp
   0x0000000000419f52 <+14>:    push   %r13
   0x0000000000419f54 <+16>:    lea    -0xe8(%rbp),%rdi
   0x0000000000419f5b <+23>:    mov    $0x30,%esi
   0x0000000000419f60 <+28>:    push   %r12
   0x0000000000419f62 <+30>:    lea    -0xa8(%rbp),%r12
   0x0000000000419f69 <+37>:    lea    -0x68(%rbp),%r13
   0x0000000000419f6d <+41>:    push   %rbx
   0x0000000000419f6e <+42>:    lea    -0x48(%rbp),%rbx
   0x0000000000419f72 <+46>:    sub    $0xd8,%rsp
   0x0000000000419f79 <+53>:    mov    %fs:0x28,%rax
   0x0000000000419f82 <+62>:    mov    %rax,-0x28(%rbp)
   0x0000000000419f86 <+66>:    xor    %eax,%eax
   0x0000000000419f88 <+68>:    rep stos %eax,%es:(%rdi)
   0x0000000000419f8a <+70>:    mov    $0x80000000,%eax
   0x0000000000419f8f <+75>:    mov    %r12,%rdi
   0x0000000000419f92 <+78>:    mov    %rax,-0xe0(%rbp)
   0x0000000000419f99 <+85>:    call   0x50f9f8 <_ZN3ue29CharReachC2Ehh>
   0x0000000000419f9e <+90>:    mov    $0x5a,%edx
   0x0000000000419fa3 <+95>:    mov    $0x41,%esi
   0x0000000000419fa8 <+100>:   mov    %r13,%rdi
   0x0000000000419fab <+103>:   call   0x50f9f8 <_ZN3ue29CharReachC2Ehh>
   0x0000000000419fb0 <+108>:   mov    $0x7a,%edx
   0x0000000000419fb5 <+113>:   mov    $0x61,%esi
   0x0000000000419fba <+118>:   mov    %rbx,%rdi
   0x0000000000419fbd <+121>:   call   0x50f9f8 <_ZN3ue29CharReachC2Ehh>
=> 0x0000000000419fc2 <+126>:   vmovdqu -0x48(%rbp),%xmm0
   0x0000000000419fc7 <+131>:   vmovdqu -0x38(%rbp),%xmm1
   0x0000000000419fcc <+136>:   lea    -0x88(%rbp),%rdi
   0x0000000000419fd3 <+143>:   mov    %r13,%rsi
   0x0000000000419fd6 <+146>:   vmovdqu %xmm0,-0x88(%rbp)
   0x0000000000419fde <+154>:   vmovdqu %xmm1,-0x78(%rbp)
   0x0000000000419fe3 <+159>:   call   0x50fad6 <_ZN3ue28bitfieldILm256EEoRERKS1_>
   0x0000000000419fe8 <+164>:   vmovdqu -0x88(%rbp),%xmm2
```

指令"vmovdqu"需要CPU支持SSE2（Streaming SIMD Extensions 2）特性。

> "vmovdqu"是x86架构下的一条指令，用于在寄存器和内存之间执行无对齐的双字（128位）数据传输。它是基于SSE2指令集提供的一种向量指令。
>
>SSE2是Intel于2000年引入的一组SIMD（Single Instruction, Multiple >Data）扩展指令。SSE2扩展了x86架构的指令集，使处理器能够同时处理多个相同类型的数据，并且在处理多媒体、图形、数值计算等方面提供了更高效的数据处理能力。
>
>要使用"vmovdqu"指令，CPU必须支持SSE2扩展，并且操作系统和编译器也需要相应地支持这些指令。大多数现代的x86架构CPU都支持SSE2特性，因此通常不会出现兼容性问题。但如果在较老的或非x86架构的CPU上运行，可能会导致该指令无法执行或产生异常。在开发时，建议根据目标平台的CPU特性要求，确保代码在相关的CPU上正常工作。

## 问题原因

该主机的CPU不支持 SSE2 特性导致的。可以通过 `lscpu` 命令 或 `cat /proc/cpuinfo` 来确定 CPU 支持的特性(flags).


## 解决方案

在CPU特性最少的机器上进行编译。






https://wiki.gentoo.org/wiki/GCC_optimization