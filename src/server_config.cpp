//
// Server configuration utils.
//

#include "server_config.h"
#include <cstdlib>
#include <unistd.h>

extern char *optarg;

server_config::server_config(int argc, char *const *argv) {
    int flag;

    while ((flag = getopt(argc, argv, "h:p:d:t")) != -1) {
        switch (flag) {
            case 'h':
                ip = optarg;
                break;

            case 'p':
                port = atoi(optarg);
                break;

            case 'd':
                dir = optarg;
                break;

            case 't':
                test = true;
                break;

            default:
                break;
        }
    }
}

bool server_config::is_filled() {
    return ip != nullptr && dir != nullptr && port > 0;
}
