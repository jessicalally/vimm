#ifndef AUTO_COMPLETE_H
#define AUTO_COMPLETE_H

#include "trie.h"

// returns the rest of the suggested word
char *get_suggestion(trie_t *root, const char *input);

#endif
