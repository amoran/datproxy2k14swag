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


                "If you're going through hell, keep going."
                                - Adam Moran
*/

#include <stdio.h>

#include "http.h"
#include "cache.h"
#include "csapp.h"

int send_request(html_header_data header);
void receive_response(int client, int server, cache_t swag_cache, char *url);
