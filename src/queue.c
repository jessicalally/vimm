#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../include/queue.h"
#include "../include/common.h"
#include "../include/trie.h"

queue_t *init_queue(void) {
  queue_t *queue = malloc(sizeof(queue_t));
  null_check(queue);

  queue->front = 0;
  queue->rear = -1;
  queue->size = 0;

  return queue; 
}

// frees all strings left in queue
void flush_queue(queue_t *queue) {
  while (!is_empty(queue)) {
    pair_t pair = dequeue(queue);
    free(pair.str);
  }
}

void free_queue(queue_t *queue) {
  free(queue);
}

void enqueue(queue_t *queue, pair_t node) {
  assert(queue);
  assert(queue->nodes);
  
  if (queue->size == SIZE) {
    // queue is full
    perror("Error: queue is full");
    exit(EXIT_FAILURE);
  }

  queue->rear++;
  queue->size++;
  queue->nodes[queue->rear] = node;
}

pair_t dequeue(queue_t *queue) {
  assert(queue);
  assert(queue->nodes);
  assert(!is_empty(queue));

  pair_t next = queue->nodes[queue->front];
  queue->front++;
  queue->size--;

  if (queue->front > queue->rear) {
    // resets queue
    queue->front = 0;
    queue->rear = -1;
  }

  return next;
}

bool is_empty(queue_t *queue) {
  assert(queue);

  return queue->size == 0;
}

