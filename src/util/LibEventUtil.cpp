#include <util/LibEventUtil.h>

#include <mutex>
#include <condition_variable>

namespace accord {
namespace util {
    
struct evthread_lock_callbacks LibEventUtil::lockCallbacks = {
    .lock_api_version = EVTHREAD_LOCK_API_VERSION,
    .supported_locktypes = 0 | EVTHREAD_LOCKTYPE_RECURSIVE,
    .alloc = &LibEventUtil::allocLock,
    .free = &LibEventUtil::freeLock,
    .lock = &LibEventUtil::lock,
    .unlock = &LibEventUtil::unlock,
};

struct evthread_condition_callbacks LibEventUtil::conditionCallbacks = {
    .condition_api_version = EVTHREAD_CONDITION_API_VERSION,
    .alloc_condition = &LibEventUtil::allocCondition,
    .free_condition = &LibEventUtil::freeCondition,
    .signal_condition = &LibEventUtil::signalCondition,
    .wait_condition = &LibEventUtil::waitCondition,
};

void *LibEventUtil::allocLock(unsigned int lockType)
{
   return new std::recursive_mutex();
}

void LibEventUtil::freeLock(void *lock, unsigned int lockType)
{
    delete (std::recursive_mutex*) lock;
}

int LibEventUtil::lock(unsigned int mode, void *lock)
{
    std::recursive_mutex *mutex = (std::recursive_mutex*) lock;
    return mutex->try_lock();
}

int LibEventUtil::unlock(unsigned int mode, void* lock)
{
    std::recursive_mutex *mutex = (std::recursive_mutex*) lock;
    mutex->unlock();
    return 0;
}

void *LibEventUtil::allocCondition(unsigned int conditionType)
{
    return new std::condition_variable_any;
}

void LibEventUtil::freeCondition(void *condition)
{
    delete (std::condition_variable_any*) condition;
}

int LibEventUtil::signalCondition(void *condition, int broadcast)
{
    std::condition_variable_any *condition_variable = (std::condition_variable_any*) condition;
    if (broadcast)
        condition_variable->notify_all();
    else
        condition_variable->notify_one();
    return 0;
}

int LibEventUtil::waitCondition(void *condition, void *lock, const struct timeval *timeout)
{
    std::condition_variable_any *condition_variable = (std::condition_variable_any*) condition;
    std::recursive_mutex *mutex = (std::recursive_mutex*) lock;
    std::unique_lock<std::recursive_mutex> uniqueLock(*mutex);
    std::chrono::microseconds duration = 
                std::chrono::seconds(timeout->tv_sec) + 
                std::chrono::microseconds(timeout->tv_usec);
    condition_variable->wait_for(uniqueLock, duration);
    return 0;
}

} /* namespace util */
} /* namespace util */
