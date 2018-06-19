# 进程的生命周期

## 1. 进程控制块 PCB 与 task_struct

进程是资源封装管理的单位，资源指占用的内存，文件系统，信号及处理
方法。线程是调度的单位，进程的区别是资源，资源是隔离的，进程把资源描述清楚就可以了

Process Control Block(PCB) 进程控制块, 在不同操作系统中用于描述进程，在 Linux 的 PCB 就是用 task_struct 来描述。
图1，描述来 task_struct 的部分成员

！[task_stuct][1]

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

PID 的数量：

```bash
$ cat /proc/sys/kernel/pid_max
32768
```

fork 炸弹

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

查看用户的 PID 限制：

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
著名的提权漏洞： [rageagainstthecage](https://blog.csdn.net/21cnbao/article/details/19040451)。 2.2 之后的安卓版本修复了此漏洞，方法
是检查 setuid 的返回值。[Android提权漏洞分析——rageagainstthecage](https://blog.csdn.net/feglass/article/details/46403501)

### linux 的 pid 与 tgid

一个进程 fork 出子进程后，从 linux 内核的角度看，对应的 pid 肯定不一
样。但是为了符合 POSIX 的标准要求， POSIX 要求规定同一个父进程 fork 出
的子进程，调用 getpid 返回的 pid 的号必须是一样的，我们用 top 命令查看进
程可以看到 fork 出的子进程与父进程的 Pid 号是一样的。 linux 实现的原理就
是通过增加一个 tgid 来实现父子进程调用 getpid 时返回值都一样的效果。

### linux 进程 task_struct 的三种数据结构

在 linux 代码中会涉及各种对 task_struct 的引用关系，比如调度算法中会将
task_struct 挂在链表上，父子进程的关系用树来描述， CFS 调度算法会用到红
黑树，通过 pid 查找进程则是用 hash 表的结构。其对应的数据结构![如图 1-2][2] 所
示

### 1.僵尸态是什么？

### 2.如何进入停止态？

### 3.什么是内存泄露？

### 4.task_struct 与 task_struct 之间的关系是什么？

## 2.llll

[1]: images/1-01.jpg "task_struct"
[2]: images/1-02.jpg "task_struct 的三种数据结构"
