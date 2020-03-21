#pragma once

#include <unordered_map>
#include <queue>
#include <tuple>
#include <cmath>
#include <cstring>

#include "def.h"
#include "coord.h"
#include "pool.h"

struct st_node {

    std::size_t win_    = 0;
    std::size_t visits_ = 0;
    std::unordered_map<coord, st_node*> next_;

    void set(bool win) noexcept {
        visits_ += 1;
        if (win) win_ += 1;
    }

    std::size_t visits() const noexcept {
        return visits_;
    }

    double rate() const noexcept {
        return win_ ? (double(win_) / double(visits())) : 0.0;
    }

    void clear() {
        win_ = visits_ = 0;
        next_.clear();
    }

    std::tuple<coord, std::size_t> get_best() noexcept {
        std::size_t rt = 0;
        coord rc;
        for (auto const & it : next_) {
            std::size_t r = it.second->visits();
            if (rt < r) {
                rt = r;
                rc = it.first;
            }
        }
        return std::make_tuple(rc, rt);
    }
};

namespace {

double score(st_node const * p, st_node const * x) noexcept {
    double log_Np = std::log(double(p->visits()));
    double     Nx =          double(x->visits());
    return x->rate() + std::sqrt(2 * log_Np / Nx);
}

template <typename C>
double highest_score(st_node const * p, coord& rc, C&& cmp) noexcept { // UCT
    double rs = 0.0;
    for (auto & it : p->next_) {
        double s = score(p, it.second);
        if (cmp(rs, s)) {
            rs = s;
            rc = it.first;
        }
    }
    return rs;
}

template <typename Q, typename C>
double highest_scores(st_node const * p, Q& que, C&& cmp) noexcept { // UCT
    double rs = 0.0;
    for (auto & it : p->next_) {
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
    st_node*& x = p->next_[c];
    if (x == nullptr) x = alloc.get();
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
            for (auto & it : p->next_) {
                que.push(it.second);
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
        auto it = data_->next_.find(c);
        if (it == data_->next_.end()) return;
        st_node* x = it->second;
        data_->next_.erase(it);
        clear(data_);
        data_ = x;
    }

    std::tuple<coord, std::size_t> get_next() {
        auto tp = data_->get_best();
        put_next(std::get<0>(tp));
        return tp;
    }
};
