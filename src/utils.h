//
// Some utils.
//

#ifndef FINAL_UTILS_H
#define FINAL_UTILS_H

#include <cstdlib>
#include <iostream>
#include <sys/sendfile.h>
#include <sys/stat.h>

void exit_error(const char *message, int code);

ssize_t do_sendfile(int out_fd, int in_fd, off_t offset, size_t count);

void urldecode2(char *dst, const char *src);

#endif //FINAL_UTILS_H
