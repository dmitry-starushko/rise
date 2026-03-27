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
#include <zstd.h>

ZxS::ZxS() noexcept: ula(memory), ports(memory, keyboard), z80(memory, ports) {
    store_ROM();
    const std::vector<uint8_t> splash {
#embed "../bin/splash.bin"
    };

    memory.apply_dump(splash, 0x8000_u16);
    z80.PC(0x8000_u16);
}

void ZxS::store_ROM() noexcept {
    _stored_ROM_zst.resize(ZSTD_compressBound(ZxMemory::Map::rom_size_));

    if (size_t const comp_size = ZSTD_compress(
            _stored_ROM_zst.data(),
            _stored_ROM_zst.size(),
            memory.pool().data(),
            ZxMemory::Map::rom_size_,
            22); ZSTD_isError(comp_size) ) {
        _stored_ROM_zst.clear();
    } else {
        _stored_ROM_zst.resize(comp_size);
    }
}

bool ZxS::restore_ROM() noexcept {
    if (_stored_ROM_zst.empty()) {
        return false;
    }

    std::vector<uint8_t> decompressed(ZxMemory::Map::rom_size_);

    if (size_t const decomp_size = ZSTD_decompress(
            decompressed.data(),
            decompressed.size(),
            _stored_ROM_zst.data(),
            _stored_ROM_zst.size()); ZSTD_isError(decomp_size) || decomp_size != ZxMemory::Map::rom_size_) {
        return false;
    }

    return static_cast<bool>(load_firmware(std::move(decompressed)));
}
