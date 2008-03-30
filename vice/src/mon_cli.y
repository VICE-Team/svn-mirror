%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vice.h"
#include "types.h"
#include "mon.h"

int yyerror(char *s);

/* Defined in the lexer */
extern int set_rol, rol, new_cmd, opt_asm;
extern void free_buffer(void);
extern void make_buffer(char *str);
extern int yylex(void);

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

%token<i> H_NUMBER D_NUMBER O_NUMBER B_NUMBER COMMAND_NAME CONVERT_OP B_DATA
%token<i> TRAIL BAD_CMD MEM_OP IF
%token<i> CMD_SIDEFX CMD_RETURN CMD_BLOCK_READ CMD_BLOCK_WRITE CMD_UP CMD_DOWN
%token<i> CMD_LOAD CMD_SAVE CMD_VERIFY CMD_IGNORE CMD_HUNT CMD_FILL CMD_MOVE
%token<i> CMD_GOTO CMD_REGISTERS CMD_READSPACE CMD_WRITESPACE CMD_DISPLAYTYPE
%token<i> CMD_MEM_DISPLAY CMD_BREAK CMD_TRACE CMD_IO CMD_BRMON CMD_COMPARE
%token<i> CMD_DUMP CMD_UNDUMP CMD_EXIT CMD_DELETE CMD_CONDITION CMD_COMMAND
%token<i> CMD_ASSEMBLE CMD_DISASSEMBLE CMD_NEXT CMD_STEP CMD_PRINT CMD_DEVICE
%token<i> CMD_HELP CMD_WATCH CMD_DISK CMD_SYSTEM CMD_QUIT CMD_CHDIR
%token<str> STRING FILENAME R_O_L ASM_LINE
%token<reg> REGISTER
%token<cond_op> COMPARE_OP
%token<dt> DATA_TYPE INPUT_SPEC
%token<action> CMD_BREAKPT_ONOFF TOGGLE

%type<a> address opt_address 
%type<arange> address_range address_opt_range
%type<cond_node> cond_expr compare_operand
%type<i> command handle_read_sidefx block_cmd number expression
%type<i> memspace memloc memaddr opt_brknum breakpt_num opt_mem_op
%type<i> register_mod opt_count meta_command command_list assemble
%type<i> memory_display space_mod file_cmd no_arg_cmds disassemble
%type<i> set_list_breakpts set_list_watchpts set_list_tracepts value
%type<i> display_type
%type<str> rest_of_line data_list data_element 

%left '+' '-'
%left '*' '/'

%%

command_list: meta_command TRAIL
            | command_list meta_command TRAIL
            | ASM_LINE TRAIL { $$ = 0; 
                               if ($1) {
                                  assemble_line($1); 
                               } else {
                                  new_cmd = 1;
                                  end_assemble_mode();
                               }
                             }
            | TRAIL { new_cmd = 1; end_assemble_mode(); }
            ;

meta_command: command 

command: COMMAND_NAME {puts("Unsupported command"); }
       | handle_read_sidefx 
       | register_mod
       | block_cmd 
       | set_list_breakpts
       | set_list_watchpts
       | set_list_tracepts
       | CMD_BREAKPT_ONOFF breakpt_num { switch_breakpt($1, $2); }
       | CMD_IGNORE breakpt_num opt_count { set_ignore_count($2, $3); }
       | CMD_MOVE address_range address { move_memory($2, $3); }
       | CMD_COMPARE address_range address { compare_memory($2, $3); }
       | CMD_GOTO address { jump($2); }
       | CMD_DELETE opt_brknum { delete_breakpoint($2); }
       | CMD_EXIT TRAIL { exit_mon = 1; YYACCEPT; }
       | display_type
       | no_arg_cmds
       | CMD_STEP opt_count { instructions_step($2); }
       | CMD_NEXT opt_count { instructions_next($2); }
       | CMD_UP opt_count { stack_up($2); }
       | CMD_DOWN opt_count { stack_down($2); }
       | file_cmd 
       | space_mod
       | memory_display 
       | assemble
       | disassemble
       | CMD_CONDITION breakpt_num cond_expr { set_brkpt_condition($2, $3); }
       | CMD_COMMAND breakpt_num STRING { set_breakpt_command($2, $3); }
       | CMD_FILL address_range data_list { fill_memory($2, $3); }
       | CMD_HUNT address_range data_list { hunt_memory($2, $3); }
       | CMD_PRINT expression { fprintf(mon_output, "\t%d\n",$2); }
       | CMD_HELP { print_help(); }
       | CMD_DISK rest_of_line { printf("DISK COMMAND: %s\n",$2); }
       | CMD_SYSTEM rest_of_line { printf("SYSTEM COMMAND: %s\n",$2); }
       | CONVERT_OP expression { print_convert($2); }
       | CMD_QUIT { printf("Quit.\n"); exit(-1); exit(0); }
       | CMD_CHDIR rest_of_line { change_dir($2); }
       | BAD_CMD { YYABORT; }
       ;

rest_of_line: R_O_L { $$ = $1; }

display_type: CMD_DISPLAYTYPE DATA_TYPE { default_datatype = $2; }
            | CMD_DISPLAYTYPE { fprintf(mon_output, "Default datatype is %s\n", 
                                        datatype_string[default_datatype]); }
            ;

assemble: CMD_ASSEMBLE address rest_of_line { start_assemble_mode($2, $3); }
        | CMD_ASSEMBLE address { start_assemble_mode($2, NULL); }
        ;

disassemble: CMD_DISASSEMBLE address_opt_range { disassemble_lines($2); }
           | CMD_DISASSEMBLE { disassemble_lines(new_range(bad_addr,bad_addr)); }
           ;

handle_read_sidefx: CMD_SIDEFX TOGGLE { sidefx = (($2==e_TOGGLE)?(sidefx^1):$2); }
                  | CMD_SIDEFX { fprintf(mon_output, "sidefx %d\n",sidefx); }

set_list_breakpts: CMD_BREAK address_opt_range { add_breakpoint($2, FALSE, FALSE, FALSE); }
                 | CMD_BREAK { print_breakpts(); }
                 ;

opt_mem_op: MEM_OP { $$ = $1; }
          | { $$ = e_load_store; }

set_list_watchpts: CMD_WATCH opt_mem_op address_opt_range { add_breakpoint($3, FALSE, 
                              ($2 == e_load || $2 == e_load_store), ($2 == e_store || $2 == e_load_store)); }
                 | CMD_WATCH { print_breakpts(); }
                 ;

set_list_tracepts: CMD_TRACE address_opt_range { add_breakpoint($2, TRUE, FALSE, FALSE); }
                 | CMD_TRACE { print_breakpts(); }
                 ;

no_arg_cmds: CMD_IO { fprintf(mon_output, "Display IO registers\n"); }
           | CMD_RETURN { fprintf(mon_output, "Continue until RTS/RTI\n"); }
           | CMD_DUMP { puts("Dump machine state."); }
           | CMD_UNDUMP { puts("Undump machine state."); }
           ;

memory_display: CMD_MEM_DISPLAY DATA_TYPE address_opt_range { display_memory($2, $3); }
              | CMD_MEM_DISPLAY address_opt_range { display_memory(0, $2); }
              | CMD_MEM_DISPLAY { display_memory(0,new_range(bad_addr,bad_addr)); }
              ;

space_mod: CMD_READSPACE memspace { fprintf(mon_output, "Setting default readspace to %s\n",SPACESTRING($2)); 
                                      default_readspace = $2; }
         | CMD_READSPACE { fprintf(mon_output, "Default readspace is %s\n",SPACESTRING(default_readspace)); }
         | CMD_WRITESPACE memspace { fprintf(mon_output, "Setting default writespace to %s\n", SPACESTRING($2)); 
                                       default_writespace = $2; }
         | CMD_WRITESPACE { fprintf(mon_output,"Default writespace is %s\n",SPACESTRING(default_writespace)); }
         | CMD_DEVICE memspace { fprintf(mon_output,"Setting default device to %s\n", SPACESTRING($2)); 
                                 default_readspace = default_writespace = $2; }
         ;

register_mod: CMD_REGISTERS { print_registers(e_default_space); }
            | CMD_REGISTERS memspace { print_registers($2); }
            | CMD_REGISTERS reg_list
            ;

reg_list: reg_list ',' reg_asgn
        | reg_asgn
        ;

reg_asgn: REGISTER '=' number { set_reg_val($1,$3); }
        ;
 
file_cmd: CMD_LOAD FILENAME address { mon_load_file($2,$3); } 
        | CMD_SAVE FILENAME address_range { mon_save_file($2,$3); } 
        | CMD_VERIFY FILENAME address { mon_verify_file($2,$3); } 
        ;

opt_count: expression { $$ = $1; }
         | { $$ = -1; }
         ;

opt_brknum: breakpt_num { $$ = $1; }
          | { $$ = -1; }
          ;

breakpt_num: number { $$ = $1; }
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

address: memloc { $$ = new_addr(e_default_space,$1); if (opt_asm) rol = 1; }
       | memspace ':' memloc { $$ = new_addr($1,$3); if (opt_asm) rol = 1; }

memspace: 'c' { $$ = e_comp_space; } 
        | 'd' { $$ = e_disk_space; }
        ;

memloc: memaddr { $$ = $1; }
      ;

memaddr: number { $$ = $1; } 

expression: expression '+' expression { $$ = $1 + $3; } 
          | expression '-' expression { $$ = $1 - $3; } 
          | expression '*' expression { $$ = $1 * $3; } 
          | expression '/' expression { $$ = ($3) ? ($1 / $3) : 1; } 
          | '(' expression ')' { $$ = $2; }
          | value  { $$ = $1; }
          ;

cond_expr: compare_operand COMPARE_OP compare_operand {
              $$ = new_cond;
              $$->child1 = $1; $$->child2 = $3; $$->operation = $2; }
         ;

compare_operand: REGISTER { $$ = new_cond; $$->operation = e_INV; 
                            $$->reg_num = $1; $$->is_reg = 1; }
               | number   { $$ = new_cond; $$->operation = e_INV;
                            $$->value = $1; $$->is_reg = 0; }
               ;

data_list: data_list data_element
         | data_element
         ;

data_element: number { add_number_to_buffer($1); }
            | STRING { add_string_to_buffer($1); }
            ;

value: number { $$ = $1; }
     | REGISTER { $$ = get_reg_val(e_comp_space, $1); /* FIXME */ }

number: H_NUMBER { $$ = $1; }
      | D_NUMBER { $$ = $1; }
      | O_NUMBER { $$ = $1; }
      | B_NUMBER { $$ = $1; }
      ;

%% 

extern FILE *yyin;
extern int yydebug;

void parse_and_execute_line(char *input)
{
   char *temp_buf;

   temp_buf = (char *) malloc(strlen(input)+3);
   strcpy(temp_buf,input);
   temp_buf[strlen(input)] = '\n';
   temp_buf[strlen(input)+1] = '\0';
   temp_buf[strlen(input)+2] = '\0';

   make_buffer(temp_buf);
   if (yyparse() != 0) {
       fprintf(mon_output, "Illegal input: %s.\n", input);
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

