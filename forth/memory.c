#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "memory.h"
#include "output.h"
#include "intmath.h"
#include "control.h"
#include "../stack.h"
#include "../sys.h"
#include "../defs.h"

// ( n addr -- )
// Stores n at the memory location pointed to by addr
void store() {
    uint32_t *ptr = sys_addr(*stack_at(0));
    int32_t n = *(int32_t *) stack_at(1);
    stack_pop(2);
    if((ptr >= sys.sys && ptr < sys.sys_top)
            || (uint32_t *) ptr == &sys.base) {
        *(int32_t *) ptr = n;
    } else {
        fprintf(stderr, "ERROR: Out-of-bounds access attempted, aborting\n");
        abort_();
    }
}

// ( d addr -- )
// Stores d at the memory location pointed to by addr
void store2() {
    int64_t* ptr = (int64_t *) sys_addr(*stack_at(0));
    int64_t n = *(int64_t*)stack_at(2);
    stack_pop(3);
    if(((uint32_t *) ptr >= sys.sys && (uint32_t *) ptr < sys.sys_top)
            || (uint32_t *) ptr == &sys.base) {
        *ptr = n;
    } else {
        fprintf(stderr, "ERROR: Out-of-bounds access attempted, aborting\n");
        abort_();
    }
}

// ( c addr -- )
// Stores c at the memory location pointed to by addr
void c_store() {
    char *ptr = (char *) sys_addr(*stack_at(0));
    char n = *(char *) stack_at(1);
    stack_pop(2);
    if((uint32_t *) ptr >= sys.sys && (uint32_t *) ptr < sys.sys_top) {
        *ptr = n;
    } else {
        fprintf(stderr, "ERROR: Out-of-bounds access attempted, aborting\n");
        abort_();
    }
}

// ( n addr -- )
// Adds n to the number stored at addr
void plus_store() {
    int32_t* ptr = (int32_t *) sys_addr(*stack_at(0));
    int32_t n = *(int32_t *) stack_at(1);
    stack_pop(2);
    if((uint32_t *) ptr >= sys.sys && (uint32_t *) ptr < sys.sys_top) {
        *ptr += n;
    } else {
        fprintf(stderr, "ERROR: Out-of-bounds access attempted, aborting\n");
        abort_();
    }
}

// ( addr -- n )
// Pushes the contents of addr to the top of the stack
void fetch() {
    int* ptr = (int32_t *) sys_addr(*stack_at(0));
    stack_pop(1);
    if((uint32_t *) ptr >= sys.sys && (uint32_t *) ptr < sys.sys_top) {
        stack_push(*ptr);
    } else {
        fprintf(stderr, "ERROR: Out-of-bounds access attempted, aborting\n");
        abort_();
    }
}

// ( addr -- d )
// Pushes the double-length contents of addr to the top of the stack
void fetch2() {
    int64_t* ptr = (int64_t *) sys_addr(*stack_at(0));
    stack_pop(1);
    if((uint32_t *) ptr >= sys.sys && (uint32_t *) ptr < sys.sys_top) {
        stack_push_d(*ptr);
    } else {
        fprintf(stderr, "ERROR: Out-of-bounds access attempted, aborting\n");
        abort_();
    }
}

// ( addr -- c )
// Pushes the byte pointed to by addr to the top of the stack
void c_fetch() {
    char* ptr = (char*) sys_addr(*stack_at(0));
    stack_pop(1);
    if((uint32_t *) ptr >= sys.sys && (uint32_t *) ptr < sys.sys_top) {
        stack_push((int32_t)*ptr);
    } else {
        fprintf(stderr, "ERROR: Out-of-bounds access attempted, aborting\n");
        abort_();
    }
}

// ( -- 4 )
// Pushes 4 (the size of a cell) onto the stack
void cell() {
    stack_push(4);
}

// ( n1 -- n2 )
// Multiplies n1 by 4
void cells() {
    *stack_at(0) *= 4;
}

// ( addr1 -- addr2 )
// Increments addr1 by the size of a cell
void cell_plus() {
    cell();
    add();
}

// In this implementation, CHARS is effectively a no-op
// It still needs to check the top of the stack though, because
// it's supposed to crash if the stack is empty
void chars() {
    *stack_at(0) *= 1;
}

void char_plus() {
    *stack_at(0) += 1;
}

// ( n c addr -- )
// Fills n1 bytes at addr with byte c
void fill() {
    char b = *(char *) stack_at(0);
    int32_t n = *(int32_t *) stack_at(1);
    char* addr = (char *) sys_addr(*stack_at(2));
    stack_pop(3);
    if((uint32_t *) addr >= sys.sys && (uint32_t *) addr < sys.sys_top) {
        for(int32_t i = 0; i < n; i++)
            addr[i] = b;
    } else {
        fprintf(stderr, "ERROR: Out-of-bounds access attempted, aborting\n");
        abort_();
    }
}

// NOT CURRENTLY USED
/*
// ( u addr -- )
// Prints the contents of u bytes at addr
void dump() {
    uint32_t c = *stack_at(0);
    int* addr = (int32_t*)stack_at(1);
    stack_pop(2);
    for(uint32_t i = 0; i < c; i++)
        if(i % 4 == 0) printf("%d ", addr[i / 4]);
}
*/

void variable() {
    char *name = get_substring(isspace);
    uint32_t *new_wd = add_def(name, 0);
    sys.gloss_head = new_wd;
    *sys.cp = CREATE_RUNTIME_ADDR;
    sys.cp += 2;
    sys.old_cp = sys.cp;
}

void constant() {
    char *name = get_substring(isspace);
    uint32_t *new_wd = add_def(name, 0);
    sys.gloss_head = new_wd;
    *sys.cp = CONSTANT_RUNTIME_ADDR;
    sys.cp++;
    *(int32_t *) sys.cp = *(int32_t *) stack_at(0);
    stack_pop(1);
    sys.cp++;
    sys.old_cp = sys.cp;
}

void allot() {
    uint32_t count = *stack_at(0);
    stack_pop(1);
    sys.cp += count;
    sys_util.alloc += count * 4;
}

void create() {
    char *name = get_substring(isspace);
    uint32_t *new_wd = add_def(name, 0);
    sys.gloss_head = new_wd;
    *sys.cp = CREATE_RUNTIME_ADDR;
    sys.old_cp = sys.cp;
    sys.cp++;
    sys_util.alloc = 0;
}

void create_runtime() {
    stack_push(forth_addr(sys.inst + 1));
    exit_();
}

void constant_runtime() {
    stack_push(*(int32_t *) (sys.inst + 1));
    exit_();
}

void comma() {
    *sys.cp = *stack_at(0);
    sys.cp++;
    stack_pop(1);
}

void c_comma() {
    char *ccp = (char *) sys.cp;
    *ccp = *(char *) stack_at(0);
    sys.cp = (uint32_t *) (ccp + 1);
    stack_pop(1);
}

// TODO: Re-implement once I separate variable / compiled word space
void to_body() {
    *stack_at(0) += 4;
}

void align() {
    if((uint32_t) sys.cp % 4 != 0) {
        uint32_t *cp_ptr = (uint32_t *) &sys.cp;
        *cp_ptr += 4 - ((uint32_t) sys.cp % 4);
    }
}

void aligned() {
    uint32_t *addr = (uint32_t *) ((uint32_t) sys.cp + ((4 - ((uint32_t) sys.cp % 4)) % 4));
    stack_push(forth_addr(addr));
}

void here() {
    stack_push(forth_addr(sys.cp));
}

void move() {
    uint32_t n = *stack_at(0);
    void *dest = (void *) sys_addr(*stack_at(1));
    void *src = (void *) sys_addr(*stack_at(2));
    stack_pop(3);
    memmove(src, dest, n);
}
