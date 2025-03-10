## 循环语句在汇编中的表示方法

无论什么语言，基本都会提供 `do-while`、`while` 和`for` 三种循环，其中：

- `do-while` 需要先执行一次循环体再检查是否开启循环，所以和另外两个不太一样；
- `for` 和 `while` 在本质上是等价的，通过简单改写代码可以把任意一种循环改成另一种方式实现；

但是从机器层面来看，实际上是不存在循环指令的。所有的循环都是**条件判断**搭配**跳转**来实现的。

实际上，我们可以在 C 中利用 `goto` 关键字完全模拟 jump 指令，再搭配上 if 条件判断，从而完全模拟汇编代码，了解 CPU 是怎么执行循环代码的。

### do-while

do-while 的执行模式用 `goto` 来表示的话，如下所示：

```text
loop:
    循环体代码
    t = 某个判断条件
    if (t)
        goto loop;
```

这样，代码开始执行的时候会优先执行一次循环体代码，然后再对条件进行判断，如果满足条件就跳转回循环开始的位置继续执行，直到循环条件不再满足。

以求阶乘为例，考虑 C 代码如下：

```c
long fact_do(long n) {
    long result = 1;
    do {
        result *= n;
        n = n - 1;
    } while (n > 1);
    return result;
}
```

转换为 `goto` 代码可以得到：

```c
long fact_goto(long n) {
    long result = 1;
 loop:
    result *= n;
    n = n - 1;
    if (n > 1)
        goto loop;
    return result;
}
```

编译为汇编以后得到的代码与 `goto` 代码基本一致：

```gas
long fact_do(long n)
n in %rdi

fact_do:
    movl    $1, %eax    ; result = 1
  .L2:
    imulq   %rdi, %rax  ; result *= n
    subq    $1, %rdi    ; n = n - 1
    cmpq    $1, %rdi    ; n > 1 ?
    jg      .L2         ; if >，goto loop
    rep; ret            ; 否则 return
```

### while / for

首先几乎所有的 for 循环都可以用 while 循环来实现，反之亦然。所以这里放在一起讨论。

对于 while 或 for 循环来说，需要首先判断条件是否满足，有可能完全不需要执行循环体。

编译器有两种方式来处理这种循环的实现逻辑：jump to middle 和 guarded-do。

#### jump to middle

这种循环在汇编场景下对比 do-while 比较麻烦的地方在于，判断条件会在两个地方同时使用到：

- 代码刚开始执行的时候，需要执行一次判断确定是否开启循环，如果需要就跳转到循环开始的位置；
- 循环过程中判断是否需要继续循环，如果需要就跳转到循环开始的位置；

我们可以把条件判断相关的代码单独封装到一个 label 下，然后把该代码放在 loop 中，但是在程序运行之初，强制 `goto` 到条件判断中：

- 如果需要开启循环，就会跳转到循环开始的位置开始执行；
- 如果不需要开启循环，就会略过所有循环指令，继续向后执行。

```text
    goto test;
 loop:
    循环体
 test:
    if (某些判断条件)
        goto loop;
```

还以求阶乘为例：

```c
long fact_while(long n){
    long result = 1;
    while (n > 1) {
        result *= n;
        n = n-1; 
    }
    return result;
}
```

用 `goto` 实现为：

```c
long fact_while_jm_goto(long n) {
    long result = 1;
    goto test;
 loop:
    result *= n;
    n = n-1; 
 test:
    if (n > 1)
        goto loop;
    return result;
}
```

编译为汇编后的代码为：

```assemble
long fact_while(long n)
n in %rdi

fact_while:
  movl    $1, %eax      ; result = 1
  jmp     .L5           ; goto test
.L6:
  imulq   %rdi, %rax    ; result *= n
  subq    $1, %rdi      ; n = n - 1
.L5:
  cmpq    $1, %rdi      ; n > 1 ?
  jg      .L6           ; if >, goto loop
  rep; ret              ; return
```

正如这个方法的名称所示，这个实现循环的方式为：在程序最开始，强制跳转到程序中间，从循环内部的条件判断开始执行代码。

#### guarded-do

jump to middle 是将 if 判断单独抽离成一个 label，在最开始强制跳转，

guarded-do 是将循环改写为类似 do-while 的形式实现的：

1. 先计算条件判断的结果（改写后的 do 部分）；
2. 再判断该结果，如果不满足条件，直接跳转到结束 label（guarded 的部分）；
3. 如果满足条件，就进入循环，循环内部还有条件判断以及 goto loop 的逻辑，和 do-while 一样；
4. 直接循环不满足后，自然执行到结束 label；

```text
    if (!判断条件)
        goto done;
 loop:
    循环体
    if (判断条件)
        goto loop;
 done:
    结束位置
```

以阶乘为例，这个版本的 goto 代码如下：

```c
long fact_while_gd_goto(long n) {
    long result = 1;
    if (n <= 1)
        goto done;
 loop:
    result *= n;
    n = n-1;
    if (n != 1)
        goto loop;
 done:
    return result;
}
```

相应的汇编代码如下：

```assembly
fact_while:
    cmpb    $1, %rdi        ; Compare n:1
    jle     .L7             ; If <=, goto done 
    movl    $1, %eax        ; Set result = 1

.L6:                        ; loop:
    imulq   %rdi, %rax      ; result *= n 
    subq    $1, %rdi        ; n = n - 1
    cmpq    $1, %rdi        ; Compare n:1
    jne     .L6             ; if !=, goto loop:
    rep; ret                ; return

.L7:                        ; done:
    movl $1, %eax           ; result = 1
    ret                     ; return
```

### 如何决定用哪种方式？

编译器在决定如何实现一个循环时，通常会考虑多个因素，包括循环的结构、循环体的大小、编译优化级别等。

- 简单循环条件： 如果循环的条件非常简单（比如只有一个计数器），编译器可能会选择 `jump to middle` 的实现方式。这种方式对于简单的计数器控制循环非常高效。
- 复杂条件判断： 如果循环条件较为复杂，可能涉及多个变量或条件判断，编译器可能会选择 `guarded-do` 方式，这样可以确保每次循环体都会执行，然后再进行判断。

除了这两种方式以外，如果循环很简单，编译器可能还会将循环完全展开为普通的赋值操作。