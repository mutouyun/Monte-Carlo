#pragma once

#include <queue>
#include <unordered_map>
#include <tuple>
#include <cmath>
#include <cstring>
#include <atomic>
#include <cstdint>

#include "def.h"
#include "coord.h"
#include "pool.h"
#include "steps.h"

struct st_node;

class nodes : public std::unordered_map<coord, st_node*> {

    using base_t = std::unordered_map<coord, st_node*>;

public:
    using base_t::unordered_map;

    bool set(coord const & c, st_node* val) noexcept {
        (*this)[c] = val;
        return true;
    }

    bool contains(coord const & c) const noexcept {
        return find(c) != end();
    }
};

struct st_node {

    std::atomic<std::uint64_t> visits_ { 0 };
    st_node* parent_ = nullptr;
    nodes    next_;

    enum : std::uint64_t {
        win_flag    = 0x100000000ull,
        visits_mask = 0x0ffffffffull
    };

    void set(bool win) noexcept {
        std::uint64_t add = 1;
        if (win) add += win_flag;
        visits_.fetch_add(add, std::memory_order_relaxed);
        // backpropagation
        if (parent_ != nullptr) parent_->set(!win);
    }

    std::size_t win() const noexcept {
        return (visits_.load(std::memory_order_relaxed) & ~visits_mask) >> 32;
    }

    std::size_t visits() const noexcept {
        return visits_.load(std::memory_order_relaxed) & visits_mask;
    }

    double rate() const noexcept {
        std::uint64_t v = visits_.load(std::memory_order_relaxed),
                      w = (v & ~visits_mask) >> 32;
        return w ? (double(w) / double(v & visits_mask)) : 0.0;
    }

    void clear() {
        visits_.store(0, std::memory_order_relaxed);
        parent_ = nullptr;
        next_ = {};
    }

    std::tuple<coord, std::size_t> get_best() noexcept {
        std::size_t rt = 0;
        coord rc;
        for (auto it : next_) {
            std::size_t t = it.second->visits();
            if (rt < t) {
                rt = t;
                rc = it.first;
            }
        }
        return std::make_tuple(rc, rt);
    }
};

namespace {

double score(st_node const * p, st_node const * x) noexcept { // UCB
    double log_Np = std::log(double(p->visits()));
    double     Nx =          double(x->visits());
    return x->rate() + std::sqrt(2 * log_Np / Nx);
}

template <typename C>
double highest_score(steps<piece_t, Invalid> const & steps, 
                     st_node const * p, coord& rc, C&& cmp) noexcept {
    double rs = 0.0;
    for (auto it : p->next_) {
        if (steps[it.first] != Empty) continue;
        double s = score(p, it.second);
        if (cmp(rs, s)) {
            rs = s;
            rc = it.first;
        }
    }
    return rs;
}

template <typename Q, typename C>
double highest_scores(steps<piece_t, Invalid> const & steps,
                      st_node const * p, Q& que, C&& cmp) noexcept {
    double rs = 0.0;
    for (auto it : p->next_) {
        if (steps[it.first] != Empty) continue;
        double s = score(p, it.second);
        if (cmp(rs, s)) {
            rs = s;
            que.clear();
            que.push_back(it.first);
        }
        else if (rs == s) {
            que.push_back(it.first);
        }
    }
    return rs;
}

st_node * build_next(pool<st_node>& alloc, st_node * p, coord const & c) {
    st_node *& x = p->next_[c];
    if (x == nullptr) {
        x = alloc.get();
        x->parent_ = p;
    }
    return x;
}

} // namespace

class layer {
    pool<st_node>& alloc_;
    st_node * data_ = nullptr;

    void clear(st_node * x) {
        std::queue<st_node*> que;
        que.push(x);
        do {
            st_node* p = que.front();
            que.pop();
            if (p == nullptr) continue;
            for (unsigned i = 0; i < p->next_.size(); ++i) {
                que.push(p->next_[i]);
            }
            alloc_.recycle(p);
        } while (!que.empty());
    }

public:
    explicit layer(pool<st_node>& a)
        : alloc_(a) {
        data_ = alloc_.get();
    }

    ~layer() { clear(data_); }

    pool<st_node>       & allocator()       noexcept { return alloc_; }
    pool<st_node> const & allocator() const noexcept { return alloc_; }

    void clear() {
        clear(data_);
        data_ = alloc_.get();
    }

    st_node       * current()       noexcept { return data_; }
    st_node const * current() const noexcept { return data_; }

    void put_next(coord const & c) {
        st_node* x;
        auto it = data_->next_.find(c);
        if (it == data_->next_.end()) {
            x = alloc_.get();
        }
        else {
            x = it->second;
            x->parent_ = nullptr;
            data_->next_.erase(it);
        }
        clear(data_);
        data_ = x;
    }

    auto get_next() {
        auto tp = data_->get_best();
        put_next(std::get<0>(tp));
        return tp;
    }
};
