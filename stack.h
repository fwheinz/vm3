#ifndef _STACK_H
#define _STACK_H

typedef int stackval_t;

struct stack {
  stackval_t *vals;
  int size;
};

typedef struct stack stack_t;

stack_t   *stack_new  (void);
void       stack_push (stack_t *stack, stackval_t val);
void       stack_set  (stack_t *stack, int pos, stackval_t val);
stackval_t stack_pop  (stack_t *stack);
stackval_t stack_peek (stack_t *stack);
void       stack_free (stack_t *stack);
int        stack_size (stack_t *stack);
int        stack_empty(stack_t *stack);


#endif /* _STACK_H */
