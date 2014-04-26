#include <stdlib.h>
#include <stdio.h>
#include "csapp.h"

#define PARTITION_QUANTITY

/* Each object forms a node in a linked list */
struct cache_object {
  size_t size;
  char* uri;
  unsigned int lru;
  struct cache_object *next;
};

typedef struct cache_object cache_object;

/* Each partition has a pointer to the first element in their linked list */
struct cache_part {
  size_t size;
  int locked;
  cache_object *first;
};

typedef struct cache_part cache_part;

/* The main cache has an array of partition structs */
struct cache_t {
  size_t size;
  cache_part parts[PARTITION_QUANTITY];
};

typedef struct cache_t cache_t;
