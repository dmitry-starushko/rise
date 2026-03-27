#pragma once
#ifndef ZXULA_H
#define ZXULA_H

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
#include "zxmemory.h"
#include "zxevents.h"

class ZxULA final:
    public event::handler<ZxULA,
    EVENT::Z80_IO_OUT,
    EVENT::ULA_BORDER_LINE_START,
    EVENT::ULA_BORDER_LEDGE_START,
    EVENT::ULA_BITMAP_ROW_START> {
  public:

    static constexpr bool precise = true;

    explicit ZxULA(ZxMemory &memory) noexcept:
        _memory(memory) {
    }
    ~ZxULA() noexcept = default;

    INLINE
    void border(uint8_t b) noexcept {
        _border = b & 0x07_u8;
    }

    INLINE
    uint8_t border() const noexcept {
        return _border;
    }

    INLINE
    void handle(const event::data<EVENT::Z80_IO_OUT> &ed) noexcept {
        using namespace zx::border;

        if (border_port(ed.port)) {
            border(ed.byte);
        }
    }

    INLINE
    void handle(const event::data<EVENT::ULA_BORDER_LINE_START> &ed) noexcept {
        _bbuff[ed.line] = _border;
    }

    INLINE
    void handle(const event::data<EVENT::ULA_BORDER_LEDGE_START> &ed) noexcept {
        _bbuff[ed.line] = _border;
    }

    INLINE
    void handle(const event::data<EVENT::ULA_BITMAP_ROW_START> &ed) noexcept {
        if constexpr (precise) {
            int row = ed.line - zx::lines::border_top;
            auto i = _memory.pool().begin() + ZxMemory::Map::attr_org_ + ((row & 0xF8) << 2);
            std::copy(i, i + zx::screen::cc_h, _abuff[row].begin());
            auto j = _memory.pool().begin() + zx::screen::offset<ZxMemory::Map::vmem_org_>(row);
            std::copy(j, j + zx::screen::cc_h, _pbuff[row].begin());
        }
    }

    using pixel_draw_t = std::function<void(int x, int y, const zx::color::rgb_t &c)>;
    using line_draw_t = std::function<void(int x0, int y0, int x1, int y1, const zx::color::rgb_t &c)>;

    template<bool precise = precise, uint16_t bitmap_org_ = ZxMemory::Map::vmem_org_>
    INLINE
    void translate_screen(bool blink_phase, pixel_draw_t set_pixel, line_draw_t draw_line) const noexcept {

        // -- Border --

        {
            int y = 0, bl = zx::border::skip_lines;

            for (; y < zx::border::disp_height; ++y) {
                draw_line(0, y, zx::screen::disp_width - 1, y, cmap_normal[_bbuff[bl++]]);
            }

            for (; y < zx::border::disp_height + zx::screen::pixels_v; ++y) {
                const auto &color = cmap_normal[_bbuff[bl++]];
                draw_line(0, y, zx::border::disp_width - 1, y, color);
                draw_line(zx::border::disp_width + zx::screen::pixels_h, y, zx::screen::disp_width - 1, y, color);
            }

            for (; y < zx::screen::disp_height; ++y) {
                draw_line(0, y, zx::screen::disp_width - 1, y, cmap_normal[_bbuff[bl++]]);
            }
        }

        // -- Bitmap --

        if constexpr (precise) { // TODO: multithread?
            for (int y = 0; y < zx::screen::pixels_v; ++y) {
                int x = zx::border::disp_width;

                for (auto pi = _pbuff[y].begin(), ai = _abuff[y].begin(); pi != _pbuff[y].end(); ++pi, ++ai) {
                    const auto &attr = reinterpret_cast<const attribute_t &>(*ai);
                    const colormap_t &cmap = attr.flag_bright ? cmap_bright : cmap_normal;
                    const std::array color_pair { cmap[attr.grb_background], cmap[attr.grb_foreground] };
                    const bool blink = blink_phase && attr.flag_blink;

                    for (uint8_t rb = *pi, mask = 0x80_u8; mask; mask >>= 1, ++x) {
                        set_pixel(x, y + zx::border::disp_height, color_pair[blink != !!(rb & mask)]);
                    }
                }
            }
        } else {
            static constexpr uint16_t char_cells_per_third_ = zx::screen::cc_h * zx::screen::cc_v / 3;
            static constexpr uint16_t bitmap_bytes_ = zx::screen::pixels_v * zx::screen::cc_h;
            static constexpr uint16_t bitmap_third_bytes_ = bitmap_bytes_ / 3;
            auto memory = _memory.pool().data();

            // -- Loop over bitmap thirds
            for (uint16_t bmt_addr = bitmap_org_, attr_addr = ZxMemory::Map::attr_org_, bmt_y = 0x0000_u16;
                    bmt_addr < ZxMemory::Map::attr_org_;
                    bmt_addr += bitmap_third_bytes_, bmt_y += zx::screen::pixels_v / 3) {
                // -- Loop over character cells
                for (uint16_t cc_addr = bmt_addr, cc_num = 0x0000_u16; cc_num < char_cells_per_third_; ++cc_addr, ++cc_num) {
                    const uint16_t cc_cx = zx::border::disp_width + ((cc_num & 0x001F_u16) << 3); // -- i.e. + (cc_num % char_cells_h) * 8;
                    const uint16_t cc_cy = zx::border::disp_height + (bmt_y + ((cc_num >> 2) & 0xFFF8_u16)); // -- i.e. bmt_y + (cc_num / char_cells_h) * 8;

                    const auto &attr = reinterpret_cast<const attribute_t &>(memory[attr_addr++]);
                    const colormap_t &cmap = attr.flag_bright ? cmap_bright : cmap_normal;
                    const std::array color_pair { cmap[attr.grb_background], cmap[attr.grb_foreground] };
                    const bool blink = blink_phase && attr.flag_blink;

                    // -- Loop over the cell's rows
                    for (uint16_t bm_addr = cc_addr, cc_row = 0x0000_u16; cc_row < 0x0008_u16; bm_addr += 0x0100_u16, ++cc_row) {
                        const uint8_t bm_byte = memory[bm_addr];
                        const uint16_t cy = cc_cy + cc_row;
                        uint16_t cx = cc_cx;

                        // -- Loop over the cell's columns
                        for (uint8_t mask = 0x80_u8; mask; mask >>= 1, ++cx) {
                            set_pixel(cx, cy, color_pair[blink != !!(bm_byte & mask)]);
                        }
                    }
                }
            }
        }
    }

  private:
    struct attribute_t {
        uint8_t grb_foreground: 3;
        uint8_t grb_background: 3;
        uint8_t flag_bright: 1;
        uint8_t flag_blink: 1;
    };
    static_assert(sizeof(attribute_t) == 1, "Unexpected union size");

    using colormap_t = std::array<zx::color::rgb_t, 8>;
    using pbuffer_t = std::array<std::array<uint8_t, zx::screen::cc_h>, zx::screen::pixels_v>;
    using abuffer_t = pbuffer_t;
    using bbuffer_t = std::array<uint8_t, zx::lines::visible>;

    ZxMemory &_memory;
    uint8_t _border {0xFF};

    pbuffer_t _pbuff{ };
    abuffer_t _abuff{ };
    bbuffer_t _bbuff{ };
    static constexpr colormap_t cmap_normal = { zx::color::rgb_t{0x00, 0x00, 0x00}, {0x00, 0x00, 0xCD}, {0xCD, 0x00, 0x00}, {0xCD, 0x00, 0xCD}, {0x00, 0xCD, 0x00}, {0x00, 0xCD, 0xCD}, {0xCD, 0xCD, 0x00}, {0xCD, 0xCD, 0xCD} };
    static constexpr colormap_t cmap_bright = { zx::color::rgb_t{0x00, 0x00, 0x00}, {0x00, 0x00, 0xFF}, {0xFF, 0x00, 0x00}, {0xFF, 0x00, 0xFF}, {0x00, 0xFF, 0x00}, {0x00, 0xFF, 0xFF}, {0xFF, 0xFF, 0x00}, {0xFF, 0xFF, 0xFF} };
};

#endif // ZXULA_H
