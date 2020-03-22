#pragma once

#include <tuple>

#include "def.h"

class coord {
    unsigned val_ = unsigned(board_pts);

public:
    coord() noexcept = default;
    coord(coord const &) noexcept = default;

    coord(unsigned v) noexcept
        : val_ {
            (v < board_pts) ? v : unsigned(board_pts)
        } {}

    coord(unsigned x, unsigned y) noexcept
        : val_ {
            ((x < board_size) && (y < board_size)) ? (x + y * board_size) : unsigned(board_pts)
        } {}

    coord& operator=(coord const &) noexcept = default;

    friend bool operator==(coord const & x, coord const & y) {
        return x.val_ == y.val_;
    }

    friend bool operator!=(coord const & x, coord const & y) {
        return !(x == y);
    }

    friend bool operator<(coord const & x, coord const & y) {
        return x.val_ < y.val_;
    }

    bool valid() const {
        return val_ < board_pts;
    }

    operator unsigned() const {
        return val_;
    }

    unsigned x() const {
        return val_ % board_size;
    }

    unsigned y() const {
        return val_ / board_size;
    }

    coord next(unsigned d, int step = 1) const {
        if (!valid()) return *this;
        int x = int(this->x()), y = int(this->y());
               if (d & direction::up) y -= step;
        else { if (d & direction::dn) y += step; }
               if (d & direction::le) x -= step;
        else { if (d & direction::ri) x += step; }
        return { unsigned(x), unsigned(y) };
    }
};

namespace std {

template <>
struct hash<coord> : hash<unsigned> {};

} // namespace std
