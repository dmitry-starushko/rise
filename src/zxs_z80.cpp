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

#include "zxs.h"
#include <ranges>

struct __attribute__((packed)) z80_header_v1 {
    uint8_t A;                  // 00 | 1 | A register
    uint8_t F;                  // 01 | 1 | F register
    uint16_t BC;                // 02 | 2 | BC register pair (LSB, i.e. C, first)
    uint16_t HL;                // 04 | 2 | HL register pair
    uint16_t PC;                // 06 | 2 | Program counter
    uint16_t SP;                // 08 | 2 | Stack pointer
    uint8_t I;                  // 10 | 1 | Interrupt register
    uint8_t R;                  // 11 | 1 | Refresh register (Bit 7 is not significant!)
    struct {                    // 12 | 1
        uint8_t R7: 1;          // -    Bit 0:   Bit 7 of the R-register
        uint8_t border: 3;      // -    Bit 1-3: Border colour
        uint8_t sam_rom: 1;     // -    Bit 4:   1=Basic SamRom switched in
        uint8_t compressed: 1;  // -    Bit 5:   1=Block of data is compressed
    };                          // -    Bit 6-7: No meaning
    uint16_t DE;                // 13 | 2 | DE register pair
    uint16_t BC_;               // 15 | 2 | BC' register pair
    uint16_t DE_;               // 17 | 2 | DE' register pair
    uint16_t HL_;               // 19 | 2 | HL' register pair
    uint8_t A_;                 // 21 | 1 | A' register
    uint8_t F_;                 // 22 | 1 | F' register
    uint16_t IY;                // 23 | 2 | IY register (Again LSB first)
    uint16_t IX;                // 25 | 2 | IX register
    uint8_t iff1;               // 27 | 1 | Interrupt flipflop, 0=DI, otherwise EI
    uint8_t iff2;               // 28 | 1 | IFF2 (not particularly important...)
    struct {                    // 29 | 1
        uint8_t imode: 2;       // -    Bit 0-1: Interrupt mode (0, 1 or 2)
        uint8_t issue_2: 1;     // -    Bit 2:   1=Issue 2 emulation
        uint8_t dbl_ifrq: 1;    // -    Bit 3:   1=Double interrupt frequency
        uint8_t video_sync: 2;  // -    Bit 4-5: 1=High video synchronisation 3=Low video synchronisation 0,2=Normal
        uint8_t joystick: 2;    // -    Bit 6-7: 0=Cursor/Protek/AGF 1=Kempston 2=Sinclair 2 Left (or user defined, for version 3 .z80 files) 3=Sinclair 2 Right
    };
};

static_assert(sizeof(z80_header_v1) == 30, "This is verified fact!");

/*
 * Version 2 and 3 .z80 files start with the same 30 byte header as version 1 files.
 * However, bit 4 and 5 of the flag byte have no meaning anymore, and the program counter (byte 6 and 7)
 * is zero to signal a version 2 or 3 file.
 *
 * The value of the word at position 30 is 23 for version 2 files, and 54 or 55 for version 3;
 * the fields marked '*' are the ones that are present in the version 2 header.
 * The final byte (marked '**') is present only if the word at position 30 is 55.
 *
 */

struct __attribute__((packed)) z80_header_v2 {
    uint16_t length;            // * 30 | 2  | Length of additional header block
    uint16_t PC;                // * 32 | 2  | Program counter
    uint8_t hw_model;           // * 34 | 1  | Hardware mode (see below)
    uint8_t mode_related;       // * 35 | 1  | If in SamRam mode, bitwise state of 74ls259. For example, bit 6=1 after an OUT 31,13 (=2*6+1) If in 128 mode, contains last OUT to 0x7ffd. If in Timex mode, contains last OUT to 0xf4.
    uint8_t iface_I_rom;        // * 36 | 1  | Contains 0xff if Interface I rom paged. If in Timex mode, contains last OUT to 0xff
    struct {                    // * 37 | 1
        uint8_t emu_R: 1;       // -    Bit 0:   1 if R register emulation on
        uint8_t emu_LDIR: 1;    // -    Bit 1:   1 if LDIR emulation on
        uint8_t AY_used: 1;     // -    Bit 2:   AY sound in use, even on 48K machines
        uint8_t _: 3;           // -    Bit 3-5: Unspecified
        uint8_t AY_fuller: 1;   // -    Bit 6:   (if bit 2 set) Fuller Audio Box emulation
        uint8_t mod_hw: 1;      // -    Bit 7:   Modify hardware (see below)
    };
    uint8_t _0xfffd;            // * 38 | 1  | Last OUT to port 0xfffd (soundchip register number)
    uint8_t sound_regs[16];     // * 39 | 16 | Contents of the sound chip registers
};

static_assert(sizeof(z80_header_v2) == 25, "This is verified fact!");

/*
 * In general, the fields have the same meaning in version 2 and 3 files, with the exception of byte 34:
 *
 * ----------------------------------------
 * Value:   Meaning in v2   Meaning in v3
 * ----------------------------------------
 * 0        48k             48k
 * 1        48k + If.1      48k + If.1
 * 2        SamRam          SamRam
 * 3        128k            48k + M.G.T.
 * 4        128k + If.1     128k
 * 5        -               128k + If.1
 * 6        -               128k + M.G.T.
 *  ----------------------------------------
 *
 */

struct __attribute__((packed)) z80_header_v3 {
    uint16_t t_low;             // _ 55 | 2  | Low T state counter
    uint8_t t_high;             // _ 57 | 1  | Hi T state counter
    uint8_t spectator;          // _ 58 | 1  | Flag byte used by Spectator (QL spec. emulator) Ignored by Z80 when loading, zero when saving
    uint8_t mgt_rom;            // _ 59 | 1  | 0xff if MGT Rom paged
    uint8_t mface_rom;          // _ 60 | 1  | 0xff if Multiface Rom paged. Should always be 0.
    uint8_t rom_ram_0;          // _ 61 | 1  | 0xff if 0-8191 is ROM, 0 if RAM
    uint8_t rom_ram_1;          // _ 62 | 1  | 0xff if 8192-16383 is ROM, 0 if RAM
    uint8_t jk_mapping[10];     // _ 63 | 10 | 5 x keyboard mappings for user defined joystick
    uint8_t jk_keys[10];        // _ 73 | 10 | 5 x ASCII word: keys corresponding to mappings above
    uint8_t mgt_type;           // _ 83 | 1  | MGT type: 0=Disciple+Epson,1=Disciple+HP,16=Plus D
    uint8_t disc_btn;           // _ 84 | 1  | Disciple inhibit button status: 0=out, 0ff=in
    uint8_t disc_flag;          // _ 85 | 1  | Disciple inhibit flag: 0=rom pageable, 0ff=not
    uint8_t _0x1ffd;            //** 86 | 1  | Last OUT to port 0x1ffd
};

static_assert(sizeof(z80_header_v3) == 32, "This is verified fact!");

/*
 * After this header block of 30 bytes the 48K bytes of Spectrum memory follows in a compressed format (if bit 5 of byte 12 is set).
 * The compression method is very simple: it replaces repetitions of at least five equal bytes by a four-byte code
 * ED ED xx yy, which stands for "byte yy repeated xx times". Only sequences of length at least 5 are coded.
 * The exception is sequences consisting of ED's; if they are encountered, even two ED's are encoded into ED ED 02 ED.
 * Finally, every byte directly following a single ED is not taken into a block, for example ED 6*00 is not encoded
 * into ED ED ED 06 00 but into ED 00 ED ED 05 00. The block is terminated by an end marker, 00 ED ED 00.
 *
 * *** Looks like 00 ED ED 00 ignored in V2 & V3 versions. - D.S.
 *
 */

/* In V2|V3 memory stored as blocks sequence:
 *
 * Byte Length  Description
 * ---------------------------------------------------------------------------------------------------------------------------------
 * 0    2       Length of compressed data (without this 3-byte header) If length=0xffff, data is 16384 bytes long and not compressed
 * 2    1       Page number of block
 * 3    [0]     Data
 * ---------------------------------------------------------------------------------------------------------------------------------
 */

/*
 * Page     In '48 mode     In '128 mode    In SamRam mode
 * -------------------------------------------------------
 * 0        48K rom         rom (basic)     48K rom
 * 1        Interface I, Disciple or Plus D rom, according to setting
 * 2        -               rom (reset)     samram rom (basic)
 * 3        -               page_index 0    samram rom (monitor,..)
 * 4        8000-bfff       page_index 1    Normal 8000-bfff
 * 5        c000-ffff       page_index 2    Normal c000-ffff
 * 6        -               page_index 3    Shadow 8000-bfff
 * 7        -               page_index 4    Shadow c000-ffff
 * 8        4000-7fff       page_index 5    4000-7fff
 * 9        -               page_index 6    -
 * 10       -               page_index 7    -
 * 11       Multiface rom   Multiface rom   -
 */


std::expected<int, std::string> ZxS::load_from_z80(std::vector<uint8_t> &&data) noexcept {
    constexpr size_t max_memory_size = 0x100000; // TODO: sure 1M is enough for all models and clones?
    auto i = data.begin();

    auto read = [&i, &data] <typename T> INLINE (T & t, size_t limit = std::numeric_limits<size_t>::max()) -> T& {
        if (auto j = i + std::min(sizeof(T), limit); j <= data.end()) {
            std::copy(i, j, reinterpret_cast<uint8_t*>(&t));
            return i = j, t;
        }

        throw std::out_of_range("Unexpected end of data");
    };

    auto decompress_block = [&i, &read] INLINE (uint16_t compressed_size, uint16_t expected_size) {
        std::vector<uint8_t> decomp;
        uint8_t b0, b1;
        decomp.reserve(max_memory_size);
        auto end = i + compressed_size;

        while (i < end) {
            if (read(b0) != 0xED || i == end) {
                decomp.push_back(b0);
                continue;
            }

            if (read(b1) != 0xED || i == end) {
                decomp.push_back(b0);
                decomp.push_back(b1);
                continue;
            }

            if (read(b0), i == end) [[unlikely]] {
                decomp.push_back(0xED);
                decomp.push_back(0xED);
                decomp.push_back(b0);
                break;
            }

            if (!!b0) [[likely]] {
                decomp.insert(decomp.end(), b0, read(b1));
            } else {
                throw std::out_of_range("Zero RLE count");
            }
        }

        if (expected_size != 0 && decomp.size() != expected_size) {
            throw std::out_of_range(std::format("Unexpected decompressed data size: got {}, expected {}", decomp.size(), expected_size));
        }

        return decomp;
    };

    try {
        enum class FMT {V1, V2, V3} format_version;
        z80_header_v1 hdr_v1{};
        z80_header_v2 hdr_v2{};
        z80_header_v3 hdr_v3{};

        if (read(hdr_v1).PC) {
            format_version = FMT::V1;
        } else if (read(hdr_v2).length < sizeof(hdr_v2)) {
            format_version = FMT::V2;
        } else if (hdr_v2.length == sizeof hdr_v2 + sizeof hdr_v3 - sizeof hdr_v2.length) {
            read(hdr_v3);
            format_version = FMT::V3;
        } else {
            read(hdr_v3, sizeof hdr_v3 - sizeof hdr_v3._0x1ffd);
            format_version = FMT::V3;
        }

        if (format_version == FMT::V1) {
            if (hdr_v1.compressed) {
                if (auto term = data.end() - 4;
                        term <= i ||
                        *(term + 0) != 0x00_u8 ||
                        *(term + 1) != 0xED_u8 ||
                        *(term + 2) != 0xED_u8 ||
                        *(term + 3) != 0x00_u8) [[unlikely]] {
                    return std::unexpected("File is corrupted: missed terminator");
                } else {
                    data.erase(term, data.end());
                    memory.apply_dump(decompress_block(data.end() - i, ZxMemory::Map::ram_size_), ZxMemory::Map::ram_org_);
                }
            } else {
                memory.apply_dump(std::vector(i, data.end()), ZxMemory::Map::ram_org_);
            }
        } else {
            if (hdr_v2.hw_model > 1) { // TODO: 128k!
                event::data<EVENT::SYS_CONFIRM_YES_NO> ed{
                    .title = "",
                    .question = "Currently, only the standard ZX Spectrum 48K is supported.\n"
                                "This snapshot is marked for a different model, but may still work.\n"
                                "Continue?"
                };

                if (event::dispatch(ed); ed.choice != 1) {
                    throw std::out_of_range("Cancelled by user");
                }
            }

            constexpr uint16_t uncompressed_page_size = 0x4000_u16;
            auto read_uncompressed = [&i, &data] INLINE {
                if (auto j = i + uncompressed_page_size; j <= data.end()) {
                    return i = j, std::vector(i, j);
                }

                throw std::out_of_range("Unexpected end of data");
            };

            const std::array<std::optional<uint16_t>, 12> page_addrs {
                0x0000_u16, {/*-I2-*/}, {/*-SR-*/}, {/*-SR-*/},
                0x8000_u16, 0xC000_u16, {/*-SH-*/}, {/*-SH-*/},
                0x4000_u16, {/*----*/}, {/*----*/}, {/*-MF-*/}
            };

            std::unordered_map<uint16_t, std::vector<uint8_t>> pages{ };
            uint16_t block_size;
            uint8_t page_index;

            while (i < data.end()) {
                if (!read(block_size)) {
                    return std::unexpected("File is corrupted: zero block size");
                }

                if (read(page_index) >= page_addrs.size()) {
                    return std::unexpected(std::format("Page index out of bounds: provided {}, expected in range 0..{}", page_index, page_addrs.size() - 1));
                }

                if (auto addr = page_addrs[page_index]; !addr) {
                    return std::unexpected(std::format("Wrong or unsupported page index: {}", page_index));
                } else {
                    if (pages.contains(*addr)) {
                        return std::unexpected(std::format("Page already in use: {}", *addr));
                    }

                    pages[*addr] = block_size != 0xFFFF_u16 ? decompress_block(block_size, uncompressed_page_size) : read_uncompressed();
                }
            }

            for (const auto &pg : pages) {
                memory.apply_dump(pg.second, pg.first);
            }
        }

        auto [_reg, _sh_reg, _ix_reg, _sp_reg, _iff1, _iff2, _imode, _halted] = z80.access_hatch();
        _reg.A = hdr_v1.A;
        _reg.F = hdr_v1.F;
        _reg.BC = hdr_v1.BC;
        _reg.DE = hdr_v1.DE;
        _reg.HL = hdr_v1.HL;
        _sh_reg.A = hdr_v1.A_;
        _sh_reg.F = hdr_v1.F_;
        _sh_reg.BC = hdr_v1.BC_;
        _sh_reg.DE = hdr_v1.DE_;
        _sh_reg.HL = hdr_v1.HL_;
        _ix_reg.IX = hdr_v1.IX;
        _ix_reg.IY = hdr_v1.IY;
        _sp_reg.PC = format_version == FMT::V1 ? hdr_v1.PC : hdr_v2.PC;
        _sp_reg.SP = hdr_v1.SP;
        _sp_reg.I = hdr_v1.I;
        _sp_reg.R = ((hdr_v1.R & 0x7F_u8) | (hdr_v1.R7 << 7));
        _iff1 = !!hdr_v1.iff1;
        _iff2 = !!hdr_v1.iff2;
        _imode = std::clamp(static_cast<Z80::IMode>(hdr_v1.imode), Z80::IMode::IM1, Z80::IMode::IM2);
        ula.border(hdr_v1.border);

        return static_cast<int>(format_version);
    } catch (const std::out_of_range &e) {
        return std::unexpected(e.what());
    }
}

std::expected<std::vector<uint8_t>, std::string> ZxS::z80_snapshot() noexcept {
    auto z80_rle_encode = []
    INLINE (auto source, bool terminator = false) noexcept {
        constexpr uint8_t marker = 0xED_u8;

        std::vector<uint8_t> encoded;
        encoded.reserve(source.size());

        auto emplace_rle = [&encoded, marker]
        INLINE (uint8_t byte, uint8_t count) noexcept {
            if (count > 4 || byte == marker) {
                encoded.emplace_back(marker);
                encoded.emplace_back(marker);
                encoded.emplace_back(count);
                encoded.emplace_back(byte);
            } else {
                while (count--) {
                    encoded.emplace_back(byte);
                }
            }
        };

        auto emplace_sequence = [&emplace_rle]
        INLINE (uint8_t byte, int count) noexcept {
            auto [quot, rem] = div(count, std::numeric_limits<uint8_t>::max());

            while (quot--) {
                emplace_rle(byte, std::numeric_limits<uint8_t>::max());
            }

            if (rem) {
                emplace_rle(byte, rem);
            }
        };

        for (auto chunks = source | std::views::chunk_by(std::equal_to{}); auto const &chunk: chunks) {
            emplace_sequence(chunk[0], chunk.size());
        }

        if (terminator) {
            encoded.emplace_back(0x00_u8);
            encoded.emplace_back(marker);
            encoded.emplace_back(marker);
            encoded.emplace_back(0x00_u8);
        }

        return encoded;
    };

    std::vector<uint8_t> snapshot(sizeof(z80_header_v1));
    z80_header_v1 &hdr_v1 = *reinterpret_cast<z80_header_v1 *>(snapshot.data());

    auto ram = memory.ram();
    auto encoded_ram = z80_rle_encode(ram, true);

    event::data<EVENT::SYS_GET_ACTIVE_JOYSTICK_TYPE> ed { .wkjc = WKJC::UNKNOWN };
    event::dispatch(ed);

    auto [_reg, _sh_reg, _ix_reg, _sp_reg, _iff1, _iff2, _imode, _halted] = z80.access_hatch();
    hdr_v1.A = _reg.A;
    hdr_v1.F = _reg.F;
    hdr_v1.BC = _reg.BC;
    hdr_v1.DE = _reg.DE;
    hdr_v1.HL = _reg.HL;
    hdr_v1.A_ = _sh_reg.A;
    hdr_v1.F_ = _sh_reg.F;
    hdr_v1.BC_ = _sh_reg.BC;
    hdr_v1.DE_ = _sh_reg.DE;
    hdr_v1.HL_ = _sh_reg.HL;
    hdr_v1.IX = _ix_reg.IX;
    hdr_v1.IY = _ix_reg.IY;
    hdr_v1.PC = _sp_reg.PC;
    hdr_v1.SP = _sp_reg.SP;
    hdr_v1.I = _sp_reg.I;
    hdr_v1.R = _sp_reg.R & 0x7F_u8;
    hdr_v1.R7 = _sp_reg.R >> 7;
    hdr_v1.iff1 = _iff1;
    hdr_v1.iff2 = _iff2;
    hdr_v1.imode = static_cast<uint8_t>(_imode);
    hdr_v1.border = ula.border();
    hdr_v1.sam_rom = 0;
    hdr_v1.issue_2 = 0; // TODO: issue_2
    hdr_v1.dbl_ifrq = 0;
    hdr_v1.video_sync = 0;
    hdr_v1.joystick = static_cast<uint8_t>(ed.wkjc);

    if (encoded_ram.size() < ram.size()) {
        hdr_v1.compressed = 1;
        snapshot.insert(snapshot.end(), encoded_ram.begin(), encoded_ram.end());
    } else {
        hdr_v1.compressed = 0;
        snapshot.insert(snapshot.end(), ram.begin(), ram.end());
    }

    return snapshot;
}
