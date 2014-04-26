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

"Your problem is how you are going to spend this one odd and precious life
 you have been issued. Whether you're going to spend it trying to look good
 and creating the illusion that you have power over people and circumstances,
 or whether you are going to taste it, enjoy it and find out the truth about
 who you are."
               -John Mann, April 2014

A Cache implementation to be used in our web proxy.

 */

#include "cache.h"

#define MAX_CACHE_SIZE 1049000;
#define MAX_OBJECT_SIZE 102400;


cache_t* cache_init (size_t alloc_size) {
  cache_t swag_cache = malloc(sizeof(struct cache_t));
  swag_cache->size = alloc_size;

  for (int i = 0; i < PARTITION_QUANTITY; i++) {
    swag_cache->parts[i] = part_init();
  }
}

cache_part part_init () {
  cache_part part = malloc(sizeof(struct cache_part));
  part->size = 0;
  part->locked = 0;
  part->cacheobject = NULL;
  return part;
}

//TODO:
//cache_search
//  -> cache_insert
//      -> object_init
//  -> cache_remove
//      -> object_free
//cache_free
//  -> parts_free
