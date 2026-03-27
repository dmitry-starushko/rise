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

#include "zxs.h"

std::expected<int, std::string> ZxS::load_firmware(std::vector<uint8_t> &&data) noexcept {
    if (data.size() > ZxMemory::Map::rom_size_) {
        return std::unexpected(std::format("Firmware size exceeds limit: #{:X} (limit #{:X})", data.size(), ZxMemory::Map::rom_size_));
    }

    memory.apply_dump(std::vector(data.begin(), data.end()), ZxMemory::Map::rom_org_);
    z80.reset();
    return 0;
}
