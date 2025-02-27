/**
 * Determine the number of page table entries (PTEs) that are needed 
 * for the following combinations of virtual address size (n) and page size (P ):
 
    n       P = 2^p         Number of PTEs
    12        1K                
    16        16K 
    24        2M 
    36        1G

    如果地址是 n 位，页表的大小是 P，求每页有多少项？

    以第一个为例，
    1. 位数是 12，可寻址空间为 2^12；
    2. 页表大小是 1024，即 2^10，即一共有 2^12 / 2^10 = 2^2 = 4 页；

    但是题目让求每页有多少项？

 */