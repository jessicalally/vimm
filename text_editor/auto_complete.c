#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "auto_complete.h"
#include "trie.h"
#include "queue.h"
#include "common.h"

// if the current user input is a word contained in the tree
// get_suggestion will return an empty string, otherwise get_suggestion
// will perform a BFS to return the shortest word contained in
// the tree that matches the start of the user input

char *get_suggestion(trie_t *root, const char *input) {
  assert(root);
  assert(root->children);
  assert(input);

  trie_t *curr = root;
  char *str = calloc(MAX_LENGTH, sizeof(char));
  null_check(str);

  // dont suggest any word if the input is empty.
  if (input[0] == '\0') {
    return str;
  }
  
  for (int i = 0; input[i] != '\0'; i++) {
    int c_pos = get_pos(input[i]);
    if (c_pos >= NUM_CHARS || c_pos < 0 || !curr->children[c_pos]) {
      // user input does not match any function/keyword in tree
      return str;
    } 
    curr = curr->children[c_pos];
  }

  // we now perform a BFS from curr 
  queue_t *queue = init_queue();
  pair_t curr_pair = {curr, str};
  enqueue(queue, curr_pair);

  while (!is_empty(queue)) {
    curr_pair = dequeue(queue);
    if (curr_pair.node->complete_word) {
      flush_queue(queue);
      free_queue(queue);
      return curr_pair.str;
    } 

    for (int i = 0; i < NUM_CHARS; i++) {
      if (curr_pair.node->children[i]) {
        char *copy = calloc(MAX_LENGTH, sizeof(char));
        null_check(copy);
        strcpy(copy, curr_pair.str);

        char c = get_char(i);
        pair_t next_pair = {curr_pair.node->children[i], strncat(copy, &c, sizeof(char))};
        enqueue(queue, next_pair);
      }
    }

    free(curr_pair.str);
  }

  free_queue(queue);
  str = calloc(MAX_LENGTH, sizeof(char));
  
  return str;
}
