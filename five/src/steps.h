#pragma once

#include <array>

#include "def.h"
#include "coord.h"

template <typename T, T InvalidT>
class steps {
    std::array<T, board_pts> datas_ {};
    unsigned set_count_ = 0;

public:
    bool full() const noexcept {
        return set_count_ >= datas_.size();
    }

    bool empty() const noexcept {
        return set_count_ == 0;
    }

    unsigned size() const noexcept {
        return set_count_;
    }

    unsigned max_size() const noexcept {
        return datas_.size();
    }

    bool set(coord const & c, T const & val) noexcept {
        if (!c.valid() || full()) return false;
        datas_[c] = val;
        ++ set_count_;
        return true;
    }

    bool unset(coord const & c, T const & val) noexcept {
        if (!c.valid() || empty()) return false;
        datas_[c] = val;
        -- set_count_;
        return true;
    }

    T get(coord const & c) const noexcept {
        if (!c.valid() || empty()) return InvalidT;
        return datas_[c];
    }

    T const & operator[](coord const & c) const noexcept {
        return datas_[c];
    }
};
