# 随机数

随机数生成器分为下面3种类型：

| 名称         | 生成类型 | 特性                   | 说明        |
| ---------- | ---- | -------------------- | --------- |
| 真随机数生成器    | 硬件生成 | 效率高、 随机性、不可预测性、不可重现性 | 需要从物理设备获取 |
| 伪随机数生成器    | 软件生成 | 效率高、随机性              | 通过算法获取    |
| 密码学伪随机数生成器 | 软件生成 | 效率高、随机性、不可预测性        | 用于密码学     |

CPU里通常会有真随机数生成器（True Random Number Generator，简称TRNG）和伪随机数生成器（Pseudo Random Number Generator，PRNG）。真随机数生成器通常是通过放大电路的热噪声来产生随机数，而伪随机数生成器则是使用线性反馈移位寄存器（Linear feedback shift register，LFSR）生成。国外还有专门使用大气噪声生成真随机数的机构 https://www.random.org 。

---

## Linux下随机数生成

###### random() 函数

```c
#include <stdlib.h>

long int random(void);
void srandom(unsigned int seed);
```

函数 `random()` 生成的是伪随机数。如果不是每次设置不同的种子，那么函数的生成随机序列是相同的。

```c
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(void)
{
   int i;
   long int rand;
 
   //srandom((long int)time(0));

   for (i = 0; i < 10; i++) {
      rand = random();
      printf("%ld\n", rand);
   }   
}
```

###### openssl rand 命令行

```bash
rand -base64 30
```

### /dev/random & /dev/urandom

`/dev/random`和`/dev/urandom`是Linux系统中提供的伪随机设备，这两个设备的任务，是提供永不为空的随机字节数据流。很多解密程序与安全应用程序（如SSH Keys,SSL Keys等）需要它们提供的随机数据流。

这两个设备的差异在于：

- `/dev/random`的 random pool 依赖于系统中断，因此在系统的中断数不足时，`/dev/random`设备会一直封锁，尝试读取的进程就会进入等待状态，直到系统的中断数充分够用, `/dev/random` 设备可以保证数据的随机性。

- `/dev/urandom`不依赖系统的中断，也就不会造成进程忙等待，但是数据的随机性也不高。

　　

----

## References

https://www.zhihu.com/question/20145173

[Linux系统产生随机数/dev/random 和 /dev/urandom - 周人假的](https://www.cnblogs.com/zhouhbing/p/5820899.html)


