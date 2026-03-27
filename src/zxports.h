#pragma once
#ifndef IOPORTS_H
#define IOPORTS_H

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
#include "zxmemory.h"
#include "zxkeyboard.h"
#include "zxevents.h"
#include <cstdint>

class ZxPorts final {
  public:

    ZxPorts(ZxMemory &mem, ZxKeyboard &kbd) noexcept:
        _memory(mem),
        _keyboard(kbd) {
    }
    ~ZxPorts() noexcept = default;

    INLINE
    static void out (const uint16_t port, const uint8_t data) noexcept {
        event::dispatch(event::data<EVENT::Z80_IO_OUT> {
            .port = port,
            .byte = data
        });
    }

    INLINE
    uint8_t in (uint16_t port) noexcept {
        using namespace zx::kbd;
        using namespace zx::joysitick;
        using namespace zx::printer;
        using namespace zx::ula;

        if (keyboard_port(port)) {
            event::data<EVENT::Z80_IO_IN> ed {
                .port = port,
                .byte = static_cast<uint8_t>(0xA0_u8 | _keyboard.scan_state(_hi(port)))
            };
            event::dispatch(ed);
            return ed.byte;
        }

        if (fake_port(port)) {
            return current_attribute();
        }

        event::data<EVENT::Z80_IO_IN> ed {
            .port = port,
            .byte = kempston_port(port) ? 0x00_u8 : 0xFF_u8
        };
        event::dispatch(ed);
        return ed.byte;
    }

    INLINE
    int frame_ticks(int ticks) noexcept {
        return (_frame_ticks = ticks);
    }

    INLINE
    uint8_t current_attribute() const noexcept {
        auto [line, ticks_in_line] = std::div(_frame_ticks, zx::ticks::per_line);

        if (line < zx::lines::border_top ||
                line >= zx::lines::border_top + zx::screen::pixels_v ||
                ticks_in_line < zx::ticks::border_side ||
                ticks_in_line >= zx::ticks::border_side + zx::ticks::bitmap_line) {
            return 0xFF_u8;
        }

        return _memory.peek(ZxMemory::Map::attr_org_ +
                            zx::screen::cc_h * ((line - zx::lines::border_top) / 8) +
                            (ticks_in_line - zx::ticks::border_side) / 4);
    }

  private:

    ZxMemory &_memory;
    ZxKeyboard &_keyboard;
    int _frame_ticks {0};
};

#endif // IOPORTS_H
