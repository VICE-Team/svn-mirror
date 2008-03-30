%{

/* Lexer for x64 monitor */
#include <assert.h>
#include <string.h>
#include "vice.h"
#include "types.h"
#include "mon.h"
#include "mon_parse.h"

#define min(a,b) ((a) < (b) ? (a) : (b))

int new_cmd = 1, quote = 0, opt_asm = 0, cur_len = 0, last_len = 0, dont_match_reg_a = 0;
int temp;
int yywrap();
int resolve_datatype(unsigned guess_type, char *num, int *val);
YY_BUFFER_STATE my_state;

#define YY_USER_ACTION { last_len = cur_len; cur_len += yyleng; }

%}

%x FNAME CMD STR ROL ASM_MODE COND_MODE REG_ASGN

%%

%{
   if (new_cmd) {
      if (asm_mode) 
         BEGIN ASM_MODE;
      else
         BEGIN CMD;
      new_cmd = 0;
      cur_len = 0;
      last_len = 0;
   }
%}

<CMD>[^ \t\n]*		{ if ( (temp = cmd_lookup_index(yytext)) < 0)
                             return BAD_CMD;
                          else {
                             if (cmd_get_next_state(temp) == STATE_INITIAL) 
                                 BEGIN INITIAL;
                             else if (cmd_get_next_state(temp) == STATE_FNAME) 
                                 BEGIN FNAME;
                             else if (cmd_get_next_state(temp) == STATE_ROL) 
                                 BEGIN ROL;
                             else if (cmd_get_next_state(temp) == STATE_REG_ASGN) 
                                 BEGIN REG_ASGN;

                             return cmd_get_token(temp);
                          }
                        }

;		{ new_cmd = 1; return CMD_SEP; }

on  		{ yylval.i = e_ON; return TOGGLE; }
off 		{ yylval.i = e_OFF; return TOGGLE; }
toggle		{ yylval.i = e_TOGGLE; return TOGGLE; }

load 		{ yylval.i = e_load; return MEM_OP; }
store 		{ yylval.i = e_store; return MEM_OP; }

if		{ BEGIN COND_MODE; return IF; }

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


\.S		{ yylval.i = e_sprite; return INPUT_SPEC; }
\.C		{ yylval.i = e_character; return INPUT_SPEC; }
\.\$		{ yylval.i = e_hexadecimal; return INPUT_SPEC; }
\.\+		{ yylval.i = e_decimal; return INPUT_SPEC; }
\.%		{ yylval.i = e_binary; return INPUT_SPEC; }
\.&		{ yylval.i = e_octal; return INPUT_SPEC; }
\.TA		{ yylval.i = e_text_ascii; return INPUT_SPEC; }
\.TP		{ yylval.i = e_text_petscii; return INPUT_SPEC; }
\.\>		{ yylval.i = e_6502_asm; return INPUT_SPEC; }

D		{ yylval.i = e_decimal; return DATA_TYPE; }
H		{ yylval.i = e_hexadecimal; return DATA_TYPE; }
B		{ yylval.i = e_binary; return DATA_TYPE; }
O		{ yylval.i = e_octal; return DATA_TYPE; }
C		{ yylval.i = e_character; return DATA_TYPE; }
S		{ yylval.i = e_sprite; return DATA_TYPE; }
TA		{ yylval.i = e_text_ascii; return DATA_TYPE; }
TP		{ yylval.i = e_text_petscii; return DATA_TYPE; }

c:              { return MEM_COMP; }
d:              { return MEM_DISK; }


<*>[ \t]*
<*>\n          { dont_match_reg_a = 0; new_cmd = 1; opt_asm = 0; return TRAIL; }


<FNAME>\"[0-9a-zA-Z\.]+\" { yytext[yyleng-1] = '\0';
                   yylval.str = strdup(yytext+1);
                   BEGIN INITIAL; return FILENAME; }

<ROL>[^ \t\n][^\n]*          { yylval.str = strdup(yytext); return R_O_L; }

<REG_ASGN>{
	A 		{ yylval.i = e_A; return REGISTER; }
	X 		{ yylval.i = e_X; return REGISTER; }
	Y 		{ yylval.i = e_Y; return REGISTER; }
	PC 		{ yylval.i = e_PC; return REGISTER; }
	SP		{ yylval.i = e_SP; return REGISTER; }
	=		{ BEGIN COND_MODE; return EQUALS; }
}

<COND_MODE>{
	== 		{ yylval.i = e_EQU; return COMPARE_OP; }
	!= 		{ yylval.i = e_NEQ; return COMPARE_OP; }
	\<=		{ yylval.i = e_LTE; return COMPARE_OP; }
	\<		{ yylval.i = e_LT;  return COMPARE_OP; }
	\>		{ yylval.i = e_GT;  return COMPARE_OP; }
	\>=		{ yylval.i = e_GTE; return COMPARE_OP; }
	&&		{ yylval.i = e_AND; return COMPARE_OP; }
	\|\|		{ yylval.i = e_OR;  return COMPARE_OP; }

	\.A 		{ yylval.i = e_A; return REGISTER; }
	\.X 		{ yylval.i = e_X; return REGISTER; }
	\.Y 		{ yylval.i = e_Y; return REGISTER; }
	\.PC 		{ yylval.i = e_PC; return REGISTER; }
	\.SP		{ yylval.i = e_SP; return REGISTER; }

	,		{ BEGIN REG_ASGN; return REG_ASGN_SEP; }
        \(		{ return L_PAREN; }
        \)		{ return R_PAREN; }
}

<ASM_MODE>[a-zA-Z]{3,4}                           { yylval.str = strdup(yytext); return OPCODE; }
<ASM_MODE,INITIAL>\.[a-zA-Z][a-zA-Z0-9]*            { yylval.str = strdup(yytext); return LABEL; }

<ASM_MODE>(a|A)/[ \t]*[:\n]           { if (!dont_match_reg_a) return REG_A; 
                                        yylval.i = 0x0a; return H_NUMBER; }
<ASM_MODE>x|X           { return REG_X; }
<ASM_MODE>y|Y           { return REG_Y; }

<ASM_MODE>,             { return COMMA; }
<ASM_MODE>\(            { dont_match_reg_a = 1; return L_PAREN; }
<ASM_MODE>\)            { return R_PAREN; }
<ASM_MODE>#             { dont_match_reg_a = 1; return ARG_IMMEDIATE; }
<ASM_MODE>:             { dont_match_reg_a = 0; return INST_SEP; }

<ASM_MODE,INITIAL,COND_MODE>{
	1[01]*                  { return resolve_datatype(B_NUMBER, yytext, &(yylval.i)); }
	%[01]+                  { yylval.i = strtol(yytext+1, NULL, 2); return B_NUMBER; }
	&[0-7]+                 { yylval.i = strtol(yytext+1, NULL, 8); return O_NUMBER; }
	[0-7]*                  { return resolve_datatype(O_NUMBER, yytext, &(yylval.i)); }
	\+[0-9]+                { yylval.i = strtol(yytext+1, NULL, 10); return D_NUMBER; }
	[0-9]+                  { return resolve_datatype(D_NUMBER, yytext, &(yylval.i)); }
	\$[0-9a-fA-F]+          { yylval.i = strtol(yytext+1, NULL, 16); return H_NUMBER; }
	[0-9a-fA-F]+            { yylval.i = strtol(yytext, NULL, 16); return H_NUMBER; }
}

[^ \t]		{ return yytext[0]; }

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
