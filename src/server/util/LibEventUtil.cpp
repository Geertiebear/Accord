#include <accordserver/util/LibEventUtil.h>

#include <event2/event.h>
#include <event2/bufferevent_struct.h>
#include <event2/bufferevent_ssl.h>
#include <openssl/ssl.h>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include <thread>

#include <accordserver/log/Logger.h>

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

void LibEventUtil::init()
{
	event_enable_debug_mode();
	evthread_set_lock_callbacks(&LibEventUtil::lockCallbacks);
	evthread_set_condition_callbacks(&LibEventUtil::conditionCallbacks);
	evthread_set_id_callback(&LibEventUtil::idCallback);
	event_set_log_callback(&LibEventUtil::logCallback);
}

void LibEventUtil::logCallback(int severity, const char *message)
{
   switch (severity) {
      case _EVENT_LOG_DEBUG: log::Logger::log(log::DEBUG, std::string(message)); break;
      case _EVENT_LOG_MSG: log::Logger::log(log::INFO, std::string(message)); break;
      case _EVENT_LOG_WARN: log::Logger::log(log::WARNING, std::string(message)); break;
      case _EVENT_LOG_ERR: log::Logger::log(log::ERROR, std::string(message)); break;
      default: log::Logger::log(log::ERROR, std::string(message)); break;
   }
}

unsigned long LibEventUtil::idCallback()
{
	return std::hash<std::thread::id>()(std::this_thread::get_id());
}

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

void LibEventUtil::freeBufferEvent(struct bufferevent *bufferEvent)
{
   evutil_socket_t socket = event_get_fd(&bufferEvent->ev_read);
   SSL *ssl = bufferevent_openssl_get_ssl(bufferEvent);
   SSL_free(ssl);
   close(socket);
   bufferevent_free(bufferEvent);
}

} /* namespace util */
} /* namespace accord */
