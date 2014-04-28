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


                "Those who forget history are doomed to
                 repeat it."
                                - John Mann


http.h

This file defines the data structure that abstracts HTTP request headers. It
also defines prototypes for the functions that will parse URLs, request headers,
etc.
*/

/* Include libraries. */
#include <stdlib.h>
#include <string.h>

/* Include our files. */
#include "csapp.h"

/* TODO */
#define MAX_PORT_DIGITS 15

/* You won't lose style points for including these long lines in your code */
static char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static char *accept_hdr = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
static char *accept_encoding_hdr = "Accept-Encoding: gzip, deflate\r\n";
static char *connection_hdr = "Connection: close\r\n";
static char *proxy_connection_hdr = "Proxy-Connection: close\r\n";

/* Define data structures. */
struct html_header_data {
    /* Request destination. */
    int port;
    char *host;
    char *directory;

    /* Request type. */
    char *method;

    /* Required headers. */
    char *user_agent;
    char *accept;
    char *accept_encoding;
    char *connection;
    char *proxy_connection;

    /* Extra headers. */
    char **extra_headers;
    int num_extra_headers;
    int len_extra_headers; /* number of characters in extra headers */
};

typedef struct html_header_data* html_header_data;

/* Define URL-parsing functions. */
void parse_url(char *buffer, char **host, int *port, char **directory);
void parse_request_prologue(char *buffer, char **method, char **host,
                            int *port, char **directory);

/* html_header_data function prototypes. */
html_header_data parse_request_header(rio_t *file, int file_id);
void proxify_header(html_header_data header);
void print_html_header_data(html_header_data header);
void free_html_header_data(html_header_data header);
