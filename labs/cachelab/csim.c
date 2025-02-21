#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    bool valid;          // 标记缓存是否有效
    unsigned long tag;   // tag bits，高位地址值，用来做 key 定位具体的缓存行
    int data[64];   // 缓存数据，用来存储二进制数据
    int lru_counter;    // lru 计数器，用来做替换策略
} CacheLine;

typedef struct {
    CacheLine* lines;   // 一个缓存组中包含多个缓存行
    int line_count;     // 手动记录每个缓存组中缓存行的数量
} CacheSet;

typedef struct {
    CacheSet* sets;     // 一个缓存中包含多个缓存组
    int set_count;      // 手动记录缓存组的数量
} Cache;

// 定义统计变量，最后给 printSummary 函数使用
int hits = 0, misses = 0, evictions = 0;

/**
 * 根据配置信息初始化缓存对象并返回，方便后续操作
 */
Cache initCache(int set_count, int lines_per_set) {
    Cache cache;
    cache.set_count = set_count;
    cache.sets = (CacheSet*)malloc(sizeof(CacheSet) * set_count);

    for (int i = 0; i < set_count; i++) {
        cache.sets[i].lines = (CacheLine*)malloc(sizeof(CacheLine) * lines_per_set);
        cache.sets[i].line_count = lines_per_set;
        for (int j = 0; j < lines_per_set; j++) {
            cache.sets[i].lines[j].valid = false;
            cache.sets[i].lines[j].tag = 0;
            cache.sets[i].lines[j].lru_counter = 0;
        }
    }
    return cache;
}


void freeCache(Cache cache) {
    for (int i = 0; i < cache.set_count; i++) {
        free(cache.sets[i].lines);
    }
    free(cache.sets);
}

int main()
{


    printSummary(0, 0, 0);
    return 0;
}
