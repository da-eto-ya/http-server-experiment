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

//    if ((sock = open_socket()) < 0) {
//        exit_error("Can't open socket", -3);
//    }
//
//    if (bind_socket(sock) < 0) {
//        exit_error("Can't bind socket", -4);
//    }
//
//    if (listen(sock, SOMAXCONN) < 0) {
//        exit_error("Can't listen", -5);
//    }

    work_queue<int> queue;
    create_workers(queue);

    for (unsigned int i = 0; i < WORKERS_COUNT; ++i) {
        threads[i] = std::thread(*workers[i]);
    }

    std::thread main_thread([&] { main_loop(sock, queue);});
    main_thread.join();

    for (unsigned int i = 0; i < WORKERS_COUNT; ++i) {
        threads[i].join();
    }
}

void server::create_workers(work_queue<int> &queue) {
    for (unsigned int i = 0; i < WORKERS_COUNT; ++i) {
        workers[i] = new worker(queue);
    }
}

int server::open_socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
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

//    while (true) {
//        if ((connection = accept(sock, (sockaddr *) &client, &address_size)) != -1) {
//            std::unique_lock<std::mutex> lock(queue.m);
//
//        } else {
//            // print err
//        }
//    }
    queue.done = false;

    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::unique_lock<std::mutex> lock(queue.m);
        std::cout << "producing " << i << '\n';
        queue.storage.push_back(i);
        queue.notified = true;
        queue.cond.notify_one();
    }

    queue.done = true;
    queue.cond.notify_all();
}
