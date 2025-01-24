void test() {

    int x;
    float f;
    double d;

    // true
    // double 精度比 int 高，所以转回来的时候 x 的值不会变，也不会丢失精度
    int a = x == (int)(double)x;

    // false
    // float 小数位只能存 23 位，x 转换为 float 会因为截断和舍入导致不再是原来的值
    // float 最大能表示到 2^{24}（约 16777216)，因为有一个隐含 1 不需要表示，所以能表示 24 位
    // 对于 x 在 [-2^{23}, 2^{23}] 以内，表达式恒成立。
    // 对于 x 超过 2^{24}（约 16777216），float 无法精确表示整数，可能会导致 x != (int)(float)x。
    int b = x == (int)(float)x;

    // false
    // 1. double 的精度更高，转 float 后会丢失精度
    // 2. 如果 d 的值过大，转为 float 之后仍然可能超出 float 可以表达的范围，最后的结果会变成无穷大
    int c = d == (double)(float)d;

    // true
    // double 精度比 float 高，来回转换不会丢失信息
    int d = f == (float)(double)f;

    // true
    // 浮点数的取非就是简单对它的符号位取反，不涉及其他位的变化
    int e = f == -(-f);

    // true
    // 在执行除法之前，分子和分母都会被转换为浮点数表示
    int f = 1.0 /2 == 1 / 2.0;

    // true
    // 浮点数不会溢出，超出表示范围后会变成无穷大，仍然满足条件
    int g = d * d >= 0;

    // false
    // 因为舍入的问题，如果 f 的值很大而 d 的值很小， f+d 会把 d 的内容舍弃掉，导致等式左侧变成 0
    int h = (f + d) - f == d;
}