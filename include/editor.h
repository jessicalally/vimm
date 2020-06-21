#ifndef EDITOR_H
#define EDITOR_H

#include "vector.h"
#include "trie.h"
#include <unistd.h>

#define BACKSPACE 127
#define ctrl(x) ((x)&0x1f)
#define MAX_LINE_LENGTH 512

typedef struct pos {
  int line;
  int col;
} pos_t;

typedef struct {
  vector_t *lines;
  pos_t txt_pos;
  int top_ln;
  trie_t *auto_trie;

  bool prompting;
  char *prompt;
  char **headers;
} term_buf;

typedef enum { NEUTRAL, UP, DOWN, LEFT, RIGHT } direction;

#define ln(term) term->txt_pos.line
#define col(term) term->txt_pos.col

//#define clear() write(STDIN_FILENO, "\x1b[2J", 4)
#define erase_line() write(STDIN_FILENO, "\x1b[2K", 4)

#endif
