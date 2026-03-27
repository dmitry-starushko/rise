#pragma once
#ifndef ZXTAPEACTIVATOR_H
#define ZXTAPEACTIVATOR_H

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
#include <optional>

class ZxTapeActivator final: public event::handler<ZxTapeActivator, EVENT::Z80_IO_IN, EVENT::ULA_VBLANK_START, EVENT::TAPE_SET_APDS_MODE> {
    constexpr static unsigned ftf_init_value_0_ = 0.25 * zx::rate::frame_rate;
    constexpr static unsigned ftf_init_value_1_ = 1.2 * zx::rate::frame_rate;
    constexpr static unsigned rlf_treshold_ = 256;

  public:

    ZxTapeActivator() = default;
    ~ZxTapeActivator() = default;

    INLINE
    void assumed_state(std::optional<bool> state) noexcept {
        if ((_assumed_state = state)) {
            _frames_to_fire = *_assumed_state ? ftf_init_value_1_ : ftf_init_value_0_;
        }
    }

  public:

    INLINE
    void handle(event::data<EVENT::TAPE_SET_APDS_MODE> &ed) noexcept {
        ed.mode_name = _mode_names.at(_mode = ed.mode);
    }

    INLINE
    void handle(const event::data<EVENT::Z80_IO_IN> &ed) noexcept {
        using namespace zx::tape;

        if (tape_port(ed.port)) {
            _ubc.feed(_hi(ed.port));
            ++_reads_last_frame;
        }
    }

    INLINE
    void handle(event::data<EVENT::ULA_VBLANK_START> &) noexcept {
        if (_frames_to_fire && _assumed_state) {
            if (bool state = _ubc.uniform &&
                             (_reads_last_frame > rlf_treshold_) &&
                             (_mode != APDS_MODE::M_STRICT || (_ubc.first_byte & 0x7F) == 0x7F); state != *_assumed_state) {
                if (!--_frames_to_fire && _mode != APDS_MODE::M_OFF) {
                    if (state) {
                        event::dispatch(event::data<EVENT::TAPE_PLAY_ON> {});
                    } else {
                        event::dispatch(event::data<EVENT::TAPE_PLAY_OFF> {});
                    }
                }
            } else {
                _frames_to_fire = *_assumed_state ? ftf_init_value_1_ : ftf_init_value_0_;
            }
        }

        _ubc.reset();
        _reads_last_frame = 0;
    }

  private:

    struct uniform_byte_checker {
        bool uniform;
        int first_byte;

        INLINE
        void reset() noexcept {
            uniform = true;
            first_byte = -1;
        }

        INLINE
        void feed(uint8_t u8) noexcept {
            if (uniform) {
                if (first_byte >= 0) [[likely]] {
                    uniform = first_byte == u8;
                } else {
                    first_byte = u8;
                }
            }
        }

        uniform_byte_checker() {
            reset();
        }
    };

  private:

    unsigned _frames_to_fire {0};
    unsigned _reads_last_frame {0};
    uniform_byte_checker _ubc;
    std::optional<bool> _assumed_state {};
    APDS_MODE _mode = APDS_MODE::M_OFF;

    inline static const std::flat_map<APDS_MODE, std::string> _mode_names {
        {APDS_MODE::M_OFF, "off"},
        {APDS_MODE::M_LENIENT, "lenient"},
        {APDS_MODE::M_STRICT, "strict"}
    };
};

#endif // ZXTAPEACTIVATOR_H
