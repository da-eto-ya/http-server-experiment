//
// Simple server.
//

#ifndef FINAL_SERVER_H
#define FINAL_SERVER_H

#include "server_config.h"

class server {
private:
    server_config &config;

    void daemonize();

public:
    server(server_config &config) : config(config) { }

    void setup();
};

#endif //FINAL_SERVER_H
