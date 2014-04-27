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

/**
 * port_from_url (char pointer) -> int
 *
 * This
 *
 * Parameters:
 *      url (char*): Pointer to a NULL-terminated string.
 *
 * Returns:
 *      The port in the given URL as an integer or -1 if there was no port
 *      found in the input.
 */
int port_from_url(char *url) {
    int a = 0; /* interation */

    /* Escape case: no port in NULL url. */
    if (url == NULL) {
        return -1;
    }

    /* Loop through the URL until we reach the end or... */
    while (url[a] != '\0') {
        /* ...find a colon, indicating a port. */
        if (url[a] == ':') {
            return atoi(url + a + 1);
        }

        a++;
    }

    /* Nothing found. We're done here. */
    return -1;
}



/**
 * host_from_url (char pointer) -> (char pointer)
 *
 * This function extracts the host from the given URL and returns it as a
 * NULL-terminated string.
 *
 * Parameters:
 *      url (char*): Pointer to a NULL-terminated string. We assume that this
 *                   value is a valid URL.
 *
 * Returns:
 *      The host URL from the given URL. For example, passing in the string
 *      "http://www.nasa.gov:1969/space_the_movie.html" would return the string
 *      "www.nasa.gov".
 *
 *      Another example: "http://www.imdb.com/name/nm0339304/" would return
 *      "www.imdb.com".
 */
char *host_from_url(char *url) {
    int a = 7; /* iteration */
    int len = 0;
    char *to_return;

    while (url[a] != '\0' && url[a] != ':' && url[a] != '/') {
        len += 1;
    }

    /* Mallocate the new string. */
    to_return = (char*)(malloc(sizeof(char) * (len + 1)));
    to_return[len] = '\0';
    strncpy(to_return, (url + 7), len);

    /* We're done here. */
    return to_return;
}



/**
 * page_from_url (char pointer) -> (char pointer)
 *
 * Returns the URL of the page.
 *
 * Parameters:
 *      url (char*): Pointer to a NULL-terminated string. We assume that this
 *                   value is a valid URL.
 *
 * Returns:
 *      The page URL from the given URL. For example, passing in the string
 *      "http://www.nasa.gov:1969/space_the_movie.html" would return the string
 *      "/space_the_movie.html".
 *
 *      Another example: "http://www.imdb.com/name/nm0339304/" would return
 *      "/name/nm0339304/".
 */
char *page_from_url(char *url) {
    int a = 0; /* iteration */
    int url_len = strlen(url);
    int page_len = 0;
    char *to_return;

    /* Skip forward seven characters (len(http://)) -- we know this is valid
     * input. */
    a = 7;

    while (url[a] != '/' && url[a] != '\0') {
        a++;
    }

    a -= 1;
    page_len = url_len - a;

    /* Mallocate the new string. */
    to_return = (char*)(malloc(sizeof(char) * (page_len + 1)));
    to_return[page_len] = '\0';
    strncpy(to_return, (url + a), page_len);

    /* We're done here. */
    return to_return;
}

html_header_data parse_request_header(rio_t *robust_io, int file_id) {
    char buffer[MAXLINE];

    size_t sz = sizeof(struct html_header_data);

    char *host = (char*)(malloc(sizeof(char) * MAXLINE));
    char *port = (char*)(malloc(sizeof(char) * MAXLINE));
    port[0] = '\0';
    char *rest = (char*)(malloc(sizeof(char) * MAXLINE));
    char *directory = (char*)(malloc(sizeof(char) * MAXLINE));
    char *method = (char*)(malloc(sizeof(char) * MAXLINE));

    html_header_data header = (html_header_data)(malloc(sz));

    Rio_readinitb(robust_io, file_id);
    Rio_readlineb(robust_io, buffer, MAXLINE);
    sscanf(buffer, "%s %s", method, rest);
    sscanf(rest, "http://%[^:]:%s", host, rest);
    sscanf(rest, "%[^/]%s", port, directory);


    if (port[0] == '\0') {
        port = "80";
    }

    printf("Port: %s \n", port);

    header->host = host;
    header->directory = directory;
    header->port = port;
    header->method = method;

    printf("%s", buffer);
    printf("%s\n", method);
    printf("%s\n", host);
    printf("%s\n", port);
    printf("%s\n", directory);
    printf("%s http://%s:%s%s \n", method, host, port, directory);

    /* We're done here. */
    return header;
}

void proxify_header(html_header_data header) {
    header->user_agent = user_agent_hdr;
    header->accept = accept_hdr;
    header->accept_encoding = accept_encoding_hdr;
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
void send_request(html_header_data header) {
    /* Setup. */
    char buffer[MAXLINE];

    /* Get the host and port we're interested in. */
    char *host = header->host;
    char *method = header->method;
    char *directory = header->directory;
    char *port = header->port;

    /* Declare extra header variables. */
    char *extra_header;
    int a = 0;

    /* Try to open a connection with the host on the port. */
    int conn_file = Open_clientfd_r(host, atoi(port));

    /* Write headers to the connection. */
    sprintf(buffer, "%s http://%s:%s%s \r\n", method, host, port, directory);
    sprintf(buffer, "%s%s\r\n", buffer, header->user_agent);
    sprintf(buffer, "%s%s\r\n", buffer, header->accept);
    sprintf(buffer, "%s%s\r\n", buffer, header->accept_encoding);
    sprintf(buffer, "%s%s\r\n", buffer, header->connection);
    sprintf(buffer, "%s%s\r\n", buffer, header->proxy_connection);

    /* Write additional headers. Thanks to denial, we don't have to worry about
     * buffer overflow! */
    while (a < header->num_extra_headers) {
        extra_header = header->extra_headers[a];
        sprintf(buffer, "%s%s\r\n", buffer, extra_header);
        a++;
    }

    /* The final carriage return. */
    sprintf(buffer, "%s\r\n", buffer);

    /* Write the buffer to the request file. */
    Rio_writen(conn_file, buffer, strlen(buffer));
}
