#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    bool valid;         // 标记缓存是否有效
    unsigned long tag;  // tag bits，高位地址值，用来做 key 定位具体的缓存行
    int lru_counter;    // lru 计数器，用来做替换策略
    // int data[64];    // 正常情况下，缓存数据就存储在这里
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

// 根据配置信息初始化缓存对象并返回，方便后续操作
// 注意这里的 Cache 是开辟在栈空间上的，而不是在堆中动态申请的
Cache init_cache(int set_count, int lines_per_set) {
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

void free_cache(Cache cache) {
    for (int i = 0; i < cache.set_count; i++) {
        free(cache.sets[i].lines);
    }
    free(cache.sets);
}

void access_cache(Cache cache, unsigned long address, int set_bits, int block_bits) {

    // 取出来 set_bits 和 tag_bits，用来在缓存组中进行匹配使用
    // 先右移扔掉所有的 block_bits，(1 << set_bits) 是缓存组数量，对其 -1 就是 0111，做 & 就可以得到对应的缓存组下标
    unsigned long set_index = (address >> block_bits) & ((1 << set_bits) - 1);
    // 右移扔掉所有的 set_bits 和 block_bits，得到 tag_bits 作为缓存行的 key 值
    unsigned long tag_bits = address >> (set_bits + block_bits);

    // 根据 set_index 从 cache 里找缓存组
    CacheSet set = cache.sets[set_index];

    // 遍历缓存组，根据 tag 匹配缓存行
    for (int i = 0; i < set.line_count; i++){
        // 如果缓存有效并且可以匹配中 tag，则命中缓存，标记 lru_counter
        if (set.lines[i].valid && set.lines[i].tag == tag_bits) {
            set.lines[i].lru_counter = 0;
            // 标记命中
            hits++;
            return;
        }
    }
    
    // 标记未命中
    misses++;

    // 找到合适的位置更新缓存：valid 为 false，或者 lru_counter 最大，证明其不是最新值
    int evict_index = 0, max_lru = -1;
    for (int i = 0; i < set.line_count; i++){
        if (set.lines[i].valid) {
            if (set.lines[i].lru_counter > max_lru) {
                max_lru = set.lines[i].lru_counter;
                evict_index = i;
            }
        } else {
            evict_index = i;
            break;
        }
    }

    // 标记缓存替换
    if (set.lines[evict_index].valid) {
        evictions++;
    }

    // 将最新的值缓存在目标位置
    set.lines[evict_index].valid= true;
    set.lines[evict_index].tag = tag_bits;
    set.lines[evict_index].lru_counter = 0;
}

void parse_trace(Cache cache, const char* tracefile, int set_bits, int block_bits) {
    FILE *file = fopen(tracefile, "r");
    if (!file) exit(EXIT_FAILURE);

    // 用 fscanf 函数，按照某种格式逐行读取，类似 bomblab 里 read_six_numbers 一样
    char op;
    unsigned long address;
    int bytes;
    while((fscanf(file, " %c %ld,%d", &op, &address, &bytes)) > 0) {
        // // Load, Store, Modify 都至少需要访问缓存一次
        // access_cache(cache, address, set_bits, block_bits);
        // // 如果是 Modify 的话，还会有第二次
        // if (op == 'M') {
        //     access_cache(cache, address, set_bits, block_bits);
        // }
        if (op == 'L' || op == 'S') {
            access_cache(cache, address, set_bits, block_bits);
        } else if (op == 'M') {
            access_cache(cache, address, set_bits, block_bits);
            access_cache(cache, address, set_bits, block_bits);
        }
    }
    fclose(file);
}

int main(int argc, char **argv)
{
    // 声明变量，用来接收命令行传递的参数
    int set_bits, lines_per_set, block_bits;
    char* tracefile = NULL;

    // 利用 getopt 方法获取命令行参数，并将其赋值给提前声明好的变量
    int opt;
    while((opt = getopt(argc, argv, "s:E:b:t:")) != -1) {
        switch (opt){
            case 's': set_bits = atoi(optarg); break;
            case 'E': lines_per_set = atoi(optarg); break;
            case 'b': block_bits = atoi(optarg); break;
            case 't': tracefile = optarg; break;
            default: exit(EXIT_FAILURE);
        }
    }
    if (!tracefile) exit(EXIT_FAILURE);

    // 根据命令行参数初始化缓存
    // 注意这里命令行指定的是表示缓存组的位数，但是初始化缓存的时候需要的是具体的组数
    // 每一位都能表示 0 或者 1，所以位数能表达的数量实际上就是 2 的位数次幂，比如 3 位能表示 8 个不同的值
    // 这里可以写成 2^set_bits，但是我们知道 1 << x 等价于 2^x，而移位效率更高
    // 所以这里我们使用 1 << set_bits 来得到缓存组的数量
    Cache cache = init_cache(1 << set_bits, lines_per_set);

    // 解析文件并模拟缓存行为
    parse_trace(cache, tracefile, set_bits, block_bits);

    free_cache(cache);

    printSummary(hits, misses, evictions);
    return 0;
}
