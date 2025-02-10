## Solution

bomblab 提供了一个二进制文件 bomb，执行后提示输入内容，如果输出错误程序就会爆炸。

需要利用 gdb 进行 debug，反编译破解二进制文件，通过查看汇编指令还原程序逻辑，确定需要输入的内容是什么。

程序一共有 6 个阶段，难度系数按阶段增加。

### 第一阶段

执行 bomb 程序后设置断点，反编译 `phase_1` 方法如下：

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

继续