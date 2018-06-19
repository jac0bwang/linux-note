# Linux 进程创建过程

- Linux 进程资源是如何处理的？
- 进程与进程间是怎样的关系？
- 进程死亡后会不会进程出生到死亡，生，死，睡过程内存泄露？
- 不同生命周期下进程资源的处理方式有什么差异？

## 1. 进程创建 ### fork 进程出生时资源处理

fork 出子进程后，子进程的资源就直接从父进程的进程结构 task_struct 拷
贝出同样的信息。进程 P2 刚创建后，其资源是一模一样的。之后随着进程变化，
本着谁修改谁分裂的原则进行资源变化的处理。最难分裂的是内存，分裂内存最好是
有 MMU 存在

![fork 的执行图][1]

### 进程分裂时的资源变化 – COW

父子进程刚诞生时所得到的资源是一样的，那这些资源在何时发生变化，以及变化后有
什么影响，这就涉及到了 linux 的 copy-on-write 技术，下面通过具体的实例来
说明父子进程的资源变化流程。

COW(copy-on-write) 技术是进程 fork 时采用的，涉及到虚拟内存和实际内存的
映射关系。采用了 COW 技术后，进程处理会有一些现象需要重点注意。比如fork 之
后的父子进程读写同一个全局变量时，一个变量在不同的进程会显示出不同的值

![COW 示意图][2]

```c
//process-codes/day2/cow.c
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int data = 10;

int child_process()
{
        printf("Child process %d, data %d\n",getpid(),data);
        data = 20;
        printf("Child process %d, data %d\n",getpid(),data);
        _exit(0);
}

int main(int argc, char* argv[])
{
        int pid;
        pid = fork();

        if(pid==0) {
                child_process();
        }
        else{
                sleep(1);
                printf("Parent process %d, data %d\n",getpid(), data);
                exit(0);
        }
}
/*
# 输出：
child process 9491, data 10
child process 9491, data 20
parent process 9490, data 10
*/
```

下面我们具体分析程序背后采用 COW 的原理和流程。fork 进程前后的内存关系

- fork 前：全局变量 data 对应数据段内存 vir 和 phy 都在数据段，权限为可读可写。
- fork 后： vir 和 Phy 的权限全部变成只读权限，读内存正常，写内存会进入page fault 缺页中断。
- fork 后写内存： 写内存后，发生缺页中断， Linux 会重新申请一个 4k 内存，将新物理内存指向更改了内地址的进程 vir。同时将老的 4k 内存拷贝给新的内存，同时将权限改为 R+W，这样父子进程的同一个 vir 虚拟地址就分别对应 2 个独立的可读可写的物理地址。总之谁先写谁拿到新的物理内存，原内存留给剩下的进程。

### vfork

COW 技术必须借助 MMU（内存管理单元）来实现。 COW 是通过改变虚
拟内存和物理内存的映射关系来实现，没有 MMU 的系统，无法实现虚拟内存
和物理内存的映射。也无法调用 fork 函数，无 MMU 系统对应调用的是 vfork
函数。没有 MMU(Copy-On-Write) 不支持 fork

使用 vfork : 父进程阻塞直到子进程: **exit** 或者 **exec**

![vfork 示意图][3]

### pthread_create

创建一个线程 ： 当 P1 和 P2 都用同一个资源，资源结构体不进行拷贝， P2 的资源指针直
接指向 P1，这样就体现出线程的特征：可以调度又共享一样的资源。 Linux 中
也是这样来实现线程的，调用 pthread_create 时，会调到 CLONE 的 API，这样
就会让 P2 的资源指针指向 P1。
![thread 示意图][4]

### POSIX 标准

PID 和 TGID

- TGID -- thread group ID
- getpid -- 返回的是 tgid
- __NR_gettid -- 返回的是 pid
- pthread_self -- c 库分配的一个编号

![PID 和 TGID 示意图][5]

```c
//process-codes/day2/thread.c
#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <linux/unistd.h>
#include <sys/syscall.h>

static pid_t gettid( void )
{
        return syscall(__NR_gettid);
}

static void *thread_fun(void *param)
{
        printf("thread pid:%d, tid:%d pthread_self:%lu\n", getpid(), gettid(),pthread_self());
        while(1);
        return NULL;
}

int main(void)
{
        pthread_t tid1, tid2;
        int ret;

        printf("thread pid:%d, tid:%d pthread_self:%lu\n", getpid(), gettid(),pthread_self());

        ret = pthread_create(&tid1, NULL, thread_fun, NULL);
        if (ret == -1) {
                perror("cannot create new thread");
                return -1;
        }

        ret = pthread_create(&tid2, NULL, thread_fun, NULL);
        if (ret == -1) {
                perror("cannot create new thread");
                return -1;
        }

        if (pthread_join(tid1, NULL) != 0) {
                perror("call pthread_join function fail");
                return -1;
        }

        if (pthread_join(tid2, NULL) != 0) {
                perror("call pthread_join function fail");
                return -1;
        }

        return 0;
}
```

## 2. 进程死亡

fork 执行后，就会变成 2 个进程返回，而不是一个进程返回两次。两个进
程用的是同一段代码，不同的是在判断 fork 的返回值后会走向不同的分支。子
进程返回的是 0，则 if(pid == 0) 后执行的是子进程，父进程接收到的返回值是
子进程的 pid 值。如下所示：

linux 中子进程死亡时首先变成僵尸，父进程通过 wait 来获取子进程的死
亡原因。调用的 API waitpid，父进程通过分析子进程的退出码就可以知
道具体的退出原因了。

如果子进程在返回之前父进程结束任务，则子进程死亡后有个原则，首先托付给 subreaper，如果没有 subreaper
则托付给 init。
![回收子进程示意图][6]

进程可以通过 API 来设置自己为 subreaper。subreaper 要注意调用 wait 来处理可能托付过来的僵尸进程。
![设置自己为 subreaper][8]

Example:
    杀掉父进程，查看子进程的状态

## 3. 睡眠以及唤醒

浅度睡眠和深度睡眠
![随眠][7]

**globalfifo_read** 读不到数据时，分几种情况，

- 非阻塞，立即返回
- 阻塞，设置为浅度睡眠， schedule()时才会睡眠

唤醒分2种情况：

- 唤醒是不是信号唤醒
- 不是信号唤醒就是数据返回

```c
static ssize_t globalfifo_read(struct file *filp, char __user *buf,
                               size_t count, loff_t *ppos)
{
        int ret;
        struct globalfifo_dev *dev = container_of(filp->private_data,
                struct globalfifo_dev, miscdev);

        DECLARE_WAITQUEUE(wait, current);

        mutex_lock(&dev->mutex);
        add_wait_queue(&dev->r_wait, &wait);

        while (dev->current_len == 0) {
                if (filp->f_flags & O_NONBLOCK) {
                        ret = -EAGAIN;
                        goto out;
                }
                __set_current_state(TASK_INTERRUPTIBLE);
                mutex_unlock(&dev->mutex);

                schedule();
                if (signal_pending(current)) {
                        ret = -ERESTARTSYS;
                        goto out2;
                }

                mutex_lock(&dev->mutex);
        }

        if (count > dev->current_len)
                count = dev->current_len;

        if (copy_to_user(buf, dev->mem, count)) {
                ret = -EFAULT;
                goto out;
        } else {
                memcpy(dev->mem, dev->mem + count, dev->current_len - count);
                dev->current_len -= count;
                printk(KERN_INFO "read %d bytes(s),current_len:%d\n", count,
                       dev->current_len);

                wake_up_interruptible(&dev->w_wait);

                ret = count;
        }
 out:
        mutex_unlock(&dev->mutex);
 out2:
        remove_wait_queue(&dev->r_wait, &wait);
        set_current_state(TASK_RUNNING);
        return ret;
}

static ssize_t globalfifo_write(struct file *filp, const char __user *buf,
                                size_t count, loff_t *ppos)
{
        struct globalfifo_dev *dev = container_of(filp->private_data,
                struct globalfifo_dev, miscdev);

        int ret;
        DECLARE_WAITQUEUE(wait, current);

        mutex_lock(&dev->mutex);
        add_wait_queue(&dev->w_wait, &wait);

        while (dev->current_len == GLOBALFIFO_SIZE) {
                if (filp->f_flags & O_NONBLOCK) {
                        ret = -EAGAIN;
                        goto out;
                }
                __set_current_state(TASK_INTERRUPTIBLE);

                mutex_unlock(&dev->mutex);

                schedule();
                if (signal_pending(current)) {
                        ret = -ERESTARTSYS;
                        goto out2;
                }

                mutex_lock(&dev->mutex);
        }

        if (count > GLOBALFIFO_SIZE - dev->current_len)
                count = GLOBALFIFO_SIZE - dev->current_len;

        if (copy_from_user(dev->mem + dev->current_len, buf, count)) {
                ret = -EFAULT;
                goto out;
        } else {
                dev->current_len += count;
                printk(KERN_INFO "written %d bytes(s),current_len:%d\n", count,
                       dev->current_len);

                wake_up_interruptible(&dev->r_wait);

                if (dev->async_queue) {
                        kill_fasync(&dev->async_queue, SIGIO, POLL_IN);
                        printk(KERN_DEBUG "%s kill SIGIO\n", __func__);
                }

                ret = count;
        }

 out:
        mutex_unlock(&dev->mutex);
 out2:
        remove_wait_queue(&dev->w_wait, &wait);
        set_current_state(TASK_RUNNING);
        return ret;
}
```

## 4. 第 1 个进程，进程 0 与进程 1

开机后进程 0 创建出进程 1，开机后进程 0 会退化成 idle 进程， idle 进程的
优先级最低。此进程运行的原则是所有其他的进程不运行时它就开始运行，当运行 idle 
进程时， cpu 就设置成低功耗模式。
(注：与开机键中的 suspend 的区别是 idle 状态时只有 cpu 是低耗， suspend 时显示器电源等其他设备也会进入
低功耗)。

此设计的精妙之处在于，如果不用进程 0，进程进入低功耗模式的判
断标准就变成了所有进程退出后要检查一下是否是最后一个进程，如果是最后
一个就进入低功耗模式。这样的设计就会把检查状态耦合到了每个进程之中。
增加进程 0 设计的好处在于，设计就简化只要判断是否在 idle 进程就可以了。
实现了去耦合。

![进程1/0，idle][9]

[1]: images/2_01.jpg "fork 的执行图"
[2]: images/2_02.jpg "COW 示意图"
[3]: images/2_03.jpg "vfork 示意图"
[4]: images/2_04.jpg "thread 示意图"
[5]: images/2_05.jpg "PID 和 TGID 示意图"
[6]: images/2_06.jpg "回收子进程示意图"
[7]: images/2_07.jpg "随眠"
[8]: images/2_08.jpg "设置自己为 subreaper"
[9]: images/2_09.jpg "进程1/0，idle"