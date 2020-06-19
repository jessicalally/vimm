#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include "trie.h"
#include "common.h"

// includes all lower case, upper case characters, underscore and digits
#define ALPHABET_SIZE 26
#define MAX_LINE_CHARS 100

trie_t *init_trie(void) {
  return create_node();
}

void destroy_trie(trie_t *trie) {
  if (trie) { 
    for (int i = 0; i < NUM_CHARS; i++) {
      destroy_trie(trie->children[i]);
    }

    free(trie->children);
    free(trie);
  }
}

trie_t *create_node(void) {
  trie_t *new = malloc(sizeof(trie_t));
  null_check(new);

  new->children = calloc(NUM_CHARS, sizeof(trie_t *));
  null_check(new->children);

  new->complete_word = false;
  
  return new;
}

// gets the position of a character in the children array
int get_pos(const char c) {
  if (c >= 'a' && c <= 'z') {
    return c - 'a';
  } else if (c >= 'A' && c <= 'Z') {
    return ALPHABET_SIZE + (c - 'A');
  } else if (c >= '0' && c <= '9') {
    return (ALPHABET_SIZE * 2) + (c - '0');
  } else if (c == '_') {
    return NUM_CHARS - 1;
  }

  return INT_MAX;  
}

// gets the character from a position in the children array
char get_char(const int i) {
 if (i >= 0 && i < ALPHABET_SIZE) {
    return 'a' + i;
  } else if (i >= ALPHABET_SIZE && i < ALPHABET_SIZE * 2) {
    return 'A' + (i - ALPHABET_SIZE);
  } else if (i >= 2 * ALPHABET_SIZE && i < (2 * ALPHABET_SIZE) + 10) {
    return '0' + (i - (2 * ALPHABET_SIZE));
  } else if (i == NUM_CHARS - 1) {
    return '_';  }

  return CHAR_MAX;
}

// returns true if word is present in tree
bool find(trie_t *root, const char *word) {
  assert(root);
  assert(root->children);
  assert(word);

  int c_pos = get_pos(word[0]);
  
  if (c_pos == INT_MAX) {
    return false;
  }

  if (!root->children[c_pos]) {
    return false;
  }

  if (word[1] == '\0') {
    if (root->children[c_pos]->complete_word) {
      return true;
    } 
    return false;
  }

  return find(root->children[c_pos], word + sizeof(char));
}

// return false if word is already in the tree, inserts word into tree otherwise
bool insert_word(trie_t *root, const char *word) {
  assert(root);
  assert(root->children);
  assert(word);

  int c_pos = get_pos(word[0]);
  if (c_pos == INT_MAX) {
    return false;
  }

  if (!root->children[c_pos]) {
    trie_t *new_node = create_node();
    root->children[c_pos] = new_node;
  }

  if (word[1] == '\0') {
    if (root->children[c_pos]->complete_word) {
      return false;
    }
    root->children[c_pos]->complete_word = true;
    return true;
  } else {
    return insert_word(root->children[c_pos], word + sizeof(char));
  }
}

bool remove_word(trie_t *root, const char *word) {
  assert(root);
  assert(root->children);
  assert(word);

  int c_pos = get_pos(word[0]);
  if (c_pos == INT_MAX || !root->children[c_pos]) {
    return false;
  }

  if (word[1] == '\0') {
    root->children[c_pos]->complete_word = false;
    return true;
  } else {
    return remove_word(root->children[c_pos], word + sizeof(char));
  }
}

static bool update_from_file(trie_t *root, const char *path, bool insert) {
  FILE *file = fopen(path, "r");

  if (!file) {
    return false;
  }
  
  char word[MAX_LINE_CHARS];

  while (fgets(word, MAX_LINE_CHARS, file)) {
    strtok(word, "\n");
    strtok(word, "\r");
    if (insert) {
      insert_word(root, word);
    } else {
      remove_word(root, word);
    }
  }

  fclose(file);
  return true;
}

bool load_from_file(trie_t *root, const char *path) {
  return update_from_file(root, path, true);
}

bool remove_from_file(trie_t *root, const char *path) {
  return update_from_file(root, path, false);
}
