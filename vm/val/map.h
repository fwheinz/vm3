#ifndef _MAP_H
#define _MAP_H

struct map {
  val_t **keys;
  val_t **vals;
  int size;
};

typedef struct map map_t;

map_t *map_new (void);
void map_free (map_t *);
val_t *map_set (map_t *, val_t *, val_t *);
val_t *map_get (map_t *, val_t *);
val_t *map_get_key (map_t *, int nr);
val_t *map_get_val (map_t *, int nr);
int  map_len (map_t *);
int  map_mark (map_t *);

val_t *v_map_create (void);
val_t *v_map_index (val_t *a, val_t *i);
val_t *v_map_index_assign (val_t *a, val_t *i, val_t *v);

void val_register_map (void);



#endif /* _MAP_H */
