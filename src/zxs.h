#pragma once
#ifndef ZXS_H
#define ZXS_H

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

#include "zxbridge.h"
#include "zxports.h"
#include "zxkeyboard.h"
#include "zxmemory.h"
#include "z80cpu.h"
#include "zxbeeper.h"
#include "zxtape.h"
#include "zxula.h"
#include "zxevents.h"

class ZxS: public event::handler<ZxS, EVENT::SYS_LOAD_Z80_SNAPSHOT> {
  public:

    ZxBridge bridge;
    ZxKeyboard keyboard;
    ZxMemory memory;
    ZxBeeper beeper;
    ZxTape tape;
    ZxULA ula;
    ZxPorts ports;
    Z80 z80;

    ZxS() noexcept;
    ~ZxS() noexcept = default;

    std::expected<int, std::string> load_from_z80(std::vector<uint8_t> &&data) noexcept;
    std::expected<int, std::string> load_from_sna(std::vector<uint8_t> &&data) noexcept;
    std::expected<int, std::string> load_from_mgc(std::vector<uint8_t> &&data) noexcept;
    std::expected<int, std::string> load_firmware(std::vector<uint8_t> &&data) noexcept;
    std::expected<std::vector<uint8_t>, std::string> z80_snapshot() noexcept;

    bool restore_ROM() noexcept;

    void handle(event::data<EVENT::SYS_LOAD_Z80_SNAPSHOT> &ed) {
        load_from_z80(std::move(ed.z80_snapshot));
    }

  private:

    void store_ROM() noexcept;

  private:

    std::vector<uint8_t> _stored_ROM_zst{ };
};

#endif // ZXS_H
