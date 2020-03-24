#include <queue>
#include <stack>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <random>
#include <type_traits>
#include <future>

#include "five.h"
#include "def.h"
#include "layer.h"
#include "board.h"
#include "pool.h"
#include "async.h"

namespace {

bool volatile run__ = false;
std::atomic<std::size_t> total__;
std::thread job__;
board board__;
layer all__ { *(new pool<st_node>) };
std::mutex lc_all__;

std::size_t limit__ = std::size_t(-1), timeout__ = 30;
call_t call__ = nullptr;
void * p__ = nullptr;

auto& random_engine() {
    thread_local std::default_random_engine gen(std::random_device{}());
    return gen;
}

std::size_t random(std::size_t b, std::size_t e) {
    // [b, e)
    return std::uniform_int_distribution<std::size_t> { b, e - 1 } (random_engine());
}

bool simulation(board& b, coord na) {
    bool win;
    unsigned k = 0;

    for (std::deque<coord> list;
        !(win = b.set_and_check(na)) && !b.full();
        ++k, list.clear()) {

        coord u = b.get_urgency();
        if (u.valid()) {
            na = u;
        }
        else {
            b.next_steps([](coord const &) { return true; }, list);
            na = list[random(0, list.size())];
        }
    }

    total__.fetch_add(1, std::memory_order_relaxed);
    return win ? !(k & 1) : !!(k & 1);
}

std::vector<int> expansion(st_node* curr, board&& b, std::deque<coord> const & list) {
    // build all nexts
    for (coord c : list) {
        build_next(all__.allocator(), curr, c);
    }
    // simulate all coords in list
    std::vector<std::promise<bool>> wins { list.size() };
    for (size_t i = 0; i < list.size(); ++i) {
        async::works().push([b, i, curr, &list, &wins]() mutable {
            bool win = simulation(b, list[i]);
            curr->next_[list[i]]->set(win);
            wins[i].set_value(win);
        });
    }
    // waiting for results
    std::vector<int> results;
    for (auto & w : wins) {
        results.push_back(w.get_future().get() ? 0 : 1);
    }
    return results;
}

void selection(st_node* curr, board&& b) {

    std::stack<st_node*> policy_stack;
    policy_stack.push(curr);

    std::vector<int> wins;
    auto set_and_check = [&](coord na) mutable {
        auto nx = curr->next_[na];
        bool win;
        if ((win = b.set_and_check(na)) || b.full()) {
            if (curr == all__.current()) lc_all__.lock();
            nx->set(win);
            if (curr == all__.current()) lc_all__.unlock();
            wins.push_back(win ? 0 : 1);
            return true;
        }
        policy_stack.push(curr = nx);
        return false;
    };

    for (std::deque<coord> list;; list.clear()) {
        coord u = b.get_urgency();
        if (u.valid()) {
            if (!curr->next_.contains(u)) {
                list.push_back(u);
            }
            else if (set_and_check(u)) {
                break;
            }
            else continue;
        }
        else if (curr->next_.empty()) {
            b.next_steps([](coord const &) { return true; }, list);
        }
        else b.next_steps([curr](coord const & c) {
            return !curr->next_.contains(c);
        }, list);

        if (list.empty()) {
            highest_scores(b.get_steps(), curr, list, [](double a, double b) { return a < b; });
            if (set_and_check(list[random(0, list.size())])) {
                break;
            }
            else continue;
        }
        else {
            wins = expansion(curr, board(b), list);
            break;
        }
    }

    // backpropagation
    do {
        if (policy_stack.size() <= 2) lc_all__.lock();
        for (int flag : wins) {
            policy_stack.top()->set(flag != 0);
        }
        if (policy_stack.size() <= 2) lc_all__.unlock();
        policy_stack.pop();
        for (int& flag : wins) flag = (flag == 0) ? 1 : 0;
    } while (!policy_stack.empty());
}

bool do_calc() {
    run__ = true;
    total__.store(0, std::memory_order_relaxed);
    job__   = std::thread([] {
        while (run__) selection(all__.current(), board(board__));
    });
    for (unsigned cnt = 0; run__; ++cnt) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (!job__.joinable()) break;
        std::size_t t = total__.load(std::memory_order_relaxed);
        if (call__  != nullptr) {
            lc_all__.lock();
            auto curr = all__.current();
            if (curr->next_.empty()) {
                lc_all__.unlock();
                call__(p__, t, nullptr, 0);
            }
            else {
                st_info* list = new st_info[curr->next_.size()];
                std::size_t i = 0;
                for (auto const & it : curr->next_) {
                    list[i].x_      = int(it.first.x());
                    list[i].y_      = int(it.first.y());
                    list[i].rate_   = it.second->rate();
                    list[i].score_  = score(curr, it.second);
                    list[i].win_    = int(it.second->win_);
                    list[i].visits_ = int(it.second->visits());
                    ++i;
                }
                lc_all__.unlock();
                call__(p__, t, list, curr->next_.size());
                delete [] list;
            }
        }
        if ((cnt >= timeout__) || (t >= limit__)) break;
    }
    if (run__) {
        run__ = false;
        if (job__.joinable()) job__.join();
        return true;
    }
    return false;
}

} // namespace

void five_prepare(call_t call, std::size_t limit, std::size_t timeout, void* p) {
    limit__ = limit;
    if (timeout < std::size_t(-1)) {
        timeout__ = timeout;
    }
    call__ = call;
    p__    = p;
}

void five_start_game() {
    run__ = false;
    if (job__.joinable()) job__.join();
    all__.clear();
    board__ = {};
}

std::size_t five_calc_next(unsigned* px, unsigned* py) {
    if ((px == nullptr) || (py == nullptr)) return 0.0;
    coord c { *px, *py };
    if (c.valid()) {
        all__.put_next(c);
        board__.set(c);
    }
    if (do_calc()) {
        auto tp = all__.get_next();
        *px = std::get<0>(tp).x();
        *py = std::get<0>(tp).y();
        board__.set(std::get<0>(tp));
        return std::get<1>(tp);
    }
    else return 0;
}
