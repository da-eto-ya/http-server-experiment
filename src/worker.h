//
// Worker class.
//

#ifndef FINAL_WORKER_H
#define FINAL_WORKER_H


#include "work_queue.h"

class worker {
private:
    work_queue<int> &queue;

public:
    worker(work_queue<int> &queue) : queue(queue) { };

    void operator()();
};


#endif //FINAL_WORKER_H
