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
 * %rax                  
 * 0x104
 * $0x108
 * (%rax)
 * 4(%rax)
 * 9(%rax,%rdx)
 * 260(%rcx,%rdx)
 * 0xFC(,%rcx,4)
 * (%rax,%rdx,4)
 */
