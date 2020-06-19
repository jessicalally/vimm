#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vector.h"

int asserts_ran = 0, asserts_failed = 0, tests_ran = 0, tests_failed = 0;

#define assert_eq(a, b) do { asserts_ran++; if ((a) != (b)) { printf("%s(line %d): got: %d | expected: %d\n", __func__, __LINE__, a, b); asserts_failed++; } } while (0)

#define assert_eq_silent(a, b) do { asserts_ran++; if ((a) != (b)) { asserts_failed++; } } while (0)

#define assert_ptr_notnull(p) do { asserts_ran++; if (p == NULL) { printf("%s(line %d): pointer should not be NULL\n", __func__, __LINE__); asserts_failed++; return; } } while (0)

#define run_test(test) do { \
asserts_ran = asserts_failed = 0; test(); \
tests_ran++;\
if (asserts_failed > 0) { \
tests_failed++; \
 printf("****%s: %d asserts failed out of %d asserts\n\n", #test, asserts_failed, asserts_ran); \
 }\
} while (0)

void test_create_vector(void) {
  vector_t *vec = new_vector();
  assert_ptr_notnull(vec);
  free_vector(vec);
}

void test_append(void){
  vector_t *vec = new_vector();
  assert_eq(len(vec), 0);

  append(vec, 0);
  assert_eq(len(vec), 1);

  // test append over capacitance
  int amount = cap(vec) * 2 + 1;
  char *data = malloc(amount);
  for (int i=0;i<amount;i++){
    append(vec, data[i]);
  }
  assert_eq(len(vec), amount+1);
  
  free_vector(vec);
  free(data);
}

void test_get(void){
  vector_t *vec = new_vector();
  int amount = cap(vec) * 2 + 1;
  char *data = malloc(amount);
  for (int i=0;i<amount;i++){
    data[i] = i;
    append(vec, data[i]);
  }

  for (int i=0; i<amount;i++){
    char got = getas(vec, i, char);
    assert_eq(got, data[i]);
  }

  free_vector(vec);
  free(data);
}

void test_insert(void){
  vector_t *vec = new_vector();
  append(vec, 1);
  append(vec, 2);
  append(vec, 3);

  insert(vec, 0, 5);
  assert_eq(len(vec), 4);
  assert_eq(getas(vec,0,int), 5);
  assert_eq(getas(vec,1,int),1);

  insert(vec, 1, 10);
  assert_eq(len(vec), 5);
  assert_eq(getas(vec,1,int), 10);

  int amount = cap(vec) *2 + 1;
  char *data = malloc(amount);
  for (int i=0; i<amount; i++) {
    data[i] = (char) i % 128;
    insert(vec, 0, (char) data[i]);
  }

  for (int i=0; i<amount; i++){
    char got = getas(vec, i, char);
    assert_eq(got, (char) data[amount-i-1] % 128);
  }

  free_vector(vec);
  free(data);
}

void test_del(void){
  vector_t *vec = new_vector();
  append(vec, 1);
  append(vec, 2);
  append(vec, 3);

  del(vec, 1);
  assert_eq(getas(vec,0,int),1);
  assert_eq(getas(vec,1,int),3);

  free_vector(vec);
}

void test_as_string(void){
  vector_t *vec = new_vector();
  char *str = "The quick brown fox jumps over the lazy dog.";
  for (int i=0;i < strlen(str); i++){
    append(vec, str[i]);
  }
  assert_eq(strcmp(as_string(vec), str),0);
  assert_eq(strcmp(as_string_from(vec, 3), &str[3]),0);

  free_vector(vec);
}

void run() {
  tests_ran = 0;
  tests_failed = 0;
  run_test(test_create_vector);
  run_test(test_append);
  run_test(test_get);
  run_test(test_insert);
  run_test(test_del);
  run_test(test_as_string);
  if (tests_failed == 0) {
    printf("********************\n"
           "WELL DONE!\n"
           "********************\n");
  } else {
    printf("****************************************************\n"
           "%d incorrect functions out of %d tested\n"
           "****************************************************\n\n", tests_failed, tests_ran);
  }
}

int main(int argc, char **argv) {
	run();
	return EXIT_SUCCESS;
}