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

- break main

### 单步调试

- stepi [n] 单个指令执行，如果遇到方法也会进去
- nexti [n] 同上，但是遇到方法会直接执行完成，不会进入
- finish 在方法内部可以直接执行完成返回

### 查看信息

- info stack 查看调用栈
- info registers 查看寄存器
- disas main 反编译二进制文件

### 布局

- layout src 源代码模式
- layout asm 汇编模式
- layout split 分屏

### 参考

- [CS:APP3e Bomb Lab Writeup](https://csapp.cs.cmu.edu/3e/bomblab.pdf)
- [Beej's Quick Guide to GDB](https://beej.us/guide/bggdb/)
- [Two-page x86-64 GDB cheat sheet](https://csapp.cs.cmu.edu/3e/docs/gdbnotes-x86-64.txt)
- [Linux C编程一站式学习](https://akaedu.github.io/book/ch10.html)
- [用 GDB 调试程序](https://blog.csdn.net/haoel/article/details/2879)