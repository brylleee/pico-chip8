#include <stdint.h>
#include <stdlib.h>

#ifndef STACK_H
#define STACK_H

struct Stack {
    uint8_t sp;
    uint16_t *values;
};

struct Stack* init_stack();
void push_stack(struct Stack*, uint16_t);
uint16_t pop_stack(struct Stack*);
uint16_t peek_stack(struct Stack*);

#endif