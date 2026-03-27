#pragma once
#ifndef ZXDEFAULTJOYSTICK_H
#define ZXDEFAULTJOYSTICK_H

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

class ZxDefaultJoystick final:
    public ZxKeyboardMappedJoystick <
    zx::kbd::key::D7,
    zx::kbd::key::D6,
    zx::kbd::key::D5,
    zx::kbd::key::D8,
    zx::kbd::key::NOT_A_KEY,
    zx::kbd::key::NOT_A_KEY,
    zx::kbd::key::CAPS_SHIFT >,
    public fg::artifact<ZxDefaultJoystick, "Cursor control"_art_kind, ZxJoystick> {
  public:

    ARTIFACT

    std::string_view name() const noexcept override {
        return artifact_kind();
    }
};

#endif // ZXDEFAULTJOYSTICK_H
