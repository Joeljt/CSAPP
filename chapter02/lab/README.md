## data lab

datalab 主要考察数字在计算机中的表示以及对位运算的理解和应用。

一共包含 13 道题目，这里记录一下每道题的解法以及涉及到的知识点；同时，我认为官方的难度排名不合理，这里重新调整一下难度顺序。

### bitXor

只使用 ~ 和 & 操作符，在不超过 14 次调用的情况下实现 ^ 操作。

```c
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  // return ~(~(x & ~y) & ~(~x & y));
  return ~(x & y) & ~(~x & ~y);
}
```

### tmin

最简单的一道题，1 左移 31 位就可以把 1 移动到符号位，后面 31 位补 0，最后得到的结果就是 TMin。

```c
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return 1 << 31;
}
```

### isTmax

根据这个题目，对于整数的表示，形成一些条件反射式的认识。

- TMax + 1 会溢出成 TMin；
- 任何数字都有其相反数使之相加等于 0，TMin 的相反数是它本身；
- 异或操作的自反性，任何数和其自身做异或，结果都是 0；
- 使用 !! 符号可以将任意值转变为 0 或 1 的逻辑判断；
- & 除了按位求与之外，还可以用来做最简单的逻辑判断，是 && 的底层实现；

```c
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
    // return !!x & !!(x + 1) & !(x & (x + 1)) & !((x ^ (x + 1)) + 1);
    return !(~x ^ (x + 1)) & !!(x + 1);
}
```

### allOddBits

根据这个题目学会如何构建掩码以及判断逻辑。

```c
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  int mask = (0xAA << 8) | 0xAA;
  mask = (mask << 16) | mask;
  return !((x & mask) ^ mask);
}
```

### negate

简单题。一个数字的负数表示就是 two's complement，也就是按位取反后 +1。

```c
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x + 1;
}
```

### isAsciiDigit

```c
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  int a = !((x >> 4) ^ 0x3);

  int last = x & 0xf;
  // int b = !(last >> 3);
  // int c = !(last ^ 0x8) | !(last ^ 0x9);
  
  // 0-9 的数字，减 10 小于 0，算术右移 31 位，结果是 -1（32 个 1）
  // a-f > 10，逻辑右移 31 位，结果是 0（32 个 0）
  // 这里 >10 的结果全是 0，刚好可以表示非
  int d = (last + (~10 + 1)) >> 31; 

  // return a & (b | c);
  return a & d;
}
```

### conditional

```c
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  return 2;
}
```

### isLessOrEqual

```c
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  return 2;
}
```

### logicalNeg

```c
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  return 2;
}
```

### howManyBits

```c
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  return 0;
}
```

### floatScale2

```c
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  return 2;
}
```

### floatFloat2Int

```c
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  return 2;
}
```

### floatPower2

```c
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
    return 2;
}
```

