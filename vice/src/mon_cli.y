%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vice.h"
#include "types.h"
#include "asm.h"

#undef M_ADDR
#include "mon.h"

#define new_mode(x,y) (LO16_TO_HI16(x)|y)

static int yyerror(char *s);
static int temp;

extern char *playback_name;
extern bool playback;

/* Defined in the lexer */
extern int new_cmd, opt_asm;
extern void free_buffer(void);
extern void make_buffer(char *str);
extern int yylex(void);
extern int cur_len, last_len;

%}

%union {
	M_ADDR a;
	M_ADDR_RANGE arange;
        int i;
        REG_ID reg;
        CONDITIONAL cond_op;
        CONDITIONAL_NODE *cond_node;
        DATATYPE dt;
        ACTION action;
        char *str;
}

%token<i> H_NUMBER D_NUMBER O_NUMBER B_NUMBER CONVERT_OP B_DATA
%token<i> TRAIL BAD_CMD MEM_OP IF MEM_COMP MEM_DISK CMD_SEP REG_ASGN_SEP EQUALS
%token<i> CMD_SIDEFX CMD_RETURN CMD_BLOCK_READ CMD_BLOCK_WRITE CMD_UP CMD_DOWN
%token<i> CMD_LOAD CMD_SAVE CMD_VERIFY CMD_IGNORE CMD_HUNT CMD_FILL CMD_MOVE
%token<i> CMD_GOTO CMD_REGISTERS CMD_READSPACE CMD_WRITESPACE CMD_DISPLAYTYPE
%token<i> CMD_MEM_DISPLAY CMD_BREAK CMD_TRACE CMD_IO CMD_BRMON CMD_COMPARE
%token<i> CMD_DUMP CMD_UNDUMP CMD_EXIT CMD_DELETE CMD_CONDITION CMD_COMMAND
%token<i> CMD_ASSEMBLE CMD_DISASSEMBLE CMD_NEXT CMD_STEP CMD_PRINT CMD_DEVICE
%token<i> CMD_HELP CMD_WATCH CMD_DISK CMD_SYSTEM CMD_QUIT CMD_CHDIR CMD_BANK
%token<i> CMD_LOAD_LABELS CMD_SAVE_LABELS CMD_ADD_LABEL CMD_DEL_LABEL CMD_SHOW_LABELS
%token<i> CMD_RECORD CMD_STOP CMD_PLAYBACK
%token<i> L_PAREN R_PAREN ARG_IMMEDIATE REG_A REG_X REG_Y COMMA INST_SEP
%token<str> STRING FILENAME R_O_L OPCODE LABEL
%token<reg> REGISTER
%left<cond_op> COMPARE_OP
%token<dt> DATA_TYPE INPUT_SPEC
%token<action> CMD_CHECKPT_ONOFF TOGGLE

%type<a> address opt_address 
%type<arange> address_range address_opt_range
%type<cond_node> cond_expr compare_operand
%type<i> command block_cmd number expression
%type<i> memspace memloc memaddr opt_brknum breakpt_num opt_mem_op
%type<i> register_mod opt_count command_list top_level value
%type<i> asm_operand_mode assembly_instruction end_cmd
%type<i> assembly_instr_list post_assemble opt_memspace
%type<str> rest_of_line data_list data_element 

%type<i> symbol_table_rules asm_rules memory_rules checkpoint_rules
%type<i> checkpoint_control_rules monitor_state_rules
%type<i> monitor_misc_rules file_rules cmd_file_rules

%left '+' '-'
%left '*' '/'

%%

top_level: command_list
         | assembly_instruction TRAIL
         | TRAIL { new_cmd = 1; asm_mode = 0; }
         ;

command_list: command
            | command_list command
            ;

end_cmd: CMD_SEP
       | TRAIL
       ;

command: CMD_BANK end_cmd { fprintf(mon_output, "Bank command not done yet.\n"); }
       | register_mod
       | CMD_GOTO address end_cmd { jump($2); }
       | CMD_IO end_cmd { fprintf(mon_output, "Display IO registers\n"); }
       | CMD_RETURN end_cmd { fprintf(mon_output, "Continue until RTS/RTI\n"); }
       | CMD_DUMP end_cmd { puts("Dump machine state."); }
       | CMD_UNDUMP end_cmd { puts("Undump machine state."); }
       | CMD_STEP opt_count end_cmd { instructions_step($2); }
       | CMD_NEXT opt_count end_cmd { instructions_next($2); }
       | CMD_UP opt_count end_cmd { stack_up($2); }
       | CMD_DOWN opt_count end_cmd { stack_down($2); }
       | block_cmd 
       | symbol_table_rules
       | memory_rules
       | asm_rules
       | checkpoint_rules
       | checkpoint_control_rules
       | monitor_state_rules
       | monitor_misc_rules
       | file_rules 
       | cmd_file_rules 
       | BAD_CMD { YYABORT; }
       ;

symbol_table_rules: CMD_LOAD_LABELS opt_memspace FILENAME end_cmd 	{ mon_load_symbols($2, $3); }
                  | CMD_SAVE_LABELS opt_memspace FILENAME end_cmd 	{ mon_save_symbols($2, $3); }
                  | CMD_ADD_LABEL address LABEL end_cmd 		{ add_name_to_symbol_table($2, $3); }
                  | CMD_DEL_LABEL opt_memspace LABEL end_cmd 		{ remove_name_from_symbol_table($2, $3); }
                  | CMD_SHOW_LABELS opt_memspace end_cmd 		{ print_symbol_table($2); }
                  ;

asm_rules: CMD_ASSEMBLE address { start_assemble_mode($2, NULL); } post_assemble end_cmd 
         | CMD_ASSEMBLE address end_cmd 		{ start_assemble_mode($2, NULL); }
         | CMD_DISASSEMBLE address_opt_range end_cmd 	{ disassemble_lines($2); }
         | CMD_DISASSEMBLE end_cmd 			{ disassemble_lines(new_range(bad_addr,bad_addr)); }
         ;

memory_rules: CMD_MOVE address_range address end_cmd 		  { move_memory($2, $3); }
            | CMD_COMPARE address_range address end_cmd 	  { compare_memory($2, $3); }
            | CMD_FILL address_range data_list end_cmd 		  { fill_memory($2, $3); }
            | CMD_HUNT address_range data_list end_cmd 		  { hunt_memory($2, $3); }
            | CMD_MEM_DISPLAY DATA_TYPE address_opt_range end_cmd { display_memory($2, $3); }
            | CMD_MEM_DISPLAY address_opt_range end_cmd 	  { display_memory(0, $2); }
            | CMD_MEM_DISPLAY end_cmd 				  { display_memory(0,new_range(bad_addr,bad_addr)); }
            ;

checkpoint_rules: CMD_BREAK address_opt_range end_cmd { add_breakpoint($2, FALSE, FALSE, FALSE); }
                | CMD_BREAK address_opt_range IF cond_expr end_cmd { temp = add_breakpoint($2, FALSE, FALSE, FALSE); 
                                                              set_brkpt_condition(temp, $4); }
                | CMD_WATCH opt_mem_op address_opt_range end_cmd { add_breakpoint($3, FALSE, 
                              ($2 == e_load || $2 == e_load_store), ($2 == e_store || $2 == e_load_store)); }
                | CMD_TRACE address_opt_range end_cmd { add_breakpoint($2, TRUE, FALSE, FALSE); }
                | CMD_BREAK end_cmd { print_breakpts(); }
                | CMD_TRACE end_cmd { print_breakpts(); }
                | CMD_WATCH end_cmd { print_breakpts(); }
                ;


checkpoint_control_rules: CMD_CHECKPT_ONOFF breakpt_num end_cmd 	 { switch_breakpt($1, $2); }
                        | CMD_IGNORE breakpt_num opt_count end_cmd 	 { set_ignore_count($2, $3); }
                        | CMD_DELETE opt_brknum end_cmd 		 { delete_breakpoint($2); }
                        | CMD_CONDITION breakpt_num IF cond_expr end_cmd { set_brkpt_condition($2, $4); }
                        | CMD_COMMAND breakpt_num STRING end_cmd 	 { set_breakpt_command($2, $3); }
                        ;

monitor_state_rules: CMD_SIDEFX TOGGLE end_cmd { sidefx = (($2==e_TOGGLE)?(sidefx^1):$2); }
                   | CMD_SIDEFX end_cmd { fprintf(mon_output, "sidefx %d\n",sidefx); }
                   | CMD_DISPLAYTYPE DATA_TYPE end_cmd { default_datatype = $2; }
                   | CMD_DISPLAYTYPE end_cmd { fprintf(mon_output, "Default datatype is %s\n", 
                                      datatype_string[default_datatype]); }
                   | CMD_READSPACE memspace end_cmd { fprintf(mon_output, "Setting default readspace to %s\n",
                                              SPACESTRING($2)); default_readspace = $2; }
                   | CMD_READSPACE end_cmd { fprintf(mon_output, "Default readspace is %s\n",
                                     SPACESTRING(default_readspace)); }
                   | CMD_WRITESPACE memspace end_cmd { fprintf(mon_output, "Setting default writespace to %s\n", 
                                               SPACESTRING($2)); default_writespace = $2; }
                   | CMD_WRITESPACE end_cmd { fprintf(mon_output,"Default writespace is %s\n",
                                      SPACESTRING(default_writespace)); }
                   | CMD_DEVICE memspace end_cmd { fprintf(mon_output,"Setting default device to %s\n", 
                                           SPACESTRING($2)); default_readspace = default_writespace = $2; }
                   | CMD_QUIT end_cmd { printf("Quit.\n"); exit(-1); exit(0); }
                   | CMD_EXIT end_cmd { exit_mon = 1; YYACCEPT; }
                   ;

monitor_misc_rules: CMD_DISK rest_of_line end_cmd 	{ execute_disk_command($2); }
                  | CMD_PRINT expression end_cmd 	{ fprintf(mon_output, "\t%d\n",$2); }
                  | CMD_HELP end_cmd 			{ print_help(-1); }
                  | CMD_HELP rest_of_line  end_cmd 	{ print_help(cmd_lookup_index($2)); }
                  | CMD_SYSTEM rest_of_line end_cmd 	{ printf("SYSTEM COMMAND: %s\n",$2); }
                  | CONVERT_OP expression end_cmd 	{ print_convert($2); }
                  | CMD_CHDIR rest_of_line end_cmd 	{ change_dir($2); }
                  ;

file_rules: CMD_LOAD FILENAME address end_cmd { mon_load_file($2,$3); } 
          | CMD_SAVE FILENAME address_range end_cmd { mon_save_file($2,$3); } 
          | CMD_VERIFY FILENAME address end_cmd { mon_verify_file($2,$3); } 
          ;

cmd_file_rules: CMD_RECORD FILENAME end_cmd { record_commands($2); }
              | CMD_STOP end_cmd { end_recording(); }
              | CMD_PLAYBACK FILENAME end_cmd { playback=TRUE; playback_name = $2; }
              ;

rest_of_line: R_O_L { $$ = $1; }
            | { $$ = NULL; }
            ;

opt_mem_op: MEM_OP { $$ = $1; }
          | { $$ = e_load_store; }

register_mod: CMD_REGISTERS { print_registers(e_default_space); }
            | CMD_REGISTERS memspace { print_registers($2); }
            | CMD_REGISTERS reg_list
            ;

reg_list: reg_list REG_ASGN_SEP reg_asgn
        | reg_asgn
        ;

reg_asgn: REGISTER EQUALS number { set_reg_val($1, default_writespace, $3); }
        | memspace REGISTER '=' number { set_reg_val($2, $1, $4); }
        ;
 
opt_count: expression { $$ = $1; }
         | { $$ = -1; }
         ;

opt_brknum: breakpt_num { $$ = $1; }
          | { $$ = -1; }
          ;

breakpt_num: D_NUMBER { $$ = $1; }
           ;

block_cmd: CMD_BLOCK_READ expression expression opt_address { block_cmd(0,$2,$3,$4); }
         | CMD_BLOCK_WRITE expression expression address { block_cmd(1,$2,$3,$4); }
         ;

address_range: address address { $$ = new_range($1,$2); }
             | address '|' '+' number { $$ = new_range($1,new_addr(e_default_space,addr_location($1)+$4)); }
             | address '|' '-' number { $$ = new_range($1,new_addr(e_default_space,addr_location($1)-$4)); }
             ;

address_opt_range: address opt_address { $$ = new_range($1,$2); }

opt_address: address { $$ = $1; }
           |         { $$ = bad_addr; }
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

memloc: memaddr { $$ = check_addr_limits($1); }
      ;

memaddr: number { $$ = $1; } 

expression: expression '+' expression { $$ = $1 + $3; } 
          | expression '-' expression { $$ = $1 - $3; } 
          | expression '*' expression { $$ = $1 * $3; } 
          | expression '/' expression { $$ = ($3) ? ($1 / $3) : 1; } 
          | '(' expression ')' { $$ = $2; }
          | value  { $$ = $1; }
          ;

cond_expr: cond_expr COMPARE_OP cond_expr {
              $$ = new_cond; $$->is_parenthized = FALSE;
              $$->child1 = $1; $$->child2 = $3; $$->operation = $2; }
         | L_PAREN cond_expr R_PAREN { $$ = $2; $$->is_parenthized = TRUE; }
         | compare_operand { $$ = $1; }
         ;

compare_operand: REGISTER { $$ = new_cond; $$->operation = e_INV; $$->is_parenthized = FALSE;
                            $$->reg_num = $1; $$->is_reg = TRUE; }
               | memspace REGISTER { $$ = new_cond; $$->operation = e_INV;  $$->is_parenthized = FALSE;
                            $$->reg_num = $2; $$->is_reg = TRUE; }
               | number   { $$ = new_cond; $$->operation = e_INV; $$->is_parenthized = FALSE;
                            $$->value = $1; $$->is_reg = FALSE; }
               ;

data_list: data_list data_element
         | data_element
         ;

data_element: number { add_number_to_buffer($1); }
            | STRING { add_string_to_buffer($1); }
            ;

value: number { $$ = $1; }
     | REGISTER { $$ = get_reg_val(default_readspace, $1); }
     | memspace REGISTER { $$ = get_reg_val($1, $2); }
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

asm_operand_mode: ARG_IMMEDIATE number { $$ = new_mode(IMMEDIATE,$2); }
                | number { if ($1 < 0x100)
                              $$ = new_mode(ZERO_PAGE,$1);
                           else
                              $$ = new_mode(ABSOLUTE,$1);
                         }
                | number COMMA REG_X  { if ($1 < 0x100)
                                           $$ = new_mode(ZERO_PAGE_X,$1);
                                        else
                                           $$ = new_mode(ABSOLUTE_X,$1);
                                      }
                | number COMMA REG_Y  { if ($1 < 0x100)
                                           $$ = new_mode(ZERO_PAGE_Y,$1);
                                        else
                                           $$ = new_mode(ABSOLUTE_Y,$1);
                                      }
                | L_PAREN number R_PAREN  { $$ = new_mode(ABS_INDIRECT,$2); }
                | L_PAREN number COMMA REG_X R_PAREN { $$ = new_mode(INDIRECT_X,$2); }
                | L_PAREN number R_PAREN COMMA REG_Y { $$ = new_mode(INDIRECT_Y,$2); }
                | { $$ = new_mode(IMPLIED,0); }
                | REG_A { $$ = new_mode(ACCUMULATOR,0); }
                ;


%% 

extern FILE *yyin;
extern int yydebug;

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
       fprintf(mon_output, "Illegal input:\n  %s.\n", input);
       for (i=0;i<last_len;i++)   fprintf(mon_output, " ");
       fprintf(mon_output, "  ^\n");
       new_cmd = 1;
   }
   free_buffer();
}

int yyerror(char *s)
{
   YYABORT;
   fprintf(stderr, "ERR:%s\n",s);
   return 0;
}

