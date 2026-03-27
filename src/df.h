#pragma once
#ifndef DF_H
#define DF_H

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

#include "inline.h"
#include <memory>
#include <list>
#include <queue>
#include <set>
#include <tuple>
#include <complex>
#include <concepts>
#include <cassert>
#include <generator>
#include <ranges>
#include <optional>
#include <variant>
#include <functional>
#include <algorithm>

namespace df {

/* -- This is truncated version of library special for RISE project -- */

enum class tfa {        // -- Transfer Function Approximation
    butterworth_t,      // -- Butterworth's, `tan` variant
    butterworth_s,      // -- Butterworth's, `sin` variant
    chebyshev_i,        // -- Чебышёва, т.1
    chebyshev_ii,       // -- Чебышёва, т 2
    __default__ = butterworth_t
};

enum class band {       // -- Band types
    low_pass,
    high_pass,
    band_pass,
    band_reject,
};

using complex = std::complex<double>;

namespace r {

template<typename R, typename ... T> using builder = std::function<std::generator<R>(T...)>;
using cp_source = builder<complex>;
using cp_transform = builder<complex, const complex &>;
using approx_result = std::tuple<std::list<complex>, std::list<complex>, std::list<complex>>;
using namespace std::complex_literals;

constexpr double Pi = std::numbers::pi;
constexpr double Pi_2 = 2.0 * std::numbers::pi;
constexpr double default_scale = 1e12;
constexpr double default_epsilon = 0.33;

template<unsigned u> concept positive = u > 0u;
template<unsigned u> concept even = !(u & 1u);
template<unsigned u> concept odd = !even<u>;

constexpr bool validate_apx(double rate, double f1, double f2) noexcept {
    return 0.0 < f1 && f1 < f2 && 2.0 * f2 < rate;
}

constexpr bool validate_tf(double rate, double f) noexcept {
    return 0.0 < rate && 0.0 <= f && 2.0 * f <= rate;
}

constexpr complex z(double rate, double f) noexcept {
    assert(validate_tf(rate, f));
    return std::exp(-1i * Pi_2 * f / rate);
}

constexpr std::tuple<complex, complex> solve(const complex &a, const complex &b, const complex &c) noexcept {
    const complex d = std::sqrt(b * b - 4.0 * a * c);
    const complex e = 0.5 / a;
    return {e *(d - b), -e *(d + b)};
}

template<unsigned n, unsigned from = 0, unsigned top = n> requires positive<n> && (from < top)
std::generator<complex> minus_one_roots() noexcept {
    const double alpha = Pi_2 / n;

    for (auto k = from; k < top; ++k) {
        co_yield std::polar(1.0, (k + 0.5) * alpha);
    }
}

template<unsigned n, unsigned from = 0, unsigned top = n> requires positive<n> && (from < top)   /* -- roots of equation epsilon^2 * T(n,x)^2 == -1 -- */
auto chebyshev_poles_i(double epsilon) noexcept {
    assert(epsilon != 0.0);
    const complex imag = 2.0i * asinh(1.0 / epsilon);

    return [imag]() noexcept->std::generator<complex> {
        for (auto k = from; k < top; ++k) {
            const complex r = cos(((4 * k + 1) * Pi - imag) / (2.0 * n));
            co_yield r;
            co_yield std::conj(r);
        }
    };
}

inline std::list<complex> build_poles(cp_source &&src, cp_transform &&tfm) noexcept {
    std::list<complex> poles;

    for (const complex &r : src()) {
        for (const complex &p : tfm(r)) {
            if (std::norm(p) <= 1.0) {
                poles.push_back(p);
            }
        }
    }

    return poles;
}

template<unsigned range> requires positive<range>
std::list<complex> build_zeroes(double a, double b) noexcept {
    std::list<complex> zeroes;

    for (unsigned k = range >> 1; k--;) {
        zeroes.push_back(a);
        zeroes.push_back(b);
    }

    if constexpr (odd<range>) {
        assert(a == b);
        zeroes.push_back(a);
    }

    return zeroes;
}

constexpr complex bilin_trans(const complex &s) noexcept {
    return (1.0 + s) / (1.0 - s);
}

template<typename T>
std::optional<T> drop (std::queue<T> &q) noexcept {
    using popper = std::unique_ptr < std::queue<T>, decltype([](std::queue<T> *q) noexcept {
        q->pop();
    }) >;

    if (q.empty()) {
        return {};
    } else {
        popper _(&q);
        return q.front();
    }
}

/* ------------------------------------------------------------------------------------------------
 * Transfer Function Approxers
 * ------------------------------------------------------------------------------------------------
 */

template<unsigned, band, tfa>
class tf_approxer {
  protected:
    static approx_result approx(auto ...) {
        static_assert(false, "The required approximation is not defined");
        return {};
    }
};

template<unsigned range> requires positive<range>
class tf_approxer<range, band::band_pass, tfa::chebyshev_i> {
  protected:
    static approx_result approx(double rate, double f1, double f2, double epsilon = default_epsilon) noexcept {
        assert(validate_apx(rate, f1, f2));

        const double w1 = tan(Pi * f1 / rate);
        const double w2 = tan(Pi * f2 / rate);
        const double aa = w1 * w2;
        const double bb = w2 - w1;
        const auto [o1, o2] = solve(1.0, bb * cos(Pi / (2.0 * range)), -aa);
        const double fm1 = (atan(o1) / Pi).real();
        const double fm2 = (atan(o2) / Pi).real();
        auto cpt = [&](const complex & r) noexcept -> std::generator<complex> {
            auto [p1, p2] = solve(1.0, bb * r, -aa);
            co_yield bilin_trans(p1 * 1.0i);
            co_yield bilin_trans(p2 * 1.0i);
        };

        return {build_zeroes<2u * range>(1.0, -1.0), build_poles(chebyshev_poles_i<range>(epsilon), cpt), {z(1.0, std::max(fm1, fm2))}};
    }
};

/* ------------------------------------------------------------------------------------------------
 * Realizations
 * ------------------------------------------------------------------------------------------------
 */

class abstract_realization {
  protected:
    abstract_realization() noexcept = default;
    virtual void reset(const approx_result &) noexcept = 0;
    virtual double operator()(double) noexcept = 0;
    virtual complex trans_func_(complex) const noexcept = 0;
};

template<typename T> concept realization = std::derived_from<T, abstract_realization>;

class cascaded:
    public abstract_realization {

  protected:
    void reset(const approx_result &ar) noexcept override {
        static auto zero_pair_to_polynomial = [](const conj_pair & p) noexcept->std::tuple<double, double> {
            if (std::holds_alternative<complex>(p)) [[likely]] {
                const complex &c = std::get<complex>(p);
                return {-2.0 * c.real(), std::norm(c)};
            } else {
                const auto &[a, b] = std::get<real_pair>(p);
                return {-(a + b), a * b};
            }
        };

        static auto pole_pair_to_polynomial = [](const conj_pair & p) noexcept->std::tuple<double, double> {
            if (std::holds_alternative<complex>(p)) [[likely]] {
                const complex &c = std::get<complex>(p);
                return {2.0 * c.real(), -std::norm(c)};
            } else {
                const auto &[a, b] = std::get<real_pair>(p);
                return {a + b, -a * b};
            }
        };

        const auto &[zeroes, poles, calibration_points] = ar;
        std::queue<conj_pair> zpq, ppq;

        for (const auto &z_pair : conj_pairs(zeroes)) {
            zpq.push(z_pair);
        }

        for (const auto &p_pair : conj_pairs(poles)) {
            ppq.push(p_pair);
        }

        _flow.clear();

        do {
            std::optional<conj_pair> zpair = drop(zpq);
            std::optional<conj_pair> ppair = drop(ppq);

            if (zpair && ppair) [[likely]] {
                const auto &[a1, a2] = zero_pair_to_polynomial(*zpair);
                const auto &[b1, b2] = pole_pair_to_polynomial(*ppair);
                _flow.push_back(std::make_unique<e_combo>(a1, a2, b1, b2));
            } else if (zpair) {
                const auto &[a1, a2] = zero_pair_to_polynomial(*zpair);
                _flow.push_back(std::make_unique<e_fir>(a1, a2));
            } else if (ppair) {
                const auto &[b1, b2] = pole_pair_to_polynomial(*ppair);
                _flow.push_back(std::make_unique<e_iir>(b1, b2));
            } else {
                break;
            };
        } while (true);

        _gain = 1.0;
        auto gains = calibration_points | std::views::transform([this](const complex & cp) noexcept {
            return std::abs(trans_func_(cp));
        });

        auto max_gain = *std::max_element(gains.begin(), gains.end());
        assert(max_gain > 0.0);
        _gain = 1.0 / max_gain;
    }

    INLINE
    double operator()(double sample) noexcept override {
        return std::ranges::fold_left(_flow, sample * _gain, [] INLINE (const auto & s, const auto & e) noexcept {
            return e->proc(s);
        });
    }

    complex trans_func_(complex z) const noexcept override {
        return std::ranges::fold_left(_flow, complex(_gain), [&z](const auto & t, const auto & e) noexcept {
            return t * e->trans_func(z);
        });
    }

  private:
    using real_pair = std::tuple<double, double>;
    using conj_pair = std::variant<complex, real_pair>;

    struct abstract_element {
        virtual ~abstract_element() noexcept = default;
        virtual double proc(double sample) noexcept = 0;
        virtual complex trans_func(const complex &z) noexcept = 0;
    };

    struct e_fir final:
        abstract_element {

        e_fir(double a1, double a2) noexcept:
            _a1(a1), _a2(a2) {
        }

        double proc(double sample) noexcept override {
            double out = sample + _a1 * _id1 + _a2 * _id2;
            return _id2 = _id1, _id1 = sample, out;
        }

        complex trans_func(const complex &z) noexcept override {
            return 1.0 + _a1 * z + _a2 * z * z;
        }

        double _a1 = 0.0;
        double _a2 = 0.0;

        double _id1 = 0.0;
        double _id2 = 0.0;
    };

    struct e_iir final:
        abstract_element {

        e_iir(double b1, double b2) noexcept:
            _b1(b1), _b2(b2) {
        }

        INLINE
        double proc(double sample) noexcept override {
            sample += _b1 * _od1 + _b2 * _od2;
            return _od2 = _od1, _od1 = sample;
        }

        complex trans_func(const complex &z) noexcept override {
            return 1.0 / (1.0 - _b1 * z - _b2 * z * z);
        }

        double _b1 = 0.0;
        double _b2 = 0.0;

        double _od1 = 0.0;
        double _od2 = 0.0;
    };

    struct e_combo final:
        abstract_element {

        e_combo(double a1, double a2, double b1, double b2) noexcept:
            _a1(a1), _a2(a2), _b1(b1), _b2(b2) {
        }

        INLINE
        double proc(double sample) noexcept override {
            double out = sample + _a1 * _id1 + _a2 * _id2 + _b1 * _od1 + _b2 * _od2;
            return _id2 = _id1, _id1 = sample, _od2 = _od1, _od1 = out;
        }

        complex trans_func(const complex &z) noexcept override {
            return (1.0 + _a1 * z + _a2 * z * z) / (1.0 - _b1 * z - _b2 * z * z);
        }

        double _a1 = 0.0;
        double _a2 = 0.0;
        double _b1 = 0.0;
        double _b2 = 0.0;

        double _id1 = 0.0;
        double _id2 = 0.0;
        double _od1 = 0.0;
        double _od2 = 0.0;
    };

    std::list<std::unique_ptr<abstract_element>> _flow;
    double _gain = 0.0;

    template<double scale = default_scale>
    static std::generator<conj_pair> conj_pairs(const std::list<complex> &clist) noexcept {
        using scaled = long long;
        std::set<std::tuple<scaled, scaled>> co_mem;
        std::optional<double> re_mem;

        for (const complex &c : clist) {
            scaled im = std::llround(c.imag() * scale);

            if (im) [[likely]] {
                scaled re = std::llround(c.real() * scale);

                if (co_mem.erase({re, -im})) {
                    co_yield complex{c.real(), std::abs(c.imag())};
                } else {
                    co_mem.insert({re, im});
                }
            } else {
                if (re_mem) {
                    co_yield real_pair{*re_mem, c.real()};
                    re_mem.reset();
                } else {
                    re_mem = c.real();
                }
            }
        }

        if (re_mem) {
            co_yield real_pair{*re_mem, 0.0};
        }

        assert(co_mem.empty());
    }
};

} // r namespace

/* ------------------------------------------------------------------------------------------------
 * Filter
 * ------------------------------------------------------------------------------------------------
 */

template <unsigned range, band bnd, tfa apx = tfa::__default__, r::realization rlzn = r::cascaded>
class filter final:
    private r::tf_approxer<range, bnd, apx>,
    private rlzn {

    using approxer = r::tf_approxer<range, bnd, apx>;
    using realization = rlzn;
    using realization::reset;

  public:
    filter() noexcept = default;

    filter(double rate, auto ... params) noexcept {
        reset(rate, params ...);
    }

    void reset(double rate, auto ... params) noexcept {
        realization::reset(approxer::approx(_rate = rate, params ...));
    }

    using realization::operator();

  private:
    double _rate = 0.0;
};

} // df namespace

#endif // DF_H
