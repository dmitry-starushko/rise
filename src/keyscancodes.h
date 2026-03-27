#pragma once
#ifndef KEYSCANCODES_H
#define KEYSCANCODES_H

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

/*INDENT-OFF*/

#ifdef __linux__

enum class KSC : unsigned {
    K1 = 0x0A,              K2 = 0x0B,              K3 = 0x0C,              K4 = 0x0D,
    K5 = 0x0E,              K6 = 0x0F,              K7 = 0x10,              K8 = 0x11,
    K9 = 0x12,              K0 = 0x13,              Q = 0x18,               W = 0x19,
    E = 0x1A,               R = 0x1B,               T = 0x1C,               Y = 0x1D,
    U = 0x1E,               I = 0x1F,               O = 0x20,               P = 0x21,
    A = 0x26,               S = 0x27,               D = 0x28,               F = 0x29,
    G = 0x2A,               H = 0x2B,               J = 0x2C,               K = 0x2D,
    L = 0x2E,               Z = 0x34,               X = 0x35,               C = 0x36,
    V = 0x37,               B = 0x38,               N = 0x39,               M = 0x3A,
    LEFT_SHIFT = 0x32,      LEFT_CTRL = 0x25,       LEFT_ALT = 0x40,        RIGHT_SHIFT = 0x3E,
    RIGHT_CTRL = 0x69,      RIGHT_ALT = 0x6C,       SPACE = 0x41,           ENTER = 0x24,
    MINUS = 0x14,           EQUAL = 0x15,           BACK = 0x16,            SEMICOLON = 0x2F,
    APOSTROPHE = 0x30,      COMMA = 0x3B,           DOT = 0x3C,             SLASH = 0x3D,
    ARR_LEFT = 0x71,        ARR_RIGHT = 0x72,       ARR_UP = 0x6F,          ARR_DOWN = 0x74,
    NUM_SLASH = 0x6A,       NUM_ASTERISK = 0x3F,    NUM_MINUS = 0x52,       NUM_PLUS = 0x56,
    NUM_ENTER = 0x68,       NUM_DOT = 0x5B,         NUM_1 = 0x57,           NUM_2 = 0x58,
    NUM_3 = 0x59,           NUM_4 = 0x53,           NUM_5 = 0x54,           NUM_6 = 0x55,
    NUM_7 = 0x4F,           NUM_8 = 0x50,           NUM_9 = 0x51,           NUM_0 = 0x5A,
    TILDA = 0x31,           ESC = 0x09
};

#elif defined _WIN64

enum class KSC : unsigned {
    K1 = 0x02,              K2 = 0x03,              K3 = 0x04,              K4 = 0x05,
    K5 = 0x06,              K6 = 0x07,              K7 = 0x08,              K8 = 0x09,
    K9 = 0x0A,              K0 = 0x0B,              Q = 0x10,               W = 0x11,
    E = 0x12,               R = 0x13,               T = 0x14,               Y = 0x15,
    U = 0x16,               I = 0x17,               O = 0x18,               P = 0x19,
    A = 0x1E,               S = 0x1F,               D = 0x20,               F = 0x21,
    G = 0x22,               H = 0x23,               J = 0x24,               K = 0x25,
    L = 0x26,               Z = 0x2C,               X = 0x2D,               C = 0x2E,
    V = 0x2F,               B = 0x30,               N = 0x31,               M = 0x32,
    LEFT_SHIFT = 0x2A,      LEFT_CTRL = 0x1D,       LEFT_ALT = 0x38,        RIGHT_SHIFT = 0x36,
    RIGHT_CTRL = 0x00,      RIGHT_ALT = 0x138,      SPACE = 0x39,           ENTER = 0x1C,
    MINUS = 0x0C,           EQUAL = 0x0D,           BACK = 0x0E,            SEMICOLON = 0x27,
    APOSTROPHE = 0x28,      COMMA = 0x33,           DOT = 0x34,             SLASH = 0x35,
    ARR_LEFT = 0xE04B,      ARR_RIGHT = 0xE04D,     ARR_UP = 0xE048,        ARR_DOWN = 0xE050,
    NUM_SLASH = 0x135,      NUM_ASTERISK = 0x37,    NUM_MINUS = 0x4A,       NUM_PLUS = 0x4E,
    NUM_ENTER = 0x11C,      NUM_DOT = 0x53,         NUM_1 = 0x4F,           NUM_2 = 0x50,
    NUM_3 = 0x51,           NUM_4 = 0x4B,           NUM_5 = 0x4C,           NUM_6 = 0x4D,
    NUM_7 = 0x47,           NUM_8 = 0x48,           NUM_9 = 0x49,           NUM_0 = 0x52,
    TILDA = 0x29,           ESC = 0x00
};

#endif

/*INDENT-ON*/

#endif // KEYSCANCODES_H
