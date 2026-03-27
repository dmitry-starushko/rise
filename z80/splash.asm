                device ZXSPECTRUM48

                org #8000
bitmap_size     equ 192 * 32
attr_size       equ 24 * 32

                di
                jp start
image           incbin "splash-img.bin"

                macro solid_ln line
                if line < 192
                ld hl, #4000 + ((line & 7) shl 8) + ((line & 56) shl 2) + (line & 192) * 32
                ld de, #4001 + ((line & 7) shl 8) + ((line & 56) shl 2) + (line & 192) * 32
                ld bc, 31
                ld (hl), #ff
                ldir
                endif
                endm

                macro copy_ln line
                ld hl, image + ((line & 7) shl 8) + ((line & 56) shl 2) + (line & 192) * 32
                ld de, #4000 + ((line & 7) shl 8) + ((line & 56) shl 2) + (line & 192) * 32
                ld bc, 32
                halt
                ldir
                solid_ln line + 2
                endm

                macro attr_ln n, col
                ld hl, #4000 + bitmap_size + n * 32
                ld de, #4001 + bitmap_size + n * 32
                ld bc, 31
                ld (hl), col | #40
                ldir
                endm

                macro ccell_ln n
                copy_ln n * 8 + 0
                attr_ln n, 1
                copy_ln n * 8 + 1
                attr_ln n, 2
                copy_ln n * 8 + 2
                attr_ln n, 4
                copy_ln n * 8 + 3
                attr_ln n, 2
                copy_ln n * 8 + 4
                attr_ln n, 3
                copy_ln n * 8 + 5
                attr_ln n, 5
                copy_ln n * 8 + 6
                attr_ln n, 6
                copy_ln n * 8 + 7
                ld hl, image + bitmap_size + n * 32
                ld de, #4000 + bitmap_size + n * 32
                ld bc, 32
                ldir
                endm

                macro show_screen
                dup 24, i
                ccell_ln i
                edup
                endm

start           xor a
                out (#fe), a

                ld hl, #4000
                ld de, #4001
                ld bc, bitmap_size - 1
                ld (hl), a
                ldir

                ld hl, #4000 + bitmap_size
                ld de, #4001 + bitmap_size
                ld bc, attr_size - 1
                ld (hl), #47
                ldir

                show_screen

                ld c, 150
await           halt
                dec c
                jr z, final
                xor a
                in a, (#fe)
                and #1f
                cp #1f
                jr z, await
final           jp #0000

                savebin "../bin/splash.bin", #8000, $ - #8000
                end start
        
