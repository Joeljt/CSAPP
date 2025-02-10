## gdb

gdb 是 Linux 下非常强大的一款 debug 工具，这里整理一些基本命令和参考资料。

### 启动

- gdb [-tui] [-c core] [exename]
    - `-tui` 启动 GUI 模式
    - `-c core` 指定当前程序的 dumpcore 名称，程序崩溃时保存到的目录
- run [arg1] [arg2]
    - 在命令行中执行代码并设置参数
- file
    - 启动 gdb 后再加载程序
- quit

### 断点

- break location

### 查看信息



### 参考

- [CS:APP3e Bomb Lab Writeup](https://csapp.cs.cmu.edu/3e/bomblab.pdf)
- [Linux C编程一站式学习](https://akaedu.github.io/book/ch10.html)
- [用 GDB 调试程序](https://blog.csdn.net/haoel/article/details/2879)