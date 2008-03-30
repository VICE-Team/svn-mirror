/*
 * mon.h - The VICE built-in monitor.
 *
 * Written by
 *  Daniel Sladic (sladic@eecg.toronto.edu)
 *  Ettore Perazzoli (ettore@comm2000.it)
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#ifndef _MON_H
#define _MON_H

#include <stdio.h>
#include <assert.h>

#include "mos6510.h"
#include "mem.h"
#include "interrupt.h"

typedef int bool;
#define TRUE 1
#define FALSE 0

#define HI16(x) ((x)&0xffff0000)
#define LO16(x) ((x)&0xffff)
#define LO16_TO_HI16(x) (((x)&0xffff)<<16)
#define HI16_TO_LO16(x) (((x)>>16)&0xffff)

#define HI32(x) ((x)&0xffffffff00000000)
#define LO32(x) ((x)&0xffffffff)
#define LO32_TO_HI32(x) (((x)&0xffffffff)<<16)
#define HI32_TO_LO32(x) (((x)>>16)&0xffffffff)

enum mon_int {
    MI_NONE = 0,
    MI_BREAK = 1 << 0,
    MI_WATCH = 1 << 1,
    MI_STEP = 1 << 2,
};

enum t_reg_id {
   e_A,
   e_X,
   e_Y,
   e_PC,
   e_SP
};
typedef enum t_reg_id REG_ID;

extern char *register_string[];
extern char *datatype_string[];

enum t_memory_op {
   e_load,
   e_store,
   e_load_store
};
typedef enum t_memory_op MEMORY_OP;

enum t_memspace {
   e_default_space = 0,
   e_comp_space,
   e_disk_space,
   e_invalid_space
};
typedef enum t_memspace MEMSPACE;

typedef unsigned int M_ADDR;

#ifdef LONG_LONG
typedef long long M_ADDR_RANGE;
#else

struct t_address_range {
   M_ADDR start_addr;
   M_ADDR end_addr;
};
typedef struct t_address_range *M_ADDR_RANGE;

#endif

enum t_conditional {
   e_INV,
   e_EQU,
   e_NEQ,
   e_GT,
   e_LT,
   e_GTE,
   e_LTE,
   e_AND,
   e_OR
};
typedef enum t_conditional CONDITIONAL;
#define new_cond ((CONDITIONAL_NODE *)(malloc(sizeof(CONDITIONAL_NODE))))

enum t_datatype {
   e_default_datatype,
   e_hexadecimal,
   e_decimal,
   e_binary,
   e_octal,
   e_character,
   e_sprite,
   e_text_ascii,
   e_text_petscii,
   e_6502_asm
};
typedef enum t_datatype DATATYPE;

enum t_action {
   e_ON,
   e_OFF,
   e_TOGGLE
};
typedef enum t_action ACTION;

struct t_cond_node {
   int operation;
   int value;
   int reg_num;
   bool is_reg;
   struct t_cond_node *child1;
   struct t_cond_node *child2;
};
typedef struct t_cond_node CONDITIONAL_NODE;

struct t_breakpoint {
   int brknum;
   M_ADDR_RANGE range;
   bool trace;
   bool enabled;
   bool watch_load;
   bool watch_store;
   int hit_count;
   int ignore_count;
   CONDITIONAL_NODE *condition;
   char *command;
};
typedef struct t_breakpoint breakpoint;

struct t_break_list {
   breakpoint *brkpt;
   struct t_break_list *next;
};
typedef struct t_break_list BREAK_LIST;

extern char *memspace_string[];
extern char *cond_op_string[];

extern char *myinput;

/* ------------------------------------------------------------------------- */

typedef void monitor_toggle_func_t(int value);

/* This is the standard interface through which the monitor accesses a
   certain CPU.  */
struct monitor_interface {

    /* Pointer to the registers of the CPU.  */
    mos6510_regs_t *cpu_regs;

    /* Pointer to the alarm/interrupt status.  */
    cpu_int_status_t *int_status;

    /* Pointer to the machine's clock counter.  */
    CLOCK *clk;

    /* Pointer to a function that writes to memory.  */
    read_func_t *read_func;

    /* Pointer to a function that reads from memory.  */
    store_func_t *store_func;

    /* Pointer to a function to disable/enable watchpoint checking.  */
    monitor_toggle_func_t *toggle_watchpoints_func;

};
typedef struct monitor_interface monitor_interface_t;

#define HASH_ARRAY_SIZE 256

struct symbol_entry {
   ADDRESS addr;
   char *name;
   struct symbol_entry *next;
};
typedef struct symbol_entry symbol_entry_t;

struct symbol_table {
   symbol_entry_t *name_list;
   symbol_entry_t *addr_hash_table[HASH_ARRAY_SIZE];
};
typedef struct symbol_table symbol_table_t;

#define HASH_ADDR(x) ((x)%0xff)


/* ------------------------------------------------------------------------- */

/* Global variables */

#define NUM_MEMSPACES 3
#define DEFAULT_DISASSEMBLY_SIZE 40
#define SPACESTRING(val) (val)?"disk":"computer"

extern FILE *mon_output;
extern int sidefx;
extern int exit_mon;
extern int default_datatype;
extern int default_readspace;
extern int default_writespace;
extern bool asm_mode;
extern MEMSPACE caller_space;
extern BREAK_LIST *breakpoints[NUM_MEMSPACES];
extern BREAK_LIST *watchpoints_load[NUM_MEMSPACES];
extern BREAK_LIST *watchpoints_store[NUM_MEMSPACES];
extern unsigned mon_mask[NUM_MEMSPACES];

#define any_breakpoints(mem) (breakpoints[(mem)] != NULL)
#define any_watchpoints_load(mem) (watchpoints_load[(mem)] != NULL)
#define any_watchpoints_store(mem) (watchpoints_store[(mem)] != NULL)
#define any_watchpoints(mem) (watchpoints_load[(mem)] || watchpoints_store[(mem)])

#define check_breakpoints(mem, addr) check_checkpoint(mem, addr, breakpoints[mem])
#define check_watchpoints_load(mem, addr) check_checkpoint(mem, addr, watchpoints_load[mem])
#define check_watchpoints_store(mem, addr) check_checkpoint(mem, addr, watchpoints_store[mem])
 
extern M_ADDR bad_addr;
extern M_ADDR_RANGE bad_addr_range;

extern MEMSPACE addr_memspace(M_ADDR a);
extern unsigned addr_location(M_ADDR a);
extern void set_addr_memspace(M_ADDR *a, MEMSPACE m);
extern void set_addr_location(M_ADDR *a, unsigned l);
extern bool inc_addr_location(M_ADDR *a, unsigned inc);
extern bool is_valid_addr(M_ADDR a);
extern void change_dir(char *path);
extern M_ADDR new_addr(MEMSPACE m, unsigned l);

M_ADDR addr_range_start(M_ADDR_RANGE ar);
MEMSPACE addr_range_start_memspace(M_ADDR_RANGE ar);
unsigned addr_range_start_location(M_ADDR_RANGE ar);
void set_addr_range_start(M_ADDR_RANGE ar, M_ADDR a);

M_ADDR addr_range_end(M_ADDR_RANGE ar);
MEMSPACE addr_range_end_memspace(M_ADDR_RANGE ar);
unsigned addr_range_end_location(M_ADDR_RANGE ar);
void set_addr_range_end(M_ADDR_RANGE ar, M_ADDR a);

M_ADDR_RANGE new_range(M_ADDR a1, M_ADDR a2);
void free_range(M_ADDR_RANGE ar);

extern unsigned check_addr_limits(unsigned val);
extern bool is_valid_addr_range(M_ADDR_RANGE range);
extern void add_number_to_buffer(int number);
extern void add_string_to_buffer(char *str);
void monitor_init(monitor_interface_t *maincpu_interface,
                  monitor_interface_t *true1541_interface_init);
extern void print_help(void);
extern void start_assemble_mode(M_ADDR addr, char *asm_line);
extern void disassemble_lines(M_ADDR_RANGE range);
extern void display_memory(int data_type, M_ADDR_RANGE range);
extern void move_memory(M_ADDR_RANGE src, M_ADDR dest);
extern void compare_memory(M_ADDR_RANGE src, M_ADDR dest);
extern void fill_memory(M_ADDR_RANGE dest, unsigned char *data);
extern void hunt_memory(M_ADDR_RANGE dest, unsigned char *data);
extern void mon_load_file(char *filename, M_ADDR start_addr);
extern void mon_save_file(char *filename, M_ADDR_RANGE range);
extern void mon_verify_file(char *filename, M_ADDR start_addr);
extern void instructions_step(int count);
extern void instructions_next(int count);
extern void stack_up(int count);
extern void stack_down(int count);
extern void block_cmd(int op, int track, int sector, M_ADDR addr);
extern void switch_breakpt(int op, int breakpt_num);
extern void set_ignore_count(int breakpt_num, int count);
extern void print_breakpts(void);
extern void delete_breakpoint(int brknum);
extern void set_brkpt_condition(int brk_num, CONDITIONAL_NODE *cnode);
extern void set_breakpt_command(int brk_num, char *cmd);
extern int add_breakpoint(M_ADDR_RANGE range, bool is_trace, bool is_load, bool is_store);
extern void print_convert(int val);
extern check_checkpoint(MEMSPACE mem, ADDRESS addr, BREAK_LIST *list);

extern unsigned int get_reg_val(MEMSPACE mem, int reg_id);
extern unsigned char get_mem_val(MEMSPACE mem, unsigned mem_addr);
extern void set_reg_val(int reg_id, MEMSPACE mem, WORD val);
extern void set_mem_val(MEMSPACE mem, unsigned mem_addr, unsigned char val);
extern void print_registers();
extern void jump(M_ADDR addr);

extern char *symbol_table_lookup_name(MEMSPACE mem, ADDRESS addr);
extern int symbol_table_lookup_addr(MEMSPACE mem, char *name);
extern void add_name_to_symbol_table(MEMSPACE mem, char *name, ADDRESS addr);
extern void remove_name_from_symbol_table(MEMSPACE mem, char *name);
extern void print_symbol_table(MEMSPACE mem);
extern void free_symbol_table(MEMSPACE mem);
extern void mon_load_symbols(char *filename, MEMSPACE mem);
extern void mon_save_symbols(char *filename, MEMSPACE mem);

extern void watch_push_load_addr(ADDRESS addr, MEMSPACE mem);
extern void watch_push_store_addr(ADDRESS addr, MEMSPACE mem);
extern bool mon_force_import(MEMSPACE mem);

extern void mon_helper(ADDRESS a);
extern void mon(ADDRESS a);

extern int mon_assemble_instr(char *opcode_name, unsigned operand);
#endif
