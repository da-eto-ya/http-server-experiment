//
// Created by ilya on 03.03.16.
//

#include <iostream>
#include <thread>
#include <unistd.h>
#include <cstring>
#include "worker.h"

void worker::operator()() {
    auto tid = std::this_thread::get_id();

    while (!queue.done) {
        int connection = 0;
        {
            std::unique_lock<std::mutex> lock(queue.m);
            while (!queue.notified) {
                queue.cond.wait(lock);
            }

            if (queue.done) {
                break;
            }

            connection = queue.storage.front();
            queue.storage.pop_front();
        }

        // TODO: real work
        std::cout << tid << " : " << connection << std::endl;
        const char *buf = "Hello, World!\n";
        write(connection, buf, strlen(buf));
        close(connection);
        queue.notified = false;
    }

    std::cout << tid << " exit" << std::endl;
}
