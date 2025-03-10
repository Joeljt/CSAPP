/**
 * It is common to find assembly-code lines of the form
 *  
 *   xorq %rcx,%rcx
 *
 * in code that was generated from C where no exclusive-or operations were present.
 *
 * A. Explain the effect of this particular exclusive-or instruction and what useful operation it implements.
 * 
 * 上述指令用来将 %rax 设置为 0。利用了异或操作的自反性，即任何值和它自身异或的结果都是 0。
 * 对应的 C 代码应该是 x = 0
 * 
 * B. What would be the more straightforward way to express this operation in assembly code?
 * 
 * 直接用 mov 指令也可以实现相同的效果：movq $0, %rax。
 * 
 * C. Compare the number of bytes to encode any two of these three different implementations of the same operation.
 *
 * 1. xor 版本只需要 3 个字节即可完成该操作，movq 版本需要 7 个字节。
 * 2. 或者可以利用 x86-64 的 32 位寄存器操作特性，即为寄存器赋值 32 位值的时候，会隐式清空高位字节：
 *      xorl %edx, %edx (2 字节)
 *      movl $0, %edx (5 字节)
 * 
 */