#pragma once
#ifndef ZXJOYSTICK_KEMPSTON_H
#define ZXJOYSTICK_KEMPSTON_H

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

#include "zxjoystick.h"
#include "zxevents.h"
#include "forge.h"

using namespace fg::literals;

class ZxKempstonJoystick final:
    public ZxJoystick,
    public fg::artifact<ZxKempstonJoystick, "Kempston"_art_kind, ZxJoystick>,
    public event::handler<ZxKempstonJoystick, EVENT::Z80_IO_IN>,
    public event::handler<ZxKempstonJoystick, EVENT::SYS_GET_ACTIVE_JOYSTICK_TYPE> {

  public:

    ARTIFACT

    std::string_view name() const noexcept override {
        return artifact_kind();
    }

    virtual void up(bool state) noexcept override {
        _up = state;
    }

    virtual void down(bool state) noexcept override {
        _down = state;
    }

    virtual void left(bool state) noexcept override {
        _left = state;
    }

    virtual void right(bool state) noexcept override {
        _right = state;
    }

    virtual void fire_0(bool state) noexcept override {
        _fire_0 = state;
    }

    virtual void fire_1(bool state) noexcept override {
        _fire_1 = state;
    }

    virtual void deactivate() noexcept override { }

    INLINE
    void handle(event::data<EVENT::Z80_IO_IN> &ed) noexcept {
        using namespace zx::joysitick;

        if (kempston_port(ed.port)) {
            ed.byte = _state;
        }
    }

    INLINE
    static void handle(event::data<EVENT::SYS_GET_ACTIVE_JOYSTICK_TYPE> &ed) noexcept {
        ed.wkjc = WKJC::KEMPSTON;
    }

  private:

    union {
        uint8_t _state {0};
        struct {
            uint8_t _right: 1;
            uint8_t _left: 1;
            uint8_t _down: 1;
            uint8_t _up: 1;
            uint8_t _fire_0: 1;
            uint8_t _: 2;
            uint8_t _fire_1: 1;
        };
    };
};

#endif // ZXJOYSTICK_KEMPSTON_H
