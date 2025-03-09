
## 捕获中断信号

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

`ctrl-z` 发送的 SIGTSTP 信号，处理方式与 SIGINT 类似。

```c
void sigtstp_handler(int sig) {
    // 当用户按下 ctrl-z，这个函数就会被执行
}
Signal(SIGTSTP, sigtstp_handler);   /* ctrl-z */
```

