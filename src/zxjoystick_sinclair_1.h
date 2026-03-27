#pragma once
#ifndef ZXJOYSTICK_SINCLAIR_1_H
#define ZXJOYSTICK_SINCLAIR_1_H

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
#include "zxjoystick.h"
#include "forge.h"

using namespace fg::literals;

class ZxSinclairJoystick1 final: public ZxKeyboardMappedJoystick <
    zx::kbd::key::D9,
    zx::kbd::key::D8,
    zx::kbd::key::D6,
    zx::kbd::key::D7,
    zx::kbd::key::D0,
    zx::kbd::key::NOT_A_KEY,
    zx::kbd::key::NOT_A_KEY >,
    public fg::artifact<ZxSinclairJoystick1, "Sinclair (Interface II) Right"_art_kind, ZxJoystick>,
    public event::handler<ZxSinclairJoystick1, EVENT::SYS_GET_ACTIVE_JOYSTICK_TYPE> {
  public:

    ARTIFACT

    std::string_view name() const noexcept override {
        return artifact_kind();
    }

    INLINE
    static void handle(event::data<EVENT::SYS_GET_ACTIVE_JOYSTICK_TYPE> &ed) noexcept {
        ed.wkjc = WKJC::SINCLAIR_RIGHT;
    }
};

#endif // ZXJOYSTICK_SINCLAIR_1_H
