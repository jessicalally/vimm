#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

#define NUM_CHARS 63

typedef struct TrieNode {
  
  struct TrieNode **children;
  bool complete_word;

} trie_t;

trie_t *init_trie(void);

void destroy_trie(trie_t *trie);

trie_t *create_node(void);

int get_pos(const char c);

char get_char(const int i);

bool find(trie_t *root, const char *word);

bool insert_word(trie_t *root, const char *word);

bool remove_word(trie_t *root, const char *word);

bool load_from_file(trie_t *root, const char *path);

bool remove_from_file(trie_t *root, const char *path);

#endif
