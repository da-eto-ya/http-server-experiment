//
// Server configuration utils.
//

#include "server_config.h"
#include <cstdlib>
#include <unistd.h>

extern char *optarg;

server_config server_config_read_opts(int argc, char *const *argv) {
    server_config config = {};
    int flag;

    while ((flag = getopt(argc, argv, "h:p:d:t")) != -1) {
        switch (flag) {
            case 'h':
                config.ip = optarg;
                break;

            case 'p':
                config.port = atoi(optarg);
                break;

            case 'd':
                config.dir = optarg;
                break;

            case 't':
                config.test = true;
                break;

            default:
                break;
        }
    }

    return config;
}
