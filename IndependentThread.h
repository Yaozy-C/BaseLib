//
// Created by Yaozy on 2020/9/21.
//

#ifndef BASE_INDEPENDENTTHREAD_H
#define BASE_INDEPENDENTTHREAD_H

#include <mutex>
#include <thread>
#include <condition_variable>
#include "SafeQue.h"

namespace Base {
    template<typename T>
    class IndependentThread {
    private:
        std::thread _thread;
        std::mutex mtx;
        std::condition_variable _cv;

        bool _shutdown = false;
    public:
        Base::Queue<T> _tasks;

        IndependentThread() {
            _thread = std::thread(&IndependentThread::Execute, this);
        };

        ~IndependentThread() = default;

        virtual void Execute() {
            while (!_shutdown) {
                std::unique_lock<std::mutex> _lock(mtx);
                T task;
                while (_tasks.empty()) {
                    if (_shutdown) {
                        return;
                    }
                    _cv.wait(_lock);
                }
                if (_tasks.Deque(task))
                    Run(task);
            }
        };

        void Shutdown() {
            while (!_tasks.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            {
                std::unique_lock<std::mutex> lock(mtx);
                _shutdown = true;
            }
            _cv.notify_all();
            if (_thread.joinable())
                _thread.join();
        };

        void AddTask(T &task) {
            _tasks.Enque(task);
            _cv.notify_all();
        };

    protected:
        virtual void Run(T &task) = 0;
    };
}


#endif //BASE_INDEPENDENTTHREAD_H
