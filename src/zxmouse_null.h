#pragma once
#ifndef ZXMOUSE_NULL_H
#define ZXMOUSE_NULL_H

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

#include "zxmouse.h"
#include "forge.h"

using namespace fg::literals;

class ZxNullMouse final:
    public ZxMouse,
    public fg::artifact<ZxNullMouse, "Disconnected"_art_kind, ZxMouse> {

  public:

    ARTIFACT

    std::string_view name() const noexcept override {
        return artifact_kind();
    }

    virtual void update(uint8_t, uint8_t, bool, bool, bool) noexcept override {
    }

    bool is_null() const noexcept override {
        return true;
    }
};

#endif // ZXMOUSE_NULL_H
