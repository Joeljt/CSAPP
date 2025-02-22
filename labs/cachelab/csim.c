#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define MEMORY_SIZE (1 << 20) // 1MB 主存
unsigned char memory[MEMORY_SIZE];

typedef struct {
    bool valid;         // 标记缓存是否有效
    unsigned long tag;  // tag bits，高位地址值，用来做 key 定位具体的缓存行
    int lru_counter;    // lru 计数器，用来做替换策略
    char* data;         // 缓存块数组，具体大小不定，根据 block_bits 动态申请
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

void init_memory() {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = (unsigned char)(i & 0xFF); // 让数据有规律，方便调试
    }
}

// 根据配置信息初始化缓存对象并返回，方便后续操作
// 注意这里的 Cache 是开辟在栈空间上的，而不是在堆中动态申请的
Cache* init_cache(int set_count, int lines_per_set, int block_size) {
    Cache* cache = (Cache*)malloc(sizeof(Cache));
    cache->set_count = set_count;
    cache->sets = (CacheSet*)malloc(sizeof(CacheSet) * set_count);

    for (int i = 0; i < set_count; i++) {
        cache->sets[i].lines = (CacheLine*)malloc(sizeof(CacheLine) * lines_per_set);
        cache->sets[i].line_count = lines_per_set;
        for (int j = 0; j < lines_per_set; j++) {
            cache->sets[i].lines[j].valid = false;
            cache->sets[i].lines[j].tag = 0;
            cache->sets[i].lines[j].lru_counter = 0;
            cache->sets[i].lines[j].data = (char*)malloc(sizeof(char) * block_size);
        }
    }
    return cache;
}

void free_cache(Cache* cache) {
    for (int i = 0; i < cache->set_count; i++) {
        CacheSet* currentSet = &cache->sets[i];
        for (int j = 0; j < currentSet->line_count; j++) {
            free(currentSet->lines[j].data); 
        }
        free(currentSet->lines);
    }
    free(cache->sets);
    free(cache);
}

char access_cache(Cache* cache, unsigned long address, int set_bits, int block_bits) {

    // 取出来 set_bits 和 tag_bits，用来在缓存组中进行匹配使用
    // 先右移扔掉所有的 block_bits，(1 << set_bits) 是缓存组数量，对其 -1 就是 0111，做 & 就可以得到对应的缓存组下标
    unsigned long set_index = (address >> block_bits) & ((1 << set_bits) - 1);
    // 右移扔掉所有的 set_bits 和 block_bits，得到 tag_bits 作为缓存行的 key 值
    unsigned long tag_bits = address >> (set_bits + block_bits);
    // 取出当前地址的 offset，用来获取缓存数据，即 data 数组索引
    // unsigned long block_offset = address & ((1 << block_bits) - 1);

    // 根据 set_index 从 cache 里找缓存组
    // 用指针访问，操作对应的缓存组而非副本
    CacheSet* set = &cache->sets[set_index];

    // 遍历缓存组，根据 tag 匹配缓存行
    for (int i = 0; i < set->line_count; i++){
        // 如果缓存有效并且可以匹配中 tag，则命中缓存，标记 lru_counter

        if (set->lines[i].valid && set->lines[i].tag == tag_bits) {
            // 将命中的行 lru_counter 设为 0，表示最新命中，其余的全部自增
            hits++;
            set->lines[i].lru_counter = 0;
            for (int j = 0; j < set->line_count; j++){
                if (i != j && set->lines[j].valid) set->lines[j].lru_counter++;
            }
            // 返回命中的缓存值
            // return set->lines[i].data[block_offset];
            return 0;
        }
    }
    
    // 标记未命中，所有未命中的 lru counter 都 +1
    misses++;
    for (int j = 0; j < set->line_count; j++){
        if (set->lines[j].valid) set->lines[j].lru_counter++;
    }

    // 找到合适的位置更新缓存：valid 为 false，或者 lru_counter 最大，证明其不是最新值
    int evict_index = -1, max_lru = -1;
    for (int i = 0; i < set->line_count; i++){
        if (!set->lines[i].valid) {
            evict_index = i;
            break;
        }
        if (set->lines[i].lru_counter > max_lru) {
            max_lru = set->lines[i].lru_counter;
            evict_index = i;
        }
    }

    // 标记缓存替换
    if (set->lines[evict_index].valid) {
        evictions++;
    }

    // 将最新的值缓存在目标位置
    set->lines[evict_index].valid= true;
    set->lines[evict_index].tag = tag_bits;
    set->lines[evict_index].lru_counter = 0;

    // 未命中的情况下，需要从主存里复制到缓存
    // 抹掉低 block_bits 位，从而对齐到该块的起始地址
    // unsigned long block_start = address & ~((1 << block_bits) - 1);
    // memcpy(set->lines[evict_index].data, &memory[block_start], (1 << block_bits));

    // return set->lines[evict_index].data[block_offset];
    return 0;
}

void parse_trace(Cache* cache, const char* tracefile, int set_bits, int block_bits) {
    FILE *file = fopen(tracefile, "r");
    if (!file) exit(EXIT_FAILURE);

    // 用 fscanf 函数，按照某种格式逐行读取，类似 bomblab 里 read_six_numbers 一样
    char op;
    unsigned long address;
    int bytes;
    while((fscanf(file, " %c %lx,%d", &op, &address, &bytes)) > 0) {
        if (op == 'L' || op == 'S') {
            // Load, Store, Modify 都需要访问缓存一次
            access_cache(cache, address, set_bits, block_bits);
        } else if (op == 'M') {
            // 如果是 Modify 的话，还会有第二次
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

    // 初始化模拟主存，循环填充 0~255 
    init_memory();

    // 根据命令行参数初始化缓存
    // 注意这里命令行指定的是表示缓存组的位数，但是初始化缓存的时候需要的是具体的组数
    // 每一位都能表示 0 或者 1，所以位数能表达的数量实际上就是 2 的位数次幂，比如 3 位能表示 8 个不同的值
    // 这里可以写成 2^set_bits，但是我们知道 1 << x 等价于 2^x，而移位效率更高
    // 所以这里我们使用 1 << set_bits 来得到缓存组的数量
    Cache* cache = init_cache(1 << set_bits, lines_per_set, 1 << block_bits);

    // 解析文件并模拟缓存行为
    parse_trace(cache, tracefile, set_bits, block_bits);

    // 回收内存
    free_cache(cache);

    printSummary(hits, misses, evictions);
    return 0;
}
