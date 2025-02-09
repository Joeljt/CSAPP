## 动态申请内存的调用栈

正常情况下，如果能够在编译期确定一个函数内部都声明了哪些变量，那对于该函数，其调用栈中局部变量的内存占用是可以直接计算出来的。

然后，编译器可以优化汇编代码，通过下移 `%rsp` 指针确定的位数一次性分配好需要的内存空间，然后在该空间内放置变量即可。

但是，如果栈中的变量是动态的，比如根据传入的某个参数动态创建数组，那就需要利用 `%rbp` 和 `%rsp` 的配合来管理这些动态分配的局部变量。

### %rbp

`%rbp` 的语义是 frame pointer 或者 base pointer，其工作原理是：

1. 方法开始执行时，先保存上一个栈帧的 `%rbp` 值，然后将当前的 `%rsp` 值赋给 `%rbp`，作为新栈帧的基准地址；
2. 在整个函数执行期间，`%rbp` 保持不变，所有局部变量都通过相对于 `%rbp` 的固定偏移量来访问；
3. `%rsp` 则会随着动态内存分配的需求不断调整，在栈上分配所需的空间；
4. 函数返回前，通过 `leave` 指令恢复 `%rsp` 和 `%rbp` 的值，清理当前栈帧；

注意 `%rbp` 和 `%rsp` 各司其职：`%rbp` 提供稳定的参考点用于访问局部变量，而 `%rsp` 负责动态管理栈空间。

同时，`%rbp` 是 callee-saved 寄存器，所以在使用该寄存器时需要在函数开始时保存其值，最后执行完毕后将值还原。

以下面的 C 代码为例：

```c
long vframe(long n, long idx, long *q)  {
    long i;
    long *p[n];
    p[0] = &i;
    for (i = 1; i < n; i++)
        p[i] = q;
    return *p[idx];
}
```

这个代码利用传入的参数申请了一个动态大小的数组，同时对该数组进行了一些操作。

上述代码转换为汇编如下：

```assembly
vframe:
  pushq   %rbp                  ; 保存上一个栈帧的 base pointer
  movq    %rsp, %rbp            ; 设置新的 base pointer，建立新栈帧的基准点
  subq    $16, %rsp             ; 分配固定大小的栈空间
  leaq    22(,%rdi,8), %rax     ; 计算动态数组需要的空间：8*n + 22
  andq    $-16, %rax            ; 内存对齐
  subq    %rax, %rsp            ; 为动态数组分配栈空间
  leaq    7(%rsp), %rax         
  shrq    $3, %rax
  leaq    0(,%rax,8), %r8   
  movq    %r8, %rcx

...

.L3:                            ; 循环开始
  movq    %rdx, (%rcx,%rax,8)   ; 存储值到数组：p[i] = q
  addq    $1, %rax              ; i++
  movq    %rax, -8(%rbp)        ; 将计数器 i 存储到内存里以免 %rax 在其他地方被覆盖
.L2:
  movq    -8(%rbp), %rax        ; 加载计数器 i 的值
  cmpq    %rdi, %rax            ; 比较 i 和 n
  jl      .L3                   ; 如果 i < n，继续循环

...

  leave                         ; 清理栈帧：
                                ; 1. movq %rbp, %rsp (恢复栈指针)
                                ; 2. popq %rbp (恢复上一个栈帧的 base pointer)
  ret
```

### `%rsp` vs `%rbp`

实际上，在某些情况下确实可以只用 `%rsp` 而不使用 `%rbp`。这就是所谓的"frame pointer omission"（FPO）或"frame pointer elimination"优化。

但使用 `%rbp` 有几个重要优势：

1. `%rsp` 值会变化，而 `%rbp` 不会变

由于所有程序共享 `%rsp`，所以在涉及到函数调用的时候，有可能发现 `%rsp` 的值会变化，导致取不到原来的值。

```text
foo:
    # 假设只用 rsp
    subq    $24, %rsp          ; 分配栈空间
    movq    %rax, (%rsp)       ; 存储值
    
    # 如果这里调用其他函数
    push    %rax               ; rsp 改变了
    call    bar               ; rsp 又改变了
    pop     %rax               ; rsp 恢复
    
    # 现在要访问之前存的值
    movq    (%rsp), %rax       ; ❌ 错误！偏移量变了
    movq    8(%rsp), %rax      ; ✅ 需要重新计算正确的偏移量
```

而使用 `%rbp` 的话，无论 `%rsp` 如何变化，都可以用固定偏移量访问参数和局部变量：

```text
foo:
    # 使用 rbp
    pushq   %rbp
    movq    %rsp, %rbp
    
    # 动态分配栈空间
    movq    %rdi, %rax         ; n
    salq    $3, %rax           ; n * 8
    subq    %rax, %rsp         ; 分配 n*8 字节
    
    # 无论 rsp 如何变化，都可以用固定偏移量访问参数和局部变量
    movq    16(%rbp), %rax     ; 访问参数
    movq    -8(%rbp), %rax     ; 访问局部变量
```

2. 调试和栈回溯：

`%rbp` 形成了栈帧的链表：

```text
  栈顶
  frame3: [saved rbp] -> frame2
  frame2: [saved rbp] -> frame1
  frame1: [saved rbp] -> frame0
  栈底
```  

这使得调试器可以轻松地遍历调用栈，准确定位每个函数的局部变量，显示完整的调用链。

此外，在处理异常时需要回溯栈，生成的代码在有 `%rbp` 的时候也会更简化：

```text
# 使用 rbp
movq    -8(%rbp), %rax     ; 固定偏移量，编译时就能确定

# 只用 rsp
movq    24(%rsp), %rax     ; 偏移量需要动态计算
                           ; 要考虑当前栈深度
```

所以，虽然理论上可以只用 `%rsp` 完成动态内存的分配工作，但是使用 `%rbp` 可以简化编译器工作，提供更稳定的引用点，支持更好的调试体验，并且使异常处理更可靠。
