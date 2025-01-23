
int foo() {
    return -1;
}

int bar() {
    return 0;
}

void test() {
    int x = foo();
    int y = bar();

    unsigned ux = (unsigned) x;
    unsigned uy = (unsigned) y;

    // false
    // 如果 x 等于 TMin，则 x - 1 会溢出为 TMax，所以 x - 1 < 0 为 false
    int a = (x > 0) || (x-1 < 0);

    // true
    // 如果 x 不以 0x0111 结尾，则 (x & 7) != 7 一定成立
    // 如果 x 以 0x111 结尾，则 x << 29 以后是 111，一定是负数，(x << 29 < 0) 一定成立
    int b = (x & 7 ) != 7 || (x << 29 < 0);

    // false
    // 对于 x 比较大的情况，x * x 会溢出，有可能是负数
    int c = x * x >= 0;

    // true
    // 这两个条件一定有一个成立
    int d = x < 0 || -x <= 0;

    // false
    // 如果 x 是 TMin，即 -2147483648，-x 也是 TMin，同样小于 0
    int e = x > 0 || -x >= 0;

    // true
    // 有符号与无符号相比较的时候，会统一转成无符号值，所以成立
    int f = x + y == ux + uy;

    // true
    // -n = ~n + 1，所以 ~y = -y - 1
    // x * (-y-1) + ux * uy == -x
    // -x*y - x + ux * uy == -x
    // 又有，有符号与无符号相比较的时候，会统一转成无符号值，所以 x * y == ux * uy
    // -x == -x
    int g = x * ~y + ux * uy == -x;

}

