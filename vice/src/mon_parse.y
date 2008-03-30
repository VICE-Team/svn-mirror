/* -*- C -*-
 *
 * mon_parse.y - Parser for the VICE built-in monitor.
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

%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vice.h"
#include "types.h"
#include "asm.h"
#include "utils.h"
#include "mon.h"

#define join_ints(x,y) (LO16_TO_HI16(x)|y)
#define separate_int1(x) (HI16_TO_LO16(x))
#define separate_int2(x) (LO16(x))

static int yyerror(char *s);
static int temp;

/* Defined in the lexer */
extern int new_cmd, opt_asm;
extern void free_buffer(void);
extern void make_buffer(char *str);
extern int yylex(void);
extern int cur_len, last_len;

#define ERR_ILLEGAL_INPUT 1     /* Generic error as returned by yacc.  */
#define ERR_RANGE_BAD_START 2
#define ERR_RANGE_BAD_END 3
#define ERR_BAD_CMD 4
#define ERR_EXPECT_BRKNUM 5
#define ERR_EXPECT_END_CMD 6
#define ERR_MISSING_CLOSE_PAREN 7
#define ERR_INCOMPLETE_COMPARE_OP 8
#define ERR_EXPECT_FILENAME 9
#define ERR_ADDR_TOO_BIG 10
#define ERR_IMM_TOO_BIG 11

#define BAD_ADDR (new_addr(e_invalid_space, 0))
#define CHECK_ADDR(x) ((x) == LO16(x))

%}

%union {
	MON_ADDR a;
        int i;
        REG_ID reg;
        CONDITIONAL cond_op;
        CONDITIONAL_NODE *cond_node;
        RADIXTYPE rt;
        ACTION action;
        char *str;
}

%token<i> H_NUMBER D_NUMBER O_NUMBER B_NUMBER CONVERT_OP B_DATA
%token<i> TRAIL BAD_CMD MEM_OP IF MEM_COMP MEM_DISK CMD_SEP REG_ASGN_SEP EQUALS
%token<i> CMD_SIDEFX CMD_RETURN CMD_BLOCK_READ CMD_BLOCK_WRITE CMD_UP CMD_DOWN
%token<i> CMD_LOAD CMD_SAVE CMD_VERIFY CMD_IGNORE CMD_HUNT CMD_FILL CMD_MOVE
%token<i> CMD_GOTO CMD_REGISTERS CMD_READSPACE CMD_WRITESPACE CMD_RADIX
%token<i> CMD_MEM_DISPLAY CMD_BREAK CMD_TRACE CMD_IO CMD_BRMON CMD_COMPARE
%token<i> CMD_DUMP CMD_UNDUMP CMD_EXIT CMD_DELETE CMD_CONDITION CMD_COMMAND
%token<i> CMD_ASSEMBLE CMD_DISASSEMBLE CMD_NEXT CMD_STEP CMD_PRINT CMD_DEVICE
%token<i> CMD_HELP CMD_WATCH CMD_DISK CMD_SYSTEM CMD_QUIT CMD_CHDIR CMD_BANK
%token<i> CMD_LOAD_LABELS CMD_SAVE_LABELS CMD_ADD_LABEL CMD_DEL_LABEL CMD_SHOW_LABELS
%token<i> CMD_RECORD CMD_STOP CMD_PLAYBACK CMD_CHAR_DISPLAY CMD_SPRITE_DISPLAY
%token<i> CMD_TEXT_DISPLAY CMD_ENTER_DATA CMD_ENTER_BIN_DATA
%token<i> L_PAREN R_PAREN ARG_IMMEDIATE REG_A REG_X REG_Y COMMA INST_SEP
%token<str> STRING FILENAME R_O_L OPCODE LABEL BANKNAME
%token<reg> REGISTER
%left<cond_op> COMPARE_OP
%token<rt> RADIX_TYPE INPUT_SPEC
%token<action> CMD_CHECKPT_ONOFF TOGGLE

%type<a> address opt_address
%type<cond_node> cond_expr compare_operand
%type<i> command number expression
%type<i> memspace memloc memaddr breakpt_num opt_mem_op
%type<i> register_mod opt_count command_list top_level value
%type<i> asm_operand_mode assembly_instruction end_cmd register
%type<i> assembly_instr_list post_assemble opt_memspace
%type<str> rest_of_line data_list data_element filename opt_bankname

%type<i> symbol_table_rules asm_rules memory_rules checkpoint_rules
%type<i> checkpoint_control_rules monitor_state_rules
%type<i> monitor_misc_rules disk_rules cmd_file_rules
%type<i> machine_state_rules data_entry_rules

%left '+' '-'
%left '*' '/'

%%

top_level: command_list { $$ = 0; }
         | assembly_instruction TRAIL { $$ = 0; }
         | TRAIL { new_cmd = 1; asm_mode = 0;  $$ = 0; }
         ;

command_list: command
            | command_list command
            ;

end_cmd: CMD_SEP
       | TRAIL
       | error { return ERR_EXPECT_END_CMD; }
       ;

command: machine_state_rules
       | symbol_table_rules
       | memory_rules
       | asm_rules
       | checkpoint_rules
       | checkpoint_control_rules
       | monitor_state_rules
       | monitor_misc_rules
       | disk_rules
       | cmd_file_rules
       | data_entry_rules
       | BAD_CMD { return ERR_BAD_CMD; }
       ;

machine_state_rules: CMD_BANK opt_memspace opt_bankname end_cmd { mon_bank($2,$3); }
                   | CMD_GOTO address end_cmd { mon_jump($2); }
                   | CMD_IO end_cmd { fprintf(mon_output, "Display IO registers\n"); }
                   | CMD_RETURN end_cmd { mon_instruction_return(); }
                   | CMD_DUMP end_cmd { puts("Dump machine state."); }
                   | CMD_UNDUMP end_cmd { puts("Undump machine state."); }
                   | CMD_STEP opt_count end_cmd { mon_instructions_step($2); }
                   | CMD_NEXT opt_count end_cmd { mon_instructions_next($2); }
                   | CMD_UP opt_count end_cmd { mon_stack_up($2); }
                   | CMD_DOWN opt_count end_cmd { mon_stack_down($2); }
                   | register_mod
                   ;

register_mod: CMD_REGISTERS end_cmd		{ mon_print_registers(default_memspace); }
            | CMD_REGISTERS memspace end_cmd    { mon_print_registers($2); }
            | CMD_REGISTERS reg_list end_cmd
            ;

symbol_table_rules: CMD_LOAD_LABELS opt_memspace filename end_cmd 	{ mon_load_symbols($2, $3); }
                  | CMD_SAVE_LABELS opt_memspace filename end_cmd 	{ mon_save_symbols($2, $3); }
                  | CMD_ADD_LABEL address LABEL end_cmd 		{ mon_add_name_to_symbol_table($2, $3); }
                  | CMD_DEL_LABEL opt_memspace LABEL end_cmd 		{ mon_remove_name_from_symbol_table($2, $3); }
                  | CMD_SHOW_LABELS opt_memspace end_cmd 		{ mon_print_symbol_table($2); }
                  ;

asm_rules: CMD_ASSEMBLE address { mon_start_assemble_mode($2, NULL); } post_assemble end_cmd
         | CMD_ASSEMBLE address end_cmd 		{ mon_start_assemble_mode($2, NULL); }
         | CMD_DISASSEMBLE address opt_address end_cmd  { mon_disassemble_lines($2,$3); }
         | CMD_DISASSEMBLE end_cmd 			{ mon_disassemble_lines(BAD_ADDR, BAD_ADDR); }
         ;

memory_rules: CMD_MOVE address address address end_cmd 		  { mon_move_memory($2, $3, $4); }
            | CMD_COMPARE address address address end_cmd 	  { mon_compare_memory($2, $3, $4); }
            | CMD_FILL address address data_list end_cmd 	  { mon_fill_memory($2, $3, $4); }
            | CMD_HUNT address address data_list end_cmd 	  { mon_hunt_memory($2, $3, $4); }
            | CMD_MEM_DISPLAY RADIX_TYPE address opt_address end_cmd { mon_display_memory($2, $3, $4); }
            | CMD_MEM_DISPLAY address opt_address end_cmd 	  { mon_display_memory(default_radix, $2, $3); }
            | CMD_MEM_DISPLAY end_cmd 				  { mon_display_memory(default_radix, BAD_ADDR, BAD_ADDR); }
            | CMD_CHAR_DISPLAY address opt_address end_cmd 	  { mon_display_data($2, $3, 8, 8); }
            | CMD_CHAR_DISPLAY end_cmd				  { mon_display_data(BAD_ADDR, BAD_ADDR, 8, 8); }
            | CMD_SPRITE_DISPLAY address opt_address end_cmd 	  { mon_display_data($2, $3, 24, 21); }
            | CMD_SPRITE_DISPLAY end_cmd			  { mon_display_data(BAD_ADDR, BAD_ADDR, 24, 21); }
            | CMD_TEXT_DISPLAY address opt_address end_cmd 	  { mon_display_memory(0, $2, $3); }
            | CMD_TEXT_DISPLAY end_cmd 				  { mon_display_memory(0, BAD_ADDR, BAD_ADDR); }
            ;

checkpoint_rules: CMD_BREAK address opt_address end_cmd { mon_add_checkpoint($2, $3, FALSE, FALSE, FALSE); }
                | CMD_BREAK address opt_address IF cond_expr end_cmd {
                          temp = mon_add_checkpoint($2, $3, FALSE, FALSE, FALSE);
                          mon_set_checkpoint_condition(temp, $5); }
                | CMD_WATCH opt_mem_op address opt_address end_cmd { mon_add_checkpoint($3, $4, FALSE,
                              ($2 == e_load || $2 == e_load_store), ($2 == e_store || $2 == e_load_store)); }
                | CMD_TRACE address opt_address end_cmd { mon_add_checkpoint($2, $3, TRUE, FALSE, FALSE); }
                | CMD_BREAK end_cmd { mon_print_checkpoints(); }
                | CMD_TRACE end_cmd { mon_print_checkpoints(); }
                | CMD_WATCH end_cmd { mon_print_checkpoints(); }
                ;


checkpoint_control_rules: CMD_CHECKPT_ONOFF breakpt_num end_cmd 	 { mon_switch_checkpoint($1, $2); }
                        | CMD_IGNORE breakpt_num opt_count end_cmd 	 { mon_set_ignore_count($2, $3); }
                        | CMD_DELETE breakpt_num end_cmd 		 { mon_delete_checkpoint($2); }
                        | CMD_DELETE end_cmd				 { mon_delete_checkpoint(-1); }
                        | CMD_CONDITION breakpt_num IF cond_expr end_cmd { mon_set_checkpoint_condition($2, $4); }
                        | CMD_COMMAND breakpt_num STRING end_cmd 	 { mon_set_checkpoint_command($2, $3); }
                        ;

monitor_state_rules: CMD_SIDEFX TOGGLE end_cmd 	       { sidefx = (($2==e_TOGGLE)?(sidefx^1):$2); }
                   | CMD_SIDEFX end_cmd 	       { fprintf(mon_output, "I/O side effects are %s\n",sidefx ? "enabled" : "disabled"); }
                   | CMD_RADIX RADIX_TYPE end_cmd      { default_radix = $2; }
                   | CMD_RADIX end_cmd
                     {
                         const char *p;

                         if (default_radix == e_hexadecimal)
                             p = "Hexadecimal";
                         else if (default_radix == e_decimal)
                             p = "Decimal";
                         else if (default_radix == e_octal)
                             p = "Octal";
                         else if (default_radix == e_binary)
                             p = "Binary";
                         else
                             p = "Unknown";

                         fprintf(mon_output, "Default radix is %s\n", p);
                     }

                   | CMD_DEVICE memspace end_cmd       { fprintf(mon_output,"Setting default device to `%s'\n",
                                                         _mon_space_strings[(int) $2]); default_memspace = $2; }
                   | CMD_QUIT end_cmd 		       { exit_mon = 2; YYACCEPT; }
                   | CMD_EXIT end_cmd 		       { exit_mon = 1; YYACCEPT; }
                   ;

monitor_misc_rules: CMD_DISK rest_of_line end_cmd 	{ mon_execute_disk_command($2); }
                  | CMD_PRINT expression end_cmd 	{ fprintf(mon_output, "\t%d\n",$2); }
                  | CMD_HELP end_cmd 			{ mon_print_help(NULL); }
                  | CMD_HELP rest_of_line end_cmd 	{ mon_print_help($2); }
                  | CMD_SYSTEM rest_of_line end_cmd 	{ printf("SYSTEM COMMAND: %s\n",$2); }
                  | CONVERT_OP expression end_cmd 	{ mon_print_convert($2); }
                  | CMD_CHDIR rest_of_line end_cmd 	{ mon_change_dir($2); }
                  ;

disk_rules: CMD_LOAD filename address end_cmd 			{ mon_load_file($2,$3); }
          | CMD_SAVE filename address address end_cmd 		{ mon_save_file($2,$3,$4); }
          | CMD_VERIFY filename address end_cmd 		{ mon_verify_file($2,$3); }
          | CMD_BLOCK_READ expression expression opt_address end_cmd	{ mon_block_cmd(0,$2,$3,$4); }
          | CMD_BLOCK_WRITE expression expression address end_cmd	{ mon_block_cmd(1,$2,$3,$4); }
          ;

cmd_file_rules: CMD_RECORD filename end_cmd 	{ mon_record_commands($2); }
              | CMD_STOP end_cmd 		{ mon_end_recording(); }
              | CMD_PLAYBACK filename end_cmd 	{ playback=TRUE; playback_name = $2; }
              ;

data_entry_rules: CMD_ENTER_DATA address data_list end_cmd { mon_fill_memory($2, BAD_ADDR, $3); }
                | CMD_ENTER_BIN_DATA end_cmd { printf("Not yet.\n"); }
                ;

rest_of_line: R_O_L { $$ = $1; }
            ;

opt_bankname: BANKNAME
	| { $$ = NULL; }
	;

filename: FILENAME
        | error { return ERR_EXPECT_FILENAME; }
        ;

opt_mem_op: MEM_OP { $$ = $1; }
          | { $$ = e_load_store; }
          ;

register: REGISTER          { $$ = new_reg(default_memspace, $1); }
        | memspace REGISTER { $$ = new_reg($1, $2); }
        ;

reg_list: reg_list REG_ASGN_SEP reg_asgn
        | reg_asgn
        ;

reg_asgn: register EQUALS number { mon_set_reg_val(reg_memspace($1), reg_regid($1), $3); }
        ;

opt_count: expression { $$ = $1; }
         | { $$ = -1; }
         ;

breakpt_num: number { $$ = $1; }
           | error { return ERR_EXPECT_BRKNUM; }
           ;

opt_address: address { $$ = $1; }
           |         { $$ = BAD_ADDR; }
           ;

address: memloc { $$ = new_addr(e_default_space,$1); if (opt_asm) new_cmd = asm_mode = 1; }
       | memspace memloc { $$ = new_addr($1,$2); if (opt_asm) new_cmd = asm_mode = 1; }
       ;

opt_memspace: memspace { $$ = $1; }
            |          { $$ = e_default_space; }
            ;

memspace: MEM_COMP { $$ = e_comp_space; }
        | MEM_DISK { $$ = e_disk_space; }
        ;

memloc: memaddr { $$ = $1; if (!CHECK_ADDR($1)) return ERR_ADDR_TOO_BIG; }
      ;

memaddr: number { $$ = $1; }

expression: expression '+' expression { $$ = $1 + $3; }
          | expression '-' expression { $$ = $1 - $3; }
          | expression '*' expression { $$ = $1 * $3; }
          | expression '/' expression { $$ = ($3) ? ($1 / $3) : 1; }
          | '(' expression ')' 	      { $$ = $2; }
          | '(' expression error      { return ERR_MISSING_CLOSE_PAREN; }
          | value  		      { $$ = $1; }
          ;

cond_expr: cond_expr COMPARE_OP cond_expr { $$ = new_cond; $$->is_parenthized = FALSE;
                                            $$->child1 = $1; $$->child2 = $3; $$->operation = $2; }
         | cond_expr COMPARE_OP error 	  { return ERR_INCOMPLETE_COMPARE_OP; }
         | L_PAREN cond_expr R_PAREN 	  { $$ = $2; $$->is_parenthized = TRUE; }
         | L_PAREN cond_expr error 	  { return ERR_MISSING_CLOSE_PAREN; }
         | compare_operand 		  { $$ = $1; }
         ;

compare_operand: register { $$ = new_cond; $$->operation = e_INV; $$->is_parenthized = FALSE;
                            $$->reg_num = $1; $$->is_reg = TRUE; }
               | number   { $$ = new_cond; $$->operation = e_INV; $$->is_parenthized = FALSE;
                            $$->value = $1; $$->is_reg = FALSE; }
               ;

data_list: data_list data_element
         | data_element
         ;

data_element: number { mon_add_number_to_buffer($1); }
            | STRING { mon_add_string_to_buffer($1); }
            ;

value: number { $$ = $1; }
     | register { $$ = mon_get_reg_val(reg_memspace($1), reg_regid($1)); }
     ;

number: H_NUMBER { $$ = $1; }
      | D_NUMBER { $$ = $1; }
      | O_NUMBER { $$ = $1; }
      | B_NUMBER { $$ = $1; }
      ;

assembly_instr_list: assembly_instr_list INST_SEP assembly_instruction
                   | assembly_instruction INST_SEP assembly_instruction
                   ;

assembly_instruction: OPCODE asm_operand_mode { $$ = 0;
                                                if ($1) {
                                                    mon_assemble_instr($1, $2);
                                                } else {
                                                    new_cmd = 1;
                                                    asm_mode = 0;
                                                }
                                                opt_asm = 0;
                                              }

post_assemble: assembly_instruction
             | assembly_instr_list { asm_mode = 0; }
             ;

asm_operand_mode: ARG_IMMEDIATE number { if ($2 > 0xff) return ERR_IMM_TOO_BIG;
                                         $$ = join_ints(IMMEDIATE,$2); }
                | number { if ($1 < 0x100)
                              $$ = join_ints(ZERO_PAGE,$1);
                           else
                              $$ = join_ints(ABSOLUTE,$1);
                         }
                | number COMMA REG_X  { if ($1 < 0x100)
                                           $$ = join_ints(ZERO_PAGE_X,$1);
                                        else
                                           $$ = join_ints(ABSOLUTE_X,$1);
                                      }
                | number COMMA REG_Y  { if ($1 < 0x100)
                                           $$ = join_ints(ZERO_PAGE_Y,$1);
                                        else
                                           $$ = join_ints(ABSOLUTE_Y,$1);
                                      }
                | L_PAREN number R_PAREN  { $$ = join_ints(ABS_INDIRECT,$2); }
                | L_PAREN number COMMA REG_X R_PAREN { $$ = join_ints(INDIRECT_X,$2); }
                | L_PAREN number R_PAREN COMMA REG_Y { $$ = join_ints(INDIRECT_Y,$2); }
                | { $$ = join_ints(IMPLIED,0); }
                | REG_A { $$ = join_ints(ACCUMULATOR,0); }
                ;


%%

void parse_and_execute_line(char *input)
{
   char *temp_buf;
   int i, rc;

   temp_buf = (char *) malloc(strlen(input)+3);
   strcpy(temp_buf,input);
   i = strlen(input);
   temp_buf[i++] = '\n';
   temp_buf[i++] = '\0';
   temp_buf[i++] = '\0';

   make_buffer(temp_buf);
   if ( (rc =yyparse()) != 0) {
       fprintf(mon_output, "ERROR -- ");
       switch(rc) {
           case ERR_BAD_CMD:
               fprintf(mon_output, "Bad command:\n");
               break;
           case ERR_RANGE_BAD_START:
               fprintf(mon_output, "Bad first address in range:\n");
               break;
           case ERR_RANGE_BAD_END:
               fprintf(mon_output, "Bad second address in range:\n");
               break;
           case ERR_EXPECT_BRKNUM:
               fprintf(mon_output, "Checkpoint number expected:\n");
               break;
           case ERR_EXPECT_END_CMD:
               fprintf(mon_output, "Unexpected token:\n");
               break;
           case ERR_MISSING_CLOSE_PAREN:
               fprintf(mon_output, "')' expected:\n");
               break;
           case ERR_INCOMPLETE_COMPARE_OP:
               fprintf(mon_output, "Compare operation missing an operand:\n");
               break;
           case ERR_EXPECT_FILENAME:
               fprintf(mon_output, "Expecting a filename:\n");
               break;
           case ERR_ADDR_TOO_BIG:
               fprintf(mon_output, "Address too large:\n");
               break;
           case ERR_IMM_TOO_BIG:
               fprintf(mon_output, "Immediate argument too large:\n");
               break;
           case ERR_ILLEGAL_INPUT:
           default:
               fprintf(mon_output, "Wrong syntax:\n");
       }
       fprintf(mon_output, "  %s\n", input);
       for (i = 0; i < last_len; i++)
           fprintf(mon_output, " ");
       fprintf(mon_output, "  ^\n");
       asm_mode = 0;
       new_cmd = 1;
   }
   free_buffer();
}

static int yyerror(char *s)
{
   YYABORT;
   fprintf(stderr, "ERR:%s\n",s);
   return 0;
}

