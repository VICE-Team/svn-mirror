/*
 * montypes.h - The VICE built-in monitor, internal interface.
 *
 * Written by
 *  Daniel Sladic <sladic@eecg.toronto.edu>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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

#ifndef _MONTYPES_H
#define _MONTYPES_H

#include "monitor.h"
#include "types.h"

/* Types */

typedef int bool;

enum t_reg_id {
   e_A,
   e_X,
   e_Y,
   e_PC,
   e_SP,
   e_FLAGS,
   e_AF,
   e_BC,
   e_DE,
   e_HL,
   e_IX,
   e_IY,
   e_I,
   e_R,
   e_AF2,
   e_BC2,
   e_DE2,
   e_HL2
};
typedef enum t_reg_id REG_ID;

enum t_memory_op {
   e_load,
   e_store,
   e_load_store
};
typedef enum t_memory_op MEMORY_OP;

typedef unsigned int MON_ADDR;
typedef unsigned int MON_REG;

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

enum t_radixtype {
   e_default_radix,
   e_hexadecimal,
   e_decimal,
   e_octal,
   e_binary
};
typedef enum t_radixtype RADIXTYPE;

enum t_action {
   e_OFF = 0,
   e_ON = 1,
   e_TOGGLE = 2
};
typedef enum t_action ACTION;

struct cond_node_s {
   int operation;
   int value;
   MON_REG reg_num;
   bool is_reg;
   bool is_parenthized;
   struct cond_node_s *child1;
   struct cond_node_s *child2;
};
typedef struct cond_node_s cond_node_t;

typedef void monitor_toggle_func_t(int value);

/* Defines */
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define HI16(x) ((x)&0xffff0000)
#define LO16(x) ((x)&0xffff)
#define LO16_TO_HI16(x) (((x)&0xffff)<<16)
#define HI16_TO_LO16(x) (((x)>>16)&0xffff)

#define STATE_INITIAL  0
#define STATE_FNAME    1
#define STATE_REG_ASGN 2
#define STATE_ROL      3
#define STATE_BNAME    4
#define STATE_CTYPE    5

#define FIRST_SPACE e_comp_space
#define LAST_SPACE e_disk9_space

#define DEFAULT_DISASSEMBLY_SIZE 40

#define any_watchpoints_load(mem) (watchpoints_load[(mem)] != NULL)
#define any_watchpoints_store(mem) (watchpoints_store[(mem)] != NULL)

#define new_cond ((cond_node_t *)(lib_malloc(sizeof(cond_node_t))))
#define addr_memspace(ma) (HI16_TO_LO16(ma))
#define addr_location(ma) (LO16(ma))
#define new_addr(m, l) (LO16_TO_HI16(m) | (l))
#define new_reg(m, r) (LO16_TO_HI16(m) | (r))
#define reg_memspace(mr) (HI16_TO_LO16(mr))
#define reg_regid(mr) (LO16(mr))

/* Global variables */

extern const char *_mon_space_strings[];

struct console_s;
struct monitor_cpu_type_s;

extern struct console_s *console_log;
extern int sidefx;
extern int exit_mon;
extern int mon_console_close_on_leaving;
extern RADIXTYPE default_radix;
extern MEMSPACE default_memspace;
extern bool asm_mode;
extern char *playback_name;
extern bool playback;
extern MON_ADDR asm_mode_addr;
extern struct monitor_cpu_type_s monitor_cpu_type;
extern MON_ADDR dot_addr[NUM_MEMSPACES];
extern const char *mon_memspace_string[];
extern int mon_stop_output;
extern monitor_interface_t *mon_interfaces[NUM_MEMSPACES];
extern bool force_array[NUM_MEMSPACES];
extern unsigned char data_buf[256];
extern unsigned int data_buf_len;

/* Function declarations */
extern void mon_add_number_to_buffer(int number);
extern void mon_add_string_to_buffer(char *str);
extern void mon_display_screen(void);
extern void mon_instructions_step(int count);
extern void mon_instructions_next(int count);
extern void mon_instruction_return(void);
extern void mon_stack_up(int count);
extern void mon_stack_down(int count);
extern void mon_print_convert(int val);
extern void mon_change_dir(const char *path);
extern void mon_bank(MEMSPACE mem, const char *bank);
extern void mon_display_io_regs(void);
extern void mon_evaluate_default_addr(MON_ADDR *a);
extern void mon_set_mem_val(MEMSPACE mem, WORD mem_addr, BYTE val);
extern bool mon_inc_addr_location(MON_ADDR *a, unsigned inc);
extern void mon_start_assemble_mode(MON_ADDR addr, char *asm_line);
extern long mon_evaluate_address_range(MON_ADDR *start_addr, MON_ADDR *end_addr,
                                       bool must_be_range, WORD default_len);

extern bool check_drive_emu_level_ok(int drive_num);
extern void mon_print_conditional(cond_node_t *cnode);
extern void mon_delete_conditional(cond_node_t *cnode);
extern int mon_evaluate_conditional(cond_node_t *cnode);
extern bool mon_is_valid_addr(MON_ADDR a);
extern bool mon_is_in_range(MON_ADDR start_addr, MON_ADDR end_addr,
                            unsigned loc);
extern void mon_print_bin(int val, char on, char off);
extern BYTE mon_get_mem_val(MEMSPACE mem, WORD mem_addr);
extern BYTE mon_get_mem_val_ex(MEMSPACE mem, int bank, WORD mem_addr);
extern void mon_jump(MON_ADDR addr);
extern void mon_keyboard_feed(const char *string);
extern char *mon_symbol_table_lookup_name(MEMSPACE mem, WORD addr);
extern int mon_symbol_table_lookup_addr(MEMSPACE mem, char *name);
extern void mon_add_name_to_symbol_table(MON_ADDR addr, char *name);
extern void mon_remove_name_from_symbol_table(MEMSPACE mem, char *name);
extern void mon_print_symbol_table(MEMSPACE mem);
extern void mon_load_symbols(MEMSPACE mem, const char *filename);
extern void mon_save_symbols(MEMSPACE mem, const char *filename);

extern void mon_record_commands(char *filename);
extern void mon_end_recording(void);

#endif

