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
#include "maincpu.h"
#include "1541cpu.h"
#include "file.h"
#include "macro.h"
#include "misc.h"
#include "mshell.h"
#include "mem.h"
#include "interrupt.h"
#include "resources.h"

#ifdef PET
#define NO_DRIVE
void maincpu_turn_watchpoints_on() {}
void maincpu_turn_watchpoints_off() {}
#endif

#ifndef NO_DRIVE

#include "true1541.h"

#define LOAD_1541(a)		  (read_func[(a) >> 10](a))
#define LOAD_ZERO_1541(a)	  (true1541_ram[(a) & 0xff])
#define STORE_1541(a, b)	  (store_func[(a) >> 10]((a), (b)))
#define STORE_ZERO_1541(a, b)  (true1541_ram[(a) & 0xff] = (b))

#endif

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

MEMSPACE caller_space;

ADDRESS watch_load_array[5][NUM_MEMSPACES];
ADDRESS watch_store_array[5][NUM_MEMSPACES];
unsigned watch_load_count[NUM_MEMSPACES];
unsigned watch_store_count[NUM_MEMSPACES];
bool force_array[NUM_MEMSPACES];

M_ADDR dot_addr[NUM_MEMSPACES];
int breakpoint_count;
unsigned char data_buf[256];
unsigned data_buf_len;
int stop_on_start;
bool asm_mode;
M_ADDR asm_mode_addr;
unsigned next_or_step_stop;

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


int default_display_number[] = {40, /* default = hex */
                                40, /* hexadecimal */
                                40, /* decimal */
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

/* 6502 */

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

#if 0
#define M_NONE 0
#define M_LOAD 1
#define M_STORE 2
#define M_LOAD_STORE 3

int memory_ops[] = { /* 0x00 */ M_NONE , M_LOAD , M_NONE , M_BOTH , M_NONE , M_LOAD , M_BOTH , M_BOTH ,
                     /* 0x08 */ M_STORE, M_NONE , M_NONE , M_NONE , M_LOAD , M_LOAD , M_BOTH , M_BOTH ,
                     /* 0x10 */ M_NONE , M_LOAD , M_NONE , M_BOTH , M_NONE , M_LOAD , M_BOTH , M_BOTH ,
                     /* 0x18 */ M_NONE , M_LOAD , M_NONE , M_BOTH , M_LOAD , M_LOAD , M_BOTH , M_BOTH ,
                     /* 0x20 */ M_STORE, M_LOAD , M_NONE , M_BOTH , M_LOAD , M_LOAD , M_BOTH , M_BOTH ,
                     /* 0x28 */ M_LOAD , M_NONE , M_NONE , M_NONE , M_LOAD , M_LOAD , M_BOTH , M_BOTH ,
                     /* 0x20 */ M_NONE , M_LOAD , M_NONE , M_BOTH , M_NONE , M_LOAD , M_BOTH , M_BOTH ,
                     /* 0x38 */ M_NONE , M_LOAD , M_NONE , M_BOTH , M_LOAD , M_LOAD , M_BOTH , M_BOTH ,
                     /* 0x40 */ M_LOAD , M_LOAD , M_NONE , M_BOTH , M_NONE , M_LOAD , M_BOTH , M_BOTH ,
                     /* 0x48 */ M_STORE, M_NONE , M_NONE , M_NONE , M_NONE , M_LOAD , M_BOTH , M_BOTH ,
                     /* 0x50 */ M_NONE , M_LOAD , M_NONE , M_BOTH , M_NONE , M_LOAD , M_BOTH , M_BOTH ,
                     /* 0x58 */ M_NONE , M_LOAD , M_NONE , M_BOTH , M_LOAD , M_LOAD , M_BOTH , M_BOTH ,
                     M_LOAD, , M_NONE, , M_NONE, , , ,  , , , , M_LOAD, , , ,
                     M_NONE, , M_NONE, , M_NONE, , , ,  , , M_NONE, , M_LOAD, , , ,
                     M_NONE, , M_NONE, , , , , ,  , M_NONE, , , , , , ,
                     M_NONE, , M_NONE, , , , , ,  , , , , , , , ,
                     , , , , , , , ,  , , , , , , , ,
                     M_NONE, , M_NONE, , , , , ,  , , , , , , , ,
                     , , M_NONE, , , , , ,  , , , , , , , ,
                     M_NONE, , M_NONE, , M_NONE, , , ,  , , M_NONE, , M_LOAD, , , ,
                     , , M_NONE, , , , , ,  , , , , , , , ,
                     M_NONE, , M_NONE, , M_NONE, , , ,  , , M_NONE, , M_LOAD, , , ,
                   };
#endif

unsigned int get_reg_val(MEMSPACE mem, int reg_id)
{
   if (mem == e_comp_space) {
      switch(reg_id) {
         case e_A:
            return accumulator;
         case e_X:
            return x_register;
         case e_Y:
            return y_register;
         case e_PC:
            return program_counter;
         case e_SP:
            return stack_pointer;
         default:
            assert(FALSE);
      }
   } else if (mem == e_disk_space) {
#ifndef NO_DRIVE
      if (app_resources.true1541) {
         switch(reg_id) {
            case e_A:
               return true1541_accumulator;
            case e_X:
               return true1541_x_register;
            case e_Y:
               return true1541_y_register;
            case e_PC:
               return true1541_program_counter;
            case e_SP:
               return true1541_stack_pointer;
            default:
               assert(FALSE);
         }
      } else {
         puts("True1541 emulation is not turned on.");
      }
#else
      puts("True1541 emulation no supported for this machine.");
#endif
   }
   return 0;
}

unsigned char get_mem_val(MEMSPACE mem, unsigned mem_addr)
{
   if (mem == e_comp_space) {
      if (mem_addr < 0x0100)
         return LOAD_ZERO(mem_addr);
      else
         return LOAD(mem_addr);
   }
   else if (mem == e_disk_space) {
#ifndef NO_DRIVE
      if (app_resources.true1541) {
         if (mem_addr < 0x0100)
            return LOAD_ZERO_1541(mem_addr);
         else
            return LOAD_1541(mem_addr);
      } else {
         puts("True1541 emulation is not turned on.");
      }
#else
      puts("True1541 emulation no supported for this machine.");
#endif
   }
   else
      assert(FALSE);

   return 0;
}

bool mon_force_import(MEMSPACE mem)
{
   bool result;

   result = force_array[mem];
   force_array[mem] = FALSE;

   return result;
}

void set_reg_val(int reg_id, WORD val)
{
   MEMSPACE mem = default_writespace;

   if (mem == e_comp_space) {
      switch(reg_id) {
         case e_A:
            accumulator = val;
            break;
         case e_X:
            x_register = val;
            break;
         case e_Y:
            y_register = val;
            break;
         case e_PC:
            program_counter = val;
            break;
         case e_SP:
            stack_pointer = val;
            break;
         default:
            assert(FALSE);
      }
   } else if (mem == e_disk_space) {
#ifndef NO_DRIVE
      if (app_resources.true1541) {
         switch(reg_id) {
            case e_A:
               true1541_accumulator = val;
               break;
            case e_X:
               true1541_x_register = val;
               break;
            case e_Y:
               true1541_y_register = val;
               break;
            case e_PC:
               true1541_program_counter = val;
               break;
            case e_SP:
               true1541_stack_pointer = val;
               break;
            default:
               assert(FALSE);
         }
      } else {
         puts("True1541 emulation is not turned on.");
      }
#else
      puts("True1541 emulation no supported for this machine.");
#endif
   }
   force_array[mem] = TRUE;
}

void print_registers(MEMSPACE mem)
{
   int i;

   if (mem == e_default_space)
      mem = default_readspace;

   if (mem == e_disk_space) {
#ifndef NO_DRIVE
      if (!app_resources.true1541) {
         puts("True1541 emulation is not turned on.");
         return;
      }
#else
      puts("True1541 emulation no supported for this machine.");
#endif
   }

   for (i=0;i<=e_SP;i++) {
      if (i) printf(",");
      printf(" %s = %x ",register_string[i],get_reg_val(mem,i));
   }
   puts("");
}

void set_mem_val(MEMSPACE mem, unsigned mem_addr, unsigned char val)
{
   if (mem == e_comp_space) {
      if (mem_addr < 0x0100)
         STORE_ZERO(mem_addr,val);
      else
         STORE(mem_addr,val);
   }
   else if (mem == e_disk_space) {
#ifndef NO_DRIVE
      if (app_resources.true1541) {
         if (mem_addr < 0x0100)
            STORE_ZERO_1541(mem_addr,val);
         else
            STORE_1541(mem_addr,val);
      } else {
         puts("True1541 emulation is not turned on.");
      }
#else
      puts("True1541 emulation no supported for this machine.");
#endif
   }
   else
      assert(FALSE);
}


void jump(M_ADDR addr)
{
   /* FIXME - memspace */
   set_reg_val(e_PC, addr_location(addr));
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
M_ADDR evaluate_default_addr(M_ADDR a, bool is_read)
{
   if (addr_memspace(a) != e_default_space)
      return a;

   if (is_read)
      set_addr_memspace(&a,default_readspace);
   else
      set_addr_memspace(&a,default_writespace);

   return a;
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
      printf("Overflow warning\n");
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
      printf("Invalid range: Endpoints are in different"
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
      printf("%c",digit);
      if (divisor == 256)
         printf(" ");
      divisor /= 2;
   }
}

void print_hex(int val)
{
   if (val > 255)
      printf("0x%04x\n",val);
   else
      printf("0x%02x\n",val);
}

void print_octal(int val)
{
   if (val > 511)
      printf("0%06o\n",val);
   else
      printf("0%03o\n",val);
}


void print_convert(int val)
{
   printf("+%d\n",val);
   print_hex(val);
   print_octal(val);
   print_bin(val,'1','0'); puts("");
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

void init_monitor()
{
   int i;

   yydebug = 0;
   sidefx = e_OFF;
   default_datatype = e_hexadecimal;
   default_readspace = e_comp_space;
   default_writespace = e_comp_space;
   instruction_count = 0;
   icount_is_next = FALSE;
   breakpoint_count = 1;
   data_buf_len = 0;
   stop_on_start = 1;
   asm_mode = 0;
   next_or_step_stop = 0;

   watch_load_occurred = FALSE;
   watch_store_occurred = FALSE;

   for (i=1;i<NUM_MEMSPACES;i++) {
      dot_addr[i] = new_addr(e_default_space + i, 0);
      watch_load_count[i] = 0;
      watch_store_count[i] = 0;
   }

   bad_addr = new_addr(e_invalid_space, 0);
   bad_addr_range = new_range(bad_addr, bad_addr);

   caller_space = e_comp_space;

   asm_mode_addr = bad_addr;
}

void print_help() { printf("No help yet.\n"); }


void start_assemble_mode(M_ADDR addr, char *asm_line)
{
   asm_mode = 1;

   assert(is_valid_addr(addr));
   addr = evaluate_default_addr(addr, FALSE);
   asm_mode_addr = addr;

   assemble_line(asm_line);
}

void end_assemble_mode()
{
   asm_mode = 0;
}

extern int interpret_instr(char *line, ADDRESS adr, int mode);

void assemble_line(char *line)
{
   int bump_count = 1;
   MEMSPACE mem;
   unsigned loc;

   if (!line)
      return;

   mem = addr_memspace(asm_mode_addr);
   loc = addr_location(asm_mode_addr);
   /* line[strlen(line)-1] = '\0'; */

   /* printf("Assemble '%s' to address %s:0x%04x\n",line,memspace_string[mem],addr_location(asm_mode_addr)); */
   bump_count = interpret_instr(line, loc, 0); /* FIXME ? MODE */
   if (bump_count >= 0) {
      inc_addr_location(&asm_mode_addr, bump_count);
      dot_addr[mem] = asm_mode_addr;
   } else {
      printf("Assemble error: %d\n",bump_count);
   }
}

unsigned disassemble_instr(M_ADDR addr)
{
   BYTE op, p1, p2;
   MEMSPACE mem;
   unsigned loc;

   mem = addr_memspace(addr);
   loc = addr_location(addr);

   op = get_mem_val(mem, loc);
   p1 = get_mem_val(mem, loc+1);
   p2 = get_mem_val(mem, loc+2);

   printf(".%s:%04x   %s\n",memspace_string[addr_memspace(addr)],loc,
          sprint_disassembled(loc, op, p1, p2, MODE_HEX));  /* FIXME HEX */
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
      printf(">%s:%04x ",memspace_string[mem],addr);
      max_width = default_display_per_line[data_type];
      for (i=0,real_width=0;i<max_width;i++) {
         switch(data_type) {
            case e_text_petscii:
               printf("%c",p_toascii(get_mem_val(mem,addr+i),0));
               real_width++;
               break;
            case e_decimal:
               memset(printables,0,50);
               if (addr+i <= last_addr) {
                  printf("%3d ",get_mem_val(mem,addr+i));
                  real_width++;
               }
               else
                  printf("    ");
               break;
            case e_hexadecimal:
               memset(printables,0,50);
               if (addr+i <= last_addr) {
                  printf("%02x ",get_mem_val(mem,addr+i));
                  real_width++;
               }
               else
                  printf("   ");
               break;
            case e_octal:
               memset(printables,0,50);
               if (addr+i <= last_addr) {
                  printf("%03o ",get_mem_val(mem,addr+i));
                  real_width++;
               }
               else
                  printf("   ");
               break;
            case e_character:
               print_bin(get_mem_val(mem,addr+i),'*','.');
               printf(" \n");
               real_width++;
               break;
            default:

         }

      }

      if (data_type == e_decimal || data_type == e_hexadecimal) {
         memory_to_string(printables, mem, addr, real_width, FALSE);
         printf("\t%s",printables);
      }
      puts("");
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
  evaluate_default_addr(dest,FALSE);
  dst = addr_location(dest);
  len = end - start + 1;
  buf = (BYTE *) malloc(sizeof(BYTE) * len);

  src_mem = addr_range_start_memspace(src);
  dest_mem = addr_memspace(dest);

  if (len < 0) len += 65536;

  /* FIXME: handle overlap */
  for (i=0; i<len; i++) {
     buf[i] = get_mem_val(src_mem, start+i);
  }

  for (i=0; i<len; i++) {
     printf("Moving addr:0x%x to addr:0x%x\n",start+i, dst+i);
     set_mem_val(dest_mem, dst+i, buf[i]);
  }

  free_range(src);
}


void compare_memory(M_ADDR_RANGE src, M_ADDR dest)
{
  unsigned i, len, start, end, dst;
  MEMSPACE src_mem, dest_mem;

  if (is_valid_range(src))
     evaluate_default_addr_range(&src, TRUE);
  else
     assert(FALSE);

  start = addr_range_start_location(src);
  end = addr_range_end_location(src);

  assert(is_valid_addr(dest));
  evaluate_default_addr(dest,TRUE);
  dst = addr_location(dest);
  len = end - start + 1;
  if (len < 0) len += 65536;

  src_mem = addr_range_start_memspace(src);
  dest_mem = addr_memspace(dest);

  /* FIXME: handle overlap & memspaces */
  for (i=0; i<len; i++) {
     printf("Comparing addr:0x%x with addr:0x%x\n",start+i, dst+i);
     if (get_mem_val(dest_mem, dst+i) != get_mem_val(src_mem, start+i))
        printf("0x%x\n",start+i);
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
  while (i < len) {
     set_mem_val(dest_mem, start+i, data_buf[index++]);
     if (index >= strlen(data_buf)) index = 0;
     i++;
  }

  free_range(dest);
}


void hunt_memory(M_ADDR_RANGE dest, unsigned char *data)
{
  unsigned len, start, end;

  if (is_valid_range(dest))
     evaluate_default_addr_range(&dest, TRUE);
  else
     assert(FALSE);

  start = addr_range_start_location(dest);
  end = addr_range_end_location(dest);
  len = end - start + 1;
  if (len < 0) len += 0x10000;

  /* FIXME: handle overlap & memspaces */
#if 0
  for (i=0; i<(len-strlen(data)); i++) {
     if (strncmp(&(memory_vals[start+i]),data,strlen(data)) == 0)
        printf("Found match at addr:0x%x\n",start+i);
  }
#endif

  free_range(dest);
}


/* *** FILE COMMANDS *** */

void change_dir(char *path)
{
    if (chdir(path) < 0) {
	perror(path);
    }

    printf("Changing to directory: %s",path);
}


void mon_load_file(char *filename, M_ADDR start_addr)
{
    FILE   *fp;
    ADDRESS adr;
    int     b1, b2;
    int     ch;

    evaluate_default_addr(start_addr, FALSE);

    if (NULL == (fp = fopen(filename, READ))) {
	perror(filename);
	printf("Loading failed.\n");
	return;
    }

    b1 = fgetc(fp);
    b2 = fgetc(fp);

    if (!is_valid_addr(start_addr)) {	/* No Load address given */
	if (b1 == 1)	/* Load Basic */
	    mem_get_basic_text(&adr, NULL);
	else
	    adr = LOHI ((BYTE)b1,(BYTE)b2);
    } else  {
       adr = addr_location(start_addr);
    }

    printf("Loading %s", filename);
    printf(" from %04X\n", adr);

    ch = fread (ram + adr, 1, ram_size - adr, fp);
    printf ("%x bytes\n", ch);

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
	printf("Saving failed.\n");
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
   evaluate_default_addr(start_addr, TRUE);

   printf("Verify file %s at address 0x%04x\n", filename, addr_location(start_addr));
}


/* *** INSTRUCTION COMMANDS *** */


void instructions_step(int count)
{
   printf("Stepping through the next %d instruction(s).\n",
          (count>=0)?count:1);
   instruction_count = (count>=0)?count:1;
   icount_is_next = FALSE;
   exit_mon = 1;
   maincpu_trigger_trap(mon_helper);
}

void instructions_next(int count)
{
   printf("Nexting through the next %d instruction(s).\n",
          (count>=0)?count:1);
   instruction_count = (count>=0)?count:1;
   icount_is_next = TRUE;
   exit_mon = 1;
   maincpu_trigger_trap(mon_helper);
}

void stack_up(int count)
{
   printf("Going up %d stack frame(s).\n",
          (count>=0)?count:1);
}

void stack_down(int count)
{
   printf("Going down %d stack frame(s).\n",
          (count>=0)?count:1);
}


/* *** DISK COMMANDS *** */


void block_cmd(int op, int track, int sector, M_ADDR addr)
{
   assert(is_valid_addr(addr));
   evaluate_default_addr(addr, op == 0);

   if (!op)
   {
      if (is_valid_addr(addr))
         printf("Read track %d sector %d to screen\n", track, sector);
      else
         printf("Read track %d sector %d into address 0x%04x\n", track, sector, addr_location(addr));
   }
   else
   {
      printf("Write data from address 0x%04x to track %d sector %d\n", addr_location(addr), track, sector);
   }

}


/* *** BREAKPOINT COMMANDS *** */


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
      printf("#%d not a valid breakpoint\n",breakpt_num);
   }
   else
   {
      bp->enabled = op;
      printf("Set breakpoint #%d to state:%s\n",breakpt_num, (op==e_ON)?"enable":"disable");
   }
}

void set_ignore_count(int breakpt_num, int count)
{
   breakpoint *bp;
   bp = find_breakpoint(breakpt_num);

   if (!bp)
   {
      printf("#%d not a valid breakpoint\n",breakpt_num);
   }
   else
   {
      bp->ignore_count = count;
      printf("Ignoring the next %d crossings of breakpoint #%d\n",count, breakpt_num);
   }
}

void print_breakpt_info(breakpoint *bp)
{
   if (bp->trace) {
      printf("TRACE: ");
   } else if (bp->watch_load || bp->watch_store) {
      printf("WATCH: ");
   } else {
      printf("BREAK: ");
   }
   printf("%d A:0x%04x",bp->brknum,addr_range_start_location(bp->range));
   if (is_valid_addr(bp->range->end_addr))
      printf("-0x%04x",addr_range_end_location(bp->range));

   if (bp->watch_load)
      printf(" load");
   if (bp->watch_store)
      printf(" store");

   printf("   %s\n",(bp->enabled==e_ON)?"enabled":"disabled");

   if (bp->condition) {
      printf("\tCondition: ");
      print_conditional(bp->condition);
      puts("");
   }
   if (bp->command)
      printf("\tCommand: %s\n",bp->command);
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
      printf("No breakpoints are set\n");
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

void delete_breakpoint(int brknum)
{
   int i;
   breakpoint *bp = NULL;
   MEMSPACE mem;

   if (brknum == -1)
   {
      /* Add user confirmation here. */
      puts("Deleting all breakpoints");
      for (i=1;i<breakpoint_count;i++)
      {
         bp = find_breakpoint(i);
         if (bp)
            delete_breakpoint(i);
      }
   }
   else if ( !(bp = find_breakpoint(brknum)) )
   {
      printf("#%d not a valid breakpoint\n",brknum);
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

void print_conditional(CONDITIONAL_NODE *cnode)
{
   /* Do an in-order traversal of the tree */
   if (cnode->operation != e_INV)
   {
      assert(cnode->child1 && cnode->child2);
      print_conditional(cnode->child1);
      printf(" %s ",cond_op_string[cnode->operation]);
      print_conditional(cnode->child2);
   }
   else
   {
      if (cnode->is_reg)
         printf("%s",register_string[cnode->reg_num]);
      else
         printf("%d",cnode->value);
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
            printf("Unexpected conditional operator: %d\n",cnode->operation);
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


void set_brkpt_condition(int brk_num, CONDITIONAL_NODE *cnode)
{
   breakpoint *bp;
   bp = find_breakpoint(brk_num);

   if (!bp)
   {
      printf("#%d not a valid breakpoint\n",brk_num);
   }
   else
   {
      bp->condition = cnode;

      printf("Setting breakpoint %d condition to: ",brk_num);
      print_conditional(cnode);
      puts("");
#if 0
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
      printf("#%d not a valid breakpoint\n",brk_num);
   }
   else
   {
      bp->command = cmd;
      printf("Setting breakpoint %d command to: %s\n",brk_num, cmd);
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


bool check_watchpoints_load(MEMSPACE mem, unsigned eff_addr)
{
   BREAK_LIST *ptr;
   bool result = FALSE;

   ptr = search_breakpoint_list(watchpoints_load[mem],eff_addr);

   while (ptr && is_in_range(ptr->brkpt->range, eff_addr)) {
      printf("WATCH-LOAD(%d) 0x%04x\n",ptr->brkpt->brknum,eff_addr);
      result = TRUE;
      ptr = ptr->next;
   }

   return result;
}

bool check_watchpoints_store(MEMSPACE mem, unsigned eff_addr)
{
   BREAK_LIST *ptr;
   bool result = FALSE;

   ptr = search_breakpoint_list(watchpoints_store[mem],eff_addr);

   while (ptr && is_in_range(ptr->brkpt->range, eff_addr)) {
      printf("WATCH-STORE(%d) 0x%04x\n",ptr->brkpt->brknum,eff_addr);
      result = TRUE;
      ptr = ptr->next;
   }

   return result;
}

bool check_breakpoints(MEMSPACE mem)
{
   BREAK_LIST *ptr;
   breakpoint *bp;
   bool result = FALSE;
   M_ADDR temp;

   ptr = search_breakpoint_list(breakpoints[mem],get_reg_val(mem,e_PC));

   while (ptr && is_in_range(ptr->brkpt->range,get_reg_val(mem,e_PC))) {
      bp = ptr->brkpt;
      if (bp && bp->enabled==e_ON) {
         if (bp->trace) {
            /* Check if PC is in trace range */
            if (is_in_range(bp->range, get_reg_val(mem,e_PC))) {
               temp = new_addr(mem, get_reg_val(mem,e_PC));
               printf("TRACE:(%d) ",bp->brknum);
               disassemble_instr(temp);
            }
         } else {
            if (get_reg_val(mem,e_PC) == (addr_range_start_location(bp->range))) {
               bp->hit_count++;

               if (bp->condition) {
                  if (!evaluate_conditional(bp->condition)) {
                     result = TRUE;
                  }
               }

               if (bp->ignore_count) {
                  bp->ignore_count--;
                  result = TRUE;
               }

               printf("BREAK(%d) 0x%04x\n",bp->brknum,get_reg_val(mem,e_PC));
               if (bp->command) {
                  printf("Executing: %s\n",bp->command);
                  parse_and_execute_line(bp->command);
               }
               result = TRUE;
            }
         }
      }
      ptr = ptr->next;
   }
   return result;
}


bool check_stop_status(MEMSPACE mem, bool op_is_load, bool op_is_store, unsigned eff_addr)
{
   bool ret_val = FALSE;

   if (stop_on_start)
   {
      stop_on_start = 0;
      return TRUE;
   }

   ret_val |= check_breakpoints(mem);

   if (op_is_load) {
      ret_val |= check_watchpoints_load(mem, eff_addr);
   }
   if (op_is_store) {
      ret_val |= check_watchpoints_store(mem, eff_addr);
   }

   ret_val |= (next_or_step_stop!=0);
   next_or_step_stop = 0;
   return ret_val;
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

void check_maincpu_breakpoints(ADDRESS addr)
{
   if (check_breakpoints(e_comp_space))
      mon(program_counter);

   maincpu_trigger_trap(check_maincpu_breakpoints);
}

void check_true1541_breakpoints(ADDRESS addr)
{
#ifndef NO_DRIVE
   if (check_breakpoints(e_disk_space))
      mon(true1541_program_counter);

   true1541_trigger_trap(check_true1541_breakpoints);
#endif
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

void watch_push_load_addr(ADDRESS addr, MEMSPACE mem)
{
   if (inside_monitor)
      return;

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

   while (watch_load_count[mem]) {
      watch_load_count[mem]--;
      count = watch_load_count[mem];
      if (check_watchpoints_load(mem, watch_load_array[count][mem]))
         trap = TRUE;
   }
   return trap;
}

bool watchpoints_check_stores(MEMSPACE mem)
{
   bool trap = FALSE;
   unsigned count;

   while (watch_store_count[mem]) {
      watch_store_count[mem]--;
      count = watch_store_count[mem];
      if (check_watchpoints_store(mem, watch_store_array[count][mem]))
         trap = TRUE;
   }
   return trap;
}

#if 0
bool is_breakpt_a_range(breakpoint *bp)
{
   if (bp->end_addr)
      return TRUE;

   return FALSE;
}

int check_breakpt_range_subsets(breakpoint *bp1, breakpoint *bp2)
{
  /* Returns < 0 if bp1 is a subset of bp2
             = 0 if bp1 and bp2 and exclusive
             > 0 if bp2 is a subset of bp1
   */

   unsigned start1, end1, start2, end2;

   assert(is_breakpt_a_range(bp1));
   assert(is_breakpt_a_range(bp2));


   start1 = bp1->start_addr->location;
   end1 = bp1->end_addr->location;
   start2 = bp2->start_addr->location;
   end2 = bp2->end_addr->location;

   if ( (start1 >= start2) && (end1 <= end2) )
      return -1;

   if ( (start2 >= start1) && (end2 <= end1) )
      return 1;

   return 0;
}

int check_breakpt_range_overlaps(breakpoint *bp1, breakpoint *bp2)
{
  /* Returns < 0 if upper bp1 overlaps lower bp2
             = 0 if bp1 and bp2 and exclusive
             > 0 if upper bp2 overlaps lower bp1
   */

   unsigned start1, end1, start2, end2;

   assert(is_breakpt_a_range(bp1));
   assert(is_breakpt_a_range(bp2));

   start1 = bp1->start_addr->location;
   end1 = bp1->end_addr->location;
   start2 = bp2->start_addr->location;
   end2 = bp2->end_addr->location;

   if ( end1 >= start2 )
      return -1;

   if ( end2 >= start1 )
      return 1;

   return 0;
}

int check_breakpt_in_range(breakpoint *bp_point, breakpoint *bp_range)
{
  /* Returns < 0 if point < range
             = 0 if point is in range
             > 0 if point > range
   */
   unsigned addr, start, end;

   assert(!is_breakpt_a_range(bp_point));
   assert(is_breakpt_a_range(bp_range));

   addr = bp_point->start_addr->location;
   start = bp_range->start_addr->location;
   end = bp_range->end_addr->location;

   if ( addr < start )
      return -1;

   if ( addr > end )
      return 1;

   return 0;
}
#endif

void mon_helper(ADDRESS a)
{
    if (watch_load_occurred) {
       if (watchpoints_check_loads(e_comp_space)) {
          caller_space = e_comp_space;
          mon(a);
       }
#ifndef NO_DRIVE
       if (watchpoints_check_loads(e_disk_space)) {
          caller_space = e_disk_space;
          mon(a);
       }
#endif
       watch_load_occurred = FALSE;
    }

    if (watch_store_occurred) {
       if (watchpoints_check_stores(e_comp_space)) {
          caller_space = e_comp_space;
          mon(a);
       }
#ifndef NO_DRIVE
       if (watchpoints_check_stores(e_disk_space)) {
          caller_space = e_disk_space;
          mon(a);
       }
#endif
       watch_store_occurred = FALSE;
    }

    if (instruction_count) {
       instruction_count--;
       if (!instruction_count) {
          mon(a);
       }
    }

    if (any_watchpoints(e_comp_space))
       maincpu_trigger_trap(mon_helper);
        
#ifndef NO_DRIVE
    if (any_watchpoints(e_disk_space))
       true1541_trigger_trap(mon_helper);
#endif
        
}

void mon(ADDRESS a)
{
   char prompt[40];

   inside_monitor = TRUE;
   dot_addr[caller_space] = new_addr(caller_space, a);

   do {
      sprintf(prompt, "[%c,R:%s,W:%s] (%s:$%x) ",(sidefx==e_ON)?'S':'-', memspace_string[default_readspace],
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
               sprintf(prompt, "[%c,R:%s,W:%s] ",(sidefx==e_ON)?'S':'-', memspace_string[default_readspace],
                               memspace_string[default_writespace]);
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

   if (any_breakpoints(e_comp_space))
      maincpu_breakpoints_on();
   else
      maincpu_breakpoints_off();
    
   if (any_watchpoints(e_comp_space)) {
      maincpu_turn_watchpoints_on();
      maincpu_trigger_trap(mon_helper);
   }
   else
      maincpu_turn_watchpoints_off();
    
#ifndef NO_DRIVE
   if (any_breakpoints(e_disk_space))
      true1541_breakpoints_on();
   else
      true1541_breakpoints_off();

   if (any_watchpoints(e_disk_space)) {
      true1541_turn_watchpoints_on();
      true1541_trigger_trap(mon_helper);
   }
   else
      true1541_turn_watchpoints_off();
#endif
    
   exit_mon = 0;
}
