#pragma once
#ifndef TZX_H
#define TZX_H

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

/*
 * https://worldofspectrum.net/TZXformat.html
 */

struct __attribute__((packed)) tzx_header {
    char signature[7];
    uint8_t _0x1A;
    uint8_t revision_major;
    uint8_t revision_minor;
};

inline int revision(const tzx_header &h) noexcept {
    return h.revision_major * 1000 + h.revision_minor;
}

enum class BLOCK_ID : uint8_t {
    STD_DATA = 0x10,
    TURBO_DATA = 0x11,
    PURE_TONE = 0x12,
    PULSE_SEQ = 0x13,
    PURE_DATA = 0x14,
    DIRECT_REC = 0x15,
    CSW = 0x18,
    GENERALIZED_DATA = 0x19,
    SILENCE = 0x20,
    GROUP_START = 0x21,
    GROUP_END = 0x22,
    JUMP = 0x23,
    LOOP_START = 0x24,
    LOOP_END = 0x25,
    CALL_SEQ = 0x26,
    RETURN = 0x27,
    SELECT = 0x28,
    STOP_48K = 0x2A,
    SET_LEVEL = 0x2B,
    DESCRIPTION = 0x30,
    MESSAGE = 0x31,
    ARCHIVE_INFO = 0x32,
    HW_TYPE = 0x33,
    CUSTOM = 0x35,
    GLUE = 0x5A,
    UNKNOWN = 0xFF
};

template<BLOCK_ID id> struct blk_header;

/* ID 10 - Standard Speed Data Block
 * ---------------------------------
 * length: [02,03]+04
 * ---------------------------------
 * This block must be replayed with the standard Spectrum ROM timing values - see the values in curly brackets in block ID 11.
 * The pilot tone consists in 8063 pulses if the first data byte (flag byte) is < 128, 3223 otherwise. This block can be used
 * for the ROM loading routines AND for custom loading routines that use the same timings as ROM ones do.
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::STD_DATA> {
    uint16_t pause_after;       // -- Pause after this block (ms.) {1000}
    uint16_t length;            // -- Length of data that follow
    /* uint8_t [this->lenght]   // -- Data as in .TAP files */
};

/* ID 11 - Turbo Speed Data Block
 * ------------------------------
 * length: [0F,10,11]+12
 * ------------------------------
 * This block is very similar to the normal TAP block but with some additional info on the timings and other important differences.
 * The same tape encoding is used as for the standard speed data block. If a block should use some non-standard sync or pilot tones
 * (i.e. all sorts of protection schemes) then use the next three blocks to describe it.
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::TURBO_DATA> {
    uint16_t pilot_pulse_tck;   // -- Length of PILOT pulse {2168}
    uint16_t sync_p1_tck;       // -- Length of SYNC first pulse {667}
    uint16_t sync_p2_tck;       // -- Length of SYNC second pulse {735}
    uint16_t zero_pulse_tck;    // -- Length of ZERO bit pulse {855}
    uint16_t one_pulse_tck;     // -- Length of ONE bit pulse {1710}
    uint16_t pilot_pulses;      // -- Length of PILOT tone (number of pulses) {8063 header (flag<128), 3223 data (flag>=128)}
    uint8_t last_byte_bits;     // -- Used bits in the last byte (other bits should be 0) {8} (e.g. if this is 6, then the bits used (x) in the last byte are: xxxxxx00, where MSb is the leftmost bit, LSb is the rightmost bit)
    uint16_t pause_after;       // -- WORD   Pause after this block (ms.) {1000}
    uint8_t length[3];          // -- Length of data that follow
    /* uint8_t [this->lenght]   // -- Data as in .TAP files */
};


/* ID 12 - Pure Tone
 * ------------------
 * length: 04
 * ------------------
 * This will produce a tone which is basically the same as the pilot tone in the ID 10, ID 11 blocks.
 * You can define how long the pulse is and how many pulses are in the tone.
 *
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::PURE_TONE> {
    uint16_t pulse_tck;        // -   WORD    Length of one pulse in T-states
    uint16_t pulses;           // -   WORD    Number of pulses
};

/* ID 13 - Pulse sequence
 * -----------------------
 * length: [00]*02+01
 * -----------------------
 * This will produce N pulses, each having its own timing. Up to 255 pulses can be stored in this block;
 * this is useful for non-standard sync tones used by some protection schemes.
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::PULSE_SEQ> {
    uint8_t length;             // -- Number of pulses
    /* uint16_t _[N] */         // -- Pulses' lengths

};

/*  ID 14 - Pure Data Block
 *  ------------------------
 *  length: [07,08,09]+0A
 *  ------------------------
 *  This is the same as in the turbo loading data block, except that it has no pilot or sync pulses.
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::PURE_DATA> {
    uint16_t zero_pulse_tck;    // -- Length of ZERO bit pulse
    uint16_t one_pulse_tck;     // -- Length of ONE bit pulse
    uint8_t last_byte_bits;     // -- Used bits in last byte (other bits should be 0) (e.g. if this is 6, then the bits used (x) in the last byte are: xxxxxx00, where MSb is the leftmost bit, LSb is the rightmost bit)
    uint16_t pause_after;       // -- Pause after this block (ms.)
    uint8_t length[3];          // -- Length of data that follow
    /* uint8_t _[length] */     // -- Data as in .TAP files
};

/* ID 15 - Direct Recording
 * -------------------------
 * length: [05,06,07]+08
 * -------------------------
 * This block is used for tapes which have some parts in a format such that the turbo loader block cannot be used.
 * This is not like a VOC file, since the information is much more compact. Each sample value is represented by
 * one bit only (0 for low, 1 for high) which means that the block will be at most 1/8 the size of the equivalent VOC.
 *
 * The preferred sampling frequencies are 22050 or 44100 Hz (158 or 79 T-states/sample). Please, if you can, don't
 * use other sampling frequencies.
 *
 * Please use this block only if you cannot use any other block.
*/

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::DIRECT_REC> {
    uint16_t ticks_per_sample;  // -- Number of T-states per sample (bit of data)
    uint16_t pause_after;       // -- Pause after this block in milliseconds (ms.)
    uint8_t last_byte_bits;     // -- Used bits (samples) in last byte of data (1-8) (e.g. if this is 2, only first two samples of the last byte will be played)
    uint8_t length[3];          // -- N Length of samples' data
    /* uint8_t _[length] */     // -- Samples data. Each bit represents a state on the EAR port (i.e. one sample). MSb is played first.

};

/* ID 18 - CSW Recording
 * -------------------------
 * length: [00,01,02,03]+04
 * -------------------------
 * This block contains a sequence of raw pulses encoded in CSW format v2 (Compressed Square Wave).
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::CSW> {
    uint16_t block_length;      // -- 10+N   Block length (without these four bytes)
    uint16_t pause_after;       // -- Pause after this block (in ms).
    uint8_t sample_rate[3];     // -- Sampling rate
    uint8_t compression_type;   // -- Compression type 0x01: RLE 0x02: Z-RLE
    uint32_t pulses;            // -- Number of stored pulses (after decompression, for validation purposes)
    /* uint8_t _[N] */          // -- CSW data, encoded according to the CSW file format specification. // WTF?!!!!!!!!!!!!

};

/* ID 19 - Generalized Data Block
 * -------------------------------
 * length: [00,01,02,03]+04
 * -------------------------------
 * This block has been specifically developed to represent an extremely wide range of data encoding techniques.
 * The basic idea is that each loading component (pilot tone, sync pulses, data) is associated to a specific
 * sequence of pulses, where each sequence (wave) can contain a different number of pulses from the others.
 * In this way we can have a situation where bit 0 is represented with 4 pulses and bit 1 with 8 pulses.
 *
 * SYMDEF structure format
 * .......................................................
 * Offset   Value   Type        Description
 * .......................................................
 * 0x00     -       BYTE        Symbol flags
 *          b0-b1:              starting symbol polarity
 *          00:                 opposite to the current level (make an edge, as usual) - default
 *          01:                 same as the current level (no edge - prolongs the previous pulse)
 *          10:                 force low level
 *          11:                 force high level
 * 0x01     -       WORD[MAXP]  Array of pulse lengths.
 *
 * The alphabet is stored using a table where each symbol is a row of pulses. The number of columns (i.e. pulses)
 * of the table is the length of the longest sequence amongst all (MAXP=NPP or NPD, for pilot/sync or data blocks
 * respectively); shorter waves are terminated by a zero-length pulse in the sequence.
 *
 * Any number of data symbols is allowed, so we can have more than two distinct waves; for example, imagine a
 * loader which writes two bits at a time by encoding them with four distinct pulse lengths: this loader would have
 * an alphabet of four symbols, each associated to a specific sequence of pulses (wave).
 *
 * PRLE structure format
 * ......................................................
 * Offset   Value   Type        Description
 * ......................................................
 * 0x00     -       BYTE        Symbol to be represented
 * 0x01     -       WORD        Number of repetitions
 * ......................................................
 *
 * Most commonly, pilot and sync are repetitions of the same pulse, thus they are represented using a very simple RLE
 * encoding structure which stores the symbol and the number of times it must be repeated.
 *
 * Each symbol in the data stream is represented by a string of NB bits of the block data, where NB = ceiling(Log2(ASD)).
 * Thus the length of the whole data stream in bits is NB*TOTD, or in bytes DS=ceil(NB*TOTD/8).
 *
 * --------
 * Example
 * --------
 *
 * A typical Spectrum's standard loading header can be represented like this:
 *
 * .....................................................................................................
 * Offset   Value                           Description
 * .....................................................................................................
 * 0x00     59 (0x3B)                       Total block length
 * 0x04     1000 (0x03E8)                   Pause after this block (ms.)
 * 0x06     2                               Total number of symbols in pilot/sync block
 * 0x0A     2                               Max pulses per symbol
 * 0x0B     2                               Number of symbols in pilot/sync alphabet
 * 0x0C     152 (0x98)                      Total number of symbols (bits, in this case) in data stream
 * 0x10     2                               Max pulses per data symbol
 * 0x11     2                               Number of symbols in data alphabet
 * 0x12     SYMDEF[0]: (0, 2168, 0)
 *          SYMDEF[1]: (0, 667, 735)        Pilot/sync symbol definitions
 * 0x1C     PRLE[0]: (0, 8063)
 *          PRLE[1]: (1, 1)                 Pilot/sync data block
 * 0x22     SYMDEF[0]: (0, 855, 855)
 *          SYMDEF[1]: ( 0, 1710, 1710)     Data symbol definitions
 * 0x2C     00000000 - 0x00                 Data stream:
 *          00000011 - 0x03                 00: loading flag (0x00 = header)
 *          01001010 - 0x4A                 03: data type (0x03 = "Bytes:")
 *          01010000 - 0x50                 4A,50,53,50,20,20,20,20,20,20: file name ("JPSP")
 *          01010011 - 0x53                 1B00: data length
 *          01010000 - 0x50                 4000: start address
 *          00100000 - 0x20                 8000: autostart (no meaning here)
 *          00100000 - 0x20                 C1: checksum
 *          00100000 - 0x20
 *          00100000 - 0x20
 *          00100000 - 0x20
 *          00100000 - 0x20
 *          00000000 - 0x00
 *          00011011 - 0x1B
 *          00000000 - 0x00
 *          01000000 - 0x40
 *          00000000 - 0x00
 *          10000000 - 0x80
 *          11000001 - 0xC1
 * .....................................................................................................
 *
 * The very same data information encoded to work with the ZX81 would look like this:
 *
 * .....................................................................................................
 * Offset   Value                           Description
 * .....................................................................................................
 * 0x00     107 (0x6B)                      Total block length
 * 0x04     1000 (0x03E8)                   Pause after this block (ms.)
 * 0x06     0                               Total number of records in pilot/sync block
 * 0x0A     0                               Max pulses per symbol
 * 0x0B     0                               Number of symbols in pilot/sync alphabet
 * 0x0C     152 (0x98)                      Total number of records in data stream
 * 0x10     18 (0x12)                       Max pulses per data symbol
 * 0x11     2                               Number of symbols in data alphabet
 * 0x12     SYMDEF[0]: (3, 530, 520, 530,   Data symbol definitions
 *          520, 530, 520, 530, 4689, 0, 0, (referred to 3.5MHz clock frequency)
 *          0, 0, 0, 0, 0, 0, 0, 0 )
 *          SYMDEF[1]: (3, 530, 520, 530,
 *          520, 530, 520, 530, 520, 530,
 *          520, 530, 520, 530, 520, 530,
 *          520, 530, 4689)
 * 0x5C     00000000 - 0x00                 Data stream
 *          00000011 - 0x03
 *          01001010 - 0x4A
 *          01010000 - 0x50
 *          01010011 - 0x53
 *          01010000 - 0x50
 *          00100000 - 0x20
 *          00100000 - 0x20
 *          00100000 - 0x20
 *          00100000 - 0x20
 *          00100000 - 0x20
 *          00100000 - 0x20
 *          00000000 - 0x00
 *          00011011 - 0x1B
 *          00000000 - 0x00
 *          01000000 - 0x40
 *          00000000 - 0x00
 *          10000000 - 0x80
 *          11000001 - 0xC1
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::GENERALIZED_DATA> {
    uint32_t block_length;      // -- Block length (without these four bytes)
    uint16_t pause_after;       // -- Pause after this block (ms)
    uint32_t totp;              // -- TOTP Total number of symbols in pilot/sync block (can be 0)
    uint8_t npp;                // -- NPP Maximum number of pulses per pilot/sync symbol
    uint8_t asp;                // -- ASP Number of pilot/sync symbols in the alphabet table (0=256)
    uint32_t totd;              // -- TOTD Total number of symbols in data stream (can be 0)
    uint8_t npd;                // -- NPD Maximum number of pulses per data symbol
    uint8_t asd;                // -- ASD Number of data symbols in the alphabet table (0=256)
    // 0x12                                                 -   SYMDEF[ASP]     Pilot and sync symbols definition table This field is present only if TOTP>0
    // 0x12+(2*NPP+1)*ASP                                   -   PRLE[TOTP]      Pilot and sync data stream This field is present only if TOTP>0
    // 0x12+(TOTP>0)*((2*NPP+1)*ASP)+TOTP*3                 -   SYMDEF[ASD]     Data symbols definition table This field is present only if TOTD>0
    // 0x12+(TOTP>0)*((2*NPP+1)*ASP)+TOTP*3+(2*NPD+1)*ASD   -   BYTE[DS]        Data stream This field is present only if TOTD>0

};

/* ID 20 - Pause (silence) or 'Stop the Tape' command
 * ---------------------------------------------------
 * length: 02
 * ---------------------------------------------------
 * This will make a silence (low amplitude level (0)) for a given time in milliseconds.
 * If the value is 0 then the emulator or utility should (in effect) STOP THE TAPE, i.e.
 * should not continue loading until the user or emulator requests it.
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::SILENCE> {
    uint16_t duration_ms;       // -- Pause duration (ms.)
};

/* ID 21 - Group start
 * --------------------
 * length: [00]+01
 * --------------------
 * This block marks the start of a group of blocks which are to be treated as one single (composite) block.
 * This is very handy for tapes that use lots of subblocks like Bleepload (which may well have over 160 custom
 * loading blocks). You can also give the group a name (example 'Bleepload Block 1').
 *
 * For each group start block, there must be a group end block. Nesting of groups is not allowed.
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::GROUP_START> {
    uint8_t length;             // -- L Length of the group name string
    /* char _[length] */        // -- Group name in ASCII format (please keep it under 30 characters long)
};

/* ID 22 - Group end
 * ------------------
 * length: 00
 * ------------------
 * This indicates the end of a group. This block has no body.
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::GROUP_END> { };

/* ID 23 - Jump to block
 * ----------------------
 * length: 02
 * ----------------------
 * This block will enable you to jump from one block to another within the file. The value is a signed short
 * word (usually 'signed short' in C);
 * Some examples:
 *     Jump 0 = 'Loop Forever' - this should never happen
 *     Jump 1 = 'Go to the next block' - it is like NOP in assembler ;)
 *     Jump 2 = 'Skip one block'
 *     Jump -1 = 'Go to the previous block'
 *
 * All blocks are included in the block count!
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::JUMP> {
    int16_t offset;             // -- Relative jump value
};

/* ID 24 - Loop start
 * -------------------
 * length: 02
 * -------------------
 * If you have a sequence of identical blocks, or of identical groups of blocks, you can use this block
 * to tell how many times they should be repeated. This block is the same as the FOR statement in BASIC.
 *
 * For simplicity reasons don't nest loop blocks!
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::LOOP_START> {
    uint16_t repetitions;       // -- Number of repetitions (greater than 1)
};

/* ID 25 - Loop end
 * -----------------
 * length: 00
 * -----------------
 * This is the same as BASIC's NEXT statement. It means that the utility should jump back to the start
 * of the loop if it hasn't been run for the specified number of times.
 *
 * This block has no body.
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::LOOP_END> { };

/* ID 26 - Call sequence
 * ----------------------
 * length: [00,01]*02+02
 * ----------------------
 * This block is an analogue of the CALL Subroutine statement. It basically executes a sequence of
 * blocks that are somewhere else and then goes back to the next block. Because more than one call can
 * be normally used you can include a list of sequences to be called. The 'nesting' of call blocks is also
 * not allowed for the simplicity reasons. You can, of course, use the CALL blocks in the LOOP sequences and
 * vice versa. The value is relative for the obvious reasons - so that you can add some blocks in the beginning
 * of the file without disturbing the call values. Please take a look at 'Jump To Block' for reference on the values.
*/

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::CALL_SEQ> {
    uint16_t call_num;          // -- N Number of calls to be made
    /* int16_t _[N] */          // -- Array of call block numbers (relative-signed offsets)
};

/* ID 27 - Return from sequence
 * -----------------------------
 * length: 00
 * -----------------------------
 * This block indicates the end of the Called Sequence. The next block played will be the block after the last
 * CALL block (or the next Call, if the Call block had multiple calls).
 *
 * Again, this block has no body.
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::RETURN> { };

/* ID 28 - Select block
 * ---------------------
 * length: [00,01]+02
 * ---------------------
 * This block is useful when the tape consists of two or more separately-loadable parts. With this block,
 * you are able to select one of the parts and the utility/emulator will start loading from that block.
 * For example you can use it when the game has a separate Trainer or when it is a multiload. Of course,
 * to make some use of it the emulator/utility has to show a menu with the selections when it encounters
 * such a block. All offsets are relative signed words.
 *
 * SELECT structure format
 * ....................................................................................
 * Offset   Value   Type    Description
 * ....................................................................................
 * 0x00     -       WORD    Relative Offset
 * 0x02     L       BYTE    Length of description text
 * 0x03     -       CHAR[L] Description text (please use single line and max. 30 chars)
 * ....................................................................................
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::SELECT> {
    uint16_t block_length;      // -- Length of the whole block (without these two bytes)
    uint8_t opt_count;          // -- N Number of selections
    /*SELECT _[alt_count];*/    // -- List of selections
    struct __attribute__((packed)) SELECT {
                int16_t offset;
                uint8_t length;
                /*char _[length]*/
    };
};

/* ID 2A - Stop the tape if in 48K mode
 * -------------------------------------
 * length: 04
 * -------------------------------------
 * When this block is encountered, the tape will stop ONLY if the machine is an 48K Spectrum.
 * This block is to be used for multiloading games that load one level at a time in 48K mode,
 * but load the entire tape at once if in 128K mode.
 *
 * This block has no body of its own, but follows the extension rule.
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::STOP_48K> {
    uint32_t block_length;      // --  0 Length of the block without these four bytes (0)
};

/* ID 2B - Set signal level
 * -------------------------
 * length: 05
 * -------------------------
 * This block sets the current signal level to the specified value (high or low). It should be used
 * whenever it is necessary to avoid any ambiguities, e.g. with custom loaders which are
 * level-sensitive.
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::SET_LEVEL> {
    uint32_t block_length;      // -- 1 Block length (without these four bytes)
    uint8_t level;              // -- Signal level (0=low, 1=high)
};

/* ID 30 - Text description
 * -------------------------
 * length: [00]+01
 * -------------------------
 * This is meant to identify parts of the tape, so you know where level 1 starts, where to rewind to when the
 * game ends, etc. This description is not guaranteed to be shown while the tape is playing, but can be read
 * while browsing the tape or changing the tape pointer.
 *
 * The description can be up to 255 characters long but please keep it down to about 30 so the programs can
 * show it in one line (where this is appropriate).
 *
 * Please use 'Archive Info' block for title, authors, publisher, etc.
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::DESCRIPTION> {
    uint8_t length;             // -- N Length of the text description
    /* char _[length] */        // -- Text description in ASCII format
};

/* ID 31 - Message block
 * ----------------------
 * length: [01]+02
 * ----------------------
 * This will enable the emulators to display a message for a given time. This should not
 * stop the tape and it should not make silence. If the time is 0 then the emulator should
 * wait for the user to press a key.
 *
 * The text message should:
 * stick to a maximum of 30 chars per line;
 * use single 0x0D (13 decimal) to separate lines;
 * stick to a maximum of 8 lines.
 *
 * If you do not obey these rules, emulators may display your message in any way they like.
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::MESSAGE> {
    uint8_t show_time;          // -- Time (in seconds) for which the message should be displayed
    uint8_t length;             // -- N Length of the text message
    /* char _[length] */        // -- Message that should be displayed in ASCII format
};

/* ID 32 - Archive info
 * ---------------------
 * length: [00,01]+02
 * ---------------------
 * Use this block at the beginning of the tape to identify the title of the game, author, publisher, year of
 * publication, price (including the currency), type of software (arcade adventure, puzzle, word processor, ...),
 * protection scheme it uses (Speedlock 1, Alkatraz, ...) and its origin (Original, Budget re-release, ...), etc.
 * This block is built in a way that allows easy future expansion. The block consists of a series of text strings.
 * Each text has its identification number (which tells us what the text means) and then the ASCII text.
 * To make it possible to skip this block, if needed, the length of the whole block is at the beginning of it.
 *
 * If all texts on the tape are in English language then you don't have to supply the 'Language' field
 *
 * The information about what hardware the tape uses is in the 'Hardware Type' block, so no need for it here.

 * TEXT structure format
 * .......................................................
 * Offset   Value   Type    Description
 * .......................................................
 * 0x00     -       BYTE    Text identification byte:
 *          00 - Full title
 *          01 - Software house/publisher
 *          02 - Author(s)
 *          03 - Year of publication
 *          04 - Language
 *          05 - Game/utility type
 *          06 - Price
 *          07 - Protection scheme/loader
 *          08 - Origin
 *          FF - Comment(s)
 * 0x01     L       BYTE    Length of text string
 * 0x02     -       CHAR[L] Text string in ASCII format
 * .......................................................
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::ARCHIVE_INFO> {
    uint16_t block_length;      // -- Length of the whole block (without these two bytes)
    uint8_t str_count;          // -- N Number of text strings
    /*TEXT _[str_count];*/      // -- List of text strings
    struct __attribute__((packed)) TEXT {
                uint8_t kind;
                uint8_t length;
                /* char _ [length];*/
    };
};

/* ID 33 - Hardware type
 * ----------------------
 * length: [00]*03+01
 * ----------------------
 *
 * HWINFO structure format
 * ....................................................................................
 * Offset   Value   Type    Description
 * ....................................................................................
 * 0x00     -       BYTE    Hardware type
 * 0x01     -       BYTE    Hardware ID
 * 0x02     -       BYTE    Hardware information:
 *          00              - The tape RUNS on this machine or with this hardware, but may or may not
 *                            use the hardware or special features of the machine.
 *          01              - The tape USES the hardware or special features of the machine, such as
 *                            extra memory or a sound chip.
 *          02              - The tape RUNS but it DOESN'T use the hardware or special features of the
 *                            machine.
 *          03              - The tape DOESN'T RUN on this machine or with this hardware.
 * ....................................................................................
 *
 * This blocks contains information about the hardware that the programs on this tape use. Please include
 * only machines and hardware for which you are 100% sure that it either runs (or doesn't run) on or with,
 * or you know it uses (or doesn't use) the hardware or special features of that machine.
 *
 * If the tape runs only on the ZX81 (and TS1000, etc.) then it clearly won't work on any Spectrum or Spectrum
 * variant, so there's no need to list this information.
 *
 * If you are not sure or you haven't tested a tape on some particular machine/hardware combination then do
 * not include it in the list.
 *
 * The list of hardware types and IDs is somewhat large, and may be found at the end of the format description.
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::HW_TYPE> {
    uint8_t hwi_count;          // -- N Number of machines and hardware types for which info is supplied
    /*HWINFO _[hwi_count]*/     // -- List of machines and hardware
    struct __attribute__((packed)) HWINFO {
                uint8_t hw_type;
                uint8_t hw_id;
                uint8_t hw_info;
    };
};

/* ID 35 - Custom info block
 * -------------------------
 * length: [10,11,12,13]+14
 * -------------------------
 * This block can be used to save any information you want. For example, it might contain some information
 * written by a utility, extra settings required by a particular emulator, or even poke data.
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::CUSTOM> {
    char ids[10];               // -- Identification string (in ASCII)
    uint32_t length;            // -- L Length of the custom info
    /*uint8_t _[length]*/       // -- Custom info
};

/* ID 5A - "Glue" block
 * ---------------------
 * length: 09
 * ---------------------
 * Just skip these 9 bytes and you will end up on the next ID.
 *
 * This block is generated when you merge two ZX Tape files together. It is here so that you can easily copy
 * the files together and use them. Of course, this means that resulting file would be 10 bytes longer than
 * if this block was not used. All you have to do if you encounter this block ID is to skip next 9 bytes.
 *
 * If you can avoid using this block for this purpose, then do so; it is preferable to use a utility to join
 * the two files and ensure that they are both of the higher version number.
 */

template<>
struct __attribute__((packed)) blk_header<BLOCK_ID::GLUE> {
    uint8_t _[9];// -- Value: { "XTape!", 0x1A, MajR, MinR }
};

#endif // TZX_H
