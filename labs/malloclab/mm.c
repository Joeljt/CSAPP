/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ohno",
    /* First member's full name */
    "Joseph",
    /* First member's email address */
    "noway",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""};

#define WSIZE 4
#define DSIZE 8

// 内存不足时向系统申请的内存大小
#define CHUNKSIZE (1 << 12)

#define MAX(x, y) ((x) > (y) ? (x) : (y))

// 将内存块大小和分配信息打包到一起，放到 header/footer 里
// 这里 size 是向上取整过的，末尾 3 位都是 0，size | isAllocated 是可以正常工作的
#define PACK(size, isAllocted) ((size) | (isAllocted))

// 对给定地址的取值和赋值操作
// 传入的一般可能是 void* 的泛型指针，在使用的时候需要转换成具体的类型才能解引用
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

// 从给定的地址（header 或者 footer）中取出存储的 block size 以及分配信息
#define GET_SIZE(p) (GET(p) & ~0x7))
#define GET_ALLOC(p) (GET(p) & 0x1)

// 下面操作的都是 block pointer，即跳过 header 字后返回给用户的地址值
// 1. 存储的 block size = header(WSIZE) + payload + footer(WSIZE);
// 2. 操作 bp 的时候需要先转为 char* 才能进行算数运算
// 用户得到的地址值减去一个字长就是 header 所在的位置
#define HDRP(bp) ((char *)(bp) - WSIZE)
// 从 bp 开始，加上 block size，再减去 header+footer 的双字偏移，即为 footer 地址
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

// 找到相邻块的地址信息
// 后一个相邻块地址：当前位置 + 当前块的 size 信息即可
// 正好可以把当前块的 footer 和下一个块的 header 跳过去
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
// 前一个相邻块地址 
// 1. 当前位置向前移动双字，找到前一个块的 footer 后取出前一个块的大小；
// 2. 当前位置减去这个大小就正好可以得到前一个内存块的位置；
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE))

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
}
