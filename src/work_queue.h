//
// Queue for work jobs.
//

#ifndef FINAL_WORK_QUEUE_H
#define FINAL_WORK_QUEUE_H


#include <mutex>
#include <condition_variable>
#include <list>

template<typename T>
struct work_queue {
    std::list<T> storage;
    std::mutex m;
    std::condition_variable cond;
    bool notified;
    bool done;
};


#endif //FINAL_WORK_QUEUE_H
