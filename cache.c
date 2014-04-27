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

                   "Deleted code is debugged code."
                               -Adam, April 2014

A Cache implementation to be used in our web proxy.

 */

#include "cache.h"
#include <string.h>

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
  pthread_mutex_init(&part->locked, NULL);
  part->first = NULL;
  return part;
}

int hash(char* url) {
  int bruce = 1435; //the sum of "BruceGreenwood"
  int c;

  while (c = *url++) {
    bruce = c + (bruce << 6) + (bruce << 16) - bruce;
  }

  return hash % PARTITION_QUANTITY;
}


cache_object cache_search(cache_t swag_cache, char* url) {
  int target_part_num = hash(url);
  cache_part target_part = swag_cache->parts[target_part_num];
  cache_object cur_obj = target_part->first;
  cache_object final_obj = NULL;

  int cur_lock;
  cur_lock = pthread_mutex_lock(&target_part->locked);
  if (cur_lock != 0) {
    //couldn't get lock
    return NULL;
  }

  while (cur_obj != NULL) {
    if (strcmp(url, cur_obj->url) == 0) {
      final_obj = cur_obj;
      break;
    }
    cur_obj = cur_obj->next;
  }

  if (final_obj = NULL) {
    //didn't find a match, releasing lock
    pthread_mutex_unlock(&target_part->locked);
  }

  if (final_obj->prev != NULL) {
    cache_object gap_prev = final_obj->prev;
    cache_object gap_next = final_obj->next;

    //insert final_obj at beginning of partition
    target_part->first->prev = final_obj;
    final_obj->next = target_part->first;
    final_obj->prev = NULL;
    target_part->first = final_obj;

    gap_prev->next = gap_next;
    if (gap_next != NULL) {
      gap_next->prev = gap_prev;
    }
  }
  pthread_mutex_unlock(&target_part->locked);
  return final_obj;
}


cache_t cache_insert(cache_t c, char* url, void* ptr, int obj_size) {

  int target_part_num = hash(url);
  cache_part target_part = swag_cache->parts[target_part_num];
  int cur_lock;


  cur_lock = pthread_mutex_lock(&target_part->locked);

  if (cur_lock == 0) {
    target_part->size+=obj_size;
    cache_object tracker = target_part->first;

    cache_object cur_obj = malloc(sizeof(struct cache_object));
    cur_obj->url = url;
    cur_obj->size = obj_size;
    cur_obj->value = malloc(obj_size);
    memcpy(cur_obj->ptr_to_item, ptr, obj_size);
    strcpy(cur_obj->url, url);

    while (1) {
      if (tracker->next == NULL) {
        break;
      }
      tracker = tracker->next
    }

    cach_object temp;
    while (target_part->size > (c->size/PARTITION_QUANTITY)) {
      target_part->size -= tracker->size;
      //free target_part TODO
      tracker = tracker->prev
      tracker->next = NULL;
    }


    if (target_part->first == NULL) {
      //insert in empty
      cur_obj->next = NULL;
      cur_obj->prev = NULL;
      target_part->first = cur_obj;
    }
    else {
      //insert in beginning
      target_part->first->prev = cur_obj;
      cur_obj->next = target_part->first;
      cur_obj->prev = NULL;
      target_part->first = cur_obj;
    }

    pthread_mutex_unlock(&target_part->locked);
  }
  return;
}

void object_free(cache_object cur_obj) {
  free(cur_obj->url);
  free(cur_obj->ptr_to_item);
  free(cur_obj);
  return;
}
