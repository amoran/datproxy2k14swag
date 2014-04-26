/*

                    John Mann and Adam Moran Present
                ____        _    ____           _             
               |  _ \  __ _| |_ / ___|__ _  ___| |__   ___    
               | | | |/ _` | __| |   / _` |/ __| '_ \ / _ \   
               | |_| | (_| | |_| |__| (_| | (__| | | |  __/   
               |____/ \__,_|\__|\____\__,_|\___|_| |_|\___|   
              ____  _    _ _  _     ______        ___    ____ 
             |___ \| | _| | || |   / ___\ \      / / \  / ___|
               __) | |/ / | || |_  \___ \\ \ /\ / / _ \| |  _ 
              / __/|   <| |__   _|  ___) |\ V  V / ___ \ |_| |
             |_____|_|\_\_|  |_|   |____/  \_/\_/_/   \_\____|

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
 *      "www.nasa.gov"
 */
char *host_from_url(char *url) {
    int a = 0; /* iteration */
    int b = 0; /* iteration */
    int found_dubslash = 0;
    int len = 0;
    char *to_return;

    while (url[a] != '\0') {
        /* If we see a slash... */
        if (url[a] == '/') {
            /* ...followed by another one... */
            a++;
            if (url[a] == '/') {
                /* ...we've found the two slashes after the protocol. */
                a++;
                found_dubslash = 1;
                break;
            }
        }
        a++;
    }

    /* No double slashes? Must be invalid input and must return NULL. :-( */
    if (!found_dubslash) {
        return NULL;
    }

    b = a;
    while (url[b] != '\0' && url[b] != ':' && url[b] != '/') {
        b += 1;
    }

    len = b - a;

    /* Mallocate the new string. */
    to_return = (char*)(malloc(sizeof(char) * (len + 1)));
    to_return[len] = '\0';
    strncpy(to_return, (url + a), len);

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

char *create_url(char *host, int port_num, char *page) {
    int host_len = strlen(host);
    int page_len = strlen(page);
    int port_len = 1;
    char *port

    int total_len = 0;

    char *to_return;

    /* Through magic, convert the integer "port_num" to the string "port" and
     * get the length of that string. The "port" string includes the 
     * colon. */

    /* Calculate the total length of the URL. Then mallocate this new URL
     * string. */
    total_len = 7 + host_len + port_len + page_len;
    to_return = (char*)(malloc(sizeof(char) * total_len + 1));
    to_return[total_len] = '\0';

    /* Copy the substrings into the string. */
    strncpy(to_return, http, 7);
    strncpy((to_return + 7), host, host_len);
    strncpy((to_return + 7 + host_len), port, port_len);
    strncpy((to_return + 7 + host_len + port_len), page, page_len);

    /* We're done here. */
    return to_return;
}

html_header_data parse_header(char *header_str) {
    return NULL;
}

void proxify_header(html_header_data header) {
    header->user_agent = user_agent_hdr;
    header->accept = accept_hdr;
    header->accept_encoding = accept_encoding_hdr;
}

void send_request(html_header_data header) {

}