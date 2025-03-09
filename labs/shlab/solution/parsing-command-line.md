## 解析命令行的工具函数

命令行一般会指定不同的参数，程序需要从用户输入的命令行中提取不同的部分，进行相应的处理。

这个过程中往往会用到如下工具函数：

- `getopt`: 从 `argc` 以及 `argv` 中提取参数，根据指定的字符串格式顺序取值；
- `atoi`: 将一个 ASCII 码转为 int 值，常见场景为转换 `getopt` 读出来的命令行的某个参数值；
  - atoi 接收的是指针，从目标位置开始向后遍历，找到最长的一个数字序列，并将其转为数字

```c
int main(int argc, char **argv) {
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
}
```

`man 3 getopt` 了解详细信息。



