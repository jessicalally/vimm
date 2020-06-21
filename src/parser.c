#include <string.h>
#include <assert.h>
#include "../include/common.h"
#include "../include/parser.h"
#include "../include/trie.h"
#include "../include/vector.h"

#define MAX_HEADER_LEN 100
#define MAX_PATH_LEN 120
#define INCLUDE_MACRO "#include"

void free_headers(char **headers) {
  if (headers) {
    for (int i = 0; headers[i]; i++) {
      free(headers[i]);
    }

    free(headers);
  }
}

static bool add(char ***headers, char *header) {
  assert(headers);
  assert(*headers);
  assert(header);

  int i = 0;

  for (; (*headers)[i]; i++) {
    if (strcmp((*headers)[i], header) == 0) {
      return false;
    }
  }
  
  *headers = realloc(*headers, (i + 2) * sizeof(char *));
  null_check(*headers);

  (*headers)[i] = header;
  (*headers)[i + 1] = NULL;
  
  return true;
}

void compare_headers(trie_t *root, char **orig, char **new) {
  if (orig) {
    for (int i = 0; orig[i]; i++) {
      // see if header is present in new headers array
      bool is_present = false;

      for (int j = 0; new[j]; j++) {
        if (strcmp(orig[i], new[j]) == 0) {
          is_present = true;
          break;
        }
      }

      if (!is_present) {
        char path[MAX_PATH_LEN] = "src/function_lists/";
        strcat(path, orig[i]);
        strcat(path, ".txt");
        remove_from_file(root, path);
      }
    }

    free_headers(orig);
  } 
}


char **parse_lines(trie_t *root, vector_t *lines, char **headers) {
  char **new_headers = calloc(1, sizeof(char *));

  for (int i = 0; i < len(lines); i++) {
    vector_t *line = getas(lines, i, vector_t *);
    char *copy = strdup(as_string(line));
    char *orig_copy = copy;
    null_check(copy);

    while (*copy == ' ') {
      ++copy;
    }

    if (strncmp(copy, INCLUDE_MACRO, strlen(INCLUDE_MACRO)) == 0) {
      // line declares a header file
      strtok_r(copy, "<\"", &copy);
      char *header = strdup(strtok(copy, "."));
      if (add(&new_headers, header)) {
        char path[MAX_PATH_LEN] = "src/function_lists/";
        strcat(path, header);
        strcat(path, ".txt");
        load_from_file(root, path);
      }
    }

    free(orig_copy);
  }

  compare_headers(root, headers, new_headers);

  return new_headers;
}

