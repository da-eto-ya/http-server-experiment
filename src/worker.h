//
// Worker class.
//

#ifndef FINAL_WORKER_H
#define FINAL_WORKER_H


#include "work_queue.h"
#include "http_parser.h"

class worker {
private:
    work_queue<int> &queue;
    char *dir;

public:
    worker(work_queue<int> &queue, char *dir) : queue(queue), dir(dir) { };

    void operator()();
};


#endif //FINAL_WORKER_H
