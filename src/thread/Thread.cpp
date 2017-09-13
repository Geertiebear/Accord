#include <thread/Thread.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <system_error>

#include <log/Logger.h>

namespace accord {
namespace thread {
    
Thread::Thread(WorkQueue *queue) : queue(queue), thread(), running(true)
{
}

Thread::~Thread()
{
    running = false;
    if (thread.joinable())
        thread.join();
}

void Thread::run()
{
    while (running) {
        std::unique_lock<std::mutex> lock(awakeMutex);
        awakeCondition.wait(lock, [this]{return awake;});
        
        Work work = queue->at(queue->size() - 1);
        int ret = read(work.clientSocket, &work.netBuffer[0], 1023);
        if (ret == EAGAIN || ret == EWOULDBLOCK)
            Logger::log(ERROR, "Error reading from socket: " + std::to_string(ret));
        
        if (work.netBuffer[0] == '\0') {
            Logger::log(DEBUG, "Client disconnected, closing socket");
            close(work.clientSocket);
            
            queue->pop_back();
            if (queue->size() == 0)
                awake = false;
            continue;
        }
        //do actual work
        this->work(work);
    }
}

void Thread::stop()
{
    this->~Thread();
}

void Thread::wake()
{
    std::unique_lock<std::mutex> lock(awakeMutex);
    awake = true;
    awakeCondition.notify_one();
}

void Thread::start()
{
    thread = std::thread(&Thread::run, this);
    try {
        thread.detach();
    } catch (std::system_error &e) {
        Logger::log(ERROR, "Error detatching thread!");
    }
}

void Thread::work(const Work &work)
{
    pid_t tid = syscall(SYS_gettid);
    Logger::log(DEBUG, "Work function called from thread tid " + std::to_string(tid));
}

} /* namespace thread */
} /* namespace auth */
