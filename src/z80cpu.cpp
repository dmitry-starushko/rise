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

#include "z80cpu.h"
#include <utility>

int Z80::exec_noprefix() noexcept {

    inc_refresh_register();

    switch (fetch_pc_byte()) {

        // .......... nop
        case 0x00:

        // .......... ld a,a
        case 0x7F:

        // .......... ld b,b
        case 0x40:

        // .......... ld c,c
        case 0x49:

        // .......... ld d,d
        case 0x52:

        // .......... ld e,e
        case 0x5B:

        // .......... ld h,h
        case 0x64:

        // .......... ld l,l
        case 0x6D:
            return 4;

        // .......... ld a,b
        case 0x78:
            _reg.A = _reg.B;
            return 4;

        // .......... ld a,c
        case 0x79:
            _reg.A = _reg.C;
            return 4;

        // .......... ld a,d
        case 0x7A:
            _reg.A = _reg.D;
            return 4;

        // .......... ld a,e
        case 0x7B:
            _reg.A = _reg.E;
            return 4;

        // .......... ld a,h
        case 0x7C:
            _reg.A = _reg.H;
            return 4;

        // .......... ld a,l
        case 0x7D:
            _reg.A = _reg.L;
            return 4;

        // .......... ld b,c
        case 0x41:
            _reg.B = _reg.C;
            return 4;

        // .......... ld b,d
        case 0x42:
            _reg.B = _reg.D;
            return 4;

        // .......... ld b,e
        case 0x43:
            _reg.B = _reg.E;
            return 4;

        // .......... ld b,h
        case 0x44:
            _reg.B = _reg.H;
            return 4;

        // .......... ld b,l
        case 0x45:
            _reg.B = _reg.L;
            return 4;

        // .......... ld b,a
        case 0x47:
            _reg.B = _reg.A;
            return 4;

        // .......... ld c,b
        case 0x48:
            _reg.C = _reg.B;
            return 4;

        // .......... ld c,d
        case 0x4A:
            _reg.C = _reg.D;
            return 4;

        // .......... ld c,e
        case 0x4B:
            _reg.C = _reg.E;
            return 4;

        // .......... ld c,h
        case 0x4C:
            _reg.C = _reg.H;
            return 4;

        // .......... ld c,l
        case 0x4D:
            _reg.C = _reg.L;
            return 4;

        // .......... ld c,a
        case 0x4F:
            _reg.C = _reg.A;
            return 4;

        // .......... ld d,b
        case 0x50:
            _reg.D = _reg.B;
            return 4;

        // .......... ld d,c
        case 0x51:
            _reg.D = _reg.C;
            return 4;

        // .......... ld d,e
        case 0x53:
            _reg.D = _reg.E;
            return 4;

        // .......... ld d,h
        case 0x54:
            _reg.D = _reg.H;
            return 4;

        // .......... ld d,l
        case 0x55:
            _reg.D = _reg.L;
            return 4;

        // .......... ld d,a
        case 0x57:
            _reg.D = _reg.A;
            return 4;

        // .......... ld e,b
        case 0x58:
            _reg.E = _reg.B;
            return 4;

        // .......... ld e,c
        case 0x59:
            _reg.E = _reg.C;
            return 4;

        // .......... ld e,d
        case 0x5A:
            _reg.E = _reg.D;
            return 4;

        // .......... ld e,h
        case 0x5C:
            _reg.E = _reg.H;
            return 4;

        // .......... ld e,l
        case 0x5D:
            _reg.E = _reg.L;
            return 4;

        // .......... ld e,a
        case 0x5F:
            _reg.E = _reg.A;
            return 4;

        // .......... ld h,b
        case 0x60:
            _reg.H = _reg.B;
            return 4;

        // .......... ld h,c
        case 0x61:
            _reg.H = _reg.C;
            return 4;

        // .......... ld h,d
        case 0x62:
            _reg.H = _reg.D;
            return 4;

        // .......... ld h,e
        case 0x63:
            _reg.H = _reg.E;
            return 4;

        // .......... ld h,l
        case 0x65:
            _reg.H = _reg.L;
            return 4;

        // .......... ld h,a
        case 0x67:
            _reg.H = _reg.A;
            return 4;

        // .......... ld l,b
        case 0x68:
            _reg.L = _reg.B;
            return 4;

        // .......... ld l,c
        case 0x69:
            _reg.L = _reg.C;
            return 4;

        // .......... ld l,d
        case 0x6A:
            _reg.L = _reg.D;
            return 4;

        // .......... ld l,e
        case 0x6B:
            _reg.L = _reg.E;
            return 4;

        // .......... ld l,h
        case 0x6C:
            _reg.L = _reg.H;
            return 4;

        // .......... ld l,a
        case 0x6F:
            _reg.L = _reg.A;
            return 4;

        // .......... ld b,(hl)
        case 0x46:
            _reg.B = peek_byte(_reg.HL);
            return 7;

        // .......... ld c,(hl)
        case 0x4E:
            _reg.C = peek_byte(_reg.HL);
            return 7;

        // .......... ld d,(hl)
        case 0x56:
            _reg.D = peek_byte(_reg.HL);
            return 7;

        // .......... ld e,(hl)
        case 0x5E:
            _reg.E = peek_byte(_reg.HL);
            return 7;

        // .......... ld h,(hl)
        case 0x66:
            _reg.H = peek_byte(_reg.HL);
            return 7;

        // .......... ld l,(hl)
        case 0x6E:
            _reg.L = peek_byte(_reg.HL);
            return 7;

        // .......... ld a,(hl)
        case 0x7E:
            _reg.A = peek_byte(_reg.HL);
            return 7;

        // .......... ld a,(bc)
        case 0x0A:
            _reg.A = peek_byte(wz_set_plus_one(_reg.BC));
            return 7;

        // .......... ld a,(de)
        case 0x1A:
            _reg.A = peek_byte(wz_set_plus_one(_reg.DE));
            return 7;

        // .......... ld a,(nn)
        case 0x3A:
            _reg.A = peek_byte(wz_set_plus_one(fetch_pc_word()));
            return 13;

        // .......... ld (hl),b
        case 0x70:
            poke_byte(_reg.HL, _reg.B);
            return 7;

        // .......... ld (hl),c
        case 0x71:
            poke_byte(_reg.HL, _reg.C);
            return 7;

        // .......... ld (hl),d
        case 0x72:
            poke_byte(_reg.HL, _reg.D);
            return 7;

        // .......... ld (hl),e
        case 0x73:
            poke_byte(_reg.HL, _reg.E);
            return 7;

        // .......... ld (hl),h
        case 0x74:
            poke_byte(_reg.HL, _reg.H);
            return 7;

        // .......... ld (hl),l
        case 0x75:
            poke_byte(_reg.HL, _reg.L);
            return 7;

        // .......... ld (hl),a
        case 0x77:
            poke_byte(_reg.HL, _reg.A);
            return 7;

        // .......... ld (bc),a
        case 0x02:
            poke_byte(wz_set_plus_one(_reg.BC, _reg.A), _reg.A);
            return 7;

        // .......... ld (de),a
        case 0x12:
            poke_byte(wz_set_plus_one(_reg.DE, _reg.A), _reg.A);
            return 7;

        // .......... ld (nn),a
        case 0x32:
            poke_byte(wz_set_plus_one(fetch_pc_word(), _reg.A), _reg.A);
            return 13;

        // .......... ld b,n
        case 0x06:
            _reg.B = fetch_pc_byte();
            return 7;

        // .......... ld c,n
        case 0x0E:
            _reg.C = fetch_pc_byte();
            return 7;

        // .......... ld d,n
        case 0x16:
            _reg.D = fetch_pc_byte();
            return 7;

        // .......... ld e,n
        case 0x1E:
            _reg.E = fetch_pc_byte();
            return 7;

        // .......... ld h,n
        case 0x26:
            _reg.H = fetch_pc_byte();
            return 7;

        // .......... ld l,n
        case 0x2E:
            _reg.L = fetch_pc_byte();
            return 7;

        // .......... ld a,n
        case 0x3E:
            _reg.A = fetch_pc_byte();
            return 7;

        // .......... ld (hl),n
        case 0x36:
            poke_byte(_reg.HL, fetch_pc_byte());
            return 10;

        // .......... ld bc,nn
        case 0x01:
            _reg.BC = fetch_pc_word();
            return 10;

        // .......... ld de,nn
        case 0x11:
            _reg.DE = fetch_pc_word();
            return 10;

        // .......... ld hl,nn
        case 0x21:
            _reg.HL = fetch_pc_word();
            return 10;

        // .......... ld sp,nn
        case 0x31:
            _sp_reg.SP = fetch_pc_word();
            return 10;

        // .......... ld hl,(nn)
        case 0x2A:
            _reg.HL = peek_word(wz_set_plus_one(fetch_pc_word()));
            return 16;

        // .......... ld (nn),hl
        case 0x22:
            poke_word(wz_set_plus_one(fetch_pc_word()), _reg.HL);
            return 16;

        // .......... ld sp,hl
        case 0xF9:
            _sp_reg.SP = _reg.HL;
            return 6;

        // .......... push bc
        case 0xC5:
            push(_reg.BC);
            return 11;

        // .......... push de
        case 0xD5:
            push(_reg.DE);
            return 11;

        // .......... push hl
        case 0xE5:
            push(_reg.HL);
            return 11;

        // .......... push af
        case 0xF5:
            push(_reg.AF);
            return 11;

        // .......... pop bc
        case 0xC1:
            _reg.BC = pop();
            return 10;

        // .......... pop de
        case 0xD1:
            _reg.DE = pop();
            return 10;

        // .......... pop hl
        case 0xE1:
            _reg.HL = pop();
            return 10;

        // .......... pop af
        case 0xF1:
            _reg.AF = pop();
            return 10;

        // .......... jp addr
        case 0xC3:
            jump(fetch_pc_word());
            return 10;

        // .......... jp nz,addr
        case 0xC2:
            cond_jump(fetch_pc_word(), ZF_off());
            return 10;

        // .......... jp z,addr
        case 0xCA:
            cond_jump(fetch_pc_word(), ZF_on());
            return 10;

        // .......... jp nc,addr
        case 0xD2:
            cond_jump(fetch_pc_word(), CF_off());
            return 10;

        // .......... jp c,addr
        case 0xDA:
            cond_jump(fetch_pc_word(), CF_on());
            return 10;

        // .......... jp po,addr
        case 0xE2:
            cond_jump(fetch_pc_word(), PF_off());
            return 10;

        // .......... jp pe,addr
        case 0xEA:
            cond_jump(fetch_pc_word(), PF_on());
            return 10;

        // .......... jp p,addr
        case 0xF2:
            cond_jump(fetch_pc_word(), SF_off());
            return 10;

        // .......... jp m,addr
        case 0xFA:
            cond_jump(fetch_pc_word(), SF_on());
            return 10;

        // .......... jr disp
        case 0x18:
            jump_r(fetch_pc_byte<int8_t>());
            return 12;

        // .......... jr c,disp
        case 0x38:
            return cond_jump_r<12, 7>(fetch_pc_byte<int8_t>(), CF_on());

        // .......... jr nc,disp
        case 0x30:
            return cond_jump_r<12, 7>(fetch_pc_byte<int8_t>(), CF_off());

        // .......... jr z,disp
        case 0x28:
            return cond_jump_r<12, 7>(fetch_pc_byte<int8_t>(), ZF_on());

        // .......... jr nz,disp
        case 0x20:
            return cond_jump_r<12, 7>(fetch_pc_byte<int8_t>(), ZF_off());

        // .......... jp (hl)
        case 0xE9:
            jump<false>(_reg.HL);
            return 4;

        // .......... djnz disp
        case 0x10:
            return cond_jump_r<13, 8>(fetch_pc_byte<int8_t>(), --_reg.B != 0);

        // .......... ex de,hl
        case 0xEB:
            std::swap(_reg.DE, _reg.HL);
            return 4;

        // .......... ex af,af'
        case 0x08:
            std::swap(_reg.AF, _sh_reg.AF);
            return 4;

        // .......... exx
        case 0xD9:
            std::swap(_reg.BC, _sh_reg.BC);
            std::swap(_reg.DE, _sh_reg.DE);
            std::swap(_reg.HL, _sh_reg.HL);
            return 4;

        // .......... ex (sp),hl
        case 0xE3:
            swap_stack(_reg.HL);
            return 19;

        // .......... in a,(n)
        case 0xDB:
            _reg.A = _ioports.in(wz_set_plus_one(_word(fetch_pc_byte(), _reg.A)));
            return 11;

        // .......... out (n),a
        case 0xD3:
            _ioports.out(wz_set_plus_one(_word(fetch_pc_byte(), _reg.A), _reg.A), _reg.A);
            return 11;

        // .......... call addr
        case 0xCD:
            call(fetch_pc_word());
            return 17;

        // .......... call nz,addr
        case 0xC4:
            return cond_call<17, 10>(fetch_pc_word(), ZF_off());

        // .......... call z,addr
        case 0xCC:
            return cond_call<17, 10>(fetch_pc_word(), ZF_on());

        // .......... call nc,addr
        case 0xD4:
            return cond_call<17, 10>(fetch_pc_word(), CF_off());

        // .......... call c,addr
        case 0xDC:
            return cond_call<17, 10>(fetch_pc_word(), CF_on());

        // .......... call po,addr
        case 0xE4:
            return cond_call<17, 10>(fetch_pc_word(), PF_off());

        // .......... call pe,addr
        case 0xEC:
            return cond_call<17, 10>(fetch_pc_word(), PF_on());

        // .......... call p,addr
        case 0xF4:
            return cond_call<17, 10>(fetch_pc_word(), SF_off());

        // .......... call m,addr
        case 0xFC:
            return cond_call<17, 10>(fetch_pc_word(), SF_on());

        // .......... ret
        case 0xC9:
            ret();
            return 10;

        // .......... ret nz
        case 0xC0:
            return cond_ret<11, 5>(ZF_off());

        // .......... ret z
        case 0xC8:
            return cond_ret<11, 5>(ZF_on());

        // .......... ret nc
        case 0xD0:
            return cond_ret<11, 5>(CF_off());

        // .......... ret c
        case 0xD8:
            return cond_ret<11, 5>(CF_on());

        // .......... ret po
        case 0xE0:
            return cond_ret<11, 5>(PF_off());

        // .......... ret pe
        case 0xE8:
            return cond_ret<11, 5>(PF_on());

        // .......... ret p
        case 0xF0:
            return cond_ret<11, 5>(SF_off());

        // .......... ret m
        case 0xF8:
            return cond_ret<11, 5>(SF_on());

        // .......... rst 00
        case 0xC7:
            rst<address::rst_00h>();
            return 11;

        // .......... rst 08
        case 0xCF:
            rst<address::rst_08h>();
            return 11;

        // .......... rst 10
        case 0xD7:
            rst<address::rst_10h>();
            return 11;

        // .......... rst 18
        case 0xDF:
            rst<address::rst_18h>();
            return 11;

        // .......... rst 20
        case 0xE7:
            rst<address::rst_20h>();
            return 11;

        // .......... rst 28
        case 0xEF:
            rst<address::rst_28h>();
            return 11;

        // .......... rst 30
        case 0xF7:
            rst<address::rst_30h>();
            return 11;

        // .......... rst 38
        case 0xFF:
            rst<address::rst_38h>();
            return 11;

        // .......... rlca
        case 0x07:
            rlca();
            return 4;

        // .......... rla
        case 0x17:
            rla();
            return 4;

        // .......... rrca
        case 0x0F:
            rrca();
            return 4;

        // .......... rra
        case 0x1F:
            rra();
            return 4;

        // .......... add a,b
        case 0x80:
            add(_reg.B);
            return 4;

        // .......... add a,c
        case 0x81:
            add(_reg.C);
            return 4;

        // .......... add a,d
        case 0x82:
            add(_reg.D);
            return 4;

        // .......... add a,e
        case 0x83:
            add(_reg.E);
            return 4;

        // .......... add a,h
        case 0x84:
            add(_reg.H);
            return 4;

        // .......... add a,l
        case 0x85:
            add(_reg.L);
            return 4;

        // .......... add a,a
        case 0x87:
            add(_reg.A);
            return 4;

        // .......... add a,n
        case 0xC6:
            add(fetch_pc_byte());
            return 7;

        // .......... add a,(hl)
        case 0x86:
            add(peek_byte(_reg.HL));
            return 7;

        // .......... adc a,b
        case 0x88:
            adc(_reg.B);
            return 4;

        // .......... adc a,c
        case 0x89:
            adc(_reg.C);
            return 4;

        // .......... adc a,d
        case 0x8A:
            adc(_reg.D);
            return 4;

        // .......... adc a,e
        case 0x8B:
            adc(_reg.E);
            return 4;

        // .......... adc a,h
        case 0x8C:
            adc(_reg.H);
            return 4;

        // .......... adc a,l
        case 0x8D:
            adc(_reg.L);
            return 4;

        // .......... adc a,a
        case 0x8F:
            adc(_reg.A);
            return 4;

        // .......... adc a,n
        case 0xCE:
            adc(fetch_pc_byte());
            return 7;

        // .......... adc a,(hl)
        case 0x8E:
            adc(peek_byte(_reg.HL));
            return 7;

        // .......... sub a,b
        case 0x90:
            sub(_reg.B);
            return 4;

        // .......... sub a,c
        case 0x91:
            sub(_reg.C);
            return 4;

        // .......... sub a,d
        case 0x92:
            sub(_reg.D);
            return 4;

        // .......... sub a,e
        case 0x93:
            sub(_reg.E);
            return 4;

        // .......... sub a,h
        case 0x94:
            sub(_reg.H);
            return 4;

        // .......... sub a,l
        case 0x95:
            sub(_reg.L);
            return 4;

        // .......... sub a,a
        case 0x97:
            sub(_reg.A);
            return 4;

        // .......... sub a,n
        case 0xD6:
            sub(fetch_pc_byte());
            return 7;

        // .......... sub a,(hl)
        case 0x96:
            sub(peek_byte(_reg.HL));
            return 7;

        // .......... sbc a,b
        case 0x98:
            sbc(_reg.B);
            return 4;

        // .......... sbc a,c
        case 0x99:
            sbc(_reg.C);
            return 4;

        // .......... sbc a,d
        case 0x9A:
            sbc(_reg.D);
            return 4;

        // .......... sbc a,e
        case 0x9B:
            sbc(_reg.E);
            return 4;

        // .......... sbc a,h
        case 0x9C:
            sbc(_reg.H);
            return 4;

        // .......... sbc a,l
        case 0x9D:
            sbc(_reg.L);
            return 4;

        // .......... sbc a,a
        case 0x9F:
            sbc(_reg.A);
            return 4;

        // .......... sbc a,n
        case 0xDE:
            sbc(fetch_pc_byte());
            return 7;

        // .......... sbc a,(hl)
        case 0x9E:
            sbc(peek_byte(_reg.HL));
            return 7;

        // .......... and a,b
        case 0xA0:
            bw_and(_reg.B);
            return 4;

        // .......... and a,c
        case 0xA1:
            bw_and(_reg.C);
            return 4;

        // .......... and a,d
        case 0xA2:
            bw_and(_reg.D);
            return 4;

        // .......... and a,e
        case 0xA3:
            bw_and(_reg.E);
            return 4;

        // .......... and a,h
        case 0xA4:
            bw_and(_reg.H);
            return 4;

        // .......... and a,l
        case 0xA5:
            bw_and(_reg.L);
            return 4;

        // .......... and a,a
        case 0xA7:
            bw_and(_reg.A);
            return 4;

        // .......... and a,n
        case 0xE6:
            bw_and(fetch_pc_byte());
            return 7;

        // .......... and a,(hl)
        case 0xA6:
            bw_and(peek_byte(_reg.HL));
            return 7;

        // .......... or a,b
        case 0xB0:
            bw_or(_reg.B);
            return 4;

        // .......... or a,c
        case 0xB1:
            bw_or(_reg.C);
            return 4;

        // .......... or a,d
        case 0xB2:
            bw_or(_reg.D);
            return 4;

        // .......... or a,e
        case 0xB3:
            bw_or(_reg.E);
            return 4;

        // .......... or a,h
        case 0xB4:
            bw_or(_reg.H);
            return 4;

        // .......... or a,l
        case 0xB5:
            bw_or(_reg.L);
            return 4;

        // .......... or a,a
        case 0xB7:
            bw_or(_reg.A);
            return 4;

        // .......... or a,n
        case 0xF6:
            bw_or(fetch_pc_byte());
            return 7;

        // .......... or a,(hl)
        case 0xB6:
            bw_or(peek_byte(_reg.HL));
            return 7;

        // .......... xor a,b
        case 0xA8:
            bw_xor(_reg.B);
            return 4;

        // .......... xor a,c
        case 0xA9:
            bw_xor(_reg.C);
            return 4;

        // .......... xor a,d
        case 0xAA:
            bw_xor(_reg.D);
            return 4;

        // .......... xor a,e
        case 0xAB:
            bw_xor(_reg.E);
            return 4;

        // .......... xor a,h
        case 0xAC:
            bw_xor(_reg.H);
            return 4;

        // .......... xor a,l
        case 0xAD:
            bw_xor(_reg.L);
            return 4;

        // .......... xor a,a
        case 0xAF:
            bw_xor(_reg.A);
            return 4;

        // .......... xor a,n
        case 0xEE:
            bw_xor(fetch_pc_byte());
            return 7;

        // .......... xor a,(hl)
        case 0xAE:
            bw_xor(peek_byte(_reg.HL));
            return 7;

        // .......... cp a,b
        case 0xB8:
            cp(_reg.B);
            return 4;

        // .......... cp a,c
        case 0xB9:
            cp(_reg.C);
            return 4;

        // .......... cp a,d
        case 0xBA:
            cp(_reg.D);
            return 4;

        // .......... cp a,e
        case 0xBB:
            cp(_reg.E);
            return 4;

        // .......... cp a,h
        case 0xBC:
            cp(_reg.H);
            return 4;

        // .......... cp a,l
        case 0xBD:
            cp(_reg.L);
            return 4;

        // .......... cp a,a
        case 0xBF:
            cp(_reg.A);
            return 4;

        // .......... cp a,n
        case 0xFE:
            cp(fetch_pc_byte());
            return 7;

        // .......... cp a,(hl)
        case 0xBE:
            cp(peek_byte(_reg.HL));
            return 7;

        // .......... inc b
        case 0x04:
            inc(_reg.B);
            return 4;

        // .......... inc c
        case 0x0C:
            inc(_reg.C);
            return 4;

        // .......... inc d
        case 0x14:
            inc(_reg.D);
            return 4;

        // .......... inc e
        case 0x1C:
            inc(_reg.E);
            return 4;

        // .......... inc h
        case 0x24:
            inc(_reg.H);
            return 4;

        // .......... inc l
        case 0x2C:
            inc(_reg.L);
            return 4;

        // .......... inc a
        case 0x3C:
            inc(_reg.A);
            return 4;

        // .......... inc (hl)
        case 0x34:
            modify_at<&Z80::inc>(_reg.HL);
            return 11;

        // .......... dec b
        case 0x05:
            dec(_reg.B);
            return 4;

        // .......... dec c
        case 0x0D:
            dec(_reg.C);
            return 4;

        // .......... dec d
        case 0x15:
            dec(_reg.D);
            return 4;

        // .......... dec e
        case 0x1D:
            dec(_reg.E);
            return 4;

        // .......... dec h
        case 0x25:
            dec(_reg.H);
            return 4;

        // .......... dec l
        case 0x2D:
            dec(_reg.L);
            return 4;

        // .......... dec a
        case 0x3D:
            dec(_reg.A);
            return 4;

        // .......... dec (hl)
        case 0x35:
            modify_at<&Z80::dec>(_reg.HL);
            return 11;

        // .......... add hl,bc
        case 0x09:
            add(_reg.HL, _reg.BC);
            return 11;

        // .......... add hl,de
        case 0x19:
            add(_reg.HL, _reg.DE);
            return 11;

        // .......... add hl,hl
        case 0x29:
            add(_reg.HL, _reg.HL);
            return 11;

        // .......... add hl,sp
        case 0x39:
            add(_reg.HL, _sp_reg.SP);
            return 11;

        // .......... inc bc
        case 0x03:
            ++_reg.BC;
            return 6;

        // .......... inc de
        case 0x13:
            ++_reg.DE;
            return 6;

        // .......... inc hl
        case 0x23:
            ++_reg.HL;
            return 6;

        // .......... inc sp
        case 0x33:
            ++_sp_reg.SP;
            return 6;

        // .......... dec bc
        case 0x0B:
            --_reg.BC;
            return 6;

        // .......... dec de
        case 0x1B:
            --_reg.DE;
            return 6;

        // .......... dec hl
        case 0x2B:
            --_reg.HL;
            return 6;

        // .......... dec sp
        case 0x3B:
            --_sp_reg.SP;
            return 6;

        // .......... daa
        case 0x27:
            daa();
            return 4;

        // .......... cpl
        case 0x2F:
            cpl();
            return 4;

        // .......... ccf
        case 0x3F:
            ccf();
            return 4;

        // .......... scf
        case 0x37:
            scf();
            return 4;

        // .......... halt
        case 0x76:
            _halted = true;
            return 4;

        // .......... ei
        case 0xFB:
            enable_int();
            return 4;

        // .......... di
        case 0xF3:
            disable_int();
            return 4;

        // .......... prefix CB
        case prefix::CB:
            return exec_prefix_cb();

        // .......... prefix ED
        case prefix::ED:
            return exec_prefix_ed();

        // .......... prefix DD
        case prefix::DD:
            return exec_prefix_xd(_ix_reg.IX);

        // .......... prefix FD
        case prefix::FD:
            return exec_prefix_xd(_ix_reg.IY);

        default:
            std::unreachable();
    }

}

//#################################################################################################

int Z80::exec_prefix_cb() noexcept {

    inc_refresh_register();

    switch (fetch_pc_byte()) {

        // .......... rlc b
        case 0x00:
            rlc(_reg.B);
            return 8;

        // .......... rlc c
        case 0x01:
            rlc(_reg.C);
            return 8;

        // .......... rlc d
        case 0x02:
            rlc(_reg.D);
            return 8;

        // .......... rlc e
        case 0x03:
            rlc(_reg.E);
            return 8;

        // .......... rlc h
        case 0x04:
            rlc(_reg.H);
            return 8;

        // .......... rlc l
        case 0x05:
            rlc(_reg.L);
            return 8;

        // .......... rlc (hl)
        case 0x06:
            modify_at<&Z80::rlc>(_reg.HL);
            return 15;

        // .......... rlc a
        case 0x07:
            rlc(_reg.A);
            return 8;

        // .......... rrc b
        case 0x08:
            rrc(_reg.B);
            return 8;

        // .......... rrc c
        case 0x09:
            rrc(_reg.C);
            return 8;

        // .......... rrc d
        case 0x0A:
            rrc(_reg.D);
            return 8;

        // .......... rrc e
        case 0x0B:
            rrc(_reg.E);
            return 8;

        // .......... rrc h
        case 0x0C:
            rrc(_reg.H);
            return 8;

        // .......... rrc l
        case 0x0D:
            rrc(_reg.L);
            return 8;

        // .......... rrc (hl)
        case 0x0E:
            modify_at<&Z80::rrc>(_reg.HL);
            return 15;

        // .......... rrc a
        case 0x0F:
            rrc(_reg.A);
            return 8;

        // .......... rl b
        case 0x10:
            rl(_reg.B);
            return 8;

        // .......... rl c
        case 0x11:
            rl(_reg.C);
            return 8;

        // .......... rl d
        case 0x12:
            rl(_reg.D);
            return 8;

        // .......... rl e
        case 0x13:
            rl(_reg.E);
            return 8;

        // .......... rl h
        case 0x14:
            rl(_reg.H);
            return 8;

        // .......... rl l
        case 0x15:
            rl(_reg.L);
            return 8;

        // .......... rl (hl)
        case 0x16:
            modify_at<&Z80::rl>(_reg.HL);
            return 15;

        // .......... rl a
        case 0x17:
            rl(_reg.A);
            return 8;

        // .......... rr b
        case 0x18:
            rr(_reg.B);
            return 8;

        // .......... rr c
        case 0x19:
            rr(_reg.C);
            return 8;

        // .......... rr d
        case 0x1A:
            rr(_reg.D);
            return 8;

        // .......... rr e
        case 0x1B:
            rr(_reg.E);
            return 8;

        // .......... rr h
        case 0x1C:
            rr(_reg.H);
            return 8;

        // .......... rr l
        case 0x1D:
            rr(_reg.L);
            return 8;

        // .......... rr (hl)
        case 0x1E:
            modify_at<&Z80::rr>(_reg.HL);
            return 15;

        // .......... rr a
        case 0x1F:
            rr(_reg.A);
            return 8;

        // .......... sla b
        case 0x20:
            sla(_reg.B);
            return 8;

        // .......... sla c
        case 0x21:
            sla(_reg.C);
            return 8;

        // .......... sla d
        case 0x22:
            sla(_reg.D);
            return 8;

        // .......... sla e
        case 0x23:
            sla(_reg.E);
            return 8;

        // .......... sla h
        case 0x24:
            sla(_reg.H);
            return 8;

        // .......... sla l
        case 0x25:
            sla(_reg.L);
            return 8;

        // .......... sla (hl)
        case 0x26:
            modify_at<&Z80::sla>(_reg.HL);
            return 15;

        // .......... sla a
        case 0x27:
            sla(_reg.A);
            return 8;

        // .......... sra b
        case 0x28:
            sra(_reg.B);
            return 8;

        // .......... sra c
        case 0x29:
            sra(_reg.C);
            return 8;

        // .......... sra d
        case 0x2A:
            sra(_reg.D);
            return 8;

        // .......... sra e
        case 0x2B:
            sra(_reg.E);
            return 8;

        // .......... sra h
        case 0x2C:
            sra(_reg.H);
            return 8;

        // .......... sra l
        case 0x2D:
            sra(_reg.L);
            return 8;

        // .......... sra (hl)
        case 0x2E:
            modify_at<&Z80::sra>(_reg.HL);
            return 15;

        // .......... sra a
        case 0x2F:
            sra(_reg.A);
            return 8;

        // .......... sli b
        case 0x30:
            sli(_reg.B);
            return 8;

        // .......... sli c
        case 0x31:
            sli(_reg.C);
            return 8;

        // .......... sli d
        case 0x32:
            sli(_reg.D);
            return 8;

        // .......... sli e
        case 0x33:
            sli(_reg.E);
            return 8;

        // .......... sli h
        case 0x34:
            sli(_reg.H);
            return 8;

        // .......... sli l
        case 0x35:
            sli(_reg.L);
            return 8;

        // .......... sli (hl)
        case 0x36:
            modify_at<&Z80::sli>(_reg.HL);
            return 15;

        // .......... sli a
        case 0x37:
            sli(_reg.A);
            return 8;

        // .......... srl b
        case 0x38:
            srl(_reg.B);
            return 8;

        // .......... srl c
        case 0x39:
            srl(_reg.C);
            return 8;

        // .......... srl d
        case 0x3A:
            srl(_reg.D);
            return 8;

        // .......... srl e
        case 0x3B:
            srl(_reg.E);
            return 8;

        // .......... srl h
        case 0x3C:
            srl(_reg.H);
            return 8;

        // .......... srl l
        case 0x3D:
            srl(_reg.L);
            return 8;

        // .......... srl (hl)
        case 0x3E:
            modify_at<&Z80::srl>(_reg.HL);
            return 15;

        // .......... srl a
        case 0x3F:
            srl(_reg.A);
            return 8;

        // .......... bit 0,b
        case 0x40:
            bit<0>(_reg.B);
            return 8;

        // .......... bit 0,c
        case 0x41:
            bit<0>(_reg.C);
            return 8;

        // .......... bit 0,d
        case 0x42:
            bit<0>(_reg.D);
            return 8;

        // .......... bit 0,e
        case 0x43:
            bit<0>(_reg.E);
            return 8;

        // .......... bit 0,h
        case 0x44:
            bit<0>(_reg.H);
            return 8;

        // .......... bit 0,l
        case 0x45:
            bit<0>(_reg.L);
            return 8;

        // .......... bit 0,(hl)
        case 0x46:
            bit<0>(peek_byte(_reg.HL), _sp_reg.WZ);
            return 15;

        // .......... bit 0,a
        case 0x47:
            bit<0>(_reg.A);
            return 8;

        // .......... bit 1,b
        case 0x48:
            bit<1>(_reg.B);
            return 8;

        // .......... bit 1,c
        case 0x49:
            bit<1>(_reg.C);
            return 8;

        // .......... bit 1,d
        case 0x4A:
            bit<1>(_reg.D);
            return 8;

        // .......... bit 1,e
        case 0x4B:
            bit<1>(_reg.E);
            return 8;

        // .......... bit 1,h
        case 0x4C:
            bit<1>(_reg.H);
            return 8;

        // .......... bit 1,l
        case 0x4D:
            bit<1>(_reg.L);
            return 8;

        // .......... bit 1,(hl)
        case 0x4E:
            bit<1>(peek_byte(_reg.HL), _sp_reg.WZ);
            return 15;

        // .......... bit 1,a
        case 0x4F:
            bit<1>(_reg.A);
            return 8;

        // .......... bit 2,b
        case 0x50:
            bit<2>(_reg.B);
            return 8;

        // .......... bit 2,c
        case 0x51:
            bit<2>(_reg.C);
            return 8;

        // .......... bit 2,d
        case 0x52:
            bit<2>(_reg.D);
            return 8;

        // .......... bit 2,e
        case 0x53:
            bit<2>(_reg.E);
            return 8;

        // .......... bit 2,h
        case 0x54:
            bit<2>(_reg.H);
            return 8;

        // .......... bit 2,l
        case 0x55:
            bit<2>(_reg.L);
            return 8;

        // .......... bit 2,(hl)
        case 0x56:
            bit<2>(peek_byte(_reg.HL), _sp_reg.WZ);
            return 15;

        // .......... bit 2,a
        case 0x57:
            bit<2>(_reg.A);
            return 8;

        // .......... bit 3,b
        case 0x58:
            bit<3>(_reg.B);
            return 8;

        // .......... bit 3,c
        case 0x59:
            bit<3>(_reg.C);
            return 8;

        // .......... bit 3,d
        case 0x5A:
            bit<3>(_reg.D);
            return 8;

        // .......... bit 3,e
        case 0x5B:
            bit<3>(_reg.E);
            return 8;

        // .......... bit 3,h
        case 0x5C:
            bit<3>(_reg.H);
            return 8;

        // .......... bit 3,l
        case 0x5D:
            bit<3>(_reg.L);
            return 8;

        // .......... bit 3,(hl)
        case 0x5E:
            bit<3>(peek_byte(_reg.HL), _sp_reg.WZ);
            return 15;

        // .......... bit 3,a
        case 0x5F:
            bit<3>(_reg.A);
            return 8;

        // .......... bit 4,b
        case 0x60:
            bit<4>(_reg.B);
            return 8;

        // .......... bit 4,c
        case 0x61:
            bit<4>(_reg.C);
            return 8;

        // .......... bit 4,d
        case 0x62:
            bit<4>(_reg.D);
            return 8;

        // .......... bit 4,e
        case 0x63:
            bit<4>(_reg.E);
            return 8;

        // .......... bit 4,h
        case 0x64:
            bit<4>(_reg.H);
            return 8;

        // .......... bit 4,l
        case 0x65:
            bit<4>(_reg.L);
            return 8;

        // .......... bit 4,(hl)
        case 0x66:
            bit<4>(peek_byte(_reg.HL), _sp_reg.WZ);
            return 15;

        // .......... bit 4,a
        case 0x67:
            bit<4>(_reg.A);
            return 8;

        // .......... bit 5,b
        case 0x68:
            bit<5>(_reg.B);
            return 8;

        // .......... bit 5,c
        case 0x69:
            bit<5>(_reg.C);
            return 8;

        // .......... bit 5,d
        case 0x6A:
            bit<5>(_reg.D);
            return 8;

        // .......... bit 5,e
        case 0x6B:
            bit<5>(_reg.E);
            return 8;

        // .......... bit 5,h
        case 0x6C:
            bit<5>(_reg.H);
            return 8;

        // .......... bit 5,l
        case 0x6D:
            bit<5>(_reg.L);
            return 8;

        // .......... bit 5,(hl)
        case 0x6E:
            bit<5>(peek_byte(_reg.HL), _sp_reg.WZ);
            return 15;

        // .......... bit 5,a
        case 0x6F:
            bit<5>(_reg.A);
            return 8;

        // .......... bit 6,b
        case 0x70:
            bit<6>(_reg.B);
            return 8;

        // .......... bit 6,c
        case 0x71:
            bit<6>(_reg.C);
            return 8;

        // .......... bit 6,d
        case 0x72:
            bit<6>(_reg.D);
            return 8;

        // .......... bit 6,e
        case 0x73:
            bit<6>(_reg.E);
            return 8;

        // .......... bit 6,h
        case 0x74:
            bit<6>(_reg.H);
            return 8;

        // .......... bit 6,l
        case 0x75:
            bit<6>(_reg.L);
            return 8;

        // .......... bit 6,(hl)
        case 0x76:
            bit<6>(peek_byte(_reg.HL), _sp_reg.WZ);
            return 15;

        // .......... bit 6,a
        case 0x77:
            bit<6>(_reg.A);
            return 8;

        // .......... bit 7,b
        case 0x78:
            bit<7>(_reg.B);
            return 8;

        // .......... bit 7,c
        case 0x79:
            bit<7>(_reg.C);
            return 8;

        // .......... bit 7,d
        case 0x7A:
            bit<7>(_reg.D);
            return 8;

        // .......... bit 7,e
        case 0x7B:
            bit<7>(_reg.E);
            return 8;

        // .......... bit 7,h
        case 0x7C:
            bit<7>(_reg.H);
            return 8;

        // .......... bit 7,l
        case 0x7D:
            bit<7>(_reg.L);
            return 8;

        // .......... bit 7,(hl)
        case 0x7E:
            bit<7>(peek_byte(_reg.HL), _sp_reg.WZ);
            return 15;

        // .......... bit 7,a
        case 0x7F:
            bit<7>(_reg.A);
            return 8;

        // .......... res 0,b
        case 0x80:
            res<0>(_reg.B);
            return 8;

        // .......... res 0,c
        case 0x81:
            res<0>(_reg.C);
            return 8;

        // .......... res 0,d
        case 0x82:
            res<0>(_reg.D);
            return 8;

        // .......... res 0,e
        case 0x83:
            res<0>(_reg.E);
            return 8;

        // .......... res 0,h
        case 0x84:
            res<0>(_reg.H);
            return 8;

        // .......... res 0,l
        case 0x85:
            res<0>(_reg.L);
            return 8;

        // .......... res 0,(hl)
        case 0x86:
            modify_at<&Z80::res<0>>(_reg.HL);
            return 15;

        // .......... res 0,a
        case 0x87:
            res<0>(_reg.A);
            return 8;

        // .......... res 1,b
        case 0x88:
            res<1>(_reg.B);
            return 8;

        // .......... res 1,c
        case 0x89:
            res<1>(_reg.C);
            return 8;

        // .......... res 1,d
        case 0x8A:
            res<1>(_reg.D);
            return 8;

        // .......... res 1,e
        case 0x8B:
            res<1>(_reg.E);
            return 8;

        // .......... res 1,h
        case 0x8C:
            res<1>(_reg.H);
            return 8;

        // .......... res 1,l
        case 0x8D:
            res<1>(_reg.L);
            return 8;

        // .......... res 1,(hl)
        case 0x8E:
            modify_at<&Z80::res<1>>(_reg.HL);
            return 15;

        // .......... res 1,a
        case 0x8F:
            res<1>(_reg.A);
            return 8;

        // .......... res 2,b
        case 0x90:
            res<2>(_reg.B);
            return 8;

        // .......... res 2,c
        case 0x91:
            res<2>(_reg.C);
            return 8;

        // .......... res 2,d
        case 0x92:
            res<2>(_reg.D);
            return 8;

        // .......... res 2,e
        case 0x93:
            res<2>(_reg.E);
            return 8;

        // .......... res 2,h
        case 0x94:
            res<2>(_reg.H);
            return 8;

        // .......... res 2,l
        case 0x95:
            res<2>(_reg.L);
            return 8;

        // .......... res 2,(hl)
        case 0x96:
            modify_at<&Z80::res<2>>(_reg.HL);
            return 15;

        // .......... res 2,a
        case 0x97:
            res<2>(_reg.A);
            return 8;

        // .......... res 3,b
        case 0x98:
            res<3>(_reg.B);
            return 8;

        // .......... res 3,c
        case 0x99:
            res<3>(_reg.C);
            return 8;

        // .......... res 3,d
        case 0x9A:
            res<3>(_reg.D);
            return 8;

        // .......... res 3,e
        case 0x9B:
            res<3>(_reg.E);
            return 8;

        // .......... res 3,h
        case 0x9C:
            res<3>(_reg.H);
            return 8;

        // .......... res 3,l
        case 0x9D:
            res<3>(_reg.L);
            return 8;

        // .......... res 3,(hl)
        case 0x9E:
            modify_at<&Z80::res<3>>(_reg.HL);
            return 15;

        // .......... res 3,a
        case 0x9F:
            res<3>(_reg.A);
            return 8;

        // .......... res 4,b
        case 0xA0:
            res<4>(_reg.B);
            return 8;

        // .......... res 4,c
        case 0xA1:
            res<4>(_reg.C);
            return 8;

        // .......... res 4,d
        case 0xA2:
            res<4>(_reg.D);
            return 8;

        // .......... res 4,e
        case 0xA3:
            res<4>(_reg.E);
            return 8;

        // .......... res 4,h
        case 0xA4:
            res<4>(_reg.H);
            return 8;

        // .......... res 4,l
        case 0xA5:
            res<4>(_reg.L);
            return 8;

        // .......... res 4,(hl)
        case 0xA6:
            modify_at<&Z80::res<4>>(_reg.HL);
            return 15;

        // .......... res 4,a
        case 0xA7:
            res<4>(_reg.A);
            return 8;

        // .......... res 5,b
        case 0xA8:
            res<5>(_reg.B);
            return 8;

        // .......... res 5,c
        case 0xA9:
            res<5>(_reg.C);
            return 8;

        // .......... res 5,d
        case 0xAA:
            res<5>(_reg.D);
            return 8;

        // .......... res 5,e
        case 0xAB:
            res<5>(_reg.E);
            return 8;

        // .......... res 5,h
        case 0xAC:
            res<5>(_reg.H);
            return 8;

        // .......... res 5,l
        case 0xAD:
            res<5>(_reg.L);
            return 8;

        // .......... res 5,(hl)
        case 0xAE:
            modify_at<&Z80::res<5>>(_reg.HL);
            return 15;

        // .......... res 5,a
        case 0xAF:
            res<5>(_reg.A);
            return 8;

        // .......... res 6,b
        case 0xB0:
            res<6>(_reg.B);
            return 8;

        // .......... res 6,c
        case 0xB1:
            res<6>(_reg.C);
            return 8;

        // .......... res 6,d
        case 0xB2:
            res<6>(_reg.D);
            return 8;

        // .......... res 6,e
        case 0xB3:
            res<6>(_reg.E);
            return 8;

        // .......... res 6,h
        case 0xB4:
            res<6>(_reg.H);
            return 8;

        // .......... res 6,l
        case 0xB5:
            res<6>(_reg.L);
            return 8;

        // .......... res 6,(hl)
        case 0xB6:
            modify_at<&Z80::res<6>>(_reg.HL);
            return 15;

        // .......... res 6,a
        case 0xB7:
            res<6>(_reg.A);
            return 8;

        // .......... res 7,b
        case 0xB8:
            res<7>(_reg.B);
            return 8;

        // .......... res 7,c
        case 0xB9:
            res<7>(_reg.C);
            return 8;

        // .......... res 7,d
        case 0xBA:
            res<7>(_reg.D);
            return 8;

        // .......... res 7,e
        case 0xBB:
            res<7>(_reg.E);
            return 8;

        // .......... res 7,h
        case 0xBC:
            res<7>(_reg.H);
            return 8;

        // .......... res 7,l
        case 0xBD:
            res<7>(_reg.L);
            return 8;

        // .......... res 7,(hl)
        case 0xBE:
            modify_at<&Z80::res<7>>(_reg.HL);
            return 15;

        // .......... res 7,a
        case 0xBF:
            res<7>(_reg.A);
            return 8;

        // .......... set 0,b
        case 0xC0:
            set<0>(_reg.B);
            return 8;

        // .......... set 0,c
        case 0xC1:
            set<0>(_reg.C);
            return 8;

        // .......... set 0,d
        case 0xC2:
            set<0>(_reg.D);
            return 8;

        // .......... set 0,e
        case 0xC3:
            set<0>(_reg.E);
            return 8;

        // .......... set 0,h
        case 0xC4:
            set<0>(_reg.H);
            return 8;

        // .......... set 0,l
        case 0xC5:
            set<0>(_reg.L);
            return 8;

        // .......... set 0,(hl)
        case 0xC6:
            modify_at<&Z80::set<0>>(_reg.HL);
            return 15;

        // .......... set 0,a
        case 0xC7:
            set<0>(_reg.A);
            return 8;

        // .......... set 1,b
        case 0xC8:
            set<1>(_reg.B);
            return 8;

        // .......... set 1,c
        case 0xC9:
            set<1>(_reg.C);
            return 8;

        // .......... set 1,d
        case 0xCA:
            set<1>(_reg.D);
            return 8;

        // .......... set 1,e
        case 0xCB:
            set<1>(_reg.E);
            return 8;

        // .......... set 1,h
        case 0xCC:
            set<1>(_reg.H);
            return 8;

        // .......... set 1,l
        case 0xCD:
            set<1>(_reg.L);
            return 8;

        // .......... set 1,(hl)
        case 0xCE:
            modify_at<&Z80::set<1>>(_reg.HL);
            return 15;

        // .......... set 1,a
        case 0xCF:
            set<1>(_reg.A);
            return 8;

        // .......... set 2,b
        case 0xD0:
            set<2>(_reg.B);
            return 8;

        // .......... set 2,c
        case 0xD1:
            set<2>(_reg.C);
            return 8;

        // .......... set 2,d
        case 0xD2:
            set<2>(_reg.D);
            return 8;

        // .......... set 2,e
        case 0xD3:
            set<2>(_reg.E);
            return 8;

        // .......... set 2,h
        case 0xD4:
            set<2>(_reg.H);
            return 8;

        // .......... set 2,l
        case 0xD5:
            set<2>(_reg.L);
            return 8;

        // .......... set 2,(hl)
        case 0xD6:
            modify_at<&Z80::set<2>>(_reg.HL);
            return 15;

        // .......... set 2,a
        case 0xD7:
            set<2>(_reg.A);
            return 8;

        // .......... set 3,b
        case 0xD8:
            set<3>(_reg.B);
            return 8;

        // .......... set 3,c
        case 0xD9:
            set<3>(_reg.C);
            return 8;

        // .......... set 3,d
        case 0xDA:
            set<3>(_reg.D);
            return 8;

        // .......... set 3,e
        case 0xDB:
            set<3>(_reg.E);
            return 8;

        // .......... set 3,h
        case 0xDC:
            set<3>(_reg.H);
            return 8;

        // .......... set 3,l
        case 0xDD:
            set<3>(_reg.L);
            return 8;

        // .......... set 3,(hl)
        case 0xDE:
            modify_at<&Z80::set<3>>(_reg.HL);
            return 15;

        // .......... set 3,a
        case 0xDF:
            set<3>(_reg.A);
            return 8;

        // .......... set 4,b
        case 0xE0:
            set<4>(_reg.B);
            return 8;

        // .......... set 4,c
        case 0xE1:
            set<4>(_reg.C);
            return 8;

        // .......... set 4,d
        case 0xE2:
            set<4>(_reg.D);
            return 8;

        // .......... set 4,e
        case 0xE3:
            set<4>(_reg.E);
            return 8;

        // .......... set 4,h
        case 0xE4:
            set<4>(_reg.H);
            return 8;

        // .......... set 4,l
        case 0xE5:
            set<4>(_reg.L);
            return 8;

        // .......... set 4,(hl)
        case 0xE6:
            modify_at<&Z80::set<4>>(_reg.HL);
            return 15;

        // .......... set 4,a
        case 0xE7:
            set<4>(_reg.A);
            return 8;

        // .......... set 5,b
        case 0xE8:
            set<5>(_reg.B);
            return 8;

        // .......... set 5,c
        case 0xE9:
            set<5>(_reg.C);
            return 8;

        // .......... set 5,d
        case 0xEA:
            set<5>(_reg.D);
            return 8;

        // .......... set 5,e
        case 0xEB:
            set<5>(_reg.E);
            return 8;

        // .......... set 5,h
        case 0xEC:
            set<5>(_reg.H);
            return 8;

        // .......... set 5,l
        case 0xED:
            set<5>(_reg.L);
            return 8;

        // .......... set 5,(hl)
        case 0xEE:
            modify_at<&Z80::set<5>>(_reg.HL);
            return 15;

        // .......... set 5,a
        case 0xEF:
            set<5>(_reg.A);
            return 8;

        // .......... set 6,b
        case 0xF0:
            set<6>(_reg.B);
            return 8;

        // .......... set 6,c
        case 0xF1:
            set<6>(_reg.C);
            return 8;

        // .......... set 6,d
        case 0xF2:
            set<6>(_reg.D);
            return 8;

        // .......... set 6,e
        case 0xF3:
            set<6>(_reg.E);
            return 8;

        // .......... set 6,h
        case 0xF4:
            set<6>(_reg.H);
            return 8;

        // .......... set 6,l
        case 0xF5:
            set<6>(_reg.L);
            return 8;

        // .......... set 6,(hl)
        case 0xF6:
            modify_at<&Z80::set<6>>(_reg.HL);
            return 15;

        // .......... set 6,a
        case 0xF7:
            set<6>(_reg.A);
            return 8;

        // .......... set 7,b
        case 0xF8:
            set<7>(_reg.B);
            return 8;

        // .......... set 7,c
        case 0xF9:
            set<7>(_reg.C);
            return 8;

        // .......... set 7,d
        case 0xFA:
            set<7>(_reg.D);
            return 8;

        // .......... set 7,e
        case 0xFB:
            set<7>(_reg.E);
            return 8;

        // .......... set 7,h
        case 0xFC:
            set<7>(_reg.H);
            return 8;

        // .......... set 7,l
        case 0xFD:
            set<7>(_reg.L);
            return 8;

        // .......... set 7,(hl)
        case 0xFE:
            modify_at<&Z80::set<7>>(_reg.HL);
            return 15;

        // .......... set 7,a
        case 0xFF:
            set<7>(_reg.A);
            return 8;

        default:
            std::unreachable();
    }
}

//#################################################################################################

int Z80::exec_prefix_ed() noexcept {

    inc_refresh_register();

    switch (fetch_pc_byte()) {

        // .......... in b,(c)
        case 0x40:
            in_reg<_B_>();
            return 12;

        // .......... in c,(c)
        case 0x48:
            in_reg<_C_>();
            return 12;

        // .......... in d,(c)
        case 0x50:
            in_reg<_D_>();
            return 12;

        // .......... in e,(c)
        case 0x58:
            in_reg<_E_>();
            return 12;

        // .......... in h,(c)
        case 0x60:
            in_reg<_H_>();
            return 12;

        // .......... in l,(c)
        case 0x68:
            in_reg<_L_>();
            return 12;

        // .......... in a,(c)
        case 0x78:
            in_reg<_A_>();
            return 12;

        // .......... inf
        case 0x70:
            in_reg<__>();
            return 12;

        // .......... out (c),b
        case 0x41:
            out_byte(_reg.B);
            return 12;

        // .......... out (c),c
        case 0x49:
            out_byte(_reg.C);
            return 12;

        // .......... out (c),d
        case 0x51:
            out_byte(_reg.D);
            return 12;

        // .......... out (c),e
        case 0x59:
            out_byte(_reg.E);
            return 12;

        // .......... out (c),h
        case 0x61:
            out_byte(_reg.H);
            return 12;

        // .......... out (c),l
        case 0x69:
            out_byte(_reg.L);
            return 12;

        // .......... out (c),0
        case 0x71:
            out_byte(0);
            return 12;

        // .......... out (c),a
        case 0x79:
            out_byte(_reg.A);
            return 12;

        // .......... sbc hl,bc
        case 0x42:
            sbc(_reg.HL, _reg.BC);
            return 15;

        // .......... sbc hl,de
        case 0x52:
            sbc(_reg.HL, _reg.DE);
            return 15;

        // .......... sbc hl,hl
        case 0x62:
            sbc(_reg.HL, _reg.HL);
            return 15;

        // .......... sbc hl,sp
        case 0x72:
            sbc(_reg.HL, _sp_reg.SP);
            return 15;

        // .......... adc hl,bc
        case 0x4A:
            adc(_reg.HL, _reg.BC);
            return 15;

        // .......... adc hl,de
        case 0x5A:
            adc(_reg.HL, _reg.DE);
            return 15;

        // .......... adc hl,hl
        case 0x6A:
            adc(_reg.HL, _reg.HL);
            return 15;

        // .......... adc hl,sp
        case 0x7A:
            adc(_reg.HL, _sp_reg.SP);
            return 15;

        // .......... ld (nn),bc
        case 0x43:
            poke_word(wz_set_plus_one(fetch_pc_word()), _reg.BC);
            return 20;

        // .......... ld (nn),de
        case 0x53:
            poke_word(wz_set_plus_one(fetch_pc_word()), _reg.DE);
            return 20;

        // .......... ld (nn),hl
        case 0x63:
            poke_word(wz_set_plus_one(fetch_pc_word()), _reg.HL);
            return 20;

        // .......... ld (nn),sp
        case 0x73:
            poke_word(wz_set_plus_one(fetch_pc_word()), _sp_reg.SP);
            return 20;

        // .......... ld bc,(nn)
        case 0x4B:
            _reg.BC = peek_word(wz_set_plus_one(fetch_pc_word()));
            return 20;

        // .......... ld de,(nn)
        case 0x5B:
            _reg.DE = peek_word(wz_set_plus_one(fetch_pc_word()));
            return 20;

        // .......... ld hl,(nn)
        case 0x6B:
            _reg.HL = peek_word(wz_set_plus_one(fetch_pc_word()));
            return 20;

        // .......... ld sp,(nn)
        case 0x7B:
            _sp_reg.SP = peek_word(wz_set_plus_one(fetch_pc_word()));
            return 20;

        // .......... ld i,a
        case 0x47:
            _sp_reg.I = _reg.A;
            return 9;

        // .......... ld r,a
        case 0x4F:
            _sp_reg.R = _reg.A;
            return 9;

        // .......... ld a,i
        case 0x57:
            ld_a_ir(_sp_reg.I);
            return 9;

        // .......... ld a,r
        case 0x5F:
            ld_a_ir(_sp_reg.R);
            return 9;

        // .......... im 0
        case 0x46:
        case 0x4E:
        case 0x66:
        case 0x6E:
            _imode = IMode::IM0;
            return 8;

        // .......... im 1
        case 0x56:
        case 0x76:
            _imode = IMode::IM1;
            return 8;

        // .......... im 2
        case 0x5E:
        case 0x7E:
            _imode = IMode::IM2;
            return 8;

        // .......... retn
        case 0x45:
        case 0x55:
        case 0x65:
        case 0x75:
            ret_nmi();
            return 14;

        // .......... reti
        case 0x4D:
        case 0x5D:
        case 0x6D:
        case 0x7D:
            ret_int();
            return 14;

        // .......... neg
        case 0x44:
        case 0x4C:
        case 0x54:
        case 0x5C:
        case 0x64:
        case 0x6C:
        case 0x74:
        case 0x7C:
            neg();
            return 8;

        // .......... rrd
        case 0x67:
            rrd();
            return 18;

        // .......... rld
        case 0x6F:
            rld();
            return 18;

        // .......... ldi
        case 0xA0:
            ldi();
            return 16;

        // .......... ldd
        case 0xA8:
            ldd();
            return 16;

        // .......... ldir
        case 0xB0:
            return cond_rep<&Z80::ldi<false>, 21, 16>();

        // .......... lddr
        case 0xB8:
            return cond_rep<&Z80::ldd<false>, 21, 16>();

        // .......... cpi
        case 0xA1:
            cpi();
            return 16;

        // .......... cpd
        case 0xA9:
            cpd();
            return 16;

        // .......... cpir
        case 0xB1:
            return cond_rep<&Z80::cpi<false>, 21, 16>();

        // .......... cpdr
        case 0xB9:
            return cond_rep<&Z80::cpd<false>, 21, 16>();

        // .......... ini
        case 0xA2:
            ini();
            return 16;

        // .......... ind
        case 0xAA:
            ind();
            return 16;

        // .......... inir
        case 0xB2:
            return cond_rep<&Z80::ini, 21, 16>();

        // .......... indr
        case 0xBA:
            return cond_rep<&Z80::ind, 21, 16>();

        // .......... outi
        case 0xA3:
            outi();
            return 16;

        // .......... outd
        case 0xAB:
            outd();
            return 16;

        // .......... otir
        case 0xB3:
            return cond_rep<&Z80::outi, 21, 16>();

        // .......... otdr
        case 0xBB:
            return cond_rep<&Z80::outd, 21, 16>();

        // .......... *nop
        case 0x00:

        // .......... *nop
        case 0x01:

        // .......... *nop
        case 0x02:

        // .......... *nop
        case 0x03:

        // .......... *nop
        case 0x04:

        // .......... *nop
        case 0x05:

        // .......... *nop
        case 0x06:

        // .......... *nop
        case 0x07:

        // .......... *nop
        case 0x08:

        // .......... *nop
        case 0x09:

        // .......... *nop
        case 0x0A:

        // .......... *nop
        case 0x0B:

        // .......... *nop
        case 0x0C:

        // .......... *nop
        case 0x0D:

        // .......... *nop
        case 0x0E:

        // .......... *nop
        case 0x0F:

        // .......... *nop
        case 0x10:

        // .......... *nop
        case 0x11:

        // .......... *nop
        case 0x12:

        // .......... *nop
        case 0x13:

        // .......... *nop
        case 0x14:

        // .......... *nop
        case 0x15:

        // .......... *nop
        case 0x16:

        // .......... *nop
        case 0x17:

        // .......... *nop
        case 0x18:

        // .......... *nop
        case 0x19:

        // .......... *nop
        case 0x1A:

        // .......... *nop
        case 0x1B:

        // .......... *nop
        case 0x1C:

        // .......... *nop
        case 0x1D:

        // .......... *nop
        case 0x1E:

        // .......... *nop
        case 0x1F:

        // .......... *nop
        case 0x20:

        // .......... *nop
        case 0x21:

        // .......... *nop
        case 0x22:

        // .......... *nop
        case 0x23:

        // .......... *nop
        case 0x24:

        // .......... *nop
        case 0x25:

        // .......... *nop
        case 0x26:

        // .......... *nop
        case 0x27:

        // .......... *nop
        case 0x28:

        // .......... *nop
        case 0x29:

        // .......... *nop
        case 0x2A:

        // .......... *nop
        case 0x2B:

        // .......... *nop
        case 0x2C:

        // .......... *nop
        case 0x2D:

        // .......... *nop
        case 0x2E:

        // .......... *nop
        case 0x2F:

        // .......... *nop
        case 0x30:

        // .......... *nop
        case 0x31:

        // .......... *nop
        case 0x32:

        // .......... *nop
        case 0x33:

        // .......... *nop
        case 0x34:

        // .......... *nop
        case 0x35:

        // .......... *nop
        case 0x36:

        // .......... *nop
        case 0x37:

        // .......... *nop
        case 0x38:

        // .......... *nop
        case 0x39:

        // .......... *nop
        case 0x3A:

        // .......... *nop
        case 0x3B:

        // .......... *nop
        case 0x3C:

        // .......... *nop
        case 0x3D:

        // .......... *nop
        case 0x3E:

        // .......... *nop
        case 0x3F:

        // .......... *nop
        case 0x77:

        // .......... *nop
        case 0x7F:

        // .......... *nop
        case 0x80:

        // .......... *nop
        case 0x81:

        // .......... *nop
        case 0x82:

        // .......... *nop
        case 0x83:

        // .......... *nop
        case 0x84:

        // .......... *nop
        case 0x85:

        // .......... *nop
        case 0x86:

        // .......... *nop
        case 0x87:

        // .......... *nop
        case 0x88:

        // .......... *nop
        case 0x89:

        // .......... *nop
        case 0x8A:

        // .......... *nop
        case 0x8B:

        // .......... *nop
        case 0x8C:

        // .......... *nop
        case 0x8D:

        // .......... *nop
        case 0x8E:

        // .......... *nop
        case 0x8F:

        // .......... *nop
        case 0x90:

        // .......... *nop
        case 0x91:

        // .......... *nop
        case 0x92:

        // .......... *nop
        case 0x93:

        // .......... *nop
        case 0x94:

        // .......... *nop
        case 0x95:

        // .......... *nop
        case 0x96:

        // .......... *nop
        case 0x97:

        // .......... *nop
        case 0x98:

        // .......... *nop
        case 0x99:

        // .......... *nop
        case 0x9A:

        // .......... *nop
        case 0x9B:

        // .......... *nop
        case 0x9C:

        // .......... *nop
        case 0x9D:

        // .......... *nop
        case 0x9E:

        // .......... *nop
        case 0x9F:

        // .......... *nop
        case 0xA4:

        // .......... *nop
        case 0xA5:

        // .......... *nop
        case 0xA6:

        // .......... *nop
        case 0xA7:

        // .......... *nop
        case 0xAC:

        // .......... *nop
        case 0xAD:

        // .......... *nop
        case 0xAE:

        // .......... *nop
        case 0xAF:

        // .......... *nop
        case 0xB4:

        // .......... *nop
        case 0xB5:

        // .......... *nop
        case 0xB6:

        // .......... *nop
        case 0xB7:

        // .......... *nop
        case 0xBC:

        // .......... *nop
        case 0xBD:

        // .......... *nop
        case 0xBE:

        // .......... *nop
        case 0xBF:

        // .......... *nop
        case 0xC0:

        // .......... *nop
        case 0xC1:

        // .......... *nop
        case 0xC2:

        // .......... *nop
        case 0xC3:

        // .......... *nop
        case 0xC4:

        // .......... *nop
        case 0xC5:

        // .......... *nop
        case 0xC6:

        // .......... *nop
        case 0xC7:

        // .......... *nop
        case 0xC8:

        // .......... *nop
        case 0xC9:

        // .......... *nop
        case 0xCA:

        // .......... *nop
        case 0xCB:

        // .......... *nop
        case 0xCC:

        // .......... *nop
        case 0xCD:

        // .......... *nop
        case 0xCE:

        // .......... *nop
        case 0xCF:

        // .......... *nop
        case 0xD0:

        // .......... *nop
        case 0xD1:

        // .......... *nop
        case 0xD2:

        // .......... *nop
        case 0xD3:

        // .......... *nop
        case 0xD4:

        // .......... *nop
        case 0xD5:

        // .......... *nop
        case 0xD6:

        // .......... *nop
        case 0xD7:

        // .......... *nop
        case 0xD8:

        // .......... *nop
        case 0xD9:

        // .......... *nop
        case 0xDA:

        // .......... *nop
        case 0xDB:

        // .......... *nop
        case 0xDC:

        // .......... *nop
        case 0xDD:

        // .......... *nop
        case 0xDE:

        // .......... *nop
        case 0xDF:

        // .......... *nop
        case 0xE0:

        // .......... *nop
        case 0xE1:

        // .......... *nop
        case 0xE2:

        // .......... *nop
        case 0xE3:

        // .......... *nop
        case 0xE4:

        // .......... *nop
        case 0xE5:

        // .......... *nop
        case 0xE6:

        // .......... *nop
        case 0xE7:

        // .......... *nop
        case 0xE8:

        // .......... *nop
        case 0xE9:

        // .......... *nop
        case 0xEA:

        // .......... *nop
        case 0xEB:

        // .......... *nop
        case 0xEC:

        // .......... *nop
        case 0xED:

        // .......... *nop
        case 0xEE:

        // .......... *nop
        case 0xEF:

        // .......... *nop
        case 0xF0:

        // .......... *nop
        case 0xF1:

        // .......... *nop
        case 0xF2:

        // .......... *nop
        case 0xF3:

        // .......... *nop
        case 0xF4:

        // .......... *nop
        case 0xF5:

        // .......... *nop
        case 0xF6:

        // .......... *nop
        case 0xF7:

        // .......... *nop
        case 0xF8:

        // .......... *nop
        case 0xF9:

        // .......... *nop
        case 0xFA:

        // .......... *nop
        case 0xFB:

        // .......... *nop
        case 0xFC:

        // .......... *nop
        case 0xFD:

        // .......... *nop
        case 0xFE:

        // .......... *nop
        case 0xFF:
            return 8;

        default:
            std::unreachable();

    }
}

//#################################################################################################

int Z80::exec_prefix_xd(uint16_t &IxR) noexcept {
    struct {
        uint8_t lo;
        uint8_t hi;
    } &ixr = reinterpret_cast<decltype(ixr)>(IxR);

    static_assert(sizeof(decltype(ixr)) == sizeof(uint16_t));

    inc_refresh_register();

    switch (fetch_pc_byte()) {

        // .......... nop
        case 0x00:

        // .......... ld a,a
        case 0x7F:

        // .......... ld b,b
        case 0x40:

        // .......... ld c,c
        case 0x49:

        // .......... ld d,d
        case 0x52:

        // .......... ld e,e
        case 0x5B:

        // .......... ld irh,irh
        case 0x64:

        // .......... ld irl,irl
        case 0x6D:
            return 4 + 4;

        // .......... ld a,b
        case 0x78:
            _reg.A = _reg.B;
            return 4 + 4;

        // .......... ld a,c
        case 0x79:
            _reg.A = _reg.C;
            return 4 + 4;

        // .......... ld a,d
        case 0x7A:
            _reg.A = _reg.D;
            return 4 + 4;

        // .......... ld a,e
        case 0x7B:
            _reg.A = _reg.E;
            return 4 + 4;

        // .......... ld a,irh
        case 0x7C:
            _reg.A = ixr.hi;
            return 4 + 4;

        // .......... ld a,irl
        case 0x7D:
            _reg.A = ixr.lo;
            return 4 + 4;

        // .......... ld b,c
        case 0x41:
            _reg.B = _reg.C;
            return 4 + 4;

        // .......... ld b,d
        case 0x42:
            _reg.B = _reg.D;
            return 4 + 4;

        // .......... ld b,e
        case 0x43:
            _reg.B = _reg.E;
            return 4 + 4;

        // .......... ld b,irh
        case 0x44:
            _reg.B = ixr.hi;
            return 4 + 4;

        // .......... ld b,irl
        case 0x45:
            _reg.B = ixr.lo;
            return 4 + 4;

        // .......... ld b,a
        case 0x47:
            _reg.B = _reg.A;
            return 4 + 4;

        // .......... ld c,b
        case 0x48:
            _reg.C = _reg.B;
            return 4 + 4;

        // .......... ld c,d
        case 0x4A:
            _reg.C = _reg.D;
            return 4 + 4;

        // .......... ld c,e
        case 0x4B:
            _reg.C = _reg.E;
            return 4 + 4;

        // .......... ld c,irh
        case 0x4C:
            _reg.C = ixr.hi;
            return 4 + 4;

        // .......... ld c,irl
        case 0x4D:
            _reg.C = ixr.lo;
            return 4 + 4;

        // .......... ld c,a
        case 0x4F:
            _reg.C = _reg.A;
            return 4 + 4;

        // .......... ld d,b
        case 0x50:
            _reg.D = _reg.B;
            return 4 + 4;

        // .......... ld d,c
        case 0x51:
            _reg.D = _reg.C;
            return 4 + 4;

        // .......... ld d,e
        case 0x53:
            _reg.D = _reg.E;
            return 4 + 4;

        // .......... ld d,irh
        case 0x54:
            _reg.D = ixr.hi;
            return 4 + 4;

        // .......... ld d,irl
        case 0x55:
            _reg.D = ixr.lo;
            return 4 + 4;

        // .......... ld d,a
        case 0x57:
            _reg.D = _reg.A;
            return 4 + 4;

        // .......... ld e,b
        case 0x58:
            _reg.E = _reg.B;
            return 4 + 4;

        // .......... ld e,c
        case 0x59:
            _reg.E = _reg.C;
            return 4 + 4;

        // .......... ld e,d
        case 0x5A:
            _reg.E = _reg.D;
            return 4 + 4;

        // .......... ld e,irh
        case 0x5C:
            _reg.E = ixr.hi;
            return 4 + 4;

        // .......... ld e,irl
        case 0x5D:
            _reg.E = ixr.lo;
            return 4 + 4;

        // .......... ld e,a
        case 0x5F:
            _reg.E = _reg.A;
            return 4 + 4;

        // .......... ld irh,b
        case 0x60:
            ixr.hi = _reg.B;
            return 4 + 4;

        // .......... ld irh,c
        case 0x61:
            ixr.hi = _reg.C;
            return 4 + 4;

        // .......... ld irh,d
        case 0x62:
            ixr.hi = _reg.D;
            return 4 + 4;

        // .......... ld irh,e
        case 0x63:
            ixr.hi = _reg.E;
            return 4 + 4;

        // .......... ld irh,irl
        case 0x65:
            ixr.hi = ixr.lo;
            return 4 + 4;

        // .......... ld irh,a
        case 0x67:
            ixr.hi = _reg.A;
            return 4 + 4;

        // .......... ld irl,b
        case 0x68:
            ixr.lo = _reg.B;
            return 4 + 4;

        // .......... ld irl,c
        case 0x69:
            ixr.lo = _reg.C;
            return 4 + 4;

        // .......... ld irl,d
        case 0x6A:
            ixr.lo = _reg.D;
            return 4 + 4;

        // .......... ld irl,e
        case 0x6B:
            ixr.lo = _reg.E;
            return 4 + 4;

        // .......... ld irl,irh
        case 0x6C:
            ixr.lo = ixr.hi;
            return 4 + 4;

        // .......... ld irl,a
        case 0x6F:
            ixr.lo = _reg.A;
            return 4 + 4;

        // .......... ld b,(ir+d)
        case 0x46:
            _reg.B = peek_byte(wz_set(IxR + fetch_pc_byte<int8_t>()));
            return 8 + 4 + 7;

        // .......... ld c,(ir+d)
        case 0x4E:
            _reg.C = peek_byte(wz_set(IxR + fetch_pc_byte<int8_t>()));
            return 8 + 4 + 7;

        // .......... ld d,(ir+d)
        case 0x56:
            _reg.D = peek_byte(wz_set(IxR + fetch_pc_byte<int8_t>()));
            return 8 + 4 + 7;

        // .......... ld e,(ir+d)
        case 0x5E:
            _reg.E = peek_byte(wz_set(IxR + fetch_pc_byte<int8_t>()));
            return 8 + 4 + 7;

        // .......... ld h,(ir+d)
        case 0x66:
            _reg.H = peek_byte(wz_set(IxR + fetch_pc_byte<int8_t>()));
            return 8 + 4 + 7;

        // .......... ld l,(ir+d)
        case 0x6E:
            _reg.L = peek_byte(wz_set(IxR + fetch_pc_byte<int8_t>()));
            return 8 + 4 + 7;

        // .......... ld a,(ir+d)
        case 0x7E:
            _reg.A = peek_byte(wz_set(IxR + fetch_pc_byte<int8_t>()));
            return 8 + 4 + 7;

        // .......... ld a,(bc)
        case 0x0A:
            _reg.A = peek_byte(wz_set_plus_one(_reg.BC));
            return 4 + 7;

        // .......... ld a,(de)
        case 0x1A:
            _reg.A = peek_byte(wz_set_plus_one(_reg.DE));
            return 4 + 7;

        // .......... ld a,(nn)
        case 0x3A:
            _reg.A = peek_byte(wz_set_plus_one(fetch_pc_word()));
            return 4 + 13;

        // .......... ld (ir+d),b
        case 0x70:
            poke_byte(wz_set(IxR + fetch_pc_byte<int8_t>()), _reg.B);
            return 8 + 4 + 7;

        // .......... ld (ir+d),c
        case 0x71:
            poke_byte(wz_set(IxR + fetch_pc_byte<int8_t>()), _reg.C);
            return 8 + 4 + 7;

        // .......... ld (ir+d),d
        case 0x72:
            poke_byte(wz_set(IxR + fetch_pc_byte<int8_t>()), _reg.D);
            return 8 + 4 + 7;

        // .......... ld (ir+d),e
        case 0x73:
            poke_byte(wz_set(IxR + fetch_pc_byte<int8_t>()), _reg.E);
            return 8 + 4 + 7;

        // .......... ld (ir+d),h
        case 0x74:
            poke_byte(wz_set(IxR + fetch_pc_byte<int8_t>()), _reg.H);
            return 8 + 4 + 7;

        // .......... ld (ir+d),l
        case 0x75:
            poke_byte(wz_set(IxR + fetch_pc_byte<int8_t>()), _reg.L);
            return 8 + 4 + 7;

        // .......... ld (ir+d),a
        case 0x77:
            poke_byte(wz_set(IxR + fetch_pc_byte<int8_t>()), _reg.A);
            return 8 + 4 + 7;

        // .......... ld (bc),a
        case 0x02:
            poke_byte(wz_set_plus_one(_reg.BC, _reg.A), _reg.A);
            return 4 + 7;

        // .......... ld (de),a
        case 0x12:
            poke_byte(wz_set_plus_one(_reg.DE, _reg.A), _reg.A);
            return 4 + 7;

        // .......... ld (nn),a
        case 0x32:
            poke_byte(wz_set_plus_one(fetch_pc_word(), _reg.A), _reg.A);
            return 4 + 13;

        // .......... ld b,n
        case 0x06:
            _reg.B = fetch_pc_byte();
            return 4 + 7;

        // .......... ld c,n
        case 0x0E:
            _reg.C = fetch_pc_byte();
            return 4 + 7;

        // .......... ld d,n
        case 0x16:
            _reg.D = fetch_pc_byte();
            return 4 + 7;

        // .......... ld e,n
        case 0x1E:
            _reg.E = fetch_pc_byte();
            return 4 + 7;

        // .......... ld irh,n
        case 0x26:
            ixr.hi = fetch_pc_byte();
            return 4 + 7;

        // .......... ld irl,n
        case 0x2E:
            ixr.lo = fetch_pc_byte();
            return 4 + 7;

        // .......... ld a,n
        case 0x3E:
            _reg.A = fetch_pc_byte();
            return 4 + 7;

        // .......... ld (ir + d),n
        case 0x36: {
            uint16_t addr = wz_set(IxR + fetch_pc_byte<int8_t>());
            poke_byte(addr, fetch_pc_byte());
            return 5 + 4 + 10;
        }

        // .......... ld bc,nn
        case 0x01:
            _reg.BC = fetch_pc_word();
            return 4 + 10;

        // .......... ld de,nn
        case 0x11:
            _reg.DE = fetch_pc_word();
            return 4 + 10;

        // .......... ld ir,nn
        case 0x21:
            IxR = fetch_pc_word();
            return 4 + 10;

        // .......... ld sp,nn
        case 0x31:
            _sp_reg.SP = fetch_pc_word();
            return 4 + 10;

        // .......... ld ir,(nn)
        case 0x2A:
            IxR = peek_word(wz_set_plus_one(fetch_pc_word()));
            return 4 + 16;

        // .......... ld (nn),ir
        case 0x22:
            poke_word(wz_set_plus_one(fetch_pc_word()), IxR);
            return 4 + 16;

        // .......... ld sp,ir
        case 0xF9:
            _sp_reg.SP = IxR;
            return 4 + 6;

        // .......... push bc
        case 0xC5:
            push(_reg.BC);
            return 4 + 11;

        // .......... push de
        case 0xD5:
            push(_reg.DE);
            return 4 + 11;

        // .......... push ir
        case 0xE5:
            push(IxR);
            return 4 + 11;

        // .......... push af
        case 0xF5:
            push(_reg.AF);
            return 4 + 11;

        // .......... pop bc
        case 0xC1:
            _reg.BC = pop();
            return 4 + 10;

        // .......... pop de
        case 0xD1:
            _reg.DE = pop();
            return 4 + 10;

        // .......... pop ir
        case 0xE1:
            IxR = pop();
            return 4 + 10;

        // .......... pop af
        case 0xF1:
            _reg.AF = pop();
            return 4 + 10;

        // .......... jp addr
        case 0xC3:
            jump(fetch_pc_word());
            return 4 + 10;

        // .......... jp nz,addr
        case 0xC2:
            cond_jump(fetch_pc_word(), ZF_off());
            return 4 + 10;

        // .......... jp z,addr
        case 0xCA:
            cond_jump(fetch_pc_word(), ZF_on());
            return 4 + 10;

        // .......... jp nc,addr
        case 0xD2:
            cond_jump(fetch_pc_word(), CF_off());
            return 4 + 10;

        // .......... jp c,addr
        case 0xDA:
            cond_jump(fetch_pc_word(), CF_on());
            return 4 + 10;

        // .......... jp po,addr
        case 0xE2:
            cond_jump(fetch_pc_word(), PF_off());
            return 4 + 10;

        // .......... jp pe,addr
        case 0xEA:
            cond_jump(fetch_pc_word(), PF_on());
            return 4 + 10;

        // .......... jp p,addr
        case 0xF2:
            cond_jump(fetch_pc_word(), SF_off());
            return 4 + 10;

        // .......... jp m,addr
        case 0xFA:
            cond_jump(fetch_pc_word(), SF_on());
            return 4 + 10;

        // .......... jr disp
        case 0x18:
            jump_r(fetch_pc_byte<int8_t>());
            return 4 + 12;

        // .......... jr c,disp
        case 0x38:
            return cond_jump_r<16, 11>(fetch_pc_byte<int8_t>(), CF_on());

        // .......... jr nc,disp
        case 0x30:
            return cond_jump_r<16, 11>(fetch_pc_byte<int8_t>(), CF_off());

        // .......... jr z,disp
        case 0x28:
            return cond_jump_r<16, 11>(fetch_pc_byte<int8_t>(), ZF_on());

        // .......... jr nz,disp
        case 0x20:
            return cond_jump_r<16, 11>(fetch_pc_byte<int8_t>(), ZF_off());

        // .......... jp (ir)
        case 0xE9:
            jump<false>(IxR);
            return 4 + 4;

        // .......... djnz disp
        case 0x10:
            return cond_jump_r<17, 12>(fetch_pc_byte<int8_t>(), --_reg.B != 0);

        // .......... ex de,hl
        case 0xEB:
            std::swap(_reg.DE, _reg.HL);
            return 4 + 4;

        // .......... ex af,af'
        case 0x08:
            std::swap(_reg.AF, _sh_reg.AF);
            return 4 + 4;

        // .......... exx
        case 0xD9:
            std::swap(_reg.BC, _sh_reg.BC);
            std::swap(_reg.DE, _sh_reg.DE);
            std::swap(_reg.HL, _sh_reg.HL);
            return 4 + 4;

        // .......... ex (sp),ir
        case 0xE3:
            swap_stack(IxR);
            return 4 + 19;

        // .......... in a,(n)
        case 0xDB:
            _reg.A = _ioports.in(wz_set_plus_one(_word(fetch_pc_byte(), _reg.A)));
            return 4 + 11;

        // .......... out (n),a
        case 0xD3:
            _ioports.out(wz_set_plus_one(_word(fetch_pc_byte(), _reg.A), _reg.A), _reg.A);
            return 4 + 11;

        // .......... call addr
        case 0xCD:
            call(fetch_pc_word());
            return 4 + 17;

        // .......... call nz,addr
        case 0xC4:
            return cond_call<21, 14>(fetch_pc_word(), ZF_off());

        // .......... call z,addr
        case 0xCC:
            return cond_call<21, 14>(fetch_pc_word(), ZF_on());

        // .......... call nc,addr
        case 0xD4:
            return cond_call<21, 14>(fetch_pc_word(), CF_off());

        // .......... call c,addr
        case 0xDC:
            return cond_call<21, 14>(fetch_pc_word(), CF_on());

        // .......... call po,addr
        case 0xE4:
            return cond_call<21, 14>(fetch_pc_word(), PF_off());

        // .......... call pe,addr
        case 0xEC:
            return cond_call<21, 14>(fetch_pc_word(), PF_on());

        // .......... call p,addr
        case 0xF4:
            return cond_call<21, 14>(fetch_pc_word(), SF_off());

        // .......... call m,addr
        case 0xFC:
            return cond_call<21, 14>(fetch_pc_word(), SF_on());

        // .......... ret
        case 0xC9:
            ret();
            return 4 + 10;

        // .......... ret nz
        case 0xC0:
            return cond_ret<15, 9>(ZF_off());

        // .......... ret z
        case 0xC8:
            return cond_ret<15, 9>(ZF_on());

        // .......... ret nc
        case 0xD0:
            return cond_ret<15, 9>(CF_off());

        // .......... ret c
        case 0xD8:
            return cond_ret<15, 9>(CF_on());

        // .......... ret po
        case 0xE0:
            return cond_ret<15, 9>(PF_off());

        // .......... ret pe
        case 0xE8:
            return cond_ret<15, 9>(PF_on());

        // .......... ret p
        case 0xF0:
            return cond_ret<15, 9>(SF_off());

        // .......... ret m
        case 0xF8:
            return cond_ret<15, 9>(SF_on());

        // .......... rst 00
        case 0xC7:
            rst<address::rst_00h>();
            return 4 + 11;

        // .......... rst 08
        case 0xCF:
            rst<address::rst_08h>();
            return 4 + 11;

        // .......... rst 10
        case 0xD7:
            rst<address::rst_10h>();
            return 4 + 11;

        // .......... rst 18
        case 0xDF:
            rst<address::rst_18h>();
            return 4 + 11;

        // .......... rst 20
        case 0xE7:
            rst<address::rst_20h>();
            return 4 + 11;

        // .......... rst 28
        case 0xEF:
            rst<address::rst_28h>();
            return 4 + 11;

        // .......... rst 30
        case 0xF7:
            rst<address::rst_30h>();
            return 4 + 11;

        // .......... rst 38
        case 0xFF:
            rst<address::rst_38h>();
            return 4 + 11;

        // .......... rlca
        case 0x07:
            rlca();
            return 4 + 4;

        // .......... rla
        case 0x17:
            rla();
            return 4 + 4;

        // .......... rrca
        case 0x0F:
            rrca();
            return 4 + 4;

        // .......... rra
        case 0x1F:
            rra();
            return 4 + 4;

        // .......... add a,b
        case 0x80:
            add(_reg.B);
            return 4 + 4;

        // .......... add a,c
        case 0x81:
            add(_reg.C);
            return 4 + 4;

        // .......... add a,d
        case 0x82:
            add(_reg.D);
            return 4 + 4;

        // .......... add a,e
        case 0x83:
            add(_reg.E);
            return 4 + 4;

        // .......... add a,irh
        case 0x84:
            add(ixr.hi);
            return 4 + 4;

        // .......... add a,irl
        case 0x85:
            add(ixr.lo);
            return 4 + 4;

        // .......... add a,a
        case 0x87:
            add(_reg.A);
            return 4 + 4;

        // .......... add a,n
        case 0xC6:
            add(fetch_pc_byte());
            return 4 + 7;

        // .......... add a,(ir+d)
        case 0x86:
            add(peek_byte(wz_set(IxR + fetch_pc_byte<int8_t>())));
            return 8 + 4 + 7;

        // .......... adc a,b
        case 0x88:
            adc(_reg.B);
            return 4 + 4;

        // .......... adc a,c
        case 0x89:
            adc(_reg.C);
            return 4 + 4;

        // .......... adc a,d
        case 0x8A:
            adc(_reg.D);
            return 4 + 4;

        // .......... adc a,e
        case 0x8B:
            adc(_reg.E);
            return 4 + 4;

        // .......... adc a,irh
        case 0x8C:
            adc(ixr.hi);
            return 4 + 4;

        // .......... adc a,irl
        case 0x8D:
            adc(ixr.lo);
            return 4 + 4;

        // .......... adc a,a
        case 0x8F:
            adc(_reg.A);
            return 4 + 4;

        // .......... adc a,n
        case 0xCE:
            adc(fetch_pc_byte());
            return 4 + 7;

        // .......... adc a,(ir+d)
        case 0x8E:
            adc(peek_byte(wz_set(IxR + fetch_pc_byte<int8_t>())));
            return 8 + 4 + 7;

        // .......... sub a,b
        case 0x90:
            sub(_reg.B);
            return 4 + 4;

        // .......... sub a,c
        case 0x91:
            sub(_reg.C);
            return 4 + 4;

        // .......... sub a,d
        case 0x92:
            sub(_reg.D);
            return 4 + 4;

        // .......... sub a,e
        case 0x93:
            sub(_reg.E);
            return 4 + 4;

        // .......... sub a,irh
        case 0x94:
            sub(ixr.hi);
            return 4 + 4;

        // .......... sub a,irl
        case 0x95:
            sub(ixr.lo);
            return 4 + 4;

        // .......... sub a,a
        case 0x97:
            sub(_reg.A);
            return 4 + 4;

        // .......... sub a,n
        case 0xD6:
            sub(fetch_pc_byte());
            return 4 + 7;

        // .......... sub a,(ir+d)
        case 0x96:
            sub(peek_byte(wz_set(IxR + fetch_pc_byte<int8_t>())));
            return 8 + 4 + 7;

        // .......... sbc a,b
        case 0x98:
            sbc(_reg.B);
            return 4 + 4;

        // .......... sbc a,c
        case 0x99:
            sbc(_reg.C);
            return 4 + 4;

        // .......... sbc a,d
        case 0x9A:
            sbc(_reg.D);
            return 4 + 4;

        // .......... sbc a,e
        case 0x9B:
            sbc(_reg.E);
            return 4 + 4;

        // .......... sbc a,irh
        case 0x9C:
            sbc(ixr.hi);
            return 4 + 4;

        // .......... sbc a,irl
        case 0x9D:
            sbc(ixr.lo);
            return 4 + 4;

        // .......... sbc a,a
        case 0x9F:
            sbc(_reg.A);
            return 4 + 4;

        // .......... sbc a,n
        case 0xDE:
            sbc(fetch_pc_byte());
            return 4 + 7;

        // .......... sbc a,(ir+d)
        case 0x9E:
            sbc(peek_byte(wz_set(IxR + fetch_pc_byte<int8_t>())));
            return 8 + 4 + 7;

        // .......... and a,b
        case 0xA0:
            bw_and(_reg.B);
            return 4 + 4;

        // .......... and a,c
        case 0xA1:
            bw_and(_reg.C);
            return 4 + 4;

        // .......... and a,d
        case 0xA2:
            bw_and(_reg.D);
            return 4 + 4;

        // .......... and a,e
        case 0xA3:
            bw_and(_reg.E);
            return 4 + 4;

        // .......... and a,irh
        case 0xA4:
            bw_and(ixr.hi);
            return 4 + 4;

        // .......... and a,irl
        case 0xA5:
            bw_and(ixr.lo);
            return 4 + 4;

        // .......... and a,a
        case 0xA7:
            bw_and(_reg.A);
            return 4 + 4;

        // .......... and a,n
        case 0xE6:
            bw_and(fetch_pc_byte());
            return 4 + 7;

        // .......... and a,(ir+d)
        case 0xA6:
            bw_and(peek_byte(wz_set(IxR + fetch_pc_byte<int8_t>())));
            return 8 + 4 + 7;

        // .......... or a,b
        case 0xB0:
            bw_or(_reg.B);
            return 4 + 4;

        // .......... or a,c
        case 0xB1:
            bw_or(_reg.C);
            return 4 + 4;

        // .......... or a,d
        case 0xB2:
            bw_or(_reg.D);
            return 4 + 4;

        // .......... or a,e
        case 0xB3:
            bw_or(_reg.E);
            return 4 + 4;

        // .......... or a,irh
        case 0xB4:
            bw_or(ixr.hi);
            return 4 + 4;

        // .......... or a,irl
        case 0xB5:
            bw_or(ixr.lo);
            return 4 + 4;

        // .......... or a,a
        case 0xB7:
            bw_or(_reg.A);
            return 4 + 4;

        // .......... or a,n
        case 0xF6:
            bw_or(fetch_pc_byte());
            return 4 + 7;

        // .......... or a,(ir+d)
        case 0xB6:
            bw_or(peek_byte(wz_set(IxR + fetch_pc_byte<int8_t>())));
            return 8 + 4 + 7;

        // .......... xor a,b
        case 0xA8:
            bw_xor(_reg.B);
            return 4 + 4;

        // .......... xor a,c
        case 0xA9:
            bw_xor(_reg.C);
            return 4 + 4;

        // .......... xor a,d
        case 0xAA:
            bw_xor(_reg.D);
            return 4 + 4;

        // .......... xor a,e
        case 0xAB:
            bw_xor(_reg.E);
            return 4 + 4;

        // .......... xor a,irh
        case 0xAC:
            bw_xor(ixr.hi);
            return 4 + 4;

        // .......... xor a,irl
        case 0xAD:
            bw_xor(ixr.lo);
            return 4 + 4;

        // .......... xor a,a
        case 0xAF:
            bw_xor(_reg.A);
            return 4 + 4;

        // .......... xor a,n
        case 0xEE:
            bw_xor(fetch_pc_byte());
            return 4 + 7;

        // .......... xor a,(ir+d)
        case 0xAE:
            bw_xor(peek_byte(wz_set(IxR + fetch_pc_byte<int8_t>())));
            return 8 + 4 + 7;

        // .......... cp a,b
        case 0xB8:
            cp(_reg.B);
            return 4 + 4;

        // .......... cp a,c
        case 0xB9:
            cp(_reg.C);
            return 4 + 4;

        // .......... cp a,d
        case 0xBA:
            cp(_reg.D);
            return 4 + 4;

        // .......... cp a,e
        case 0xBB:
            cp(_reg.E);
            return 4 + 4;

        // .......... cp a,irh
        case 0xBC:
            cp(ixr.hi);
            return 4 + 4;

        // .......... cp a,irl
        case 0xBD:
            cp(ixr.lo);
            return 4 + 4;

        // .......... cp a,a
        case 0xBF:
            cp(_reg.A);
            return 4 + 4;

        // .......... cp a,n
        case 0xFE:
            cp(fetch_pc_byte());
            return 4 + 7;

        // .......... cp a,(ir+d)
        case 0xBE:
            cp(peek_byte(wz_set(IxR + fetch_pc_byte<int8_t>())));
            return 8 + 4 + 7;

        // .......... inc b
        case 0x04:
            inc(_reg.B);
            return 4 + 4;

        // .......... inc c
        case 0x0C:
            inc(_reg.C);
            return 4 + 4;

        // .......... inc d
        case 0x14:
            inc(_reg.D);
            return 4 + 4;

        // .......... inc e
        case 0x1C:
            inc(_reg.E);
            return 4 + 4;

        // .......... inc irh
        case 0x24:
            inc(ixr.hi);
            return 4 + 4;

        // .......... inc irl
        case 0x2C:
            inc(ixr.lo);
            return 4 + 4;

        // .......... inc a
        case 0x3C:
            inc(_reg.A);
            return 4 + 4;

        // .......... inc (ir + d)
        case 0x34:
            modify_at<&Z80::inc>(wz_set(IxR + fetch_pc_byte<int8_t>()));
            return 8 + 4 + 11;

        // .......... dec b
        case 0x05:
            dec(_reg.B);
            return 4 + 4;

        // .......... dec c
        case 0x0D:
            dec(_reg.C);
            return 4 + 4;

        // .......... dec d
        case 0x15:
            dec(_reg.D);
            return 4 + 4;

        // .......... dec e
        case 0x1D:
            dec(_reg.E);
            return 4 + 4;

        // .......... dec irh
        case 0x25:
            dec(ixr.hi);
            return 4 + 4;

        // .......... dec irl
        case 0x2D:
            dec(ixr.lo);
            return 4 + 4;

        // .......... dec a
        case 0x3D:
            dec(_reg.A);
            return 4 + 4;

        // .......... dec (ir+d)
        case 0x35:
            modify_at<&Z80::dec>(wz_set(IxR + fetch_pc_byte<int8_t>()));
            return 8 + 4 + 11;

        // .......... add ir,bc
        case 0x09:
            add(IxR, _reg.BC);
            return 4 + 11;

        // .......... add ir,de
        case 0x19:
            add(IxR, _reg.DE);
            return 4 + 11;

        // .......... add ir,ir
        case 0x29:
            add(IxR, IxR);
            return 4 + 11;

        // .......... add ir,sp
        case 0x39:
            add(IxR, _sp_reg.SP);
            return 4 + 11;

        // .......... inc bc
        case 0x03:
            ++_reg.BC;
            return 4 + 6;

        // .......... inc de
        case 0x13:
            ++_reg.DE;
            return 4 + 6;

        // .......... inc ir
        case 0x23:
            ++IxR;
            return 4 + 6;

        // .......... inc sp
        case 0x33:
            ++_sp_reg.SP;
            return 4 + 6;

        // .......... dec bc
        case 0x0B:
            --_reg.BC;
            return 4 + 6;

        // .......... dec de
        case 0x1B:
            --_reg.DE;
            return 4 + 6;

        // .......... dec ir
        case 0x2B:
            --IxR;
            return 4 + 6;

        // .......... dec sp
        case 0x3B:
            --_sp_reg.SP;
            return 4 + 6;

        // .......... daa
        case 0x27:
            daa();
            return 4 + 4;

        // .......... cpl
        case 0x2F:
            cpl();
            return 4 + 4;

        // .......... ccf
        case 0x3F:
            ccf();
            return 4 + 4;

        // .......... scf
        case 0x37:
            scf();
            return 4 + 4;

        // .......... halt
        case 0x76:
            _halted = true;
            return 4 + 4;

        // .......... ei
        case 0xFB:
            enable_int();
            return 4 + 4;

        // .......... di
        case 0xF3:
            disable_int();
            return 4 + 4;

        // .......... prefix
        case prefix::CB:
            return exec_prefix_xd_cb(IxR);

        // .......... prefix
        case prefix::ED:

        // .......... prefix
        case prefix::DD:

        // .......... prefix
        case prefix::FD:
            return --_sp_reg.PC, 4;

        default:
            std::unreachable();
    }
}

//#################################################################################################

int Z80::exec_prefix_xd_cb(const uint16_t &IxR) noexcept {

    uint16_t addr = wz_set(IxR + fetch_pc_byte<int8_t>());

    switch (fetch_pc_byte()) {
        // .......... rlc b
        case 0x00:
            modify_via<&Z80::rlc, &reg_t::B>(addr);
            return 23;

        // .......... rlc c
        case 0x01:
            modify_via<&Z80::rlc, &reg_t::C>(addr);
            return 23;

        // .......... rlc d
        case 0x02:
            modify_via<&Z80::rlc, &reg_t::D>(addr);
            return 23;

        // .......... rlc e
        case 0x03:
            modify_via<&Z80::rlc, &reg_t::E>(addr);
            return 23;

        // .......... rlc h
        case 0x04:
            modify_via<&Z80::rlc, &reg_t::H>(addr);
            return 23;

        // .......... rlc l
        case 0x05:
            modify_via<&Z80::rlc, &reg_t::L>(addr);
            return 23;

        // .......... rlc (IR + d)
        case 0x06:
            modify_at<&Z80::rlc>(addr);
            return 23;

        // .......... rlc a
        case 0x07:
            modify_via<&Z80::rlc, &reg_t::A>(addr);
            return 23;

        // .......... rrc b
        case 0x08:
            modify_via<&Z80::rrc, &reg_t::B>(addr);
            return 23;

        // .......... rrc c
        case 0x09:
            modify_via<&Z80::rrc, &reg_t::C>(addr);
            return 23;

        // .......... rrc d
        case 0x0A:
            modify_via<&Z80::rrc, &reg_t::D>(addr);
            return 23;

        // .......... rrc e
        case 0x0B:
            modify_via<&Z80::rrc, &reg_t::E>(addr);
            return 23;

        // .......... rrc h
        case 0x0C:
            modify_via<&Z80::rrc, &reg_t::H>(addr);
            return 23;

        // .......... rrc l
        case 0x0D:
            modify_via<&Z80::rrc, &reg_t::L>(addr);
            return 23;

        // .......... rrc (IR + d)
        case 0x0E:
            modify_at<&Z80::rrc>(addr);
            return 23;

        // .......... rrc a
        case 0x0F:
            modify_via<&Z80::rrc, &reg_t::A>(addr);
            return 23;

        // .......... rl b
        case 0x10:
            modify_via<&Z80::rl, &reg_t::B>(addr);
            return 23;

        // .......... rl c
        case 0x11:
            modify_via<&Z80::rl, &reg_t::C>(addr);
            return 23;

        // .......... rl d
        case 0x12:
            modify_via<&Z80::rl, &reg_t::D>(addr);
            return 23;

        // .......... rl e
        case 0x13:
            modify_via<&Z80::rl, &reg_t::E>(addr);
            return 23;

        // .......... rl h
        case 0x14:
            modify_via<&Z80::rl, &reg_t::H>(addr);
            return 23;

        // .......... rl l
        case 0x15:
            modify_via<&Z80::rl, &reg_t::L>(addr);
            return 23;

        // .......... rl (IR + d)
        case 0x16:
            modify_at<&Z80::rl>(addr);
            return 23;

        // .......... rl a
        case 0x17:
            modify_via<&Z80::rl, &reg_t::A>(addr);
            return 23;

        // .......... rr b
        case 0x18:
            modify_via<&Z80::rr, &reg_t::B>(addr);
            return 23;

        // .......... rr c
        case 0x19:
            modify_via<&Z80::rr, &reg_t::C>(addr);
            return 23;

        // .......... rr d
        case 0x1A:
            modify_via<&Z80::rr, &reg_t::D>(addr);
            return 23;

        // .......... rr e
        case 0x1B:
            modify_via<&Z80::rr, &reg_t::E>(addr);
            return 23;

        // .......... rr h
        case 0x1C:
            modify_via<&Z80::rr, &reg_t::H>(addr);
            return 23;

        // .......... rr l
        case 0x1D:
            modify_via<&Z80::rr, &reg_t::L>(addr);
            return 23;

        // .......... rr (IR + d)
        case 0x1E:
            modify_at<&Z80::rr>(addr);
            return 23;

        // .......... rr a
        case 0x1F:
            modify_via<&Z80::rr, &reg_t::A>(addr);
            return 23;

        // .......... sla b
        case 0x20:
            modify_via<&Z80::sla, &reg_t::B>(addr);
            return 23;

        // .......... sla c
        case 0x21:
            modify_via<&Z80::sla, &reg_t::C>(addr);
            return 23;

        // .......... sla d
        case 0x22:
            modify_via<&Z80::sla, &reg_t::D>(addr);
            return 23;

        // .......... sla e
        case 0x23:
            modify_via<&Z80::sla, &reg_t::E>(addr);
            return 23;

        // .......... sla h
        case 0x24:
            modify_via<&Z80::sla, &reg_t::H>(addr);
            return 23;

        // .......... sla l
        case 0x25:
            modify_via<&Z80::sla, &reg_t::L>(addr);
            return 23;

        // .......... sla (IR + d)
        case 0x26:
            modify_at<&Z80::sla>(addr);
            return 23;

        // .......... sla a
        case 0x27:
            modify_via<&Z80::sla, &reg_t::A>(addr);
            return 23;

        // .......... sra b
        case 0x28:
            modify_via<&Z80::sra, &reg_t::B>(addr);
            return 23;

        // .......... sra c
        case 0x29:
            modify_via<&Z80::sra, &reg_t::C>(addr);
            return 23;

        // .......... sra d
        case 0x2A:
            modify_via<&Z80::sra, &reg_t::D>(addr);
            return 23;

        // .......... sra e
        case 0x2B:
            modify_via<&Z80::sra, &reg_t::E>(addr);
            return 23;

        // .......... sra h
        case 0x2C:
            modify_via<&Z80::sra, &reg_t::H>(addr);
            return 23;

        // .......... sra l
        case 0x2D:
            modify_via<&Z80::sra, &reg_t::L>(addr);
            return 23;

        // .......... sra (IR + d)
        case 0x2E:
            modify_at<&Z80::sra>(addr);
            return 23;

        // .......... sra a
        case 0x2F:
            modify_via<&Z80::sra, &reg_t::A>(addr);
            return 23;

        // .......... sli b
        case 0x30:
            modify_via<&Z80::sli, &reg_t::B>(addr);
            return 23;

        // .......... sli c
        case 0x31:
            modify_via<&Z80::sli, &reg_t::C>(addr);
            return 23;

        // .......... sli d
        case 0x32:
            modify_via<&Z80::sli, &reg_t::D>(addr);
            return 23;

        // .......... sli e
        case 0x33:
            modify_via<&Z80::sli, &reg_t::E>(addr);
            return 23;

        // .......... sli h
        case 0x34:
            modify_via<&Z80::sli, &reg_t::H>(addr);
            return 23;

        // .......... sli l
        case 0x35:
            modify_via<&Z80::sli, &reg_t::L>(addr);
            return 23;

        // .......... sli (IR + d)
        case 0x36:
            modify_at<&Z80::sli>(addr);
            return 23;

        // .......... sli a
        case 0x37:
            modify_via<&Z80::sli, &reg_t::A>(addr);
            return 23;

        // .......... srl b
        case 0x38:
            modify_via<&Z80::srl, &reg_t::B>(addr);
            return 23;

        // .......... srl c
        case 0x39:
            modify_via<&Z80::srl, &reg_t::C>(addr);
            return 23;

        // .......... srl d
        case 0x3A:
            modify_via<&Z80::srl, &reg_t::D>(addr);
            return 23;

        // .......... srl e
        case 0x3B:
            modify_via<&Z80::srl, &reg_t::E>(addr);
            return 23;

        // .......... srl h
        case 0x3C:
            modify_via<&Z80::srl, &reg_t::H>(addr);
            return 23;

        // .......... srl l
        case 0x3D:
            modify_via<&Z80::srl, &reg_t::L>(addr);
            return 23;

        // .......... srl (IR + d)
        case 0x3E:
            modify_at<&Z80::srl>(addr);
            return 23;

        // .......... srl a
        case 0x3F:
            modify_via<&Z80::srl, &reg_t::A>(addr);
            return 23;

        // .......... bit 0,(IR + d)
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x47:
        case 0x46:
            bit<0>(peek_byte(addr), addr);
            return 23;

        // .......... bit 1,(IR + d)
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4F:
        case 0x4E:
            bit<1>(peek_byte(addr), addr);
            return 23;

        // .......... bit 2,(IR + d)
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x57:
        case 0x56:
            bit<2>(peek_byte(addr), addr);
            return 23;

        // .......... bit 3,(IR + d)
        case 0x58:
        case 0x59:
        case 0x5A:
        case 0x5B:
        case 0x5C:
        case 0x5D:
        case 0x5F:
        case 0x5E:
            bit<3>(peek_byte(addr), addr);
            return 23;

        // .......... bit 4,(IR + d)
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x67:
        case 0x66:
            bit<4>(peek_byte(addr), addr);
            return 23;

        // .......... bit 5,(IR + d)
        case 0x68:
        case 0x69:
        case 0x6A:
        case 0x6B:
        case 0x6C:
        case 0x6D:
        case 0x6F:
        case 0x6E:
            bit<5>(peek_byte(addr), addr);
            return 23;

        // .......... bit 6,(IR + d)
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x77:
        case 0x76:
            bit<6>(peek_byte(addr), addr);
            return 23;

        // .......... bit 7,(IR + d)
        case 0x78:
        case 0x79:
        case 0x7A:
        case 0x7B:
        case 0x7C:
        case 0x7D:
        case 0x7F:
        case 0x7E:
            bit<7>(peek_byte(addr), addr);
            return 23;

        // .......... res 0,b
        case 0x80:
            modify_via<&Z80::res<0>, &reg_t::B>(addr);
            return 23;

        // .......... res 0,c
        case 0x81:
            modify_via<&Z80::res<0>, &reg_t::C>(addr);
            return 23;

        // .......... res 0,d
        case 0x82:
            modify_via<&Z80::res<0>, &reg_t::D>(addr);
            return 23;

        // .......... res 0,e
        case 0x83:
            modify_via<&Z80::res<0>, &reg_t::E>(addr);
            return 23;

        // .......... res 0,h
        case 0x84:
            modify_via<&Z80::res<0>, &reg_t::H>(addr);
            return 23;

        // .......... res 0,l
        case 0x85:
            modify_via<&Z80::res<0>, &reg_t::L>(addr);
            return 23;

        // .......... res 0,(IR + d)
        case 0x86:
            modify_at<&Z80::res<0>>(addr);
            return 23;

        // .......... res 0,a
        case 0x87:
            modify_via<&Z80::res<0>, &reg_t::A>(addr);
            return 23;

        // .......... res 1,b
        case 0x88:
            modify_via<&Z80::res<1>, &reg_t::B>(addr);
            return 23;

        // .......... res 1,c
        case 0x89:
            modify_via<&Z80::res<1>, &reg_t::C>(addr);
            return 23;

        // .......... res 1,d
        case 0x8A:
            modify_via<&Z80::res<1>, &reg_t::D>(addr);
            return 23;

        // .......... res 1,e
        case 0x8B:
            modify_via<&Z80::res<1>, &reg_t::E>(addr);
            return 23;

        // .......... res 1,h
        case 0x8C:
            modify_via<&Z80::res<1>, &reg_t::H>(addr);
            return 23;

        // .......... res 1,l
        case 0x8D:
            modify_via<&Z80::res<1>, &reg_t::L>(addr);
            return 23;

        // .......... res 1,(IR + d)
        case 0x8E:
            modify_at<&Z80::res<1>>(addr);
            return 23;

        // .......... res 1,a
        case 0x8F:
            modify_via<&Z80::res<1>, &reg_t::A>(addr);
            return 23;

        // .......... res 2,b
        case 0x90:
            modify_via<&Z80::res<2>, &reg_t::B>(addr);
            return 23;

        // .......... res 2,c
        case 0x91:
            modify_via<&Z80::res<2>, &reg_t::C>(addr);
            return 23;

        // .......... res 2,d
        case 0x92:
            modify_via<&Z80::res<2>, &reg_t::D>(addr);
            return 23;

        // .......... res 2,e
        case 0x93:
            modify_via<&Z80::res<2>, &reg_t::E>(addr);
            return 23;

        // .......... res 2,h
        case 0x94:
            modify_via<&Z80::res<2>, &reg_t::H>(addr);
            return 23;

        // .......... res 2,l
        case 0x95:
            modify_via<&Z80::res<2>, &reg_t::L>(addr);
            return 23;

        // .......... res 2,(IR + d)
        case 0x96:
            modify_at<&Z80::res<2>>(addr);
            return 23;

        // .......... res 2,a
        case 0x97:
            modify_via<&Z80::res<2>, &reg_t::A>(addr);
            return 23;

        // .......... res 3,b
        case 0x98:
            modify_via<&Z80::res<3>, &reg_t::B>(addr);
            return 23;

        // .......... res 3,c
        case 0x99:
            modify_via<&Z80::res<3>, &reg_t::C>(addr);
            return 23;

        // .......... res 3,d
        case 0x9A:
            modify_via<&Z80::res<3>, &reg_t::D>(addr);
            return 23;

        // .......... res 3,e
        case 0x9B:
            modify_via<&Z80::res<3>, &reg_t::E>(addr);
            return 23;

        // .......... res 3,h
        case 0x9C:
            modify_via<&Z80::res<3>, &reg_t::H>(addr);
            return 23;

        // .......... res 3,l
        case 0x9D:
            modify_via<&Z80::res<3>, &reg_t::L>(addr);
            return 23;

        // .......... res 3,(IR + d)
        case 0x9E:
            modify_at<&Z80::res<3>>(addr);
            return 23;

        // .......... res 3,a
        case 0x9F:
            modify_via<&Z80::res<3>, &reg_t::A>(addr);
            return 23;

        // .......... res 4,b
        case 0xA0:
            modify_via<&Z80::res<4>, &reg_t::B>(addr);
            return 23;

        // .......... res 4,c
        case 0xA1:
            modify_via<&Z80::res<4>, &reg_t::C>(addr);
            return 23;

        // .......... res 4,d
        case 0xA2:
            modify_via<&Z80::res<4>, &reg_t::D>(addr);
            return 23;

        // .......... res 4,e
        case 0xA3:
            modify_via<&Z80::res<4>, &reg_t::E>(addr);
            return 23;

        // .......... res 4,h
        case 0xA4:
            modify_via<&Z80::res<4>, &reg_t::H>(addr);
            return 23;

        // .......... res 4,l
        case 0xA5:
            modify_via<&Z80::res<4>, &reg_t::L>(addr);
            return 23;

        // .......... res 4,(IR + d)
        case 0xA6:
            modify_at<&Z80::res<4>>(addr);
            return 23;

        // .......... res 4,a
        case 0xA7:
            modify_via<&Z80::res<4>, &reg_t::A>(addr);
            return 23;

        // .......... res 5,b
        case 0xA8:
            modify_via<&Z80::res<5>, &reg_t::B>(addr);
            return 23;

        // .......... res 5,c
        case 0xA9:
            modify_via<&Z80::res<5>, &reg_t::C>(addr);
            return 23;

        // .......... res 5,d
        case 0xAA:
            modify_via<&Z80::res<5>, &reg_t::D>(addr);
            return 23;

        // .......... res 5,e
        case 0xAB:
            modify_via<&Z80::res<5>, &reg_t::E>(addr);
            return 23;

        // .......... res 5,h
        case 0xAC:
            modify_via<&Z80::res<5>, &reg_t::H>(addr);
            return 23;

        // .......... res 5,l
        case 0xAD:
            modify_via<&Z80::res<5>, &reg_t::L>(addr);
            return 23;

        // .......... res 5,(IR + d)
        case 0xAE:
            modify_at<&Z80::res<5>>(addr);
            return 23;

        // .......... res 5,a
        case 0xAF:
            modify_via<&Z80::res<5>, &reg_t::A>(addr);
            return 23;

        // .......... res 6,b
        case 0xB0:
            modify_via<&Z80::res<6>, &reg_t::B>(addr);
            return 23;

        // .......... res 6,c
        case 0xB1:
            modify_via<&Z80::res<6>, &reg_t::C>(addr);
            return 23;

        // .......... res 6,d
        case 0xB2:
            modify_via<&Z80::res<6>, &reg_t::D>(addr);
            return 23;

        // .......... res 6,e
        case 0xB3:
            modify_via<&Z80::res<6>, &reg_t::E>(addr);
            return 23;

        // .......... res 6,h
        case 0xB4:
            modify_via<&Z80::res<6>, &reg_t::H>(addr);
            return 23;

        // .......... res 6,l
        case 0xB5:
            modify_via<&Z80::res<6>, &reg_t::L>(addr);
            return 23;

        // .......... res 6,(IR + d)
        case 0xB6:
            modify_at<&Z80::res<6>>(addr);
            return 23;

        // .......... res 6,a
        case 0xB7:
            modify_via<&Z80::res<6>, &reg_t::A>(addr);
            return 23;

        // .......... res 7,b
        case 0xB8:
            modify_via<&Z80::res<7>, &reg_t::B>(addr);
            return 23;

        // .......... res 7,c
        case 0xB9:
            modify_via<&Z80::res<7>, &reg_t::C>(addr);
            return 23;

        // .......... res 7,d
        case 0xBA:
            modify_via<&Z80::res<7>, &reg_t::D>(addr);
            return 23;

        // .......... res 7,e
        case 0xBB:
            modify_via<&Z80::res<7>, &reg_t::E>(addr);
            return 23;

        // .......... res 7,h
        case 0xBC:
            modify_via<&Z80::res<7>, &reg_t::H>(addr);
            return 23;

        // .......... res 7,l
        case 0xBD:
            modify_via<&Z80::res<7>, &reg_t::L>(addr);
            return 23;

        // .......... res 7,(IR + d)
        case 0xBE:
            modify_at<&Z80::res<7>>(addr);
            return 23;

        // .......... res 7,a
        case 0xBF:
            modify_via<&Z80::res<7>, &reg_t::A>(addr);
            return 23;

        // .......... set 0,b
        case 0xC0:
            modify_via<&Z80::set<0>, &reg_t::B>(addr);
            return 23;

        // .......... set 0,c
        case 0xC1:
            modify_via<&Z80::set<0>, &reg_t::C>(addr);
            return 23;

        // .......... set 0,d
        case 0xC2:
            modify_via<&Z80::set<0>, &reg_t::D>(addr);
            return 23;

        // .......... set 0,e
        case 0xC3:
            modify_via<&Z80::set<0>, &reg_t::E>(addr);
            return 23;

        // .......... set 0,h
        case 0xC4:
            modify_via<&Z80::set<0>, &reg_t::H>(addr);
            return 23;

        // .......... set 0,l
        case 0xC5:
            modify_via<&Z80::set<0>, &reg_t::L>(addr);
            return 23;

        // .......... set 0,(IR + d)
        case 0xC6:
            modify_at<&Z80::set<0>>(addr);
            return 23;

        // .......... set 0,a
        case 0xC7:
            modify_via<&Z80::set<0>, &reg_t::A>(addr);
            return 23;

        // .......... set 1,b
        case 0xC8:
            modify_via<&Z80::set<1>, &reg_t::B>(addr);
            return 23;

        // .......... set 1,c
        case 0xC9:
            modify_via<&Z80::set<1>, &reg_t::C>(addr);
            return 23;

        // .......... set 1,d
        case 0xCA:
            modify_via<&Z80::set<1>, &reg_t::D>(addr);
            return 23;

        // .......... set 1,e
        case 0xCB:
            modify_via<&Z80::set<1>, &reg_t::E>(addr);
            return 23;

        // .......... set 1,h
        case 0xCC:
            modify_via<&Z80::set<1>, &reg_t::H>(addr);
            return 23;

        // .......... set 1,l
        case 0xCD:
            modify_via<&Z80::set<1>, &reg_t::L>(addr);
            return 23;

        // .......... set 1,(IR + d)
        case 0xCE:
            modify_at<&Z80::set<1>>(addr);
            return 23;

        // .......... set 1,a
        case 0xCF:
            modify_via<&Z80::set<1>, &reg_t::A>(addr);
            return 23;

        // .......... set 2,b
        case 0xD0:
            modify_via<&Z80::set<2>, &reg_t::B>(addr);
            return 23;

        // .......... set 2,c
        case 0xD1:
            modify_via<&Z80::set<2>, &reg_t::C>(addr);
            return 23;

        // .......... set 2,d
        case 0xD2:
            modify_via<&Z80::set<2>, &reg_t::D>(addr);
            return 23;

        // .......... set 2,e
        case 0xD3:
            modify_via<&Z80::set<2>, &reg_t::E>(addr);
            return 23;

        // .......... set 2,h
        case 0xD4:
            modify_via<&Z80::set<2>, &reg_t::H>(addr);
            return 23;

        // .......... set 2,l
        case 0xD5:
            modify_via<&Z80::set<2>, &reg_t::L>(addr);
            return 23;

        // .......... set 2,(IR + d)
        case 0xD6:
            modify_at<&Z80::set<2>>(addr);
            return 23;

        // .......... set 2,a
        case 0xD7:
            modify_via<&Z80::set<2>, &reg_t::A>(addr);
            return 23;

        // .......... set 3,b
        case 0xD8:
            modify_via<&Z80::set<3>, &reg_t::B>(addr);
            return 23;

        // .......... set 3,c
        case 0xD9:
            modify_via<&Z80::set<3>, &reg_t::C>(addr);
            return 23;

        // .......... set 3,d
        case 0xDA:
            modify_via<&Z80::set<3>, &reg_t::D>(addr);
            return 23;

        // .......... set 3,e
        case 0xDB:
            modify_via<&Z80::set<3>, &reg_t::E>(addr);
            return 23;

        // .......... set 3,h
        case 0xDC:
            modify_via<&Z80::set<3>, &reg_t::H>(addr);
            return 23;

        // .......... set 3,l
        case 0xDD:
            modify_via<&Z80::set<3>, &reg_t::L>(addr);
            return 23;

        // .......... set 3,(IR + d)
        case 0xDE:
            modify_at<&Z80::set<3>>(addr);
            return 23;

        // .......... set 3,a
        case 0xDF:
            modify_via<&Z80::set<3>, &reg_t::A>(addr);
            return 23;

        // .......... set 4,b
        case 0xE0:
            modify_via<&Z80::set<4>, &reg_t::B>(addr);
            return 23;

        // .......... set 4,c
        case 0xE1:
            modify_via<&Z80::set<4>, &reg_t::C>(addr);
            return 23;

        // .......... set 4,d
        case 0xE2:
            modify_via<&Z80::set<4>, &reg_t::D>(addr);
            return 23;

        // .......... set 4,e
        case 0xE3:
            modify_via<&Z80::set<4>, &reg_t::E>(addr);
            return 23;

        // .......... set 4,h
        case 0xE4:
            modify_via<&Z80::set<4>, &reg_t::H>(addr);
            return 23;

        // .......... set 4,l
        case 0xE5:
            modify_via<&Z80::set<4>, &reg_t::L>(addr);
            return 23;

        // .......... set 4,(IR + d)
        case 0xE6:
            modify_at<&Z80::set<4>>(addr);
            return 23;

        // .......... set 4,a
        case 0xE7:
            modify_via<&Z80::set<4>, &reg_t::A>(addr);
            return 23;

        // .......... set 5,b
        case 0xE8:
            modify_via<&Z80::set<5>, &reg_t::B>(addr);
            return 23;

        // .......... set 5,c
        case 0xE9:
            modify_via<&Z80::set<5>, &reg_t::C>(addr);
            return 23;

        // .......... set 5,d
        case 0xEA:
            modify_via<&Z80::set<5>, &reg_t::D>(addr);
            return 23;

        // .......... set 5,e
        case 0xEB:
            modify_via<&Z80::set<5>, &reg_t::E>(addr);
            return 23;

        // .......... set 5,h
        case 0xEC:
            modify_via<&Z80::set<5>, &reg_t::H>(addr);
            return 23;

        // .......... set 5,l
        case 0xED:
            modify_via<&Z80::set<5>, &reg_t::L>(addr);
            return 23;

        // .......... set 5,(IR + d)
        case 0xEE:
            modify_at<&Z80::set<5>>(addr);
            return 23;

        // .......... set 5,a
        case 0xEF:
            modify_via<&Z80::set<5>, &reg_t::A>(addr);
            return 23;

        // .......... set 6,b
        case 0xF0:
            modify_via<&Z80::set<6>, &reg_t::B>(addr);
            return 23;

        // .......... set 6,c
        case 0xF1:
            modify_via<&Z80::set<6>, &reg_t::C>(addr);
            return 23;

        // .......... set 6,d
        case 0xF2:
            modify_via<&Z80::set<6>, &reg_t::D>(addr);
            return 23;

        // .......... set 6,e
        case 0xF3:
            modify_via<&Z80::set<6>, &reg_t::E>(addr);
            return 23;

        // .......... set 6,h
        case 0xF4:
            modify_via<&Z80::set<6>, &reg_t::H>(addr);
            return 23;

        // .......... set 6,l
        case 0xF5:
            modify_via<&Z80::set<6>, &reg_t::L>(addr);
            return 23;

        // .......... set 6,(IR + d)
        case 0xF6:
            modify_at<&Z80::set<6>>(addr);
            return 23;

        // .......... set 6,a
        case 0xF7:
            modify_via<&Z80::set<6>, &reg_t::A>(addr);
            return 23;

        // .......... set 7,b
        case 0xF8:
            modify_via<&Z80::set<7>, &reg_t::B>(addr);
            return 23;

        // .......... set 7,c
        case 0xF9:
            modify_via<&Z80::set<7>, &reg_t::C>(addr);
            return 23;

        // .......... set 7,d
        case 0xFA:
            modify_via<&Z80::set<7>, &reg_t::D>(addr);
            return 23;

        // .......... set 7,e
        case 0xFB:
            modify_via<&Z80::set<7>, &reg_t::E>(addr);
            return 23;

        // .......... set 7,h
        case 0xFC:
            modify_via<&Z80::set<7>, &reg_t::H>(addr);
            return 23;

        // .......... set 7,l
        case 0xFD:
            modify_via<&Z80::set<7>, &reg_t::L>(addr);
            return 23;

        // .......... set 7,(IR + d)
        case 0xFE:
            modify_at<&Z80::set<7>>(addr);
            return 23;

        // .......... set 7,a
        case 0xFF:
            modify_via<&Z80::set<7>, &reg_t::A>(addr);
            return 23;

        default:
            std::unreachable();
    }
}
