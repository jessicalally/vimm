#include "../include/editor.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "../include/vector.h"
#include "../include/trie.h"
#include "../include/auto_complete.h"
#include "../include/parser.h"

term_buf *init_term_buf(void) {
  vector_t *lines = new_vector();
  term_buf *term = calloc(1, sizeof(term_buf));
  term->lines = lines;
  return term;
}

void move_pos(pos_t pos){
  move(pos.line, pos.col);
}

pos_t cursor_pos(void) {
  pos_t pos;
  getyx(stdscr, pos.line, pos.col);
  return pos;
}

int num_lines(term_buf *term){
  return len(term->lines);
}

int visible_len(vector_t *line){
  int count = 0;
  for (int i = 0; i < len(line);i ++){
    if (get(line, i) != '\n'){
      count++;
    }
  }
  return count;
}

vector_t *cur_line(term_buf *term){
  return getas(term->lines, ln(term), vector_t *);
}

#define MIN(a,b) ((a) < (b) ? a : b)
#define MAX(a,b) ((a) > (b) ? a : b)

void render(term_buf *term){
  pos_t initial_pos = cursor_pos();
  pos_t limit;
  getmaxyx(stdscr, limit.line, limit.col);

  clear();
  pos_t cur_pos;
  cur_pos.line = cur_pos.col =0;
  move_pos(cur_pos);
  for (int i=term->top_ln; i < MIN(len(term->lines),limit.line); i++){
    vector_t *line_vec =getas(term->lines,i,vector_t *); 
    char *line = as_string(line_vec);
    if (visible_len(line_vec) - 1 <= limit.col){
      addstr(line);
      cur_pos.line++;
      cur_pos.col = 0;
      move(cur_pos.line, cur_pos.col);
    } else {
      int rest = visible_len(line_vec);
      for (int j = 0; j < rest; j++){
        addch(line[j]);
        if (j == limit.col){
          cur_pos.line++;
        }
      }
    }
  }
  move_pos(initial_pos);
}

bool scrol(term_buf *term, direction dir){
  switch(dir){
    case UP:
      if (term->top_ln >0){
        term->top_ln--;
        render(term);
        return true;
      }
      break;
    case DOWN:
    //TODO: test the bottom_ln instead?
      if (term->top_ln < num_lines(term)){
        term->top_ln++;
        render(term);
        return true;
      }
      break;
    default:
      break;
  }
  return false;
}

void move_cursor(term_buf *term, direction dir) {
  pos_t cur_pos = cursor_pos();
  pos_t limit;
  getmaxyx(stdscr, limit.line, limit.col);

  switch (dir) {
    case UP:
      if (cur_pos.line > 0) {
        cur_pos.line--;
        if (visible_len(cur_line(term)) > limit.col && col(term) > limit.col) {
          col(term) -= limit.col;
        } else {
          ln(term)--;
          col(term) = cur_pos.col = MIN(cur_pos.col, visible_len(cur_line(term)));
        }
      }
      break;
    case DOWN:
      if (visible_len(cur_line(term)) > limit.col && col(term) <= limit.col) {
        cur_pos.line++;
        col(term) += limit.col;
      } else if (ln(term) + 1 < num_lines(term)) {
        cur_pos.line++;
        ln(term)++;
        col(term) = cur_pos.col = MIN(cur_pos.col, visible_len(cur_line(term)));
      }
      break;
    case LEFT:
      if (col(term) == 0) {
        return;
      }
      col(term)--;
      if (cur_pos.col == 0) {
        cur_pos.line--;
        cur_pos.col = limit.col - 1;
      } else {
        cur_pos.col--;
      }
      break;
    case RIGHT:
      if (col(term) < visible_len(cur_line(term))) {
        col(term)++;
        if (cur_pos.col == limit.col - 1) {
          cur_pos.col = 0;
          cur_pos.line++;
        } else {
          cur_pos.col++;
        }
      }
      break;
    case NEUTRAL:
      break;
  }
  move(cur_pos.line, cur_pos.col);
}

#define delims (char[])  {' ', '(', '\t', '\n', '{', ';'}
bool is_delim(char c){
  for (int i=0; i < sizeof(delims); i++){
    if (delims[i] == c){
      return true;
    }
  }
  return false;
}
char *get_prompt(term_buf *term, trie_t *root) {
  vector_t *line = getas(term->lines, term->txt_pos.line, vector_t *);
  char *str = as_string(line);
  int i;
  for (i = term->txt_pos.col; !is_delim(str[i]) && i >= 0; i--);
  int start = i+1;
  int word_len = term->txt_pos.col - start + 1;
  char word[word_len];
  for (int i = 0; i < word_len;i++ ){
    word[i] = str[start+i];
  }
  return get_suggestion(root, word);
}

void insert_prompt(term_buf *term){
  assert(term->prompting);

  vector_t *line = getas(term->lines, ln(term), vector_t *);

  for (int i=0;i < strlen(term->prompt); i++){
    insert(line,col(term), term->prompt[i]);
    addch(term->prompt[i]);
    col(term)++;
  }
  term->prompting = false;
}

void generate_prompt(term_buf *term){
  pos_t cur_pos = cursor_pos();
  char *prompt = get_prompt(term, term->auto_trie);
  if (strlen(prompt) > 0){
    attron(COLOR_PAIR(1));
    addstr(prompt);
    attroff(COLOR_PAIR(1));
    move_pos(cur_pos);
    term->prompt = strdup(prompt);
    term->prompting = true;
  } else{
    term->prompting = false;
  }
}

void clear_prompt(term_buf *term){
  if (!term->prompting){
    return;
  }
  pos_t cur_pos = cursor_pos();
  for (int i=0;i<strlen(term->prompt);i++){
    addch(' ');
  }
  move_pos(cur_pos);
}

void insert_char(term_buf *term, char c) {
  if (ln(term) >= len(term->lines)) {
    vector_t *new_line = new_vector();
    append(term->lines, new_line);
  }

  clear_prompt(term);

  pos_t cur_pos = cursor_pos();
  vector_t *line = getas(term->lines, ln(term), vector_t *);

  if (c == '\n'){
    vector_t *new_line = new_vector();
    if (len(line) > col(term)) {
      append_str(new_line, as_string_from(line, col(term)));
    }
    del_line_from(line, col(term));
    ln(term)++;
    col(term) = 0;
    move(cur_pos.line+1, 0);
    insert(term->lines,ln(term), new_line);
    render(term);
    term->headers = parse_lines(term->auto_trie, term->lines, term->headers);
    return;
  }

  pos_t limit;
  getmaxyx(stdscr, limit.line, limit.col);
  
  if (cur_pos.col+1 >= limit.col){
    cur_pos.line++;
    cur_pos.col = 0;
  } else {
    cur_pos.col++;
  }

  insert(line, col(term), c);

  addstr(as_string_from(line, col(term)));
  col(term)++;
  render(term);
  if (col(term) == visible_len(line)){
    generate_prompt(term);
  }
  move_pos(cur_pos);

}

void backspace(term_buf *term) {
  pos_t limit;
  getmaxyx(stdscr, limit.line, limit.col);
  vector_t *line = getas(term->lines, ln(term), vector_t *);
  pos_t cur_pos = cursor_pos();
  if (col(term) == 0){
    if (ln(term) > 0){
      vector_t *prev_line = getas(term->lines, ln(term)-1, vector_t *);
      int old_prev_len = visible_len(prev_line);
      append_str(prev_line, as_string(line));
      del(term->lines, ln(term));
      ln(term)--;
      col(term) = old_prev_len; 
      move(cur_pos.line - 1, col(term) % limit.col);
      term->headers = parse_lines(term->auto_trie, term->lines, term->headers);
    }
  } else {
    del(line, col(term)-1);
    delch();
    col(term)--;
    if (cur_pos.col == 0) {
      cur_pos.line --;
      cur_pos.col = limit.col - 1;
    } else {
      cur_pos.col --;
    }
    move(cur_pos.line, cur_pos.col);
    generate_prompt(term);
  }
  render(term);
}

void open(term_buf *term, char *filename) {
  FILE *file = fopen (filename, "r");
  if (!file) {
    //error
    return;
  }
  char line [MAX_LINE_LENGTH];
  while (fgets(line, MAX_LINE_LENGTH, file)) {
    strtok(line, "\n");
    vector_t *vector = new_vector();
    append_str(vector, line);
    append(term->lines, vector);
  }
  render(term);
  fclose(file);
}

void save(term_buf *term, char *filename) {
  FILE *file = fopen(filename, "w");
  if (!file) {
    //error
    return;
  }
  vector_t *lines = term->lines;
  for (int i = 0; i < len(lines); i++) {
    fprintf(file, "%s\n", as_string(getas(lines, i, vector_t *)));
  }
  fclose(file);
}

void cleanup(void) { endwin(); }

int main(int argc, char **argv) {
  if (argc > 2){
    perror("Error: wrong number of arguments");
  }

  int c;
  initscr();
  start_color();
  raw();
  clear();
  noecho();
  cbreak(); /* Line buffering disabled. pass on everything */
  keypad(stdscr, TRUE);

  init_pair(1, COLOR_BLUE, COLOR_BLACK);

  atexit(cleanup);
  term_buf *term = init_term_buf();
  
  char *filename;
  if (argc == 2) {
    filename = argv[1];
    open(term, filename);
  }

  trie_t *auto_trie = init_trie();
  load_from_file(auto_trie, "src/function_lists/keywords.txt");
  term->auto_trie = auto_trie;
  render(term);
  while ((c = getch()) != ctrl('c')) {
    switch (c) {
      case ctrl('s'):
        if (argc == 2) {
          save(term, filename);
        }
        break;
      case BACKSPACE:
      case KEY_BACKSPACE:
        clear_prompt(term);
        backspace(term);
        break;
      case KEY_UP:
        move_cursor(term,UP);
        break;
      case KEY_DOWN:
        move_cursor(term,DOWN);
        break;
      case KEY_LEFT:
        move_cursor(term,LEFT);
        break;
      case KEY_RIGHT:
        move_cursor(term,RIGHT);
        break;
      case '\t':
        if (term->prompting){
          insert_prompt(term);
          break;
        } else {
          insert_char(term, ' ');
          insert_char(term, ' ');
          break;
        }
      default:
        insert_char(term, c);
        break;
    }

  }
  return EXIT_SUCCESS;
}
