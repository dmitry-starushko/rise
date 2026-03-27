#pragma once
#ifndef ZXMOUSE_KEMPSTON_H
#define ZXMOUSE_KEMPSTON_H

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

#include "zxmouse.h"
#include "zxevents.h"
#include "forge.h"

using namespace fg::literals;

class ZxKempstonMouse final:
    public ZxMouse,
    public fg::artifact<ZxKempstonMouse, "Kempston"_art_kind, ZxMouse>,
    public event::handler<ZxKempstonMouse, EVENT::Z80_IO_IN> {

  public:

    ARTIFACT

    std::string_view name() const noexcept override {
        return artifact_kind();
    }

    virtual void update(uint8_t x, uint8_t y, bool left, bool middle, bool right) noexcept override {
        _x = x;
        _y = y;
        _left = !left;
        _middle = !middle;
        _right = !right;
    }

    bool is_null() const noexcept override {
        return false;
    }

    INLINE
    void handle(event::data<EVENT::Z80_IO_IN> &ed) noexcept {
        using namespace zx::mouse;

        if (kempston_buttons_port(ed.port)) {
            ed.byte = _buttons;
        } else if (kempston_x_port(ed.port)) {
            ed.byte = _x;
        } else if (kempston_y_port(ed.port)) {
            ed.byte = zx::screen::pixels_v - 1 - _y;
        }
    }

  private:

    union {
        uint8_t _buttons {0xFF_u8};
        struct {
            uint8_t _left: 1;
            uint8_t _right: 1;
            uint8_t _middle: 1;
            uint8_t _reserved: 1;
        };
    };
    uint8_t _x {0x00_u8};
    uint8_t _y {0x00_u8};
};

#endif // ZXMOUSE_KEMPSTON_H
