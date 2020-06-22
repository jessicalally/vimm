#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdint.h>

typedef struct vector {
  int cap;
  int len;
  uintptr_t *arr;

  char *str_buf;
} vector_t;

vector_t *new_vector(void);

void free_vector(vector_t*);

#define len(vec) (vec)->len

#define cap(vec) (vec)->cap

vector_t *_append(vector_t *vec, uintptr_t val);

#define append(vec, val) _append(vec, (uintptr_t) (val))

vector_t *append_str(vector_t *vec, char *str);

uintptr_t get(const vector_t *vec, int index);

#define getas(vec, ind, type) (type)get(vec,ind)

char *as_string(vector_t *vec);

char *as_string_from(vector_t *vec, int index);

void _insert(vector_t *vec, int index, uintptr_t val);

#define insert(vec, ind, val) _insert(vec, ind, (uintptr_t) val)

void del(vector_t *vec, int index);

void del_line_from(vector_t *vec, int index);

#endif
