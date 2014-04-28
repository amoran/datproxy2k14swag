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


                "If I have seen further than others, it
                 is because I have stood on the shoulders
                 of giants."
                                - John Mann

http.c

This file implements the functions defined in "http.h".
*/

/* Import our AWESOME header. */
#include "http.h"



/**
 * parse_url : (char* char**, int*, char**) -> void
 *
 * For example, the following URL:
 *
 *      http://en.wikipedia.org:1956/wiki/Bruce_Greenwood
 *
 * Will set the values to:
 *
 *      host = "en.wikipedia.org"
 *      port = 1956
 *      directory = "/wiki/Bruce_Greenwood"
 *
 * Parameters:
 *      url (char*): The URL string to parse.
 *
 *      host (char**): Pointer to string where we will store the host portion
 *      of the URL.
 *
 *      port (int*): Pointer to the int where we will store th port of the
 *      request. If no port is found in the URL, this function will
 *      default to 80.
 *
 *      directory (char**): Pointer to the string where we will store the
 *      directory portion of the URL.
 *
 * Returns:
 *      All Academy Awards that Bruce Greenwood has won -- in other
 *      words, returns nothing. (This value is distinct from the Academy
 *      Awards that Bruce Greenwood deserves.)
 */
void parse_url(char *url, char **host, int *port, char **directory) {
    char protohost[50];
    char port_str[MAX_PORT_DIGITS];

    printf("%s\n", url);

    sscanf(url, "http://%[^/]%s", protohost, *directory);

    char* p_position = strchr(protohost, ':');
    if (p_position != NULL) {
        sscanf(protohost, "%[^:]:%s", *host, port_str);
        *port = atoi(port_str);
    }
    else {
        strcpy(*host, protohost);
        *port = 80;
    }

    /* We're done here. */
    printf("from parse_url %s %d %s\n\n", *host, *port, *directory);
}



/**
 * parse_request_prologue : (char*, char**, char**, int*, char**) -> void
 *
 * We refer to the first line of an HTTP request as the "prologue" of the
 * request. An example prologue:
 *
 *     GET http://en.wikipedia.org/wiki/Bruce_Greenwood HTTP/1.0\r\n
 *
 * This function scans said prologue and places important information in the
 * given pointers.
 *
 * Parameters:
 *      buffer (char*): The string we want to parse.
 *
 *      method (char**): A pointer to the string where we will store the
 *      request method.
 *
 *      host (char**): A pointer to the string where we will store the host
 *      destination.
 *
 *      port (int*): A pointer to the integer where we will store the port.
 *      If there is no port provided, then we default to port 80.
 *
 *      directory (char**): A pointer to the string where we will store the
 *      directory of the request.
 *
 * Returns:
 *      The Star Wars movies that Bruce Greenwood has been in -- in other
 *      words, nothing.
 */
void parse_request_prologue(char *buffer, char **method, char **host,
                            int *port, char **directory) {
    /* Declare variables. */
    int len = 0;
    char *url;

    char version[50];

    while (buffer[len] != '\r') {
        len++;
    }

    /* Mallocate the new strings. */
    len += 1; /* compensate for null character */
    *method = (char*)(malloc(sizeof(char) * len));
    url = (char*)(malloc(sizeof(char) * len));
    sscanf(buffer, "%s %s %s", *method, url, version);

    len = strlen(url);
    len += 1; /* compensate for null character */
    *host = (char*)(malloc(sizeof(char) * len));
    *directory = (char*)(malloc(sizeof(char) * len));
    parse_url(url, host, port, directory);

    /* We're done here. */
    free(url);
}



/**
 * parse_request_header : (rio_t, int) -> html_header_data
 * 
 * Reads a header from a file.
 *
 * Parameters:
 *      robust_io (rio_t):
 *
 *      file_id (int):
 *
 * Returns:
 *      An html_header_data struct filled with information from the file
 *      given to us. Ownership of this object (and its fields) is passed to
 *      the caller of this function.
 */
html_header_data parse_request_header(rio_t *robust_io, int file_id) {
    char buffer[MAXLINE];

    size_t sz = sizeof(struct html_header_data);

    char *host;
    char *method;
    char *directory;
    int port;

    char *compare;

    html_header_data header = (html_header_data)(malloc(sz));

    Rio_readinitb(robust_io, file_id);
    Rio_readlineb(robust_io, buffer, MAXLINE);
    parse_request_prologue(buffer, &method, &host, &port, &directory);

    /* Assign the information from parse_request_prologue to the header
     * fields. */
    header->method = method;
    header->host = host;
    header->port = port;
    header->directory = directory;

    header->extra_headers = (char**)(malloc(sizeof(char**) * 256));
    header->num_extra_headers = 0;

    Rio_readlineb(robust_io, buffer, MAXLINE);
    while (strcmp(buffer, "\r\n")) {
        compare = "User-Agent:";
        if (strncmp(buffer, compare, strlen(compare))) {
            Rio_readlineb(robust_io, buffer, MAXLINE);
            continue;
        }

        compare = "Connection:";
        if (strncmp(buffer, compare, strlen(compare))) {
            Rio_readlineb(robust_io, buffer, MAXLINE);
            continue;
        }

        compare = "Proxy-Connection:";
        if (strncmp(buffer, compare, strlen(compare))) {
            Rio_readlineb(robust_io, buffer, MAXLINE);
            continue;
        }

        compare = "Accept-Encoding:";
        if (strncmp(buffer, compare, strlen(compare))) {
            Rio_readlineb(robust_io, buffer, MAXLINE);
            continue;
        }

        compare = "Accept:";
        if (strncmp(buffer, compare, strlen(compare))) {
            Rio_readlineb(robust_io, buffer, MAXLINE);
            continue;
        }

        sz = strlen(buffer);
        header->extra_headers[header->num_extra_headers] = (char*)(malloc(sz));
        strcpy(
            header->extra_headers[header->num_extra_headers],
            buffer
        );
        header->num_extra_headers += 1;
        header->len_extra_headers += strlen(buffer);
    }

    /* DEBUG: Print stuff out! */
    printf("Whole Buffer: %s", buffer);
    printf("Method:       %s\n", method);
    printf("Host:         %s\n", host);
    printf("Port:         %d\n", port);
    printf("Directory:    %s\n\n", directory);
    printf("%s http://%s:%d%s \n", method, host, port, directory);

    /* We're done here. */
    return header;
}


/**
 *
 */
char* get_url(html_header_data header) {
    int len = 0;
    char *url;

    len = strlen(header->host);
    len += strlen(header->directory);

    url = (char*)(malloc(sizeof(char) * len + 1));
    url[len] = '\0';
    sprintf(url, "%s%s", header->host, header->directory);

    return url;
}

/**
 * proxify_header : html_header_data -> void
 *
 * Called when we want to place values in our request header to match the
 * handout specification.
 *
 * Parameters:
 *      header (html_header_data): The request data we want to modify.
 *
 * Returns:
 *      Bruce Greenwood's (likely) role in the third Star Trek movie --
 *      nothing. :-(
 */
void proxify_header(html_header_data header) {
    header->user_agent = user_agent_hdr;
    header->accept = accept_hdr;
    header->accept_encoding = accept_encoding_hdr;
    header->connection = connection_hdr;
    header->proxy_connection = proxy_connection_hdr;
}



void print_html_header_data(html_header_data header) {
    int a = 0;
    int len = header->num_extra_headers;

    printf(
        "%s http://%s:%d%s HTTP/1.0\n",
        header->method,
        header->host,
        header->port,
        header->directory
    );

    printf(header->user_agent);
    printf(header->accept);
    printf(header->accept_encoding);
    printf(header->connection);
    printf(header->proxy_connection);

    while (a < len) {
        printf(header->extra_headers[a]);
        a++;
    }
}



/**
 * free_html_header_data : html_header_data -> void
 *
 * TODO
 *
 * Parameters:
 *      header (html_header_data): The header which we will free. We will
 *      also free the allocated fields of this header.
 *
 * Returns:
 *      Nothing.
 */
void free_html_header_data(html_header_data header) {
    int a = 0;
    char *extra_header;

    free(header->host);
    free(header->method);
    free(header->directory);

    while (a < header->num_extra_headers) {
        extra_header = header->extra_headers[a];
        // free(extra_header);
        a += 1;
    }

    free(header);
}
