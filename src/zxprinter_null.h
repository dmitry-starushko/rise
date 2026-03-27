#pragma once
#ifndef ZXPRINTER_NULL_H
#define ZXPRINTER_NULL_H

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

#include "zxprinter.h"
#include "zxevents.h"
#include "forge.h"

using namespace fg::literals;

class ZxNullPrinter final:
    public ZxPrinter,
    public fg::artifact<ZxNullPrinter, "Disconnected"_art_kind, ZxPrinter>,
    public event::handler<ZxNullPrinter, EVENT::Z80_IO_IN> {

  public:

    ARTIFACT

    std::string_view name() const noexcept override {
        return artifact_kind();
    }

    bool is_null() const noexcept override {
        return true;
    }

    INLINE
    static void handle(event::data<EVENT::Z80_IO_IN> &ed) noexcept {
        using namespace zx::printer;

        if (printer_port(ed.port)) {
            ed.byte = 0x01_u8 << 6;
        }
    }
};

#endif // ZXPRINTER_NULL_H
