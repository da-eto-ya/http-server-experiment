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
