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

struct __attribute__((packed)) mgc_header {
    uint16_t AF;
    uint16_t BC;
    uint16_t DE;
    uint16_t HL;
    uint16_t AF_;
    uint16_t BC_;
    uint16_t DE_;
    uint16_t HL_;
    uint16_t IR;
    uint16_t IX;
    uint16_t IY;
    uint16_t SP;
    uint16_t PC;
    uint32_t imode;
    uint8_t iff1;
    uint8_t iff2;
    uint8_t halted;
};

static_assert(sizeof(mgc_header) == 33, "This is verified fact!");

std::expected<int, std::string> ZxS::load_from_mgc(std::vector<uint8_t> &&data) noexcept {
    constexpr auto expected_size = sizeof(mgc_header) + ZxMemory::Map::size_;

    if (data.size() != expected_size) {
        return std::unexpected(std::format("Wrong MGC data size: #{:X} (expected #{:X})", data.size(), expected_size));
    }

    memory.apply_dump(std::vector(data.begin() + sizeof(mgc_header) + ZxMemory::Map::rom_size_, data.end()), ZxMemory::Map::ram_org_);
    auto [_reg, _sh_reg, _ix_reg, _sp_reg, _iff1, _iff2, _imode, _halted] = z80.access_hatch();
    const mgc_header &hdr = *reinterpret_cast<mgc_header *>(data.data());

    _reg.AF = hdr.AF;
    _reg.BC = hdr.BC;
    _reg.DE = hdr.DE;
    _reg.HL = hdr.HL;
    _sh_reg.AF = hdr.AF_;
    _sh_reg.BC = hdr.BC_;
    _sh_reg.DE = hdr.DE_;
    _sh_reg.HL = hdr.HL_;
    _ix_reg.IX = hdr.IX;
    _ix_reg.IY = hdr.IY;
    _sp_reg.SP = hdr.SP;
    _sp_reg.PC = hdr.PC;
    _sp_reg.IR = hdr.IR;
    _iff1 = !!hdr.iff1;
    _iff2 = !!hdr.iff2;
    _imode = static_cast<Z80::IMode>(hdr.imode);

    return 0;
}
