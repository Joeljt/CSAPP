/**
 * Assume the following values are stored at the indicated memory addresses and registers:
 * Address      Value
 * ----------------------------
 * 0x100        0xFF
 * 0x104        0xAB
 * 0x108        0x13
 * 0x10C        0x11
 * 
 * Register Value
 * ----------------------------
 * %rax     0x100
 * %rcx       0x1
 * %rdx       0x3
 *
 * Fill in the following table showing the values for the indicated operands:
 * Operand               Value
 * ---------------------------------
 * %rax                   0x100     # 寄存器，直接取寄存器的值                  
 * 0x104                  0xAB      # 内存，直接取对应内存地址的值
 * $0x108                 0x108     # 常量，表示该值本身
 * (%rax)                 0xFF      # 内存，() 表示取该内存地址存储的值，类似指针的 *，所以就是取 0x100 地址的值
 * 4(%rax)                0xAB      # 内存，4 + 0x100，进位后得到 0x104，其对应的值就是 0xAB
 * 9(%rax,%rdx)           0x11      # 9 + 0x100 + 0x3，进位得到 112，也就是 0x10C，对应 0x11
 * 260(%rcx,%rdx)         0x13      # 260 + 1 + 3 = 264，16 进制表示为 0x108，对应 0x13
 * 0xFC(,%rcx,4)          0xFF      # 0xFC + 0x1 * 4 = 0x100，对应 0xFF
 * (%rax,%rdx,4)          0x11      # 0x100 + 0x3 * 4 = 0x10C，对应 0x11
 */
