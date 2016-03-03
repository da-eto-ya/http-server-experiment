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

void urldecode2(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a')
                a -= 'a'-'A';
            if (a >= 'A')
                a -= ('A' - 10);
            else
                a -= '0';
            if (b >= 'a')
                b -= 'a'-'A';
            if (b >= 'A')
                b -= ('A' - 10);
            else
                b -= '0';
            *dst++ = 16*a+b;
            src+=3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst++ = '\0';
}
