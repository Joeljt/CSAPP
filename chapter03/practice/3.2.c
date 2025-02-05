/**
 * For each of the following lines of assembly language, determine the appropriate instruction suffix based on the operands. (For example, mov can be rewritten as movb, movw, movl, or movq.)
 *   
 * mov l     %eax, (%rsp)
 * mov w    (%rax), %dx
 * mov b    $0xFF, %bl
 * mov b    (%rsp,%rdx,4), %dl
 * mov q    (%rdx), %rax
 * mov w    %dx, (%rax)
 * 
 * 在 x86-64 中，地址值一定是以 64 位的形式出现的。
 * 
 * 对应到寄存器中，就意味着一定是 (%rxx) 才能表示地址值。
 * 同时，对于确定指令的数据大小从而确定后缀来说，关注要操作的那个数据的类型，忽略内存。要操作的那个数据是什么类型，就对应什么后缀。
 *
 */