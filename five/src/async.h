#pragma once

#include <thread>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <queue>

class async {
    std::size_t  concurrency_ = std::thread::hardware_concurrency();
    std::thread* procs_       = new std::thread[concurrency_];

    bool volatile quit_ = false;
    std::mutex    lock_;
    std::condition_variable cond_;

    std::queue<std::function<void()>> works_;

public:
    static async& works() {
        static async inst;
        return inst;
    }

    async() {
        for (std::size_t i = 0; i < concurrency_; ++i) {
            procs_[i] = std::thread([this] {
                while (!quit_) {
                    std::function<void()> work;
                    {
                        auto guard = std::unique_lock { lock_ };
                        while (works_.empty()) {
                            cond_.wait(guard);
                            if (quit_) return;
                        }
                        work = std::move(works_.front());
                        works_.pop();
                    }
                    if (work) work();
                }
            });
        }
    }

    ~async() {
        quit_ = true;
        std::lock_guard { lock_ }; // barrier
        cond_.notify_all();
        for (std::size_t i = 0; i < concurrency_; ++i) {
            if (procs_[i].joinable()) procs_[i].join();
        }
        delete [] procs_;
    }

    template <typename F>
    void push(F&& work) {
        {
            [[maybe_unused]] auto guard = std::lock_guard { lock_ };
            works_.emplace(std::forward<F>(work));
        }
        cond_.notify_all();
    }
};
