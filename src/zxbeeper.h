#pragma once
#ifndef ZXBEEPER_H
#define ZXBEEPER_H

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
#include "df.h"

class ZxBeeper final: public event::handler<ZxBeeper, EVENT::Z80_IO_OUT, EVENT::Z80_STEP> {
    static constexpr int ticks_per_sample_ = 10;
    static constexpr int decimation_factor_ = 10;
    static constexpr int inner_sample_rate_ = zx::rate::z80_clock_rate / ticks_per_sample_;
    static constexpr int sample_rate_ = inner_sample_rate_ / decimation_factor_;
    static constexpr float sig_min_ = 0.0;
    static constexpr float sig_max_ = 1.0;

    static constexpr unsigned filter_range_ = 10;
    static constexpr double freq_min_ = 50.0;
    static constexpr double freq_max_ = 13'000.0;
    static constexpr double epsilon_ = 0.10;
    static constexpr size_t buff_size_ = 3 * sample_rate_ / zx::rate::frame_rate;
    static_assert(freq_min_ < freq_max_ && freq_max_ < 0.5 * sample_rate_);

    using filter_t = df::filter<filter_range_, df::band::band_pass, df::tfa::chebyshev_i>;
    using ring_buffer_t = std::array<float, buff_size_>;

  public:

    explicit ZxBeeper() noexcept = default;
    ~ZxBeeper() noexcept = default;

    INLINE
    static constexpr int sample_rate() noexcept {
        return sample_rate_;
    }

    INLINE
    static constexpr int channels() noexcept {
        return 1;
    }

    INLINE
    void reset() {
        _begin = _end = _sample_count = 0z;
    }

    INLINE
    size_t fill_buffer(uint8_t *buffer, size_t len) noexcept {
        const size_t transfer_samples = std::min(_sample_count, len / sizeof(float));
        auto buf = static_cast<float *>(static_cast<void *>(buffer));

        for (size_t c = transfer_samples; c; --c) {
            *buf++ = _ring_buff[_begin++];
            _begin %= _ring_buff.size();
        }

        return _sample_count -= transfer_samples, transfer_samples * sizeof(float);
    }

    INLINE
    void handle(const event::data<EVENT::Z80_STEP> &ed) noexcept {
        render(ed.ticks);
    }

    INLINE
    void handle(const event::data<EVENT::Z80_IO_OUT> &ed) noexcept {
        using namespace zx::beeper;

        if (beeper_port(ed.port)) {
            _signal = (ed.byte & (0x01_u8 << 4)) ? sig_max_ : sig_min_;
        }
    }

  private:

    INLINE
    void render(int ticks) noexcept {
        auto qr = std::div(_ticks + ticks, ticks_per_sample_);
        _ticks = qr.rem;

        for (auto i = qr.quot; i; --i, ++_decimation_counter, _decimation_counter %= decimation_factor_) {
            if (!_decimation_counter) {
                _ring_buff[_end++] = std::clamp(static_cast<float>(_filter(_signal)), -1.0f, 1.0f);
                _end %= _ring_buff.size();
                ++_sample_count;
            } else {
                _filter(_signal);
            }
        }
    }

  private:

    int _ticks {0};
    int _decimation_counter {0};
    float _signal {sig_min_};
    filter_t _filter {inner_sample_rate_, freq_min_, freq_max_, epsilon_};
    ring_buffer_t _ring_buff { };
    ring_buffer_t::size_type _begin {0z}, _end {0z}, _sample_count {0z};
};

#endif // ZXBEEPER_H
