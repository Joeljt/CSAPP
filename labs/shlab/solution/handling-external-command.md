## fork 子进程处理外部命令

如前所述，在确定用户输入的是外部命令后，我们就需要开辟一个子进程来处理该命令。

该进程作为一个新建的任务，有可能是前台任务，也可能是后台任务，需要将其纳入全局的任务列表进行统一管理。

同时，如果该命令是一个前台进程的话，在用户感知上就应该将父进程阻塞，等待新建的子进程把任务执行完毕以后，shell 进程再收回控制权。

```c
if (!builtin_cmd(argv)) {
    // 如果不是内置命令，就假设是一个用户指定的可执行程序的路径
    // fork 一个子进程，用 execve 去调用执该程序，如果调用失败，就说明未找到对应的命令
    if ((pid = fork()) == 0) {
        // 执行 execve 去调用执行该程序，如果调用失败，就说明未找到对应的命令
        if (execve(argv[0], argv, environ) < 0) {
            printf("%s: Command not found\n", argv[0]);
            exit(0);
        }
    }

    // 父进程
    // 将刚创建的子进程添加到作业列表
    addjob(jobs, pid, bg ? BG : FG, cmdline);

    /* 如果是前台进程，就需要等该命令执行结束；等待的方式就是父进程 waitpid */
    if (!bg) {
        waitfg(pid);
    } else {
        printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
    }
}
```

我们可以发现，这里是利用 `execve` 实现的在子进程中执行目标命令。同时，如果执行失败，就会报错我们常见的 Command not found。

这里需要注意的一点是，子进程的 printf 可以直接输出到 shell 进程，而不需要写任何 I/O 重定向的代码。

这是因为 fork 的子进程默认共享了父进程的文件描述符，即 STDIN_FILENO，STDOUT_FILENO 和 STDERR_FILENO。

所以子进程实际上就可以直接将内容输出到 shell 进程中，而不需要显式调用任何 `dub2` 之类的方法。
