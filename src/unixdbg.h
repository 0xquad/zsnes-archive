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



#ifndef UNIXDBG_H
#define UNIXDBG_H

#define INLINE    __inline__ __attribute__((__always_inline__))

INLINE static void save_regs() {
#if __WORDSIZE == 32
    __asm__("pusha");
#elif __WORDSIZE == 64
    __asm__("push %rax");
    __asm__("push %rbx");
    __asm__("push %rcx");
    __asm__("push %rdx");
    __asm__("push %rsi");
    __asm__("push %rdi");
    __asm__("push %rbp");
#endif
}

INLINE static void restore_regs() {
#if __WORDSIZE == 32
    __asm__("popa");
#elif __WORDSIZE == 64
    __asm__("pop %rbp");
    __asm__("pop %rdi");
    __asm__("pop %rsi");
    __asm__("pop %rdx");
    __asm__("pop %rcx");
    __asm__("pop %rbx");
    __asm__("pop %rax");
#endif
}

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

extern u8 romtype;
extern u8 wramdataa[];
extern u32 xa, xx, xy, xpb, xdb, xs, xd;
extern u16 xpc;
/* extern u16 xat, xst, xdt, xxt, xyt, xpc; */
/* extern u8 xdbt, xpbt; */

struct mem_breakpoint_t {
    u8 db;
    u16 offset;
    u8 mode;
};

extern struct mem_breakpoint_t *mem_breakpoints[];
extern const int NB_MEM_BREAKPOINTS;

extern struct mem_breakpoint_t * parse_breakpoint(char *text);
extern int bp_put(struct mem_breakpoint_t *bp);
extern int hash_index(struct mem_breakpoint_t *bp);
extern int breakpoint_cmp(struct mem_breakpoint_t *bp1, struct mem_breakpoint_t *bp2);
extern void print_mem_breakpoints();
extern void print_stack_backtrace(int n);



/* Debugger commands. */

extern void cmd_noop(char *cmdline);
extern void cmd_print_regs(char *cmdline);
extern void cmd_toggle_watches(char *cmdline);
extern void cmd_disassemble(char *cmdline);
extern void cmd_dump_mem(char *cmdline);
extern void cmd_set_mem(char *cmdline);
extern void cmd_list_bp(char *cmdline);
extern void cmd_help(char *cmdline);
extern void cmd_add_bp(char *cmdline);
extern void cmd_del_bp(char *cmdline);
extern void cmd_backtrace(char *cmdline);

extern struct cmd_t {
    char cmd;
    void (*function)(char *cmdline);
    char *help;
} commands[];

/* Total number of commands as defined by the previous table. */
extern const int NB_COMMANDS;


#endif  /* UNIXDBG_H */
