# 进程的生命周期

## 1. 进程控制块 PCB 与 task_struct

进程是资源封装管理的单位，资源指占用的内存，文件系统，信号及处理
方法。线程是调度的单位，进程的区别是资源，资源是隔离的，进程把资源描述清楚就可以了

Process Control Block(PCB) 进程控制块, 在不同操作系统中用于描述进程，在 Linux 的 PCB 就是用 task_struct 来描述。描述来 task_struct 的部分成员

![task_stuct][1]

1. **mm 内存资源**: 进程的内存
1. **fs 文件系统资源**: 根路径和当前路径指针
1. **files 文件系统资源**: 进程打开的文件，文件描述符数组
1. **signal 信号资源**： 不同进程可以针对同一信号挂不同的处理方法
1. **pid 属性资源**： 描述进程的属性, 进程的 PID 是有限的

当前路经是在 **fs_struct** 中的

```c
struct fs_struct {
        int users;
        spinlock_t lock;
        seqcount_t seq;
        int umask;
        int in_exec;
        struct path root, pwd;
};
```

**files**  进程打开的文件描述符 fd_array

```c
/*
 * Open file table structure
 */
struct files_struct {
  /*
   * read mostly part
   */
        atomic_t count;
        struct fdtable __rcu *fdt;
        struct fdtable fdtab;
  /*
   * written part on a separate cache line in SMP
   */
        spinlock_t file_lock ____cacheline_aligned_in_smp;
        int next_fd;
        unsigned long close_on_exec_init[1];
        unsigned long open_fds_init[1];
        struct file __rcu * fd_array[NR_OPEN_DEFAULT];
};
```

### PID 的数量：

```bash
$ cat /proc/sys/kernel/pid_max
32768
```

### fork 炸弹

```bash
:(){:|： & }； ：
# : 函数名为冒号
# () 函数参数定义
# {} 函数定义
# ：调用自己
# |：递归调用自己
# & 后台执行
# ; 函数结束
# : 调用函数
```

### 查看用户的 PID 限制：

```bash
$ ulimit -a
core file size          (blocks, -c) 0
data seg size           (kbytes, -d) unlimited
scheduling priority             (-e) 0
file size               (blocks, -f) unlimited
pending signals                 (-i) 15225
max locked memory       (kbytes, -l) 16384
max memory size         (kbytes, -m) unlimited
open files                      (-n) 1024
pipe size            (512 bytes, -p) 8
POSIX message queues     (bytes, -q) 819200
real-time priority              (-r) 0
stack size              (kbytes, -s) 8192
cpu time               (seconds, -t) unlimited
max user processes              (-u) 15225       ##<<<<<
virtual memory          (kbytes, -v) unlimited
file locks                      (-x) unlimited
```

### PID 数量限制导致安卓的一键 root

安卓的 2.2.1 之前的版本被发现一个漏洞，很容易就被一键 root，安卓的调
试软件 adb 刚开始时有 root 权限，之后 adb 调用 api setuid(shell) 把自己从 root
用户降为 shell 用户。谷歌的工程师在调用时没有检查 setuid 的返回值，即默
认 setuid 总是可以成功。黑客们利用 uid 数量有限制的属性，将 shell 用户内的
pid 进程全部用完，这样调用 setuid 时是无法成功的，但因为没有检查返回值，
导致 adb 调用 setuid(shell) 后没有降权成功，还是有 root 权限。这就是 Android
著名的提权漏洞： [rageagainstthecage](https://blog.csdn.net/21cnbao/article/details/19040451)。
2.2 之后的安卓版本修复了此漏洞，方法是检查 setuid 的返回值。[Android提权漏洞分析——rageagainstthecage](https://blog.csdn.net/feglass/article/details/46403501)

### linux 的 pid 与 tgid

一个进程 fork 出子进程后，从 linux 内核的角度看，对应的 pid 肯定不一
样。但是为了符合 POSIX 的标准要求， POSIX 要求规定同一个父进程 fork 出
的子进程，调用 getpid 返回的 pid 的号必须是一样的，我们用 top 命令查看进
程可以看到 fork 出的子进程与父进程的 Pid 号是一样的。 linux 实现的原理就
是通过增加一个 tgid 来实现父子进程调用 getpid 时返回值都一样的效果。

### linux 进程 task_struct 的三种数据结构

task_struct 与 task_struct 之间的关系是什么？
在 linux 代码中会涉及各种对 task_struct 的引用关系，比如调度算法中会将
task_struct 挂在链表上，父子进程的关系用树来描述， CFS 调度算法会用到红
黑树，通过 pid 查找进程则是用 hash 表的结构。可以通过 **pstree** 命令
进行查看，父进程回收子进程，父对子的监控。

其对应的数据结构

![如图 1-2][2]

每种数据结构选择都是根据应用场景的需求来选择实现目的效率最高的数据结构

## 2. Linux 进程的生命周期

Linux 进程的生命周期状态图 ![Linux 进程的生命周期状态图][3]

进程运行时的 3 个基本状态:

操作系统包括实时系统对应进程一般都有 3 个状态，进程在有 CPU 时对
应运行态，无 CPU 时对应就绪态和睡眠态。就绪态指所有资源都准备好，只
要有 CPU 就可以运行了。睡眠指有资源还未准备好，比如读串口数据时，数
据还未发送。此时有 CPU 也无法运行，需要等资源准备好后变成就绪态，然
后得到 CPU 后才能变成运行态，其转换关系

### linux 进程扩展的 6 个状态

1. **僵尸态**： 子进程退出后，所有资源都消失了，只剩下 task_struct，父进程在 wait 函数中可以得到子进程的死亡原因。在 wait 之前子进程的状态就是僵尸态。
2. **深度睡眠**： 等待资源到位后才醒过来
3. **浅度睡眠**: 等待资源到位或收到信号后都会醒过来
4. **暂停**： stop 状态是被外部命令作业控制等强制进程进入的状态。
5. **就绪**： 未占用 CPU，等待调度算法调度到运行态的进程
6. **运行**： 占有 CPU，正在运行的线程。

### 僵尸态

**僵尸是什么?**
资源释放了， task_struct 还在，父进程可以查到子进程的死因
子进程死了之后，linux 会自动回收资源
![子进程的退出码][4]

### 进程的内存泄露

**什么是内存泄露？**
内存泄露指随着时间的增长，进程的内存使用呈现线性增长的情况，指的
是进程一直在运行，运行中申请了内存，但使用完后并没有释放，运行期间每
次都申请内存而不释放导致系统内存越来越少的情况。这里要理解内存泄露的
原因不可能是进程死了，内存没释放。因为进程死了之后就变成僵尸， Linux
会自动将进程中申请的资源全部释放，只留下 task_struct 让父进程 wait 
来查看状态。不可能再占用内存。

**如何观察内存泄漏？**
痛多多点检测法，多点采样，可以看到内存是的图像是震荡向上的。没内存泄漏的
内存是震荡收敛的一个图形

#### 僵尸示例：

参看 process_codes/day1/life_period.c 代码
waitpid 可以清除僵尸进程，如果没有走到 wait 一直处于僵尸状态，处于僵尸状态的进程
是无法 kill 掉的, 直接杀死父进程可以回收处于僵尸态的子进程

### 暂停状态

暂停状态是进程在运行过程中，通过外部 bash 命令强制让进程进入的状
态。通过这种方法可以指定进程的 CPU 占用率。后面我们通常用 cgroup 的方
法来实现，这里仅作了解。

**如何进入停止态？** ctrl + z 进入停止状态，fg/bg可以调控前台/后台运行

```bash
# #作业控制的命令
# ctrl + z, fg/bg
$ cpulimit
$ cpulimit -l 20 -p 10111
# 限制 pid 为 10111 程序的 CPU 使用率不超过 20%
```

作业控制命令![job control][5]

### linux 进程状态的联系和区别

1. **就绪 VS 运行** linux 的调度算法只管理 **就绪和运行态** 中的进程，只对应中的就绪和占有状态的进程，这两个状态都称为 **task_running**。
2. **深度睡眠 VS 浅度睡眠** 深度睡眠只有资源到位才醒，收到信号也不醒，浅度睡眠资源到位或收到信号都会醒
3. **睡眠 VS 暂停** 睡眠是代码中未得到资源主动进入的状态，暂停是程序外部强制进程进入的状态。

### fork

fork 返回值， 父进程返回子进程的 PID, 子进程返回 0

打印几个hello

![fork_hello][6]

[1]: images/1_01.jpg "task_struct 结构体信息"
[2]: images/1_02.jpg "task_struct 链接的三种数据结构"
[3]: images/1_03.jpg "Linux 进程的生命周期状态图"
[4]: images/1_04.jpg "子进程的退出码"
[5]: images/1_05.jpg "作业控制的命令"
[6]: images/1_06.jpg "fork hello"