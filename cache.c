/* Adam Moran and John Mann
 * amoran and jmannjr
 *
 * A Cache implementation to be used in our web proxy.
 *
 */

#define MAX_CACHE_SIZE 1049000;
#define MAX_OBJECT_SIZE 102400;

#include "cache.h"

cache_t* cache_init (size_t alloc_size) {
  cache_t swag_cache = malloc(sizeof(struct cache_t));
  swag_cache->size = alloc_size;


}
