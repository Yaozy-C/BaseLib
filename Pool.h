#ifndef _POLL_HPP
#define _POLL_HPP

#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <future>
#include <condition_variable>
#include <cassert>
#include <sstream>
#include "SafeQue.h"

namespace Base {

    class ThreadPool {
    public:
        typedef std::function<void()> task_t;

        explicit ThreadPool();

        ThreadPool(const ThreadPool &) = delete;

        const ThreadPool &operator=(const ThreadPool &) = delete;

        ~ThreadPool();

        void Stop();

        template<class F, class...Args>
        auto AddTask(F &&f, Args &&... args) -> std::future<decltype(f(args...))> {
            auto ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...));
            std::function<void()> func = [ptr]() {
                (*ptr)();
            };
            tasks.Enque(func);
            cv.notify_one();
            return ptr->get_future();
        }

    private:

        void Start();

        void ThreadLoop();

        bool Take(task_t &task);

        typedef std::vector<std::shared_ptr<std::thread >> threads_t;
        typedef Base::Queue<task_t> tasks_t;

        int initSize;

        threads_t threads;
        tasks_t tasks;

        std::mutex mtx;
        std::condition_variable cv;
        std::atomic<bool> isStart;
    };
}
#endif