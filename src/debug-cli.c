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
#include <stdlib.h>
#include <string.h>
#include "unixdbg.h"

extern u8 wramdataa[], ram7fa[];
extern u8 wramdataa[], ram7fa[];


/*
 * Functions available when the debug menu is launched.
 *
 * Also, the 'r' command is special and is handled internally to the main loop
 * below, but is defined as a (null) command so that it appears in the help
 * menu with a message.
 */

/* Commands. */
struct cmd_t commands[] = {
    { 'r',  cmd_noop,           "Resume game. ex: r" }, /* See below. */
    { ';',  cmd_toggle_watches, "Disable breakpoints and resume. ex: ;" },
    { 'p',  cmd_print_regs,     "Print 65816 registers. ex: p" },
    { 'u',  cmd_disassemble,    "Disassemble instructions."
                                " ex: u, u 5, u f012 100, u c02b2c 20" },
    { 'd',  cmd_dump_mem,       "Dump 65816 WRAM memory. ex: d e182-e183" },
    { 'w',  cmd_set_mem,        "Write to memory. ex: w 7ee182 244" },
    { 'x',  cmd_del_bp,         "Remove a breakpoint. ex: x 7e e182, x *" },
    { 'a',  cmd_add_bp,         "Add a breakpoint. ex: a 7e e182 rw" },
    { 'l',  cmd_list_bp,        "List breakpoints. ex: l" },
    { 't',  cmd_backtrace,      "Show stack backtrace. ex: t, t 10" },
    { 'h',  cmd_help,           "Help menu. ex: h" },
};

/* Total number of commands as defined by the previous table. */
const int NB_COMMANDS = sizeof(commands) / sizeof(commands[0]);


/******************************************************************************
 * The debugger commands.
 ****************************************************************************/


/*
 * Print the 65816 registers.
 */
void cmd_print_regs(char *cmdline)
{
    printf("PC=%02x:%04x A=%04x X=%04x Y=%04x DB=%02x S=%04x D=%04x\n",
            /* xpbt, xpc, xat, xxt, xyt, xdbt, xst, xdt); */
            (u8)xpb, (u16)xpc, (u16)xa, (u16)xx, (u16)xy, (u8)xdb, (u16)xs, (u16)xd);
}

/*
 * Toggle the breakpoint system on or off.
 */
void cmd_toggle_watches(char *cmdline)
{
    extern void toggle_watches();

    toggle_watches();
}


extern void print_opcode(u8 *bank, u16 *offset, u8 *abits, u8 *xybits);

/*
 * Returns the flat linear address of the specified bank and offset.
 * The linear address depends on the ROM type (LoROM or HiROM).
 */
u32 linear_address(u8 bank, u16 offset)
{
    u32 addr = 0;

    if (romtype == 1 && offset >= 0x8000) {
        /* LoROM linear address. Rebase the bank on bank 0x00. */
        if (bank >= 0xf0)
            bank -= 0x80;
        if (bank < 0x80)
            addr = bank * 0x8000 + offset - 0x8000;
    }
    else if (romtype == 2) {
        /* HiROM linear address. Rebase the bank on bank 0x00. */
        if (bank < 0x40 && offset < 0x8000)
            ; /* Exceptionnal case (invalid ROM address). Do nothing. */
        else {
            if (bank >= 0xc0)
                bank -= 0xc0;
            if (bank >= 0x40 && bank < 0x70 && offset < 0x8000)
                bank -= 0x40;
            if (bank < 0x40)
                addr = (bank << 16) + offset;
        }
    }

    return addr;
}

/*
 * Disassemble a part of the ROM code.
 */
void cmd_disassemble(char *cmdline)
{
    /* static u8 *last_addr; */
    char buf[4];
    int nb_inst = 20;
    int ret;

    ret = sscanf(cmdline, "%1s %d", buf, &nb_inst);
    if (ret == 1 || ret == 2) {
        int i;
        u8 pb = (u8)xpb;
        u16 pc = (u16)xpc;
        u8 abits = 8, xybits = 8;
        for (i = 0; i < nb_inst; i++) {
            print_opcode(&pb, &pc, &abits, &xybits);
        }
    }
    else
        printf("invalid argument\n");
}

/*
 * Used by dump_mem().
 */
static void hex_dump(u8 *mem, int from, int to, int base)
{
    int i;
    int len = to - from + 1;

    for (i = 0; i < len; i++) {
        int pos = i % 16;
        if (pos == 0) {
            printf("%06x: ", base + from + i);
        }

        if (pos == 8)
            putchar(' ');
        printf("%02x ", mem[from + i]);

        if (pos == 15)
            putchar('\n');
    }
    putchar('\n');
}

/*
 * Dumps a portion of memory to screen.
 */
#define MIN(a,b) ((a)<(b)?(a):(b))
void cmd_dump_mem(char *cmdline)
{
    char buf[4];
    int from, to;
    int ret;

    ret = sscanf(cmdline, "%1s %x-%x", buf, &from, &to);
    if (ret == 2) {
        /* If the upper bound is not specified, set a default. */
        to = MIN(from, 0x7fffff);
    }

    /* Dump at least a complete 'row' of 16 bytes. */
    from &= ~0x0f;
    to |= 0x0f;

    if (ret == 2 || ret == 3) {
        if (from >= 0x7e0000)
            from -= 0x7e0000;
        if (to >= 0x7e0000)
            to -= 0x7e0000;

        if (from > to || to - from >= 0x10000 || to >= 0x20000)
            printf("invalid range\n");
        else {
            if (from < 0x10000 && to >= 0x10000)
                to = 0xffff;
            if (from >= 0 && from < 0x10000)
                hex_dump(wramdataa, from, to, 0x7e0000);
            else if (from >= 0x10000 && from < 0x20000) {
                from -= 0x10000;
                to -= 0x10000;
                hex_dump(ram7fa, from, to, 0x7f0000);
            }
            else
                printf("invalid range\n");
        }
    }
    else
        printf("invalid syntax: should be d xxxx[-yyyy]\n");
}

/*
 * Set a byte to the specified value in memory.
 */
void cmd_set_mem(char *cmdline)
{
    char buf[4];
    int addr;
    int val;
    int ret;

    ret = sscanf(cmdline, "%1s %x %d", buf, &addr, &val);

    if (ret == 3) {
        if (addr >= 0x7e0000 && addr < 0x800000) {
            addr -= 0x7e0000;
        }
        if (addr >= 0 && addr < 0x10000) {
            wramdataa[addr] = (u8)val;
            printf("[0x%06x]=0x%02x\n", 0x7e0000 + addr, val);
        }
        else if (addr >= 0x10000 && addr < 0x20000) {
            ram7fa[addr - 0x10000] = (u8)val;
            printf("[0x%06x]=0x%02x\n", 0x7e0000 + addr, val);
        }
        else
            printf("invalid address range\n");
    }
    else
        printf("invalid argument\n");
}

/*
 * Prints the available commands in the debugger.
 */
void cmd_help(char *cmdline)
{
    int i;

    printf("Available commands:\n");
    printf("-------------------\n");
    for (i = 0; i < NB_COMMANDS; i++) {
        printf("%c  %s\n", commands[i].cmd, commands[i].help);
    }
}

/*
 * Add a new memory breakpoint to the list or replace it if it already exists
 * (given a databank and an address).
 */
void cmd_add_bp(char *cmdline)
{
    if (strlen(cmdline) > 2) {
        struct mem_breakpoint_t *bp;
        char *p;
        cmdline += 2;
        for (p = cmdline; *p; p++)
            if (*p == ' ')
                *p = ':';
        bp = parse_breakpoint(cmdline);
        if (bp && !bp_put(bp))
            printf("error\n");
    }
}

/*
 * Remove a breakpoint from the list. All breakpoints can be removed at once if
 * a star '*" is specified.
 */
void cmd_del_bp(char *cmdline)
{
    if (strlen(cmdline) > 2 && cmdline[2] == '*') {
        int i;
        /* A start was specified. Scan the table and remove all breakpoints by
         * not forgeting to set them back to NULL. */
        for (i = 0; i < NB_MEM_BREAKPOINTS; i++)
            if (mem_breakpoints[i] != NULL) {
                free(mem_breakpoints[i]);
                mem_breakpoints[i] = NULL;
            }
    }
    else {
        int ret;
        int index;
        struct mem_breakpoint_t bp;
        char buf[4];
        int i;

        ret = sscanf(cmdline, "%1s %x %x",
                     buf, (int *)&bp.db, (int *)&bp.offset);

        if (ret != 3) {
            printf("invalid argument\n");
            return;
        }

        index = hash_index(&bp);
        i = index;
        while (!breakpoint_cmp(mem_breakpoints[i], &bp)) {
            i = (i + 1) % NB_MEM_BREAKPOINTS;
            if (i == index) {
                /* We scanned the whole table and came back to our original
                 * position. */
                printf("breakpoint does not exist\n");
                return;
            }
        }

        /* Land here only if we found the breakpoint we looked for. */
        free(mem_breakpoints[i]);
        mem_breakpoints[i] = NULL;

    }
}

void cmd_noop(char *cmdline)
{
}

void cmd_list_bp(char *cmdline)
{
    print_mem_breakpoints();
}

void cmd_backtrace(char *cmdline)
{
    int ret;
    char buf[4];
    int n;

    ret = sscanf(cmdline, "%1s %d", buf, &n);
    if (ret < 2)
        n = 10;

    print_stack_backtrace(n);
}

