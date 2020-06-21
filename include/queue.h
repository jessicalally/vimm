#include <stdbool.h>
#include "trie.h"

#define SIZE 10000

typedef struct Pair {
  trie_t *node;
  char *str;
} pair_t;

typedef struct Queue {
  pair_t nodes[SIZE];
  int front;
  int rear;
  int size;
} queue_t;

queue_t *init_queue(void);

void flush_queue(queue_t *queue);

void free_queue(queue_t *queue);

void enqueue(queue_t *queue, pair_t node);

pair_t dequeue(queue_t *queue);

bool is_empty(queue_t *queue);

