## Shell lab

在这个实验中，我们需要在官方提供的模板基础上，实现 7 个方法，最终会得到一个 tiny shell。

通过完成这个实验，我们将会接触到一些基本的系统编程，并收获以下技能：

1. 基本的命令行交互，参数的传递和解析；
2. 进程的创建和回收，进程组、作业管理等；
3. 信号的基本处理，包括处理函数的注册、信号的屏蔽、Ctrl-C/Ctrl-Z 的处理；
4. shell 程序的基本工作流程；

整个实验的完成与官方的 writeup 息息相关，其中提供了大量线索和提醒，几乎每句话都有用。确保最终完成实验后，writeup 中的每个提醒都能在代码中找到具体的应用。

同时，附带的 trace 文件也是实现引导，按顺序一个一个测试并与 rtest 做比对，即实验希望我们完成的顺序，有点儿测试驱动开发的意思。

这里按照这个顺序，记录一下完成这个 lab 的过程中涉及到的内容以及相应的知识点。

### 终端只是个普通程序

实现这个 lab 的第一件事就是对终端祛魅，意识到我们每天在用的终端实际上也只是一个普通的 C 程序。

如我们所知，这个程序从 main 程序开始，配置好一系列必要变量以后，开启一个死循环，每轮循环都输出一句 prompt，然后等待用户输入。

```c
int main(int argc, char **argv)
{
    char c;
    char cmdline[MAXLINE];

    while (1) {
        // 输出 tsh> 到 stdout，提示用户输入
        printf("tsh> ");
        fflush(stdout);

        // 读取用户输入并进行处理
        if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
            app_error("fgets error");
        if (feof(stdin))
        { /* End of file (ctrl-d) */
            fflush(stdout);
            exit(0);
        }

        /* Evaluate the command line */
        eval(cmdline);
        fflush(stdout);
        fflush(stdout);
    }
    exit(0); /* control never reaches here */
}
```

既然终端程序也只是一个普通的 C 程序，从 main 函数开始执行，那问题就是：用户的输入是怎么传进来的？

我们知道命令行以空格作为分隔来区分命令的不同部分，命令程序或者具体的参数。

当执行具体的函数时，系统能确保相关的参数信息可以通过 main 函数的两个参数获得：

```c
int main(int argc, char *argv[])
```

以 `/bin/ls -l -d &` 为例，其中：

1. `argc` 表示参数数量，其值为 4，表示输入的值中有 4 部分；
2. `argv` 表示具体的参数列表，以字符串数组的形式表示上面的输入；
    - `/bin/ls`
    - `-l`
    - `-d`
    - `&`

### 解析命令行的工具函数

命令行一般会指定不同的参数，程序需要从用户输入的命令行中提取不同的部分，进行相应的处理。

这个过程中往往会用到如下工具函数：

- `getopt`: 从 `argc` 以及 `argv` 中提取参数，根据指定的字符串格式顺序取值；
- `atoi`: 将一个 ASCII 码转为 int 值，常见场景为转换 `getopt` 读出来的命令行的某个参数值；

```c
int main(int argc, char **argv) {
    // 声明变量，用来接收命令行传递的参数
    int set_bits, lines_per_set, block_bits;
    char* tracefile = NULL;

    // 利用 getopt 方法获取命令行参数，并将其赋值给提前声明好的变量
    int opt;
    while((opt = getopt(argc, argv, "s:E:b:t:")) != -1) {
        switch (opt){
            case 's': set_bits = atoi(optarg); break;
            case 'E': lines_per_set = atoi(optarg); break;
            case 'b': block_bits = atoi(optarg); break;
            case 't': tracefile = optarg; break;
            default: exit(EXIT_FAILURE);
        }
    }
}
```

`man 3 getopt` 了解详细信息。

### 捕获 `ctrl-c` 和 `ctrl-z`

根据第八章的学习，我们知道 Linux 内核会在用户按下特定组合键的时候发送信号，用户需要拦截这种信号并进行处理。

以 `ctrl-c` 发送的 SIGINT 信号为例，我们需要：

1. 调用 `signal` 函数完成 signal handler 的 install；
2. 在自己声明的 signal handler 中进行特定的处理；

```c
void sigint_handler(int sig) {
    // 当用户按下 ctrl-c，这个函数就会被执行
}
Signal(SIGINT, sigint_handler);   /* ctrl-c */
```

### 有效输入？系统内置命令？还是某个程序？

通过对 `argv` 参数的一系列格式化操作，我们可以得到一个相对规范的参数列表。

这个时候我们就可以对输入的命令进行一个基本的拆分和判断：

1. `argv[0]` 根本没有值，用户可能没有输入任何有效内容，只是打了个回车，我们也不需要做任何处理；
2. `argv[0]` 要么是内置命令，要么是某个指定路径的二进制文件；
    - 程序需要遍历内置命令列表，确定当前命令是否是内置命令，如果是则直接执行；
    - 否则就会认为该命令是二进制文件的路径，需要新开一个子进程去处理；
    - 注意，即使是存在系统环境变量里的命令，比如 `ls`，也是需要新开一个子进程处理的，其不属于内置命令。

外部命令需要新开子进程处理的一个原因是进程隔离，因为 shell 无法确定外部命令的功能，需要用子进程来保护 shell 本身的内存空间不受破坏。

```c
int builtin_cmd(char **argv) {
    if (!strcmp(argv[0], "quit")) {
        exit(0);
    }
    return 0; /* not a builtin command */
}
```

在我们的 tsh 中，因为内置命令很少，我们只需要用 `strcmp` 来做简单的字符串匹配即可。

注意 `strcmp` 会在相等的时候返回 0，大于或者小于的时候返回 1 或者 -1，所以这里 `!` 实际上是等于 0 的判断，即是否相等。

### fork 子进程处理外部命令

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

### fork 期间屏蔽 SIGCHLD 信号避免并发问题


