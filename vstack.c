#include <assert.h>
#include <stdlib.h>

#include "vstack.h"

vstack_t *vstack_new (void) {
  vstack_t *stack = malloc (sizeof *stack);
  assert(stack != NULL);
  stack->vals = NULL;
  stack->size = 0;

  return stack;
}

void vstack_push (vstack_t *stack, vstackval_t val) {
  stack->vals = realloc(stack->vals, (stack->size + 1) * sizeof (vstackval_t));
  assert(stack->vals != NULL);
  stack->vals[stack->size++] = val;
}

vstackval_t vstack_pop (vstack_t *stack) {
  assert(stack->size > 0);
  return stack->vals[--stack->size];
}

vstackval_t vstack_peek (vstack_t *stack) {
  assert(stack->size > 0);
  return stack->vals[stack->size - 1];
}

int vstack_size (vstack_t *stack) {
  return stack->size;
}

int vstack_empty (vstack_t *stack) {
  return vstack_size(stack) == 0;
}

void vstack_free (vstack_t *stack) {
  free(stack->vals);
  free(stack);
}

