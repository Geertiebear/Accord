#ifndef ACCORD_UTIL_LIB_EVENT_UTIL_H
#define ACCORD_UTIL_LIB_EVENT_UTIL_H

#include <event2/thread.h>

namespace accord {
namespace util {

class LibEventUtil {
public:
    static struct evthread_lock_callbacks lockCallbacks;
    static struct evthread_condition_callbacks conditionCallbacks;
    
    // init stuff
    static void init();
    static void logCallback(int severity, const char *msg);

    //threading stuff
	 static unsigned long idCallback();
    static void *allocLock(unsigned int lockType);
    static void freeLock(void *lock, unsigned int lockType);
    static int lock(unsigned int mode, void *lock);
    static int unlock(unsigned int mode, void *lock);
    
    static void *allocCondition(unsigned int conditionType);
    static void freeCondition(void *condition);
    static int signalCondition(void *condition, int broadcast);
    static int waitCondition(void *condition, void *lock, const struct timeval *timeout);
};

} /* namespace util */
} /* namespace util */

#endif
