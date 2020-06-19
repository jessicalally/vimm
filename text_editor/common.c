#include <stdlib.h>
#include <stdio.h>
#include "common.h"

void null_check(void *ptr) {
  if (!ptr) {
    perror("Error: memory allocation failed.");
    exit(EXIT_FAILURE);
  }
}
