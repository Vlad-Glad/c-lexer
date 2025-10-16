// examples/demo.c
#include <stdio.h>          // preprocessor + comment

#define S(x) #x             // stringize: MacroHash
#define CAT(a,b) a##b       // token concatenation: MacroConcat
#define PI 3.14159f
#define HEX_FF 0xFFu

/* Multiline comment:
   demonstrates block format */

enum Color { Red = 1, Green = 2, Blue = 3 };

struct Point {
    int x, y;
};

static int add(int a, int b) { return a + b; }

int sum_all(int n, ...) {     // ellipsis ...
    int s = 0;
    // For demo purposes – just return n (without va_list)
    s += n;
    return s;
}

int main(void) {
    int x = 42;               // IntLiteral
    unsigned u = HEX_FF;      // 0xFFu
    float r = PI;             // FloatLiteral with suffix f
    double d = 1e-3;          // exponent notation
    char c = '\n';            // CharLiteral with escape
    const char* msg = "Hello, C!\n"; // StringLiteral

    struct Point p = { .x = 10, .y = 20 }; // designated initialization

    if (x > 0 && u >= 0u) {   // comparison and logical operators
        printf("%s", msg);
    }
    else {
        puts("else branch");
    }

    int y = add(5, 7);
    printf("add=%d, d=%.6f, r=%.2f\n", y, d, r);

    // use of macros # and ##
    printf("S(Hello) -> %s\n", S(Hello));
    int CAT(var, Name) = 123; // will become varName
    printf("varName=%d\n", varName);

    // ternary operator example
    int max = (x > y) ? x : y;
    printf("max=%d\n", max);

    // call of a variadic function
    int t = sum_all(3, 10, 20, 30);
    printf("sum_all(...) demo: %d\n", t);

    // shifts and compound assignments
    u <<= 1;
    u |= 0x2u;
    printf("u=%u\n", u);

    return 0;
}
