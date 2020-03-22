#pragma once

#include <array>
#include <deque>
#include <algorithm>
#include <cstddef>

#include "def.h"
#include "coord.h"

class board {
    piece_t next_pie_ = Black;
    std::size_t set_count_ = 0;
    std::array<piece_t, board_pts> datas_ {};
    coord urgency_;

    // "xxxx_", "xxx_x", "xx_xx", "x_xxx", "_xxxx"
    coord urgent(coord const & c, unsigned d) const {
        piece_t const pie = get(c);

        auto const get_match = [this, pie, d](coord&& x, unsigned cnt, int step = 1) {
            unsigned k = 0;
            for (; k < cnt; ++k) {
                x = x.next(d, step);
                if (get(x) != pie) break;
            }
            return k;
        };

        coord x = c;
        unsigned l = get_match(std::move(x), 4);
        if (get(x) == Empty) switch (l) {
        case 0: // ???c_???
            switch (get_match(coord(x), 3)) {
            case 3:  // c_xxx
                return x;
            case 2:  // xc_xx
                return (get_match(coord(c), 1, -1) == 1) ? x : coord{};
            case 1:  // xxc_x
                return (get_match(coord(c), 2, -1) == 2) ? x : coord{};
            default: // xxxc_
                return (get_match(coord(c), 3, -1) == 3) ? x : coord{};
            }
        case 1: // ??cx_??
            switch (get_match(coord(x), 2)) {
            case 2:  // cx_xx
                return x;
            case 1:  // xcx_x
                return (get_match(coord(c), 1, -1) == 1) ? x : coord{};
            default: // xxcx_
                return (get_match(coord(c), 2, -1) == 2) ? x : coord{};
            }
        case 2: // ?cxx_?
            return (get_match(coord(x), 1)     == 1) ? x /* cxx_x */ :
                   (get_match(coord(c), 1, -1) == 1) ? x /* xcxx_ */ : coord{};
        case 3: // cxxx_
            return x;
        default:
            return {};
        }
        else return {};
    }

    unsigned count(coord const & c, unsigned d) const {
        piece_t const pie = get(c);
        unsigned cnt = 0;
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

    coord get_urgency() const noexcept {
        return urgency_;
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

        static unsigned const list[] = {
            unsigned(direction::up), unsigned(direction::dn),
            unsigned(direction::le), unsigned(direction::ri),
            direction::up | direction::le, direction::dn | direction::ri,
            direction::up | direction::ri, direction::dn | direction::le
        };

        for (auto d : list) {
            coord x = urgent(c, d);
            if (x.valid()) { urgency_ = x; return; }
        }
        urgency_ = {};
    }

    piece_t get(coord const & c) const {
        if (!c.valid()) return Invalid;
        return datas_[c];
    }

    bool set_and_check(coord const & c) {
        if (!c.valid()) return false;
        set(c);

        static unsigned const list[][2] = {
            { unsigned(direction::up), unsigned(direction::dn) },
            { unsigned(direction::le), unsigned(direction::ri) },
            { direction::up | direction::le, direction::dn | direction::ri },
            { direction::up | direction::ri, direction::dn | direction::le }
        };

        for (auto const & ds : list) {
            unsigned cnt = count(c, ds[0]) + 1;
            if (cnt >= win_count) return true;
            cnt += count(c, ds[1]);
            if (cnt >= win_count) return true;
        }
        return false;
    }
};
