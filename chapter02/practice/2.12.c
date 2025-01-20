/**
 * 使用布尔代数做掩码运算：
 * 1. 通过对 0 取反得到全为 1 的掩码；
 * 2. 通过对掩码求与，可以屏蔽掉某些位；
 * 
 * 比如 0x12345678 & 0xFF 可以屏蔽掉高 24 位，只保留低 8 位，得到 0x00000078。
 * 
 * ------------------------------------------------------------
 * 对于 x = 0x87654321, with w = 32.
 * 
 * A. The least significant byte of x, with all other bits set to 0. [0x00000021]
 *  抹掉高 24 位，保留低 8 位，对 0xFF 求与即可：x & 0xFF
 * 
 * B. All but the least significant byte of x complemented, with the least significant byte left unchanged. [0x789ABC21]
 *  低八位保持不动，其余位求补：x ^ ~0xFF
 * 
 * C. The least significant byte set to all ones, and all other bytes of x left un- changed. [0x876543FF]
 *  保留高 24 位，覆盖低 8 位为 1，对 0xFF 求或即可：x | 0xFF
 * 
 */


