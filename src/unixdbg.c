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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "unixdbg.h"

extern u8 mem_watches;

u8 mem_watches = 0;

const u8 MODE_READ     = 0x01;
const u8 MODE_WRITE    = 0x02;

const static int SIZEOF_MEM_BREAKPOINT_T = sizeof(struct mem_breakpoint_t);
const static int MEM_BP_DB_OFFSET =
    (int)&((struct mem_breakpoint_t *)NULL)->db;
const static int MEM_BP_ADDR_OFFSET =
    (int)&((struct mem_breakpoint_t *)NULL)->offset;

/* Don't set this too high because the game will become unplayable when there
 * are this many breakpoints loaded. */
struct mem_breakpoint_t *mem_breakpoints[37] = { NULL }; /* NULLs. */

const int NB_MEM_BREAKPOINTS = sizeof(mem_breakpoints) /
                               sizeof(mem_breakpoints[0]);



/*
 * Returns the minimal congruent address based on a bank and an offset.
 */
u32 min_addr_bankoffset(u8 bank, u16 offset)
{
    /* LoROM */
    if (romtype == 1) {
        if (bank >= 0x80 && bank < 0xf0)
            bank -= 0x80;

        if (offset < 0x8000 && bank < 0x40)
            bank = 0;
        if (offset < 0x2000 && (bank < 0x40 || bank == 0x7e))
            bank = 0;
    }

    /* HiROM */
    else if (romtype == 2) {
        if (bank >= 0x80 && bank < 0xc0)
            bank -= 0x80;
        else if (bank >= 0xc0 && bank < 0xf0) {
            if (offset < 0x8000)
                /* eg: 0xc03333 => 0x403333 */
                bank -= 0x80;
            else
                /* eg: 0xc09999 => 0x009999 */
                bank -= 0xc0;
        }

        if ((bank < 0x30 && offset < 0x8000) ||
            (bank < 0x40 && offset < 0x6000) ||
            (bank == 0x7e && offset < 0x2000))
            bank = 0;
    }

    return (bank << 16) + offset;
}

/*
 * Returns the minimal congruent address of the specified address.
 */
u32 min_addr(u32 address)
{
    u8 bank;
    u16 offset;

    /* Extract 0x123456 into bank=0x12 and offset=0x3456. */
    bank = (u8)(address >> 16);
    offset = address & 0xffff;

    return min_addr_bankoffset(bank, offset);
}

/*
 * Returns non-zero if the two specified addresses are congruent (ie: refer to
 * the same physical memory for the 65816 CPU).
 */
static int is_congruent(u32 addr1, u32 addr2)
{
    return min_addr(addr1) == min_addr(addr2);
}

/*
 * Parses a line of text to return a memory breakpoint, or NULL if the format is
 * invalid.
 */
struct mem_breakpoint_t * parse_breakpoint(char *text)
{
    struct mem_breakpoint_t *bp;
    char mode[2] = {0,0}, *p;
    int ret;

    bp = (struct mem_breakpoint_t *) malloc(SIZEOF_MEM_BREAKPOINT_T);
    if (!bp)
        return NULL;

    ret = sscanf(text, "%02x:%04x:%2c",
                 (int *)&bp->db, (int *)&bp->offset, mode);

    if (ret != 3) {
        free(bp);
        return NULL;
    }

    p = mode;
    bp->mode = 0;
    while (*p) {
        switch (*p) {
            case 'r': case 'R': bp->mode |= MODE_READ;  break;
            case 'w': case 'W': bp->mode |= MODE_WRITE; break;
        }
        p++;
    }

    return bp;
}

/*
 * Returns the index into the hash table of memory breakpoints based on the
 * specified breakpoint. The index is based on the data bank register and the
 * offset within that bank.
 */
int hash_index(struct mem_breakpoint_t *bp)
{
    return bp->offset % NB_MEM_BREAKPOINTS;
    /* return min_addr_bankoffset(bp->db, bp->offset) % NB_MEM_BREAKPOINTS; */
}

/*
 * Returns non-zero if two specified breakpoints are equal or zero otherwise.
 * Two memory breakpoints are equal if their addresses are congruent to each
 * other (point to the same address).
 */
int breakpoint_cmp(struct mem_breakpoint_t *bp1, struct mem_breakpoint_t *bp2)
{
    return bp1 && bp2 && is_congruent(bp1->db * 0x10000 + bp1->offset,
                                      bp2->db * 0x10000 + bp2->offset);
}

/*
 * Returns 0 if the breakpoint table is full, or non-zero if the breakpoint
 * has successfully been put into the table.
 */
int bp_put(struct mem_breakpoint_t *bp)
{
    int index = hash_index(bp);
    int i = index;

    while (mem_breakpoints[i]) {
        if (breakpoint_cmp(mem_breakpoints[i], bp))
            break;
        i = (i + 1) % NB_MEM_BREAKPOINTS;
        if (i == index)
            return 0;
    }

    if (mem_breakpoints[i] == NULL) {
        mem_breakpoints[i] = bp;
    }
    else if (breakpoint_cmp(mem_breakpoints[i], bp)) {
        /* Already present. Copy new breakpoint information (any field that are
           not used in the index calculation (data bank and address). */
        mem_breakpoints[i]->mode = bp->mode;
    }

    return 1;
}

/*
 * Returns non-zero if the specified breakpoint exists in the table.
 */
static int bp_exists2(struct mem_breakpoint_t *bp)
{
    int index = hash_index(bp);
    int i = index;

    if (!mem_breakpoints[i]) {
        return 0;
    }
    else {
        while (!breakpoint_cmp(mem_breakpoints[i], bp)) {
            i = (i + 1) % NB_MEM_BREAKPOINTS;
            if (i == index)
                return 0;
        }
        return mem_breakpoints[i]->mode & bp->mode;
    }
}

void handle_directr8(u8 lobyte, u32 dpr)
{
    /* if (xpb != 0 && xpc != 0xb073) */
    if (lobyte == 0x9d) {
        printf("lobyte=%02x dpr=%04x pc=%02x:%04x xdb=%02x\n",
                lobyte, dpr, xpb, xpc, xdb);
        printf(" romtype=%d min(0x00%04x)=%06x min(7e039d)=%06x\n",
                romtype,
                dpr+lobyte,
                min_addr_bankoffset(0, dpr+lobyte),
                min_addr_bankoffset(0x7e, 0x039d));
    }
}

struct mem_breakpoint_t bp_triggered;
int bp_exists(u8 bank, u16 offset, u8 mode)
{
    int r;

    bp_triggered.db = bank;
    bp_triggered.offset = offset;
    bp_triggered.mode = mode;

    r = bp_exists2(&bp_triggered);
#if 0
    if (offset == 0x39d)
    printf("exists for bank=%02x offset=%04x\n", bank, offset);
#endif

    return r;
}

/*
 * Empty the hash table.
 */
void free_all_breakpoints()
{
    int i;

    for (i = 0; i < NB_MEM_BREAKPOINTS; i++)
        if (mem_breakpoints[i]) {
            free(mem_breakpoints[i]);
            mem_breakpoints[i] = NULL;
        }
}

/*
 * Reads the breakpoint file and initialize the hash table with any valid
 * breakpoint found. This function may be called more than once (ie: to read
 * the file again). In that case, all previous breakpoints are lost.
 */
void init_unix_debugger()
{
    FILE *specfile;

    printf("Loading memory breakpoints...\n");
    specfile = fopen("/tmp/zsnes-bp.ini", "r");

    free_all_breakpoints();

    if (specfile) {
        char line[1024];
        int linenum = 0;
        while (fgets(line, sizeof(line), specfile) != NULL) {
            char *p, *q;
            struct mem_breakpoint_t *bp;

            linenum++;
            p = line;
            while (isspace(*p)) p++;
            if (*p == ';' || *p == '\n' || !*p)
                continue;
            q = strrchr(line, '\n');
            if (q)
                *q = '\0';
            bp = parse_breakpoint(p);
            if (!bp) {
                fprintf(stderr, "invalid breakpoint line at line %d: %s\n",
                        linenum, line);
            }
            else if (!bp_put(bp)) {
                fprintf(stderr, "too many breakpoints, ignoring the rest\n");
                break;
            }
            else {
                printf("Loaded breakpoint %02x:%04x\n", bp->db, bp->offset);
            }
        }
        fclose(specfile);
    }
    else {
        printf("/tmp/zsnes-bp.ini does not exist; none loaded.\n");
    }
}

/*
 * Lists the breakpoints on screen.
 */
void print_mem_breakpoints()
{
    int i, k;

    for (i = 0, k = 0; i < NB_MEM_BREAKPOINTS; i++) {
        struct mem_breakpoint_t *bp = mem_breakpoints[i];
        if (bp != NULL) {
            char *mode_read  = bp->mode & MODE_READ  ? "READ"  : "";
            char *mode_write = bp->mode & MODE_WRITE ? "WRITE" : "";
            printf("#%-3d (%04d) 0x%02x%04x %-5s%-5s\n",
                    k, i, bp->db, bp->offset, mode_read, mode_write);
            k++;
        }
    }

}

/*
 * Turn the breakpoint system on or off.
 */
void toggle_watches()
{
    mem_watches = !mem_watches;
    printf("Memory watches turned %s\n", mem_watches ? "on" : "off");

    if (mem_watches) {
        /* If they have just been turned on, print the current list of
         * breakpoints. */
        printf("Current watches:\n");
        print_mem_breakpoints();
    }
}

/*
 * Called from src/gui/guikeys.inc to interact with the breakpoint system. The
 * following commands are recognized:
 *  - Toggle the breakpoint system on and off with ";"
 *  - Reinitialize breakpoints from the breakpoint file with "+"
 */
void handle_debugger_keys(int key)
{
    save_regs();

    /* printf("handling key: %d\n", key); */
    switch (key) {
    case ';':
        toggle_watches();
        break;
    case '+':
        init_unix_debugger();
        break;
    default:
        break;
    }

    restore_regs();
}


/*
 * Supposed to hook the execution of every instruction.
 * Putting a call to this function in src/cpu/execute.asm just below the
 * label .start_over will not do. A better place cannot be found.
 * So this function is currently not used.
 */
void hook_exec(u8 *curr_addr)
{
    u8 opcode = *curr_addr;
    extern u8 *romdata;

    printf("opcode=%02x @0x%08x\n", opcode, curr_addr - romdata);
}



/* Functions about the stack backtrace. */

struct btnode_t {
    u8 bank;
    u16 offset;
    struct btnode_t *next;
};

static struct btnode_t *btlist_head = NULL;
static int nb_btnodes = 0;

int bt_empty()
{
    return nb_btnodes == 0;
}

void print_bt_list()
{
    struct btnode_t *node = btlist_head;

    while (node != NULL) {
        printf("[%02x:%04x]->", node->bank, node->offset);
        node = node->next;
    }
    puts("/");
}

void bt_push(u8 bank, u16 offset)
{
    struct btnode_t *node;
    return;

    node = (struct btnode_t *) malloc(sizeof(struct btnode_t));
    node->bank = bank;
    node->offset = offset;
    node->next = btlist_head;
    btlist_head = node;

    nb_btnodes++;
}

void bt_pop(/*u8 *bank, u16 *offset*/)
{
    return;
    if (!bt_empty()) {
        struct btnode_t *tmp;
        /* *bank = btlist_head->bank; */
        /* *offset = btlist_head->offset; */
        tmp = btlist_head;
        btlist_head = tmp->next;
        free(tmp);
        nb_btnodes--;
    }
}

/*
 * Print the last n backtrace elements from the ROM call stack.
 */
void print_stack_backtrace(int n)
{
    int i;
    struct btnode_t *node = btlist_head;

    n = n < nb_btnodes ? n : nb_btnodes;
    printf("WARNING: BACKTRACES NOT CURRENTLY IMPLEMENTED\n");
    printf("Showing %d/%d elements in call stack:\n", n, nb_btnodes);
    for (i = 0; i < n && node != NULL; i++) {
        printf("#%-2d %02x:%04x\n", i, node->bank, node->offset + 1);
        node = node->next;
    }
}


/*
 * The main function that handle the debug menu. Called from src/cpu/memory.asm.
 */
void debug_menu()
{
    save_regs();

    const char *mode = bp_triggered.mode & MODE_READ  ? "READ"  :
                       bp_triggered.mode & MODE_WRITE ? "WRITE" : "<unknown>";

    putchar('\n');
    printf("BROKE because of a %s at %02x:%04x\n",
            mode, bp_triggered.db, bp_triggered.offset);
    cmd_print_regs(NULL);
    printf("*Note: The breakpoint is triggered a bit further"
           " than the PC shown above.\n");
    printf("Press 'h' for help.\n\n");

    /* mem_watches can be toggled with the key ';'. */
    while (mem_watches) {
        int i;
        char cmd[80];

        printf("> "); fflush(stdout);
        fgets(cmd, sizeof(cmd), stdin);

        if (*cmd == 'r')
            break; /* Resume game until next break. */

        /* Lookup the command and run it, if found. */
        for (i = 0; i < NB_COMMANDS; i++) {
            if (commands[i].cmd == cmd[0]) {
                commands[i].function(cmd);
                break;
            }
        }
    }

    restore_regs();
}


/*
 * Just prints its argument. To be used from assembly to print values.
 */
void print_arg(u32 n)
{
    save_regs();
    printf("0x%08x\n", n);
    restore_regs();
}

