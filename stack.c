#include <stdint.h>
#include <stdlib.h>

#include "stack.h"

const size_t CAPACITY = 16;

struct Stack* init_stack() {
    struct Stack *s = malloc(sizeof(struct Stack));
    s->sp = 0;
    s->values = malloc(CAPACITY * sizeof(uint16_t));

    return s;
}

void push_stack(struct Stack *stack, uint16_t value) {
    if(stack->sp > 16) return;
    stack->values[(stack->sp)++] = value;
}

uint16_t pop_stack(struct Stack *stack) {
    if(stack->sp <= 0) return 0;
    return stack->values[--(stack->sp)];
}

uint16_t peek_stack(struct Stack *stack) {
    return stack->values[stack->sp];
}