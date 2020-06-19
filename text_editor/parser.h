#ifndef PARSER_H
#define PARSER_H
#include "trie.h"
#include "vector.h"

void free_headers(char **headers);

char **parse_lines(trie_t *root, vector_t *lines, char **headers);

#endif
