#include "request.h"
#include "cache.h"
#include "csapp.h"

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
 * reveive_response : (int, int) -> void
 *
 * Reads a server's response from a file "server_file" and outputs it to the
 * client connection "client_file".
 *
 * Parameters:
 *      client_file (int):
 *
 *      server_file (int):
 *
 * Returns:
 *      The movies that Bruce Greenwood has been in that also include Scott
 *      Bakula -- in other words, nothing.
 */
void receive_response(int client_file, int server_file) {
    char buffer[MAXLINE];
    size_t response_size;

    while ((response_size = Rio_readn(server_file, buffer, MAXLINE)) > 0) {
        Rio_writen(client_file, buffer, response_size);
    }
}