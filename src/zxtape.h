#pragma once
#ifndef ZXTAPE_H
#define ZXTAPE_H

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
#include "zxevents.h"
#include "zxtapeactivator.h"
#include <functional>
#include <span>
#include <cstdint>
#include <expected>
#include <vector>
#include <string>
#include <format>
#include <stack>
#include <memory>
#include <boost/algorithm/string/trim.hpp>

class ZxTape final:
    public event::handler<ZxTape,
    EVENT::Z80_STEP,
    EVENT::Z80_IO_IN,
    EVENT::TAPE_PLAY_ON,
    EVENT::TAPE_PLAY_OFF,
    EVENT::TAPE_REWIND,
    EVENT::TAPE_FAST_FORWARD,
    EVENT::TAPE_REWIND_TO_START,
    EVENT::TAPE_RESET,
    EVENT::TAPE_IMAGE_LOADED> {
    constexpr static bool signal_ = true;
    constexpr static bool no_signal_ = !signal_;
    constexpr static int std_short_pilot_pulses = 3223;
    constexpr static int std_long_pilot_pulses = 8063;
    constexpr static int std_pilot_pulse_tck = 2168;
    constexpr static int std_sync_p1_tck = 667;
    constexpr static int std_sync_p2_tck = 735;
    constexpr static int std_pulse_bit_0_tck = 855;
    constexpr static int std_pulse_bit_1_tck = 1710;
    constexpr static int std_short_pause_tck = zx::rate::z80_clock_rate;
    constexpr static int std_long_pause_tck = zx::rate::z80_clock_rate * 5 / 2;
    constexpr static size_t max_stack_depth = 16;

  public:

    ZxTape() noexcept = default;
    ~ZxTape() noexcept = default;

    [[nodiscard]]
    std::expected<int, std::string> open_tap(std::vector<uint8_t> &&tap_bytes) noexcept;
    [[nodiscard]]
    std::expected<int, std::string> open_tzx(std::vector<uint8_t> &&tzx_bytes) noexcept;

  public:

    INLINE
    void handle(const event::data<EVENT::Z80_STEP> &ed) noexcept {
        render(ed.ticks);
    }

    INLINE
    void handle(event::data<EVENT::Z80_IO_IN> &ed) noexcept {
        using namespace zx::tape;

        if (tape_port(ed.port)) {
            ed.byte |= static_cast<uint8_t>(_out) << 6;
        }
    }

    INLINE
    void handle(const event::data<EVENT::TAPE_PLAY_ON> &) noexcept {
        power_(true);
    }

    INLINE
    void handle(const event::data<EVENT::TAPE_PLAY_OFF> &) noexcept {
        power_(false);
    }

    INLINE
    void handle(const event::data<EVENT::TAPE_REWIND> &) noexcept {
        if (!_supply) {
            cursor_(cursor_() - 1z);
        }
    }

    INLINE
    void handle(const event::data<EVENT::TAPE_FAST_FORWARD> &) noexcept {
        if (!_supply) {
            cursor_(cursor_() + 1z);
        }
    }

    INLINE
    void handle(const event::data<EVENT::TAPE_REWIND_TO_START> &) noexcept {
        if (!_supply) {
            reset_<false>();
        }
    }

    INLINE
    void handle(const event::data<EVENT::TAPE_RESET> &) noexcept {
        if (!_supply) {
            reset_<true>();
        }
    }

    INLINE
    void handle(const event::data<EVENT::TAPE_IMAGE_LOADED> &ed) noexcept {
        _activator.assumed_state(ed.blocks.empty() ? std::nullopt : std::optional<bool>(false));
    }

  private:

    INLINE
    void render(int ticks) noexcept {
        _out = _supply ? render_(ticks) : no_signal_;
    }

    INLINE
    void power_(bool supply) {
        if (_supply != supply) {
            if (_supply = supply; _supply) {
                _generator = nullptr;
                goto_(cursor_());
                event::dispatch(event::data<EVENT::TAPE_STARTED> {});
            } else {
                event::dispatch(event::data<EVENT::TAPE_STOPPED> {.has_blocks = _generators.size() > 0});
            }

            _activator.assumed_state(supply);
        }
    }

    INLINE
    size_t cursor_(size_t index) noexcept {
        index = !_generators.empty() ? static_cast<size_t>(std::clamp(static_cast<signed long>(index), 0l, static_cast<signed long>(_generators.size()) - 1l)) : 0z;
        event::dispatch(event::data<EVENT::TAPE_CURSOR_MOVED> {.block_index = _cursor = index});
        return _cursor;
    }

    INLINE
    size_t cursor_() noexcept {
        return _cursor;
    }

    INLINE
    void break_rendering_(std::string_view msg) noexcept {
        event::dispatch(event::data<EVENT::SYS_SHOW_MESSAGE> {
            .type = MSG_KIND::ERROR,
            .message = msg
        });
        power_(false);
    }

    INLINE
    void goto_(size_t index) noexcept {
        _next_index = index;
    }

    INLINE
    bool render_(int ticks) noexcept {
        while (true) {
            if (_generator) {
                if (auto result = _generator(_out, ticks); result) {
                    return *result;
                } else {
                    _generator = nullptr;
                    ticks = result.error();
                }
            } else if (_next_index < _generators.size()) {
                _generator = _generators[cursor_(_next_index++)];
            } else {
                return _out;
            }
        }
    }

    template<bool hard_reset>
    INLINE
    void reset_() noexcept {
        if constexpr (hard_reset) {
            _generators.clear();
            _tape_bytes.clear();
            event::dispatch(event::data<EVENT::TAPE_IMAGE_LOADED> {.blocks = {}});
        }

        _generator = nullptr;
        _call_stack = {};
        _loop_stack = {};
        _out = no_signal_;
        power_(false);
        goto_(cursor_(0z));
    }

    // --- Generators

    using result_t = std::expected<bool, int>;
    using generator_t = std::function<result_t(bool, int)>;

    INLINE
    inline static auto stub() noexcept {
        return []
        (bool, int ticks) mutable noexcept -> result_t {
            return std::unexpected(ticks);
        };
    }

    INLINE
    auto inverter() noexcept {
        return [this]
        (bool, int ticks) noexcept -> result_t {
            _out = !_out;
            return std::unexpected(ticks);
        };
    }

    INLINE
    auto level(bool val) noexcept {
        return [this, val]
        (bool, int ticks) noexcept -> result_t {
            _out = val;
            return std::unexpected(ticks);
        };
    }

    INLINE
    generator_t silence(int duration_tck) noexcept {
        if (duration_tck > 0) {
            return [this, _ticks = 0, _duration = duration_tck ]
            (bool curr_out, int ticks) mutable noexcept -> result_t {
                if ((_ticks += ticks) == ticks) {
                    _activator.assumed_state({});
                    return !curr_out;
                }

                if (_ticks < _duration) {
                    return curr_out;
                }

                _activator.assumed_state(true);
                return std::unexpected(_ticks - _duration);
            };
        }

        return stub();
    }

    INLINE
    inline static auto meander(int front_pulse_tck, int back_pulse_tck, int pulse_count) noexcept {
        const int period = front_pulse_tck + back_pulse_tck;
        const int duration = period * (pulse_count / 2) + front_pulse_tck * (pulse_count % 2);

        return [ _ticks = 0,
                 _duration = duration,
                 _period = period,
                 _last_pulse = true,
                 front_pulse_tck ]
        (bool curr_out, int ticks) mutable noexcept -> result_t {
            if ((_ticks += ticks) < _duration) {
                bool actual_pulse = _ticks % _period > front_pulse_tck;
                bool keep = _last_pulse == actual_pulse;
                return _last_pulse = actual_pulse, keep ? curr_out : !curr_out;
            }

            return std::unexpected(_ticks - _duration);
        };
    }

    INLINE
    inline static auto pulses(const std::span<uint16_t> &&pulses_ticks) noexcept {
        auto ps_ticks = std::make_shared<std::vector<uint16_t>>();
        ps_ticks->reserve(pulses_ticks.size() + 1);
        ps_ticks->emplace_back(uint16_t{0});
        ps_ticks->insert(ps_ticks->end(), pulses_ticks.begin(), pulses_ticks.end());

        return [_ps_ticks = ps_ticks, _ix = 0u, _ticks = 0u]
        (bool curr_out, int ticks) mutable noexcept -> result_t {
            auto p_ticks = (*_ps_ticks)[_ix];

            if ((_ticks += ticks) <= p_ticks) {
                return curr_out;
            }

            _ticks -= p_ticks;

            if (++_ix >= _ps_ticks->size()) {
                return std::unexpected(_ticks);
            }

            return !curr_out;
        };
    }

    INLINE
    inline static auto bytes_pulses(uint16_t zero_pulse_tck, uint16_t one_pulse_tck, std::span<uint8_t> &&span) noexcept {
        std::vector<std::uint16_t> ps_ticks;
        ps_ticks.reserve(span.size() * 8 * 2);

        for (const uint8_t &byte : span) {
            for (uint8_t mask = 0x80_u8; mask; mask >>= 1) {
                auto p_ticks = (byte & mask) ? one_pulse_tck : zero_pulse_tck;
                ps_ticks.emplace_back(p_ticks);
                ps_ticks.emplace_back(p_ticks);
            }
        }

        return pulses(ps_ticks);
    }

    INLINE
    inline static auto bits_pulses(uint16_t zero_pulse_tck, uint16_t one_pulse_tck, uint8_t byte, uint8_t bits) noexcept {
        std::vector<uint16_t> ps_ticks;
        ps_ticks.reserve((bits = std::clamp(bits, 1_u8, 7_u8)) * 2);

        for (uint8_t mask = 0x80; bits; --bits, mask >>= 1) {
            auto p_ticks = (byte & mask) ? one_pulse_tck : zero_pulse_tck;
            ps_ticks.emplace_back(p_ticks);
            ps_ticks.emplace_back(p_ticks);
        }

        return pulses(ps_ticks);
    }

    INLINE
    inline static auto splice(auto ... segment) noexcept {
        std::array<generator_t, sizeof...(segment)> segments { segment... };
        return [_segments = segments, _segment_index = 0u]
        (bool curr_out, int ticks) mutable noexcept -> result_t {
            while (_segment_index < _segments.size()) {
                if (auto result = _segments[_segment_index](curr_out, ticks); result) {
                    return *result;
                } else {
                    ticks = result.error();
                    ++_segment_index;
                }
            }

            return std::unexpected(ticks);
        };
    }

    INLINE
    auto stopper() noexcept {
        return [this]
        (bool, int ticks) noexcept -> result_t {
            power_(false);
            return std::unexpected(ticks);
        };
    }

    INLINE
    auto inverting_stopper() noexcept {
        using namespace zx::rate::literals;
        return splice(silence(10_zxms), stopper());
    }

    INLINE
    auto jump(size_t index) noexcept {
        return [this, index]
        (bool, int ticks) noexcept -> result_t {
            goto_(index);
            return std::unexpected(ticks);
        };
    }

    INLINE
    auto loop(unsigned count) noexcept {
        return [this, count]
        (bool, int ticks) noexcept -> result_t {
            if (_loop_stack.size() < max_stack_depth) {
                _loop_stack.push({count, _next_index});
            } else {
                break_rendering_("Tape playback broken: too many nested loops!");
            }

            return std::unexpected(ticks);
        };
    }

    INLINE
    auto next() noexcept {
        return [this]
        (bool, int ticks) noexcept -> result_t {
            if (!_loop_stack.empty()) {
                if (--_loop_stack.top().first) {
                    goto_(_loop_stack.top().second);
                } else {
                    _loop_stack.pop();
                }
            }

            return std::unexpected(ticks);
        };
    }

    INLINE
    auto call(std::vector<size_t> &&indices) noexcept {
        return [this, _indices = std::move(indices)]
        (bool, int ticks) noexcept -> result_t {
            if (!_indices.empty()) {
                if (_call_stack.size() < max_stack_depth) {
                    _call_stack.push(_next_index);

                    for (auto i = _indices.rbegin(); i != _indices.rend() - 1; ++i) {
                        _call_stack.push(*i);
                    }

                    goto_(_indices.front());
                } else {
                    break_rendering_("Tape playback broken: too many nested calls!");
                }
            }

            return std::unexpected(ticks);
        };
    }

    INLINE
    auto retn() noexcept {
        return [this]
        (bool, int ticks) noexcept -> result_t {
            if (!_call_stack.empty()) {
                goto_(_call_stack.top());
                _call_stack.pop();
            }

            return std::unexpected(ticks);
        };
    }

    // -- Helpers

    INLINE
    inline static std::string description(const std::span<uint8_t> &block) noexcept {
        if (block.empty()) {
            return "Unexpected: zero-length block!";
        }

        if (!block[0]) {
            if (block.size() >= 0x13 && block.size() <= 0x20) {
                auto name = boost::algorithm::trim_copy(std::string(reinterpret_cast<const char*>(&block[2]), 10));
                auto par1 = *reinterpret_cast<uint16_t *>(&block[14]);

                auto var_name = [h = _hi(par1)]->std::string {
                    switch (h >> 5) {
                        case 0x04:
                            return std::string(1, static_cast<char>(0x60_u8 + (h & 0x1F_u8)));

                        case 0x06:
                            return std::format("{}$", std::string(1, static_cast<char>(0x60_u8 + (h & 0x1F_u8))));

                        default:
                            return "?";
                    }
                };

                switch (block[1]) {
                    case 0:
                        return std::format("Header, program: \"{}\", start line: {}", name, par1);

                    case 1:
                        return std::format("Header, num. array: \"{}\", var: \"{}\"", name, var_name());

                    case 2:
                        return std::format("Header, sym. array: \"{}\", var: \"{}\"", name, var_name());

                    case 3:
                        return std::format("Header, code: \"{}\", origin: #{:04X}", name, par1);

                    default:
                        return std::format("Header, unknown type: {}", static_cast<int>(block[1]));
                }
            }

            return "Bytes";
        }

        return "Bytes";
    }

  private:

    using ev_til_blocks_t = std::decay_t<decltype(event::data<EVENT::TAPE_IMAGE_LOADED>::blocks)>;
    using ev_til_block_t = ev_til_blocks_t::value_type;

    size_t _next_index{};
    size_t _cursor{};
    generator_t _generator {nullptr};
    std::vector<uint8_t> _tape_bytes {};
    std::vector<generator_t> _generators {};
    std::stack<size_t> _call_stack{};
    std::stack<std::pair<unsigned, size_t>> _loop_stack{};
    bool _out {no_signal_};
    bool _supply {false};
    ZxTapeActivator _activator;
};

#endif // ZXTAPE_H
