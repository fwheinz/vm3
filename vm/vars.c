#include <stdlib.h>
#include <string.h>

#include "prog.h"

static struct varalloc varalloc;

struct var *_var_lookup (char *id, struct var *list, int len) {
  for (int i = len-1; i >= 0; i--) {
    if (strcmp(list[i].id, id) == 0)
      return &list[i];
  }

  return NULL;
}

struct var *_var_add (char *id, struct var **list, int *len) {
  if (id && _var_lookup(id, *list, *len))
    return NULL;

  *list = realloc(*list, (*len+1) * sizeof(**list));
  if (id) {
    (*list)[*len].id = strdup(id);
    (*list)[*len].nr = *len;
  } else {
    (*list)[*len].id = strdup("BLOCKBORDER");
    (*list)[*len].nr = VAR_BLOCKBORDER;
  }
  (*list)[*len].type = -1;
  (*len)++;

  return &(*list)[*len-1];
}

struct var *var_add_global (char *id) {
  return _var_add(id, &varalloc.globals, &varalloc.nrglobals);
}

struct var *var_add_local (char *id) {
  return _var_add(id, &varalloc.locals, &varalloc.nrlocals);
}

struct var *var_get_or_addlocal (char *id) {
  struct var *ret = _var_lookup(id, varalloc.globals, varalloc.nrglobals);
  if (!ret)
    ret = _var_lookup(id, varalloc.locals, varalloc.nrlocals);
  if (!ret)
    ret = _var_add(id, &varalloc.locals, &varalloc.nrlocals);

  return ret;
}

void var_enter_block (void) {
  var_add_local(NULL);
}

void var_leave_block (void) {
  for (int i = varalloc.nrlocals-1; i >= 0; i--) {
    if (varalloc.locals[i].nr == VAR_BLOCKBORDER) {
      varalloc.nrlocals = i - 1;
      break;
    }
  }
}

void var_reset (void) {
  varalloc.nrlocals = 0;
}


