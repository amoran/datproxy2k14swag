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


html.c
*/

/* Import our AWESOME header. */
#include "html.h"

void parse_url(char *url, char **host, int *port, char **directory) {
    int a = 7;

    int host_start = 7;
    int host_end = 0;

    int port_start = 0;
    int port_end = 0;

    int dir_start = 0;
    int dir_end = 0;

    int len = strlen(url);

    char *port_str;
    //char *bg_temp = (char*)(malloc(sizeof(char) * MAXLINE));

    /*
    sscanf(url, "http://%[^(:|/)]%s", *host, bg_temp);
    printf("From parse_url %s %s\n", *host, bg_temp);
    sscanf(bg_temp, "%[^/]%s", port_str, *directory);
    printf("Port str%s\n\n", port_str);
    */

    while (a < len) {
        if (url[a] == ':') {
            port_start = a + 1;
            host_end = a;
            break;
        }

        else if (url[a] == '/') {
            port_start = -1;
            port_end = -1;
            port = 80;
            host_end = a;
            dir_start = a + 1;
            break;
        }

        a++;
    }

    *host = (char*)(malloc(sizeof(char) * (host_end - host_start + 1)));
    strncpy(*host, (url + host_start), host_end - host_start);

    if (port_start > 0) {
        a = port_start;

        while (a < len) {
            if (url[a] == '/') {
                port_end = a;
                dir_start = a + 1;
                break;
            }

            else if (url[a+1] == '\0') {
                port_end = a + 1;
                dir_start = -1;
                dir_end = -1;
                break;
            }

            a++;
        }

        port_str = (char*)(malloc(sizeof(char) * (port_end - port_start + 1)));
        strncpy(port_str, (url + port_start), (port_end - port_start));
    }

    

    /* Set the port. */
    *port = atoi(port_str);

    /* We're done here. */
    //free(port_str);
    //free(bg_temp);
}



/**
 * TODO
 *
 * We refer to the first line of an HTTP request as the "prologue" of the
 * request. An example prologue:
 * 
 *     GET http://en.wikipedia.org/wiki/Bruce_Greenwood HTTP/1.0\r\n
 *
 * This function scans said prologue and places
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
 *      port (int*): 
 *
 *      directory (char**):
 */
void parse_request_prologue(char *buffer, char **method, char **host,
                            int *port, char **directory) {
    /* Declare variables. */
    int len = 0;
    char *url;

    while (buffer[len] != '\r') {
        len++;
    }

    /* Mallocate the new strings. */
    len += 1; /* compensate for null character */
    *method = (char*)(malloc(sizeof(char) * len));
    url = (char*)(malloc(sizeof(char) * len));
    sscanf(buffer, "%s %s", *method, url);

    len = strlen(url);
    len += 1; /* compensate for null character */
    *host = (char*)(malloc(sizeof(char) * len));
    *directory = (char*)(malloc(sizeof(char) * len));
    parse_url(url, host, port, directory);

    /* We're done here. */
    free(url);
}

html_header_data parse_request_header(rio_t *robust_io, int file_id) {
    char buffer[MAXLINE];

    size_t sz = sizeof(struct html_header_data);

    char *host;
    char *method;
    char *directory;
    int port;

    html_header_data header = (html_header_data)(malloc(sz));

    Rio_readinitb(robust_io, file_id);
    Rio_readlineb(robust_io, buffer, MAXLINE);
    parse_request_prologue(buffer, &method, &host, &port, &directory);

    header->method = method;
    header->host = host;
    header->port = port;
    header->directory = directory;

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
 * proxify_header (html_header_data) -> void
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
 * send_request (html_header_data) -> int
 *
 * Called when we want to fire off a request whose header matches the one
 * provided for us.
 *
 * Parameters:
 *      header (html_header_data): The data we use to create the request header.
 *
 * Returns:
 *      An integer representing the socket file, or something. Do we "listen"
 *      for a response with this thing? The fuck if I know.
 *
 * Note:
 *      http://i.imgur.com/xVyoSl.jpg
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

    /* Write headers to the connection. */
    //sprintf(buffer, "%s http://%s:%d%s HTTP/1.0\r\n", method, host, port, directory);
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
        sprintf(buffer, "%s%s\r\n", buffer, extra_header);
        a++;
    }

    /* The final carriage return. */
    sprintf(buffer, "%s\r\n", buffer);
    printf("%s", buffer);

    conn_file = Open_clientfd_r(header->host, port);

    /* Write the buffer to the request file. */
    Rio_writen(conn_file, buffer, strlen(buffer));

    /* We're done here. */
    return conn_file;
}



void receive_response(int client_file, int server_file) {
    char buffer[MAXLINE];
    size_t response_size;

    while ((response_size = Rio_readn(server_file, buffer, MAXLINE)) > 0) {
        Rio_writen(client_file, buffer, response_size); 
    }
}
