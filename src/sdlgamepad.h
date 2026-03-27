#pragma once
#ifndef SDLGAMEPAD_H
#define SDLGAMEPAD_H

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

#include "zxevents.h"
#include "inline.h"
#include <SDL3/SDL.h>
#include <memory>
#include <map>

class SdlGamepad final: public event::handler<SdlGamepad, EVENT::SYS_PERIODIC> {
  public:

    SdlGamepad() {
        discover_gamepads();
        select_gamepad();
    }

    ~SdlGamepad() = default;

    INLINE
    bool button_state(SDL_GamepadButton button) const noexcept {
        return _gamepad ? SDL_GetGamepadButton(_gamepad.get(), button) : false;
    }

    INLINE
    float axis_value(SDL_GamepadAxis axis) const noexcept {
        return _gamepad ? SDL_GetGamepadAxis(_gamepad.get(), axis) / 32'768.0f : 0.0f;
    }

  public:

    INLINE
    void handle(const event::data<EVENT::SYS_PERIODIC> &) noexcept {
        process_events();
    }

  private:

    INLINE
    void discover_gamepads() noexcept {
        using joysticks_ptr = std::unique_ptr <SDL_JoystickID, decltype(&SDL_free)>;

        int count = 0;
        joysticks_ptr joysticks{ SDL_GetJoysticks(&count), &SDL_free};

        for (_discovered_gamepads.clear(); auto jid : std::span(joysticks.get(), count)) {
            if (SDL_IsGamepad(jid)) {
                _discovered_gamepads[jid] = SDL_GetGamepadNameForID(jid);
            }
        }
    }

    INLINE
    void select_gamepad() noexcept {
        if (_discovered_gamepads.empty()) {
            return;
        }

        std::vector<std::string_view> items;
        std::flat_map<int, SDL_JoystickID> select_map;

        for (items.reserve(_discovered_gamepads.size()); const auto &cj : _discovered_gamepads) {
            select_map[items.size()] = cj.first;
            items.push_back(cj.second);
        }

        event::data<EVENT::SYS_SELECT_OPTION> ed {.title = "Select", .hint = "Select gamepad for use", .items = items, .choice = -1};

        if (_discovered_gamepads.size() > 1) {
            event::dispatch(ed);
        }

        if (ed.choice >= 0 || !_gamepad) {
            open_gamepad(select_map[std::clamp(ed.choice, 0, int(select_map.size()) - 1)]);
        }
    }

    INLINE
    void open_gamepad(SDL_JoystickID gpid) noexcept {
        close_gamepad();

        if (_gamepad.reset(SDL_OpenGamepad(gpid)); _gamepad) {
            _gamepad_id = gpid;
            _gamepad_name = SDL_GetGamepadName(_gamepad.get());
            event::dispatch(event::data<EVENT::GPAD_CONNECTED> {.gamepad_id = _gamepad_id, .gamepad_name = _gamepad_name});
        }
    }

    INLINE
    void close_gamepad() noexcept {
        if (_gamepad) {
            event::dispatch(event::data<EVENT::GPAD_DISCONNECTED> {.gamepad_id = _gamepad_id, .gamepad_name = _gamepad_name});
            _gamepad = nullptr;
            _gamepad_id = 0;
            _gamepad_name.clear();
        }
    }

    INLINE
    void process_events() noexcept {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {

                case SDL_EVENT_GAMEPAD_ADDED:

                    if (auto jid = event.gdevice.which; SDL_IsGamepad(jid) && !_discovered_gamepads.contains(jid)) {
                        _discovered_gamepads[jid] = SDL_GetGamepadNameForID(jid);
                        select_gamepad();
                    }

                    break;

                case SDL_EVENT_GAMEPAD_REMOVED:

                    if (auto jid = event.gdevice.which; _discovered_gamepads.contains(jid)) {
                        _discovered_gamepads.erase(jid);

                        if (jid == _gamepad_id) {
                            close_gamepad();
                            select_gamepad();
                        }
                    }

                    break;

                case SDL_EVENT_GAMEPAD_BUTTON_DOWN:

                case SDL_EVENT_GAMEPAD_BUTTON_UP:

                case SDL_EVENT_GAMEPAD_AXIS_MOTION:

                    if (event.gdevice.which == _gamepad_id) {
                        event::dispatch(event::data<EVENT::GPAD_EVENT_FIRED> {.event = event});
                    }

                    break;
            }
        }
    }

  private:

    std::map<SDL_JoystickID, std::string> _discovered_gamepads {};
    std::unique_ptr <SDL_Gamepad, decltype(&SDL_CloseGamepad)> _gamepad{nullptr, &SDL_CloseGamepad};
    SDL_JoystickID _gamepad_id = 0;
    std::string _gamepad_name {};
};

#endif // SDLGAMEPAD_H
