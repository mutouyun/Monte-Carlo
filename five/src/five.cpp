#include <queue>
#include <stack>
#include <thread>
#include <atomic>
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

std::size_t limit__ = -1, timeout__ = 30;
void (* call__)(std::size_t) = nullptr;

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
    auto set_and_finished = [&b, &win](coord const & na) {
        return (win = b.set_and_check(na)) || b.full();
    };
    unsigned k = 0;
    if (!set_and_finished(na)) {
        std::deque<coord> steps;
        b.holes(steps);
        shuffle(steps);
        do {
            ++ k;
            na = steps.front();
            steps.pop_front();
        } while (!set_and_finished(na));
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
    for (std::deque<coord> steps;; steps.clear(), ai_round = !ai_round) {
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
                win = !win;
                break;
            }
            else policy_stack.push(curr = nx);
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
        while (run__) selection(all__.current(), board(board__));
    });
    for (unsigned cnt = 0; run__; ++cnt) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (call__  != nullptr) call__(total__);
        if ((cnt >= timeout__) || (total__ >= limit__)) break;
    }
    if (run__) {
        run__ = false;
        if (job__.joinable()) job__.join();
        return true;
    }
    return false;
}

} // namespace

void five_prepare(void(* call)(std::size_t), std::size_t limit, std::size_t timeout) {
    limit__ = limit;
    if (timeout < std::size_t(-1)) {
        timeout__ = timeout;
    }
    call__ = call;
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
