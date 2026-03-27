#pragma once
#ifndef ZXBRIDGE_H
#define ZXBRIDGE_H

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
#include "zxjoystick_default.h"
#include "zxmouse_null.h"
#include "zxprinter_null.h"
#include "keyscancodes.h"
#include "forge.h"
#include <flat_map>
#include <vector>
#include <memory>

class ZxBridge final: public event::handler<ZxBridge, EVENT::GPAD_EVENT_FIRED> {
  public:

    ZxBridge() noexcept;
    ~ZxBridge() noexcept = default;

    INLINE
    void key_down (unsigned scan_code) noexcept {
        if (auto ton = _trig_on.find(scan_code); ton != _trig_on.end()) {
            ton->second(*_virt_joystick);
        } else {
            translate_key<true>(scan_code);
        }
    }

    INLINE
    void key_up (unsigned scan_code) noexcept {
        if (auto toff = _trig_off.find(scan_code); toff != _trig_off.end()) {
            toff->second(*_virt_joystick);
        } else {
            translate_key<false>(scan_code);
        }
    }

    // --- Joystick

    INLINE
    std::string_view current_virtual_joystick() const noexcept {
        return _virt_joystick->name();
    }

    INLINE
    bool select_virtual_joystick(std::string_view kind) {
        if (auto joystick = fg::forge<ZxJoystick>::craft(kind); joystick) {
            _virt_joystick->deactivate();
            _virt_joystick = std::move(*joystick);
            return true;
        }

        return false;
    }

    INLINE
    static std::vector<std::string_view> available_joyticks() noexcept {
        return fg::forge<ZxJoystick>::recipes();
    }

    // --- Mouse

    INLINE
    std::string_view current_mouse() const noexcept {
        return _mouse->name();
    }

    INLINE bool mouse_is_null_object() const noexcept {
        return _mouse->is_null();
    }

    INLINE
    bool select_mouse(std::string_view kind) {
        if (auto mouse = fg::forge<ZxMouse>::craft(kind); mouse) {
            _mouse->update(0, 0, false, false, false);
            _mouse = std::move(*mouse);
            return true;
        }

        return false;
    }

    INLINE
    void update_mouse(uint8_t x, uint8_t y, bool left, bool middle, bool right) noexcept {
        _mouse->update(x, y, left, middle, right);
    }

    INLINE
    static std::vector<std::string_view> available_mouses() noexcept {
        return fg::forge<ZxMouse>::recipes();
    }

  public:

    INLINE
    void handle(const event::data<EVENT::GPAD_EVENT_FIRED> &ed) noexcept {
        using namespace zx::kbd;

        const static std::flat_map<SDL_GamepadButton, std::function<void()>> butt_down {
            {SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_LEFT, [this] INLINE noexcept {_virt_joystick->left(true);}},
            {SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_RIGHT, [this] INLINE noexcept {_virt_joystick->right(true);}},
            {SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_UP, [this] INLINE noexcept {_virt_joystick->up(true);}},
            {SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_DOWN, [this] INLINE noexcept {_virt_joystick->down(true);}},
            {SDL_GamepadButton::SDL_GAMEPAD_BUTTON_WEST, [this] INLINE noexcept {event::dispatch(event::data<EVENT::KEY_DOWN> {.key = key::SPACE});}},
            {SDL_GamepadButton::SDL_GAMEPAD_BUTTON_NORTH, [this] INLINE noexcept {event::dispatch(event::data<EVENT::KEY_DOWN> {.key = key::ENTER});}},
        };

        const static std::flat_map<SDL_GamepadButton, std::function<void()>> butt_up {
            {SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_LEFT, [this] INLINE noexcept {_virt_joystick->left(false);}},
            {SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_RIGHT, [this] INLINE noexcept {_virt_joystick->right(false);}},
            {SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_UP, [this] INLINE noexcept {_virt_joystick->up(false);}},
            {SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_DOWN, [this] INLINE noexcept {_virt_joystick->down(false);}},
            {SDL_GamepadButton::SDL_GAMEPAD_BUTTON_WEST, [this] INLINE noexcept {event::dispatch(event::data<EVENT::KEY_UP> {.key = key::SPACE});}},
            {SDL_GamepadButton::SDL_GAMEPAD_BUTTON_NORTH, [this] INLINE noexcept {event::dispatch(event::data<EVENT::KEY_UP> {.key = key::ENTER});}},
        };

        switch (ed.event.gdevice.type) {
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                if (auto i = butt_down.find(static_cast<SDL_GamepadButton>(ed.event.gbutton.button)); i != butt_down.end()) {
                    i->second();
                } else {
                    _virt_joystick->fire_0(true);
                }

                break;

            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                if (auto i = butt_up.find(static_cast<SDL_GamepadButton>(ed.event.gbutton.button)); i != butt_up.end()) {
                    i->second();
                } else {
                    _virt_joystick->fire_0(false);
                }

                break;

            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                if (auto axis = ed.event.gaxis.axis; 0 <= axis && axis <= 3) {
                    constexpr decltype(ed.event.gaxis.value) dead = 32'768 / 8;
                    auto value = ed.event.gaxis.value;

                    if (axis & 1) {
                        if (value < -dead) {
                            _virt_joystick->up(true);
                        } else {
                            _virt_joystick->up(false);
                        }

                        if (value > dead) {
                            _virt_joystick->down(true);
                        } else {
                            _virt_joystick->down(false);
                        }
                    } else {
                        if (value < -dead) {
                            _virt_joystick->left(true);
                        } else {
                            _virt_joystick->left(false);
                        }

                        if (value > dead) {
                            _virt_joystick->right(true);
                        } else {
                            _virt_joystick->right(false);
                        }
                    }
                }

                break;

            default:
                break;
        }
    }

  private:

    enum JTRSC : unsigned {
        UP = static_cast<unsigned>(KSC::ARR_UP),
        DOWN = static_cast<unsigned>(KSC::ARR_DOWN),
        LEFT = static_cast<unsigned>(KSC::ARR_LEFT),
        RIGHT = static_cast<unsigned>(KSC::ARR_RIGHT),
        FIRE_0 = static_cast<unsigned>(KSC::LEFT_ALT),
        FIRE_1 = static_cast<unsigned>(KSC::TILDA)
    };

    template<bool down>
    INLINE
    void translate_key(unsigned scan_code) noexcept {
        if (auto kci = _key_map.find(static_cast<KSC>(scan_code)); kci != _key_map.end()) {
            for (auto k : kci->second) {
                if constexpr (down) {
                    event::dispatch(event::data<EVENT::KEY_DOWN> {.key = k});
                } else {
                    event::dispatch(event::data<EVENT::KEY_UP> {.key = k});
                }
            }
        }
    }

  private:

    const std::flat_map<KSC, std::vector<zx::kbd::key>> _key_map;

    const std::flat_map<unsigned, std::function<void(ZxJoystick &)>> _trig_on {
        {JTRSC::UP, [] INLINE (auto & j) noexcept { j.up(true); }},
        {JTRSC::DOWN, [] INLINE (auto & j) noexcept { j.down(true); }},
        {JTRSC::LEFT, [] INLINE (auto & j) noexcept { j.left(true); }},
        {JTRSC::RIGHT, [] INLINE (auto & j) noexcept { j.right(true); }},
        {JTRSC::FIRE_0, [] INLINE (auto & j) noexcept { j.fire_0(true); }},
        {JTRSC::FIRE_1, [] INLINE (auto & j) noexcept { j.fire_1(true); }},
    };

    const std::flat_map<unsigned, std::function<void(ZxJoystick &)>> _trig_off {
        {JTRSC::UP, [] INLINE (auto & j) noexcept { j.up(false); }},
        {JTRSC::DOWN, [] INLINE (auto & j) noexcept { j.down(false); }},
        {JTRSC::LEFT, [] INLINE (auto & j) noexcept { j.left(false); }},
        {JTRSC::RIGHT, [] INLINE (auto & j) noexcept { j.right(false); }},
        {JTRSC::FIRE_0, [] INLINE (auto & j) noexcept { j.fire_0(false); }},
        {JTRSC::FIRE_1, [] INLINE (auto & j) noexcept { j.fire_1(false); }},
    };

    std::unique_ptr<ZxJoystick> _virt_joystick{std::make_unique<ZxDefaultJoystick>()};
    std::unique_ptr<ZxMouse> _mouse{std::make_unique<ZxNullMouse>()};
    std::unique_ptr<ZxPrinter> _printer{std::make_unique<ZxNullPrinter>()};
};

#endif // ZXBRIDGE_H
