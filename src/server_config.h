//
// Server configuration utils.
//

#ifndef FINAL_SERVER_CONFIG_H
#define FINAL_SERVER_CONFIG_H

struct server_config {
    char *ip = nullptr;
    int port = 0;
    char *dir = nullptr;
    bool test = false;

    server_config(int argc, char *const *argv);

    bool is_filled();
};

#endif //FINAL_SERVER_CONFIG_H
