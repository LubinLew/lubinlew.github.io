[【干货】APScheduler官方文档翻译 - 简书](https://www.jianshu.com/p/4f5305e220f0)

APscheduler全称Advanced Python Scheduler，作用为在指定的时间规则执行指定的作业。

- 指定时间规则的方式可以是间隔多久执行，可以是指定日期时间的执行，也可以类似Linux系统中Crontab中的方式执行任务。  

- 指定的任务就是一个Python函数。



## 基本概念

APScheduler四大组件：

- 触发器 `triggers` ：用于设定触发任务的条件
- 任务储存器 `job stores`：用于存放任务，把任务存放在内存或数据库中
- 执行器 `executors`： 用于执行任务，可以设定执行模式为单线程或线程池
- 调度器 `schedulers`： 把上方三个组件作为参数，通过创建调度器实例来运行

### 触发器

每一个任务都有自己的触发器，触发器用于决定任务下次运行的时间。

### 任务储存器

默认情况下，任务存放在内存中。也可以配置存放在不同类型的数据库中。如果任务存放在数据库中，那么任务的存取有一个序列化和反序列化的过程，同时修改和搜索任务的功能也是由任务储存器实现。

> **注！一个任务储存器不要共享给多个调度器，否则会导致状态混乱**

### 执行器

任务会被执行器放入线程池或进程池去执行，执行完毕后，执行器会通知调度器。

### 调度器

一个调度器由上方三个组件构成，一般来说，一个程序只要有一个调度器就可以了。开发者也不必直接操作任务储存器、执行器以及触发器，因为调度器提供了统一的接口，通过调度器就可以操作组件，比如任务的增删改查。

## 调度器组件详解

根据开发需求选择相应的组件，下面是不同的调度器组件：

- `BlockingScheduler` 阻塞式调度器：适用于只跑调度器的程序。
- `BackgroundScheduler` 后台调度器：适用于非阻塞的情况，调度器会在后台独立运行。
- `AsyncIOScheduler` AsyncIO调度器，适用于应用使用AsnycIO的情况。
- `GeventScheduler` Gevent调度器，适用于应用通过Gevent的情况。
- `TornadoScheduler` Tornado调度器，适用于构建Tornado应用。
- `TwistedScheduler` Twisted调度器，适用于构建Twisted应用。
- `QtScheduler` Qt调度器，适用于构建Qt应用。

任务储存器的选择，要看任务是否需要持久化。如果你运行的任务是无状态的，选择默认任务储存器`MemoryJobStore`就可以应付。但是，如果你需要在程序关闭或重启时，保存任务的状态，那么就要选择持久化的任务储存器。如果，作者推荐使用`SQLAlchemyJobStore`并搭配`PostgreSQL`作为后台数据库。这个方案可以提供强大的数据整合与保护功能。

执行器的选择，同样要看你的实际需求。默认的`ThreadPoolExecutor`线程池执行器方案可以满足大部分需求。如果，你的程序是计算密集型的，那么最好用`ProcessPoolExecutor`进程池执行器方案来充分利用多核算力。也可以将`ProcessPoolExecutor`作为第二执行器，混合使用两种不同的执行器。

配置一个任务，就要设置一个任务触发器。触发器可以设定任务运行的周期、次数和时间。APScheduler有三种内置的触发器：

- `date` 日期：触发任务运行的具体日期
- `interval` 间隔：触发任务运行的时间间隔
- `cron` 周期：触发任务运行的周期

一个任务也可以设定多种触发器，比如，可以设定同时满足所有触发器条件而触发，或者满足一项即触发。复合触发器，请查阅一下文档：[链接](https://apscheduler.readthedocs.io/en/latest/modules/triggers/combining.html#module-apscheduler.triggers.combining)


