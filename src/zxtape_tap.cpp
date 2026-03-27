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

#include "zxtape.h"
#include <list>

std::expected<int, std::string> ZxTape::open_tap(std::vector<uint8_t> &&tap_bytes) noexcept {
    std::list<std::pair<size_t, size_t>> blocks;
    uint8_t lo;

    if (const size_t tbsz = tap_bytes.size(); tbsz > 2z) {
        for (size_t i = 0; i + 1 < tbsz;) {
            if (size_t bsz = (lo = tap_bytes[i++], _word(lo, tap_bytes[i++])); bsz > 0) {
                if (size_t j = i + bsz; j <= tap_bytes.size()) {
                    blocks.emplace_back(i, bsz);
                    i = j;
                } else {
                    return std::unexpected("Invalid TAP file structure: block out of bounds");
                }
            } else {
                return std::unexpected("Zero-length block detected");
            }
        }
    } else {
        return std::unexpected("No blocks found");
    }

    reset_<true>();
    _tape_bytes = std::move(tap_bytes);
    _generators.reserve(blocks.size());

    ev_til_blocks_t ev_til_blocks;

    auto til_block = [&ev_til_blocks] INLINE
    (const std::string & type, const std::string & size, const std::string & desc) noexcept {
        ev_til_blocks.emplace_back(ev_til_block_t{
            .type = type,
            .size = size.empty() ? "-" : size,
            .desc = desc.empty() ? "-" : desc,
        });
    };

    for (auto [org, size] : blocks) {
        std::span<uint8_t> span(&_tape_bytes[org], size);
        bool header = !span[0];

        _generators.emplace_back(
                    splice(meander(std_pilot_pulse_tck, std_pilot_pulse_tck, header ? std_long_pilot_pulses : std_short_pilot_pulses),
                           meander(std_sync_p1_tck, std_sync_p2_tck, 2),
                           bytes_pulses(std_pulse_bit_0_tck, std_pulse_bit_1_tck, std::move(span)),
                           silence(header ? std_short_pause_tck : std_long_pause_tck)));
        til_block("Std. speed block",
                  std::format("#{:04X}", span.size()),
                  description(span));
    }

    _generators.emplace_back(stopper());
    til_block("End of tape", "", "Stop playing");

    event::dispatch(event::data<EVENT::TAPE_IMAGE_LOADED> {.blocks = std::move(ev_til_blocks)});
    reset_<false>();
    return _generators.size();
}

