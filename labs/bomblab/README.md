## Solution

bomblab 提供了一个二进制文件 bomb，执行后提示输入内容，如果输出错误程序就会爆炸。

需要利用 gdb 进行 debug，反编译破解二进制文件，通过查看汇编指令还原程序逻辑，确定需要输入的内容是什么。

程序一共有 6 个阶段，难度系数按阶段增加。

### 第一阶段

执行 bomb 程序后设置断点 `b phase_1`，反编译该方法如下：

```assemby
(gdb) disas phase_1
Dump of assembler code for function phase_1:
   0x0000000000400ee0 <+0>:	sub    $0x8,%rsp
   0x0000000000400ee4 <+4>:	mov    $0x402400,%esi
   0x0000000000400ee9 <+9>:	callq  0x401338 <strings_not_equal>
   0x0000000000400eee <+14>:	test   %eax,%eax
   0x0000000000400ef0 <+16>:	je     0x400ef7 <phase_1+23>
   0x0000000000400ef2 <+18>:	callq  0x40143a <explode_bomb>
   0x0000000000400ef7 <+23>:	add    $0x8,%rsp
   0x0000000000400efb <+27>:	retq   
End of assembler dump.
```

通过分析代码可以发现：

1. 栈指针向下移动，开辟了 8 字节空间；
2. 更新寄存器 `%esi` 的值为 `0x402400`，这是一个内存地址值；
3. 之后调用 `strings_not_equal` 方法后，用 test 指令检查一致性；
4. 如果值相同就回收栈空间后返回，否则爆炸结束。

那问题就很清晰了，只需要确认地址 `0x402400` 存储的内容是什么即可：

```assembly
(gdb) x/s 0x402400
0x402400:	"Border relations with Canada have never been better."
```

第一个炸弹拆除完成。🎉

### 第二阶段

继续下一个，断点到 `b phase_2` 后反编译，会发现方法内部会先调用一个 `read_six_numbers` 方法：

```assembly
0x0000000000400f05 <+9>:	callq  0x40145c <read_six_numbers>
```
所以我们先来看一下这个方法的逻辑：

```assembly
(gdb) disas read_six_numbers
Dump of assembler code for function read_six_numbers:
   0x000000000040145c <+0>:	sub    $0x18,%rsp
   0x0000000000401460 <+4>:	mov    %rsi,%rdx
   0x0000000000401463 <+7>:	lea    0x4(%rsi),%rcx
   0x0000000000401467 <+11>:	lea    0x14(%rsi),%rax
   0x000000000040146b <+15>:	mov    %rax,0x8(%rsp)
   0x0000000000401470 <+20>:	lea    0x10(%rsi),%rax
   0x0000000000401474 <+24>:	mov    %rax,(%rsp)
   0x0000000000401478 <+28>:	lea    0xc(%rsi),%r9
   0x000000000040147c <+32>:	lea    0x8(%rsi),%r8
   0x0000000000401480 <+36>:	mov    $0x4025c3,%esi
   0x0000000000401485 <+41>:	mov    $0x0,%eax
   0x000000000040148a <+46>:	callq  0x400bf0 <__isoc99_sscanf@plt>
   0x000000000040148f <+51>:	cmp    $0x5,%eax
   0x0000000000401492 <+54>:	jg     0x401499 <read_six_numbers+61>
   0x0000000000401494 <+56>:	callq  0x40143a <explode_bomb>
   0x0000000000401499 <+61>:	add    $0x18,%rsp
   0x000000000040149d <+65>:	retq   
End of assembler dump.
```

可以发现是对寄存器做了一堆操作后，调用了 C 标准库的 sscanf 函数，然后根据返回值是否大于 5 决定是否引爆炸弹。

其中，sscanf 函数的作用是接收可变长度的参数，按给定格式从输入字符串中提取内容，返回值是匹配中的元素个数。

很明显，这个方法就是和函数名一样的效果，用来强制要求输入的内容是 6 个数字：

1. 第一个参数在寄存器 `%rdi` 中，表示输入的内容；
2. 第二个参数是格式化规格，储存在寄存器 `%rsi` 里，也就是 <+36> 的操作，检查后发现格式是 `%d %d %d %d %d %d`，意味着我们需要按照这个格式输入 6 个整数;
3. 剩下的 6 个参数，前 4 个在寄存器 `%rax`、`%rdx`、`%r8`、`%r9` 里，后两个在栈上，就是操作 `%rsp` 的两次；

如果满足这个条件，就能通过这个函数的格式校验，正常返回 `phase_2` 继续执行。

```assembly
(gdb) disas phase_2
Dump of assembler code for function phase_2:
=> 0x0000000000400efc <+0>:	push   %rbp                             
   0x0000000000400efd <+1>:	push   %rbx
   0x0000000000400efe <+2>:	sub    $0x28,%rsp                       ; 1. 分配 40 bytes：2 个寄存器指针 + 6 个 int
   0x0000000000400f02 <+6>:	mov    %rsp,%rsi                        ; 2. 将栈指针（栈顶元素）以第二个参数传给 read_six_numbers 
   0x0000000000400f05 <+9>:	callq  0x40145c <read_six_numbers>      ; 3. 调用 read_six_numbers 获取 6 个数字
   0x0000000000400f0a <+14>:	cmpl   $0x1,(%rsp)                  ; 4. 判断栈顶元素是否为 1
   0x0000000000400f0e <+18>:	je     0x400f30 <phase_2+52>        ; 5. 如果栈顶元素是 1，执行 <+52>
   0x0000000000400f10 <+20>:	callq  0x40143a <explode_bomb>
   0x0000000000400f15 <+25>:	jmp    0x400f30 <phase_2+52>
   0x0000000000400f17 <+27>:	mov    -0x4(%rbx),%eax              ; 9. %rbx - 4 指向当前 %rbx 的前一个元素，赋值给 %rax
   0x0000000000400f1a <+30>:	add    %eax,%eax                    ; 10. 更新 %rax = 2 * %rax
   0x0000000000400f1c <+32>:	cmp    %eax,(%rbx)                  ; 11. 判断 %rbx 是否是它前一个元素的 2 倍
   0x0000000000400f1e <+34>:	je     0x400f25 <phase_2+41>        ; 12. 如果当前位置的元素是前一个元素的 2 倍，则继续执行 <+41>
   0x0000000000400f20 <+36>:	callq  0x40143a <explode_bomb>
   0x0000000000400f25 <+41>:	add    $0x4,%rbx                    ; 13. %rbx + 4，向上移动到上一个数字，即下一个数字
   0x0000000000400f29 <+45>:	cmp    %rbp,%rbx                    ; 14. 和第 6 个数做比较
   0x0000000000400f2c <+48>:	jne    0x400f17 <phase_2+27>        ; 15. 如果当前不是第 6 个数，回到 <+27>，即第 9 步，循环
   0x0000000000400f2e <+50>:	jmp    0x400f3c <phase_2+64>        ; 16. 6 个数字都满足从 1 开始 2 倍递增，跳出循环，执行 <+64>
   0x0000000000400f30 <+52>:	lea    0x4(%rsp),%rbx               ; 6. 取出 %rsp + 4，赋值给 %rbx，第二个参数 2nd
   0x0000000000400f35 <+57>:	lea    0x18(%rsp),%rbp              ; 7. 取出 %rsp + 24，这里是定位到了第六个数字所在的地址
   0x0000000000400f3a <+62>:	jmp    0x400f17 <phase_2+27>        ; 8. 执行 <+27>
   0x0000000000400f3c <+64>:	add    $0x28,%rsp                   ; 17. 回收开辟的内存
   0x0000000000400f40 <+68>:	pop    %rbx
   0x0000000000400f41 <+69>:	pop    %rbp
   0x0000000000400f42 <+70>:	retq                                ; 18. 结束
End of assembler dump.
```

分析汇编代码可知，我们需要输入的是一个从 1 开始，公比为 2 的等比数列，一共 6 个数字，每个数字之间以空格分隔，即：`1 2 4 8 16 32`。

第二个炸弹拆除完成。🎉

### 第三阶段

我们继续看第三阶段，反汇编如下：

```assembly
(gdb) disas phase_3
Dump of assembler code for function phase_3:
=> 0x0000000000400f43 <+0>:	sub    $0x18,%rsp
   0x0000000000400f47 <+4>:	lea    0xc(%rsp),%rcx
   0x0000000000400f4c <+9>:	lea    0x8(%rsp),%rdx
   0x0000000000400f51 <+14>:	mov    $0x4025cf,%esi                       ; %d %d，要求输入两个整数
   0x0000000000400f56 <+19>:	mov    $0x0,%eax
   0x0000000000400f5b <+24>:	callq  0x400bf0 <__isoc99_sscanf@plt>
   0x0000000000400f60 <+29>:	cmp    $0x1,%eax
   0x0000000000400f63 <+32>:	jg     0x400f6a <phase_3+39>
   0x0000000000400f65 <+34>:	callq  0x40143a <explode_bomb>
   0x0000000000400f6a <+39>:	cmpl   $0x7,0x8(%rsp)                       
   0x0000000000400f6f <+44>:	ja     0x400fad <phase_3+106>               ; 如果第一个值大于 7，就引爆炸弹
   0x0000000000400f71 <+46>:	mov    0x8(%rsp),%eax                       ; 取出第一个参数的地址值
   0x0000000000400f75 <+50>:	jmpq   *0x402470(,%rax,8)                   ; 
   0x0000000000400f7c <+57>:	mov    $0xcf,%eax
   0x0000000000400f81 <+62>:	jmp    0x400fbe <phase_3+123>
   0x0000000000400f83 <+64>:	mov    $0x2c3,%eax
   0x0000000000400f88 <+69>:	jmp    0x400fbe <phase_3+123>
   0x0000000000400f8a <+71>:	mov    $0x100,%eax
   0x0000000000400f8f <+76>:	jmp    0x400fbe <phase_3+123>
   0x0000000000400f91 <+78>:	mov    $0x185,%eax
   0x0000000000400f96 <+83>:	jmp    0x400fbe <phase_3+123>
   0x0000000000400f98 <+85>:	mov    $0xce,%eax
   0x0000000000400f9d <+90>:	jmp    0x400fbe <phase_3+123>
   0x0000000000400f9f <+92>:	mov    $0x2aa,%eax
   0x0000000000400fa4 <+97>:	jmp    0x400fbe <phase_3+123>
   0x0000000000400fa6 <+99>:	mov    $0x147,%eax
   0x0000000000400fab <+104>:	jmp    0x400fbe <phase_3+123>
   0x0000000000400fad <+106>:	callq  0x40143a <explode_bomb>
   0x0000000000400fb2 <+111>:	mov    $0x0,%eax
   0x0000000000400fb7 <+116>:	jmp    0x400fbe <phase_3+123>
   0x0000000000400fb9 <+118>:	mov    $0x137,%eax
   0x0000000000400fbe <+123>:	cmp    0xc(%rsp),%eax
   0x0000000000400fc2 <+127>:	je     0x400fc9 <phase_3+134>
   0x0000000000400fc4 <+129>:	callq  0x40143a <explode_bomb>
   0x0000000000400fc9 <+134>:	add    $0x18,%rsp
   0x0000000000400fcd <+138>:	retq   
End of assembler dump.
```

### 第四阶段

(gdb) disas phase_4
Dump of assembler code for function phase_4:
=> 0x000000000040100c <+0>:	sub    $0x18,%rsp
   0x0000000000401010 <+4>:	lea    0xc(%rsp),%rcx
   0x0000000000401015 <+9>:	lea    0x8(%rsp),%rdx
   0x000000000040101a <+14>:	mov    $0x4025cf,%esi
   0x000000000040101f <+19>:	mov    $0x0,%eax
   0x0000000000401024 <+24>:	callq  0x400bf0 <__isoc99_sscanf@plt>
   0x0000000000401029 <+29>:	cmp    $0x2,%eax                        
   0x000000000040102c <+32>:	jne    0x401035 <phase_4+41>            ; 限制参数是两个整数
   0x000000000040102e <+34>:	cmpl   $0xe,0x8(%rsp)                   
   0x0000000000401033 <+39>:	jbe    0x40103a <phase_4+46>            ; 第一个参数需要小于 14
   0x0000000000401035 <+41>:	callq  0x40143a <explode_bomb>
   0x000000000040103a <+46>:	mov    $0xe,%edx                        ; edx = 14，第三个参数
   0x000000000040103f <+51>:	mov    $0x0,%esi                        ; esi = 0，第二个参数
   0x0000000000401044 <+56>:	mov    0x8(%rsp),%edi                   ; 第一个输入放到 edi，第一个参数
   0x0000000000401048 <+60>:	callq  0x400fce <func4>                 ; 用上面三个值调用 func4(x, 0, 14)
   0x000000000040104d <+65>:	test   %eax,%eax                        ; 返回值不为 0
   0x000000000040104f <+67>:	jne    0x401058 <phase_4+76>
   0x0000000000401051 <+69>:	cmpl   $0x0,0xc(%rsp)                   ; 判断第二个输入是否为 0
   0x0000000000401056 <+74>:	je     0x40105d <phase_4+81>            ; 等于 0 就结束
   0x0000000000401058 <+76>:	callq  0x40143a <explode_bomb>
   0x000000000040105d <+81>:	add    $0x18,%rsp
   0x0000000000401061 <+85>:	retq   
End of assembler dump.
(gdb) disas func4

func4(int x, int y, int z)
x = rdi, y = rsi, z = rdx
x 未知，y = 0，z = 14

Dump of assembler code for function func4:
   0x0000000000400fce <+0>:	sub    $0x8,%rsp
   0x0000000000400fd2 <+4>:	mov    %edx,%eax                    
   0x0000000000400fd4 <+6>:	sub    %esi,%eax                    ; m = z - y = 第三个参数 - 第二个参数 = 14
   0x0000000000400fd6 <+8>:	mov    %eax,%ecx                    ; n = m >> 31，n 是 m 的符号位，0 或者 1
   0x0000000000400fd8 <+10>:	shr    $0x1f,%ecx                
   0x0000000000400fdb <+13>:	add    %ecx,%eax                  ; rax = 上面两个值加一起，然后做算术右移 = 7
   0x0000000000400fdd <+15>:	sar    %eax                       ; 
   0x0000000000400fdf <+17>:	lea    (%rax,%rsi,1),%ecx         ; %rcx = 上面的运算结果 + 第二个参数 = 7
   0x0000000000400fe2 <+20>:	cmp    %edi,%ecx                  ; 比较运算结果是否小于传入的值
   0x0000000000400fe4 <+22>:	jle    0x400ff2 <func4+36>        ; if n < x --- rdi 是输出的第一个值
   0x0000000000400fe6 <+24>:	lea    -0x1(%rcx),%edx            ; 如果大于等于，就给新值 -1，当作第三个参数传入，递归，第二个参数还是 0
   0x0000000000400fe9 <+27>:	callq  0x400fce <func4>           ; 递归调用 func4(x, ,--z)
   0x0000000000400fee <+32>:	add    %eax,%eax                
   0x0000000000400ff0 <+34>:	jmp    0x401007 <func4+57>        ; 无条件跳转：rax *= 2
   0x0000000000400ff2 <+36>:	mov    $0x0,%eax                  ; 如果输入的值小于计算的值，就：1. 清空 rax
   0x0000000000400ff7 <+41>:	cmp    %edi,%ecx                  ; 再比较 输入值和 ecx 的大小
   0x0000000000400ff9 <+43>:	jge    0x401007 <func4+57>        ; 如果大于等于，就 return 0 -------- 这个判断主要用来清空 rax 返回 0 用
   0x0000000000400ffb <+45>:	lea    0x1(%rcx),%esi             ; 否则 更新第二个参数为 rcx + 1，递归调用 func
   0x0000000000400ffe <+48>:	callq  0x400fce <func4>           ; func(rdi, rcx+1, rdx)
   0x0000000000401003 <+53>:	lea    0x1(%rax,%rax,1),%eax      ; 什么时候调用？ 2 * rax + 1 
   0x0000000000401007 <+57>:	add    $0x8,%rsp
   0x000000000040100b <+61>:	retq   
End of assembler dump.

用 0 和 14 操作，算出 x