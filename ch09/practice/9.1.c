/**
 * Complete the following table, filling in the missing entries and replacing each question mark with the appropriate integer. 
 * Use the following units: 
 * K = 2^10 (kilo), M = 2^20 (mega), G = 2^30 (giga), T = 2^40 (tera), P = 2^50 (peta), or E = 2^60 (exa).
 * 
 * Number of virtual address bits (n)          Number of virtual addresses (N)         Largest possible virtual address
 *          4                                                                    
 *                                                   2 ^ ? = 16K
 *                                                                                               2^24 - 1 = ?M - 1
 *                                                   2 ^ ? = 64T
 *          54
 * 
 * 
 * 地址位数、地址空间大小、最大地址的转换，基础训练，对地址值形成一些基础认知。
 * 
 * 1. 如果地址值为 4 位，则可寻址空间为 2^4=16，可表示的最大地址为 2^4-1=15。
 * 2. 如果可寻址空间为 16K，即 16 * 2^10 = 2^14，则地址值是 14 位，可表示的最大地址为 2^14 - 1 = 16383。
 * 3. 如果最大地址值是 2^24 - 1，那就是地址位数是 24 位，可寻址空间是 2^24，2^24 转换为 M 是 2^4 + 2^20，即 16M - 1。
 * 4. 64T = 2^6 * 2^40，所以位数是 46 位，最大地址是 2^46 - 1 = 64T - 1。
 * 5. 和第一个相同，可寻址空间为 2^54，最大地址值是 2^54 - 1。
 * 
 */