/*

                        John Mann and Adam Moran Present

               _____        _   _____  _____   ______   ____     __
              |  __ \      | | |  __ \|  __ \ / __ \ \ / /\ \   / /
              | |  | | __ _| |_| |__) | |__) | |  | \ V /  \ \_/ /
              | |  | |/ _` | __|  ___/|  _  /| |  | |> <    \   /
              | |__| | (_| | |_| |    | | \ \| |__| / . \    | |
              |_____/ \__,_|\__|_|    |_|  \_\\____/_/ \_\   |_|
          ___  _   __ _  _       _  _    _______          __     _____
         |__ \| | /_ | || |    _| || |_ / ____\ \        / /\   / ____|
            ) | | _| | || |_  |_  __  _| (___  \ \  /\  / /  \ | |  __
           / /| |/ / |__   _|  _| || |_ \___ \  \ \/  \/ / /\ \| | |_ |
          / /_|   <| |  | |   |_  __  _|____) |  \  /\  / ____ \ |__| |
         |____|_|\_\_|  |_|     |_||_| |_____/    \/  \/_/    \_\_____|

                     "Deleted code is debugged code."
                                             -Adam, April 2014

A Cache implementation to be used in our web proxy.
 */

#include "cache.h"
#include <string.h>


/**
 * cache_init: size_t -> cache_t
 *
 * Called to initialize all parts of the cache.
 *
 * Parameters:
 *      alloc_size (size_t): the size of the cache.  Usually max_cache_size.
 *
 * Returns:
 *      the cache, completely initialized, partitions and all
 *
 */
cache_t cache_init (size_t alloc_size) {
    int i;
    cache_t swag_cache = malloc(sizeof(struct cache_t));
    swag_cache->size = alloc_size;

    for (i = 0; i < PARTITION_QUANTITY; i++) {
        swag_cache->parts[i] = part_init();
    }

    return swag_cache;
}

/**
 * part_init: void -> cache_part
 *
 * Called from cache_init to initialize all of the partitions in the cache.
 *
 * Parameters:
 *      alloc_size (size_t): the size of the cache.  Usually max_cache_size.
 *
 * Returns:
 *      the completely initialized partition.
 *
 */
cache_part part_init () {
    cache_part part;
    int success;

    part = malloc(sizeof(struct cache_part));
    part->size = 0;

    success = pthread_mutex_init(&part->locked, NULL);
    if (success != 0) {
        printf("Oh shit... :/\n");
    }

    part->first = NULL;
    return part;
}

/**
 * hash: url -> int
 *
 * Hashes a string (the url) into an integer using.  This is the sdbm
 * hashing algorithm implemented to reflect the awesomeness of Bruce
 * Greenwood.
 *
 * Parameters:
 *      url (char*) -> int
 *
 * Returns:
 *      an integer representing which partition the key will be stored in.
 *
 */
unsigned int hash(char* url) {
    int bruce = 1435; //the sum of "BruceGreenwood"
    int c;

    while ((c = *url++)) {
        bruce = c + (bruce << 6) + (bruce << 16) - bruce;
    }

    return ((unsigned int)bruce % PARTITION_QUANTITY);
}

/**
 * cache_search: cache_t , (char*) -> cache_object
 *
 * Searches the cache for the url given.  It finds the corresponding
 * partition and mutex locks it to find a match.  If it finds a match
 * it moves it to the beginning of the partition (to signify most recently
 * used) and returns it.  If it doesn't find a match, it returns NULL.
 *
 * Parameters:
 *      url (char*) * swag_cache (cache_t) -> cache_object
 *
 * Returns:
 *      the cache_object that it found. If it didn't find a cache object
 *      it returns NULL, signifying the object wasn't in the cache
 *
 */
cache_object cache_search(cache_t swag_cache, char* url) {
    unsigned int target_part_num = hash(url);
    cache_part target_part = swag_cache->parts[target_part_num];
    cache_object cur_obj = target_part->first;
    cache_object final_obj = NULL;

    int cur_lock = 0;
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

    if (final_obj == NULL) {
        //didn't find a match, releasing lock
        pthread_mutex_unlock(&target_part->locked);
    }

    else if (final_obj->prev != NULL) {
        cache_object gap_prev = final_obj->prev;
        cache_object gap_next = final_obj->next;

        /* Insert final_obj at beginning of partition. */
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

/**
 * cache_insert: cache_t , (char*) , (void*) , int -> void
 *
 * Takes a url and the object at that url and stores it into the provided
 * cache.  It stores it at the beginning of the partition returned by
 * hash(url).  The beginning is the most recently used.
 *
 * Parameters:
 *      url, ptr, obj_size, c
 *
 * Returns:
 *      The cache with the object inserted.
 *
 */
cache_t cache_insert(cache_t c, char* url, void* ptr, int obj_size) {
    /* Notes about THE FUTURE:
     * Check for object size greater than MAX_OBJECT_SIZE. */
    int target_part_num = hash(url);
    cache_part target_part = c->parts[target_part_num];
    int cur_lock;

    cur_lock = pthread_mutex_lock(&target_part->locked);

    if (cur_lock == 0) {
        target_part->size+=obj_size;
        cache_object tracker = target_part->first;

        cache_object cur_obj = malloc(sizeof(struct cache_object));
        cur_obj->url = url;
        cur_obj->size = obj_size;
        cur_obj->ptr_to_item = malloc(obj_size);
        memcpy(cur_obj->ptr_to_item, ptr, obj_size);
        strcpy(cur_obj->url, url);

        if (target_part->first == NULL) {
            /* We have an empty list. Splice in the new object with NULL
             * links. */
            cur_obj->next = NULL;
            cur_obj->prev = NULL;
            target_part->first = cur_obj;
            pthread_mutex_unlock(&target_part->locked);
            return c;
        }

        /* Venture to the end of the list. */
        while (tracker->next != NULL) {
            tracker = tracker->next;
        }

        while (target_part->size > (c->size/PARTITION_QUANTITY)) {
            target_part->size -= tracker->size;
            //free target_part TODO
            tracker = tracker->prev;
            tracker->next = NULL;
        }

        if (target_part->first == NULL) {
            //insert in empty
            cur_obj->next = NULL;
            cur_obj->prev = NULL;
            target_part->first = cur_obj;
        }

        /* Insert in beginning since the new object is the most recently
         * used. */
        target_part->first->prev = cur_obj;
        cur_obj->next = target_part->first;
        cur_obj->prev = NULL;
        target_part->first = cur_obj;

        pthread_mutex_unlock(&target_part->locked);
    }

    return c;
}

void object_free(cache_object cur_obj) {
    free(cur_obj->url);
    free(cur_obj->ptr_to_item);
    free(cur_obj);
    return;
}
