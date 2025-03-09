## 终端只是个普通程序

实现这个 lab 的第一件事就是对终端祛魅，意识到我们每天在用的终端实际上也只是一个普通的 C 程序。

如我们所知，这个程序从 main 程序开始，配置好一系列必要变量以后，开启一个死循环，每轮循环都输出一句 prompt，然后等待用户输入。

```c
int main(int argc, char **argv) {
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
