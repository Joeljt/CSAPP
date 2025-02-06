/**
 * In the following excerpts from a disassembled binary, 
 * some of the information has been replaced by X’s. Answer the following questions about these instructions.
 *
 *    A. What is the target of the je instruction below? (You do not need to know anything about the callq instruction here.)
 *       4003fa: 74 02                 je     XXXXXX
 *       4003fc: ff d0                 callq  *%rax
 * 
 *      跳转指令目标位置的编码格式是：target 在指令中的编码 + 当前指令的下一条指令的地址
 *      在本题中，target 的编码是 0x02，下一条指令的地址是 4003fc，所以目标位置就是：4003fe
 *
 *    B. What is the target of the je instruction below? 
 *       40042f: 74 f4                 je XXXXXX
 *       400431: 5d                    pop    %rbp
 * 
 *       同上，但是要注意这里 target 的指令是 0xf4，要当作 1 字节带符号数看，所以这是 -12。
 *       目标位置就是：0x400431 - 0xf4 = 0x400425
 *
 *    C. What is the address of the ja and pop instructions?
 *       XXXXXX: 77 02                 ja     400547
 *       XXXXXX: 5d                    pop    %rbp
 *       
 *       这题主要是考察内存地址的计算。
 *       先确定 jmp 指令的下一条指令，400547 - 0x02 即可得到 400545；
 *       然后再确定 jmp 指令所在的位置，jmp 指令用了 2 个字节，所以应该是 400545 - 2 = 400543。
 *
 *    D. Inthecodethatfollows,thejumptargetisencodedinPC-relativeformasa4- byte two’s-complement number. 
 *      The bytes are listed from least significant to most, reflecting the little-endian byte ordering of x86-64. 
 *      What is the address of the jump target?
 *       4005e8: e9 73 ff ff ff        jmpq   XXXXXXX
 *       4005ed: 90                    nop
 *    
 *       这里考察多个知识点：小端法的表示、jump 指令编码的方式等。
 *       首先这个 jump 指令是一个远跳转，需要用 4 字节进行编码，e9 是 jmp 指令，后面的才是地址编码。
 *       这里的地址使用小端法表示，即低位在前，高位在后，所以实际的内存地址是 0xffffff73，即 -141。
 *       然后再根据 jump 目标的计算方式：0x4005ed - 141，得到 400560。
 * 
 */