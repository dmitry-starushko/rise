#pragma once
#ifndef ZXEVENTS_H
#define ZXEVENTS_H

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

#include "events.h"
#include "zxdefs.h"
#include <SDL3/SDL.h>
#include <cstdint>
#include <list>

enum class APDS_MODE : int { M_OFF = 0, M_LENIENT = 1, M_STRICT = 2 };
enum class MSG_KIND : uint8_t { INFO, WARN, ERROR };

enum class EVENT {
    /* --- Z80 events ---
     *
     */

    Z80_STEP,
    Z80_SET_PC_TRAP,
    Z80_RELEASE_PC_TRAP,
    Z80_PC_TRAPPED,
    Z80_IO_IN,
    Z80_IO_OUT,


    /* --- System events ---
     *
     */

    SYS_NMI,
    SYS_RESTORE_ROM,
    SYS_GET_ACTIVE_JOYSTICK_TYPE,
    SYS_SHOW_MESSAGE,
    SYS_SHOW_STATUS,
    SYS_SELECT_OPTION,
    SYS_CONFIRM_YES_NO,
    SYS_LOAD_Z80_SNAPSHOT,
    SYS_SET_PERFORMANCE_FACTOR,
    SYS_PERIODIC,

    /* --- ULA events ---
     *
     */

    ULA_BORDER_LINE_START,
    ULA_BORDER_LEDGE_START,
    ULA_BORDER_REDGE_START,
    ULA_BITMAP_ROW_START,
    ULA_HBLANK_START,
    ULA_VBLANK_START,
    ULA_SET_PIXEL,
    ULA_DRAW_HLINE,

    /* --- Tape events ---
     *
     */

    TAPE_IMAGE_LOADED,
    TAPE_STARTED,
    TAPE_STOPPED,
    TAPE_CURSOR_MOVED,
    TAPE_PLAY_ON,
    TAPE_PLAY_OFF,
    TAPE_REWIND,
    TAPE_FAST_FORWARD,
    TAPE_REWIND_TO_START,
    TAPE_RESET,
    TAPE_SET_APDS_MODE,

    /* --- Keyboard events ---
     *
     */

    KEY_DOWN,
    KEY_UP,

    /* --- Keyboard events ---
     *
     */

    GPAD_CONNECTED,
    GPAD_DISCONNECTED,
    GPAD_EVENT_FIRED
};

// --- Z80 ---

USE_FAST_UNSAFE_BUS(Z80_STEP)
USE_FAST_UNSAFE_BUS(Z80_SET_PC_TRAP)
USE_FAST_UNSAFE_BUS(Z80_RELEASE_PC_TRAP)
USE_FAST_UNSAFE_BUS(Z80_PC_TRAPPED)
USE_FAST_UNSAFE_BUS(Z80_IO_IN)
USE_FAST_UNSAFE_BUS(Z80_IO_OUT)

template<>
struct event::data<EVENT::Z80_STEP> {
    const int ticks;
};

template<>
struct event::data<EVENT::Z80_SET_PC_TRAP> {
    const uint16_t from;
    const uint16_t to;
    bool result = false;
};

template<>
struct event::data<EVENT::Z80_RELEASE_PC_TRAP> {
    const uint16_t from;
    bool result = false;
};

template<>
struct event::data<EVENT::Z80_PC_TRAPPED> {
    const uint16_t from;
    const uint16_t pc;
};

template<>
struct event::data<EVENT::Z80_IO_IN> {
    const uint16_t port;
    uint8_t byte;
};

template<>
struct event::data<EVENT::Z80_IO_OUT> {
    const uint16_t port;
    const uint8_t byte;
};

// --- SYS ---

USE_FAST_UNSAFE_BUS(SYS_SET_PERFORMANCE_FACTOR)
USE_FAST_UNSAFE_BUS(SYS_PERIODIC)
USE_FAST_UNSAFE_BUS(SYS_NMI)
USE_FAST_UNSAFE_BUS(SYS_GET_ACTIVE_JOYSTICK_TYPE)

enum class WKJC : uint8_t {
    CURSOR = 0x00, KEMPSTON = 0x01, SINCLAIR_LEFT = 0x02, SINCLAIR_RIGHT = 0x03, UNKNOWN = 0x80,
};

template<>
struct event::data<EVENT::SYS_GET_ACTIVE_JOYSTICK_TYPE> {
    WKJC wkjc = WKJC::UNKNOWN;
};

template<>
struct event::data<EVENT::SYS_SHOW_MESSAGE> {
    const MSG_KIND type;
    const std::string_view message;
};

template<>
struct event::data<EVENT::SYS_SHOW_STATUS> {
    const std::string_view text;
    const unsigned show_time;
};

template<>
struct event::data<EVENT::SYS_CONFIRM_YES_NO> {
    const std::string_view title;
    const std::string_view question;
    int choice = -1;
};

template<>
struct event::data<EVENT::SYS_SELECT_OPTION> {
    const std::string_view title;
    const std::string_view hint;
    const std::vector<std::string_view> items;
    int choice = -1;
};

template<>
struct event::data<EVENT::SYS_LOAD_Z80_SNAPSHOT> {
    std::vector<uint8_t> &&z80_snapshot;
};

template<>
struct event::data<EVENT::SYS_SET_PERFORMANCE_FACTOR> {
    const unsigned perf_factor;
};

template<>
struct event::data<EVENT::SYS_PERIODIC> {
    const unsigned perf_factor;
    const bool overload;
};

template<>
struct event::data<EVENT::SYS_NMI> {
};

template<>
struct event::data<EVENT::SYS_RESTORE_ROM> {
};

// --- ULA ---

USE_FAST_UNSAFE_BUS(ULA_BORDER_LINE_START)
USE_FAST_UNSAFE_BUS(ULA_BORDER_LEDGE_START)
USE_FAST_UNSAFE_BUS(ULA_BORDER_REDGE_START)
USE_FAST_UNSAFE_BUS(ULA_BITMAP_ROW_START)
USE_FAST_UNSAFE_BUS(ULA_HBLANK_START)
USE_FAST_UNSAFE_BUS(ULA_VBLANK_START)
USE_FAST_UNSAFE_BUS(ULA_SET_PIXEL)
USE_FAST_UNSAFE_BUS(ULA_DRAW_HLINE)

template<>
struct event::data<EVENT::ULA_BORDER_LINE_START> {
    const int line;
};

template<>
struct event::data<EVENT::ULA_BORDER_LEDGE_START> {
    const int line;
};

template<>
struct event::data<EVENT::ULA_BITMAP_ROW_START> {
    const int line;
};

template<>
struct event::data<EVENT::ULA_BORDER_REDGE_START> {
    const int line;
};

template<>
struct event::data<EVENT::ULA_HBLANK_START> {
    const int line;
};

template<>
struct event::data<EVENT::ULA_VBLANK_START> {
};

template<>
struct event::data<EVENT::ULA_SET_PIXEL> { // TODO use instead callbacks
    const int x;
    const int y;
    const zx::color::rgb_t color;
};

template<>
struct event::data<EVENT::ULA_DRAW_HLINE> { // TODO use instead callbacks
    const int x0;
    const int x1;
    const int y;
    const zx::color::rgb_t color;
};

// --- Tape ---

USE_FAST_UNSAFE_BUS(TAPE_SET_APDS_MODE)
USE_FAST_UNSAFE_BUS(TAPE_CURSOR_MOVED)
USE_FAST_UNSAFE_BUS(TAPE_PLAY_ON)
USE_FAST_UNSAFE_BUS(TAPE_PLAY_OFF)
USE_FAST_UNSAFE_BUS(TAPE_STARTED)
USE_FAST_UNSAFE_BUS(TAPE_STOPPED)

template<>
struct event::data<EVENT::TAPE_IMAGE_LOADED> {
    struct block_info {
        std::string type;
        std::string size;
        std::string desc;
    };
    const std::list<block_info> blocks;
};

template<>
struct event::data<EVENT::TAPE_STARTED> {
};

template<>
struct event::data<EVENT::TAPE_STOPPED> {
    const bool has_blocks;
};

template<>
struct event::data<EVENT::TAPE_CURSOR_MOVED> {
    const size_t block_index;
};

template<>
struct event::data<EVENT::TAPE_PLAY_ON> {
};

template<>
struct event::data<EVENT::TAPE_PLAY_OFF> {
};

template<>
struct event::data<EVENT::TAPE_REWIND> {
};

template<>
struct event::data<EVENT::TAPE_FAST_FORWARD> {
};

template<>
struct event::data<EVENT::TAPE_REWIND_TO_START> {
};

template<>
struct event::data<EVENT::TAPE_RESET> {
};

template<>
struct event::data<EVENT::TAPE_SET_APDS_MODE> {
    const APDS_MODE mode;
    std::string_view mode_name;
};

// --- Keyboard ---

template<>
struct event::data<EVENT::KEY_DOWN> {
    const zx::kbd::key key;
};

template<>
struct event::data<EVENT::KEY_UP> {
    const zx::kbd::key key;
};

// --- Gamepad ---

USE_FAST_UNSAFE_BUS(GPAD_CONNECTED)
USE_FAST_UNSAFE_BUS(GPAD_DISCONNECTED)
USE_FAST_UNSAFE_BUS(GPAD_EVENT_FIRED)

template<>
struct event::data<EVENT::GPAD_CONNECTED> {
    const SDL_JoystickID gamepad_id;
    const std::string_view gamepad_name;
};

template<>
struct event::data<EVENT::GPAD_DISCONNECTED> {
    const SDL_JoystickID gamepad_id;
    const std::string_view gamepad_name;
};

template<>
struct event::data<EVENT::GPAD_EVENT_FIRED> {
    const SDL_Event &event;
};

#endif // ZXEVENTS_H
