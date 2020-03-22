#pragma once

#include <array>
#include <deque>
#include <algorithm>
#include <cstddef>

#include "def.h"
#include "coord.h"

namespace {



} // namespace

class board {
    piece_t next_pie_ = Black;
    std::size_t set_count_ = 0;
    std::array<piece_t, board_pts> datas_ {};

    unsigned count(coord const & c, unsigned d) const {
        unsigned cnt = 0;
        piece_t  pie = get(c);
        for (coord x = c.next(d); x.valid(); ++cnt, x = x.next(d)) {
            if (get(x) != pie) break;
        }
        return cnt;
    }

    unsigned count(coord const & c, direction d) const {
        return count(c, unsigned(d));
    }

public:
    bool full() const noexcept {
        return set_count_ >= datas_.size();
    }

    bool empty() const noexcept {
        return set_count_ == 0;
    }

    piece_t next_pie() const noexcept {
        return next_pie_;
    }

    template <typename Q>
    void holes(Q& que) const noexcept {
        for (unsigned i = 0; i < datas_.size(); ++i) {
            if (datas_[i] != Empty) continue;
            que.push_back(i);
        }
    }

    template <typename F, typename Q>
    void next_steps(F && check, Q& que) const noexcept {
        if (empty()) {
            que.push_back(coord(7, 7));
            return;
        }
        int min_x = board_size, max_x = 0,
            min_y = board_size, max_y = 0;
        for (unsigned i = 0; i < datas_.size(); ++i) {
            if (datas_[i] == Empty) continue;
            int x = coord(i).x(), y = coord(i).y();
            if (min_x > x) min_x = x;
            if (max_x < x) max_x = x;
            if (min_y > y) min_y = y;
            if (max_y < y) max_y = y;
        }
        min_x = std::max(min_x - 1, 0);
        max_x = std::min(max_x + 1, board_size - 1);
        min_y = std::max(min_y - 1, 0);
        max_y = std::min(max_y + 1, board_size - 1);
        for (int i = min_x; i <= max_x; ++i) {
            for (int j = min_y; j <= max_y; ++j) {
                coord c { unsigned(i), unsigned(j) };
                if ((datas_[c] != Empty) || !check(c)) continue;
                que.push_back(c);
            }
        }
    }

    void set(coord const & c) {
        if (!c.valid()) return;
        datas_[c] = next_pie_;
        next_pie_ = (next_pie_ == White) ? Black : White;
        ++ set_count_;
    }

    piece_t get(coord const & c) const {
        if (!c.valid()) return Empty;
        return datas_[c];
    }

    bool set_and_check(coord const & c) {
        if (!c.valid()) return false;
        set(c);

        unsigned cnt = count(c, direction::up) + 1;
        if (cnt >= win_count) return true;
        cnt += count(c, direction::down);
        if (cnt >= win_count) return true;

        cnt = count(c, direction::left) + 1;
        if (cnt >= win_count) return true;
        cnt += count(c, direction::right);
        if (cnt >= win_count) return true;

        cnt = count(c, direction::up | direction::left) + 1;
        if (cnt >= win_count) return true;
        cnt += count(c, direction::down | direction::right);
        if (cnt >= win_count) return true;

        cnt = count(c, direction::up | direction::right) + 1;
        if (cnt >= win_count) return true;
        cnt += count(c, direction::down | direction::left);
        if (cnt >= win_count) return true;

        return false;
    }
};
