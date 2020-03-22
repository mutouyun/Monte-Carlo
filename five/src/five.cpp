#include <queue>
#include <stack>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <random>
#include <type_traits>

#include "five.h"
#include "def.h"
#include "layer.h"
#include "board.h"
#include "pool.h"

namespace {

bool volatile run__ = false;
std::size_t volatile total__;
std::thread job__;
board board__;
layer all__ { *(new pool<st_node>) };
std::mutex lc_all__;

std::size_t limit__ = -1, timeout__ = 30;
call_t call__ = nullptr;
void * p__ = nullptr;

auto& random_engine() {
    static std::default_random_engine gen(std::random_device{}());
    return gen;
}

std::size_t random(std::size_t b, std::size_t e) {
    // [b, e)
    return std::uniform_int_distribution<std::size_t> { b, e - 1 } (random_engine());
}

template <typename Q>
void shuffle(Q& que) {
    std::shuffle(que.begin(), que.end(), random_engine());
}

bool simulation(board& b, coord na) {
    bool win;
    unsigned k = 0;

    for (std::deque<coord> steps;
        !(win = b.set_and_check(na)) && !b.full();
        ++k, steps.clear()) {

        coord u = b.get_urgency();
        if (u.valid()) {
            na = u;
        }
        else {
            b.next_steps([](coord const &) { return true; }, steps);
            na = steps[random(0, steps.size())];
        }
    }

    ++ total__;
    return win ? !(k & 1) : !!(k & 1);
}

bool expansion(st_node* curr, board&& b, std::deque<coord> const & steps) {
    for (coord c : steps) {
        // build all nexts
        build_next(all__.allocator(), curr, c);
    }
    coord na = steps[random(0, steps.size())];
    bool win = simulation(b, na);
    curr->next_[na]->set(win);
    return win;
}

#if 1
void selection(st_node* curr, board&& b) {

    static auto const get_max = [](double a, double b) { return (a == 0.0) || (a < b); };
    static auto const get_min = [](double a, double b) { return (a == 0.0) || (b < a); };

    std::stack<st_node*> policy_stack;
    policy_stack.push(curr);

    bool ai_round = true, win;
    auto set_and_check = [&](coord na) mutable {
        auto nx = curr->next_[na];
        if ((win = b.set_and_check(na)) || b.full()) {
            nx->set(win);
            win = !win;
            return true;
        }
        policy_stack.push(curr = nx);
        return false;
    };

    for (std::deque<coord> steps;; steps.clear(), ai_round = !ai_round) {
        coord u = b.get_urgency();
        if (u.valid()) {
            if (curr->next_.find(u) == curr->next_.end()) {
                steps.push_back(u);
            }
            else if (set_and_check(u)) {
                break;
            }
            else continue;
        }
        else if (curr->next_.empty()) {
            b.next_steps([](coord const &) { return true; }, steps);
        }
        else b.next_steps([curr](coord const & c) {
            return curr->next_.find(c) == curr->next_.end();
        }, steps);

        if (steps.empty()) {
            highest_scores(curr, steps, ai_round ? get_max : get_min);
            if (set_and_check(steps[random(0, steps.size())])) {
                break;
            }
            else continue;
        }
        else {
            win = !expansion(curr, board(b), steps);
            break;
        }
    }

    // backpropagation
    do {
        policy_stack.top()->set(win);
        policy_stack.pop();
    } while (win = !win, !policy_stack.empty());
}
#else
bool selection(st_node* curr, board&& b, bool ai_round = true) {

    static auto const get_max = [](double a, double b) { return (a == 0.0) || (a < b); };
    static auto const get_min = [](double a, double b) { return (a == 0.0) || (b < a); };

    bool win;
    std::deque<coord> steps;

    if (curr->next_.empty()) {
        b.next_steps([](coord const &) { return true; }, steps);
    }
    else b.next_steps([curr](coord const & c) {
        return curr->next_.find(c) == curr->next_.end();
    }, steps);

    if (steps.empty()) {
        highest_scores(curr, steps, ai_round ? get_max : get_min);
        coord na = steps[random(0, steps.size())];
        auto  nx = curr->next_[na];
        if ((win = b.set_and_check(na)) || b.full()) {
            nx->set(win);
            curr->set(win = !win);
        }
        else curr->set((win = !selection(nx, std::move(b), !ai_round)));
    }
    else {
        win = !expansion(curr, board(b), steps);
        curr->set(win);
    }
    return win;
}
#endif

bool do_calc() {
    run__   = true;
    total__ = 0;
    job__   = std::thread([] {
        while (run__) {
            lc_all__.lock();
            selection(all__.current(), board(board__));
            lc_all__.unlock();
        }
    });
    for (unsigned cnt = 0; run__; ++cnt) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (call__  != nullptr) {
            lc_all__.lock();
            auto curr = all__.current();
            if (curr->next_.empty()) {
                lc_all__.unlock();
                call__(p__, total__, nullptr, 0);
            }
            else {
                st_info* list = new st_info[curr->next_.size()];
                std::size_t i = 0;
                for (auto const & it : curr->next_) {
                    list[i].x_      = int(it.first.x());
                    list[i].y_      = int(it.first.y());
                    list[i].rate_   = it.second->rate();
                    list[i].score_  = score(curr, it.second);
                    list[i].visits_ = int(it.second->visits());
                    ++i;
                }
                lc_all__.unlock();
                call__(p__, total__, list, curr->next_.size());
                delete [] list;
            }
        }
        if ((cnt >= timeout__ * 10) || (total__ >= limit__)) break;
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
