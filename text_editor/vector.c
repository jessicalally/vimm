#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "vector.h"

#define INIT_CAP 256

static void grow(vector_t *vec){
  int new_cap = vec->cap * 2;
  vec->arr = realloc(vec->arr, new_cap * sizeof(uintptr_t));
  vec->cap = new_cap;
}

vector_t *new_vector(void){
  uintptr_t *arr = malloc(INIT_CAP* sizeof(uintptr_t));
  vector_t *vec = calloc(1, sizeof(vector_t));
  vec->cap = INIT_CAP;
  vec->arr = arr;
  return vec;
}

void free_vector(vector_t *vec){
  assert(vec != NULL);

  free(vec->arr);
  if (vec->str_buf != NULL){
    free(vec->str_buf);
  }
  free(vec);
}

vector_t *_append(vector_t *vec, uintptr_t val){
  assert(sizeof(val) <= sizeof(uintptr_t));
  if (len(vec) + 1 > cap(vec)) {
    grow(vec);
  }
  vec->arr[len(vec)] = val;
  vec->len++;
  return vec;
}

vector_t *append_str(vector_t *vec, char *str){
  for (int i =0; i < strlen(str); i++){
    append(vec, str[i]);
  }
  return vec;
}


uintptr_t get(const vector_t *vec, int index){
  if(index >= len(vec) || index < 0){
    char err[128];
    sprintf(err,"FATAL: index %d out of bounds of length %d", index, len(vec));
    perror(err);
    exit(EXIT_FAILURE);
  } 
  return vec->arr[index];
}

void _insert(vector_t *vec, int index, uintptr_t val){
  assert(sizeof(val) <= sizeof(uintptr_t));
  if(index > len(vec) || index < 0){
    char err[128];
    sprintf(err,"FATAL: index %d out of bounds of length %d", index, len(vec));
    perror(err);
    exit(EXIT_FAILURE);
  } 

  if (index == len(vec)){
    append(vec, val);
    return;
  }

  if (len(vec)+1 > cap(vec)){
    grow(vec);
  }
  
  memmove(&vec->arr[index+1], &vec->arr[index], (len(vec)-index) * sizeof(uintptr_t));
  vec->arr[index]=val;
  vec->len++;
}

void del(vector_t *vec, int index){
  if(index > len(vec) || index < 0){
    char err[128];
    sprintf(err,"FATAL: index %d out of bounds of length %d", index, len(vec));
    perror(err);
    exit(EXIT_FAILURE);
  } 

  if (index == len(vec)){
    vec->len--;
    return;
  }

  memmove(&vec->arr[index], &vec->arr[index+1],(len(vec)-index-1) * sizeof(uintptr_t));
  vec->len--;
}

void del_line_from(vector_t *vec, int index) {
  int length = len(vec);
  for (int i = index; i < length; i++) {
    del(vec, index);
  }
}

char *as_string(vector_t *vec){
  return as_string_from(vec,0);
}

char *as_string_from(vector_t *vec, int index){
  vec->str_buf = realloc(vec->str_buf, len(vec) * sizeof(char)+1);
  for (int i=index; i<len(vec); i++){
    vec->str_buf[i] = (char) vec->arr[i];
  }
  vec->str_buf[len(vec)] = '\0';
  return (char *) &vec->str_buf[index];
}
