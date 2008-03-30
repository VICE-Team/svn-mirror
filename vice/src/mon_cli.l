%{

/* Lexer for x64 monitor */
#include <assert.h>
#include <string.h>
#include "vice.h"
#include "types.h"
#include "mon.h"
#include "mon_parse.h"

YY_BUFFER_STATE my_state;
#define min(a,b) ((a) < (b) ? (a) : (b))

int yywrap();

int new_cmd = 1, quote = 0, rol = 0, rol_lookahead = 0;
extern char *myinput;

%}

%x FNAME CMD STR ROL ASM_MODE

%%

%{
   if (new_cmd) {
      BEGIN CMD;
      new_cmd = 0;
   }
   if (asm_mode) {
      BEGIN ASM_MODE;
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
<CMD>fill		{ BEGIN INITIAL; return CMD_FILL; }
<CMD>hunt		{ BEGIN INITIAL; return CMD_HUNT; }
<CMD>move 		{ BEGIN INITIAL; return CMD_MOVE; }
<CMD>compare		{ BEGIN INITIAL; return CMD_COMPARE; }
<CMD>goto		{ BEGIN INITIAL; return CMD_GOTO; }
<CMD>reg |
<CMD>registers 		{ BEGIN INITIAL; return CMD_REGISTERS; }
<CMD>readspace		{ BEGIN INITIAL; return CMD_READSPACE; }
<CMD>writespace		{ BEGIN INITIAL; return CMD_WRITESPACE; }
<CMD>displaytype     	{ BEGIN INITIAL; return CMD_DISPLAYTYPE; }
<CMD>m|M 		{ BEGIN INITIAL; return CMD_MEM_DISPLAY; }
<CMD>break		{ BEGIN INITIAL; return CMD_BREAK; }
<CMD>trace		{ BEGIN INITIAL; return CMD_TRACE; }
<CMD>watch		{ BEGIN INITIAL; return CMD_WATCH; }
<CMD>c 			{ BEGIN INITIAL; return CMD_CONTINUE; }
<CMD>~ 			{ BEGIN INITIAL; return CONVERT_OP; }
<CMD>enable		{ BEGIN INITIAL; yylval.i = e_ON; return CMD_BREAKPT_ONOFF; }
<CMD>disable		{ BEGIN INITIAL; yylval.i = e_OFF; return CMD_BREAKPT_ONOFF; }
<CMD>io			{ BEGIN INITIAL; return CMD_IO; }
<CMD>brmon		{ BEGIN INITIAL; return CMD_BRMON; }
<CMD>dump		{ BEGIN INITIAL; return CMD_DUMP; }
<CMD>undump		{ BEGIN INITIAL; return CMD_UNDUMP; }
<CMD>delete		{ BEGIN INITIAL; return CMD_DELETE; }
<CMD>condition		{ BEGIN INITIAL; return CMD_CONDITION; }
<CMD>command		{ BEGIN INITIAL; rol_lookahead = 1; return CMD_COMMAND; }
<CMD>exit		{ BEGIN INITIAL; return CMD_EXIT; }
<CMD>quit		{ BEGIN INITIAL; return CMD_QUIT; }
<CMD>A			{ BEGIN INITIAL; return CMD_ASSEMBLE; }
<CMD>D			{ BEGIN INITIAL; return CMD_DISASSEMBLE; }
<CMD>print		{ BEGIN INITIAL; return CMD_PRINT; }
<CMD>system  		{ BEGIN ROL; return CMD_SYSTEM; }
<CMD>cd			{ BEGIN ROL; return CMD_CHDIR; }
<CMD>@ 			{ BEGIN ROL; return CMD_DISK; }

<CMD>\? |
<CMD>h |
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
<STR>[0-9a-zA-Z]+ { yylval.str = strdup(yytext); return STRING; }
<STR>\n 	{ puts("ERROR:"); exit(-1); }

0[xX][0-9a-fA-F]+ { yylval.i = strtol(yytext, NULL, 16);
                    return H_NUMBER;
                  }

\$[0-9a-fA-F]+ { yylval.i = strtol(yytext+1, NULL, 16);
                 return H_NUMBER;
               }

0[0-7]+ { yylval.i = strtol(yytext, NULL, 8);
           return O_NUMBER;
         }
[0-9]* { yylval.i = strtol(yytext, NULL, 10);
         return D_NUMBER;
       }
%[01]+ { yylval.i = strtol(yytext+1, NULL, 2);
         return B_NUMBER;
       }

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

D		{ yylval.i = e_decimal; return DATA_TYPE; }
H		{ yylval.i = e_hexadecimal; return DATA_TYPE; }
B		{ yylval.i = e_binary; return DATA_TYPE; }
O		{ yylval.i = e_octal; return DATA_TYPE; }
C		{ yylval.i = e_character; return DATA_TYPE; }
S		{ yylval.i = e_sprite; return DATA_TYPE; }
TA		{ yylval.i = e_text_ascii; return DATA_TYPE; }
TP		{ yylval.i = e_text_petscii; return DATA_TYPE; }

== 		{ yylval.i = e_EQU; return COMPARE_OP; }
!= 		{ yylval.i = e_NEQ; return COMPARE_OP; }
\<=		{ yylval.i = e_LTE; return COMPARE_OP; }
\<		{ yylval.i = e_LT;  return COMPARE_OP; }
\>		{ yylval.i = e_GT;  return COMPARE_OP; }
\>=		{ yylval.i = e_GTE; return COMPARE_OP; }
&&		{ yylval.i = e_AND; return COMPARE_OP; }
\|\|		{ yylval.i = e_OR;  return COMPARE_OP; }


[ \t]*\n          { new_cmd = 1; return TRAIL; }


<FNAME>\"[0-9a-zA-Z]+\" { yytext[yyleng-1] = '\0';
                   yylval.str = strdup(yytext+1);
                   BEGIN INITIAL; return FILENAME; }

<ROL>\n          { new_cmd = 1; return TRAIL; }
<ROL>[^ \t].*          { yylval.str = strdup(yytext); return R_O_L; }

<ASM_MODE>.+\n     { yylval.str = strdup(yytext); return ASM_LINE; }
<ASM_MODE>\n     { yylval.str = NULL; end_assemble_mode(); BEGIN CMD; return ASM_LINE; }

%%


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
