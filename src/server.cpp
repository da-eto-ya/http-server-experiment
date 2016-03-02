//
// Simple server.
//

#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include "server.h"

void server::daemonize() {
    int pid = fork();

    if (pid < 0) {
        fprintf(stderr, "\n");
        exit(-2);
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

void server::setup() {
    if (!config.test) {
        daemonize();
    }
}
