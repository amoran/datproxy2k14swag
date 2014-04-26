/*

                    John Mann and Adam Moran Present
                ____        _    ____           _
               |  _ \  __ _| |_ / ___|__ _  ___| |__   ___
               | | | |/ _` | __| |   / _` |/ __| '_ \ / _ \
               | |_| | (_| | |_| |__| (_| | (__| | | |  __/
               |____/ \__,_|\__|\____\__,_|\___|_| |_|\___|
              ____  _    _ _  _     ______        ___    ____
             |___ \| | _| | || |   / ___\ \      / / \  / ___|
               __) | |/ / | || |_  \___ \\ \ /\ / / _ \| |  _
              / __/|   <| |__   _|  ___) |\ V  V / ___ \ |_| |
             |_____|_|\_\_|  |_|   |____/  \_/\_/_/   \_\____|


                "Did I get jiggy with Will? I would have
                 to say no! Did I have fun with Will? I
                 would have to say yeah! But you know, I
                 did not jig with the man, if you know
                 what I'm saying!"
                                -Adam Moran

cache.h

*/

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
