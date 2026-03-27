#pragma once
#ifndef ZXMOUSE_H
#define ZXMOUSE_H

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

#include <cstdint>
#include <string_view>

class ZxMouse {
  public:

    ZxMouse() noexcept = default;
    virtual ~ZxMouse() noexcept = default;
    virtual std::string_view name() const noexcept = 0;
    virtual bool is_null() const noexcept = 0;
    virtual void update(uint8_t x, uint8_t y, bool left, bool middle, bool right) noexcept = 0;
};

#endif // ZXMOUSE_H
