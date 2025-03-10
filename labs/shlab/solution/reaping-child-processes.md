## 子进程执行完成后的回收和处理

writeup 要求回收所有执行完毕的子进程，但是不阻塞 shell 进程。

在 sigchld_handler 里调用 waitpid，并利用 option 参数配置得到不阻塞的效果。

```c
void sigchld_handler(int sig) {
    int olderrno = errno;

    int status;
    pid_t pid;

    // waitpid 用来等待子进程结束或者被强行终止
    // pid = -1 表示监听所有子进程
    // status 用来记录结束的时候的状态
    // WNOHANG 表示不等待正在执行中的子进程，立即返回 0
    // WUNTRACED 表示返回已经停止或者挂起的子进程的 PID
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)
    {
        // 子进程正常结束
        if (WIFEXITED(status))
        {
            deletejob(jobs, pid);
        }

        // 被未捕获的信号强制停止
        else if (WIFSIGNALED(status))
        {
            printf("Job [%d] (%d) terminated by signal %d\n", pid2jid(pid), pid, WTERMSIG(status));
            deletejob(jobs, pid);
        }

        // 被挂起
        else if (WIFSTOPPED(status))
        {
            struct job_t *job = getjobpid(jobs, pid);
            if (job != NULL)
            {
                job->state = ST;
                printf("Job [%d] (%d) stopped by signal %d\n", pid2jid(pid), pid, WSTOPSIG(status));
            }
        }
    }

    errno = olderrno;
    return;
}
```

当子进程结束的时候，会触发 SIGCHLD 信号，然后系统会调用 sigchld_handler 来处理。

我们在这个方法中，调用 `waitpid` 来回收所有子进程，但是通过配置 `WNOHANG` 参数，使得 `waitpid` 不会阻塞，而是立即返回。

同时，根据 status 的值去判断子进程的退出状态，并进行相应的处理。

- 正常结束，调用 `deletejob` 删除任务；
- 被未捕获的信号强制停止，调用 `deletejob` 删除任务；
- 被挂起，更新任务状态，并打印日志；




