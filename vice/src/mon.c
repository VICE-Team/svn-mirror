/*
 * mon.c - The VICE built-in monitor.
 *
 * Written by
 *  Daniel Sladic (sladic@eecg.toronto.edu)
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

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>

#include "asm.h"
#undef M_ADDR
#include "mon.h"
#include "charsets.h"
#include "file.h"
#include "misc.h"
#include "mshell.h"
#include "interrupt.h"
#include "resources.h"
#include "vsync.h"

#define TEST(x) ((x)!=0)

static monitor_interface_t *maincpu_interface;
static monitor_interface_t *true1541_interface;

/* Global variables */

FILE *mon_output = stdout;

extern void parse_and_execute_line(char *input);
extern char *readline ( char *prompt );
extern void add_history ( char *str );

char *myinput = NULL, *last_cmd = NULL;
int exit_mon = 0;

int sidefx;
int default_datatype;
int default_readspace;
int default_writespace;
bool inside_monitor = FALSE;
unsigned instruction_count;
bool icount_is_next;
BREAK_LIST *breakpoints[NUM_MEMSPACES];
BREAK_LIST *watchpoints_load[NUM_MEMSPACES];
BREAK_LIST *watchpoints_store[NUM_MEMSPACES];
monitor_interface_t *mon_interfaces[NUM_MEMSPACES];

MEMSPACE caller_space;

ADDRESS watch_load_array[5][NUM_MEMSPACES];
ADDRESS watch_store_array[5][NUM_MEMSPACES];
unsigned watch_load_count[NUM_MEMSPACES];
unsigned watch_store_count[NUM_MEMSPACES];
bool force_array[NUM_MEMSPACES];
symbol_table_t monitor_labels[NUM_MEMSPACES];

M_ADDR dot_addr[NUM_MEMSPACES];
int breakpoint_count;
unsigned char data_buf[256];
unsigned data_buf_len;
bool asm_mode;
M_ADDR asm_mode_addr;
unsigned next_or_step_stop;
unsigned mon_mask[NUM_MEMSPACES];

bool watch_load_occurred;
bool watch_store_occurred;

char *cond_op_string[] = { "",
                           "==",
                           "!=",
                           ">",
                           "<",
                           ">=",
                           "<=",
                           "&&",
                           "||"
                          };


int default_display_number[] = {100, /* default = hex */
                                100, /* hexadecimal */
                                100, /* decimal */
                                20, /* binary */
                                40, /* octal */
                                 8, /* character */
                                 1, /* sprite */
                                80, /* text ascii */
                                80, /* text petscii */
                                10  /* asm */
                               };

int default_display_per_line[] = { 8, /* default = hex */
                                   8, /* hexadecimal */
                                   8, /* decimal */
                                   2, /* binary */
                                   8, /* octal */
                                   1, /* character */
                                   1, /* sprite */
                                  10, /* text ascii */
                                  10, /* text petscii */
                                   1  /* asm */
                                 };

char *memspace_string[] = {"", "C", "8" };

char *register_string[] = { "A",
                            "X",
                            "Y",
                            "PC",
                            "SP"
                          };


char *datatype_string[] = { "",
                            "hexadecimal",
                            "decimal",
                            "binary",
                            "octal",
                            "character",
                            "sprite",
                            "text ascii",
                            "text petscii",
                            "asm"
                          };


/* *** REGISTER AND MEMORY OPERATIONS *** */


unsigned int get_reg_val(MEMSPACE mem, int reg_id)
{
    mos6510_regs_t *reg_ptr = NULL;

    if (mem == e_disk_space) {
        if (true1541_interface == NULL) {
            fprintf(mon_output, "True1541 emulation not supported for this machine.\n");
            return 0;
        }

/* FIXME: IMO, the user should be allowed to access the true 1541 emulation
   even when it's (temporarily?) disabled.  */
#if 0
        else if (!app_resources.true1541) {
            fprintf(mon_output, "True1541 emulation is not turned on.\n");
            return 0;
        }
#endif
    }

    reg_ptr = mon_interfaces[mem]->cpu_regs;

    switch(reg_id) {
      case e_A:
        return reg_ptr->a;
      case e_X:
        return reg_ptr->x;
      case e_Y:
        return reg_ptr->y;
      case e_PC:
        return reg_ptr->pc;
      case e_SP:
        return reg_ptr->sp;
      default:
        assert(FALSE);
    }
    return 0;
}

void set_reg_val(int reg_id, MEMSPACE mem, WORD val)
{
    mos6510_regs_t *reg_ptr = NULL;

    if (mem == e_disk_space) {
        if (true1541_interface == NULL) {
            fprintf(mon_output, "True1541 emulation not supported for this machine.\n");
            return;
        }
#if 0
        else if (!app_resources.true1541) {
            fprintf(mon_output, "True1541 emulation is not turned on.\n");
            return;
        }
#endif
    }

    reg_ptr = mon_interfaces[mem]->cpu_regs;

    switch(reg_id) {
      case e_A:
        reg_ptr->a = val;
        break;
      case e_X:
        reg_ptr->x = val;
        break;
      case e_Y:
        reg_ptr->y = val;
        break;
      case e_PC:
        reg_ptr->pc = val;
        break;
      case e_SP:
        reg_ptr->sp = val;
        break;
      default:
        assert(FALSE);
    }
    force_array[mem] = TRUE;
}

void print_registers(MEMSPACE mem)
{
    mos6510_regs_t *regs;

    if (mem == e_default_space)
        mem = default_readspace;
    else if (mem == e_disk_space) {
        if (true1541_interface == NULL) {
            fprintf(mon_output, "True1541 emulation not supported for this machine.\n");
            return;
        }
#if 0
        else if (!app_resources.true1541) {
            fprintf(mon_output, "True1541 emulation is not turned on.\n");
            return;
        }
#endif
    } else
        assert(FALSE);

    regs = mon_interfaces[mem]->cpu_regs;
    fprintf(mon_output, "  ADDR AR XR YP SP 01 NV-BDIZC\n");
    fprintf(mon_output, ".;%04x %02x %02x %02x %02x %02x %d%d%c%d%d%d%d%d\n",
            get_reg_val(mem,e_PC), get_reg_val(mem,e_A), get_reg_val(mem,e_X),
            get_reg_val(mem,e_Y), get_reg_val(mem,e_SP), get_mem_val(mem,1),
            TEST(regs->p.n), TEST(regs->p.v), 'x', TEST(regs->p.b),
            TEST(regs->p.d), TEST(regs->p.i), TEST(regs->p.z),
            TEST(regs->p.c));
}

unsigned char get_mem_val(MEMSPACE mem, unsigned mem_addr)
{
   if (mem == e_disk_space) {
       if (true1541_interface == NULL) {
           fprintf(mon_output, "True1541 emulation not supported for this machine.\n");
           return 0;
       }
#if 0
       else if (!app_resources.true1541) {
           fprintf(mon_output, "True1541 emulation is not turned on.\n");
           return 0;
       }
#endif
   }

   return mon_interfaces[mem]->read_func(mem_addr);
}

void set_mem_val(MEMSPACE mem, unsigned mem_addr, unsigned char val)
{
   if (mem == e_comp_space) {
   } else if (mem == e_disk_space) {
       if (true1541_interface == NULL) {
           fprintf(mon_output, "True1541 emulation not supported for this machine.\n");
           return;
       }
#if 0
       else if (!app_resources.true1541) {
           fprintf(mon_output, "True1541 emulation is not turned on.\n");
           return;
       }
#endif
   } else
       assert(FALSE);

   mon_interfaces[mem]->store_func(mem_addr, val);
}

void jump(M_ADDR addr)
{
   set_reg_val(e_PC, addr_memspace(addr), addr_location(addr));
   exit_mon = 1;
}

/* *** ADDRESS FUNCTIONS *** */

MEMSPACE addr_memspace(M_ADDR a) { return HI16_TO_LO16(a); }
unsigned addr_location(M_ADDR a) { return LO16(a); }
void set_addr_memspace(M_ADDR *a, MEMSPACE m) { *a = LO16(*a) | LO16_TO_HI16(m); }
void set_addr_location(M_ADDR *a, unsigned l) { *a = HI16(*a) | LO16(l); }
bool inc_addr_location(M_ADDR *a, unsigned inc)
{
   unsigned new_loc = LO16(*a) + inc;
   *a = HI16(*a) | LO16(new_loc);

   return !(new_loc == LO16(new_loc));
}
bool is_valid_addr(M_ADDR a) { return HI16_TO_LO16(a) != e_invalid_space; }
M_ADDR new_addr(MEMSPACE m, unsigned l) { return (m<<16)|l; }
M_ADDR evaluate_default_addr(M_ADDR *a, bool is_read)
{
   if (addr_memspace(*a) != e_default_space)
      return *a;

   if (is_read)
      set_addr_memspace(a,default_readspace);
   else
      set_addr_memspace(a,default_writespace);

   return *a;
}

M_ADDR bad_addr;

/* *** ADDRESS RANGE FUNCTIONS *** */

#ifdef LONG_LONG

M_ADDR addr_range_start(M_ADDR_RANGE ar) { return LO32(ar); }
MEMSPACE addr_range_start_memspace(M_ADDR_RANGE ar) { return addr_memspace(LO32(ar)); }
unsigned addr_range_start_location(M_ADDR_RANGE ar) { return addr_location(LO32(ar)); }
void set_addr_range_start(M_ADDR_RANGE ar, M_ADDR a) { ar = HI32(ar) | a; }

M_ADDR addr_range_end(M_ADDR_RANGE ar) { return HI32_TO_LO32(ar); }
MEMSPACE addr_range_end_memspace(M_ADDR_RANGE ar) { return addr_memspace(HI32_TO_LO32(ar)); }
unsigned addr_range_end_location(M_ADDR_RANGE ar) { return addr_location(HI32_TO_LO32(ar)); }
void set_addr_range_end(M_ADDR_RANGE ar, M_ADDR a) { ar = LO32(ar) | LO32_TO_HI32(a); }

M_ADDR_RANGE new_range(M_ADDR a1, M_ADDR a2) { return LO32_TO_HI32(a2) | a1; }
void free_range(M_ADDR_RANGE ar) {;}

#else

M_ADDR addr_range_start(M_ADDR_RANGE ar) { return ar->start_addr; }
MEMSPACE addr_range_start_memspace(M_ADDR_RANGE ar) { return addr_memspace(ar->start_addr); }
unsigned addr_range_start_location(M_ADDR_RANGE ar) { return addr_location(ar->start_addr); }
void set_addr_range_start(M_ADDR_RANGE ar, M_ADDR a) { ar->start_addr = a; }

M_ADDR addr_range_end(M_ADDR_RANGE ar) { return ar->end_addr; }
MEMSPACE addr_range_end_memspace(M_ADDR_RANGE ar) { return addr_memspace(ar->end_addr); }
unsigned addr_range_end_location(M_ADDR_RANGE ar) { return addr_location(ar->end_addr); }
void set_addr_range_end(M_ADDR_RANGE ar, M_ADDR a) { ar->end_addr = a; }

M_ADDR_RANGE new_range(M_ADDR a1, M_ADDR a2)
{
   M_ADDR_RANGE ar;
   ar = (M_ADDR_RANGE)(malloc(sizeof(struct t_address_range)));
   set_addr_range_start(ar, a1);;
   set_addr_range_end(ar, a2);;

   return ar;
}
void free_range(M_ADDR_RANGE ar) { free(ar); }

#endif

bool is_in_range(M_ADDR_RANGE ar, unsigned loc)
{
   unsigned start, end;

   start = addr_range_start_location(ar);

   if (!is_valid_addr(addr_range_end(ar)))
      return (loc == start);

   end = addr_range_end_location(ar);

   if (end < start)
      return ((loc>=start) || (loc<=end));

   return ((loc>=start) && (loc<=end));
}

bool is_valid_range(M_ADDR_RANGE range)
{
   return (addr_memspace(addr_range_start(range)) != e_invalid_space);
}

void evaluate_default_addr_range(M_ADDR_RANGE *range, bool is_read)
{
   M_ADDR a1, a2;
   MEMSPACE mem1, mem2, def;

   a1 = addr_range_start(*range);
   a2 = addr_range_end(*range);
   mem1 = addr_memspace(a1);
   mem2 = addr_memspace(a2);

   if (is_read)
      def = default_readspace;
   else
      def = default_writespace;

   assert(mem1 != e_invalid_space);

   if (mem1 == e_default_space) {
      if (mem2 == e_default_space) {
         set_addr_memspace(&a1,def);
         set_addr_memspace(&a2,def);
      } else if (mem2 != e_invalid_space) {
         set_addr_memspace(&a1,mem2);
      } else {
         set_addr_memspace(&a1,def);
      }
   } else {
      if (mem2 == e_default_space) {
         set_addr_memspace(&a2,mem1);
      } else if (mem2 != e_invalid_space) {
         assert(mem1 == mem2);
      }
   }
   set_addr_range_start(*range,a1);
   set_addr_range_end(*range,a2);
}

M_ADDR_RANGE bad_addr_range;

/* *** ULTILITY FUNCTIONS *** */

void check_address(M_ADDR addr) {}
void check_range(M_ADDR_RANGE range) {}

unsigned check_addr_limits(unsigned val)
{
   if (val != LO16(val))
   {
      fprintf(mon_output, "Overflow warning: $%x -> $ffff\n", val);
      return 0xffff;
   }

   return val;
}

bool is_valid_addr_range(M_ADDR_RANGE range)
{
   M_ADDR start, end;

   start = addr_range_start(range);
   end = addr_range_end(range);

   if ((addr_memspace(start) != addr_memspace(end)) &&
       ((addr_memspace(start) != e_default_space) ||
        (addr_memspace(end) != e_default_space))) {
      fprintf(mon_output, "Invalid range: Endpoints are in different"
             " memory spaces\n");
      return FALSE;
   }
   return TRUE;
}

void print_bin(int val, char on, char off)
{
   int divisor;
   char digit;

   if (val > 4095)
      divisor = 32768;
   else if (val > 255)
      divisor = 2048;
   else
      divisor = 128;

   while (divisor) {
      digit = (val & divisor) ? on : off;
      fprintf(mon_output, "%c",digit);
      if (divisor == 256)
         fprintf(mon_output, " ");
      divisor /= 2;
   }
}

void print_hex(int val)
{
   if (val > 255)
      fprintf(mon_output, "0x%04x\n",val);
   else
      fprintf(mon_output, "0x%02x\n",val);
}

void print_octal(int val)
{
   if (val > 511)
      fprintf(mon_output, "0%06o\n",val);
   else
      fprintf(mon_output, "0%03o\n",val);
}


void print_convert(int val)
{
   fprintf(mon_output, "+%d\n",val);
   print_hex(val);
   print_octal(val);
   print_bin(val,'1','0'); fprintf(mon_output, "\n");
}

void add_number_to_buffer(int number)
{
   data_buf[data_buf_len++] = (number & 0xff);
   if (number > 0xff)
      data_buf[data_buf_len++] = ( (number>>8) & 0xff);
   data_buf[data_buf_len] = '\0';
}

void add_string_to_buffer(char *str)
{
   strcpy(&(data_buf[data_buf_len]), str);
   data_buf_len += strlen(str);
   data_buf[data_buf_len] = '\0';
   free(str);
}

void clear_buffer()
{
   data_buf_len = 0;
}

void memory_to_string(char *buf, MEMSPACE mem, unsigned addr, unsigned len, bool petscii)
{
    int i, val;

    for (i=0;i<len;i++) {
       val = get_mem_val(mem, addr);

       if (petscii)
          buf[i] = p_toascii(val,0);
       if (isprint(val))
          buf[i] = val;
       else
          buf[i] = '.';

       addr++;
    }
}


/* *** MISC COMMANDS *** */

extern int yydebug;

void monitor_init(monitor_interface_t *maincpu_interface_init,
                  monitor_interface_t *true1541_interface_init)
{
   int i, j;

   yydebug = 0;
   sidefx = e_OFF;
   default_datatype = e_hexadecimal;
   default_readspace = e_comp_space;
   default_writespace = e_comp_space;
   instruction_count = 0;
   icount_is_next = FALSE;
   breakpoint_count = 1;
   data_buf_len = 0;
   asm_mode = 0;
   next_or_step_stop = 0;

   watch_load_occurred = FALSE;
   watch_store_occurred = FALSE;

   for (i=1;i<NUM_MEMSPACES;i++) {
      dot_addr[i] = new_addr(e_default_space + i, 0);
      watch_load_count[i] = 0;
      watch_store_count[i] = 0;
      mon_mask[i] = MI_NONE;
      monitor_labels[i].name_list = NULL;
      for (j=0;j<HASH_ARRAY_SIZE;j++)
         monitor_labels[i].addr_hash_table[j] = NULL;
   }

   bad_addr = new_addr(e_invalid_space, 0);
   bad_addr_range = new_range(bad_addr, bad_addr);

   caller_space = e_comp_space;

   asm_mode_addr = bad_addr;

   maincpu_interface = maincpu_interface_init;
   true1541_interface = true1541_interface_init;

   mon_interfaces[e_comp_space] = maincpu_interface;
   mon_interfaces[e_disk_space] = true1541_interface;
}

void print_help()
{
    fprintf(mon_output, "No help yet.\n");
}


/* *** ASSEMBLY/DISASSEMBLY *** */


void start_assemble_mode(M_ADDR addr, char *asm_line)
{
   asm_mode = 1;

   assert(is_valid_addr(addr));
   evaluate_default_addr(&addr, FALSE);
   asm_mode_addr = addr;
}

int mon_assemble_instr(char *opcode_name, unsigned operand)
{
   WORD operand_value = LO16(operand);
   WORD operand_mode = HI16_TO_LO16(operand);
   BYTE opcode = 0;
   int i, len, branch_offset;
   bool found = FALSE;
   MEMSPACE mem;
   unsigned loc;

   mem = addr_memspace(asm_mode_addr);
   loc = addr_location(asm_mode_addr);

   for (i=0;i<=0xff;i++) {
      if (!strcasecmp(lookup[i].mnemonic, opcode_name)) {
         if (lookup[i].addr_mode == operand_mode) {
            opcode = i;
            found = TRUE;
         }

         /* Special case: Register A not specified for ACCUMULATOR mode. */
         if ( (operand_mode == IMPLIED) && (lookup[i].addr_mode == ACCUMULATOR) ) {
            opcode = i;
            found = TRUE;
         }

         /* Special case: RELATIVE mode looks like ZERO_PAGE or ABSOLUTE modes */
         if ( ((operand_mode == ZERO_PAGE) || (operand_mode == ABSOLUTE)) &&
              (lookup[i].addr_mode == RELATIVE)) {
            branch_offset = operand_value - loc - 2;
            if ( (branch_offset > 127) || (branch_offset < -128) ) {
               fprintf(mon_output, "Branch offset too large\n");
               return -1;
            }
            operand_value = (branch_offset & 0xff);
            opcode = i;
            found = TRUE;
         }
      }
   }

   if (!found) {
      fprintf(mon_output, "\"%s\" is not a valid opcode.\n",opcode_name);
      return -1;
   }

   len = clength[lookup[opcode].addr_mode];

   ram[loc] = opcode;
   if (len >= 2)
      ram[loc+1] = operand_value & 0xff;
   if (len >= 3)
      ram[loc+2] = (operand_value >> 8) & 0xff;

   if (len >= 0) {
      inc_addr_location(&asm_mode_addr, len);
      dot_addr[mem] = asm_mode_addr;
   } else {
      fprintf(mon_output, "Assemble error: %d\n",len);
   }
   return len;
}

unsigned disassemble_instr(M_ADDR addr)
{
   BYTE op, p1, p2;
   MEMSPACE mem;
   unsigned loc, mode;

   mem = addr_memspace(addr);
   loc = addr_location(addr);

   op = get_mem_val(mem, loc);
   p1 = get_mem_val(mem, loc+1);
   p2 = get_mem_val(mem, loc+2);

   /* sprint_disassembled() only supports hex and decimal.
    * Unless the default datatype is decimal, we default
    * to hex.
    */
   mode = (default_datatype == e_decimal) ? 0 : MODE_HEX;

   fprintf(mon_output, ".%s:%04x   %s\n",memspace_string[mem],loc,
           sprint_disassembled(loc, op, p1, p2, mode));
   return clength[lookup[op].addr_mode];
}

void disassemble_lines(M_ADDR_RANGE range)
{
   M_ADDR start, end;
   MEMSPACE mem;
   unsigned end_loc;
   bool valid_range;

   valid_range = is_valid_range(range);

   if (valid_range) {
      evaluate_default_addr_range(&range, TRUE);
      mem = addr_memspace(addr_range_start(range));
      start = addr_range_start(range);
      end = addr_range_end(range);
      dot_addr[mem] = start;

      if (is_valid_addr(end))
         end_loc = addr_location(end);
      else
         end_loc = addr_location(start) + DEFAULT_DISASSEMBLY_SIZE;
   } else {
      mem = default_readspace;
      end_loc = addr_location(dot_addr[mem]) + DEFAULT_DISASSEMBLY_SIZE;
   }

   end_loc = end_loc & 0xffff;

   while (addr_location(dot_addr[mem]) <= end_loc)
      inc_addr_location(&(dot_addr[mem]), disassemble_instr(dot_addr[mem]));

   free_range(range);
}


/* *** MEMORY COMMANDS *** */


void display_memory(int data_type, M_ADDR_RANGE range)
{
   unsigned i, addr, last_addr, max_width, real_width;
   char printables[50];
   MEMSPACE mem;

   if (!data_type)
      data_type = default_datatype;

   if (is_valid_range(range)) {
      evaluate_default_addr_range(&range, TRUE);
      addr = addr_range_start_location(range);
      mem = addr_memspace(addr_range_start(range));
   }
   else {
      mem = default_readspace;
      addr = addr_location(dot_addr[mem]);
   }

   if (is_valid_addr(addr_range_end(range)))
      last_addr = addr_range_end_location(range);
   else
      last_addr = addr + default_display_number[data_type] - 1;

   while (addr <= last_addr) {
      fprintf(mon_output, ">%s:%04x ",memspace_string[mem],addr);
      max_width = default_display_per_line[data_type];
      for (i=0,real_width=0;i<max_width;i++) {
         switch(data_type) {
            case e_text_petscii:
               fprintf(mon_output, "%c",p_toascii(get_mem_val(mem,addr+i),0));
               real_width++;
               break;
            case e_decimal:
               memset(printables,0,50);
               if (addr+i <= last_addr) {
                  fprintf(mon_output, "%3d ",get_mem_val(mem,addr+i));
                  real_width++;
               }
               else
                  fprintf(mon_output, "    ");
               break;
            case e_hexadecimal:
               memset(printables,0,50);
               if (addr+i <= last_addr) {
                  fprintf(mon_output, "%02x ",get_mem_val(mem,addr+i));
                  real_width++;
               }
               else
                  fprintf(mon_output, "    ");
               break;
            case e_octal:
               memset(printables,0,50);
               if (addr+i <= last_addr) {
                  fprintf(mon_output, "%03o ",get_mem_val(mem,addr+i));
                  real_width++;
               }
               else
                  fprintf(mon_output, "    ");
               break;
            case e_character:
               print_bin(get_mem_val(mem,addr+i),'*','.');
               fprintf(mon_output, " \n");
               real_width++;
               break;
            default:

         }

      }

      if (data_type == e_decimal || data_type == e_hexadecimal) {
         memory_to_string(printables, mem, addr, real_width, FALSE);
         fprintf(mon_output, "\t%s",printables);
      }
      fprintf(mon_output, "\n");
      addr += real_width;
   }

   set_addr_location(&(dot_addr[mem]),last_addr+1);

   free_range(range);
}


void move_memory(M_ADDR_RANGE src, M_ADDR dest)
{
  unsigned i, len, start, end, dst;
  MEMSPACE src_mem, dest_mem;
  BYTE *buf;

  if (is_valid_range(src))
     evaluate_default_addr_range(&src, TRUE);
  else
     assert(FALSE);

  start = addr_range_start_location(src);
  end = addr_range_end_location(src);

  assert(is_valid_addr(dest));
  evaluate_default_addr(&dest,FALSE);
  dst = addr_location(dest);
  len = end - start + 1;
  buf = (BYTE *) malloc(sizeof(BYTE) * len);

  src_mem = addr_range_start_memspace(src);
  dest_mem = addr_memspace(dest);

  if (len < 0) len += 65536;

  for (i=0; i<len; i++)
     buf[i] = get_mem_val(src_mem, start+i);

  for (i=0; i<len; i++) {
     set_mem_val(dest_mem, dst+i, buf[i]);
  }

  free_range(src);
}


void compare_memory(M_ADDR_RANGE src, M_ADDR dest)
{
  unsigned i, len, start, end, dst;
  MEMSPACE src_mem, dest_mem;
  BYTE byte1, byte2;

  if (is_valid_range(src))
     evaluate_default_addr_range(&src, TRUE);
  else
     assert(FALSE);

  start = addr_range_start_location(src);
  end = addr_range_end_location(src);

  assert(is_valid_addr(dest));
  evaluate_default_addr(&dest,TRUE);
  dst = addr_location(dest);
  len = end - start + 1;
  if (len < 0) len += 65536;

  src_mem = addr_range_start_memspace(src);
  dest_mem = addr_memspace(dest);

  for (i=0; i<len; i++) {
     byte1 = get_mem_val(dest_mem, dst+1);
     byte2 = get_mem_val(src_mem, start+1);

     if (byte1 != byte2)
        fprintf(mon_output, "$%04x $%04x: %02x %02x\n",start+i, dst+i, byte1, byte2);
  }

  free_range(src);
}


void fill_memory(M_ADDR_RANGE dest, unsigned char *data)
{
  unsigned i, index, len, start, end;
  MEMSPACE dest_mem;

  if (is_valid_range(dest))
     evaluate_default_addr_range(&dest, TRUE);
  else
     assert(FALSE);

  start = addr_range_start_location(dest);
  end = addr_range_end_location(dest);
  len = end - start + 1;
  if (len < 0) len += 65536;

  dest_mem = addr_range_start_memspace(dest);

  i = 0;
  index = 0;
  while (i <= len) {
     set_mem_val(dest_mem, start+i, data_buf[index++]);
     if (index >= strlen(data_buf)) index = 0;
     i++;
  }

  clear_buffer();
  free_range(dest);
}


void hunt_memory(M_ADDR_RANGE dest, unsigned char *data)
{
  unsigned len, start, end, data_len, i, next_read;
  BYTE *buf;
  MEMSPACE mem;

  if (is_valid_range(dest))
     evaluate_default_addr_range(&dest, TRUE);
  else
     assert(FALSE);

  mem = addr_range_start_memspace(dest);
  start = addr_range_start_location(dest);
  end = addr_range_end_location(dest);
  len = end - start + 1;
  if (len < 0) len += 0x10000;

  data_len = strlen(data_buf);
  buf = (BYTE *) malloc(sizeof(BYTE) * data_len);

  /* Fill buffer */
  for (i=0; i<data_len; i++)
     buf[i] = get_mem_val(mem, start+i);

  /* Do compares */
  next_read = start + data_len;

  for (i=0; i<(len-data_len); i++,next_read++) {
     if (memcmp(buf,data_buf,data_len) == 0)
        fprintf(mon_output, "0x%04x\n",start+i);

     memmove(&(buf[0]), &(buf[1]), data_len-1);
     buf[data_len-1] = get_mem_val(mem, next_read);
   }

  clear_buffer();
  free_range(dest);
}


/* *** FILE COMMANDS *** */

void change_dir(char *path)
{
    if (chdir(path) < 0) {
	perror(path);
    }

    fprintf(mon_output, "Changing to directory: %s",path);
}


void mon_load_file(char *filename, M_ADDR start_addr)
{
    FILE   *fp;
    ADDRESS adr;
    int     b1, b2;
    int     ch;

    evaluate_default_addr(&start_addr, FALSE);

    if (NULL == (fp = fopen(filename, READ))) {
	perror(filename);
	fprintf(mon_output, "Loading failed.\n");
	return;
    }

    b1 = fgetc(fp);
    b2 = fgetc(fp);

    if (!is_valid_addr(start_addr)) {	/* No Load address given */
	if (b1 == 1)	/* Load Basic */
	    mem_get_basic_text(&adr, NULL);
	else
	    adr = (BYTE)b1 | ((BYTE)b2 << 8);
    } else  {
       adr = addr_location(start_addr);
    }

    fprintf(mon_output, "Loading %s", filename);
    fprintf(mon_output, " from %04X\n", adr);

    ch = fread (ram + adr, 1, ram_size - adr, fp);
    fprintf(mon_output, "%x bytes\n", ch);

    /* set end of load addresses like kernal load */ /*FCP*/
    mem_set_basic_text(adr, adr + ch);

    fclose(fp);
}

void mon_save_file(char *filename, M_ADDR_RANGE range)
{
   FILE   *fp;
   unsigned adr;
   unsigned end;

   assert(is_valid_addr_range(range));
   evaluate_default_addr_range(&range, TRUE);

   adr = addr_range_start_location(range);
   end = addr_range_end_location(range);

   if (NULL == (fp = fopen(filename, WRITE))) {
	perror(filename);
	fprintf(mon_output, "Saving failed.\n");
   } else {
	printf("Saving file `%s'...\n", filename);
	fputc((BYTE) adr & 0xff, fp);
	fputc((BYTE) (adr >> 8) & 0xff, fp);
	fwrite((char *) (ram + adr), 1, end - adr, fp);
	fclose(fp);
   }

   free_range(range);
}

void mon_verify_file(char *filename, M_ADDR start_addr)
{
   assert(is_valid_addr(start_addr));
   evaluate_default_addr(&start_addr, TRUE);

   fprintf(mon_output, "Verify file %s at address 0x%04x\n", filename, addr_location(start_addr));
}

void mon_load_symbols(MEMSPACE mem, char *filename)
{
    FILE   *fp;
    ADDRESS adr;
    char name[256];
    char *name_ptr;

    /* FIXME - something better than this? */
    if (mem == e_default_space)
       mem = e_comp_space;

    if (NULL == (fp = fopen(filename, READ))) {
	perror(filename);
	fprintf(mon_output, "Loading failed.\n");
	return;
    }

    fprintf(mon_output, "Loading symbol table from %s\n", filename);

    while (!feof(fp)) {
       fscanf(fp, "%x %s\n", (int *) &adr, name);
       name_ptr = (char *) malloc((strlen(name)+1) * sizeof(char));
       strcpy(name_ptr, name);
       fprintf(mon_output, "Read (0x%x:%s)\n",adr, name_ptr);
       add_name_to_symbol_table(new_addr(mem, adr), name_ptr);
    }

    fclose(fp);
}

void mon_save_symbols(MEMSPACE mem, char *filename)
{
    FILE   *fp;
    symbol_entry_t *sym_ptr;

    /* FIXME - something better than this? */
    if (mem == e_default_space)
       mem = e_comp_space;

    if (NULL == (fp = fopen(filename, WRITE))) {
	perror(filename);
	fprintf(mon_output, "Saving failed.\n");
	return;
    }

    fprintf(mon_output, "Saving symbol table to %s\n", filename);

    sym_ptr = monitor_labels[mem].name_list;

    while (sym_ptr) {
       fprintf(fp, "%04x %s\n", sym_ptr->addr, sym_ptr->name);
       fprintf(mon_output, "Write (0x%x:%s)\n",sym_ptr->addr,sym_ptr-> name);
       sym_ptr = sym_ptr->next;
    }

    fclose(fp);
}


/* *** SYMBOL TABLE *** */


char *symbol_table_lookup_name(MEMSPACE mem, ADDRESS addr)
{
   symbol_entry_t *sym_ptr;

   sym_ptr = monitor_labels[mem].addr_hash_table[HASH_ADDR(addr)];
   while (sym_ptr) {
      if (addr == sym_ptr->addr)
         return sym_ptr->name;
      sym_ptr = sym_ptr->next;
   }

   return NULL;
}

int symbol_table_lookup_addr(MEMSPACE mem, char *name)
{
   symbol_entry_t *sym_ptr;

   sym_ptr = monitor_labels[mem].name_list;
   while (sym_ptr) {
      printf("CMP: %s %s\n",sym_ptr->name, name);
      if (strcmp(sym_ptr->name, name) == 0)
         return sym_ptr->addr;
      sym_ptr = sym_ptr->next;
   }

   return -1;
}

void add_name_to_symbol_table(M_ADDR addr, char *name)
{
   symbol_entry_t *sym_ptr;
   char *old_name;
   int old_addr;
   MEMSPACE mem = addr_memspace(addr);
   ADDRESS loc = addr_location(addr);

   /* FIXME - something better than this? */
   if (mem == e_default_space)
      mem = e_comp_space;

   if ( (old_name = symbol_table_lookup_name(mem, loc)) ) {
      fprintf(mon_output, "Replacing label %s with %s for address $%0x4x\n",
              old_name, name, loc);
   }
   if ( (old_addr = symbol_table_lookup_addr(mem, name)) >= 0) {
      fprintf(mon_output, "Changing address of label %s from $%04x to $%04x\n",
              name, old_addr, loc);
   }

   /* Add name to name list */
   sym_ptr = (symbol_entry_t *) malloc(sizeof(symbol_entry_t));
   sym_ptr->name = name;
   sym_ptr->addr = loc;

   sym_ptr->next = monitor_labels[mem].name_list;
   monitor_labels[mem].name_list = sym_ptr;

   /* Add address to hash table */
   sym_ptr = (symbol_entry_t *) malloc(sizeof(symbol_entry_t));
   sym_ptr->name = name;
   sym_ptr->addr = addr;

   sym_ptr->next = monitor_labels[mem].addr_hash_table[HASH_ADDR(addr)];
   monitor_labels[mem].addr_hash_table[HASH_ADDR(addr)] = sym_ptr;
}

void remove_name_from_symbol_table(MEMSPACE mem, char *name)
{
   unsigned addr;
   symbol_entry_t *sym_ptr, *prev_ptr;

   /* FIXME - something better than this? */
   if (mem == e_default_space)
      mem = e_comp_space;

   if (name == NULL) {
      /* FIXME - prompt user */
      free_symbol_table(mem);
      return;
   } else if ( (addr = symbol_table_lookup_addr(mem, name)) < 0) {
      fprintf(mon_output, "Symbol %s not found.\n", name);
      return;
   }

   /* Remove entry in name list */
   sym_ptr = monitor_labels[mem].name_list;
   prev_ptr = NULL;
   while (sym_ptr) {
      if (strcmp(sym_ptr->name, name) == 0) {
         /* Name memory is freed below. */
         addr = sym_ptr->addr;
         if (prev_ptr)
            prev_ptr->next = sym_ptr->next;
         else
            monitor_labels[mem].name_list = NULL;

         free(sym_ptr);
         break;
      }
      prev_ptr = sym_ptr;
      sym_ptr = sym_ptr->next;
   }

   /* Remove entry in address hash table */
   sym_ptr = monitor_labels[mem].addr_hash_table[HASH_ADDR(addr)];
   prev_ptr = NULL;
   while (sym_ptr) {
      if (addr == sym_ptr->addr) {
         free (sym_ptr->name);
         if (prev_ptr)
            prev_ptr->next = sym_ptr->next;
         else
            monitor_labels[mem].addr_hash_table[HASH_ADDR(addr)] = NULL;

         free(sym_ptr);
         return;
      }
      prev_ptr = sym_ptr;
      sym_ptr = sym_ptr->next;
   }
}

void print_symbol_table(MEMSPACE mem)
{
   symbol_entry_t *sym_ptr;

   /* FIXME - something better than this? */
   if (mem == e_default_space)
      mem = e_comp_space;

   sym_ptr = monitor_labels[mem].name_list;
   while (sym_ptr) {
      fprintf(mon_output, "$%04x %s\n",sym_ptr->addr, sym_ptr->name);
      sym_ptr = sym_ptr->next;
   }
}

void free_symbol_table(MEMSPACE mem)
{
   symbol_entry_t *sym_ptr, *temp;
   int i;

   /* Remove name list */
   sym_ptr = monitor_labels[mem].name_list;
   while (sym_ptr) {
      /* Name memory is freed below. */
      temp = sym_ptr;
      sym_ptr = sym_ptr->next;
      free(temp);
   }

   /* Remove address hash table */
   for (i=0;i<HASH_ARRAY_SIZE;i++) {
      sym_ptr = monitor_labels[mem].addr_hash_table[i];
      while (sym_ptr) {
         free (sym_ptr->name);
         temp = sym_ptr;
         sym_ptr = sym_ptr->next;
         free(temp);
      }
   }
}


/* *** INSTRUCTION COMMANDS *** */


void instructions_step(int count)
{
   fprintf(mon_output, "Stepping through the next %d instruction(s).\n",
          (count>=0)?count:1);
   instruction_count = (count>=0)?count:1;
   icount_is_next = FALSE;
   exit_mon = 1;

   mon_mask[e_comp_space] |= MI_STEP;
   monitor_trap_on(maincpu_interface->int_status);
}

void instructions_next(int count)
{
   fprintf(mon_output, "Nexting through the next %d instruction(s).\n",
          (count>=0)?count:1);
   instruction_count = (count>=0)?count:1;
   icount_is_next = TRUE;
   exit_mon = 1;

   mon_mask[e_comp_space] |= MI_STEP;
   monitor_trap_on(maincpu_interface->int_status);
}

void stack_up(int count)
{
   fprintf(mon_output, "Going up %d stack frame(s).\n",
          (count>=0)?count:1);
}

void stack_down(int count)
{
   fprintf(mon_output, "Going down %d stack frame(s).\n",
          (count>=0)?count:1);
}


/* *** DISK COMMANDS *** */


void block_cmd(int op, int track, int sector, M_ADDR addr)
{
   assert(is_valid_addr(addr));
   evaluate_default_addr(&addr, op == 0);

   if (!op)
   {
      if (is_valid_addr(addr))
         fprintf(mon_output, "Read track %d sector %d to screen\n", track, sector);
      else
         fprintf(mon_output, "Read track %d sector %d into address 0x%04x\n", track, sector, addr_location(addr));
   }
   else
   {
      fprintf(mon_output, "Write data from address 0x%04x to track %d sector %d\n",
              addr_location(addr), track, sector);
   }

}


/* *** CONDITIONAL EXPRESSIONS *** */

void print_conditional(CONDITIONAL_NODE *cnode)
{
   /* Do an in-order traversal of the tree */
   if (cnode->operation != e_INV)
   {
      assert(cnode->child1 && cnode->child2);
      print_conditional(cnode->child1);
      fprintf(mon_output, " %s ",cond_op_string[cnode->operation]);
      print_conditional(cnode->child2);
   }
   else
   {
      if (cnode->is_reg)
         fprintf(mon_output, "%s",register_string[cnode->reg_num]);
      else
         fprintf(mon_output, "%d",cnode->value);
   }
}


int evaluate_conditional(CONDITIONAL_NODE *cnode)
{
   /* Do a post-order traversal of the tree */
   if (cnode->operation != e_INV)
   {
      assert(cnode->child1 && cnode->child2);
      evaluate_conditional(cnode->child1);
      evaluate_conditional(cnode->child2);

      switch(cnode->operation) {
         case e_EQU:
            cnode->value = ((cnode->child1->value) == (cnode->child2->value));
            break;
         case e_NEQ:
            cnode->value = ((cnode->child1->value) != (cnode->child2->value));
            break;
         case e_GT :
            cnode->value = ((cnode->child1->value) > (cnode->child2->value));
            break;
         case e_LT :
            cnode->value = ((cnode->child1->value) < (cnode->child2->value));
            break;
         case e_GTE:
            cnode->value = ((cnode->child1->value) >= (cnode->child2->value));
            break;
         case e_LTE:
            cnode->value = ((cnode->child1->value) <= (cnode->child2->value));
            break;
         case e_AND:
            cnode->value = ((cnode->child1->value) && (cnode->child2->value));
            break;
         case e_OR :
            cnode->value = ((cnode->child1->value) || (cnode->child2->value));
            break;
         default:
            fprintf(mon_output, "Unexpected conditional operator: %d\n",cnode->operation);
            assert(0);
      }
   }
   else
   {
      if (cnode->is_reg)
         cnode->value = get_reg_val(e_comp_space, cnode->reg_num); /* FIXME memspace */
   }

   return cnode->value;
}


void delete_conditional(CONDITIONAL_NODE *cnode)
{
   if (cnode) {
      if (cnode->child1)
         delete_conditional(cnode->child1);
      if (cnode->child2)
         delete_conditional(cnode->child2);
      free(cnode);
   }
}


/* *** BREAKPOINT COMMANDS *** */


void remove_breakpoint_from_list(BREAK_LIST **head, breakpoint *bp)
{
   BREAK_LIST *cur_entry, *prev_entry;

   cur_entry = *head;
   prev_entry = NULL;

   while (cur_entry) {
      if (cur_entry->brkpt == bp)
         break;

      prev_entry = cur_entry;
      cur_entry = cur_entry->next;
   }

   if (!cur_entry) {
      assert(FALSE);
   } else {
     if (!prev_entry) {
        *head = cur_entry->next;
     } else {
         prev_entry->next = cur_entry->next;
     }

     free(cur_entry);
   }
}

breakpoint *find_breakpoint(int brknum)
{
   BREAK_LIST *ptr;
   int i;

   for (i=e_comp_space;i<=e_disk_space;i++) {
      ptr = breakpoints[i];
      while (ptr) {
         if (ptr->brkpt->brknum == brknum)
            return ptr->brkpt;
         ptr = ptr->next;
      }

      ptr = watchpoints_load[i];
      while (ptr) {
         if (ptr->brkpt->brknum == brknum)
            return ptr->brkpt;
         ptr = ptr->next;
      }

      ptr = watchpoints_store[i];
      while (ptr) {
         if (ptr->brkpt->brknum == brknum)
            return ptr->brkpt;
         ptr = ptr->next;
      }
   }

   return NULL;
}

void switch_breakpt(int op, int breakpt_num)
{
   breakpoint *bp;
   bp = find_breakpoint(breakpt_num);

   if (!bp)
   {
      fprintf(mon_output,"#%d not a valid breakpoint\n",breakpt_num);
   }
   else
   {
      bp->enabled = op;
      fprintf(mon_output, "Set breakpoint #%d to state:%s\n",breakpt_num, (op==e_ON)?"enable":"disable");
   }
}

void set_ignore_count(int breakpt_num, int count)
{
   breakpoint *bp;
   bp = find_breakpoint(breakpt_num);

   if (!bp)
   {
      fprintf(mon_output, "#%d not a valid breakpoint\n",breakpt_num);
   }
   else
   {
      bp->ignore_count = count;
      fprintf(mon_output, "Ignoring the next %d crossings of breakpoint #%d\n",count, breakpt_num);
   }
}

void print_breakpt_info(breakpoint *bp)
{
   if (bp->trace) {
      fprintf(mon_output, "TRACE: ");
   } else if (bp->watch_load || bp->watch_store) {
      fprintf(mon_output, "WATCH: ");
   } else {
      fprintf(mon_output, "BREAK: ");
   }
   fprintf(mon_output, "%d A:0x%04x",bp->brknum,addr_range_start_location(bp->range));
   if (is_valid_addr(bp->range->end_addr))
      fprintf(mon_output, "-0x%04x",addr_range_end_location(bp->range));

   if (bp->watch_load)
      fprintf(mon_output, " load");
   if (bp->watch_store)
      fprintf(mon_output, " store");

   fprintf(mon_output, "   %s\n",(bp->enabled==e_ON)?"enabled":"disabled");

   if (bp->condition) {
      fprintf(mon_output, "\tCondition: ");
      print_conditional(bp->condition);
      fprintf(mon_output, "\n");
   }
   if (bp->command)
      fprintf(mon_output, "\tCommand: %s\n",bp->command);
}

void print_breakpts()
{
   int i, any_set=0;
   breakpoint *bp;

   for (i=1;i<breakpoint_count;i++)
   {
      if ( (bp = find_breakpoint(i)) )
      {
         print_breakpt_info(bp);
         any_set = 1;
      }
   }

   if (!any_set)
      fprintf(mon_output, "No breakpoints are set\n");
}

void delete_breakpoint(int brknum)
{
   int i;
   breakpoint *bp = NULL;
   MEMSPACE mem;

   if (brknum == -1)
   {
      /* Add user confirmation here. */
      fprintf(mon_output, "Deleting all breakpoints\n");
      for (i=1;i<breakpoint_count;i++)
      {
         bp = find_breakpoint(i);
         if (bp)
            delete_breakpoint(i);
      }
   }
   else if ( !(bp = find_breakpoint(brknum)) )
   {
      fprintf(mon_output, "#%d not a valid breakpoint\n",brknum);
      return;
   }
   else
   {
      mem = addr_range_start_memspace(bp->range);

      if ( !(bp->watch_load) && !(bp->watch_store) ) {
         remove_breakpoint_from_list(&(breakpoints[mem]), bp);
      } else {
         if ( bp->watch_load )
            remove_breakpoint_from_list(&(watchpoints_load[mem]), bp);
         if ( bp->watch_store )
            remove_breakpoint_from_list(&(watchpoints_store[mem]), bp);
      }
   }

   delete_conditional(bp->condition);
   free_range(bp->range);
   if (bp->command)
      free(bp->command);
}

void set_brkpt_condition(int brk_num, CONDITIONAL_NODE *cnode)
{
   breakpoint *bp;
   bp = find_breakpoint(brk_num);

   if (!bp)
   {
      fprintf(mon_output, "#%d not a valid breakpoint\n",brk_num);
   }
   else
   {
      bp->condition = cnode;

      fprintf(mon_output, "Setting breakpoint %d condition to: ",brk_num);
      print_conditional(cnode);
      fprintf(mon_output, "\n");
#if 0
      /* DEBUG */
      evaluate_conditional(cnode);
      printf("Condition evaluates to: %d\n",cnode->value);
#endif
   }
}


void set_breakpt_command(int brk_num, char *cmd)
{
   breakpoint *bp;
   bp = find_breakpoint(brk_num);

   if (!bp)
   {
      fprintf(mon_output, "#%d not a valid breakpoint\n",brk_num);
   }
   else
   {
      bp->command = cmd;
      fprintf(mon_output, "Setting breakpoint %d command to: %s\n",brk_num, cmd);
   }
}


BREAK_LIST *search_breakpoint_list(BREAK_LIST *head, unsigned loc)
{
   BREAK_LIST *cur_entry;

   cur_entry = head;

   /* The list should be sorted in increasing order. If the current entry
      is > than the search item, we can drop out early.
   */
   while (cur_entry) {
      if (is_in_range(cur_entry->brkpt->range, loc))
         return cur_entry;

      cur_entry = cur_entry->next;
   }

   return NULL;
}

int compare_breakpoints(breakpoint *bp1, breakpoint *bp2)
{
   unsigned addr1, addr2;
   /* Returns < 0 if bp1 < bp2
              = 0 if bp1 = bp2
              > 0 if bp1 > bp2
   */

   addr1 = addr_range_start_location(bp1->range);
   addr2 = addr_range_start_location(bp2->range);

   if ( addr1 < addr2 )
      return -1;

   if ( addr1 > addr2 )
      return 1;

   return 0;
}

bool check_checkpoint(MEMSPACE mem, ADDRESS addr, BREAK_LIST *list)
{
   BREAK_LIST *ptr;
   breakpoint *bp;
   bool result = FALSE;
   M_ADDR temp;
   char *type;

   ptr = search_breakpoint_list(list,addr);

   while (ptr && is_in_range(ptr->brkpt->range,addr)) {
      bp = ptr->brkpt;
      ptr = ptr->next;
      if (bp && bp->enabled==e_ON) {
         /* If condition test fails, skip this checkpoint */
         if (bp->condition) {
            if (!evaluate_conditional(bp->condition)) {
               continue;
            }
         }

         /* Check if the user specified some ignores */
         if (bp->ignore_count) {
            bp->ignore_count--;
            continue;
         }

         bp->hit_count++;

         result = TRUE;

         temp = new_addr(mem, get_reg_val(mem, e_PC));
         if (bp->trace) {
            type = "Trace";
            result = FALSE;
         }
         else if (bp->watch_load)
            type = "Watch-load";
         else if (bp->watch_store)
            type = "Watch-store";
         else
            type = "Break";

         fprintf(mon_output, "#%d (%s) ",bp->brknum, type);
         disassemble_instr(temp);

         if (bp->command) {
            fprintf(mon_output, "Executing: %s\n",bp->command);
            parse_and_execute_line(bp->command);
         }
      }
   }
   return result;
}

void add_to_breakpoint_list(BREAK_LIST **head, breakpoint *bp)
{
   BREAK_LIST *new_entry, *cur_entry, *prev_entry;

   new_entry = (BREAK_LIST *) malloc(sizeof(BREAK_LIST));
   new_entry->brkpt = bp;

   cur_entry = *head;
   prev_entry = NULL;

   /* Make sure the list is in increasing order. (Ranges are entered
      based on the lower bound) This way if the searched for address is
      less than the current ptr, we can skip the rest of the list. Note
      that ranges that wrap around 0xffff aren't handled in this scheme.
      Suggestion: Split the range and create two entries.
   */
   while (cur_entry && (compare_breakpoints(cur_entry->brkpt, bp) <= 0) ) {
      prev_entry = cur_entry;
      cur_entry = cur_entry->next;
   }

   if (!prev_entry) {
      *head = new_entry;
      new_entry->next = cur_entry;
      return;
   }

   prev_entry->next = new_entry;
   new_entry->next = cur_entry;
}

int add_breakpoint(M_ADDR_RANGE range, bool is_trace, bool is_load, bool is_store)
{
   breakpoint *new_bp;
   MEMSPACE mem;

   assert(is_valid_range(range));
   evaluate_default_addr_range(&range,TRUE);

   new_bp = (breakpoint *) malloc(sizeof(breakpoint));

   new_bp->brknum = breakpoint_count++;
   new_bp->range = range;
   new_bp->trace = is_trace;
   new_bp->enabled = e_ON;
   new_bp->hit_count = 0;
   new_bp->ignore_count = 0;
   new_bp->condition = NULL;
   new_bp->command = NULL;
   new_bp->watch_load = is_load;
   new_bp->watch_store = is_store;

   mem = addr_range_start_memspace(range);
   if (!is_load && !is_store)
      add_to_breakpoint_list(&(breakpoints[mem]), new_bp);
   else {
      if (is_load)
         add_to_breakpoint_list(&(watchpoints_load[mem]), new_bp);
      if (is_store)
         add_to_breakpoint_list(&(watchpoints_store[mem]), new_bp);
   }

   print_breakpt_info(new_bp);
   return new_bp->brknum;
}


/* *** WATCHPOINTS *** */


void watch_push_load_addr(ADDRESS addr, MEMSPACE mem)
{
   if (inside_monitor)
      return;

   assert(watch_load_count[mem] < 5);

   watch_load_occurred = TRUE;
   watch_load_array[watch_load_count[mem]][mem] = addr;
   watch_load_count[mem]++;
}

void watch_push_store_addr(ADDRESS addr, MEMSPACE mem)
{
   if (inside_monitor)
      return;

   watch_store_occurred = TRUE;
   watch_store_array[watch_load_count[mem]][mem] = addr;
   watch_store_count[mem]++;
}

bool watchpoints_check_loads(MEMSPACE mem)
{
   bool trap = FALSE;
   unsigned count;

   count = watch_load_count[mem];
   watch_load_count[mem] = 0;
   while (count) {
      count--;
      if (check_watchpoints_load(mem, watch_load_array[count][mem]))
         trap = TRUE;
   }
   return trap;
}

bool watchpoints_check_stores(MEMSPACE mem)
{
   bool trap = FALSE;
   unsigned count;

   count = watch_store_count[mem];
   watch_store_count[mem] = 0;

   while (watch_store_count[mem]) {
      count--;
      if (check_watchpoints_store(mem, watch_store_array[count][mem]))
         trap = TRUE;
   }
   return trap;
}


/* *** CPU INTERFACES *** */


bool mon_force_import(MEMSPACE mem)
{
   bool result;

   result = force_array[mem];
   force_array[mem] = FALSE;

   return result;
}

void mon_check_icount(ADDRESS a)
{
    if (instruction_count) {
        instruction_count--;

        mon_mask[e_comp_space] &= ~MI_STEP;
        monitor_trap_off(true1541_interface->int_status);

        if (!instruction_count) {
            mon(a);
        }
    }
}

void mon_check_watchpoints(ADDRESS a)
{
    if (watch_load_occurred) {
        if (watchpoints_check_loads(e_comp_space)) {
            caller_space = e_comp_space;
            mon(a);
        }
        if (watchpoints_check_loads(e_disk_space)) {
            caller_space = e_disk_space;
            mon(a);
        }
        watch_load_occurred = FALSE;
    }

    if (watch_store_occurred) {
        if (watchpoints_check_stores(e_comp_space)) {
            caller_space = e_comp_space;
            mon(a);
        }
        if (watchpoints_check_stores(e_disk_space)) {
            caller_space = e_disk_space;
            mon(a);
        }
        watch_store_occurred = FALSE;
    }
}

void mon(ADDRESS a)
{
   char prompt[40];
   bool do_trap = FALSE;

   inside_monitor = TRUE;
   suspend_speed_eval();

   dot_addr[caller_space] = new_addr(caller_space, a);

   printf("\n** Monitor\n");

   do {
      sprintf(prompt, "[%c,R:%s,W:%s] (%s:$%04x) ",(sidefx==e_ON)?'S':'-', memspace_string[default_readspace],
              memspace_string[default_writespace], memspace_string[caller_space],
              addr_location(dot_addr[caller_space]));

      if (asm_mode) {
         sprintf(prompt,".%04x  ", addr_location(asm_mode_addr));
      }

      myinput = readline(prompt);
      if (myinput) {
         if (!myinput[0]) {
            if (!asm_mode) {
               /* Repeat previous command */
               free(myinput);

               if (last_cmd)
                  myinput = strdup(last_cmd);
               else
                  myinput = NULL;
            } else {
               /* Leave asm mode */
               sprintf(prompt, "[%c,R:%s,W:%s] (%s:$%04x) ",(sidefx==e_ON)?'S':'-',
                       memspace_string[default_readspace], memspace_string[default_writespace],
                       memspace_string[caller_space], addr_location(dot_addr[caller_space]));
            }
         } else {
             /* Nonempty line */
             add_history(myinput);
         }

         if (myinput) {
             parse_and_execute_line(myinput);
         }
      }
      if (last_cmd)
          free(last_cmd);

      last_cmd = myinput;
   } while (!exit_mon);
   inside_monitor = FALSE;
   suspend_speed_eval();

   if (any_breakpoints(e_comp_space)) {
       mon_mask[e_comp_space] |= MI_BREAK;
       do_trap = TRUE;
   } else {
       mon_mask[e_comp_space] &= ~MI_BREAK;
   }

   if (any_watchpoints(e_comp_space)) {
       do_trap = TRUE;
       mon_mask[e_comp_space] |= MI_WATCH;
       maincpu_interface->toggle_watchpoints_func(1);
   } else {
       mon_mask[e_comp_space] &= ~MI_WATCH;
       maincpu_interface->toggle_watchpoints_func(0);
   }

   if (do_trap)
       monitor_trap_on(maincpu_interface->int_status);
   else
       monitor_trap_off(maincpu_interface->int_status);

   do_trap = FALSE;

   if (any_breakpoints(e_disk_space)) {
       mon_mask[e_disk_space] |= MI_BREAK;
       do_trap = TRUE;
   } else {
       mon_mask[e_disk_space] &= ~MI_BREAK;
   }

   if (any_watchpoints(e_disk_space)) {
       mon_mask[e_disk_space] |= MI_WATCH;
       true1541_interface->toggle_watchpoints_func(1);
       do_trap = TRUE;
   } else {
       mon_mask[e_disk_space] &= ~MI_WATCH;
       true1541_interface->toggle_watchpoints_func(0);
   }

   if (do_trap)
       monitor_trap_on(true1541_interface->int_status);
   else
       monitor_trap_off(true1541_interface->int_status);

   exit_mon = 0;
}
