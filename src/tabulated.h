#pragma once
#ifndef TABULATED_H
#define TABULATED_H

/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *  ____  ___ ____  _____
 * |  _ \|_ _/ ___|| ____|
 * | |_) || |\___ \|  _|
 * |  _ < | | ___) | |___
 * |_| \_\___|____/|_____|
 *
 * Copyright (C) 2026 Dmitry Starushko <dmitry.starushko@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 */

#include "inline.h"
#include <array>
#include <concepts>
#include <cstdint>
#include <limits>

// *INDENT-OFF*

template<auto formula, std::unsigned_integral T = uint8_t>
class tabulated_func {
    static_assert(sizeof(T) <= 2, "Too wide type!");
    using value_t = decltype(formula(T{0}));
    using table_t = std::array<value_t, std::numeric_limits<T>::max() + 1>;

public:

    tabulated_func() = delete;

    INLINE
    static value_t operator[](T arg) noexcept {
        return _table[arg];
    }

private:

    static constexpr auto generate() {
        table_t tt;
        for (T arg = 0; value_t &result : tt) {
            result = formula(arg++);
        }
        return tt;
    }

    inline static const table_t _table {generate()};
};

// *INDENT-ON*

template<auto formula, std::unsigned_integral T = uint8_t>
INLINE
inline constexpr auto tabulated(T x) noexcept {
    return tabulated_func<formula, T>::operator[](x);
}

#endif // TABULATED_H
