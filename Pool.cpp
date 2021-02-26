
#include "Pool.h"
#include "Log.h"

namespace Base {
    static std::string GetTid() {
        std::stringstream tmp;
        tmp << std::this_thread::get_id();
        return tmp.str();
    }

    ThreadPool::ThreadPool() : mtx(), cv(), isStart(false), initSize(1) {
        int cpu = (int) std::thread::hardware_concurrency();
        if (cpu >= 4)
            initSize = cpu - 2;
        Start();
    }

    ThreadPool::~ThreadPool() {
        if (isStart) {
            Stop();
        }
    }

    void ThreadPool::Start() {
        assert(threads.empty());
        isStart = true;
        threads.reserve(initSize);
        std::unique_lock<std::mutex> lock(mtx);
        for (int i = 0; i < initSize; ++i) {
            std::shared_ptr<std::thread> thread_(new std::thread([this] { ThreadLoop(); }));
            threads.emplace_back(thread_);
        }
    }

    void ThreadPool::Stop() {
        while (!tasks.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        isStart = false;
        cv.notify_all();

        for (auto &thread : threads) {
            if (thread->joinable())
                thread->join();
        }
        threads.clear();
    }

    void ThreadPool::ThreadLoop() {
        cpu_set_t mask;
        CPU_ZERO(&mask);

        if (initSize >= 0) {
            std::unique_lock<std::mutex> lock(mtx);
            LOG_DEBUG(GetTid() + "    " + std::to_string(initSize));
            CPU_SET(initSize--, &mask);
        }

        pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask);
        while (isStart) {
            task_t task;
            if (Take(task)) task();
        }
    }

    bool ThreadPool::Take(task_t &task) {
        std::unique_lock<std::mutex> lock(mtx);
        while (tasks.empty() && isStart) {
            cv.wait(lock);
        }

        if (tasks.Deque(task) && isStart) {
            return true;
        }
        return false;
    }
}