#pragma once
#ifndef Z80_CPU_H
#define Z80_CPU_H

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

#include <cstdint>
#include <tuple>
#include <type_traits>
#include "tabulated.h"
#include "djrs.h"
#include "zxdefs.h"
#include "zxevents.h"
#include "zxports.h"
#include "zxmemory.h"

class Z80 final: public event::handler<Z80, EVENT::Z80_SET_PC_TRAP, EVENT::Z80_RELEASE_PC_TRAP, EVENT::SYS_NMI> {
  public:

    struct address {
        static constexpr uint16_t nmi = 0x0066_u16;
        static constexpr uint16_t im1 = 0x0038_u16;
        static constexpr uint16_t rst_00h = 0x0000_u16;
        static constexpr uint16_t rst_08h = 0x0008_u16;
        static constexpr uint16_t rst_10h = 0x0010_u16;
        static constexpr uint16_t rst_18h = 0x0018_u16;
        static constexpr uint16_t rst_20h = 0x0020_u16;
        static constexpr uint16_t rst_28h = 0x0028_u16;
        static constexpr uint16_t rst_30h = 0x0030_u16;
        static constexpr uint16_t rst_38h = 0x0038_u16;
    };

    enum class IMode : uint8_t { IM0 = 0, IM1 = 1, IM2 = 2 };

    Z80(ZxMemory &memory, ZxPorts &ioports) noexcept: _memory{memory}, _ioports{ioports} {
        reset();
    }
    ~Z80() noexcept = default;

    INLINE
    void handle(event::data<EVENT::Z80_SET_PC_TRAP> &ed) noexcept {
        ed.result = _pc_traps.add_range(ed.from, ed.to);
    }

    INLINE
    void handle(event::data<EVENT::Z80_RELEASE_PC_TRAP> &ed) noexcept {
        ed.result = _pc_traps.remove_range(ed.from);
    }

    INLINE
    void handle(const event::data<EVENT::SYS_NMI> &) noexcept {
        handle_nmi();
    }

    INLINE
    void reset() noexcept {
        _sp_reg.PC =
        _sp_reg.SP =
        _sp_reg.IR =
        _ix_reg.IX =
        _ix_reg.IY = 0x0000_u16;
        _iff1 =
        _iff2 =
        _halted = false;
        _imode = IMode::IM0;
    }

    INLINE
    void handle_int(uint8_t bus) noexcept {
        _halted = false;

        if (_iff1) {
            disable_int();

            switch (_imode) {
                case IMode::IM0:
                    /* Not used in ZX Spectrum */
                    break;

                case IMode::IM1:
                    call(address::im1);
                    break;

                case IMode::IM2:
                    uint16_t addr = _word(bus, _sp_reg.I);
                    call(peek_word(addr));
                    break;
            }
        }
    }

    INLINE
    void handle_nmi() noexcept {
        _halted = false;
        _iff1 = false;
        call(address::nmi);
    }

    INLINE
    int exec() noexcept {
        if (auto from = _pc_traps.find_range(_sp_reg.PC); from) {
            event::dispatch(event::data<EVENT::Z80_PC_TRAPPED> {.from = *from, .pc = _sp_reg.PC});
        }

        int ticks = _halted ? 4 : exec_noprefix();
        event::dispatch(event::data<EVENT::Z80_STEP> {.ticks = ticks});
        return ticks;
    }

    INLINE
    uint16_t PC() const noexcept {
        return _sp_reg.PC;
    }

    INLINE
    void PC(uint16_t addr) noexcept {
        _sp_reg.PC = addr;
    }

    INLINE
    uint16_t SP() const noexcept {
        return _sp_reg.SP;
    }

    INLINE
    IMode IM() const noexcept {
        return _imode;
    }

    INLINE
    bool int_state() const noexcept {
        return _iff1;
    }

    INLINE
    auto access_hatch() noexcept {
        return std::tie(_reg, _sh_reg, _ix_reg, _sp_reg, _iff1, _iff2, _imode, _halted);
    }

    INLINE
    auto access_hatch() const noexcept {
        return std::tie(_reg, _sh_reg, _ix_reg, _sp_reg, _iff1, _iff2, _imode, _halted);
    }

  private:

    struct prefix {
        static constexpr uint8_t CB = 0xCB_u8;
        static constexpr uint8_t ED = 0xED_u8;
        static constexpr uint8_t DD = 0xDD_u8;
        static constexpr uint8_t FD = 0xFD_u8;
    };

    struct mask {
        static constexpr uint8_t BIT_0 = 1 << 0;
        static constexpr uint8_t BIT_1 = 1 << 1;
        static constexpr uint8_t BIT_2 = 1 << 2;
        static constexpr uint8_t BIT_3 = 1 << 3;
        static constexpr uint8_t BIT_4 = 1 << 4;
        static constexpr uint8_t BIT_5 = 1 << 5;
        static constexpr uint8_t BIT_6 = 1 << 6;
        static constexpr uint8_t BIT_7 = 1 << 7;

        static constexpr uint8_t FLAG_C = BIT_0;
        static constexpr uint8_t FLAG_N = BIT_1;
        static constexpr uint8_t FLAG_P = BIT_2;
        static constexpr uint8_t FLAG_V = BIT_2;
        static constexpr uint8_t FLAG_3 = BIT_3;
        static constexpr uint8_t FLAG_H = BIT_4;
        static constexpr uint8_t FLAG_5 = BIT_5;
        static constexpr uint8_t FLAG_Z = BIT_6;
        static constexpr uint8_t FLAG_S = BIT_7;
        static constexpr uint8_t ALL_BUT_CF = FLAG_N | FLAG_P | FLAG_3 | FLAG_H | FLAG_5 | FLAG_Z | FLAG_S;
    };

    union nibbles_t {
        uint8_t u8;
        struct {
            uint8_t nb0: 4;
            uint8_t nb1: 4;
        };
    };

    struct reg_t;
    struct sp_reg_t;

    template<std::unsigned_integral T>
    static constexpr T hbit_mask = std::rotr(T{1}, 1); // high-order bit mask

    template<std::unsigned_integral T>
    static constexpr T b3_mask = hbit_mask<T> >> 4; // high-byte bit 3 mask

    template<std::unsigned_integral T>
    static constexpr T b5_mask = hbit_mask<T> >> 2; // high-byte bit 5 mask

    template<std::unsigned_integral T>
    static constexpr T range_mask = static_cast<T>(-1); // range mask(0xFF..FF)

    template<std::unsigned_integral T>
    static constexpr T hc_mask = range_mask<T> >> 4; // half-carry mask(0x0F..FF)

    template<std::unsigned_integral U>
    using mut_command = void (Z80::*)(U &) noexcept;

    using rep_command = bool (Z80::*)() noexcept;

    // --- ALU helpers ----------------------------------------------------------------------------

    template<std::unsigned_integral U, std::signed_integral W = int32_t> requires(sizeof(U) < sizeof(W))
    INLINE
    static uint8_t add_with_flags(U &dst, U src, bool carry = false) noexcept {
        using signed_t = std::make_signed_t<U>;
        const W w_carry = static_cast<W>(carry);
        const W w_dst_us = static_cast<W>(dst);
        const W w_src_us = static_cast<W>(src);
        const W w_dst_sg = static_cast<W>(static_cast<signed_t>(dst));
        const W w_src_sg = static_cast<W>(static_cast<signed_t>(src));
        const W w_sum_us = w_dst_us + w_src_us + w_carry;
        const W w_sum_sg = w_dst_sg + w_src_sg + w_carry;
        const W w_hsm_us = (w_dst_us & hc_mask<U>) + (w_src_us & hc_mask<U>) + w_carry;
        const U u_res = static_cast<U>(w_sum_us);

        reg_t flags;
        flags.CF = std::numeric_limits<U>::max() < w_sum_us;
        flags.NF = 0;
        flags.PVF = (w_sum_sg < std::numeric_limits<signed_t>::lowest()) || (std::numeric_limits<signed_t>::max() < w_sum_sg);
        flags.B3F = static_cast<bool>(w_sum_us & b3_mask<U>);
        flags.HF = hc_mask<U> < w_hsm_us;
        flags.B5F = static_cast<bool>(w_sum_us & b5_mask<U>);
        flags.ZF = u_res == 0;
        flags.SF = static_cast<signed_t>(u_res) < 0;

        dst = u_res;
        return flags.F;
    }

    template<bool b3b5_from_result, std::unsigned_integral U, std::signed_integral W = int32_t> requires(sizeof(U) < sizeof(W))
    INLINE
    static uint8_t sub_with_flags(U &dst, U src, bool carry = false) noexcept {
        using signed_t = std::make_signed_t<U>;
        const W w_carry = static_cast<W>(carry);
        const W w_dst_us = static_cast<W>(dst);
        const W w_src_us = static_cast<W>(src);
        const W w_dst_sg = static_cast<W>(static_cast<signed_t>(dst));
        const W w_src_sg = static_cast<W>(static_cast<signed_t>(src));
        const W w_dif_us = w_dst_us - w_src_us - w_carry;
        const W w_dif_sg = w_dst_sg - w_src_sg - w_carry;
        const W w_hdf_us = (w_dst_us & hc_mask<U>) - (w_src_us & hc_mask<U>) - w_carry;
        const U u_res = static_cast<U>(w_dif_us);

        reg_t flags;
        flags.CF = w_dif_us < 0;
        flags.NF = 1;
        flags.PVF = (w_dif_sg < std::numeric_limits<signed_t>::lowest()) || (std::numeric_limits<signed_t>::max() < w_dif_sg);
        flags.HF = w_hdf_us < 0;
        flags.ZF = u_res == 0;
        flags.SF = static_cast<signed_t>(u_res) < 0;

        if constexpr (b3b5_from_result) {
            flags.B3F = static_cast<bool>(w_dif_us & b3_mask<U>);
            flags.B5F = static_cast<bool>(w_dif_us & b5_mask<U>);
        } else {
            flags.B3F = static_cast<bool>(w_src_us & b3_mask<U>);
            flags.B5F = static_cast<bool>(w_src_us & b5_mask<U>);
        }

        dst = u_res;
        return flags.F;
    }

    template<bool hf, auto operation> requires bin_op_u8<decltype(operation)>
    INLINE
    static uint8_t logic_with_flags(uint8_t &dst, uint8_t src) noexcept {
        dst = static_cast<uint8_t>(operation(dst, src));
        return tabulated <
        [] (uint8_t _dst) noexcept {
            reg_t flags;
            flags.CF = 0;
            flags.NF = 0;
            flags.PVF = parity(_dst);
            flags.B3F = test_bit<3>(_dst);
            flags.HF = hf;
            flags.B5F = test_bit<5>(_dst);
            flags.ZF = !_dst;
            flags.SF = test_bit<7>(_dst);
            return flags.F;
        } > (dst);
    }

    template<auto bit_n, std::integral T>
    INLINE
    static constexpr bool test_bit(T t) noexcept requires bit_position<T, bit_n> {
        return t & (T{1} << bit_n);
    }

    // -- Flags helpers ---------------------------------------------------------------------------

    INLINE
    static uint8_t apply_flags_mask(uint8_t old_flags, uint8_t new_flags, uint8_t set_mask) noexcept {
        return ((old_flags ^ new_flags) & ~set_mask) ^ new_flags;
    }

    INLINE
    void update_flags(uint8_t new_flags, uint8_t set_mask) noexcept {
        _reg.F = apply_flags_mask(_reg.F, new_flags, set_mask);
    }

    INLINE
    static bool parity(uint8_t u8) noexcept {
        return tabulated <
        [] (uint8_t _u8) noexcept {
            return !(std::popcount(_u8) & 1);
        } > (u8);
    }

    INLINE
    void set_flags_shift_8080() noexcept {
        _reg.NF = 0;
        _reg.B3F = _reg.A3;
        _reg.HF = 0;
        _reg.B5F = _reg.A5;
    }

    INLINE
    void set_flags_shift_z80(uint8_t u8) noexcept {
        update_flags(tabulated <
        [] (uint8_t _u8) noexcept {
            reg_t flags;
            flags.NF = 0;
            flags.PVF = parity(_u8);
            flags.B3F = test_bit<3>(_u8);
            flags.HF = 0;
            flags.B5F = test_bit<5>(_u8);
            flags.ZF = !_u8;
            flags.SF = test_bit<7>(_u8);
            return flags.F;
        } > (u8), mask::ALL_BUT_CF);
    }

    INLINE
    bool ZF_off() const noexcept {
        return _reg.ZF == 0;
    }

    INLINE
    bool ZF_on() const noexcept {
        return _reg.ZF != 0;
    }

    INLINE
    bool CF_off() const noexcept {
        return _reg.CF == 0;
    }

    INLINE
    bool CF_on() const noexcept {
        return _reg.CF != 0;
    }

    INLINE
    bool PF_off() const noexcept {
        return _reg.PVF == 0;
    }

    INLINE
    bool PF_on() const noexcept {
        return _reg.PVF != 0;
    }

    INLINE
    bool SF_off() const noexcept {
        return _reg.SF == 0;
    }

    INLINE
    bool SF_on() const noexcept {
        return _reg.SF != 0;
    }

    INLINE
    bool NF_off() const noexcept {
        return _reg.NF == 0;
    }

    INLINE
    bool NF_on() const noexcept {
        return _reg.NF != 0;
    }

    INLINE
    bool HF_off() const noexcept {
        return _reg.HF == 0;
    }

    INLINE
    bool HF_on() const noexcept {
        return _reg.HF != 0;
    }

    // --- RAM helpers ----------------------------------------------------------------------------

    INLINE
    uint8_t peek_byte(uint16_t addr) noexcept {
        return _memory.peek(addr);
    }

    INLINE
    void poke_byte(uint16_t addr, uint8_t data) noexcept {
        _memory.poke(addr, data);
    }

    INLINE
    uint16_t peek_word(uint16_t addr) noexcept {
        return _word(peek_byte(addr), peek_byte(addr + 1));
    }

    INLINE
    void poke_word(uint16_t addr, uint16_t data) noexcept {
        poke_byte(addr, _lo(data));
        poke_byte(addr + 1, _hi(data));
    }

    template<auto m_struct, auto m_reg, bool incr = true, bool post = true>
    INLINE
    void store_byte(uint8_t u8) noexcept {
        static_assert(std::same_as < std::decay_t < decltype(this->*m_struct.*m_reg) >, uint16_t >, "'store' is only defined for 16-bit address registers");
        uint16_t &r16 = this->*m_struct.*m_reg;

        if constexpr (post) {
            if constexpr (incr) {
                poke_byte(r16++, u8);
            } else {
                poke_byte(r16--, u8);
            }
        } else {
            if constexpr (incr) {
                poke_byte(++r16, u8);
            } else {
                poke_byte(--r16, u8);
            }
        };
    }

    template<auto m_struct, auto m_reg, bool incr = true, bool post = true>
    INLINE
    uint8_t fetch_byte() noexcept {
        static_assert(std::same_as < std::decay_t < decltype(this->*m_struct.*m_reg) >, uint16_t >, "'fetch' is only defined for 16-bit address registers");
        uint16_t &r16 = this->*m_struct.*m_reg;

        if constexpr (post) {
            if constexpr (incr) {
                return peek_byte(r16++);
            } else {
                return peek_byte(r16--);
            }
        } else {
            if constexpr (incr) {
                return peek_byte(++r16);
            } else {
                return peek_byte(--r16);
            }
        };
    }

    template<auto m_struct, auto m_reg>
    INLINE
    void store_word(uint16_t u16) noexcept {
        store_byte<m_struct, m_reg, true, true>(_lo(u16));
        store_byte<m_struct, m_reg, true, true>(_hi(u16));
    }

    template<auto m_struct, auto m_reg>
    INLINE
    void push_word(uint16_t u16) noexcept {
        store_byte<m_struct, m_reg, false, false>(_hi(u16));
        store_byte<m_struct, m_reg, false, false>(_lo(u16));
    }

    template<auto m_struct, auto m_reg>
    INLINE
    uint16_t fetch_word() noexcept {
        auto lo = fetch_byte<m_struct, m_reg, true, true>();
        return _word(lo, fetch_byte<m_struct, m_reg, true, true>());
    }

    template<typename T = uint8_t> requires (std::convertible_to<uint8_t, T> && sizeof(T) == 1)
    INLINE
    T fetch_pc_byte() noexcept {
        return fetch_byte<_PC_>();
    }

    template<typename T = uint16_t> requires (std::convertible_to<uint16_t, T> && sizeof(T) == 2)
    INLINE
    T fetch_pc_word() noexcept {
        return fetch_word<_PC_>();
    }

    // --- WZ helpers -----------------------------------------------------------------------------

    INLINE
    uint16_t wz_set(uint16_t u16) noexcept {
        return _sp_reg.WZ = u16, u16;
    }

    INLINE
    uint16_t wz_set_plus_one(uint16_t u16) noexcept {
        return _sp_reg.WZ = u16 + 1, u16;
    }

    INLINE
    uint16_t wz_set_plus_one(uint16_t u16, uint8_t u8) noexcept {
        return _sp_reg.WZL = u16 + 1, _sp_reg.WZH = u8, u16;
    }

    INLINE
    void wz_inc() noexcept {
        ++_sp_reg.WZ;
    }

    INLINE
    void wz_dec() noexcept {
        --_sp_reg.WZ;
    }

    // --- Command helpers ------------------------------------------------------------------------

    INLINE
    void inc_refresh_register() noexcept {
        ++_sp_reg.R_counter;
    }

    template<mut_command<uint8_t> cmd>
    INLINE
    void modify_at(uint16_t addr) noexcept {
        uint8_t u8 = peek_byte(addr);
        (this->*cmd)(u8);
        poke_byte(addr, u8);
    }

    template<mut_command<uint8_t> cmd, uint8_t reg_t::*reg>
    INLINE
    void modify_via(uint16_t addr) noexcept {
        uint8_t &r8 = _reg.*reg;
        r8 = peek_byte(addr);
        (this->*cmd)(r8);
        poke_byte(addr, r8);
    }

    template<rep_command cmd, int bs_ticks, int no_bs_ticks, uint16_t backstep = 0x0002_u16>
    INLINE
    int cond_rep() noexcept {
        return (this->*cmd)() ? (_sp_reg.PC -= backstep, bs_ticks) : no_bs_ticks;
    }

    INLINE
    void swap_stack(uint16_t &r16) noexcept {
        uint16_t u16 = peek_word(_sp_reg.SP);
        poke_word(_sp_reg.SP, r16);
        wz_set(r16 = u16);
    }

    template<uint8_t (Z80::*fetch)() noexcept, bool alone, auto wz_mod> // cpi | cpd
    INLINE
    bool cpx() noexcept {
        uint8_t ra {_reg.A};
        uint8_t mb = (this->*fetch)();
        update_flags(sub_with_flags<false>(ra, mb), mask::FLAG_N | mask::FLAG_H | mask::FLAG_Z | mask::FLAG_S);

        ra -= _reg.HF;
        _reg.B3F = test_bit<3>(ra);
        _reg.B5F = test_bit<1>(ra);

        if constexpr (alone) {
            return (this->*wz_mod)(), (_reg.PVF = --_reg.BC != 0) && ZF_off();
        } else {
            return ((_reg.PVF = --_reg.BC != 0) && ZF_off()) ? (wz_set(_sp_reg.PC - 1), true) : ((this->*wz_mod)(), false) ;
        }
    }

    template<uint8_t (Z80::*fetch)() noexcept, void (Z80::*store)(uint8_t) noexcept, bool alone> // ldi | ldd
    INLINE
    bool ldx() noexcept {
        uint8_t u8;
        (this->*store)(u8 = (this->*fetch)());
        u8 += _reg.A;
        _reg.NF = 0;
        _reg.B3F = test_bit<3>(u8);
        _reg.HF = 0;
        _reg.B5F = test_bit<1>(u8);

        if constexpr (alone) {
            return (_reg.PVF = --_reg.BC != 0);
        } else {
            return (_reg.PVF = --_reg.BC != 0) ? (wz_set(_sp_reg.PC - 1), true) : false;
        }
    }

    template<auto m_struct, auto m_reg>
    INLINE
    void in_reg() noexcept {
        static_assert (std::same_as < std::decay_t < decltype(this->*m_struct.*m_reg) >, uint8_t >, "'in_reg' is only defined for 8-bit registers");
        uint8_t &r8 = this->*m_struct.*m_reg;
        r8 = _ioports.in(wz_set_plus_one(_reg.BC));

        update_flags(tabulated <
        [] (uint8_t _r8) noexcept {
            reg_t flags;
            flags.NF = 0;
            flags.PVF = parity(_r8);
            flags.B3F = test_bit<3>(_r8);
            flags.HF = 0;
            flags.B5F = test_bit<5>(_r8);
            flags.ZF = !_r8;
            flags.SF = test_bit<7>(_r8);
            return flags.F;
        } > (r8), mask::ALL_BUT_CF);
    }

    INLINE
    void out_byte(uint8_t u8) noexcept {
        _ioports.out(wz_set_plus_one(_reg.BC), u8);
    }

    INLINE
    uint8_t iox_in() noexcept {
        return _ioports.in(_reg.BC);
    }

    INLINE
    void iox_out(uint8_t u8) noexcept {
        _ioports.out(_reg.BC, u8);
    }

    enum class iox_mod : int32_t { ini = 1, ind = -1, outi = 0, outd = 0 };

    template<uint8_t (Z80::*fetch)() noexcept, void (Z80::*store)(uint8_t) noexcept, iox_mod mod> // ini | ind | outi | outd
    INLINE
    bool iox() noexcept {
        static_assert(mod == iox_mod::ini || mod == iox_mod::ind || mod == iox_mod::outi);
        uint32_t n;

        (this->*store)(n = (this->*fetch)());
        _reg.ZF = !--_reg.B;
        _reg.NF = test_bit<7>(n);
        _reg.B3F = test_bit<3>(_reg.B);
        _reg.B5F = test_bit<5>(_reg.B);
        _reg.SF = test_bit<7>(_reg.B);

        if constexpr (static_cast<bool>(mod)) {
            n += (_reg.C + static_cast<int32_t>(mod)) & 0xFF;
        } else {
            n += _reg.L;
        }

        _reg.HF = _reg.CF = n > 0xFF;
        _reg.PVF = parity((n & 0x07) ^ _reg.B);

        return ZF_off();
    }

    INLINE
    void ld_a_ir(uint8_t r8) noexcept { // ld A,I|R
        _reg.A = r8;
        _reg.NF = 0;
        _reg.PVF = _iff2;
        _reg.B3F = _reg.A3;
        _reg.HF = 0;
        _reg.B5F = _reg.A5;
        _reg.ZF = !_reg.A;
        _reg.SF = _reg.A7;
    }

    // --- Command implementations ---

    template<bool update_wz = true>
    INLINE
    void jump(uint16_t addr) noexcept {
        if constexpr (update_wz) {
            _sp_reg.PC = wz_set(addr);
        } else {
            _sp_reg.PC = addr;
        }
    }

    INLINE
    void cond_jump(uint16_t addr, bool cond) noexcept {
        if (cond) {
            jump(addr);
        } else {
            wz_set(addr);
        }
    }

    INLINE
    void jump_r(int8_t disp) noexcept {
        jump(_sp_reg.PC + disp);
    }

    template<int jr_ticks, int no_jr_ticks>
    INLINE
    int cond_jump_r(int8_t disp, bool cond) noexcept {
        return cond ? (jump_r(disp), jr_ticks) : no_jr_ticks;
    }

    INLINE
    void call(uint16_t addr) noexcept {
        push(_sp_reg.PC);
        jump(addr);
    }

    template<int call_ticks, int no_call_ticks>
    INLINE
    int cond_call(uint16_t addr, bool cond) noexcept {
        return cond ? (call(addr), call_ticks) : (wz_set(addr), no_call_ticks);
    }

    INLINE
    void ret() noexcept {
        jump(pop());
    }

    template <int ret_ticks, int no_ret_ticks>
    INLINE
    int cond_ret(bool cond) noexcept {
        return cond ? (ret(), ret_ticks) : no_ret_ticks;
    }

    template<uint16_t addr> requires rst_address<addr>
    INLINE
    void rst() noexcept {
        call(addr);
    }

    INLINE
    void ret_nmi() noexcept {
        _iff1 = _iff2;
        ret();
    }

    INLINE
    void ret_int() noexcept {
        ret();
    }

    INLINE
    void enable_int() noexcept {
        _iff1 = _iff2 = true;
    }

    INLINE
    void disable_int() noexcept {
        _iff1 = _iff2 = false;
    }

    INLINE
    void push(uint16_t data) noexcept {
        push_word<_SP_>(data);
    }

    INLINE
    uint16_t pop() noexcept {
        return fetch_word<_SP_>();
    }

    template<bool alone = true>
    INLINE
    bool ldi() noexcept {
        return ldx<&Z80::fetch_byte<_HL_>, &Z80::store_byte<_DE_>, alone>();
    }

    template<bool alone = true>
    INLINE
    bool ldd() noexcept {
        return ldx<&Z80::fetch_byte<_HL_, false>, &Z80::store_byte<_DE_, false>, alone>();
    }

    template<bool alone = true>
    INLINE
    bool cpi() noexcept {
        return cpx<&Z80::fetch_byte<_HL_>, alone, &Z80::wz_inc>();
    }

    template<bool alone = true>
    INLINE
    bool cpd() noexcept {
        return cpx<&Z80::fetch_byte<_HL_, false>, alone, &Z80::wz_dec>();
    }

    INLINE
    bool ini() noexcept {
        wz_set(_reg.BC + 1);
        return iox<&Z80::iox_in, &Z80::store_byte<_HL_>, iox_mod::ini>();
    }

    INLINE
    bool ind() noexcept {
        wz_set(_reg.BC - 1);
        return iox <&Z80::iox_in, &Z80::store_byte<_HL_, false>, iox_mod::ind> ();
    }

    INLINE
    bool outi() noexcept {
        auto res = iox<&Z80::fetch_byte<_HL_>, &Z80::iox_out, iox_mod::outi>();
        return wz_set(_reg.BC + 1), res;
    }

    INLINE
    bool outd() noexcept {
        auto res = iox<&Z80::fetch_byte<_HL_, false>, &Z80::iox_out, iox_mod::outd>();
        return wz_set(_reg.BC - 1), res;
    }

    INLINE
    void add(uint8_t u8) noexcept {
        _reg.F = add_with_flags(_reg.A, u8);
    }

    INLINE
    void adc(uint8_t u8) noexcept {
        _reg.F = add_with_flags(_reg.A, u8, _reg.CF);
    }

    INLINE
    void sub(uint8_t u8) noexcept {
        _reg.F = sub_with_flags<true>(_reg.A, u8);
    }

    INLINE
    void sbc(uint8_t u8) noexcept {
        _reg.F = sub_with_flags<true>(_reg.A, u8, _reg.CF);
    }

    INLINE
    void daa() noexcept {
        static constexpr uint8_t corr[4][2] = {{0x00_u8, 0x00_u8}, {0x06_u8, 0xFA_u8}, {0x60_u8, 0xA0_u8}, {0x66_u8, 0x9A_u8}};
        int selector = 0;

        if (_reg.HF || (_reg.A_nb0 > 0x09_u8)) {
            selector = 1;
        }

        if (_reg.CF || (_reg.A > 0x99_u8)) {
            _reg.CF = 1;
            selector += 2;
        }

        _reg.HF = (_reg.NF && !_reg.HF) ? false : (_reg.NF ? _reg.A_nb0 < 0x06_u8 : _reg.A_nb0 >= 0x0A_u8);
        _reg.A += corr[selector][_reg.NF];

        _reg.SF = _reg.A7;
        _reg.ZF = !_reg.A;
        _reg.PVF = parity(_reg.A);
        _reg.B5F = _reg.A5;
        _reg.B3F = _reg.A3;
    }

    INLINE
    void cpl() noexcept {
        _reg.A = ~ _reg.A;
        _reg.NF = _reg.HF = 1;
        _reg.B3F = _reg.A3;
        _reg.B5F = _reg.A5;
    }

    INLINE
    void ccf() noexcept {
        _reg.HF = _reg.CF;
        _reg.CF = !_reg.CF;
        _reg.NF = 0;
        _reg.B3F = _reg.A3;
        _reg.B5F = _reg.A5;
    }

    INLINE
    void scf() noexcept {
        _reg.CF = 1;
        _reg.NF = _reg.HF = 0;
        _reg.B3F = _reg.A3;
        _reg.B5F = _reg.A5;
    }

    INLINE
    void neg() noexcept {
        uint8_t u8 = _reg.A;
        _reg.A = 0x00_u8;
        _reg.F = sub_with_flags<true>(_reg.A, u8);
    }

    INLINE
    void bw_and(uint8_t u8) noexcept {
        _reg.F = logic_with_flags < true, [] INLINE (uint8_t lh, uint8_t rh) noexcept -> uint8_t {
            return lh & rh;
        } > (_reg.A, u8);
    }

    INLINE
    void bw_or(uint8_t u8) noexcept {
        _reg.F = logic_with_flags < false, [] INLINE (uint8_t lh, uint8_t rh) noexcept -> uint8_t {
            return lh | rh;
        } > (_reg.A, u8);
    }

    INLINE
    void bw_xor(uint8_t u8) noexcept {
        _reg.F = logic_with_flags < false, [] INLINE (uint8_t lh, uint8_t rh) noexcept -> uint8_t {
            return lh ^ rh;
        } > (_reg.A, u8);
    }

    INLINE
    void cp(uint8_t u8) noexcept {
        uint8_t t_ {_reg.A};
        _reg.F = sub_with_flags<false>(t_, u8);
    }

    INLINE
    void inc(uint8_t &u8) noexcept {
        update_flags(add_with_flags(u8, 0x01_u8), mask::ALL_BUT_CF);
    }

    INLINE
    void dec(uint8_t &u8) noexcept {
        update_flags(sub_with_flags<true>(u8, 0x01_u8), mask::ALL_BUT_CF);
    }

    /* -- I8080-compatible rotations --- */

    INLINE
    void rlca() noexcept {
        _reg.A = std::rotl(_reg.A, 1);
        _reg.CF = _reg.A0;
        set_flags_shift_8080();
    }

    INLINE
    void rrca() noexcept {
        _reg.A = std::rotr(_reg.A, 1);
        _reg.CF = _reg.A7;
        set_flags_shift_8080();
    }

    INLINE
    void rla() noexcept {
        auto cf = _reg.CF;
        _reg.CF = _reg.A7;
        _reg.A <<= 1;
        _reg.A0 = cf;
        set_flags_shift_8080();
    }

    INLINE
    void rra() noexcept {
        auto cf = _reg.CF;
        _reg.CF = _reg.A0;
        _reg.A >>= 1;
        _reg.A7 = cf;
        set_flags_shift_8080();
    }

    /* -- Extended SoR (shift-or-rotation) operations */

    INLINE
    void rrd() noexcept {
        nibbles_t nbs;
        nbs.u8 = peek_byte(wz_set_plus_one(_reg.HL));

        auto n = nbs.nb0;
        nbs.nb0 = nbs.nb1;
        nbs.nb1 = _reg.A_nb0;
        _reg.A_nb0 = n;

        poke_byte(_reg.HL, nbs.u8);
        set_flags_shift_z80(_reg.A);
    }

    INLINE
    void rld() noexcept {
        nibbles_t nbs;
        nbs.u8 = peek_byte(wz_set_plus_one(_reg.HL));

        auto n = _reg.A_nb0;
        _reg.A_nb0 = nbs.nb1;
        nbs.nb1 = nbs.nb0;
        nbs.nb0 = n;

        poke_byte(_reg.HL, nbs.u8);
        set_flags_shift_z80(_reg.A);
    }

    INLINE
    void rlc(uint8_t &u8) noexcept {
        u8 = std::rotl(u8, 1);
        _reg.CF = test_bit<0>(u8);
        set_flags_shift_z80(u8);
    }

    INLINE
    void rl(uint8_t &u8) noexcept {
        auto cf = _reg.CF;
        _reg.CF = test_bit<7>(u8);
        u8 = (u8 << 1) | cf;
        set_flags_shift_z80(u8);
    }

    INLINE
    void rrc(uint8_t &u8) noexcept {
        u8 = std::rotr(u8, 1);
        _reg.CF = test_bit<7>(u8);
        set_flags_shift_z80(u8);
    }

    INLINE
    void rr(uint8_t &u8) noexcept {
        auto cf = _reg.CF ? 0x80_u8 : 0x00_u8;
        _reg.CF = test_bit<0>(u8);
        u8 = (u8 >> 1) | cf;
        set_flags_shift_z80(u8);
    }

    INLINE
    void sla(uint8_t &u8) noexcept {
        _reg.CF = test_bit<7>(u8);
        u8 <<= 1;
        set_flags_shift_z80(u8);
    }

    INLINE
    void sra(uint8_t &u8) noexcept {
        _reg.CF = test_bit<0>(u8);
        u8 = static_cast<int8_t>(u8) >> 1;
        set_flags_shift_z80(u8);
    }

    INLINE
    void sli(uint8_t &u8) noexcept {
        _reg.CF = test_bit<7>(u8);
        u8 <<= 1;
        u8 |= mask::BIT_0;
        set_flags_shift_z80(u8);
    }

    INLINE
    void srl(uint8_t &u8) noexcept {
        _reg.CF = test_bit<0>(u8);
        u8 >>= 1;
        set_flags_shift_z80(u8);
    }

    /* -- Bit operations */

    template<int bit_n> requires bit_position<uint8_t, bit_n>
    INLINE
    void bit(uint8_t u8) noexcept {
        update_flags(tabulated <
        [] (uint8_t _u8) noexcept {
            reg_t flags;
            bool bit_state = test_bit<bit_n>(_u8);
            flags.NF = 0;
            flags.PVF = flags.ZF = !bit_state;
            flags.B3F = test_bit<3>(_u8);
            flags.HF = 1;
            flags.B5F = test_bit<5>(_u8);
            flags.SF = (bit_n == 7) && bit_state;
            return flags.F;
        } > (u8), mask::ALL_BUT_CF);
    }

    template<int bit_n> requires bit_position<uint8_t, bit_n>
    INLINE
    void bit(uint8_t u8, uint16_t u16) noexcept {
        bool bit_state = test_bit<bit_n>(u8);
        _reg.NF = 0;
        _reg.PVF = _reg.ZF = !bit_state;
        _reg.B3F = test_bit<11>(u16);
        _reg.HF = 1;
        _reg.B5F = test_bit<13>(u16);
        _reg.SF = (bit_n == 7) && bit_state;
    }

    template<int bit_n> requires bit_position<uint8_t, bit_n>
    INLINE
    void set(uint8_t &u8) noexcept {
        u8 |= mask::BIT_0 << bit_n;
    }

    template<int bit_n> requires bit_position<uint8_t, bit_n>
    INLINE
    void res(uint8_t &u8) noexcept {
        u8 &= ~(mask::BIT_0 << bit_n);
    }

    /* -- 16-bit arithmetics */

    INLINE
    void add(uint16_t &dst, uint16_t src) noexcept {
        wz_set_plus_one(dst);
        update_flags(add_with_flags(dst, src), mask::FLAG_C | mask::FLAG_N | mask::FLAG_H | mask::FLAG_3 | mask::FLAG_5);
    }

    INLINE
    void adc(uint16_t &dst, uint16_t src) noexcept {
        wz_set_plus_one(dst);
        _reg.F = add_with_flags(dst, src, _reg.CF);
    }

    INLINE
    void sbc(uint16_t &dst, uint16_t src) noexcept {
        wz_set_plus_one(dst);
        _reg.F = sub_with_flags<true>(dst, src, _reg.CF);
    }

    int exec_noprefix() noexcept;
    int exec_prefix_cb() noexcept;
    int exec_prefix_ed() noexcept;
    int exec_prefix_xd(uint16_t &IxR) noexcept;
    int exec_prefix_xd_cb(const uint16_t &IxR) noexcept;

  private:

    ZxMemory &_memory;
    ZxPorts &_ioports;

    bool _halted;
    bool _iff1;
    bool _iff2;
    IMode _imode;

    struct reg_t { // g.p. registers
        union {
            uint16_t AF;
            struct {
                union {
                    uint8_t F;
                    struct {
                        uint8_t CF: 1;
                        uint8_t NF: 1;
                        uint8_t PVF: 1;
                        uint8_t B3F: 1;
                        uint8_t HF: 1;
                        uint8_t B5F: 1;
                        uint8_t ZF: 1;
                        uint8_t SF: 1;
                    };
                };
                union {
                    uint8_t A;
                    struct {
                        uint8_t A0: 1;
                        uint8_t A1: 1;
                        uint8_t A2: 1;
                        uint8_t A3: 1;
                        uint8_t A4: 1;
                        uint8_t A5: 1;
                        uint8_t A6: 1;
                        uint8_t A7: 1;
                    };
                    struct {
                        uint8_t A_nb0: 4;
                        uint8_t A_nb1: 4;
                    };
                };
            };
        };
        union {
            uint16_t BC;
            struct {
                uint8_t C;
                uint8_t B;
            };
        };
        union {
            uint16_t DE;
            struct {
                uint8_t E;
                uint8_t D;
            };
        };
        union {
            uint16_t HL;
            struct {
                uint8_t L;
                uint8_t H;
            };
        };
    } _reg, _sh_reg;

    struct ix_reg_t { // index registers
        union {
            uint16_t IX;
            struct {
                uint8_t IXL;
                uint8_t IXH;
            };
        };
        union {
            uint16_t IY;
            struct {
                uint8_t IYL;
                uint8_t IYH;
            };
        };
    } _ix_reg;

    struct sp_reg_t { // special registers
        union {
            uint16_t IR;
            struct {
                union {
                    uint8_t R;
                    struct {
                        uint8_t R_counter: 7;
                        uint8_t R_bit7: 1;
                    };
                };
                uint8_t I;
            };
        };
        union {
            uint16_t PC;
            struct {
                uint8_t PCL;
                uint8_t PCH;
            };
        };
        union {
            uint16_t SP;
            struct {
                uint8_t SPL;
                uint8_t SPH;
            };
        };
        union {
            uint16_t WZ;
            struct {
                uint8_t WZL;
                uint8_t WZH;
            };
        };
    } _sp_reg;

    struct { // fake register for INF
        uint8_t _;
    } _;

    disjoint_ranges_set<uint16_t> _pc_traps;

    static_assert(std::endian::native == std::endian::little, "This code assumes little-endian architecture");
    static_assert(sizeof _reg == sizeof(uint16_t) * 4, "Unexpected union size");
    static_assert(sizeof _ix_reg == sizeof(uint16_t) * 2, "Unexpected union size");
    static_assert(sizeof _sp_reg == sizeof(uint16_t) * 4, "Unexpected union size");
    static_assert(sizeof(nibbles_t) == sizeof(uint8_t), "Unexpected union size");
};

#endif // Z80_CPU_H
