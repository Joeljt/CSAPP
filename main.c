#include <stdio.h>
#include <stdlib.h>

struct A {
  char a;
  int b;
  char c;
};

struct B {
  short a;  // 2字节
  double b; // 8字节
  short c;  // 2字节
};

struct C {
  char a;
  char c;
  char b;
};

struct D {
    char a;  // 1字节
    char b;  // 1字节
    int c;   // 4字节
    double d;// 8字节
};

struct Example {
    char a;     // 1字节
    int b;      // 4字节
    char c;     // 1字节
    double d;   // 8字节
};

struct BetterExample {
    double d;   // 0-7  (8字节对齐)
    int b;      // 8-11 (4字节对齐)
    char a;     // 12   (1字节对齐)
    char c;     // 13   (1字节对齐)
    // 14-15 填充到8字节的整数倍
};

struct E {
  double a;
  char b;
  int c;
};

char *my_gets(char *s)
{
    int c;
    char *dest = s; 

    while ((c = getchar()) != '\n' && c != EOF)
        *dest++ = c;
    
    if (c == EOF && dest == s)
        return NULL;
    *dest++ = '\0'; /* Terminate string */
    return s;
}

/* Read input line and write it back */
void echo()
{
    char buf[8];  /* Way too small! */
    my_gets(buf);
    puts(buf);
}

int main() {
    printf("sizeof(struct A) = %lu\n", sizeof(struct A));
    printf("sizeof(struct B) = %lu\n", sizeof(struct B));
    printf("sizeof(struct C) = %lu\n", sizeof(struct C));
    printf("sizeof(struct D) = %lu\n", sizeof(struct D));
    printf("sizeof(struct Example) = %lu\n", sizeof(struct Example));
    printf("sizeof(struct BetterExample) = %lu\n", sizeof(struct BetterExample));
    printf("sizeof(struct E) = %lu\n", sizeof(struct E));
    echo();
    return 0;
}
