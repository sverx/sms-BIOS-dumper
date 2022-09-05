/* *********************************************************************
      SMS BIOS dump program
************************************************************************ */

#define LAST_SEG   3     //  4 segments total = 128 KB

#define CODE_IN_RAM_SIZE  256
#define TEMP_BUF_SIZE     (4*1024)

#include <stdio.h>
#include <stdbool.h>
#include "../SMSlib/SMSlib.h"

unsigned char code_in_RAM[CODE_IN_RAM_SIZE];   // 256 bytes should be enough for everything
unsigned char temp_buf[TEMP_BUF_SIZE];
unsigned char seg;
unsigned int kp;
unsigned char BIOS_bank_to_map;

void ldir_BIOS_SRAM_seg0 (void) __naked {
  /* *************************
     NOTE: this code will be copied to RAM and run from there!
     *************************  */
  __asm
    di               ; interrupts should be disabled!

    ; 1st half ***********

    ld hl,#0x0000    ; src (BIOS)
    ld de,#0x8000    ; dst (SRAM)
    ld b,#4          ; 4 times (4 times 4KB)

outloop1:
    push bc
      ld a,#0b11100011    ; reset bits 3,4 (enable BIOS/RAM) and set bits 5,6,7 (to disable card/cartridge/expansion)
      out (#0x3E),a       ; do!

      push de
        ld de,#_temp_buf         ; dst (RAM)
        ld bc,#TEMP_BUF_SIZE     ; 4K
        ldir

        ld a,#0b10101011  ; reset bits 4,6 (enable RAM/cartridge) and set bits 3,5,7 (to disable BIOS/card/expansion)
        ; ld a,(_SMS_Port3EBIOSvalue)
        out (#0x3E),a     ; restore it

      pop de

      ld a,#0x08       ; select SRAM bank 1
      ld (#0xfffc),a

      push hl
        ld hl,#_temp_buf         ; src (RAM)
        ld bc,#TEMP_BUF_SIZE     ; 4K
        ldir
      pop hl

      ld a,#0x00       ; disable SRAM
      ld (#0xfffc),a

    pop bc
    djnz outloop1

    ; 2nd half ***********

    ld b,#4          ; 4 times
    ld de,#0x8000    ; dst (SRAM)

outloop2:
    push bc

      ld a,#0b11100011    ; reset bits 3,4 (enable BIOS/RAM) and set bits 5,6,7 (to disable card/cartridge/expansion)
      out (#0x3E),a       ; do!

      push de
        ld de,#_temp_buf         ; dst (RAM)
        ld bc,#TEMP_BUF_SIZE     ; 4K
        ldir

        ld a,#0b10101011  ; reset bits 4,6 (enable RAM/cartridge) and set bits 3,5,7 (to disable BIOS/card/expansion)
        ; ld a,(_SMS_Port3EBIOSvalue)
        out (#0x3E),a     ; restore it

      pop de

      ld a,#0x0C       ; select SRAM bank 2
      ld (#0xfffc),a

      push hl
        ld hl,#_temp_buf         ; src (RAM)
        ld bc,#TEMP_BUF_SIZE     ; 4K
        ldir
      pop hl

      ld a,#0x00       ; disable SRAM
      ld (#0xfffc),a

    pop bc
    djnz outloop2

    ei               ; re-enable interrupts!

    ret              ; because I am naked ;)
  __endasm;
}

void ldir_BIOS_SRAM_segx (void) __naked {
  /* *************************
     NOTE: this code will be copied to RAM and run from there!
     *************************  */
  __asm
    di               ; interrupts should be disabled!

    ; 1st half ***********

    ld hl,#0x8000    ; src (BIOS)
    ld de,#0x8000    ; dst (SRAM)
    ld b,#4          ; 4 times (4 times 4KB)

outloop1_x:
    push bc
      ld a,#0b11100011    ; reset bits 3,4 (enable BIOS/RAM) and set bits 5,6,7 (to disable card/cartridge/expansion)
      out (#0x3E),a       ; do!

      ld a,(#_BIOS_bank_to_map)
      ld (#0xffff),a   ; map requested BIOS ROM bank

      push de
        ld de,#_temp_buf         ; dst (RAM)
        ld bc,#TEMP_BUF_SIZE     ; 4K
        ldir

        ld a,#0b10101011  ; reset bits 4,6 (enable RAM/cartridge) and set bits 3,5,7 (to disable BIOS/card/expansion)
        ; ld a,(_SMS_Port3EBIOSvalue)
        out (#0x3E),a     ; restore it
      pop de

      ld a,#0x08       ; select SRAM bank 1
      ld (#0xfffc),a

      push hl
        ld hl,#_temp_buf         ; src (RAM)
        ld bc,#TEMP_BUF_SIZE     ; 4K
        ldir
      pop hl

      ld a,#0x00       ; disable SRAM
      ld (#0xfffc),a

    pop bc
    djnz outloop1_x

    ; 2nd half ***********

    ld hl,#0x8000    ; src (BIOS)
    ld de,#0x8000    ; dst (SRAM)
    ld b,#4          ; 4 times (4 times 4KB)

outloop2_x:
    push bc

      ld a,#0b11100011    ; reset bits 3,4 (enable BIOS/RAM) and set bits 5,6,7 (to disable card/cartridge/expansion)
      out (#0x3E),a       ; do!

      ld a,(#_BIOS_bank_to_map)
      inc a
      ld (#0xffff),a   ; map requested BIOS ROM bank (the next one in this case)

      push de
        ld de,#_temp_buf         ; dst (RAM)
        ld bc,#TEMP_BUF_SIZE     ; 4K
        ldir

        ld a,#0b10101011  ; reset bits 4,6 (enable RAM/cartridge) and set bits 3,5,7 (to disable BIOS/card/expansion)
        ; ld a,(_SMS_Port3EBIOSvalue)
        out (#0x3E),a     ; restore it
      pop de

      ld a,#0x0C       ; select SRAM bank 2
      ld (#0xfffc),a

      push hl
        ld hl,#_temp_buf         ; src (RAM)
        ld bc,#TEMP_BUF_SIZE     ; 4K
        ldir
      pop hl

      ld a,#0x00       ; disable SRAM
      ld (#0xfffc),a

    pop bc
    djnz outloop2_x

    ei               ; re-enable interrupts!

    ret              ; because I am naked ;)
  __endasm;
}


void dump_BIOS_seg0 (void) __naked {
  __asm
    ld hl,#_ldir_BIOS_SRAM_seg0
    ld de,#_code_in_RAM
    ld bc,#CODE_IN_RAM_SIZE
    ldir                           ; copy code in RAM
    jp _code_in_RAM
  __endasm;
}

void dump_BIOS_segx (void) __naked {
  __asm
    ld hl,#_ldir_BIOS_SRAM_segx
    ld de,#_code_in_RAM
    ld bc,#CODE_IN_RAM_SIZE
    ldir                           ; copy code in RAM
    jp _code_in_RAM
  __endasm;
}



void BIOS_dump (unsigned char segment) {
  if (segment==0)
    dump_BIOS_seg0();
  else {
    BIOS_bank_to_map=(segment*2);
    dump_BIOS_segx();
  }
}

void main (void) {
  SMS_autoSetUpTextRenderer();
  SMS_VRAMmemset(XYtoADDR(0,0),0,32*24*2);

  for (;;) {
    SMS_waitForVBlank();
    SMS_setNextTileatXY(2,2);
    printf ("DUMP segment : %d",seg);

    kp=SMS_getKeysPressed();
    if ((kp & PORT_A_KEY_UP) && (seg>0))
      seg--;
    else if ((kp & PORT_A_KEY_DOWN) && (seg<LAST_SEG))
      seg++;
    else if (kp & PORT_A_KEY_1) {
      BIOS_dump(seg);
      SMS_setNextTileatXY(2,4);
      printf ("Segment %d dumped.",seg);
    }
  }
}

SMS_EMBED_SEGA_ROM_HEADER_16KB(9999,0);
SMS_EMBED_SDSC_HEADER_AUTO_DATE_16KB(1,0,"sverx","BIOSdump","128KB BIOS dumper for ED X series");
