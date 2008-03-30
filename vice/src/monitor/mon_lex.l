/* -*- C -*-
 *
 * mon_lex.l - Lexer for the VICE built-in monitor.
 *
 * Written by
 *  Daniel Sladic <sladic@eecg.toronto.edu>
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

%{

/* Lexer for x64 monitor */

#include "vice.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "mon_command.h"
#include "montypes.h"
#include "mon_parse.h" /* FIXME ! */
#include "types.h"


#define YY_ALWAYS_INTERACTIVE 1

#undef min
#define min(a,b) ((a) < (b) ? (a) : (b))

int new_cmd = 1, quote = 0, opt_asm = 0, cur_len = 0, last_len = 0, dont_match_reg_a = 0;
int temp, push_back;
static int yywrap(void);
YY_BUFFER_STATE my_state;

#define YY_USER_ACTION { last_len = cur_len; cur_len += yyleng; }

%}

%x FNAME CMD STR ROL ASM_MODE COND_MODE REG_ASGN BNAME CTYPE

%%

%{
   if (new_cmd) {
      if (!(asm_mode && opt_asm))
         last_len = cur_len = 0;

      if (asm_mode) {
         BEGIN (ASM_MODE);
         opt_asm = 0;
      } else
         BEGIN (CMD);
      new_cmd = 0;
   }
%}

<CMD>([^ \t\n]+) { push_back = 0;
                   if ( (temp = mon_command_lookup_index(yytext, &push_back)) < 0) {
                      return BAD_CMD;
                   } else {
                      if (push_back)
                         yyless(push_back);

                      switch(mon_command_get_next_state(temp)) {
                         case STATE_INITIAL:
                            BEGIN (INITIAL);
                            break;
                         case STATE_FNAME:
                            BEGIN (FNAME);
                            break;
                         case STATE_BNAME:
                            BEGIN (BNAME);
                            break;
                         case STATE_CTYPE:
                            BEGIN (CTYPE);
                            break;
                         case STATE_ROL:
                            BEGIN (ROL);
                            break;
                         case STATE_REG_ASGN:
                            BEGIN (REG_ASGN);
                            break;
                         default:
                            assert(0);
                      }
                      yylval.i = mon_command_get_token(temp);
                      if (yylval.i == CMD_ASSEMBLE)
                         opt_asm = 1;
                      return yylval.i;
                   }
                 } 

;		{ new_cmd = 1; return CMD_SEP; }

on  		{ yylval.i = e_ON; return TOGGLE; }
off 		{ yylval.i = e_OFF; return TOGGLE; }
toggle		{ yylval.i = e_TOGGLE; return TOGGLE; }

load 		{ yylval.i = e_load; return MEM_OP; }
store 		{ yylval.i = e_store; return MEM_OP; }

if		{ BEGIN (COND_MODE); return IF; }

\"		{ if (!quote) {
                     quote = 1;
                     BEGIN (STR);
                  }
                }
<STR>\" 	{ if (quote) {
                     quote = 0;
                     BEGIN (INITIAL);
                  }
                }

<STR>[^\"\n]*      { yylval.str = lib_stralloc(yytext); return STRING; }


\.\$		{ yylval.i = e_hexadecimal; return INPUT_SPEC; }
\.\+		{ yylval.i = e_decimal; return INPUT_SPEC; }
\.%		{ yylval.i = e_binary; return INPUT_SPEC; }
\.&		{ yylval.i = e_octal; return INPUT_SPEC; }
 /* \.TA		{ yylval.i = e_text_ascii; return INPUT_SPEC; } */
 /* \.TP		{ yylval.i = e_text_petscii; return INPUT_SPEC; } */
 /* \.\>		{ yylval.i = e_6502_asm; return INPUT_SPEC; } */
 /* \.S		{ yylval.i = e_sprite; return INPUT_SPEC; } */
 /* \.C		{ yylval.i = e_character; return INPUT_SPEC; } */

D		{ yylval.i = e_decimal; return RADIX_TYPE; }
H		{ yylval.i = e_hexadecimal; return RADIX_TYPE; }
B		{ yylval.i = e_binary; return RADIX_TYPE; }
O		{ yylval.i = e_octal; return RADIX_TYPE; }
 /* C		{ yylval.i = e_character; return RADIX_TYPE; } */
 /* S		{ yylval.i = e_sprite; return RADIX_TYPE; } */
 /* TA		{ yylval.i = e_text_ascii; return RADIX_TYPE; } */
 /* TP		{ yylval.i = e_text_petscii; return RADIX_TYPE; } */

<BNAME,INITIAL,COND_MODE>(c|C):            { return MEM_COMP; }
<BNAME,INITIAL,COND_MODE>8:              { return MEM_DISK8; }
<BNAME,INITIAL,COND_MODE>9:              { return MEM_DISK9; }
<BNAME,INITIAL,COND_MODE>10:             { return MEM_DISK10; }
<BNAME,INITIAL,COND_MODE>11:             { return MEM_DISK11; }

<CTYPE>[_a-zA-Z0-9]*      { yylval.str = lib_stralloc(yytext); return CPUTYPE; }

<*>[ \t]*
<*>\n          { dont_match_reg_a = 0;
                 new_cmd = 1;
                 opt_asm = 0;
                 quote = 0;
                 BEGIN(INITIAL);
                 return TRAIL;
               }

<FNAME>\".+\" { yytext[yyleng-1] = '\0';
                yylval.str = lib_stralloc(yytext+1);
                BEGIN (INITIAL); return FILENAME; }

<BNAME>[_a-zA-Z0-9]*      { yylval.str = lib_stralloc(yytext); return BANKNAME; }

<ROL>[^ \t\n][^\n]*          { yylval.str = lib_stralloc(yytext); return R_O_L; }

<REG_ASGN>{
	A 		{ yylval.i = e_A; return REGISTER; }
	X 		{ yylval.i = e_X; return REGISTER; }
	Y 		{ yylval.i = e_Y; return REGISTER; }
	PC 		{ yylval.i = e_PC; return REGISTER; }
	SP		{ yylval.i = e_SP; return REGISTER; }
	=		{ BEGIN (COND_MODE); return EQUALS; }
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

	,		{ BEGIN (REG_ASGN); return REG_ASGN_SEP; }
        \(		{ return L_PAREN; }
        \)		{ return R_PAREN; }
}

<ASM_MODE>[a-zA-Z][a-zA-Z]\ [a-zA-Z]\,[a-zA-Z] {
    yylval.str = lib_stralloc(yytext); return OPCODE; }
<ASM_MODE>[a-zA-Z][a-zA-Z]\ [a-zA-Z]\,\([a-zA-Z][a-zA-Z]\) {
    yylval.str = lib_stralloc(yytext); return OPCODE; }
<ASM_MODE>[a-zA-Z][a-zA-Z]\ \([a-zA-Z][a-zA-Z]\)\,[a-zA-Z] {
    yylval.str = lib_stralloc(yytext); return OPCODE; }
<ASM_MODE>[a-zA-Z][a-zA-Z]\ \([a-zA-Z][a-zA-Z]\)\, {
    yylval.str = lib_stralloc(yytext); return OPCODE; }
<ASM_MODE>[a-zA-Z][a-zA-Z]\ [a-zA-Z]\, {
    yylval.str = lib_stralloc(yytext); return OPCODE; }
<ASM_MODE>[a-zA-Z][a-zA-Z][a-zA-Z]\ [0-7]\, {
    yylval.str = lib_stralloc(yytext); return OPCODE; }
<ASM_MODE>[a-zA-Z][a-zA-Z][a-zA-Z][a-zA-Z]\ [a-zA-Z]\, {
    yylval.str = lib_stralloc(yytext); return OPCODE; }
<ASM_MODE>[a-zA-Z][a-zA-Z]\ [a-zA-Z][a-zA-Z]\, {
    yylval.str = lib_stralloc(yytext); return OPCODE; }
<ASM_MODE>af|AF         { return REG_AF; }
<ASM_MODE>bc|BC         { return REG_BC; }
<ASM_MODE>de|DE         { return REG_DE; }
<ASM_MODE>hl|HL         { return REG_HL; }
<ASM_MODE>ix|IX         { return REG_IX; }
<ASM_MODE>ixh|IXH       { return REG_IXH; }
<ASM_MODE>ixl|IXL       { return REG_IXL; }
<ASM_MODE>iy|IY         { return REG_IY; }
<ASM_MODE>iyh|IYH       { return REG_IYH; }
<ASM_MODE>iyl|IYL       { return REG_IYL; }
<ASM_MODE>[a-zA-Z]{2,4}                           { yylval.str = lib_stralloc(yytext); return OPCODE; }
<ASM_MODE,INITIAL>\.[_a-zA-Z][_a-zA-Z0-9]*        { yylval.str = lib_stralloc(yytext); return LABEL; }
<ASM_MODE,INITIAL>\.[_@\?:a-zA-Z][_@\?:a-zA-Z0-9]*        { yylval.str = lib_stralloc(yytext); return LABEL; }

<ASM_MODE>(a|A)/[ \t]*[:\n]           { if (!dont_match_reg_a) return REG_A;
                                        yylval.i = 0x0a; return H_NUMBER; }
<ASM_MODE>b|B           { return REG_B; }
<ASM_MODE>c|C           { return REG_C; }
<ASM_MODE>d|D           { return REG_D; }
<ASM_MODE>e|E           { return REG_E; }
<ASM_MODE>h|H           { return REG_H; }
<ASM_MODE>l|L           { return REG_L; }
<ASM_MODE>x|X           { return REG_X; }
<ASM_MODE>y|Y           { return REG_Y; }

<ASM_MODE>,             { return COMMA; }
<ASM_MODE>\(            { dont_match_reg_a = 1; return L_PAREN; }
<ASM_MODE>\)            { dont_match_reg_a = 0; return R_PAREN; }
<ASM_MODE>#             { dont_match_reg_a = 1; return ARG_IMMEDIATE; }
<ASM_MODE>:             { dont_match_reg_a = 0; return INST_SEP; }

<ASM_MODE,INITIAL,COND_MODE>{
	1[01]*                  { yylval.str = lib_stralloc(yytext); return B_NUMBER_GUESS; }
	%[01]+                  { yylval.i = strtol(yytext+1, NULL, 2); return B_NUMBER; }
	&[0-7]+                 { yylval.i = strtol(yytext+1, NULL, 8); return O_NUMBER; }
	[0-7]*                  { yylval.str = lib_stralloc(yytext); return O_NUMBER_GUESS; }
	\+[0-9]+                { yylval.i = strtol(yytext+1, NULL, 10); return D_NUMBER; }
	[0-9]+                  { yylval.str = lib_stralloc(yytext); return D_NUMBER_GUESS; }
	\$[0-9a-fA-F]+          { yylval.i = strtol(yytext+1, NULL, 16); return H_NUMBER; }
	[0-9a-fA-F]+            { yylval.i = strtol(yytext, NULL, 16); return H_NUMBER; }
}

<*>[^ \t]		{ return yytext[0]; }

%%

void free_buffer(void)
{
   yy_delete_buffer(my_state);
}

void make_buffer(char *str)
{
    my_state = yy_scan_buffer(str, strlen(str) + 2);
    assert(my_state);
}

static int yywrap(void)
{
    return 1;
}

