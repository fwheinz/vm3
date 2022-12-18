#ifndef _VSTACK_H
#define _VSTACK_H

#include "val/val.h"

typedef val_t* vstackval_t;

struct vstack {
  vstackval_t *vals;
  int size;
};

typedef struct vstack vstack_t;

vstack_t   *vstack_new  (void);
void       vstack_push (vstack_t *stack, vstackval_t val);
vstackval_t vstack_pop  (vstack_t *stack);
vstackval_t vstack_peek (vstack_t *stack);
void       vstack_free (vstack_t *stack);
int        vstack_size (vstack_t *stack);
int        vstack_empty(vstack_t *stack);


#endif /* _VSTACK_H */
