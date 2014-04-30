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

/* You won't lose style points for including these long lines in your code */
static char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static char *accept_hdr = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
static char *accept_encoding_hdr = "Accept-Encoding: gzip, deflate\r\n";
static char *connection_hdr = "Connection: close\r\n";
static char *proxy_connection_hdr = "Proxy-Connection: close\r\n";

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
    printf("      -> Parsing Prologue \n");
    /* Declare variables. */
    int len = 0;
    char *url;

    char version[50];

    while (buffer[len] != '\r' && len < MAXLINE) {
        len++;
    }

    /* Mallocate the new strings. */
    len += 1; /* compensate for null character */
    *method = (char*)(malloc(sizeof(char) * len));
    url = (char*)(malloc(sizeof(char) * len));
    if (sscanf(buffer, "%s %s %s", *method, url, version) < 2) {
        // free(method);
        printf("         -> Failed parsing method, url, and version. Setting all to NULL and returning \n");
        *method = NULL;
        *host = NULL;
        *directory = NULL;

        return;
    }

    len = strlen(url);
    len += 1; /* compensate for null character */
    *host = (char*)(malloc(sizeof(char) * len));
    *directory = (char*)(malloc(sizeof(char) * len));
    *host[0] = '\0';
    *directory[0] = '\0';
    parse_url(url, host, port, directory);
    printf("         -> Parsed URL: \n");
    printf("            -> Method: %s \n", *method);
    printf("            -> URL: %s \n", url);
    printf("               -> Host: %s\n", *host);
    printf("               -> Port: %d \n", *port);
    printf("               -> Directory: %s \n", *directory);
    printf("            -> Version: %s \n", version);

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
 *
 *      If we were given a malformed request, then NULL is returned.
 */
html_header_data parse_request_header(rio_t *robust_io, int file_id) {
  printf("   -> Parsing Request Header \n");
    char buffer[MAXLINE];

    size_t sz = sizeof(struct html_header_data);

    char *host;
    char *method;
    char *directory;
    int port;

    char *compare;

    html_header_data header;

    rio_readinitb(robust_io, file_id);
    rio_readlineb(robust_io, buffer, MAXLINE);
    printf("      -> Raw request (first line): %s \n", buffer);
    parse_request_prologue(buffer, &method, &host, &port, &directory);

    /* Check for the NULL case where everything blew up like in The Core
     * when Bruce Greenwood couldn't stop the Earth's core from breaking. :-(
     */
    if (method == NULL || directory == NULL || host == NULL) {
        printf("      -> Method, dir, and host were NULL, returning NULL as well. \n");
        return NULL;
    }

    /* Valid input! Mallocate the header. */
    header = (html_header_data)(malloc(sz));

    /* Assign the information from parse_request_prologue to the header
     * fields. */
    header->method = method;
    header->host = host;
    header->port = port;
    header->directory = directory;
    header->extra_headers = (char*)(malloc(sizeof(char) * MAXLINE * 10));
    header->len_extra_headers = 0;

    rio_readlineb(robust_io, buffer, MAXLINE);
    while (strcmp(buffer, "\r\n")) {
        compare = "Host:";
        if (strncmp(buffer, compare, strlen(compare)) == 0) {
            sscanf(buffer, "Host: %s", host);
            header->host = host;

            /* Read the next line. */
            rio_readlineb(robust_io, buffer, MAXLINE);
            continue;
        }

        compare = "User-Agent:";
        if (strncmp(buffer, compare, strlen(compare)) == 0) {
            rio_readlineb(robust_io, buffer, MAXLINE);
            continue;
        }

        compare = "Connection:";
        if (strncmp(buffer, compare, strlen(compare)) == 0) {
            rio_readlineb(robust_io, buffer, MAXLINE);
            continue;
        }

        compare = "Proxy-Connection:";
        if (strncmp(buffer, compare, strlen(compare)) == 0) {
            rio_readlineb(robust_io, buffer, MAXLINE);
            continue;
        }

        compare = "Accept-Encoding:";
        if (strncmp(buffer, compare, strlen(compare)) == 0) {
            rio_readlineb(robust_io, buffer, MAXLINE);
            continue;
        }

        compare = "Accept:";
        if (strncmp(buffer, compare, strlen(compare)) == 0) {
            rio_readlineb(robust_io, buffer, MAXLINE);
            continue;
        }

        strcat(header->extra_headers, buffer);

        /* Read the next line. */
        rio_readlineb(robust_io, buffer, MAXLINE);
    }

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
    free(header->host);
    free(header->method);
    free(header->directory);
    free(header->extra_headers);

    free(header);
}
