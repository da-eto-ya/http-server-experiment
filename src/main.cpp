#include <cstdio>
#include "server_config.h"

int main(int argc, char **argv) {
    server_config config = server_config_read_opts(argc, argv);
    printf("HOST: %s:%d\n", config.ip, config.port);
    printf("DIR: '%s'\n", config.dir);
    printf("TEST: %d\n", (int) config.test);

    return 0;
}
