#pragma once

#include <deque>
#include <cstddef>

template <typename T>
class pool {
    std::deque<T*> ptrs_;

    T* pop() {
        T* p = ptrs_.back();
        ptrs_.pop_back();
        return p;
    }

public:
    ~pool() { clear(); }

    void clear() {
        while (!empty()) {
            delete pop();
        }
    }

    bool empty() const noexcept {
        return ptrs_.empty();
    }

    T* get() {
        if (empty()) return new T;
        T* p = pop();
        p->clear();
        return p;
    }

    void recycle(T* p) {
        ptrs_.push_back(p);
    }

    void take(pool& other) {
        while (!other.empty()) {
            ptrs_.push_back(other.ptrs_.front());
            other.ptrs_.pop_front();
        }
    }

    void counterbalance(pool* others, std::size_t size) {
        if ((others == nullptr) || (size == 0)) return;
        std::size_t total = ptrs_.size();
        for (std::size_t i = 0; i < size; ++i) {
            total += others[i].ptrs_.size();
        }
        std::size_t balanced = (total + size - 1) / size;
        while (!empty()) {
            for (std::size_t i = 0; i < size; ++i) {
                if (others[i].ptrs_.size() < balanced) {
                    others[i].ptrs_.push_back(ptrs_.front());
                    ptrs_.pop_front();
                }
            }
        }
    }
};
