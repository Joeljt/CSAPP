## Shell lab

在这个实验中，我们需要在官方提供的模板基础上，实现 7 个方法，最终会得到一个 tiny shell。

通过完成这个实验，我们将会接触到一些基本的系统编程，并收获以下技能：

1. 基本的命令行交互，参数的传递和解析；
2. 进程的创建和回收，进程组、作业管理等；
3. 信号的基本处理，包括处理函数的注册、信号的屏蔽、Ctrl-C/Ctrl-Z 的处理；
4. shell 程序的基本工作流程；

整个实验的完成与官方的 writeup 息息相关，其中提供了大量线索和提醒，几乎每句话都有用。确保最终完成实验后，writeup 中的每个提醒都能在代码中找到具体的应用。

同时，附带的 trace 文件也是实现引导，按顺序一个一个测试并与 `rtest` 做比对，即实验希望我们完成的顺序，有点儿测试驱动开发的意思。

这里按照这个顺序，记录一下完成这个 lab 的过程中涉及到的内容以及相应的知识点。

- [1. 终端只是个普通程序](./solution/terminal-no-big-deal.md)
- [2. 解析命令行的工具函数](./solution/parsing-command-line.md)
- [3. 捕获中断信号](./solution/capturing-interrupt-signals.md)
- [4. 内置命令和外部命令](./solution/builtin-vs-external-command.md)
- [5. fork子进程处理外部命令](./solution/handling-external-command.md)
- [6. 信号屏蔽](./solution/signal-masking.md)
- [7. 前台进程、进程组和信号转发](./solution/fg-proc-and-signal-forwarding.md)
- [8. 子进程执行完成后的回收和处理](./solution/reaping-child-processes.md)
- [9. 作业控制](./solution/job-control.md)

