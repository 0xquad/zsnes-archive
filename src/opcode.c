/*
Copyright (C) 1997-2005 ZSNES Team ( zsKnight, _Demo_, pagefault, Nach )

http://www.zsnes.com
http://sourceforge.net/projects/zsnes

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#include <stdio.h>
#include "unixdbg.h"

extern u32 linear_address(u8 bank, u16 offset);
extern u8 *romdata;



/* use_mx field */
#define FLAG_NONE   0
#define FLAG_M      1
#define FLAG_X      2

/* mode field */
#define MODE_BLOCK_MOVE                     0       /* $12          */
#define MODE_INTERRUPT                      1       /* $12          */
#define MODE_IMPLIED                        2       /* (no display) */
#define MODE_IMMEDIATE                      3       /* #$1234       */
#define MODE_PC_RELATIVE                    4       /* $9abc        */
#define MODE_PC_RELATIVE_LONG               5       /* $c0abcd      */
#define MODE_STACK_RELATIVE                 6       /* $12,S        */
#define MODE_STACK_INDIRECT_INDEXED         7       /* ($12,S),Y    */
#define MODE_DIRECT                         8       /* $12          */
#define MODE_DIRECT_INDEXED                 9       /* $12,X        */
#define MODE_DIRECT_INDEXED_INDIRECT        10      /* ($12,X)      */
#define MODE_DIRECT_INDIRECT_INDEXED        11      /* ($12),Y      */
#define MODE_DIRECT_INDIRECT                12      /* ($12)        */
#define MODE_DIRECT_INDIRECT_LONG           13      /* [$12]        */
#define MODE_DIRECT_INDIRECT_LONG_INDEXED   14      /* [$12],Y      */
#define MODE_ABSOLUTE                       15      /* $1234        */
#define MODE_ABSOLUTE_INDIRECT              16      /* ($1234)      */
#define MODE_ABSOLUTE_INDEXED_X             17      /* $1234,X      */
#define MODE_ABSOLUTE_INDEXED_Y             18      /* $1234,Y      */
#define MODE_ABSOLUTE_LONG                  19      /* $123456      */
#define MODE_ABSOLUTE_LONG_INDEXED          20      /* $123456,X    */
#define MODE_ABSOLUTE_INDEXED_INDIRECT      21      /* ($1234,X)    */
#define MODE_ABSOLUTE_INDIRECT_LONG         22      /* [$1234]      */


struct opcode_t {
    u8 size;                /* Size of instruction. (minimal) */
    const char *mnemonic;   /* Name of opcode. */
    int mode;               /* One of MODE_*. */
    int use_mx;             /* Whether this instruction uses the m/x-flag. */
} opcodes[] = {
    { 2, "BRK", MODE_INTERRUPT,                                 FLAG_NONE },
    { 2, "ORA", MODE_DIRECT_INDEXED_INDIRECT,                   FLAG_NONE },
    { 2, "COP", MODE_INTERRUPT,                                 FLAG_NONE },
    { 2, "ORA", MODE_STACK_RELATIVE,                            FLAG_NONE },
    { 2, "TSB", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "ORA", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "ASL", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "ORA", MODE_DIRECT_INDIRECT_LONG,                      FLAG_NONE },
    { 1, "PHP", MODE_IMPLIED,                                   FLAG_NONE },
    { 2, "ORA", MODE_IMMEDIATE,                                 FLAG_M    },
    { 1, "ASL", MODE_IMPLIED,                                   FLAG_NONE },
    { 1, "PHD", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "TSB", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "ORA", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "ASL", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 4, "ORA", MODE_ABSOLUTE_LONG,                             FLAG_NONE },
    { 2, "BPL", MODE_PC_RELATIVE,                               FLAG_NONE },
    { 2, "ORA", MODE_DIRECT_INDIRECT_INDEXED,                   FLAG_NONE },
    { 2, "ORA", MODE_DIRECT_INDIRECT,                           FLAG_NONE },
    { 2, "ORA", MODE_STACK_INDIRECT_INDEXED,                    FLAG_NONE },
    { 2, "TRB", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "ORA", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "ASL", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "ORA", MODE_DIRECT_INDIRECT_LONG_INDEXED,              FLAG_NONE },
    { 1, "CLC", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "ORA", MODE_ABSOLUTE_INDEXED_Y,                        FLAG_NONE },
    { 1, "INC", MODE_IMPLIED,                                   FLAG_NONE },
    { 1, "TCS", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "TRB", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "ORA", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 3, "ASL", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 4, "ORA", MODE_ABSOLUTE_LONG_INDEXED,                     FLAG_NONE },
    { 3, "JSR", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 2, "AND", MODE_DIRECT_INDEXED_INDIRECT,                   FLAG_NONE },
    { 4, "JSR", MODE_ABSOLUTE_LONG,                             FLAG_NONE },
    { 2, "AND", MODE_STACK_RELATIVE,                            FLAG_NONE },
    { 2, "BIT", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "AND", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "ROL", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "AND", MODE_DIRECT_INDIRECT_LONG,                      FLAG_NONE },
    { 1, "PLP", MODE_IMPLIED,                                   FLAG_NONE },
    { 2, "AND", MODE_IMMEDIATE,                                 FLAG_M    },
    { 1, "ROL", MODE_IMPLIED,                                   FLAG_NONE },
    { 1, "PLD", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "BIT", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "AND", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "ROL", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 4, "AND", MODE_ABSOLUTE_LONG,                             FLAG_NONE },
    { 2, "BMI", MODE_PC_RELATIVE,                               FLAG_NONE },
    { 2, "AND", MODE_DIRECT_INDIRECT_INDEXED,                   FLAG_NONE },
    { 2, "AND", MODE_DIRECT_INDIRECT,                           FLAG_NONE },
    { 2, "AND", MODE_STACK_INDIRECT_INDEXED,                    FLAG_NONE },
    { 2, "BIT", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "AND", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "ROL", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "AND", MODE_DIRECT_INDIRECT_LONG_INDEXED,              FLAG_NONE },
    { 1, "SEC", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "AND", MODE_ABSOLUTE_INDEXED_Y,                        FLAG_NONE },
    { 1, "DEC", MODE_IMPLIED,                                   FLAG_NONE },
    { 1, "TSC", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "BIT", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 3, "AND", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 3, "ROL", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 4, "AND", MODE_ABSOLUTE_LONG_INDEXED,                     FLAG_NONE },
    { 1, "RTI", MODE_IMPLIED,                                   FLAG_NONE },
    { 2, "EOR", MODE_DIRECT_INDEXED_INDIRECT,                   FLAG_NONE },
    { 2, "WDM", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "EOR", MODE_STACK_RELATIVE,                            FLAG_NONE },
    { 3, "MVP", MODE_BLOCK_MOVE,                                FLAG_NONE },
    { 2, "EOR", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "LSR", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "EOR", MODE_DIRECT_INDIRECT_LONG,                      FLAG_NONE },
    { 1, "PHA", MODE_IMPLIED,                                   FLAG_NONE },
    { 2, "EOR", MODE_IMMEDIATE,                                 FLAG_M    },
    { 1, "LSR", MODE_IMPLIED,                                   FLAG_NONE },
    { 1, "PHK", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "JMP", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "EOR", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "LSR", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 4, "EOR", MODE_ABSOLUTE_LONG,                             FLAG_NONE },
    { 2, "BVC", MODE_PC_RELATIVE,                               FLAG_NONE },
    { 2, "EOR", MODE_DIRECT_INDIRECT_INDEXED,                   FLAG_NONE },
    { 2, "EOR", MODE_DIRECT_INDIRECT,                           FLAG_NONE },
    { 2, "EOR", MODE_STACK_RELATIVE,                            FLAG_NONE },
    { 3, "MVN", MODE_BLOCK_MOVE,                                FLAG_NONE },
    { 2, "EOR", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "LSR", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "EOR", MODE_DIRECT_INDIRECT_LONG_INDEXED,              FLAG_NONE },
    { 1, "CLI", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "EOR", MODE_ABSOLUTE_INDEXED_Y,                        FLAG_NONE },
    { 1, "PHY", MODE_IMPLIED,                                   FLAG_NONE },
    { 1, "TCD", MODE_IMPLIED,                                   FLAG_NONE },
    { 4, "JMP", MODE_ABSOLUTE_LONG,                             FLAG_NONE },
    { 3, "EOR", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 3, "LSR", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 4, "EOR", MODE_ABSOLUTE_LONG_INDEXED,                     FLAG_NONE },
    { 1, "RTS", MODE_IMPLIED,                                   FLAG_NONE },
    { 2, "ADC", MODE_DIRECT_INDEXED_INDIRECT,                   FLAG_NONE },
    { 3, "PER", MODE_PC_RELATIVE_LONG,                          FLAG_NONE },
    { 2, "ADC", MODE_STACK_RELATIVE,                            FLAG_NONE },
    { 2, "STZ", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "ADC", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "ROR", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "ADC", MODE_DIRECT_INDIRECT_LONG,                      FLAG_NONE },
    { 1, "PLA", MODE_IMPLIED,                                   FLAG_NONE },
    { 2, "ADC", MODE_IMMEDIATE,                                 FLAG_M    },
    { 1, "ROR", MODE_IMPLIED,                                   FLAG_NONE },
    { 1, "RTL", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "JMP", MODE_ABSOLUTE_INDIRECT,                         FLAG_NONE },
    { 3, "ADC", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "ROR", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 4, "ADC", MODE_ABSOLUTE_LONG,                             FLAG_NONE },
    { 2, "BVS", MODE_PC_RELATIVE,                               FLAG_NONE },
    { 2, "ADC", MODE_DIRECT_INDIRECT_INDEXED,                   FLAG_NONE },
    { 2, "ADC", MODE_DIRECT_INDIRECT,                           FLAG_NONE },
    { 2, "ADC", MODE_STACK_INDIRECT_INDEXED,                    FLAG_NONE },
    { 2, "STZ", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "ADC", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "ROR", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "ADC", MODE_DIRECT_INDIRECT_LONG_INDEXED,              FLAG_NONE },
    { 1, "SEI", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "ADC", MODE_ABSOLUTE_INDEXED_Y,                        FLAG_NONE },
    { 1, "PLY", MODE_IMPLIED,                                   FLAG_NONE },
    { 1, "TDC", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "JMP", MODE_ABSOLUTE_INDEXED_INDIRECT,                 FLAG_NONE },
    { 3, "ADC", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 3, "ROR", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 4, "ADC", MODE_ABSOLUTE_LONG_INDEXED,                     FLAG_NONE },
    { 2, "BRA", MODE_PC_RELATIVE,                               FLAG_NONE },
    { 2, "STA", MODE_DIRECT_INDEXED_INDIRECT,                   FLAG_NONE },
    { 3, "BRL", MODE_PC_RELATIVE_LONG,                          FLAG_NONE },
    { 2, "STA", MODE_STACK_RELATIVE,                            FLAG_NONE },
    { 2, "STY", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "STA", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "STX", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "STA", MODE_DIRECT_INDIRECT_LONG,                      FLAG_NONE },
    { 1, "DEY", MODE_IMPLIED,                                   FLAG_NONE },
    { 2, "BIT", MODE_IMMEDIATE,                                 FLAG_M    },
    { 1, "TXA", MODE_IMPLIED,                                   FLAG_NONE },
    { 1, "PHB", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "STY", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "STA", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "STX", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 4, "STA", MODE_ABSOLUTE_LONG,                             FLAG_NONE },
    { 2, "BCC", MODE_PC_RELATIVE,                               FLAG_NONE },
    { 2, "STA", MODE_DIRECT_INDIRECT_INDEXED,                   FLAG_NONE },
    { 2, "STA", MODE_DIRECT_INDIRECT,                           FLAG_NONE },
    { 2, "STA", MODE_STACK_INDIRECT_INDEXED,                    FLAG_NONE },
    { 2, "STY", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "STA", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "STX", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "STA", MODE_DIRECT_INDIRECT_LONG_INDEXED,              FLAG_NONE },
    { 1, "TYA", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "STA", MODE_ABSOLUTE_INDEXED_Y,                        FLAG_NONE },
    { 1, "TXS", MODE_IMPLIED,                                   FLAG_NONE },
    { 1, "TXY", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "STZ", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "STA", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 3, "STZ", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 4, "STA", MODE_ABSOLUTE_LONG_INDEXED,                     FLAG_NONE },
    { 2, "LDY", MODE_IMMEDIATE,                                 FLAG_X    },
    { 2, "LDA", MODE_DIRECT_INDEXED_INDIRECT,                   FLAG_NONE },
    { 2, "LDX", MODE_IMMEDIATE,                                 FLAG_X    },
    { 2, "LDA", MODE_STACK_RELATIVE,                            FLAG_NONE },
    { 2, "LDY", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "LDA", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "LDX", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "LDA", MODE_DIRECT_INDIRECT_LONG,                      FLAG_NONE },
    { 1, "TAY", MODE_IMPLIED,                                   FLAG_NONE },
    { 2, "LDA", MODE_IMMEDIATE,                                 FLAG_M    },
    { 1, "TAX", MODE_IMPLIED,                                   FLAG_NONE },
    { 1, "PLB", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "LDY", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "LDA", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "LDX", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 4, "LDA", MODE_ABSOLUTE_LONG,                             FLAG_NONE },
    { 2, "BCS", MODE_PC_RELATIVE,                               FLAG_NONE },
    { 2, "LDA", MODE_DIRECT_INDIRECT_INDEXED,                   FLAG_NONE },
    { 2, "LDA", MODE_DIRECT_INDIRECT,                           FLAG_NONE },
    { 2, "LDA", MODE_STACK_INDIRECT_INDEXED,                    FLAG_NONE },
    { 2, "LDY", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "LDA", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "LDX", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "LDA", MODE_DIRECT_INDIRECT_LONG_INDEXED,              FLAG_NONE },
    { 1, "CLV", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "LDA", MODE_ABSOLUTE_INDEXED_Y,                        FLAG_NONE },
    { 1, "TSX", MODE_IMPLIED,                                   FLAG_NONE },
    { 1, "TYX", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "LDY", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 3, "LDA", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 3, "LDX", MODE_ABSOLUTE_INDEXED_Y,                        FLAG_NONE },
    { 4, "LDA", MODE_ABSOLUTE_LONG_INDEXED,                     FLAG_NONE },
    { 2, "CPY", MODE_IMMEDIATE,                                 FLAG_X    },
    { 2, "CMP", MODE_DIRECT_INDEXED_INDIRECT,                   FLAG_NONE },
    { 2, "REP", MODE_IMMEDIATE,                                 FLAG_NONE },
    { 2, "CMP", MODE_STACK_RELATIVE,                            FLAG_NONE },
    { 2, "CPY", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "CMP", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "DEC", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "CMP", MODE_DIRECT_INDIRECT_LONG,                      FLAG_NONE },
    { 1, "INY", MODE_IMPLIED,                                   FLAG_NONE },
    { 2, "CMP", MODE_IMMEDIATE,                                 FLAG_M    },
    { 1, "DEX", MODE_IMPLIED,                                   FLAG_NONE },
    { 1, "WAI", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "CPY", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "CMP", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "DEC", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 4, "CMP", MODE_ABSOLUTE_LONG,                             FLAG_NONE },
    { 2, "BNE", MODE_PC_RELATIVE,                               FLAG_NONE },
    { 2, "CMP", MODE_DIRECT_INDIRECT_INDEXED,                   FLAG_NONE },
    { 2, "CMP", MODE_DIRECT_INDIRECT,                           FLAG_NONE },
    { 2, "CMP", MODE_STACK_INDIRECT_INDEXED,                    FLAG_NONE },
    { 2, "PEI", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "CMP", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "DEC", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "CMP", MODE_DIRECT_INDIRECT_LONG_INDEXED,              FLAG_NONE },
    { 1, "CLD", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "CMP", MODE_ABSOLUTE_INDEXED_Y,                        FLAG_NONE },
    { 1, "PHX", MODE_IMPLIED,                                   FLAG_NONE },
    { 1, "STP", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "JMP", MODE_ABSOLUTE_INDIRECT_LONG,                    FLAG_NONE },
    { 3, "CMP", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 3, "DEC", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 4, "CMP", MODE_ABSOLUTE_LONG_INDEXED,                     FLAG_NONE },
    { 2, "CPX", MODE_IMMEDIATE,                                 FLAG_X    },
    { 2, "SBC", MODE_DIRECT_INDEXED_INDIRECT,                   FLAG_NONE },
    { 2, "SEP", MODE_IMMEDIATE,                                 FLAG_NONE },
    { 2, "SBC", MODE_STACK_RELATIVE,                            FLAG_NONE },
    { 2, "CPX", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "SBC", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "INC", MODE_DIRECT,                                    FLAG_NONE },
    { 2, "SBC", MODE_DIRECT_INDIRECT_LONG,                      FLAG_NONE },
    { 1, "INX", MODE_IMPLIED,                                   FLAG_NONE },
    { 2, "SBC", MODE_IMMEDIATE,                                 FLAG_M    },
    { 1, "NOP", MODE_IMPLIED,                                   FLAG_NONE },
    { 1, "XBA", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "CPX", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "SBC", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 3, "INC", MODE_ABSOLUTE,                                  FLAG_NONE },
    { 4, "SBC", MODE_ABSOLUTE_LONG,                             FLAG_NONE },
    { 2, "BEQ", MODE_PC_RELATIVE,                               FLAG_NONE },
    { 2, "SBC", MODE_DIRECT_INDIRECT_INDEXED,                   FLAG_NONE },
    { 2, "SBC", MODE_DIRECT_INDIRECT,                           FLAG_NONE },
    { 2, "SBC", MODE_STACK_INDIRECT_INDEXED,                    FLAG_NONE },
    { 3, "PEA", MODE_IMPLIED,                                   FLAG_NONE },
    { 2, "SBC", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "INC", MODE_DIRECT_INDEXED,                            FLAG_NONE },
    { 2, "SBC", MODE_DIRECT_INDIRECT_LONG_INDEXED,              FLAG_NONE },
    { 1, "SED", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "SBC", MODE_ABSOLUTE_INDEXED_Y,                        FLAG_NONE },
    { 1, "PLX", MODE_IMPLIED,                                   FLAG_NONE },
    { 1, "XCE", MODE_IMPLIED,                                   FLAG_NONE },
    { 3, "JSR", MODE_ABSOLUTE_INDEXED_INDIRECT,                 FLAG_NONE },
    { 3, "SBC", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 3, "INC", MODE_ABSOLUTE_INDEXED_X,                        FLAG_NONE },
    { 4, "SBC", MODE_ABSOLUTE_LONG_INDEXED,                     FLAG_NONE },
};



#define OPCODE_COLUMN       24

/*
 * Prints a single opcode.
 */
void print_opcode(u8 *bank, u16 *offset, u8 *abits, u8 *xybits)
{
    int i;
    int strwidth;       /* Number of characters displayed so far. */
    int oparg = 0;      /* Numeric argument of the instruction. */
    int size;           /* Temporary size of instruction in bytes. */
    u8 opcode;

    if (!bank || !offset || !abits || !xybits)
        return;

    opcode = romdata[linear_address(*bank, *offset + 0)];

    /* Print the current address and the initial opcode. */
    strwidth = printf("%02x:%04x %02x ", *bank, *offset, opcode);

    /* Print the instruction bytes. */
    size = opcodes[opcode].size;
    if (opcodes[opcode].use_mx == FLAG_M && *abits == 16)
        size++;

    for (i = 1; i < opcodes[opcode].size; i++) {
        u8 rombyte = romdata[linear_address(*bank, *offset + i)];
        strwidth += printf("%02x ", rombyte);
        oparg += rombyte << (8*(i-1));
    }
    for (i = strwidth; i < OPCODE_COLUMN; i++)
        putchar(' ');

    /* Advance PC. */
    *offset += size;

    /* Print the mnemonic. */

    printf("%3s ", opcodes[opcode].mnemonic);

    /* Print the argument. */
    switch (opcodes[opcode].mode) {
    case MODE_BLOCK_MOVE:                    /* $12,$34      */
        printf("$%02x,$%02x", oparg >> 8, oparg & 0xff);
        break;
    case MODE_INTERRUPT:                     /* $12          */
        printf("$%02x", oparg);
        break;
    case MODE_IMPLIED:                       /* (no display) */
        break;
    case MODE_IMMEDIATE:                     /* #$1234       */
        if (opcodes[opcode].use_mx == FLAG_M && *abits == 8)
            printf("#$%02x", oparg);
        else if (opcodes[opcode].use_mx == FLAG_X && *xybits == 8)
            printf("#$%02x", oparg);
        else if (opcodes[opcode].use_mx == FLAG_M && *abits == 16)
            printf("#$%04x", oparg);
        else if (opcodes[opcode].use_mx == FLAG_X && *xybits == 16)
            printf("#$%04x", oparg);
        else
            /* SEP or REP. */
            printf("#$%02x", oparg);
        break;
    case MODE_PC_RELATIVE:                   /* $9abc        */
        printf("$%04x", oparg + *offset);
        break;
    case MODE_PC_RELATIVE_LONG:              /* $c0abcd      */
        printf("$%02x%04x", *bank, oparg + *offset);
        break;
    case MODE_STACK_RELATIVE:                /* $12,S        */
        printf("$%02x,S", oparg);
        break;
    case MODE_STACK_INDIRECT_INDEXED:        /* ($12,S),Y    */
        printf("($%02x,S),Y", oparg);
        break;
    case MODE_DIRECT:                        /* $12          */
        printf("$%02x", oparg);
        break;
    case MODE_DIRECT_INDEXED:                /* $12,X        */
        printf("($%02x,X)", oparg);
        break;
    case MODE_DIRECT_INDEXED_INDIRECT:       /* ($12,X)      */
        printf("($%02x,X)", oparg);
        break;
    case MODE_DIRECT_INDIRECT_INDEXED:       /* ($12),Y      */
        printf("($%02x),Y", oparg);
        break;
    case MODE_DIRECT_INDIRECT:               /* ($12)        */
        printf("($%02x)", oparg);
        break;
    case MODE_DIRECT_INDIRECT_LONG:          /* [$12]        */
        printf("[$%02x]", oparg);
        break;
    case MODE_DIRECT_INDIRECT_LONG_INDEXED:  /* [$12],Y      */
        printf("[$%02x],Y", oparg);
        break;
    case MODE_ABSOLUTE:                      /* $1234        */
        printf("$%04x", oparg);
        break;
    case MODE_ABSOLUTE_INDIRECT:             /* ($1234)      */
        printf("($%04x)", oparg);
        break;
    case MODE_ABSOLUTE_INDEXED_X:            /* $1234,X      */
        printf("$%04x,X", oparg);
        break;
    case MODE_ABSOLUTE_INDEXED_Y:            /* $1234,Y      */
        printf("$%04x,Y", oparg);
        break;
    case MODE_ABSOLUTE_LONG:                 /* $123456      */
        printf("$%06x", oparg);
        break;
    case MODE_ABSOLUTE_LONG_INDEXED:         /* $123456,X    */
        printf("$%06x,X", oparg);
        break;
    case MODE_ABSOLUTE_INDEXED_INDIRECT:     /* ($1234,X)    */
        printf("($%04x,X)", oparg);
        break;
    case MODE_ABSOLUTE_INDIRECT_LONG:        /* [$1234]      */
        printf("[$%04x]", oparg);
        break;
    default:
        break;
    }


    /* Adjust abits and xybits if the instruction is REP or SEP. */
    if (opcode == 0xe2) {
        if (oparg & 0x20) *abits = 8;
        if (oparg & 0x10) *xybits = 8;
    }
    else if (opcode == 0xc2) {
        if (oparg & 0x20) *abits = 16;
        if (oparg & 0x10) *xybits = 16;
    }

    putchar('\n');
}

