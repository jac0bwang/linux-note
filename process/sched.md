# Linux 进程调度

在 Linux 里面，进程大概可以分成两种。

1. 一种称为实时进程，也就是需要尽快执行返回结果的那种。
2. 一种是普通进程，大部分的进程其实都是这种。

优先级就是一个数值，数值越小，优先级越高。

- 实时进程，优先级的范围是 0～99；
- 普通进程，优先级的范围是 100～139。

## 1. 实时调度策略

SCHED_FIFO, SCHED_RR, SCHED_DEADLINE 是实时进程的调度策略。

- SCHED_FIFO 高优先级的进程可以抢占低优先级的进程，而相同优先级的进程先来先运行
- SCHED_RR 轮流调度算法，采用时间片，相同优先级的任务当用完时间片会被放到队列尾部，以保证公平性，而高优先级的任务也是可以抢占低优先级的任务。
- SCHED_DEADLINE，是按照任务的 deadline 进行调度的。当产生一个调度点的时候，DL 调度器总是选择其 deadline 距离当前时间点最近的那个任务，并调度它执行。

## 2. 普通调度策略

SCHED_NORMAL, SCHED_BATCH, SCHED_IDLE 是普通进程的调度策略

- SCHED_NORMAL 是普通的进程
- SCHED_BATCH 是后台进程，几乎不需要和前端进行交互
- SCHED_IDLE 是特别空闲的时候才跑的进程

## 3. 调度类

为了实现以上的调度策略，Linux 用 sched_class 把调度策略分为调度类，在实际的运行中从上到下遍历所有的调度类，找到合适的调度类：

- stop_sched_class 优先级最高的任务会使用这种策略，会中断所有其他线程，且不会被其他任务打断；
- dl_sched_class 就对应上面的 deadline 调度策略；
- rt_sched_class 就对应 RR 算法或者 FIFO 算法的调度策略，具体调度策略由进程的 task_struct->policy 指定；
- fair_sched_class 就是普通进程的调度策略；
- idle_sched_class 就是空闲进程的调度策略。

## 4. 完全公平调度算法（CFS）

这里只分析普通进程的调度策略 ： 完全公平调度算法， 其他的调度策略相对简单

### 4.1 算法的原理

**记录进程的运行时间**， CPU 会提供一个时钟，过一段时间就触发一个时钟中断。就像咱们的表滴答一下，也叫 Tick。
CFS 会为每一个进程安排一个虚拟运行时间 vruntime。如果一个进程在运行，随着时间的增长，也就是一个个 tick 的到来，进程的 vruntime 
将不断增大。没有得到执行的进程 vruntime 不变。

那些 vruntime 少的，原来受到了不公平的对待，需要给它补上，所以会优先运行这样的进程

下面是跟新 vruntime 的计算方法：

在这里得到当前的时间，以及这次的时间片开始的时间，两者相减就是这次运行的时间 delta_exec ，
但是得到的这个时间其实是实际运行的时间，需要做一定的转化才作为虚拟运行时间 vruntime

    实际运行时间 delta_exec = 当前的时间 - 本次的时间片开始的时间
    虚拟运行时间 vruntime += 实际运行时间 delta_exec * NICE_0_LOAD/ 权重

算法的主要思想就是按照比例的实现公平调度的：

对于同样的实际运行时间，给高权重的算少了，低权重的算多了，但是当选取下一个运行进程的时候，还是按照最小的 vruntime 来的，这样高权重的获得的实际运行时间自然就多了。

这就相当于给一个体重 (权重)200 斤的胖子吃两个馒头，和给一个体重 100 斤的瘦子吃一个馒头，然后说，你们两个吃的是一样多。这样虽然总体胖子比瘦子多吃了一倍，但是还是公平的。

这里的权重对应下面数组的值， NICE_0_LOAD 就是 1024，也即是优先级越高所得的虚拟运行时间就越小
这个权重可以通过 用户态做调整使用命令 nice 就可以， 不会使用可以 `man nice`, 这里的 -20 ~ 19 也就是内核 100～139 的优先级
内核还是倾向与 IO 类型的调度的，由于 IO 类型的应用大部分时间都在睡眠，所以响应的 vruntime 就会小

```c
/*
 * Nice levels are multiplicative, with a gentle 10% change for every
 * nice level changed. I.e. when a CPU-bound task goes from nice 0 to
 * nice 1, it will get ~10% less CPU time than another CPU-bound task
 * that remained on nice 0.
 *
 * The "10% effect" is relative and cumulative: from _any_ nice level,
 * if you go up 1 level, it's -10% CPU usage, if you go down 1 level  
 * it's +10% CPU usage. (to achieve that we use a multiplier of 1.25.
 * If a task goes up by ~10% and another task goes down by ~10% then  
 * the relative distance between them is ~25%.)
 */
const int sched_prio_to_weight[40] = {
 /* -20 */     88761,     71755,     56483,     46273,     36291,
 /* -15 */     29154,     23254,     18705,     14949,     11916,
 /* -10 */      9548,      7620,      6100,      4904,      3906,
 /*  -5 */      3121,      2501,      1991,      1586,      1277,
 /*   0 */      1024,       820,       655,       526,       423,
 /*   5 */       335,       272,       215,       172,       137,
 /*  10 */       110,        87,        70,        56,        45,
 /*  15 */        36,        29,        23,        18,        15,
};
```

## 5. 调度队列与调度实体

通过上面的分析， CFS 需要一个数据结构来对 vruntime 进行排序，找出最小的那个
这个能够排序的数据结构不但需要查询的时候，能够快速找到最小的，更新的时候也需要能够快速的调整排序，要知道 vruntime 可是经常在变的，
变了再插入这个数据结构，就需要重新排序。能够平衡查询和更新速度的是树，在这里使用的是**红黑树**。红黑树的的节点是应该包括 vruntime 的，称为调度实体

每个 CPU 都有自己的 struct rq 结构，其用于描述在此 CPU 上所运行的所有进程，其包括一个实时进程队列 rt_rq 和一个 CFS 运行队列 cfs_rq，
在调度时，调度器首先会先去实时进程队列找是否有实时进程需要运行，如果没有才会去 CFS 运行队列找是否有进行需要运行。

这里面 rb_root 指向的就是红黑树的根节点，这个红黑树在 CPU 看起来就是一个队列，不断的取下一个应该运行的进程。rb_leftmost 指向的是最左面的节点。

![rq数据结构的关系](images/sched_rq.jpeg)

在每个 CPU 上都有一个队列 rq，这个队列里面包含多个子队列，例如 rt_rq 和 cfs_rq，
不同的队列有不同的实现方式，cfs_rq 就是用红黑树实现的。当某个 CPU 需要找下一个任务执行的时候，会按照优先级依次调用调度类，
不同的调度类操作不同的队列。当然 rt_sched_class 先被调用，它会在 rt_rq 上找下一个任务，只有找不到的时候，
才轮到 fair_sched_class 被调用，它会在 cfs_rq 上找下一个任务。这样保证了实时任务的优先级永远大于普通任务。

![调度过程](images/sched_flow.jpeg)

## 6. 主动调度

调度的方式主要有两种方式。

1. 自愿调度（主动调度） A voluntary context switch occurs when a task blocks because it requires a resource that is unavailable.
2. 不自愿调度（抢占调度）A involuntary context switch takes place when a task executes for the duration of its time slice and then is forced to relinquish the processor.

计算机主要处理计算、网络、存储三个方面。
计算主要是 CPU 和内存的合作；网络和存储则多是和外部设备的合作；
在操作外部设备的时候，往往需要让出 CPU，内核调用 schedule() 函数让出CPU。 用户空间可以查看 `man sched` 或者 sleep(), 延时函数都可以让出 CPU

在 __schedule 里面会做两件事情，

1. 选取下一个进程，
2. 进行上下文切换。

而上下文切换又分用户态进程空间的切换和内核态的切换

![主动调度的过程](images/sched_context_switch.jpeg)

vmstat 、 pidstat 和 /proc/interrupts可以查看进程的上下文切换次数。

## 7. 抢占调度

不自愿调度（抢占调度）A involuntary context switch takes place when a task executes for the duration of its time slice and then is forced to relinquish the processor.

### 7.1 调度标记

`抢占场景一 时钟中断`

最常见的现象就是一个进程执行时间太长了，是时候切换到另一个进程了。那怎么衡量一个进程的运行时间呢？
在计算机里面有一个时钟，会过一段时间触发一次时钟中断，通知操作系统，时间又过去一个时钟周期，这是个
很好的方式，可以查看是否是需要**抢占的时间点**。

时钟周期到了，会产生中断，并调用**时钟中断**处理函数会调用 scheduler_tick()

这个函数先取出当前 cpu 的运行队列，然后得到这个队列上当前正在运行中的进程的 PCB(task_struct)，
然后调用这个 task_struct 的调度类的 tick 处理函数，这个函数就是来处理时钟事件的。
如果当前运行的进程是普通进程，调度类为 fair_sched_class，调用的处理时钟的函数为 task_tick_fair。
时钟处理函数中，它会更新当前进程的 vruntime，然后调用 check_preempt_tick 检查是否是时候被抢占了

当发现当前进程应该被抢占，不能直接把它踢下来，而是把它标记为应该被抢占。为什么呢？ 因为进程调度一定要等待正在运行的
进程调用 __schedule 才行啊，所以这里只能先标记一下。标记一个进程应该被抢占，都是调用 resched_curr 这个函数，它会调用
set_tsk_need_resched，标记进程应该被抢占，但是此时此刻，并不真的抢占，而是打上一个标签 TIF_NEED_RESCHED。

`抢占场景二 进程被唤醒`

当被唤醒的进程优先级高于 CPU 上的当前进程，就会触发抢占。wake_up() 会将这个唤醒的任务添加到队列当中，然后激活这个任务。
在激活的过程中会调用 check_preempt_curr 检查是否应该发生抢占。如果应该发生抢占，也不是直接抢占当前进程，而是**将当前进程标记**为应该被抢占

### 7.2 抢占的时机

真正的抢占还需要时机，也就是需要那么一个时刻，让正在运行中的进程有机会调用一下 __schedule。你可以想象，不可能某个
进程代码运行着，突然要去调用 __schedule，代码里面不可能这么写，所以一定要规划几个时机，这个时机分为用户态和内核态。

1. 用户态的抢占时机
   1. 从系统调用中返回的那个时刻，是一个被抢占的时机
   2. 从中断中返回的那个时刻，也是一个被抢占的时机。
2. 内核态的抢占时机
   1. 对内核态的执行中，被抢占的时机一般发生在在 preempt_enable() 中
   2. 在内核态也会遇到中断的情况，当中断返回的时候，返回的仍然是内核态。这个时候也是一个执行抢占的时机

![调度总结](images/sched_involuntary.png)
