#include <assert.h>
#include <stdlib.h>

#include "stack.h"

stack_t *stack_new (void) {
  stack_t *stack = malloc (sizeof *stack);
  assert(stack != NULL);
  stack->vals = NULL;
  stack->size = 0;

  return stack;
}

void stack_push (stack_t *stack, stackval_t val) {
  stack->vals = realloc(stack->vals, (stack->size + 1) * sizeof (stackval_t));
  assert(stack->vals != NULL);
  stack->vals[stack->size++] = val;
}

void stack_set (stack_t *stack, int pos, stackval_t val) {
  assert(stack->size > pos);
  stack->vals[pos] = val;
}

stackval_t stack_pop (stack_t *stack) {
  assert(stack->size > 0);
  return stack->vals[--stack->size];
}

stackval_t stack_peek (stack_t *stack) {
  assert(stack->size > 0);
  return stack->vals[stack->size - 1];
}

int stack_size (stack_t *stack) {
  return stack->size;
}

int stack_empty (stack_t *stack) {
  return stack_size(stack) == 0;
}

void stack_free (stack_t *stack) {
  free(stack->vals);
  free(stack);
}

