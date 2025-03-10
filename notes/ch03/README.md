## 程序的机器级表示

这一章主要是介绍 x86-64 架构的汇编语法（ATT 汇编）以及一些逆向的内容。

CPU 从最早的 16 位 8086，迭代到 32 位的 IA32，再到现代的 64 位架构，不再要求开发者能够手写汇编语言，但是看懂还是有必要的。

这里按照书中目录的结构进行笔记的整理和总结，过程中也会进行部分练习题的实现和相关知识点的整理。

- [寄存器的基本表示](./note/register-intro.md)
- [汇编指令介绍](./note/asm-code.md)
- [寻址方式对比](./note/asm-addressing.md)
- [汇编下两种不同的分支判断方式](./note/asm-branching.md)
- [循环语句在汇编中的表示](./note/asm-loops.md)
- [switch 语句在汇编中的表示](./note/asm-switch.md)
- [函数执行流程以及调用栈](./note/procedures.md)
- [内存对齐](./note/data-alignment.md)
- [数据在内存中的存储](./note/data-storage.md)
- [缓冲区溢出攻击](./note/buffer-overflow.md)
- [动态申请内存的调用栈](./note/stack-with-variable-size.md)
- [浮点数的机器级表示](./note/asm-floating-point.md)
