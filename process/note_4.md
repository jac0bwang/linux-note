# 负载均衡/实时系统

本篇文章主要介绍：

- 多核下负载均衡
- 中断负载均衡、 RPS软中断负载均衡
- cgroups和CPU资源分群分配
- Android和Docker对cgroup的采用
- Linux为什么不是硬实时的
- preempt-rt对Linux实时性的改造

练习

- 用time命令跑1个含有2个死循环线程的进程
- 用taskset调整多线程依附的CPU
- 创建和分群CPU的cgroup，调整权重和quota
- cyclictest

## 1. 多核下负载均衡

RT 进程： N个优先级最高的RT分布到N个核

    pull_rt_task()
    push_rt_task()

普通进程

    周期性负载均衡
    IDLE时负载均衡
    fork和exec时负载均衡

通过运行 time 命令可以查看一个进程的运行时间

    # process-codes/day4/two-loops.c
    # 1.编译two-loops.c, gcc two-loops.c -pthread，运行:
        $ time ./a.out
        main pid:14958, tid:3075917568
        thread pid:14958, tid:3067521856
        thread pid:14958, tid:3075914560
        ^C

        real        1m10.050s
        user        2m20.016s
        sys 0m0.004s
    * 我们得到时间分布比例，理解2个死循环被均分到2个core。

负载均衡是最大化利用 CPU 资源的方法，要求在有任务 (task_struct，中
断，软中断) 执行时，所有的 CPU 都能利用上，不产生有任务处理却有 CPU 闲
置的情况。首先从任务的优先级的角度来看， CPU 处理的任务只有下面 4 种优
先级，按高到低依次是：

| 优先级  | Linux 中 CPU 所处状态|
| ------ |-----:|
|1       | 中断 |
|2       | 软中断   |
|3       | 处于 spinlock 等关闭了调度区间的进程 |
|4       | 普通进程 |
优先级数字越低优先级越高：**中断 > 软中断 > spinlock > 普通进程**

### 可以通过 API 或者 命令行设置进程在哪个 CPU 上运行：

![CPU affinity][1]

**taskset** 命令行设置 CPU affiniy

    taskset -a -p 01 19999
    taskset -a -p 02 19999
    taskset -a -p 03 19999

### IRQ 负载均衡：

top 命令可以看到花在 hi 和 si 中断的时间，当是网络流量比较大的时候就要作中断负载均衡
![IRQ affinity][2]

### 多核间的softIRQ scaling

哪个CPU core上的软中断，必须在哪个 core 上执行, Google 做了RPS 将包处理负载均衡到多个CPU

有时候有的网卡只有一个队列，一个队列的中断只能分配到一个核， Linux
设计是一个核上抛出的软中断只能在同一个核上执行， cpu 0 上的中断抛出一个软中断， tcp/ip 协议栈也只能在 cpu 0 的软中断上处理。 google 在 linux 内核里面加入了 rps 补丁，其作用是尽管中断是在一个 cpu 核上，但 tcp/ip 协议处理的软中断可以在多个核上进行处理。 rps 的原理是收到软中断后通过核间中断的方法给另外的核发中断，让其他核处理软中断，从而支持单队列情况下的网络负载均衡。

```bash
$ echo fffe > /sys/class/net/eth1/queues/rx-0/rps_cpus
fffe
$ watch -d "cat /proc/softirqs | grep NET_RX"
```

通过 **top** 命令可以通过 “”1“” 查看每个core的使用率和运行时间，来判断负载均衡

## 2. cgroups和CPU资源分群分配

给进程指定比例的 CPU 负载–cgroup
当前的程序是按程序的需要来占用 cpu 的，这样可能会出现一些问题，比
如用户 A 和 B 在同一个服务器上，如果 A 开的线程比 B 多，可能导致 A 一直占用 cpu， B 因为线程少，占用的权重比例少而得不到 cpu。于是我们想要一个分群的概率，让 A 和 B 各占有 50% 的 CPU，不管 A 线程有多少，最多只能占50% 的 CPU，这样保证 B 即使线程数量少，也可以得到足够的 CPU 来运行。

同样的道理类似于计费的网络带宽，可以根据用户缴费的情况分配 CPU，如果
未交费，就算 CPU 空闲也不分配 CPU 给用户。

cgroup 使用方法 cgroup 主要是设置以下 3 个属性，在 /sys/fs/cgroup/cpu 目录 mkdir 一个 group 后，会出现很多属性文件，我们主要通过以下属性来查询和设置。

    cgroup.procs:       将进程号 echo 进去
    cpu.cfs_period_us:  默认是 100000 基准时间 100ms
    cpu.cfs_quota_us:   配额默认是-1 最大值，设置可以比 100000 大，它与 period的比例表示 gruop 内线程最高可占 cpu 的比例
    cpu.shares:         权重，默认是 1024，调节 cpu.shares 可以调节不同 group 的 cpu 占用率

示例：

    # 3.编译two-loops.c, gcc two-loops.c -pthread，运行三份
    * 用top观察CPU利用率，大概各自66%。
    * 创建A,B两个cgroup

            baohua@baohua-VirtualBox:/sys/fs/cgroup/cpu$ sudo mkdir A
            baohua@baohua-VirtualBox:/sys/fs/cgroup/cpu$ sudo mkdir B
    * 把3个a.out中的2个加到A，1个加到B。

            baohua@baohua-VirtualBox:/sys/fs/cgroup/cpu/A$ sudo sh -c 'echo 14995 > cgroup.procs'
            baohua@baohua-VirtualBox:/sys/fs/cgroup/cpu/A$ sudo sh -c 'echo 14998 > cgroup.procs'
            baohua@baohua-VirtualBox:/sys/fs/cgroup/cpu/A$ cd ..
            baohua@baohua-VirtualBox:/sys/fs/cgroup/cpu$ cd B/
            baohua@baohua-VirtualBox:/sys/fs/cgroup/cpu/B$ sudo sh -c 'echo 15001 > cgroup.procs'

    * 这次发现3个a.out的CPU利用率大概是50%, 50%, 100%。
    * 杀掉第2个和第3个a.out，然后调整cgroup A的quota，观察14995的CPU利用率变化

            baohua@baohua-VirtualBox:/sys/fs/cgroup/cpu/B$ kill 14998
            baohua@baohua-VirtualBox:/sys/fs/cgroup/cpu/B$ kill 15001
    * 设置A group的quota为20ms：

            baohua@baohua-VirtualBox:/sys/fs/cgroup/cpu/A$ sudo sh -c 'echo 20000 > cpu.cfs_quota_us'
    * 设置A group的quota为40ms：

            baohua@baohua-VirtualBox:/sys/fs/cgroup/cpu/A$ sudo sh -c 'echo 40000 > cpu.cfs_quota_us'
    * 以上各自情况，用top观察a.out CPU利用率。

## 3. Android和Docker对cgroup的采用

Android 5.0 以后的版本就用到了 cgroup, 前后台群使用不同的，前台权重是 1024， 后台是 52，所以前台是很容易强到 CPU 的。

Docker 在启动的时候是可以设置一些配置的简介设置 cgroup 的参数

![Docker 和 cgroup][4]

## 4. Linux为什么不是硬实时的

![Hard realtime – 可预期性][5]

### REAL TIME 实时系统的含义

实时不是越快越好，是指可预期性。比如发射导弹时，必须保证在截止期
限内发射出去，否则后果可能是灾难性的。硬实时强调在恶劣的情况下，从唤
醒到任务真正执行之间的时间是可预期的，可以保证在预期的时间内执行到。
Linux 设计不保证是可预期的，因为 Linux 在中断，软中断，及 spinlock 的区间
时是不可抢占的，这些区间内的执行时间是不可预期的。 linux 不是硬实时的
系统，是软实时。

### LINUX 无法硬实时的原因

Linux 无法硬实时的原因就是 Linux 的CPU 在的 4 类区间有 3 类是无法进行抢占的，
如果 CPU 进入了这 3 类区间，Linux 是无法保证在这 3 类区间内的运行时间的。
所以无法达到硬实时的要求。Linux 抢占的时机很多，我们主要记住不可抢占的时机：

    1、中断，
    2、软中断，
    3、 spinlock 区间的进程。
如果在这三类任务时间内唤起了高优先级的任务，任
务是不能抢占的，只有当 CPU 脱离了这三类区间后，高优先级的任务才可以
进行抢占。

![Linux为什么不硬实时][6]

## 5. preempt-rt对Linux实时性的改造

Linux RT 补丁可以使 1，2，3类区间变成 4类区间就实现来可以抢占,
把中断变成线程来运行，RT 补丁可以 做到 100us 的响应

![PREEMPT_RT补丁][7]

[1]: images/4_01.jpg "CPU affinity"
[2]: images/4_02.jpg "IRQ affinity"
[3]: images/4_03.jpg "多核RPS"
[4]: images/4_04.jpg "Docker 和 cgroup"
[5]: images/4_05.jpg "Hard realtime – 可预期性"
[6]: images/4_06.jpg "Linux为什么不硬实时"
[7]: images/4_07.jpg "PREEMPT_RT补丁"