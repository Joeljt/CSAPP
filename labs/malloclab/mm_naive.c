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

// double word alignment
#define ALIGNMENT 8

// round up to the nearest multiple of ALIGNMENT
// 5+7 & (~7) >>> 10100000 & 00001000
// erase the least significant bits to make sure
// the result is a multiple of ALIGNMENT
#define ALIGN(size) (((size) + (ALIGNMENT - 1))) & (~(ALIGNMENT - 1))

// 这个版本的实现，使用了一个 size_t 的大小存储 payload 的大小信息，类似 header
// 所以需要将对应的 header 进行对齐操作
#define HEADER (ALIGN(sizeof(size_t)))

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
    // 用户申请的内存大小，加上一个 header 的大小，再向上做对齐
    int alignedSize = ALIGN(size + HEADER);
    // 调用 sbrk 方法向内核申请内存
    void *p = mem_sbrk(alignedSize);
    if (p != (void *)-1)
    {
        // p 是泛型指针，可以将其转换成任何类型的指针并对其进行赋值
        // 这里将 p 的第一个位置转换为 size_t 类型，以便存储当前内存块的大小
        *(size_t *)p = size;
        // 从 p + HEADER 开始是给申请的地址空间，所以返回的时候做一个地址偏移
        // 所以这里在对字节数组做偏移的时候，需要将其转为 char* 后再操作：
        // 1、p 作为 void* 指针无法解引用
        // 2、这里 p 本质上是一个字节数组，因为申请 sbrk 的时候就是按字节申请的
        // 先转成 char* 做算术运算后将结果重新转回 void*，方便调用者转成目标类型
        return (void *)((char *)p + HEADER);
    }
    return NULL;
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
    // 先用 malloc 新申请一块新内存
    void *newptr = mm_malloc(size);
    if (newptr != NULL)
    {
        void *oldptr = ptr;
        // 从原来的 ptr 里向前移动一个字长，取出来当前内存块的大小
        // 同样先转成 char* 后做算术运算，然后转成 size_t*，再解引用才能拿到
        size_t targetSize = *(size_t *)(char *)oldptr - HEADER;
        if (size < targetSize)
        {
            // 如果是缩小内存空间，就将目标大小更新为最新值
            targetSize = size;
        }
        // 从 oldptr 拷贝 targetSize 大小到 newptr 中
        memcpy(newptr, oldptr, targetSize);
        mm_free(oldptr);
    }
    return NULL;
}
