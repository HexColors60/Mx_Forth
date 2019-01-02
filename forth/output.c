#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../stack.h"
#include "../sys.h"
#include "output.h"


// Prints a 64-bit unsigned number in the given base
void ubprint(uint64_t num) {
    char buf[32];
    memset(buf, 0, 32);
    buf[0] = '0';
    uint64_t tmpnum = num;
    int len = (int) (log(tmpnum) / log(sys.base));
    for(int i = len; tmpnum > 0; i--) {
        char digit = (tmpnum % sys.base) + '0';
        if (digit > '9') digit += 7;
        buf[i] = digit;
        tmpnum /= sys.base;
    }
    printf("%s ", buf);
    fflush(stdout);
}

void bprint(int64_t num) {
    if(num < 0) { 
        printf("-");
        num *= -1;
    }

    ubprint(num);
}

// ( -- )
// Prints a newline character to the terminal
void cr() {
    printf("\n");
    fflush(stdout);
}

// ( n -- )
// Prints n spaces
void spaces() {
    for(uint32_t i = 0; i < *(uint32_t*)stack_at(0); i++)
        printf(" ");
    stack_pop(1);
    if(sys.ABORT) return;
    fflush(stdout);
}

// ( -- )
// Prints a space
void space() {
    printf(" ");
    fflush(stdout);
}

// ( c -- )
// Prints character c
void emit() {
    char ch = (char)*stack_at(0);
    printf("%c", ch);
    stack_pop(1);
    if(sys.ABORT) return;
    fflush(stdout);
}

// ( n -- )
// Prints and pops the top of the stack, followed by a space
void print() {
    int32_t n = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(sys.ABORT) return;
    bprint(n);
    fflush(stdout);
}

// ( u -- )
// Unsigned int32_t print
void uprint() {
    uint32_t n = *stack_at(0);
    stack_pop(1);
    if(sys.ABORT) return;
    ubprint(n);
    fflush(stdout);
}

// ( addr u -- )
void type() {
    uint32_t u = *stack_at(0);
    char* str = (char*)*stack_at(1);
    stack_pop(2);
    if(sys.ABORT) return;
    fwrite(str, 1, u, stdout);
    fflush(stdout);
}
