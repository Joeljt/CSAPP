### 前台进程、进程组和信号转发

我们说的所有的前台或后台任务，都是指 shell 进程创建的子进程，因为内部命令只是方法调用，不会被当作 Job 管理。

正常情况下，所有的前台任务会被放在一个进程组中统一处理，这个进程组对于带管道的命令尤其有用。

但是我们的实验中不需要实现管道，所以前台进程组中只会有一个前台任务。

对于前台任务来说，如果该任务比较耗时，不会立即执行完毕，那 shell 进程就需要阻塞等待，直到该任务执行完毕才能继续 shell 进程的循环，即打印下一条 prompt 等待用户输入。

### 阻塞等待

阻塞等待的实现很简单，只需要在调用完外部命令后，利用 while + sleep 阻塞 shell 进程，直到被信号中断或者执行完毕后退出。

```c
void waitfg(pid_t pid) {
    // 阻塞交互，等待当前前台进程执行完成
    while (pid == fgpid(jobs)) {
        sleep(1);
    }
}
```

实验要求使用 sleep 阻塞，直到前台进程执行完毕。除了 sleep 之外，还可以使用 `sigsuspend` 阻塞，直到收到信号，这里不展开。

### 前台进程组以及信号转发

我们的 shell 程序运行在真实的终端里，所以 shell 进程实际上是前台进程，属于前台进程组。

这种情况下，新建的子进程实际上会和 shell 进程处于同一个进程，我们需要在创建完子进程以后，通过调用 `setpgid(0, 0)` 将其单独放在一个进程组中，进程组 ID 就是子进程的 PID。

同时，我们还需要拦截 SIGINT 和 SIGTSTP 信号，然后通过 `kill(-pid, SIGINT)` 将其转发给前台进程组。

```c
if ((pid = fork()) == 0) {
    // ... 
    setpgid(0, 0);
    // ...
}

void sigint_handler(int sig) {
    // 保存 errno 并还原，以免影响到其他程序
    int olderrno = errno;

    // 检查当前是否有正在进行中的前台进程，如果有的话就给它发个 SIGINT 信号
    pid_t pid = fgpid(jobs);
    if (pid != 0) {
        // 给 -pid 发，可以发给整个进程组
        kill(-pid, SIGINT);
    }

    errno = olderrno;
}
```

对于 `ctrl-z` 来说，效果也是一样的，只不过是不同的 handler 以及转发不同的信号。

```c
void sigtstp_handler(int sig) {
    // ...
    kill(-fgpid(jobs), SIGTSTP);
}
```