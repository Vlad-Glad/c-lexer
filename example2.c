#include <stdio.h>
#define S(x) #x
#define CAT(a,b) a##b
#define HEX_FF 0xFFu

int main(void) {
    char c = '\n';
    const char* s = "hello\tworld";
    unsigned u = HEX_FF;
    u <<= 2; u |= 0x10u; u &= ~0x1u;

    /* block
       comment */
       // line comment
    int CAT(var, Name) = 123;

    printf("%s %d %u %c\n", S(hello), varName, u, c);
    return 0;
}
