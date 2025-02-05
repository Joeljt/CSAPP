/**    
 * 将 src_t a 强制类型转换为 dest_t b，补全需要的汇编指令。
 * （假设 a 在 %rdi，b 在 %rsi，使用 %rax 作为中间值）
 * 
 * src_t            dest_t              Instuctions                 comments
 * ---------------------------------------------------------------------------------
 * long             long                movq (%rdi), %rax           读 8 字节
 *                                      movq (%rax), (%rsi)         写 8 字节
 * 
 * char             int                 movsbl (%rdi), %eax         char 带符号，读的时候带符号扩展为 32 位，转为 int 类型
 *                                      movq %eax, (%rsi)           写入 4 字节
 * 
 * char             unsigned            movsbl (%rdi), %eax         同上，机器并不关心符号转入类型的符号状态，但是要保证 bit-pattern 是正确的
 *                                      movq %eax, (%rsi)           如果 char 本身是负数，在扩展的时候必须带符号扩展才能保证结果正确
 * 
 * unsigned char    long                movzbl (%rdi), %eax         先用零扩展补全高位，再用写 32 位隐式清空高位的特性完成转换
 *                                      movq %eax, (%rsi)           最后写入 8 字节
 * 
 * int              char                movl (%rdi), %eax           读 32 字节，记录 int 值
 *                                      movb %al, (%rsi)            读低 8 位，写入目标寄存器
 * 
 * unsigned         unsigned char       movl (%rdi), %eax           同上
 *                                      movb %al, (%rsi)
 * 
 * char             short               movsbw (%rdi), %ax          读取原来的 char 类型并带符号扩展为 16 位（目标类型大小）
 *                                      movw %ax, (%rsi)            写入低位 2 字节
 * 
 * 这个题目主要是练习强制类型转换的时候，符号位的扩展问题。
 * 同时，在 unsigned char 转 long 时，巧妙地利用了 32 位隐式清空的特性，用一条指令就完成了转换并清空高位的效果。
 * 
 * 在处理类似问题的时候：
 * 1. 大类型转小类型的时候，直接使用大类型的位宽接收，然后赋值的时候用正确 size 的 mov 指令即可；
 * 2. 小类型转大类型的时候，首先注意小类型是否带符号，其次要用大类型的位宽对小类型进行扩展接收，否则盛不下；
 * 
 */