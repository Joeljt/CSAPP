## 作业控制

Unix shell 有一个 job control 的概念：

1. 所有的命令都有前台、后台以及暂停三种状态，shell 通过维护一个列表来记录所有运行中的任务；
2. 命令默认在前台执行，在执行的时候在命令末尾添加 `&` 可以将其作为后台任务执行；
3. 通过 `jobs` 命令查看正在运行中的所有任务列表；
4. 通过 `fg` 和 `bg` 来切换某个任务的执行状态；

```c
void do_bgfg(char **argv) {
    struct job_t *job = NULL;
    char *id = argv[1];
    int jid;
    pid_t pid;

    // bg 或 fg 命令，需要指定 pid 或者 jid
    // jid 需要以 % 开头，从而和 pid 进行区分
    if (id == NULL) {
        printf("%s: command requires PID or %%jobid argument\n", argv[0]);
        return;
    }

    // 根据 jid 或者 pid 进行不同的处理
    if (id[0] == '%') {
        // atoi 接收的是指针，从 &id[1] 开始往后遍历，找到最长的一个数字序列，并将其转为数字
        jid = atoi(&id[1]);
        job = getjobjid(jobs, jid);
        if (job == NULL) {
            printf("%%%d: No such job\n", jid);
            return;
        }
    } else if (isdigit(id[0])) {
        pid = atoi(id);
        job = getjobpid(jobs, pid);
        if (job == NULL) {
            printf("%d: No such process\n", pid);
            return;
        }
    } else {
            printf("%s: argument must be a PID or %%jobid\n", argv[0]);
            return;
    }

    // 给当前 job 发送 SIGCOT 信号唤醒该任务
    kill(-job->pid, SIGCONT);

    // 从参数里取出来 pid 和 jid 之后，继续判断 argv[0] 是前台还是后台
    if (!strcmp(argv[0], "bg")) {
        job->state = BG;
        printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline);
    } else {
        job->state = FG;
        waitfg(job->pid);
    }
}
```

根据命令的参数是否包含 `%`，可以分为两种情况：

- 包含 `%` 则说明是 jobId，则根据 `%` 后面的数字找到对应的任务；
- 不包含 `%`，则说明是 pid，则根据数字找到对应的进程；

找到任务以后，调用 `kill(-job->pid, SIGCONT)` 唤醒任务，并根据命令的不同设置任务的状态。

- `bg` 命令会将任务状态设置为 `BG`，并打印任务信息；
- `fg` 命令会将任务状态设置为 `FG`，并阻塞等待任务执行完成；
    - 将一个任务设置为前台任务后，需要调用 `waitfg(job->pid)` 阻塞等待任务执行完成；



