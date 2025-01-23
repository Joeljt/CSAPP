#include "stdio.h"

// ------------------- 2.25 -------------------
// 这里 length 是无符号数，所以 length - 1 会变成一个很大的数，导致 for 循环无限循环
// 修改方法：
// 1. 将 length 转换为有符号数，或者，
// 2. 将条件改成 i < length
float sum_elements(float a[], unsigned length) {
    int i;
    float result = 0;

    for (i = 0; i <= length - 1; i++) {
        result += a[i];
    }
    return result;
}

// ------------------- 2.26 -------------------
// size_t 是无符号数，所以 strlen(s) - strlen(t) 会变成一个很大的数，导致 strlonger 返回 1
// 修改方法：
// strlen(s) > strlen(t)
size_t strlen(const char *s);
int strlonger(char *s, char *t) {
    return strlen(s) - strlen(t) > 0;
}

// ------------------- FreeBSD getpeername -------------------
// maxlen 定义为 int 值，但是 memcpy 接收 size_t，导致如果传入的 maxlen < 0，
//就会给 memcpy 传入一个很大的数，然后导致访问到未授权的内存空间
// 修改方法：
// 1. 将 maxlen 定义为 size_t
// 2. 将 copy_from_kernel 的 len 和返回值都改为 size_t
void *memcpy(void *dest, const void *src, size_t n);

#define KSIZE 1024
char kbuf[KSIZE];

int copy_from_kernel(void *user_dest, int maxlen) {
    int len = KSIZE < maxlen ? KSIZE : maxlen;
    memcpy(user_dest, kbuf, len);
    return len;
}