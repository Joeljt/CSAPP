CC = gcc
# CFLAGS = -Wall -Wextra -Werror -g -fsanitize=address,undefined -fstack-protector-all -I.
CFLAGS = -Wno-deprecated-declarations -Wno-tautological-constant-compare -Wno-error

# 创建 _dist 目录
$(shell mkdir -p _dist)

TARGETS = test tiny echo_s echo_c

test: $(wildcard main.c)
	@$(CC) $(CFLAGS) -o _dist/test_$@ $^
	@./_dist/test_$@
	@echo "\n"

echo_s: ch11/src/csapp.c ch11/src/echo/echoserveri.c ch11/src/echo/echo.c
	@$(CC) $(CFLAGS) -o _dist/test_$@ $^ -I ch11/src/
	@./_dist/test_$@
	@echo "\n"

echo_c: ch11/src/csapp.c ch11/src/echo/echoclient.c
	@$(CC) $(CFLAGS) -o _dist/test_$@ $^ -I ch11/src/
	@./_dist/test_$@
	@echo "\n"

tiny: ch11/src/csapp.c ch11/src/tiny/tiny.c
	@$(CC) $(CFLAGS) -o _dist/test_$@ $^ -I ch11/src/
	@echo "\n"

# 编译所有
all: $(TARGETS)

# 清理
clean:
	@rm -rf _dist

.PHONY: all clean $(TARGETS)
