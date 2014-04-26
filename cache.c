/*

                    John Mann and Adam Moran Present
                 ____        _   ____
                |  _ \  __ _| |_|  _ \ _ __ _____  ___   _
                | | | |/ _` | __| |_) | '__/ _ \ \/ / | | |
                | |_| | (_| | |_|  __/| | | (_) >  <| |_| |
                |____/ \__,_|\__|_|   |_|  \___/_/\_\\__, |
                                                     |___/
                ____  _    _ _  _  ____
               |___ \| | _/ | || |/ ___|_      ____ _  __ _
                 __) | |/ / | || |\___ \ \ /\ / / _` |/ _` |
                / __/|   <| |__   _|__) \ V  V / (_| | (_| |
               |_____|_|\_\_|  |_||____/ \_/\_/ \__,_|\__, |
                                                      |___/

                   "Deleted code is debugged code.
                               -Adam, April 2014

A Cache implementation to be used in our web proxy.

 */

#include "cache.h"

#define MAX_CACHE_SIZE 1049000;
#define MAX_OBJECT_SIZE 102400;


cache_t cache_init (size_t alloc_size) {
  int i;
  cache_t swag_cache = malloc(sizeof(struct cache_t));
  swag_cache->size = alloc_size;

  for (i = 0; i < PARTITION_QUANTITY; i++) {
    swag_cache->parts[i] = part_init();
  }

  return swag_cache;
}

cache_part part_init () {
  cache_part part = malloc(sizeof(struct cache_part));
  part->size = 0;
  part->locked = 0;
  part->first = NULL;
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
