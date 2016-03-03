//
// Simple server.
//

#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include "server.h"
#include "utils.h"
#include <arpa/inet.h>
#include <thread>
#include "work_queue.h"

void server::daemonize() {
    int pid = fork();

    if (pid < 0) {
        exit_error("Can't fork", -2);
    }

    if (pid > 0) {
        exit(0);
    }

    // only children lives here
    setsid();
    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void server::run() {
    if (!config.test) {
        daemonize();
    }

    int sock = 0;

    if ((sock = open_socket()) < 0) {
        exit_error("Can't open socket", -3);
    }

    if (bind_socket(sock) < 0) {
        exit_error("Can't bind socket", -4);
    }

    if (listen(sock, SOMAXCONN) < 0) {
        exit_error("Can't listen", -5);
    }

    work_queue<int> queue;
    create_workers(queue);

    for (unsigned int i = 0; i < WORKERS_COUNT; ++i) {
        threads[i] = std::thread(*workers[i]);
    }

    std::thread main_thread([&] { main_loop(sock, queue); });
    main_thread.join();

    for (unsigned int i = 0; i < WORKERS_COUNT; ++i) {
        threads[i].join();
    }
}

void server::create_workers(work_queue<int> &queue) {
    for (unsigned int i = 0; i < WORKERS_COUNT; ++i) {
        workers[i] = new worker(queue, config.dir);
    }
}

int server::open_socket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock != -1) {
        int opt = 1;
        setsockopt(sock, SOL_SOCKET, (SO_REUSEADDR | SO_REUSEPORT), &opt, sizeof(opt));
    }

    return sock;
}

int server::bind_socket(int sock) {
    sockaddr_in address;
    bzero((char *) &address, sizeof address);
    address.sin_family = AF_INET;
    address.sin_port = htons((uint16_t) config.port);
    inet_pton(AF_INET, config.ip, &address.sin_addr);

    return bind(sock, (sockaddr *) &address, sizeof(address));
}

void server::main_loop(int sock, work_queue<int> &queue) {
    int connection = 0;
    sockaddr_in client;
    socklen_t address_size = sizeof(sockaddr_in);

    queue.done = false;

    // TODO: implement stop
    while (true) {
        if ((connection = accept(sock, (sockaddr *) &client, &address_size)) != -1) {
            std::unique_lock<std::mutex> lock(queue.m);
            queue.storage.push_back(connection);
            queue.notified = true;
            queue.cond.notify_one();
        } else {
            // print err
        }
    }

    queue.done = true;
    queue.cond.notify_all();
}
