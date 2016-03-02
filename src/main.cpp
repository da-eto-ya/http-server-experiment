//
// Main.
//

#include <cstdio>
#include "server_config.h"
#include "server.h"

int main(int argc, char **argv) {
    server_config config(argc, argv);

    if (!config.is_filled()) {
        fprintf(stderr, "Specify correct options\n");
        return -1;
    }

    server s(config);
    s.setup();

    return 0;
}
