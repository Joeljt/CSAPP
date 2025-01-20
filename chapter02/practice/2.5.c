/**
 * 内存是连续的地址空间。
 * 
 * 当我们得到某个变量的地址值时，拿到的一般是该变量的第一个地址信de
 * 比如，假设 x = 0x12345678，&x = 0x100，实际上内存中的布局类似这样：
 * 
 *  | 12    | 34    | 56    | 78    |
 *  | 0x100 | 0x101 | 0x102 | 0x103 |
 * 
 * 每一个地址都存储了 1 个字节，即 8 位，一个 int 类型的值占据 4 个字节，也就是上面表示的这样。
 * 当我们访问 &x 时，实际上是访问了 0x100 这个地址，再搭配上 int 类型占 4 个字节，所以我们知道向后取几个位置从而得到 x 的值。
 * 
 * 实际上，在不同的机器上，内存的布局方式可能不同。
 * 
 * 像上面这种表示的方法，高位的地址存放低位的值，叫做大端法（big endian）。
 * 
 * 比如，0x12 是最高位，但是存放在了 0x100 这个最低位上，数字的排列与真实内存地址的排列是相反的，即：
 * 
 * | 12    | 34    | 56    | 78    |
 * | 0x100 | 0x101 | 0x102 | 0x103 |
 * 
 * 除此之外，还有一种内存布局方式，叫做小端法（little endian），即低地址存放最低有效字节。
 * 
 * | 78    | 56    | 34    | 12    |
 * | 0x100 | 0x101 | 0x102 | 0x103 |
 * 
 * 0x78 是最低位，存放在 0x100 这个最低地址上。安卓和 iOS 系统都使用小端法。
 * 
 * 在网络传输时，一般需要将数据转换为大端法，否则会出现错误。
 */

#include <stdio.h>
typedef unsigned char *byte_pointer;

void show_bytes(byte_pointer start, size_t len) {
    size_t i;
    for (i=0;i<len;i++) printf(" %.2x", start[i]);
    printf("\n");
}

void show_int(int x) {
    show_bytes((byte_pointer) &x, sizeof(int));
}

void show_float(float x) {
    show_bytes((byte_pointer) &x, sizeof(float));
}

void show_pointer(void *x) {
    show_bytes((byte_pointer) &x, sizeof(void *));
}

void test_show_bytes(int val) {
    int ival = val;
    float fval = (float) ival;
    int *pval = &ival;
    show_int(ival);
    show_float(fval);
    show_pointer(pval);
}

/**
 * show_bytes 方法从最低地址值开始打印指定位数
 * 
 * 小端法下，低地址存放低位，高地址存放高位，对于 0x12345678 来说，
 * 打印 1 位时，打印 78
 * 打印 2 位时，打印 56 78
 * 打印 3 位时，打印 34 56 78
 * 
 * 大端法下，低地址存放高位，高地址存放低位，
 * 打印 1 位时，打印 12
 * 打印 2 位时，打印 12 34
 * 打印 3 位时，打印 12 34 56
 */
void test_2_5() {
    int a = 0x12345678;
    byte_pointer ap = (byte_pointer) &a;
    show_bytes(ap, 1); /* A. */
    show_bytes(ap, 2); /* B. */
    show_bytes(ap, 3); /* C. */
}

int main() {
    // int x = 12345;
    // test_show_bytes(x);
    return 0;
}