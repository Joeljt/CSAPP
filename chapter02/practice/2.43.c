/**
 * 2.43 求 M 和 N 的值
 * 
 * 编译器使用移位和加法来优化乘法，增加偏移量来保证除法的取整正确。
 * 
 * 1. 对于 M
 * x << 5 意味着 x * 2^5，x - t 意味着 - x*2^0
 * 也就是 2^5 - 2^0 = 32 - 1 = 31
 * 
 * 2. 对于 N
 * y < 0 的情况下，增加偏移量 7，偏移量 = 分母 - 1
 * 也就是 7 = 8 - 1
 * y >> 3 意味着 y / 2^3 = y / 8，也验证了分母是 8
 */

#define M -1
#define N -1

int arith(int x, int y) {
    int result = x * M + y * N;
    return result;
}

int optarith(int x, int y) {
    int t = x;
    x <<= 5;
    x -= t;
    if (y < 0) y += 7;
    y >>= 3; /* 算术右移 */
    return x+y;
}