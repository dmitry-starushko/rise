#pragma once
#ifndef KEYBOARD_H
#define KEYBOARD_H

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

#include "zxdefs.h"
#include "zxevents.h"
#include <cstdint>
#include <array>

class ZxKeyboard final: public event::handler<ZxKeyboard, EVENT::KEY_DOWN, EVENT::KEY_UP> {
  public:

    ZxKeyboard () noexcept {
        _demirows.fill(0xFF_u8);
    }
    ~ZxKeyboard() noexcept = default;

    INLINE
    void handle(const event::data<EVENT::KEY_DOWN> &ed) noexcept {
        _demirows[zx::kbd::demi_row(ed.key)] &= ~zx::kbd::key_mask(ed.key);
    }

    INLINE
    void handle(const event::data<EVENT::KEY_UP> &ed) noexcept {
        _demirows[zx::kbd::demi_row(ed.key)] |= zx::kbd::key_mask(ed.key);
    }

    INLINE
    uint8_t scan_state (uint8_t hi_bus) noexcept {
        uint8_t state = 0x1F_u8;

        for (uint8_t mask = 0x01_u8, i = 0x00_u8; mask; mask <<= 1, ++i) {
            if ((hi_bus & mask) == 0) {
                state &= _demirows[i];
            }
        }

        return state;
    }

  private:

    static constexpr int demirows_ = 8;
    std::array<uint8_t, demirows_> _demirows;
};

#endif // KEYBOARD_H
