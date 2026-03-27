#pragma once
#ifndef ZXJOYSTICK_H
#define ZXJOYSTICK_H

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

class ZxJoystick {
  public:

    ZxJoystick() noexcept = default;
    virtual ~ZxJoystick() noexcept = default;

    virtual std::string_view name() const noexcept = 0;
    virtual void up(bool) noexcept = 0;
    virtual void down(bool) noexcept = 0;
    virtual void left(bool) noexcept = 0;
    virtual void right(bool) noexcept = 0;
    virtual void fire_0(bool) noexcept = 0;
    virtual void fire_1(bool) noexcept = 0;
    virtual void deactivate() noexcept = 0;
};

template<zx::kbd::key key_up,
         zx::kbd::key key_down,
         zx::kbd::key key_left,
         zx::kbd::key key_right,
         zx::kbd::key key_fire_0,
         zx::kbd::key key_fire_1,
         zx::kbd::key key_mod>
class ZxKeyboardMappedJoystick:
    public ZxJoystick {

  public:

    virtual void up(bool state) noexcept override {
        send_key<key_up>(state);
    }

    virtual void down(bool state) noexcept override {
        send_key<key_down>(state);
    }

    virtual void left(bool state) noexcept override {
        send_key<key_left>(state);
    }

    virtual void right(bool state) noexcept override {
        send_key<key_right>(state);
    }

    virtual void fire_0(bool state) noexcept override {
        send_key<key_fire_0>(state);
    }

    virtual void fire_1(bool state) noexcept override {
        send_key<key_fire_1>(state);
    }

    virtual void deactivate() noexcept override {
        send_key<key_up>(false);
        send_key<key_down>(false);
        send_key<key_left>(false);
        send_key<key_right>(false);
        send_key<key_fire_0>(false);
        send_key<key_fire_1>(false);
    }

  private:

    template<zx::kbd::key k, bool down>
    INLINE
    static void send_key() noexcept {
        if constexpr (k != zx::kbd::key::NOT_A_KEY ) {
            if constexpr (down) {
                if constexpr (key_mod != zx::kbd::key::NOT_A_KEY ) {
                    event::dispatch(event::data<EVENT::KEY_DOWN> {.key = key_mod});
                }

                event::dispatch(event::data<EVENT::KEY_DOWN> {.key = k});

            } else {
                if constexpr (key_mod != zx::kbd::key::NOT_A_KEY ) {
                    event::dispatch(event::data<EVENT::KEY_UP> {.key = key_mod});
                }

                event::dispatch(event::data<EVENT::KEY_UP> {.key = k});

            }
        }
    }

    template<zx::kbd::key k>
    INLINE
    static void send_key(bool state) noexcept {
        if (state) {
            send_key<k, true>();
        } else {
            send_key<k, false>();
        }
    }
};

#endif // ZXJOYSTICK_H
