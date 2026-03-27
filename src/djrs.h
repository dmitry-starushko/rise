#pragma once
#ifndef DJRS_H
#define DJRS_H

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
#include <map>
#include <optional>

template<typename T> requires std::integral<T>
class disjoint_ranges_set {
  public:

    [[nodiscard]]
    INLINE
    bool add_range(T l, T r) noexcept {
        if (l > r) {
            return false;
        }

        if (auto it = _ranges.upper_bound(l);
                (it != _ranges.begin() && std::prev(it)->second >= l) || (it != _ranges.end() && r >= it->first)) {
            return false;
        }

        _ranges[l] = r;
        return true;
    }

    INLINE
    bool remove_range(T l) noexcept {
        return _ranges.erase(l) == 1;
    }

    [[nodiscard]]
    INLINE
    std::optional<T> find_range(T n) const noexcept {
        auto it = _ranges.upper_bound(n);

        if (it == _ranges.begin()) {
            return std::nullopt;
        }

        if (--it; it->second >= n) {
            return it->first;
        }

        return std::nullopt;
    }

  private:

    std::map<T, T> _ranges;
};

#endif // DJRS_H
