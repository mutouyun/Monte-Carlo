#pragma once

#include <cstddef>

#include "five_global.h"

FIVESHARED_EXPORT void five_prepare(void(*)(std::size_t), std::size_t limit = -1, std::size_t timeout = -1);
FIVESHARED_EXPORT void five_start_game();
FIVESHARED_EXPORT std::size_t five_calc_next(unsigned* px, unsigned* py);
