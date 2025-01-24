#define POS_INFINITY -1
#define NEG_INFINITY -1
#define NEG_ZERO -1

/**
 * 完成上述宏定义，已知双精度能够表示的最大的有限数大概是 1.8*10^308
 * 
 * 1. POS_INFINITY
 * 直接定义一个比这个还大的数，让 64 位浮点型的 exp 无法容纳，全部溢出为 1，并且确保 frac 全部为 0 即可
 * POS_INFINITY = 10^400 = 1e400
 * 
 * 2. NEG_INFINITY
 * 负无穷与正无穷的区别仅在于符号位，所以直接取反即可 -1e400
 * 
 * 3. NEG_ZERO
 * 负零的二进制是符号位是 1，剩下的所有值都是 0：
 * - 保证是负数
 * - exp 全为 0，确保数字小于 1，让 M = f，没有隐含 1 即可；
 * - frac 全为 0，也就是 0，所以可以简单写成 NEG_ZERO = -0.0 即可
 * 
 * 或者，IEEE 定义了规则：-1.0 / +∞ 按照 IEEE 754 规则等于 -0.0，直接写成 -1.0 / POS_INFINITY 也可以
 * 
 * IEEE 规定，参数中有一个是特殊值的时候（-0，-∞，NaN）结果都是 ∞
 */
