//
// Created by ilya on 03.03.16.
//

#include <cstring>
#include "utils.h"

void exit_error(const char *message, int code) {
    std::cerr << message << std::endl;
    std::cerr << strerror(errno) << std::endl;
    std::cerr.flush();
    exit(code);
}

ssize_t do_sendfile(int out_fd, int in_fd, off_t offset, size_t count) {
    ssize_t bytes_sent;
    size_t total_bytes_sent = 0;
    while (total_bytes_sent < count) {
        if ((bytes_sent = sendfile(out_fd, in_fd, &offset, count - total_bytes_sent)) <= 0) {
            if (errno == EINTR || errno == EAGAIN) {
                // Interrupted system call/try again
                // Just skip to the top of the loop and try again
                continue;
            }

            return -1;
        }

        total_bytes_sent += bytes_sent;
    }

    return total_bytes_sent;
}
