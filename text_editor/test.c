#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "test.h"
#include "trie.h"
#include "queue.h"
#include "common.h"
#include "auto_complete.h"
#include "parser.h"
#include "vector.h"

int asserts_ran = 0;
int asserts_failed = 0;
int tests_ran = 0 ;
int tests_failed = 0;

// macros based on doublets past paper test
#define assert_eq(a, b) do {\
  asserts_ran++;\
  if ((a) != (b)) {\
    printf("%s(line %d): got: %d | expected: %d\n", __func__, __LINE__, a, b);\
    asserts_failed++;\
  }\
} while (0)

#define assert_ptr_notnull(p) do {\
  asserts_ran++;\
  if (p == NULL) {\
    printf("%s(line %d): pointer should not be NULL\n", __func__, __LINE__);\
    asserts_failed++;\
    return;\
  } \
} while (0)

#define run_test(test) do {\
  asserts_ran = 0;\
  asserts_failed = 0;\
  test(); \
  tests_ran++;\
  if (asserts_failed > 0) {\
    tests_failed++;\
     printf("****%s: %d asserts failed out of %d asserts\n\n", #test, asserts_failed, asserts_ran);\
     }\
} while (0)

void test_init_trie(void) {
  trie_t *trie = init_trie();
  assert_ptr_notnull(trie);

  destroy_trie(trie);
}

void test_get_pos(void) {
  assert_eq(get_pos('a'), 0);
  assert_eq(get_pos('c'), 2);
  assert_eq(get_pos('z'), 25);
  assert_eq(get_pos('A'), 26);
  assert_eq(get_pos('C'), 28);
  assert_eq(get_pos('Z'), 51);
  assert_eq(get_pos('0'), 52);
  assert_eq(get_pos('4'), 56);
  assert_eq(get_pos('9'), 61);
  assert_eq(get_pos('_'), 62);
  assert_eq(get_pos('&'), INT_MAX);
}

void test_get_char(void) {
  assert_eq(get_char(0), 'a');
  assert_eq(get_char(2), 'c');
  assert_eq(get_char(25), 'z');
  assert_eq(get_char(26), 'A');
  assert_eq(get_char(28), 'C');
  assert_eq(get_char(51), 'Z');
  assert_eq(get_char(52), '0');
  assert_eq(get_char(56), '4');
  assert_eq(get_char(61), '9');
  assert_eq(get_char(62), '_');
  assert_eq(get_char(63), CHAR_MAX);
}

void test_trie_insert(void) {
  trie_t *trie = init_trie();

  assert_eq(insert_word(trie, "strtok_r"), true);
  assert_eq(insert_word(trie, "hypot"), true);
  assert_eq(insert_word(trie, "strtok_r"), false);
  assert_eq(insert_word(trie, "strstr"), true);
  assert_eq(insert_word(trie, "tcsetattr"), true);
  assert_eq(insert_word(trie, "strstr"), false);
  assert_eq(insert_word(trie, "he&llo"), false);
  assert_eq(find(trie, "strstr"), true);
  assert_eq(find(trie, "he&llo"), false);

  destroy_trie(trie);
}

void test_trie_remove(void) {
  trie_t *trie = init_trie();

  assert_eq(insert_word(trie, "strtok_r"), true);
  assert_eq(find(trie, "strtok_r"), true);
  assert_eq(remove_word(trie, "strtok_r"), true);
  assert_eq(find(trie, "strtok_r"), false);
  assert_eq(remove_word(trie, "tcsetattr"), false);
  assert_eq(remove_word(trie, "he&llo"), false);

  destroy_trie(trie);
}

void test_load_from_file() {
  trie_t *trie = init_trie();
  assert_eq(load_from_file(trie, "test.txt"), true);
  assert_eq(find(trie, "hello"), true);
  destroy_trie(trie);
}

void test_remove_from_file() {
  trie_t *trie = init_trie();

  assert_eq(load_from_file(trie, "test.txt"), true);
  assert_eq(find(trie, "hello"), true);
  assert_eq(remove_from_file(trie, "test.txt"), true);
  assert_eq(find(trie, "hello"), false);
  assert_eq(insert_word(trie, "fakeword"), true);
  assert_eq(find(trie, "fakeword"), true);
  assert_eq(remove_from_file(trie, "test.txt"), true);
  assert_eq(find(trie, "fakeword"), true);

  destroy_trie(trie);
}

void test_init_queue(void) {
  queue_t *queue = init_queue();
  assert_ptr_notnull(queue);

  free_queue(queue);
}

void test_queue(void) {
  queue_t *queue = init_queue();
  assert_eq(is_empty(queue), true);

  pair_t first = {create_node(), "a"};
  pair_t second = {create_node(), "b"};

  enqueue(queue, first);
  assert_eq(is_empty(queue), false);
  enqueue(queue, second);
  assert_eq(is_empty(queue), false);
  
  pair_t first_returned = dequeue(queue);

  assert_eq(strcmp(first_returned.str, first.str), 0);
  assert_eq(is_empty(queue), false);

  pair_t second_returned = dequeue(queue);

  assert_eq(strcmp(second_returned.str, second.str), 0);
  assert_eq(is_empty(queue), true);

  destroy_trie(first.node);
  destroy_trie(second.node);

  free_queue(queue);
}

void test_auto_complete(void) {
  trie_t *trie = init_trie();
  assert_eq(load_from_file(trie, "function_lists/stdio.txt"), true);
  
  char *suggestion = get_suggestion(trie, "f");
  assert_eq(strcmp(suggestion, "eof"), 0);
  free(suggestion);

  suggestion = get_suggestion(trie, "fg");
  assert_eq(strcmp(suggestion, "etc"), 0);
  free(suggestion);

  suggestion = get_suggestion(trie, "fpr");
  assert_eq(strcmp(suggestion, "intf"), 0);
  free(suggestion);

  suggestion = get_suggestion(trie, "fprintf");
  assert_eq(strcmp(suggestion, ""), 0);
  free(suggestion);

  suggestion = get_suggestion(trie, "");
  assert_eq(strcmp(suggestion, ""), 0);
  free(suggestion);

  suggestion = get_suggestion(trie, "£$asdewf£%$^%&");
  assert_eq(strcmp(suggestion, ""), 0);
  free(suggestion);

  destroy_trie(trie);
}

void free_2d_vector(vector_t *vec) {
  for (int i = 0; i < len(vec); i++) {
    free_vector(getas(vec, i, vector_t *));
  }
  free_vector(vec);
}

void test_header_parser(void) {
  trie_t *trie = init_trie();

  char *orig_text[4] = {"#include <stdio.h>\n", "", "#include <stdlib.h>\n", "printf(\"Hello\");\n"};
  vector_t *lines = new_vector();
  for (int i = 0; i < 4; i++){
    vector_t *line = new_vector();
    append_str(line, orig_text[i]);
    append(lines, line);
  }

  char **headers = parse_lines(trie, lines, NULL);
  free_2d_vector(lines);

  assert_eq(strcmp(headers[0], "stdio"), 0);
  assert_eq(strcmp(headers[1], "stdlib"), 0);
  assert_eq(find(trie, "printf"), true);
  assert_eq(find(trie, "malloc"), true);
  assert_eq(find(trie, "assert"), false);

  char *mod_text[3] = {"#include <stdio.h>\n", "", "printf(\"Hello\");\n"};
  lines = new_vector();
  for (int i = 0; i < 3; i++) {
    vector_t *line = new_vector();
    append_str(line, mod_text[i]);
    append(lines, line);
  }

  headers = parse_lines(trie, lines, headers);
  free_2d_vector(lines);

  assert_eq(strcmp(headers[0], "stdio"), 0);
  assert_eq(find(trie, "printf"), true);
  assert_eq(find(trie, "malloc"), false);
  assert_eq(find(trie, "assert"), false);

  free_headers(headers);
  destroy_trie(trie);
}

void run_tests(void) {
  run_test(test_init_trie);
  run_test(test_get_pos);
  run_test(test_get_char);
  run_test(test_trie_insert);
  run_test(test_trie_remove);
  run_test(test_load_from_file);
  run_test(test_remove_from_file);
  run_test(test_init_queue);
  run_test(test_queue);
  run_test(test_auto_complete);
  run_test(test_header_parser);

  if (tests_failed == 0) {
    printf("Passes all tests!\n");
  } else {
    printf("%d failed tests out of %d tested\n", tests_failed, tests_ran);
  }
}

int main(int argc, char **argv) {
  run_tests();

  if (argc == 2) {
    trie_t *trie = init_trie();

    if (!load_from_file(trie, argv[1])) {
      fprintf(stderr, "Error: Failed to load file at %s\n", argv[1]);
      exit(EXIT_FAILURE);
    }

    char input[MAX_LENGTH] = "";
    fgets(input, MAX_LENGTH, stdin);
    strtok(input, "\n");


    while (strcmp(input, "quit") != 0) {
      printf("%s", "\x1B[A");
      char *suggestion = get_suggestion(trie, input);
      printf("%s%s\n", input, suggestion);
      free(suggestion);
      fgets(input, MAX_LENGTH, stdin);
      strtok(input, "\n");
    }
  }

  return EXIT_SUCCESS;
}
