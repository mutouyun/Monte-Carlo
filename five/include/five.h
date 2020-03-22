#pragma once

#include <cstddef>

#include "five_global.h"

struct FIVESHARED_EXPORT st_info {
    int x_, y_;
    double rate_;
    double score_;
    int visits_;
};

using call_t = void(*)(void* p, std::size_t t, st_info* l, std::size_t s);

FIVESHARED_EXPORT void five_prepare(call_t call, std::size_t limit = -1, std::size_t timeout = -1, void* p = nullptr);
FIVESHARED_EXPORT void five_start_game();
FIVESHARED_EXPORT std::size_t five_calc_next(unsigned* px, unsigned* py);
