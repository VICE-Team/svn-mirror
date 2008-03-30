%{

/* Lexer for x64 monitor */
#include <assert.h>
#include <string.h>
#include "vice.h"
#include "types.h"
#include "mon.h"
#include "mon_parse.h"

#define min(a,b) ((a) < (b) ? (a) : (b))

int new_cmd = 1, quote = 0, opt_asm = 0;
int yywrap();
int resolve_datatype(unsigned guess_type, char *num, int *val);
YY_BUFFER_STATE my_state;

%}

%x FNAME CMD STR ROL ASM_MODE

%%

%{
   if (new_cmd) {
      if (asm_mode) 
         BEGIN ASM_MODE;
      else
         BEGIN CMD;
      new_cmd = 0;
   }
%}

<CMD>bank |
<CMD>basic      	{ BEGIN INITIAL; fprintf(mon_output, "Not yet supported: %s\n", yytext); return COMMAND_NAME; }

<CMD>sidefx 		{ BEGIN INITIAL; return CMD_SIDEFX; }
<CMD>return 		{ BEGIN INITIAL; return CMD_RETURN; }
<CMD>br			{ BEGIN INITIAL; return CMD_BLOCK_READ; }
<CMD>bw			{ BEGIN INITIAL; return CMD_BLOCK_WRITE; }
<CMD>up			{ BEGIN INITIAL; return CMD_UP; }
<CMD>down		{ BEGIN INITIAL; return CMD_DOWN; }
<CMD>next 		{ BEGIN INITIAL; return CMD_NEXT; }
<CMD>step 		{ BEGIN INITIAL; return CMD_STEP; }
<CMD>ignore		{ BEGIN INITIAL; return CMD_IGNORE; }
<CMD>load		{ BEGIN FNAME; return CMD_LOAD; }
<CMD>save		{ BEGIN FNAME; return CMD_SAVE; }
<CMD>verify		{ BEGIN FNAME; return CMD_VERIFY; }
<CMD>f |
<CMD>fill		{ BEGIN INITIAL; return CMD_FILL; }
<CMD>h |
<CMD>hunt		{ BEGIN INITIAL; return CMD_HUNT; }
<CMD>t |
<CMD>transfer |
<CMD>move 		{ BEGIN INITIAL; return CMD_MOVE; }
<CMD>c |
<CMD>compare		{ BEGIN INITIAL; return CMD_COMPARE; }
<CMD>goto		{ BEGIN INITIAL; return CMD_GOTO; }
<CMD>r |
<CMD>registers 		{ BEGIN INITIAL; return CMD_REGISTERS; }
<CMD>readspace		{ BEGIN INITIAL; return CMD_READSPACE; }
<CMD>writespace		{ BEGIN INITIAL; return CMD_WRITESPACE; }
<CMD>dt |
<CMD>displaytype     	{ BEGIN INITIAL; return CMD_DISPLAYTYPE; }
<CMD>m|M 		{ BEGIN INITIAL; return CMD_MEM_DISPLAY; }
<CMD>break		{ BEGIN INITIAL; return CMD_BREAK; }
<CMD>trace		{ BEGIN INITIAL; return CMD_TRACE; }
<CMD>watch		{ BEGIN INITIAL; return CMD_WATCH; }
<CMD>~ 			{ BEGIN INITIAL; return CONVERT_OP; }
<CMD>enable		{ BEGIN INITIAL; yylval.i = e_ON; return CMD_BREAKPT_ONOFF; }
<CMD>disable		{ BEGIN INITIAL; yylval.i = e_OFF; return CMD_BREAKPT_ONOFF; }
<CMD>io			{ BEGIN INITIAL; return CMD_IO; }
<CMD>brmon		{ BEGIN INITIAL; return CMD_BRMON; }
<CMD>dump		{ BEGIN INITIAL; return CMD_DUMP; }
<CMD>undump		{ BEGIN INITIAL; return CMD_UNDUMP; }
<CMD>delete		{ BEGIN INITIAL; return CMD_DELETE; }
<CMD>condition		{ BEGIN INITIAL; return CMD_CONDITION; }
<CMD>command		{ BEGIN INITIAL; return CMD_COMMAND; }
<CMD>x |
<CMD>exit		{ BEGIN INITIAL; return CMD_EXIT; }
<CMD>quit		{ BEGIN INITIAL; return CMD_QUIT; }
<CMD>a			{ BEGIN INITIAL; opt_asm = 1; return CMD_ASSEMBLE; }
<CMD>d			{ BEGIN INITIAL; return CMD_DISASSEMBLE; }
<CMD>print		{ BEGIN INITIAL; return CMD_PRINT; }
<CMD>device 		{ BEGIN INITIAL; return CMD_DEVICE; }
<CMD>system  		{ BEGIN ROL; return CMD_SYSTEM; }
<CMD>cd			{ BEGIN ROL; return CMD_CHDIR; }
<CMD>@ 			{ BEGIN ROL; return CMD_DISK; }
<CMD>load_label   	{ BEGIN FNAME; return CMD_LOAD_LABELS; }
<CMD>save_label 	{ BEGIN FNAME; return CMD_SAVE_LABELS; }
<CMD>add_label 		{ BEGIN INITIAL; return CMD_ADD_LABEL; }
<CMD>del_label 		{ BEGIN INITIAL; return CMD_DEL_LABEL; }
<CMD>label 		{ BEGIN INITIAL; return CMD_LABEL; }

<CMD>\? |
<CMD>help		{ BEGIN INITIAL; return CMD_HELP; }

<CMD>[^~@ \t\n]*		{ BEGIN INITIAL; new_cmd = 1; return BAD_CMD; }

on  		{ yylval.i = e_ON; return TOGGLE; }
off 		{ yylval.i = e_OFF; return TOGGLE; }
toggle		{ yylval.i = e_TOGGLE; return TOGGLE; }

load 		{ yylval.i = e_load; return MEM_OP; }
store 		{ yylval.i = e_store; return MEM_OP; }

if		{ return IF; }

\"		{ if (!quote) {
                     quote = 1;
                     BEGIN STR;
                  }
                }
<STR>\"		{ if (quote) {
                     quote = 0;
                     BEGIN INITIAL;
                  }
                }
<STR>[0-9a-zA-Z ]+ { yylval.str = strdup(yytext); return STRING; }
<STR>\n 	{ puts("ERROR:"); exit(-1); }

  /* [.*]+ { return B_DATA; } */


A 		{ yylval.i = e_A; return REGISTER; }
X 		{ yylval.i = e_X; return REGISTER; }
Y 		{ yylval.i = e_Y; return REGISTER; }
PC 		{ yylval.i = e_PC; return REGISTER; }
SP		{ yylval.i = e_SP; return REGISTER; }

\.S		{ yylval.i = e_sprite; return INPUT_SPEC; }
\.C		{ yylval.i = e_character; return INPUT_SPEC; }
\.\$		{ yylval.i = e_hexadecimal; return INPUT_SPEC; }
\.\+		{ yylval.i = e_decimal; return INPUT_SPEC; }
\.%		{ yylval.i = e_binary; return INPUT_SPEC; }
\.&		{ yylval.i = e_octal; return INPUT_SPEC; }
\.TA		{ yylval.i = e_text_ascii; return INPUT_SPEC; }
\.TP		{ yylval.i = e_text_petscii; return INPUT_SPEC; }
\.\>		{ yylval.i = e_6502_asm; return INPUT_SPEC; }

   /* D		{ yylval.i = e_decimal; return DATA_TYPE; } */
   /* H		{ yylval.i = e_hexadecimal; return DATA_TYPE; } */
   /* B		{ yylval.i = e_binary; return DATA_TYPE; } */
   /* O		{ yylval.i = e_octal; return DATA_TYPE; } */
   /* C		{ yylval.i = e_character; return DATA_TYPE; } */
   /* S		{ yylval.i = e_sprite; return DATA_TYPE; } */
   /* TA		{ yylval.i = e_text_ascii; return DATA_TYPE; } */
   /* TP		{ yylval.i = e_text_petscii; return DATA_TYPE; } */

c|C { return MEM_COMP; }
d|D { return MEM_DISK; }

== 		{ yylval.i = e_EQU; return COMPARE_OP; }
!= 		{ yylval.i = e_NEQ; return COMPARE_OP; }
\<=		{ yylval.i = e_LTE; return COMPARE_OP; }
\<		{ yylval.i = e_LT;  return COMPARE_OP; }
\>		{ yylval.i = e_GT;  return COMPARE_OP; }
\>=		{ yylval.i = e_GTE; return COMPARE_OP; }
&&		{ yylval.i = e_AND; return COMPARE_OP; }
\|\|		{ yylval.i = e_OR;  return COMPARE_OP; }


[ \t]*\n          { new_cmd = 1; return TRAIL; }


<FNAME>\"[0-9a-zA-Z\.]+\" { yytext[yyleng-1] = '\0';
                   yylval.str = strdup(yytext+1);
                   BEGIN INITIAL; return FILENAME; }

<ROL>\n          { new_cmd = 1; opt_asm = 0; return TRAIL; }
<ROL>[^ \t\n][^\n]*          { yylval.str = strdup(yytext); return R_O_L; }
<ROL>[ \t]*

<ASM_MODE>([b-wzB-WZ][a-zA-Z]*)+|([a-zA-Z]{2,})     { yylval.str = strdup(yytext); return OPCODE; }
<ASM_MODE,INITIAL>\.[a-zA-Z][a-zA-Z0-9]*            { yylval.str = strdup(yytext); return LABEL; }
<ASM_MODE>[ \t]*
<ASM_MODE>\n            { new_cmd = 1; return TRAIL; }

<ASM_MODE>a|A           { return REG_A; }
<ASM_MODE>x|X           { return REG_X; }
<ASM_MODE>y|Y           { return REG_Y; }

<ASM_MODE>,             { return COMMA; }
<ASM_MODE>\(            { return L_PAREN; }
<ASM_MODE>\)            { return R_PAREN; }
<ASM_MODE>#             { return ARG_IMMEDIATE; }
<ASM_MODE>:             { return INST_SEP; }

<ASM_MODE,INITIAL>1[01]*                  { return resolve_datatype(B_NUMBER, yytext, &(yylval.i)); }
<ASM_MODE,INITIAL>%[01]+                  { yylval.i = strtol(yytext+1, NULL, 2); return B_NUMBER; }
<ASM_MODE,INITIAL>&[0-7]+                 { yylval.i = strtol(yytext+1, NULL, 8); return O_NUMBER; }
<ASM_MODE,INITIAL>[1-7][0-7]*             { return resolve_datatype(O_NUMBER, yytext, &(yylval.i)); }
<ASM_MODE,INITIAL>\+[0-9]+                { yylval.i = strtol(yytext+1, NULL, 10); return D_NUMBER; }
<ASM_MODE,INITIAL>[0-9]+                  { return resolve_datatype(D_NUMBER, yytext, &(yylval.i)); }
<ASM_MODE,INITIAL>\$[0-9a-fA-F]+          { yylval.i = strtol(yytext+1, NULL, 16); return H_NUMBER; }
<ASM_MODE,INITIAL>(0x|0X)[0-9a-fA-F]+     { yylval.i = strtol(yytext+2, NULL, 16); return H_NUMBER; }
<ASM_MODE,INITIAL>[0-9a-fA-F]+            { yylval.i = strtol(yytext, NULL, 16); return H_NUMBER; }

[^ \t]		{ return yytext[0]; }
[ \t]*

%%

int resolve_datatype(unsigned guess_type, char *num, int *val)
{
   if (default_datatype == e_hexadecimal) {
      *val = strtol(num, NULL, 16);
      return H_NUMBER;
   }

   if ((guess_type == D_NUMBER) || (default_datatype == e_decimal)) {
      *val = strtol(num, NULL, 10);
      return D_NUMBER;
   }

   if ((guess_type == O_NUMBER) || (default_datatype == e_octal)) {
      *val = strtol(num, NULL, 8);
      return O_NUMBER;
   }
   
   *val = strtol(num, NULL, 2);
   return B_NUMBER;
}

void free_buffer()
{
   yy_delete_buffer(my_state);
}

void make_buffer(char *str)
{
    my_state = yy_scan_buffer(str, strlen(str) + 2);
    assert(my_state);
}

int yywrap() { return 1; }
