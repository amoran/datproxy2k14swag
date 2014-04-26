#include <stdio.h>

/* Include datproxy2k14swag headers. */
#include "html.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

int main()
{
    printf("%s%s%s", user_agent_hdr, accept_hdr, accept_encoding_hdr);

    return 0;
}
