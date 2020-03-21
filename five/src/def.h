#pragma once

enum {
    board_size = 15,
    board_pts  = board_size * board_size,
    win_count  = 5
};

using piece_t = int;

constexpr piece_t const Empty = 0;
constexpr piece_t const White = 1;
constexpr piece_t const Black = 2;

enum class direction : unsigned {
    up    = 0x01,
    down  = 0x02,
    left  = 0x04,
    right = 0x08
};

unsigned operator|(direction x, direction y) noexcept {
    return unsigned(x) | unsigned(y);
}

unsigned operator&(direction x, unsigned  y) noexcept { return unsigned(x) &          y ; }
unsigned operator&(unsigned  x, direction y) noexcept { return          x  & unsigned(y); }
unsigned operator&(direction x, direction y) noexcept { return unsigned(x) & unsigned(y); }
