#pragma once
#ifndef ZXDEFS_H
#define ZXDEFS_H

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
#include <limits>
#include <cstdint>
#include <stdexcept>

#define _A_ &Z80::_reg, &reg_t::A
#define _F_ &Z80::_reg, &reg_t::F
#define _B_ &Z80::_reg, &reg_t::B
#define _C_ &Z80::_reg, &reg_t::C
#define _D_ &Z80::_reg, &reg_t::D
#define _E_ &Z80::_reg, &reg_t::E
#define _H_ &Z80::_reg, &reg_t::H
#define _L_ &Z80::_reg, &reg_t::L

#define _AF_ &Z80::_reg, &reg_t::AF
#define _BC_ &Z80::_reg, &reg_t::BC
#define _DE_ &Z80::_reg, &reg_t::DE
#define _HL_ &Z80::_reg, &reg_t::HL

#define _IX_ &Z80::_ix_reg, &ix_reg_t::IX
#define _IY_ &Z80::_ix_reg, &ix_reg_t::IY

#define _PC_ &Z80::_sp_reg, &sp_reg_t::PC
#define _SP_ &Z80::_sp_reg, &sp_reg_t::SP
#define _IR_ &Z80::_sp_reg, &sp_reg_t::IR
#define _I_ &Z80::_sp_reg, &sp_reg_t::I
#define _R_ &Z80::_sp_reg, &sp_reg_t::R

#define __ &Z80::_, &decltype(_)::_

// --- literals

template<std::unsigned_integral T>
consteval T checked(unsigned long long u) {
    if (u > std::numeric_limits<T>::max()) {
        throw std::out_of_range{"Value too large"};
    }

    return static_cast<T>(u);
}

consteval uint8_t operator""_u8(unsigned long long u) {
    return checked<uint8_t>(u);
}

consteval uint16_t operator""_u16(unsigned long long u) {
    return checked<uint16_t>(u);
}

// --- word <=> lo & hi

INLINE
inline uint16_t _word(uint8_t lo, uint8_t hi) noexcept {
    union {
        struct {
            uint8_t l;
            uint8_t h;
        };
        uint16_t w;
    } u;
    return u.l = lo, u.h = hi, u.w;
}

INLINE
inline uint8_t _lo(uint16_t u16) noexcept {
    const union {
        [[maybe_unused]]
        uint16_t _;
        struct {
            uint8_t l;
            [[maybe_unused]]
            uint8_t h;
        };
    } &u = reinterpret_cast<decltype(u)>(u16);
    return u.l;
}

INLINE
inline uint8_t _hi(uint16_t u16) noexcept {
    const union {
        [[maybe_unused]]
        uint16_t _;
        struct {
            [[maybe_unused]]
            uint8_t l;
            uint8_t h;
        };
    } &u = reinterpret_cast<decltype(u)>(u16);
    return u.h;
}

// --- Concepts

// *INDENT-OFF*

template<typename Fn> concept bin_op_u8
    = requires(const Fn &fn, uint8_t lh, uint8_t rh) { { fn(lh, rh) } noexcept -> std::same_as<uint8_t>; };

template<typename T, int bit_n> concept bit_position
    = std::integral<T> && (0 <= bit_n) && (bit_n < sizeof(T) * 8);

template<uint16_t addr> concept rst_address
    = (addr % 8 == 0) && (addr <= 0x0038_u16);

// *INDENT-ON*

// --- Constants & typedefs

namespace zx {

namespace screen {

constexpr int pixels_h = 256;
constexpr int pixels_v = 192;
constexpr int cc_h = pixels_h / 8;
constexpr int cc_v = pixels_v / 8;
constexpr int disp_width = 320;
constexpr int disp_height = 240;

template<int _base = 0, int row_shift = 0>
INLINE
constexpr inline int offset(int row) noexcept {
    row += row_shift;
    return _base + (((row & 0xC0) << 5) | ((row & 0x38) << 2) | ((row & 0x07) << 8));
}

template<int _base = 0, int row_shift = 0>
INLINE
constexpr inline auto offset_mask(int col, int row) noexcept {
    return std::make_tuple(offset<_base, row_shift>(row) | (col >> 3), 0x80 >> (col & 0x03));
}

}

namespace lines {

constexpr int border_top = 48;
constexpr int border_bottom = 56;
constexpr int bitmap = screen::pixels_v;
constexpr int vblank = 16;
constexpr int visible = border_top + bitmap + border_bottom;
constexpr int per_frame = visible + vblank;
static_assert(per_frame == 312, "This is verified fact!");

}

namespace ticks {

constexpr int border_side = 24;
constexpr int bitmap_line = 128;
constexpr int hblank = 48;
constexpr int per_line = 2 * border_side + bitmap_line + hblank;
constexpr int per_frame = per_line * lines::per_frame;
constexpr int per_vblank = per_line * lines::vblank;
static_assert(per_line == 224, "This is verified fact!");

}

namespace rate {

constexpr int z80_clock_rate = 3'500'000;
constexpr long double frame_rate = static_cast<long double>(z80_clock_rate) / ticks::per_frame;
constexpr long double frame_duration_us = 1'000'000'000.0l / frame_rate;

namespace literals {

inline consteval int operator""_zxms(unsigned long long ms) {
    return z80_clock_rate * static_cast<int>(ms) / 1000;
}

}

}

namespace border {

constexpr int disp_width = (screen::disp_width - screen::pixels_h) / 2;
constexpr int disp_height = (screen::disp_height - screen::pixels_v) / 2;
constexpr int skip_lines = lines::border_top - disp_height;

}

namespace color {

using rgb_t = std::tuple<uint8_t, uint8_t, uint8_t>;

}

namespace kbd {

template<int demi_row, int key_num>
consteval uint8_t encode() noexcept {
    static_assert(0 <= demi_row && demi_row < 8 && 0 <= key_num && key_num < 5, "Wrong key!");
    return (static_cast<uint8_t>(demi_row) << 5) | (1_u8 << key_num) ;
}

enum class key : uint8_t {
    CAPS_SHIFT = encode<0, 0>(), Z = encode<0, 1>(), X = encode<0, 2>(), C = encode<0, 3>(), V = encode<0, 4>(),
    A = encode<1, 0>(), S = encode<1, 1>(), D = encode<1, 2>(), F = encode<1, 3>(), G = encode<1, 4>(),
    Q = encode<2, 0>(), W = encode<2, 1>(), E = encode<2, 2>(), R = encode<2, 3>(), T = encode<2, 4>(),
    D1 = encode<3, 0>(), D2 = encode<3, 1>(), D3 = encode<3, 2>(), D4 = encode<3, 3>(), D5 = encode<3, 4>(),
    D0 = encode<4, 0>(), D9 = encode<4, 1>(), D8 = encode<4, 2>(), D7 = encode<4, 3>(), D6 = encode<4, 4>(),
    P = encode<5, 0>(), O = encode<5, 1>(), I = encode<5, 2>(), U = encode<5, 3>(), Y = encode<5, 4>(),
    ENTER = encode<6, 0>(), L = encode<6, 1>(), K = encode<6, 2>(), J = encode<6, 3>(), H = encode<6, 4>(),
    SPACE = encode<7, 0>(), SYMBOL_SHIFT = encode<7, 1>(), M = encode<7, 2>(), N = encode<7, 3>(), B = encode<7, 4>(),
    NOT_A_KEY = 0x1F_u8
};

INLINE
constexpr int demi_row(key k) noexcept {
    return static_cast<int>(k) >> 5;
}

INLINE
constexpr uint8_t key_mask(key k) noexcept {
    return static_cast<uint8_t>(k) & 0x1F_u8;
}

INLINE
inline bool keyboard_port(uint16_t port) noexcept {
    return !(port & 0x0001_u16);
}

}

namespace joysitick {

INLINE
inline bool kempston_port(uint16_t port) noexcept {
    return !(port & (0x0003_u16 << 5));
}

}

namespace mouse {

INLINE
inline bool kempston_buttons_port(uint16_t port) noexcept {
    return port == 0xFADF;
}

INLINE
inline bool kempston_x_port(uint16_t port) noexcept {
    return port == 0xFBDF;
}

INLINE
inline bool kempston_y_port(uint16_t port) noexcept {
    return port == 0xFFDF;
}

}

namespace beeper {

INLINE
inline bool beeper_port(uint16_t port) noexcept {
    return kbd::keyboard_port(port);
}

}

namespace border {

INLINE
inline bool border_port(uint16_t port) noexcept {
    return kbd::keyboard_port(port);
}

}

namespace tape {

INLINE
inline bool tape_port(uint16_t port) noexcept {
    return kbd::keyboard_port(port);
}

}

namespace printer {

INLINE
inline bool printer_port(uint16_t port) noexcept {
    return !(port & (0x0001_u16 << 2));
}

}

namespace ula {

INLINE
inline bool fake_port(uint16_t port) noexcept {
    return _lo(port) == 0xFF_u8;
}

}

};

#endif // ZXDEFS_H
