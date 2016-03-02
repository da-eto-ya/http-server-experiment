//
// Simple server.
//

#ifndef FINAL_SERVER_H
#define FINAL_SERVER_H

#include <thread>
#include "server_config.h"
#include "worker.h"

#define WORKERS_COUNT 4

class server {
private:
    server_config config;
    worker *workers[WORKERS_COUNT];
    std::thread threads[WORKERS_COUNT];

    void daemonize();

    int open_socket();

    int bind_socket(int sock);

    void create_workers(work_queue<int> &queue);

    void main_loop(int sock, work_queue<int> &queue);

public:
    server(server_config config) : config(config) { }

    void run();
};

#endif //FINAL_SERVER_H
