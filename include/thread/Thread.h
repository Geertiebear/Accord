#ifndef AUTH_THREAD_THREAD_H
#define AUTH_THREAD_THREAD_H

#include <netinet/in.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace accord {
namespace thread {
    
struct Work {
    struct sockaddr_storage clientAddr;
    int clientSocket;
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
