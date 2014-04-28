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


                "North Carolina is the best state."
                                - Adam Moran

                "North Carolina is the best state."
                                - John Mann

                "I'm Canadian. British Columbia is my favorite
                 providence. I'd like to visit North Carolina
                 and decide for myself."
                                - Bruce Greenwood
*/

#include "request.h"

/**
 * send_request : html_header_data -> int
 *
 * Called when we want to fire off a request whose header matches the one
 * provided for us.
 *
 * Parameters:
 *      header (html_header_data): The data we use to create the request header.
 *
 * Returns:
 *      An integer representing the socket file to read from later.
 */
int send_request(html_header_data header) {
    /* Setup. */
    char buffer[MAXLINE];

    /* Get the host and port we're interested in. */
    char *method = header->method;
    char *directory = header->directory;
    int port = header->port;

    /* Declare extra header variables. */
    char *extra_header;
    int a = 0;

    /* Try to open a connection with the host on the port. */
    int conn_file;

    /* Calculate the content length. */
    int content_length = 0;

    /* DEBUG: Print the data. */
    printf("BEGIN PRINTING HEADER:\n");
    print_html_header_data(header);

    /* Write headers to the connection. */
    sprintf(buffer, "%s %s HTTP/1.0\r\n", method, directory);
    sprintf(buffer, "%sHost: %s:%d\r\n", buffer, header->host, header->port);
    sprintf(buffer, "%s%s", buffer, header->user_agent);
    sprintf(buffer, "%s%s", buffer, header->accept);
    sprintf(buffer, "%s%s", buffer, header->accept_encoding);
    sprintf(buffer, "%s%s", buffer, header->connection);
    sprintf(buffer, "%s%s", buffer, header->proxy_connection);

    /* Write additional headers. Thanks to denial, we don't have to worry about
     * buffer overflow! */
    while (a < header->num_extra_headers) {
        extra_header = header->extra_headers[a];
        sprintf(buffer, "%s%s", buffer, extra_header);
        a++;
    }

    /* Calculate the content length. */
    content_length = strlen(buffer);
    content_length += strlen("Content-Length: XXXXXXXXXXXX\r\n");
    //sprintf(buffer, "%sContent-Length: %12d\r\n", buffer, content_length);

    /* The final carriage return. */
    sprintf(buffer, "%s\r\n", buffer);
    printf("%s", buffer);

    conn_file = Open_clientfd_r(header->host, port);

    /* Write the buffer to the request file. */
    Rio_writen(conn_file, buffer, strlen(buffer));

    /* We're done here. */
    return conn_file;
}



/**
 * reveive_response : (int, int, char*, cache_t) -> void
 *
 * Reads a server's response from a file "server_file" and outputs it to the
 * client connection "client_file".
 *
 * Parameters:
 *      client (int):
 *
 *      server (int):
 *
 *      url (char*):
 *
 *      swag_cache (cache_t): 
 *
 * Returns:
 *      The movies that Bruce Greenwood has been in that also include Scott
 *      Bakula -- in other words, nothing.
 */
void receive_response(int client, int server, cache_t swag_cache, char *url) {
    char buffer[MAXLINE];
    size_t response_size;

    int cached_size = 0;
    int to_cache_size = 1024;
    char *to_cache;

    cache_object object = cache_search(swag_cache, url);

    while ((response_size = Rio_readn(server, buffer, MAXLINE)) > 0) {
        Rio_writen(client, buffer, response_size);

        if (!strcmp(buffer, "\r\n")) {
            break;
        }
    }

    if (object == NULL) {
        to_cache = (char*)(malloc(sizeof(char) * to_cache_size));
    }

    else {
        printf("It's cached!\n");
        Rio_writen(client, object->ptr_to_item, object->size);
        return;
    }

    while ((response_size = Rio_readn(server, buffer, MAXLINE)) > 0) {
        if (object == NULL) {
            cached_size += strlen(buffer);

            if (cached_size > to_cache_size) {
                to_cache_size *= 2;
                to_cache = (char*)(realloc(to_cache, to_cache_size));
            }

            sprintf(to_cache, "%s%s", to_cache, buffer);
            Rio_writen(client, buffer, response_size);
        }
    }

    /* Insert the file's data into the #swag cache. */
    cache_insert(swag_cache, url, to_cache, to_cache_size);
}
