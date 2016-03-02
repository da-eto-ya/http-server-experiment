//
// Main.
//

#include "server_config.h"
#include "server.h"
#include "utils.h"

int main(int argc, char **argv) {
    server_config config(argc, argv);

    if (!config.is_filled()) {
        exit_error("Incomplete config", -1);
    }

    server s(config);
    s.run();

    return 0;
}
