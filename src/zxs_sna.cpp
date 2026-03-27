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

struct __attribute__((packed)) sna_header {
    uint8_t I;                  // 00 | 1 | I
    uint16_t HL_;               // 01 | 2 | HL'
    uint16_t DE_;               // 03 | 2 | DE'
    uint16_t BC_;               // 05 | 2 | BC'
    uint16_t AF_;               // 07 | 2 | AF'
    uint16_t HL;                // 09 | 2 | HL
    uint16_t DE;                // 11 | 2 | DE
    uint16_t BC;                // 13 | 2 | BC
    uint16_t IY;                // 15 | 2 | IY
    uint16_t IX;                // 17 | 2 | IX
    uint8_t iff2;               // 19 | 1 | Interrupt (bit 2 contains IFF2, 1=EI/0=DI)
    uint8_t R;                  // 20 | 1 | R
    uint16_t AF;                // 21 | 2 | AF
    uint16_t SP;                // 23 | 2 | SP
    uint8_t imode;              // 25 | 1 | IntMode (0=IM0/1=IM1/2=IM2)
    uint8_t border;             // 26 | 1 | BorderColor (0..7, not used by Spectrum 1.7)
};

static_assert(sizeof(sna_header) == 27, "This is verified fact!");

std::expected<int, std::string> ZxS::load_from_sna(std::vector<uint8_t> &&data) noexcept {
    constexpr auto expected_size = sizeof(sna_header) + ZxMemory::Map::ram_size_;

    if (data.size() != expected_size) {
        return std::unexpected(std::format("Wrong SNA snapshot size: #{:X} (expected #{:X})", data.size(), expected_size));
    }

    memory.apply_dump(std::vector(data.begin() + sizeof(sna_header), data.end()), ZxMemory::Map::ram_org_);
    auto [_reg, _sh_reg, _ix_reg, _sp_reg, _iff1, _iff2, _imode, _halted] = z80.access_hatch();
    const sna_header &hdr = *reinterpret_cast<sna_header *>(data.data());

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
    _sp_reg.PCL = memory.peek(hdr.SP);
    _sp_reg.PCH = memory.peek(hdr.SP + 1);
    _sp_reg.SP = hdr.SP + 2;
    _sp_reg.I = hdr.I;
    _sp_reg.R = hdr.R;
    _iff1 = _iff2 = hdr.iff2 & 0x04_u8;
    _imode = std::clamp(static_cast<Z80::IMode>(hdr.imode), Z80::IMode::IM1, Z80::IMode::IM2);
    ula.border(hdr.border);

    return 0;
}
