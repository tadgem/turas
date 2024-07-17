#ifndef TURAS_STL_THREAD_H
#define TURAS_STL_THREAD_H

#include <future>

namespace turas {
    template<typename T>
    using Future = std::future<T>;
    using FutureStatus = std::future_status;

    using Mutex = std::mutex;
    using MutexLock = std::lock_guard<Mutex>;

    using Thread = std::thread;

    namespace ThisThread = std::this_thread;
}
#endif