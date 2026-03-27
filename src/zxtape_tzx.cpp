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

#include "tzx.h"
#include "zxutils.h"
#include "zxtape.h"
#include <cmath>
#include <format>
#include <list>
#include <memory>

namespace {

INLINE
inline int int24(const uint8_t(&triplet)[3]) noexcept {
    return (((triplet[2] << 8) | triplet[1]) << 8) | triplet[0];
}

}

std::expected<int, std::string> ZxTape::open_tzx(std::vector<uint8_t> &&tzx_bytes) noexcept {
    using namespace zxutils;
    constexpr auto strict_mode = true; // TODO false for release?

    auto invalid = static_cast<size_t>(-1);
    auto itr = tzx_bytes.begin();

    auto skip = [&tzx_bytes, &itr] INLINE
    (auto offset) {
        if (auto jtr = itr + offset; jtr > tzx_bytes.end()) {
            throw std::out_of_range("Unexpected end of data");
        } else {
            itr = jtr;
        }
    };

    auto skip_from = [&tzx_bytes, &itr, &skip] INLINE
    (auto from, auto offset) {
        if (from < tzx_bytes.begin() || tzx_bytes.end() <= from) {
            throw std::out_of_range("Out of bounds");
        }

        itr = from;
        skip(offset);
    };

    try {
        const auto &header = consume_as<tzx_header>(tzx_bytes, itr);

        if (std::string signature(header.signature, sizeof(header.signature)); signature != "ZXTape!") {
            return std::unexpected(std::format("Unexpected signature: {}", signature));
        }

        if (header._0x1A != 0x1A) {
            return std::unexpected(std::format("Unexpected terminator: {}", header._0x1A));
        }

        struct block_info_t {
            BLOCK_ID id;
            size_t header;
            size_t data_begin;
            size_t data_size;
        };

        std::list<block_info_t> blocks;

        // -- Preliminary parse && check blocks

        while (itr < tzx_bytes.end()) {

            block_info_t block_info {
                .id = BLOCK_ID::UNKNOWN,
                .header = invalid,
                .data_begin = invalid,
                .data_size = invalid
            };

            try {
                block_info.id = consume_as<BLOCK_ID>(tzx_bytes, itr);
                block_info.header = itr - tzx_bytes.begin();

                switch (block_info.id) {
                    case BLOCK_ID::STD_DATA: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::STD_DATA>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                        skip(bh.length * sizeof(uint8_t));

                        if (!bh.length) {
                            throw std::out_of_range("Unexpected STD DATA length: 0");
                        }
                    }
                    break;

                    case BLOCK_ID::TURBO_DATA: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::TURBO_DATA>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                        const auto length = int24(bh.length);
                        skip(length * sizeof(uint8_t));

                        if (!length) {
                            throw std::out_of_range("Unexpected TURBO DATA length: 0");
                        }

                        if (!bh.pilot_pulse_tck ||
                                !bh.sync_p1_tck ||
                                !bh.sync_p2_tck ||
                                !bh.zero_pulse_tck ||
                                !bh.one_pulse_tck ||
                                !bh.pilot_pulses ||
                                bh.last_byte_bits < 1 ||
                                bh.last_byte_bits > 8) {
                            throw std::out_of_range("Unexpected TURBO DATA parameters!");
                        }
                    }
                    break;

                    case BLOCK_ID::PURE_TONE: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::PURE_TONE>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();

                        if (!bh.pulse_tck || !bh.pulses) {
                            throw std::out_of_range("Unexpected PURE TONE parameters!");
                        }
                    }
                    break;

                    case BLOCK_ID::PULSE_SEQ: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::PULSE_SEQ>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                        skip(bh.length * sizeof(uint16_t));

                        if (!bh.length) {
                            throw std::out_of_range("Unexpected PULSE SEQUENCE length: 0");
                        }
                    }
                    break;

                    case BLOCK_ID::PURE_DATA: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::PURE_DATA>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                        const auto length = int24(bh.length);
                        skip(length * sizeof(uint8_t));

                        if (!length) {
                            throw std::out_of_range("Unexpected PURE DATA length: 0");
                        }

                        if ( !bh.zero_pulse_tck ||
                                !bh.one_pulse_tck ||
                                bh.last_byte_bits < 1 ||
                                bh.last_byte_bits > 8) {
                            throw std::out_of_range("Unexpected PURE DATA parameters!");
                        }
                    }
                    break;

                    case BLOCK_ID::DIRECT_REC: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::DIRECT_REC>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                        const auto length = int24(bh.length);
                        skip(length * sizeof(uint8_t));

                        if (!length) {
                            throw std::out_of_range("Unexpected DIRECT RECORDING length: 0");
                        }

                        if (!bh.ticks_per_sample ||
                                bh.last_byte_bits < 1 ||
                                bh.last_byte_bits > 8) {
                            throw std::out_of_range("Unexpected DIRECT RECORDING parameters!");
                        }
                    }
                    break;

                    case BLOCK_ID::CSW: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::CSW>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                        skip_from(tzx_bytes.begin() + block_info.header, bh.block_length + sizeof bh.block_length);
                    }
                    break;

                    case BLOCK_ID::GENERALIZED_DATA: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::GENERALIZED_DATA>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                        skip_from(tzx_bytes.begin() + block_info.header, bh.block_length + sizeof bh.block_length);

                        if (!bh.block_length) {
                            throw std::out_of_range("Unexpected GENERALIZED DATA block length: 0");
                        }

                        if ((bh.totp && !bh.npp) || (bh.totd && (!bh.npd || bh.asd == 1))) {
                            throw std::out_of_range("Unexpected GENERALIZED DATA parameters!");
                        }
                    }
                    break;

                    case BLOCK_ID::SILENCE: {
                        consume_as<blk_header<BLOCK_ID::SILENCE>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                    }
                    break;

                    case BLOCK_ID::GROUP_START: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::GROUP_START>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                        skip(bh.length * sizeof(uint8_t));
                    }
                    break;

                    case BLOCK_ID::GROUP_END: {
                        block_info.header = block_info.data_begin = (itr - tzx_bytes.begin()) - 1;
                    }
                    break;

                    case BLOCK_ID::JUMP: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::JUMP>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();

                        if (!bh.offset) {
                            throw std::out_of_range("Unexpected JUMP offset: 0");
                        }
                    }
                    break;

                    case BLOCK_ID::LOOP_START: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::LOOP_START>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();

                        if (bh.repetitions < 2) {
                            throw std::out_of_range(std::format("Unexpected LOOP repetitions: {}", bh.repetitions));
                        }
                    }
                    break;

                    case BLOCK_ID::LOOP_END: {
                        block_info.header = block_info.data_begin = (itr - tzx_bytes.begin()) - 1;
                    }
                    break;

                    case BLOCK_ID::CALL_SEQ: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::CALL_SEQ>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                        skip(bh.call_num * sizeof(uint16_t));

                        if (!bh.call_num) {
                            throw std::out_of_range("Unexpected CALL count: 0");
                        }
                    }
                    break;

                    case BLOCK_ID::RETURN: {
                        block_info.header = block_info.data_begin = (itr - tzx_bytes.begin()) - 1;
                    }
                    break;

                    case BLOCK_ID::SELECT: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::SELECT>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();

                        for (int ocn = bh.opt_count; ocn; --ocn) {
                            auto &sel = consume_as<blk_header<BLOCK_ID::SELECT>::SELECT>(tzx_bytes, itr);
                            skip(sel.length * sizeof(char));
                        }

                        if (!bh.block_length) {
                            throw std::out_of_range("Unexpected SELECT block length: 0");
                        }

                        if (!bh.opt_count) {
                            throw std::out_of_range("Unexpected SELECT count: 0");
                        }
                    }
                    break;

                    case BLOCK_ID::STOP_48K: {
                        consume_as<blk_header<BLOCK_ID::STOP_48K>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                    }
                    break;

                    case BLOCK_ID::SET_LEVEL: {
                        consume_as<blk_header<BLOCK_ID::SET_LEVEL>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                    }
                    break;

                    case BLOCK_ID::DESCRIPTION: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::DESCRIPTION>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                        skip(bh.length * sizeof(char));
                    }
                    break;

                    case BLOCK_ID::MESSAGE: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::MESSAGE>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                        skip(bh.length * sizeof(char));
                    }
                    break;

                    case BLOCK_ID::ARCHIVE_INFO: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::ARCHIVE_INFO>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                        skip_from(tzx_bytes.begin() + block_info.header, bh.block_length + sizeof bh.block_length);
                    }
                    break;

                    case BLOCK_ID::HW_TYPE: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::HW_TYPE>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                        skip(bh.hwi_count * sizeof(blk_header<BLOCK_ID::HW_TYPE>::HWINFO));
                    }
                    break;

                    case BLOCK_ID::CUSTOM: {
                        const auto &bh = consume_as<blk_header<BLOCK_ID::CUSTOM>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                        skip(bh.length * sizeof(uint8_t));
                    }
                    break;

                    case BLOCK_ID::GLUE: {
                        consume_as<blk_header<BLOCK_ID::GLUE>>(tzx_bytes, itr);
                        block_info.data_begin = itr - tzx_bytes.begin();
                    }
                    break;

                    default: {
                        if (revision(header) > 1'010) { // 1.10
                            skip(consume_as<uint32_t>(tzx_bytes, itr));
                            continue;
                        } else {
                            throw std::out_of_range(std::format("Unknown block id #{:X}", int(block_info.id)));
                        }
                    }
                }
            } catch (const std::out_of_range &e) {
                if constexpr (strict_mode) {
                    throw;
                } else {
                    auto message = std::format("{}\nThe program load may not complete correctly.", e.what());
                    event::dispatch(event::data<EVENT::SYS_SHOW_MESSAGE> {
                        .type = MSG_KIND::WARN,
                        .message = message
                    });
                    continue;
                }
            }

            block_info.data_size = itr - (tzx_bytes.begin() + block_info.data_begin);
            blocks.push_back(block_info);
        }

        ev_til_blocks_t ev_til_blocks;

        auto til_block = [&ev_til_blocks] INLINE
        (const std::string & type, const std::string & size, const std::string & desc) noexcept {
            ev_til_blocks.emplace_back(ev_til_block_t{
                .type = type,
                .size = size.empty() ? "-" : size,
                .desc = desc.empty() ? "-" : desc,
            });
        };

        auto compile = [this, _bc = blocks.size(), &til_block] INLINE
        (auto bi, size_t ix, void *header, std::span<uint8_t> &&span) -> generator_t {

            auto pause = [this] INLINE
            (int ms) noexcept -> generator_t {
                using namespace zx::rate::literals;
                return silence(ms * 1_zxms);
            };

            switch (bi.id) {
                case BLOCK_ID::STD_DATA: {
                    const auto &bh = *static_cast<blk_header<BLOCK_ID::STD_DATA>*>(header);
                    bool long_pilot = span[0] < 128;

                    til_block("Std. speed block",
                              std::format("#{:04X}", span.size()),
                              std::format("{}, pilot: {}, pause after: {} ms", description(span), long_pilot ? "long" : "short", bh.pause_after));

                    return splice(meander(std_pilot_pulse_tck, std_pilot_pulse_tck, long_pilot ? std_long_pilot_pulses : std_short_pilot_pulses),
                                  meander(std_sync_p1_tck, std_sync_p2_tck, 2),
                                  bytes_pulses(std_pulse_bit_0_tck, std_pulse_bit_1_tck, std::move(span)),
                                  pause(bh.pause_after));
                }

                case BLOCK_ID::TURBO_DATA: {
                    const auto &bh = *static_cast<blk_header<BLOCK_ID::TURBO_DATA>*>(header);

                    til_block("Turbo speed block",
                              std::format("#{:04X}", span.size()),
                              std::format("{}, pilot: {} pulses, pause after: {} ms, lbb: {}", description(span), bh.pilot_pulses, bh.pause_after, bh.last_byte_bits));

                    if (bh.last_byte_bits == 8) {
                        return splice(meander(bh.pilot_pulse_tck, bh.pilot_pulse_tck, bh.pilot_pulses),
                                      meander(bh.sync_p1_tck, bh.sync_p2_tck, 2),
                                      bytes_pulses(bh.zero_pulse_tck, bh.one_pulse_tck, std::move(span)),
                                      pause(bh.pause_after));
                    }

                    return splice(meander(bh.pilot_pulse_tck, bh.pilot_pulse_tck, bh.pilot_pulses),
                                  meander(bh.sync_p1_tck, bh.sync_p2_tck, 2),
                                  bytes_pulses(bh.zero_pulse_tck, bh.one_pulse_tck, span.first(span.size() - 1)),
                                  bits_pulses(bh.zero_pulse_tck, bh.one_pulse_tck, span.back(), bh.last_byte_bits),
                                  pause(bh.pause_after));
                }

                case BLOCK_ID::PURE_TONE: {
                    const auto &bh = *static_cast<blk_header<BLOCK_ID::PURE_TONE>*>(header);
                    til_block("Meander",
                              "",
                              std::format("{} pulse(s) ({} T-states per pulse)", bh.pulses, bh.pulse_tck));
                    return meander(bh.pulse_tck, bh.pulse_tck, bh.pulses);
                }

                case BLOCK_ID::PULSE_SEQ: {
                    const auto &bh = *static_cast<blk_header<BLOCK_ID::PULSE_SEQ>*>(header);
                    til_block("Pulse sequence",
                              "",
                              std::format("{} pulse(s)", bh.length));
                    return pulses(std::span<uint16_t>(reinterpret_cast<uint16_t*>(&span.front()), bh.length));
                }

                case BLOCK_ID::PURE_DATA: {
                    const auto &bh = *static_cast<blk_header<BLOCK_ID::PURE_DATA>*>(header);

                    til_block("Pure data block",
                              std::format("#{:04X}", span.size()),
                              std::format("{}, pause after: {} ms, lbb: {}", description(span), bh.pause_after, bh.last_byte_bits));

                    if (bh.last_byte_bits == 8) {
                        return splice(bytes_pulses(bh.zero_pulse_tck, bh.one_pulse_tck, std::move(span)),
                                      pause(bh.pause_after));

                    }

                    return splice(bytes_pulses(bh.zero_pulse_tck, bh.one_pulse_tck, span.first(span.size() - 1)),
                                  bits_pulses(bh.zero_pulse_tck, bh.one_pulse_tck, span.back(), bh.last_byte_bits),
                                  pause(bh.pause_after));
                }

                case BLOCK_ID::DIRECT_REC: {
                    const auto &bh = *static_cast<blk_header<BLOCK_ID::DIRECT_REC>*>(header);

                    struct pulse_t {
                        uint16_t ticks;
                        bool out;
                    };
                    std::list<pulse_t> record;
                    pulse_t *p = nullptr;

                    for (size_t bits_left = (span.size() - 1) * 8 + bh.last_byte_bits; uint8_t byte : span) {
                        for (uint8_t mask = 0x80_u8; mask && bits_left; mask >>= 1, --bits_left) {
                            if (bool out = !!(byte & mask); p && (p->out == out) && (p->ticks <= std::numeric_limits<uint16_t>::max() - bh.ticks_per_sample)) {
                                p->ticks += bh.ticks_per_sample;
                            } else {
                                p = &record.emplace_back(pulse_t{.ticks = bh.ticks_per_sample, .out = out});
                            }
                        }

                        if (!bits_left) {
                            break;
                        }
                    }

                    auto data = std::make_shared<std::vector<pulse_t>>(record.begin(), record.end());
                    auto pulses = [_data = data, _ix = 0u, _ticks = 0u]
                    (bool, int ticks) mutable noexcept -> result_t {
                        const auto p_ticks = (*_data)[_ix].ticks;

                        if ((_ticks += ticks) <= p_ticks) {
                            return (*_data)[_ix].out;
                        }

                        _ticks -= p_ticks;

                        if (++_ix >= _data->size()) {
                            return std::unexpected(_ticks);
                        }

                        return (*_data)[_ix].out;
                    };

                    til_block("Direct record",
                              std::format("#{:04X}", span.size()),
                              std::format("Pulses: {}, pause after: {} ms", record.size(), bh.pause_after));
                    return splice(pulses, pause(bh.pause_after));
                }

                case BLOCK_ID::CSW: {
                    const auto &bh = *static_cast<blk_header<BLOCK_ID::CSW>*>(header);
                    event::dispatch(event::data<EVENT::SYS_SHOW_MESSAGE> {
                        .type = MSG_KIND::WARN,
                        .message = "CSW block (ID 0x18) skipped — format is virtually unused.\nProgram may fail to load if this block contains essential data."
                    });

                    til_block("CSW block",
                              "",
                              std::format("Ignored, pause after: {} ms", bh.pause_after));
                    return pause(bh.pause_after);
                }

                case BLOCK_ID::GENERALIZED_DATA: {
                    const auto &bh = *static_cast<blk_header<BLOCK_ID::GENERALIZED_DATA>*>(header);
                    auto it = span.begin();

                    auto parse_alphabet = [&it, &span] INLINE (size_t asx, int max_np) {
                        std::unordered_map<uint8_t, std::tuple<uint8_t, std::list<uint16_t>>> alphabet{};

                        for (uint8_t psym = 0; asx; --asx, ++psym) {
                            auto jt = it;
                            std::list<uint16_t> pulses;
                            uint8_t mode = consume_as<uint8_t>(span, jt) & 0x03_u8;

                            for (int n = max_np; n; --n) {
                                if (uint16_t ticks = consume_as<uint16_t>(span, jt); ticks) {
                                    pulses.emplace_back(ticks);
                                } else {
                                    break;
                                }
                            }

                            alphabet[psym] = {mode, pulses};

                            if (it += sizeof(uint8_t) + sizeof(uint16_t) * max_np; it > span.end()) {
                                throw std::out_of_range("Unexpected end of data");
                            }
                        }

                        return alphabet;
                    };

                    using front_former_t = bool (*) (bool) noexcept;

                    static const std::array<front_former_t, 4> ffs = {
                        [] (bool s) noexcept {return !s;},
                        [] (bool s) noexcept {return s;},
                        [] (bool) noexcept {return no_signal_;},
                        [] (bool) noexcept {return signal_;}
                    };

                    struct pulse_t {
                        front_former_t ff;
                        uint16_t ticks;
                    };

                    std::list<pulse_t> record{};

                    if (bh.totp > 0) {
                        size_t asp = bh.asp ? bh.asp : 256;
                        auto alphabet = parse_alphabet(asp, bh.npp);

                        for (int n = bh.totp; n; --n) {
                            uint8_t sym = consume_as<uint8_t>(span, it);
                            uint16_t count = consume_as<uint16_t>(span, it);
                            auto &[mode, pulses] = alphabet[sym];

                            while (count--) {
                                auto p = pulses.begin();

                                for (record.emplace_back(pulse_t{.ff = ffs[mode], .ticks = *p++}); p != pulses.end(); ++p) {
                                    record.emplace_back(pulse_t{.ff = ffs[0], .ticks = *p});
                                }
                            }
                        }
                    }

                    if (bh.totd > 0) {
                        size_t asd = bh.asd ? bh.asd : 256;
                        auto alphabet = parse_alphabet(asd, bh.npd);
                        int bits_per_sym = std::ceil(std::log2(asd));
                        uint8_t mask = 0x80;

                        for (int n = bh.totd; n; --n) {
                            uint8_t sym {0};

                            for (int bits = bits_per_sym; bits; --bits) {
                                sym <<= 1;
                                sym |= (*it & mask) != 0;

                                if ((mask >>= 1) == 0) {
                                    mask = 0x80;

                                    if (++it; it > span.end()) {
                                        throw std::out_of_range("Unexpected end of data");
                                    }
                                }
                            }

                            auto &[mode, pulses] = alphabet[sym];
                            auto p = pulses.begin();

                            for (record.emplace_back(pulse_t{.ff = ffs[mode], .ticks = *p++}); p != pulses.end(); ++p) {
                                record.emplace_back(pulse_t{.ff = ffs[0], .ticks = *p});
                            }
                        }
                    }

                    auto data = std::make_shared<std::vector<pulse_t>>();
                    data->reserve(record.size() + 1);
                    data->emplace_back(pulse_t{.ff = ffs[0], .ticks = 0});
                    data->insert(data->end(), record.begin(), record.end());

                    auto pulses = [_data = data, _ix = 0u, _ticks = 0u]
                    (bool curr_out, int ticks) mutable noexcept -> result_t{
                        const auto p_ticks = (*_data)[_ix].ticks;

                        if ((_ticks += ticks) <= p_ticks) {
                            return curr_out;
                        }

                        _ticks -= p_ticks;

                        if (++_ix >= _data->size()) {
                            return std::unexpected(_ticks);
                        }

                        return (*_data)[_ix].ff(curr_out);
                    };

                    til_block("Generalized data",
                              "",
                              std::format("Pulses: {}, pause after: {} ms", record.size(), bh.pause_after));
                    return splice(pulses, pause(bh.pause_after));
                }

                case BLOCK_ID::SILENCE: {
                    const auto &bh = *static_cast<blk_header<BLOCK_ID::SILENCE>*>(header);
                    til_block("Pause (silence)",
                              "",
                              std::format("Pause: {} ms{}", bh.duration_ms, bh.duration_ms ? "" : " (stop tape)"));
                    return bh.duration_ms > 0 ? pause(bh.duration_ms) : inverting_stopper();
                }

                case BLOCK_ID::JUMP: {
                    const auto &bh = *static_cast<blk_header<BLOCK_ID::JUMP>*>(header);

                    if (auto jx = int(ix) + bh.offset; 0 <= jx && jx < int(_bc) && jx != int(ix)) {
                        til_block("Jump",
                                  "",
                                  std::format("Jump to block {}", jx + 1));
                        return jump(jx);
                    } else {
                        throw std::out_of_range(std::format("Unexpected JUMP to block {} (allowed 0 - {} except {})", jx, _bc - 1, ix));
                    }
                }

                case BLOCK_ID::LOOP_START: {
                    const auto &bh = *static_cast<blk_header<BLOCK_ID::LOOP_START>*>(header);
                    til_block("Loop start",
                              "",
                              std::format("Repetitions: {}", bh.repetitions));
                    return loop(bh.repetitions);
                }

                case BLOCK_ID::LOOP_END: {
                    til_block("Loop end",
                              "",
                              "");
                    return next();
                }

                case BLOCK_ID::CALL_SEQ: {
                    const auto &bh = *static_cast<blk_header<BLOCK_ID::CALL_SEQ>*>(header);
                    std::span<int16_t> offsets(reinterpret_cast<int16_t *>(&span.front()), bh.call_num);
                    std::vector<size_t> call_indices;

                    for (call_indices.reserve(offsets.size()); auto offs : offsets) {
                        if (auto jx = int(ix) + offs; 0 <= jx && jx < int(_bc) && jx != int(ix)) {
                            call_indices.emplace_back(jx);
                        } else {
                            throw std::out_of_range(std::format("Unexpected CALL to block {} (allowed 0 - {} except {})", jx, _bc - 1, ix));
                        }
                    }

                    til_block("Call",
                              "",
                              std::format("Call to {} entry point(s)", call_indices.size()));
                    return call(std::move(call_indices));
                }

                case BLOCK_ID::RETURN: {
                    til_block("Return",
                              "",
                              "");
                    return retn();
                }

                case BLOCK_ID::SELECT: {
                    const auto &bh = *static_cast<blk_header<BLOCK_ID::SELECT>*>(header);

                    til_block("Select",
                              "",
                              std::format("Options: {}", bh.opt_count));

                    if (bh.opt_count > 0) {
                        std::vector<size_t> jumps;
                        std::vector<std::string_view> items;
                        jumps.reserve(bh.opt_count);
                        items.reserve(bh.opt_count);
                        auto i = span.begin();

                        for (auto ctr = bh.opt_count; ctr; --ctr) {
                            int16_t offs = consume_as<uint16_t>(span, i);
                            uint8_t length = consume_as<uint8_t>(span, i);
                            std::string_view item(reinterpret_cast<char*>(&*i), length);

                            if (i += length; i > span.end()) {
                                throw std::out_of_range("Unexpected end of data");
                            }

                            if (auto jx = int(ix) + offs; jx < 0 || jx >= int(_bc) || jx == int(ix)) {
                                throw std::out_of_range(std::format("Unexpected JUMP to block {} (allowed 0 - {} except {})", jx, _bc - 1, ix));
                            } else {
                                jumps.emplace_back(jx);
                                items.emplace_back(item);
                            }
                        }

                        return [this, _items = std::move(items), _jumps = std::move(jumps)]
                        (bool, int ticks) noexcept {
                            auto ed = event::data<EVENT::SYS_SELECT_OPTION> {
                                .title = "TZX Select",
                                .hint = "Select, please:",
                                .items = _items,
                                .choice = -1
                            };

                            if (event::dispatch(ed); 0 <= ed.choice && ed.choice < int(_jumps.size())) {
                                goto_(_jumps[ed.choice]);
                            }

                            return std::unexpected(ticks);
                        };
                    }

                    return stub();
                }

                case BLOCK_ID::STOP_48K: {
                    til_block("Stop 48K",
                              "",
                              "Stop tape for ZX Spectrum 48K");
                    return inverting_stopper(); // TODO 128K!
                }

                case BLOCK_ID::SET_LEVEL: {
                    const auto &bh = *static_cast<blk_header<BLOCK_ID::SET_LEVEL>*>(header);
                    til_block("Level",
                              "",
                              std::format("Set level to {}", bh.level ? "high" : "low"));
                    return level(bh.level ? signal_ : no_signal_);
                }

                case BLOCK_ID::MESSAGE: {
                    const auto &bh = *static_cast<blk_header<BLOCK_ID::MESSAGE>*>(header);
                    std::string_view msg(reinterpret_cast<char*>(&span.front()), span.size());

                    til_block("Message",
                              "",
                              std::format("Message: \"{}\", duration: {}", msg, bh.show_time));

                    if (msg.size()) {
                        if (!bh.show_time) {
                            return [msg]
                            (bool, int ticks) noexcept {
                                event::dispatch(event::data<EVENT::SYS_SHOW_MESSAGE> {
                                    .type = MSG_KIND::INFO,
                                    .message = msg
                                });
                                return std::unexpected(ticks);
                            };
                        }

                        return [msg, show_time = bh.show_time]
                        (bool, int ticks) noexcept {
                            event::dispatch(event::data<EVENT::SYS_SHOW_STATUS> {
                                .text = msg,
                                .show_time = show_time
                            });
                            return std::unexpected(ticks);
                        };
                    }

                    return stub();
                }

                case BLOCK_ID::GROUP_START: {
                    til_block("Group start",
                              "",
                              std::format("Group name: \"{}\"", std::string(reinterpret_cast<const char*>(&span[0]), span.size())));
                    return stub();
                }

                case BLOCK_ID::GROUP_END: {
                    til_block("Group end",
                              "",
                              "");
                    return stub();
                }

                case BLOCK_ID::DESCRIPTION: {
                    til_block("Description",
                              "",
                              std::format("\"{}\"", std::string(reinterpret_cast<const char*>(&span[0]), span.size())));
                    return stub();
                }

                case BLOCK_ID::ARCHIVE_INFO: {
                    til_block("Archive info",
                              "",
                              "Ignored");
                    return stub();
                }

                case BLOCK_ID::HW_TYPE: {
                    til_block("Hardware type",
                              "",
                              "Ignored");
                    return stub();
                }

                case BLOCK_ID::CUSTOM: {
                    til_block("Custom data",
                              "",
                              "Ignored");
                    return stub();
                }

                case BLOCK_ID::GLUE: {
                    til_block("Glue block",
                              "",
                              std::format("Rev. major: {}, Rev. minor: {}", static_cast<int>(span[7]), static_cast<int>(span[8])));
                    return stub();
                }

                default: {
                    til_block("Unknown block",
                              "",
                              "Ignored");
                    return stub();
                }
            }
        };

        reset_<true>();

        _tape_bytes = std::move(tzx_bytes);
        _generators.reserve(blocks.size());

        for (size_t ix = 0; auto &bi : blocks) {
            _generators.push_back(compile(bi,
                                          ix++,
                                          &_tape_bytes[bi.header],
                                          std::span(&_tape_bytes[bi.data_begin], bi.data_size)));
        }

        _generators.emplace_back(inverting_stopper());
        til_block("End of tape", "", "Stop playing");
        event::dispatch(event::data<EVENT::TAPE_IMAGE_LOADED> {.blocks = std::move(ev_til_blocks)});

        reset_<false>();
        return _generators.size();
    } catch (const std::out_of_range &e) {
        return std::unexpected(e.what());
    }
}
