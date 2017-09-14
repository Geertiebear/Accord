#ifndef ACCORD_THREAD_THREAD_H
#define ACCORD_THREAD_THREAD_H

#include <event2/util.h>
#include <netinet/in.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace accord {
namespace thread {
    
struct Work {
    evutil_socket_t clientSocket;
    char netBuffer[1024];
};
    
using WorkQueue = std::vector<Work>;

class Thread {
public:
    Thread(WorkQueue *queue);
    ~Thread();
    
    void stop();
    void wake();
    void start();
    
    bool isAwake() { return awake; };
private:
    WorkQueue* queue;
    std::thread thread;
    bool running;
    bool awake;
    std::mutex awakeMutex;
    std::condition_variable awakeCondition;
    
    void work(const Work &work);
    void run();
};

} /* namespace thread */
} /* namespace accord */

#endif
