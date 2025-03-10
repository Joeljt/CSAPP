## 信号屏蔽

当一个进程在运行时，比如我们的 shell 进程，内核可能会在任意时刻向其发送信号，这个信号有可能会打断当前的进程执行，把 CPU 的执行权交给其他进程。

信号机制可以看作是早期的异步事件处理机制，它在用户空间和内核空间提供了一种中断机制，让内核能够通过信号来触发不同的行为或状态变化。

很明显，由于程序执行是按指令执行的，在这个上下文切换的过程中就有可能出现并发问题，尤其是信号的处理往往还是在同一进程内发生的。

### 并发问题是如何出现的

以我们创建子进程执行外部命令为例，演示一下可能会出现的并发问题。假设我们的代码如下：

```c
if (!builtin_cmd(argv)) {
    // fork 一个子进程，用 execve 去调用执该程序，如果调用失败，就说明未找到对应的命令
    if ((pid = fork()) == 0) {
        // 执行 execve 去调用执行该程序，如果调用失败，就说明未找到对应的命令
        if (execve(argv[0], argv, environ) < 0) {
            printf("%s: Command not found\n", argv[0]);
        }
    }
    // 将刚创建的子进程添加到作业列表
    addjob(jobs, pid, bg ? BG : FG, cmdline);
}
```

由于进程的调度执行完全由 CPU 控制，而当 fork 函数执行完成创建子进程后，两个进程就是同时在执行的。

假设子进程的任务很简单并且优先得到了 CPU 的控制权，那子进程就有可能会在 shell 进程执行到 `addjob` 之前执行完成并退出。

此时，`sigchld_handler` 会回收刚刚创建的子进程，并尝试维护任务列表，但是此时任务列表中还没有该任务；同时，父进程的 `addjob` 仍然会被执行，然后 shell 进程就会将一个已经执行完毕的任务添加到任务列表中，导致任务管理混乱。

### 处理并发问题

利用 `sigprocmask` 以及其配套方法，我们可以在进行某些重要操作时，为当前进程暂时屏蔽掉某些信号的接收，等完成重要操作后再将屏蔽接触，从而避免并发问题。


```c
if (!builtin_cmd(argv)) {

        // 在 fork 子进程之前，屏蔽 SIGCHLD 信号
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask, NULL);

        if ((pid = fork()) == 0) {
            // 刚创建的子进程会继承父进程的配置，所以需要在子进程里先解除信号屏蔽
            // 否则如果子进程再创建孙进程，孙进程结束的时候无法通知到当前子进程，就会变成僵尸进程
            sigprocmask(SIG_UNBLOCK, &mask, NULL);

            if (execve(argv[0], argv, environ) < 0) {
                printf("%s: Command not found\n", argv[0]);
                exit(0);
            }
        }

        // 将刚创建的子进程添加到作业列表
        addjob(jobs, pid, bg ? BG : FG, cmdline);

        // 将子进程添加到任务中后，解除 SIG_CHLD 信号的屏蔽
        // 使用 UNBLOCK 而不是 SETMASK 以免不小心更新了整个屏蔽集，可能带来预料以外的问题
        sigprocmask(SIG_UNBLOCK, &mask, NULL);

        // ... ...
    }
}
```

在上面的代码中，我们在 fork 子进程之前，先为 shell 进程屏蔽掉 `SIG_CHLD` 信号，等将子进程添加到任务列表中以后再取消屏蔽，从而规避上述风险。

需要注意的是，刚刚 fork 的子进程会继承父进程的一切配置，所以在子进程创建完毕后，需要先解除对 `SIG_CHLD` 信号的屏蔽。

否则如果子进程再创建新的进程，由于它收不到 `SIG_CHLD` 信号，会无法回收其内部创建的其他子进程，使其成为僵尸进程。