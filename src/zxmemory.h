#pragma once
#ifndef MEMORY_H
#define MEMORY_H

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
#include <cstdint>
#include <array>
#include <vector>
#include <span>

class ZxMemory final {
  public:

    struct Map { //TODO to zxdefs?
        static constexpr int size_ = 0x10000;
        static constexpr uint16_t rom_org_ = 0x0000_u16;
        static constexpr uint16_t rom_size_ = 0x4000_u16;
        static constexpr uint16_t ram_org_ = rom_size_;
        static constexpr uint16_t ram_size_ = size_ - rom_size_;
        static constexpr uint16_t vmem_org_ = ram_org_;
        static constexpr uint16_t vmem_size_ = 0x1B00_u16;
        static constexpr uint16_t bitmap_org_ = vmem_org_;
        static constexpr uint16_t bitmap_size_ = zx::screen::cc_h * zx::screen::pixels_v;
        static constexpr uint16_t attr_org_ = bitmap_org_ + bitmap_size_;
        static constexpr uint16_t attr_size_ = zx::screen::cc_h * zx::screen::cc_v;
        static constexpr uint16_t entry_warm_reset_ = 0x11B7_u16;
    };

    using pool_t = std::array<uint8_t, Map::size_>;

    ZxMemory() noexcept;
    ~ZxMemory() noexcept = default;

    INLINE
    uint8_t peek (uint16_t addr) const noexcept {
        return _pool[addr];
    }

    INLINE
    void poke (uint16_t addr, uint8_t data) noexcept {
        if (addr >= Map::rom_size_) {
            _pool[addr] = data;
        }
    }

    INLINE
    const pool_t &pool() const noexcept {
        return _pool;
    }

    INLINE
    std::span<uint8_t> ram() noexcept {
        return std::span<uint8_t> {_pool.begin() + Map::rom_size_, _pool.end()};
    }

    INLINE
    void apply_dump(const std::vector<uint8_t> &bytes, uint16_t start_addr) noexcept {
        // TODO: revamp for paged memory
        if (start_addr < _pool.size()) {
            auto count = std::min(bytes.size(), _pool.size() - start_addr);
            std::ranges::copy_n(bytes.begin(), count, _pool.begin() + start_addr);
        }
    }

  private:

    pool_t _pool;
};
#endif // MEMORY_H
