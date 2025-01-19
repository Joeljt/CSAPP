CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -fsanitize=address,undefined -fstack-protector-all -I.

# 创建 _dist 目录
$(shell mkdir -p _dist)

TARGETS = ch02

ch02: $(wildcard chapter02/practice/*.c)
	@$(CC) $(CFLAGS) -DTEST_CH02 -o _dist/test_$@ $^
	@./_dist/test_$@
	@echo "\n"

# 编译所有
all: $(TARGETS)

# 清理
clean:
	@rm -rf _dist

.PHONY: all clean $(TARGETS)
