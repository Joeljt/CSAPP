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

#define ALIGNMENT DSIZE
#define ALIGN(size) ((size + (ALIGNMENT - 1)) & (~(ALIGNMENT - 1)))

// 将内存块大小和分配信息打包到一起，放到 header/footer 里
// 这里 size 是向上取整过的，末尾 3 位都是 0，size | isAllocated 是可以正常工作的
#define PACK(size, isAllocted) ((size) | (isAllocted))

// 对给定地址的取值和赋值操作
// 传入的一般可能是 void* 的泛型指针，在使用的时候需要转换成具体的类型才能解引用
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

// 从给定的地址（header 或者 footer）中取出存储的 block size 以及分配信息
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

// 下面操作的都是 block pointer，即跳过 header 字后返回给用户的地址值
// 1. 存储的 block size = header(WSIZE) + payload + footer(WSIZE);
// 2. 操作 bp 的时候需要先转为 char* 才能进行算数运算
// 用户得到的地址值减去一个字长就是 header 所在的位置
#define HDRP(bp) ((char *)(bp) - WSIZE)
// 从 bp 开始，加上 block size，再减去 header+footer 的双字偏移，即为 footer 地址
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp) - DSIZE))

// 找到相邻块的地址信息
// 后一个相邻块地址：当前位置 + 当前块的 size 信息即可
// 正好可以把当前块的 footer 和下一个块的 header 跳过去
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
// 前一个相邻块地址
// 1. 当前位置向前移动双字，找到前一个块的 footer 后取出前一个块的大小；
// 2. 当前位置减去这个大小就正好可以得到前一个内存块的位置；
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE))

// 指向首个内存块的指针
static char *heap_listp = 0;

static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void printblock(void *bp);
static void checkheap(int verbose);
static void checkblock(void *bp);

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    // 先分配 4 个字大小，用来存储 prologue(header + footer) 和 epilogue(end flag)
    // 多出来一个用来对齐 8 字节
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;

    // 填充字，用来确保 8 字节对齐
    PUT(heap_listp, 0);

    // 在 header 和 footer 中存储当前块的大小信息，并标记其已经被分配
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));

    // 在末尾增加一个 epilogue，大小为 0，allocated 为 true
    // 标记可用内存空间的末尾
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));

    // 将指针指向 header 和 footer 中间，也就是 block pointer 的位置
    heap_listp += (2 * WSIZE);

    // 为当前可用内存空间再分配 CHUNKSIZE 大小
    if(extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;

    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    // 边界检查
    if (size == 0)
    {
        return NULL;
    }
    else if (heap_listp == 0)
    {
        mm_init();
    }

    // size_t adjustSize;
    size_t extendSize;
    char *bp;

    // 无论 size 是否超过 8 字节，都会被向上取整到最近的 8 字节倍数
    // +DSIZE 是 header + footer 的额外开销
    size = ALIGN(size + DSIZE);

    // 在整个数组中查找空闲位置，并将找到的空间返回给申请者
    if ((bp = find_fit(size)) != NULL)
    {
        place(bp, size);
        return bp;
    }

    // 如果没有找到足够使用的空间，就向内核申请再开辟一块新地址
    extendSize = MAX(size, CHUNKSIZE);
    if ((bp = extend_heap(extendSize / WSIZE)) != NULL) {
        place(bp, size);
        return bp;
    }

    return NULL;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    // 边界检查
    if (bp == 0)
    {
        return;
    }
    else if (heap_listp == 0)
    {
        mm_init();
    }

    // 从 header 里取出当前块大小
    // 简单地将 header 和 footer 里的 allocate 标记为 0 即可
    // 标记完成后，与前后相邻的内存块做合并
    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
}

static void *find_fit(size_t asize)
{
    void *bp;
    // 从 heap_listp 开始遍历所有内存块，直到 epilogue 结束
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
    {
        // first fit
        // 找到第一个未分配并且大小满足目标申请大小的可用内存块
        if (!GET_ALLOC(HDRP(bp)) && GET_SIZE(HDRP(bp)) >= asize) {
            // 找到满足条件的内存块以后，返回其指针
            return bp;
        }
    }
    return NULL;
}

static void place(void *bp, size_t asize) 
{
    // 获取当前内存块的 size 信息
    size_t csize = GET_SIZE(HDRP(bp));

    // 如果内存块的空间比申请的空间要大，并且剩余的空间满足最小空间要求
    // 最小空间要求：header + footer + 一个 8 字节的块 = 2 * DSIZE
    if ((csize - asize) >= 2 * DSIZE) 
    {
        // 给当前 bp 的所在的块设置 header 和 footer，确定一个分配的内存块
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        // 更新 bp 指针到下一个空闲块
        bp = NEXT_BLKP(bp);
        // 更新空闲块的大小，因为刚刚分配了 asize 大小，需要减掉
        PUT(HDRP(bp), PACK((csize - asize), 0));
        PUT(FTRP(bp), PACK((csize - asize), 0));
    } 
    else
    {
        // 当前 bp 指向的内存块刚好容纳申请的大小，或剩余空间不满足最小的空间要求
        // 在当前 bp 的指向位置更新申请空间即可
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

static void *coalesce(void *bp)
{
    // 从当前 bp 出发
    // 检查前后相邻的内存块的分配状态，决定是否需要合并，回收内存碎片
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    // 1. 如果前后内存块都已分配，则不做任何处理
    if (prev_alloc && next_alloc)
        return bp;

    // 2. 如果前面的已分配，后面的未分配
    else if (prev_alloc && !next_alloc)
    {
        // 把后面内存块的大小取出来和当前的合并到一起，并更新 header 和 footer
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        // HDRP 中已经有了最新的 size，直接用 FTRP 就可以得到下一个内存块的 footer 位置
        PUT(FTRP(bp), PACK(size, 0));
    }

    // 3. 如果前面的未分配，后面的已分配
    else if (!prev_alloc && next_alloc)
    {
        // 把前面内存块的大小和当前块的大小合并到一起
        size += GET_SIZE(FTRP(PREV_BLKP(bp)));
        // 给前一个内存块的 header 和当前内存块的 footer 更新最新的 size 信息
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        // 更新 bp 指向为前一个内存块的 bp，确保 bp 指向的是空闲内存的起点
        bp = PREV_BLKP(bp);
    }

    // 4. 前后内存块都是未分配的
    else
    {
        // 把三个内存块的大小都合并到一起
        size_t prev_size = GET_SIZE(PREV_BLKP(bp));
        size_t next_size = GET_SIZE(NEXT_BLKP(bp));
        size += (prev_size + next_size);
        // 更新前一个块的 header 和后一个块的 footer
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        // 更新 bp
        bp = PREV_BLKP(bp);
    }

    // 返回更新后的 bp 指针
    return bp;
}

static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    // 8 字节对齐
    size = (words % 2) == 1 ? (words + 1) * WSIZE : words * WSIZE;
    // size = ALIGN(words * WSIZE);

    // 向内核额外申请内存空间，并把这部分新内存和已有的合并到一起
    // mem_sbrk 返回的是 block pointer
    if ((long)(bp = mem_sbrk(size)) != -1)
    {
        // prologue
        // 在 header 和 footer 中存储 size 信息，并且标记为未分配状态
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));

        // epilogue
        // footer 后面的一个字用来做结束标识
        // PUT(FTRP(bp) + WSIZE, PACK(0, 1));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

        // 把新分配的内存块与现有的做 merge，返回合并后的块指针
        return coalesce(bp);
    }
    return NULL;
}

static void printblock(void *bp)
{
    size_t hsize, halloc, fsize, falloc;

    checkheap(0);
    hsize = GET_SIZE(HDRP(bp));
    halloc = GET_ALLOC(HDRP(bp));
    fsize = GET_SIZE(FTRP(bp));
    falloc = GET_ALLOC(FTRP(bp));

    if (hsize == 0)
    {
        printf("%p: EOL\n", bp);
        return;
    }

    printf("%p: header: [%ld:%c] footer: [%ld:%c]\n", bp,
           hsize, (halloc ? 'a' : 'f'),
           fsize, (falloc ? 'a' : 'f'));
}

static void checkblock(void *bp)
{
    if ((size_t)bp % 8)
        printf("Error: %p is not doubleword aligned\n", bp);
    if (GET(HDRP(bp)) != GET(FTRP(bp)))
        printf("Error: header does not match footer\n");
}

/*
 * checkheap - Minimal check of the heap for consistency
 */
void checkheap(int verbose)
{
    char *bp = heap_listp;

    if (verbose)
        printf("Heap (%p):\n", heap_listp);

    if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp)))
        printf("Bad prologue header\n");
    checkblock(heap_listp);

    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
    {
        if (verbose)
            printblock(bp);
        checkblock(bp);
    }

    if (verbose)
        printblock(bp);
    if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp))))
        printf("Bad epilogue header\n");
}

// void main() {
//     checkheap(1);
// }
