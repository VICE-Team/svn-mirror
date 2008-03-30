
/*  A Bison parser, made from mon_cli.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	H_NUMBER	258
#define	D_NUMBER	259
#define	O_NUMBER	260
#define	B_NUMBER	261
#define	COMMAND_NAME	262
#define	CONVERT_OP	263
#define	B_DATA	264
#define	TRAIL	265
#define	BAD_CMD	266
#define	MEM_OP	267
#define	IF	268
#define	MEM_COMP	269
#define	MEM_DISK	270
#define	CMD_SIDEFX	271
#define	CMD_RETURN	272
#define	CMD_BLOCK_READ	273
#define	CMD_BLOCK_WRITE	274
#define	CMD_UP	275
#define	CMD_DOWN	276
#define	CMD_LOAD	277
#define	CMD_SAVE	278
#define	CMD_VERIFY	279
#define	CMD_IGNORE	280
#define	CMD_HUNT	281
#define	CMD_FILL	282
#define	CMD_MOVE	283
#define	CMD_GOTO	284
#define	CMD_REGISTERS	285
#define	CMD_READSPACE	286
#define	CMD_WRITESPACE	287
#define	CMD_DISPLAYTYPE	288
#define	CMD_MEM_DISPLAY	289
#define	CMD_BREAK	290
#define	CMD_TRACE	291
#define	CMD_IO	292
#define	CMD_BRMON	293
#define	CMD_COMPARE	294
#define	CMD_DUMP	295
#define	CMD_UNDUMP	296
#define	CMD_EXIT	297
#define	CMD_DELETE	298
#define	CMD_CONDITION	299
#define	CMD_COMMAND	300
#define	CMD_ASSEMBLE	301
#define	CMD_DISASSEMBLE	302
#define	CMD_NEXT	303
#define	CMD_STEP	304
#define	CMD_PRINT	305
#define	CMD_DEVICE	306
#define	CMD_HELP	307
#define	CMD_WATCH	308
#define	CMD_DISK	309
#define	CMD_SYSTEM	310
#define	CMD_QUIT	311
#define	CMD_CHDIR	312
#define	CMD_LOAD_LABELS	313
#define	CMD_SAVE_LABELS	314
#define	CMD_ADD_LABEL	315
#define	CMD_DEL_LABEL	316
#define	CMD_LABEL	317
#define	L_PAREN	318
#define	R_PAREN	319
#define	ARG_IMMEDIATE	320
#define	REG_A	321
#define	REG_X	322
#define	REG_Y	323
#define	COMMA	324
#define	INST_SEP	325
#define	STRING	326
#define	FILENAME	327
#define	R_O_L	328
#define	OPCODE	329
#define	LABEL	330
#define	REGISTER	331
#define	COMPARE_OP	332
#define	DATA_TYPE	333
#define	INPUT_SPEC	334
#define	CMD_BREAKPT_ONOFF	335
#define	TOGGLE	336

#line 1 "mon_cli.y"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vice.h"
#include "types.h"
#include "asm.h"

#undef M_ADDR
#include "mon.h"

int yyerror(char *s);

/* Defined in the lexer */
extern int new_cmd, opt_asm;
extern void free_buffer(void);
extern void make_buffer(char *str);
extern int yylex(void);

#define new_mode(x,y) (LO16_TO_HI16(x)|y)


#line 24 "mon_cli.y"
typedef union {
	M_ADDR a;
	M_ADDR_RANGE arange;
        int i;
        REG_ID reg;
        CONDITIONAL cond_op;
        CONDITIONAL_NODE *cond_node;
        DATATYPE dt;
        ACTION action;
        char *str;
} YYSTYPE;
#ifndef YYDEBUG
#define YYDEBUG 1
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		226
#define	YYFLAG		-32768
#define	YYNTBASE	92

#define YYTRANSLATE(x) ((unsigned)(x) <= 336 ? yytranslate[x] : 134)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    90,
    91,    84,    82,    86,    83,     2,    85,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    88,     2,     2,
    87,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    89,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
    76,    77,    78,    79,    80,    81
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     7,    10,    12,    14,    16,    18,    20,    22,
    24,    26,    28,    31,    35,    39,    43,    46,    49,    52,
    54,    56,    59,    62,    65,    68,    70,    72,    74,    76,
    78,    82,    86,    90,    94,    97,    99,   102,   105,   108,
   110,   113,   116,   119,   123,   126,   128,   130,   132,   135,
   137,   141,   145,   148,   150,   152,   153,   158,   161,   164,
   166,   169,   171,   174,   176,   178,   179,   183,   185,   188,
   190,   192,   194,   196,   198,   202,   205,   207,   210,   212,
   215,   217,   220,   222,   225,   228,   232,   234,   238,   244,
   248,   252,   256,   258,   259,   261,   262,   264,   269,   274,
   277,   282,   287,   290,   292,   293,   295,   299,   301,   303,
   305,   307,   311,   315,   319,   323,   327,   329,   333,   335,
   339,   341,   344,   346,   348,   350,   352,   354,   358,   360,
   362,   364,   366,   369,   371,   375,   379,   383,   389,   395,
   396
};

static const short yyrhs[] = {    93,
    10,     0,    92,    93,    10,     0,    98,    10,     0,    10,
     0,    94,     0,     7,     0,   103,     0,   111,     0,   118,
     0,   104,     0,   106,     0,   107,     0,    80,   117,     0,
    25,   117,   115,     0,    28,   119,   122,     0,    39,   119,
   122,     0,    29,   122,     0,    43,   116,     0,    42,    10,
     0,    96,     0,   108,     0,    49,   115,     0,    48,   115,
     0,    20,   115,     0,    21,   115,     0,   114,     0,   110,
     0,   109,     0,   100,     0,   102,     0,    44,   117,   127,
     0,    45,   117,    71,     0,    27,   119,   129,     0,    26,
   119,   129,     0,    50,   126,     0,    52,     0,    54,    95,
     0,    55,    95,     0,     8,   126,     0,    56,     0,    57,
    95,     0,    58,    72,     0,    59,    72,     0,    60,   122,
    75,     0,    61,    75,     0,    62,     0,    11,     0,    73,
     0,    33,    78,     0,    33,     0,    97,    70,    98,     0,
    98,    70,    98,     0,    74,   133,     0,    98,     0,    97,
     0,     0,    46,   122,   101,    99,     0,    46,   122,     0,
    47,   120,     0,    47,     0,    16,    81,     0,    16,     0,
    35,   120,     0,    35,     0,    12,     0,     0,    53,   105,
   120,     0,    53,     0,    36,   120,     0,    36,     0,    37,
     0,    17,     0,    40,     0,    41,     0,    34,    78,   120,
     0,    34,   120,     0,    34,     0,    31,   123,     0,    31,
     0,    32,   123,     0,    32,     0,    51,   123,     0,    30,
     0,    30,   123,     0,    30,   112,     0,   112,    86,   113,
     0,   113,     0,    76,    87,   132,     0,   123,    88,    76,
    87,   132,     0,    22,    72,   122,     0,    23,    72,   119,
     0,    24,    72,   122,     0,   126,     0,     0,   117,     0,
     0,   132,     0,    18,   126,   126,   121,     0,    19,   126,
   126,   122,     0,   122,   122,     0,   122,    89,    82,   132,
     0,   122,    89,    83,   132,     0,   122,   121,     0,   122,
     0,     0,   124,     0,   123,    88,   124,     0,    14,     0,
    15,     0,   125,     0,   132,     0,   126,    82,   126,     0,
   126,    83,   126,     0,   126,    84,   126,     0,   126,    85,
   126,     0,    90,   126,    91,     0,   131,     0,   128,    77,
   128,     0,    76,     0,   123,    88,    76,     0,   132,     0,
   129,   130,     0,   130,     0,   132,     0,    71,     0,   132,
     0,    76,     0,   123,    88,    76,     0,     3,     0,     4,
     0,     5,     0,     6,     0,    65,   132,     0,   132,     0,
   132,    69,    67,     0,   132,    69,    68,     0,    63,   132,
    64,     0,    63,   132,    69,    67,    64,     0,    63,   132,
    64,    69,    68,     0,     0,    66,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    70,    71,    72,    73,    76,    78,    79,    80,    81,    82,
    83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
    93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
   103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
   113,   114,   115,   116,   117,   118,   119,   122,   124,   125,
   129,   130,   133,   143,   144,   147,   148,   148,   151,   152,
   155,   156,   158,   159,   162,   163,   165,   167,   170,   171,
   174,   175,   176,   177,   180,   181,   182,   185,   187,   188,
   190,   191,   195,   196,   197,   200,   201,   204,   205,   208,
   209,   210,   213,   214,   217,   218,   221,   224,   225,   228,
   229,   230,   233,   235,   236,   239,   240,   242,   243,   246,
   249,   251,   252,   253,   254,   255,   256,   259,   264,   266,
   268,   272,   273,   276,   277,   280,   281,   282,   285,   286,
   287,   288,   291,   292,   297,   302,   307,   308,   309,   310,
   311
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","H_NUMBER",
"D_NUMBER","O_NUMBER","B_NUMBER","COMMAND_NAME","CONVERT_OP","B_DATA","TRAIL",
"BAD_CMD","MEM_OP","IF","MEM_COMP","MEM_DISK","CMD_SIDEFX","CMD_RETURN","CMD_BLOCK_READ",
"CMD_BLOCK_WRITE","CMD_UP","CMD_DOWN","CMD_LOAD","CMD_SAVE","CMD_VERIFY","CMD_IGNORE",
"CMD_HUNT","CMD_FILL","CMD_MOVE","CMD_GOTO","CMD_REGISTERS","CMD_READSPACE",
"CMD_WRITESPACE","CMD_DISPLAYTYPE","CMD_MEM_DISPLAY","CMD_BREAK","CMD_TRACE",
"CMD_IO","CMD_BRMON","CMD_COMPARE","CMD_DUMP","CMD_UNDUMP","CMD_EXIT","CMD_DELETE",
"CMD_CONDITION","CMD_COMMAND","CMD_ASSEMBLE","CMD_DISASSEMBLE","CMD_NEXT","CMD_STEP",
"CMD_PRINT","CMD_DEVICE","CMD_HELP","CMD_WATCH","CMD_DISK","CMD_SYSTEM","CMD_QUIT",
"CMD_CHDIR","CMD_LOAD_LABELS","CMD_SAVE_LABELS","CMD_ADD_LABEL","CMD_DEL_LABEL",
"CMD_LABEL","L_PAREN","R_PAREN","ARG_IMMEDIATE","REG_A","REG_X","REG_Y","COMMA",
"INST_SEP","STRING","FILENAME","R_O_L","OPCODE","LABEL","REGISTER","COMPARE_OP",
"DATA_TYPE","INPUT_SPEC","CMD_BREAKPT_ONOFF","TOGGLE","'+'","'-'","'*'","'/'",
"','","'='","':'","'|'","'('","')'","command_list","meta_command","command",
"rest_of_line","display_type","assembly_instr_list","assembly_instruction","post_assemble",
"assemble","@1","disassemble","handle_read_sidefx","set_list_breakpts","opt_mem_op",
"set_list_watchpts","set_list_tracepts","no_arg_cmds","memory_display","space_mod",
"register_mod","reg_list","reg_asgn","file_cmd","opt_count","opt_brknum","breakpt_num",
"block_cmd","address_range","address_opt_range","opt_address","address","memspace",
"memloc","memaddr","expression","cond_expr","compare_operand","data_list","data_element",
"value","number","asm_operand_mode", NULL
};
#endif

static const short yyr1[] = {     0,
    92,    92,    92,    92,    93,    94,    94,    94,    94,    94,
    94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
    94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
    94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
    94,    94,    94,    94,    94,    94,    94,    95,    96,    96,
    97,    97,    98,    99,    99,   101,   100,   100,   102,   102,
   103,   103,   104,   104,   105,   105,   106,   106,   107,   107,
   108,   108,   108,   108,   109,   109,   109,   110,   110,   110,
   110,   110,   111,   111,   111,   112,   112,   113,   113,   114,
   114,   114,   115,   115,   116,   116,   117,   118,   118,   119,
   119,   119,   120,   121,   121,   122,   122,   123,   123,   124,
   125,   126,   126,   126,   126,   126,   126,   127,   128,   128,
   128,   129,   129,   130,   130,   131,   131,   131,   132,   132,
   132,   132,   133,   133,   133,   133,   133,   133,   133,   133,
   133
};

static const short yyr2[] = {     0,
     2,     3,     2,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     2,     3,     3,     3,     2,     2,     2,     1,
     1,     2,     2,     2,     2,     1,     1,     1,     1,     1,
     3,     3,     3,     3,     2,     1,     2,     2,     2,     1,
     2,     2,     2,     3,     2,     1,     1,     1,     2,     1,
     3,     3,     2,     1,     1,     0,     4,     2,     2,     1,
     2,     1,     2,     1,     1,     0,     3,     1,     2,     1,
     1,     1,     1,     1,     3,     2,     1,     2,     1,     2,
     1,     2,     1,     2,     2,     3,     1,     3,     5,     3,
     3,     3,     1,     0,     1,     0,     1,     4,     4,     2,
     4,     4,     2,     1,     0,     1,     3,     1,     1,     1,
     1,     3,     3,     3,     3,     3,     1,     3,     1,     3,
     1,     2,     1,     1,     1,     1,     1,     3,     1,     1,
     1,     1,     2,     1,     3,     3,     3,     5,     5,     0,
     1
};

static const short yydefact[] = {     0,
     6,     0,     4,    47,    62,    72,     0,     0,    94,    94,
     0,     0,     0,     0,     0,     0,     0,     0,    83,    79,
    81,    50,    77,    64,    70,    71,     0,    73,    74,     0,
    96,     0,     0,     0,    60,    94,    94,     0,     0,    36,
    66,     0,     0,    40,     0,     0,     0,     0,     0,    46,
   140,     0,     0,     0,     5,    20,     0,    29,    30,     7,
    10,    11,    12,    21,    28,    27,     8,    26,     9,   129,
   130,   131,   132,   108,   109,   127,     0,     0,    39,   117,
   126,    61,     0,     0,    24,    93,    25,     0,     0,     0,
    94,    97,     0,     0,     0,   106,   110,   111,     0,     0,
    17,     0,    85,    87,    84,    78,    80,    49,     0,    76,
   105,    63,    69,     0,    19,    18,    95,     0,     0,    56,
    59,    23,    22,    35,    82,    65,     0,    48,    37,    38,
    41,    42,    43,     0,    45,     0,     0,   141,   134,    53,
    13,     0,     1,     3,     0,     0,     0,     0,     0,     0,
   105,     0,    90,    91,    92,    14,   125,    34,   123,   124,
     0,   100,     0,    33,    15,     0,     0,     0,    75,   103,
   104,    16,   119,     0,    31,     0,   121,    32,     0,    67,
    44,     0,   133,     0,     2,   116,   128,   112,   113,   114,
   115,    98,    99,   122,     0,     0,   107,    88,    86,     0,
     0,     0,     0,    55,    54,    57,   137,     0,   135,   136,
   101,   102,     0,   120,   118,     0,     0,     0,     0,    89,
    51,    52,   139,   138,     0,     0
};

static const short yydefgoto[] = {    53,
    54,    55,   129,    56,   204,    57,   206,    58,   179,    59,
    60,    61,   127,    62,    63,    64,    65,    66,    67,   103,
   104,    68,    85,   116,    91,    69,    93,   110,   170,   111,
    95,    96,    97,    86,   175,   176,   158,   159,    80,    98,
   140
};

static const short yypact[] = {   328,
-32768,   166,-32768,-32768,   -80,-32768,   166,   166,   166,   166,
   -61,   -58,   -39,   185,   219,   219,   219,   219,    57,    33,
    33,   -27,   194,   219,   219,-32768,   219,-32768,-32768,    53,
   185,   185,   185,   219,   219,   166,   166,   166,    33,-32768,
    64,     4,     4,-32768,     4,    20,    34,   219,    15,-32768,
    61,   185,   263,    74,-32768,-32768,    86,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   166,    25,    47,-32768,
-32768,-32768,    38,    38,-32768,    47,-32768,   219,   219,   219,
   166,-32768,    12,   172,    27,-32768,-32768,-32768,    12,   219,
-32768,    21,    26,-32768,    29,-32768,-32768,-32768,   219,-32768,
   219,-32768,-32768,   219,-32768,-32768,-32768,   254,    36,   108,
-32768,-32768,-32768,    47,-32768,-32768,   219,-32768,-32768,-32768,
-32768,-32768,-32768,    44,-32768,   185,   185,-32768,    56,-32768,
-32768,   144,-32768,-32768,   122,    79,   166,   166,   166,   166,
    55,    55,-32768,-32768,-32768,-32768,-32768,    12,-32768,-32768,
   -59,-32768,   185,    12,-32768,   185,    57,    81,-32768,-32768,
-32768,-32768,-32768,    70,-32768,    82,-32768,-32768,    87,-32768,
-32768,    24,-32768,    31,-32768,-32768,-32768,    19,    19,-32768,
-32768,-32768,-32768,-32768,   185,   185,-32768,-32768,-32768,    29,
    76,    97,   254,   109,   125,-32768,   123,   129,-32768,-32768,
-32768,-32768,   185,-32768,-32768,    87,    87,   134,   146,-32768,
-32768,-32768,-32768,-32768,   212,-32768
};

static const short yypgoto[] = {-32768,
   164,-32768,    35,-32768,-32768,  -106,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    51,-32768,     9,-32768,   151,-32768,     5,   -22,    68,   126,
    18,    65,-32768,     2,-32768,    28,   128,   -96,-32768,    -2,
-32768
};


#define	YYLAST		408


static const short yytable[] = {    81,
    82,   112,   113,    79,    81,    81,    81,    81,    83,    84,
    88,    92,   121,    89,    70,    71,    72,    73,    87,    78,
    99,   100,   195,   196,    78,    78,    78,    78,    92,    92,
    92,   114,    90,    81,    81,    81,   105,   106,   107,   124,
    70,    71,    72,    73,   122,   123,    74,    75,   139,    92,
   108,    74,    75,    78,    78,    78,   125,    70,    71,    72,
    73,   194,   115,    70,    71,    72,    73,   194,    74,    75,
    74,    75,   205,   -68,    81,   126,   128,   130,   145,   131,
    81,    81,   157,   143,   151,   152,   169,   207,    81,   135,
   160,   132,   208,   154,    78,   144,   160,   209,   210,   156,
    78,    78,   149,   150,   180,   133,   178,   166,    78,   221,
   222,   167,   146,    76,   163,   177,   168,   -58,   181,   147,
   148,   149,   150,   136,   184,   137,   138,    77,   147,   148,
   149,   150,   102,   182,   183,   174,   147,   148,   149,   150,
    94,    94,    94,   101,    81,    81,    81,    81,   188,   189,
   190,   191,    94,   185,   187,   160,   201,   202,   203,   120,
    51,   160,   213,   198,    78,    78,    78,    78,    70,    71,
    72,    73,   214,   134,    70,    71,    72,    73,   216,    74,
    75,   117,   118,   119,   200,    74,    75,    70,    71,    72,
    73,   218,   211,   212,   217,   219,    70,    71,    72,    73,
   177,   223,   141,   147,   148,   149,   150,    74,    75,   224,
   220,   226,   186,   153,    94,   155,   142,   199,   192,   162,
   174,    70,    71,    72,    73,   165,   164,   197,     0,     0,
   215,     0,    74,    75,     0,     0,   171,     0,     0,   172,
     0,    76,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    77,    70,    71,    72,    73,
   161,     0,   225,     0,     0,     0,     0,    74,    75,     1,
     2,   109,     0,     4,     0,     0,   171,   193,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
     0,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,     0,     0,     0,     0,   173,
     0,     0,     0,     0,     1,     2,     0,     3,     4,     0,
     0,     0,    52,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
    22,    23,    24,    25,    26,     0,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    51,     0,     0,     0,     0,     0,    52
};

static const short yycheck[] = {     2,
    81,    24,    25,     2,     7,     8,     9,    10,     7,     8,
    72,    14,    35,    72,     3,     4,     5,     6,    10,     2,
    16,    17,    82,    83,     7,     8,     9,    10,    31,    32,
    33,    27,    72,    36,    37,    38,    19,    20,    21,    38,
     3,     4,     5,     6,    36,    37,    14,    15,    51,    52,
    78,    14,    15,    36,    37,    38,    39,     3,     4,     5,
     6,   158,    10,     3,     4,     5,     6,   164,    14,    15,
    14,    15,   179,    10,    77,    12,    73,    43,    77,    45,
    83,    84,    71,    10,    83,    84,   109,    64,    91,    75,
    93,    72,    69,    89,    77,    10,    99,    67,    68,    91,
    83,    84,    84,    85,   127,    72,    71,    87,    91,   216,
   217,    86,    88,    76,    88,   118,    88,    10,    75,    82,
    83,    84,    85,    63,    69,    65,    66,    90,    82,    83,
    84,    85,    76,   136,   137,   118,    82,    83,    84,    85,
    15,    16,    17,    18,   147,   148,   149,   150,   147,   148,
   149,   150,    27,    10,    76,   158,    76,    88,    77,    34,
    74,   164,    87,   166,   147,   148,   149,   150,     3,     4,
     5,     6,    76,    48,     3,     4,     5,     6,    70,    14,
    15,    31,    32,    33,   167,    14,    15,     3,     4,     5,
     6,    69,   195,   196,    70,    67,     3,     4,     5,     6,
   203,    68,    52,    82,    83,    84,    85,    14,    15,    64,
   213,     0,    91,    88,    89,    90,    53,   167,   151,    94,
   203,     3,     4,     5,     6,   100,    99,   163,    -1,    -1,
   203,    -1,    14,    15,    -1,    -1,   111,    -1,    -1,   114,
    -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    90,     3,     4,     5,     6,
    89,    -1,     0,    -1,    -1,    -1,    -1,    14,    15,     7,
     8,    78,    -1,    11,    -1,    -1,   151,   152,    16,    17,
    18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    -1,    39,    40,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
    58,    59,    60,    61,    62,    -1,    -1,    -1,    -1,    76,
    -1,    -1,    -1,    -1,     7,     8,    -1,    10,    11,    -1,
    -1,    -1,    80,    16,    17,    18,    19,    20,    21,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    -1,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    74,    -1,    -1,    -1,    -1,    -1,    80
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/lib/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "/usr/lib/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 4:
#line 73 "mon_cli.y"
{ new_cmd = 1; asm_mode = 0; ;
    break;}
case 6:
#line 78 "mon_cli.y"
{puts("Unsupported command"); ;
    break;}
case 13:
#line 85 "mon_cli.y"
{ switch_breakpt(yyvsp[-1].action, yyvsp[0].i); ;
    break;}
case 14:
#line 86 "mon_cli.y"
{ set_ignore_count(yyvsp[-1].i, yyvsp[0].i); ;
    break;}
case 15:
#line 87 "mon_cli.y"
{ move_memory(yyvsp[-1].arange, yyvsp[0].a); ;
    break;}
case 16:
#line 88 "mon_cli.y"
{ compare_memory(yyvsp[-1].arange, yyvsp[0].a); ;
    break;}
case 17:
#line 89 "mon_cli.y"
{ jump(yyvsp[0].a); ;
    break;}
case 18:
#line 90 "mon_cli.y"
{ delete_breakpoint(yyvsp[0].i); ;
    break;}
case 19:
#line 91 "mon_cli.y"
{ exit_mon = 1; YYACCEPT; ;
    break;}
case 22:
#line 94 "mon_cli.y"
{ instructions_step(yyvsp[0].i); ;
    break;}
case 23:
#line 95 "mon_cli.y"
{ instructions_next(yyvsp[0].i); ;
    break;}
case 24:
#line 96 "mon_cli.y"
{ stack_up(yyvsp[0].i); ;
    break;}
case 25:
#line 97 "mon_cli.y"
{ stack_down(yyvsp[0].i); ;
    break;}
case 31:
#line 103 "mon_cli.y"
{ set_brkpt_condition(yyvsp[-1].i, yyvsp[0].cond_node); ;
    break;}
case 32:
#line 104 "mon_cli.y"
{ set_breakpt_command(yyvsp[-1].i, yyvsp[0].str); ;
    break;}
case 33:
#line 105 "mon_cli.y"
{ fill_memory(yyvsp[-1].arange, yyvsp[0].str); ;
    break;}
case 34:
#line 106 "mon_cli.y"
{ hunt_memory(yyvsp[-1].arange, yyvsp[0].str); ;
    break;}
case 35:
#line 107 "mon_cli.y"
{ fprintf(mon_output, "\t%d\n",yyvsp[0].i); ;
    break;}
case 36:
#line 108 "mon_cli.y"
{ print_help(); ;
    break;}
case 37:
#line 109 "mon_cli.y"
{ printf("DISK COMMAND: %s\n",yyvsp[0].str); ;
    break;}
case 38:
#line 110 "mon_cli.y"
{ printf("SYSTEM COMMAND: %s\n",yyvsp[0].str); ;
    break;}
case 39:
#line 111 "mon_cli.y"
{ print_convert(yyvsp[0].i); ;
    break;}
case 40:
#line 112 "mon_cli.y"
{ printf("Quit.\n"); exit(-1); exit(0); ;
    break;}
case 41:
#line 113 "mon_cli.y"
{ change_dir(yyvsp[0].str); ;
    break;}
case 42:
#line 114 "mon_cli.y"
{ mon_load_symbols(yyvsp[0].str, e_comp_space); ;
    break;}
case 43:
#line 115 "mon_cli.y"
{ mon_save_symbols(yyvsp[0].str, e_comp_space); ;
    break;}
case 44:
#line 116 "mon_cli.y"
{ add_name_to_symbol_table(e_comp_space, yyvsp[0].str, yyvsp[-1].a); ;
    break;}
case 45:
#line 117 "mon_cli.y"
{ remove_name_from_symbol_table(e_comp_space, yyvsp[0].str); ;
    break;}
case 46:
#line 118 "mon_cli.y"
{ print_symbol_table(e_comp_space); ;
    break;}
case 47:
#line 119 "mon_cli.y"
{ YYABORT; ;
    break;}
case 48:
#line 122 "mon_cli.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 49:
#line 124 "mon_cli.y"
{ default_datatype = yyvsp[0].dt; ;
    break;}
case 50:
#line 125 "mon_cli.y"
{ fprintf(mon_output, "Default datatype is %s\n", 
                                        datatype_string[default_datatype]); ;
    break;}
case 53:
#line 133 "mon_cli.y"
{ yyval.i = 0; 
                               if (yyvsp[-1].str) {
                                  mon_assemble_instr(yyvsp[-1].str, yyvsp[0].i);
                               } else {
                                  new_cmd = 1;
                                  asm_mode = 0;
                               }
                               opt_asm = 0;
                             ;
    break;}
case 55:
#line 144 "mon_cli.y"
{ asm_mode = 0; ;
    break;}
case 56:
#line 147 "mon_cli.y"
{ start_assemble_mode(yyvsp[0].a, NULL); ;
    break;}
case 58:
#line 148 "mon_cli.y"
{ start_assemble_mode(yyvsp[0].a, NULL); ;
    break;}
case 59:
#line 151 "mon_cli.y"
{ disassemble_lines(yyvsp[0].arange); ;
    break;}
case 60:
#line 152 "mon_cli.y"
{ disassemble_lines(new_range(bad_addr,bad_addr)); ;
    break;}
case 61:
#line 155 "mon_cli.y"
{ sidefx = ((yyvsp[0].action==e_TOGGLE)?(sidefx^1):yyvsp[0].action); ;
    break;}
case 62:
#line 156 "mon_cli.y"
{ fprintf(mon_output, "sidefx %d\n",sidefx); ;
    break;}
case 63:
#line 158 "mon_cli.y"
{ add_breakpoint(yyvsp[0].arange, FALSE, FALSE, FALSE); ;
    break;}
case 64:
#line 159 "mon_cli.y"
{ print_breakpts(); ;
    break;}
case 65:
#line 162 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 66:
#line 163 "mon_cli.y"
{ yyval.i = e_load_store; ;
    break;}
case 67:
#line 165 "mon_cli.y"
{ add_breakpoint(yyvsp[0].arange, FALSE, 
                              (yyvsp[-1].i == e_load || yyvsp[-1].i == e_load_store), (yyvsp[-1].i == e_store || yyvsp[-1].i == e_load_store)); ;
    break;}
case 68:
#line 167 "mon_cli.y"
{ print_breakpts(); ;
    break;}
case 69:
#line 170 "mon_cli.y"
{ add_breakpoint(yyvsp[0].arange, TRUE, FALSE, FALSE); ;
    break;}
case 70:
#line 171 "mon_cli.y"
{ print_breakpts(); ;
    break;}
case 71:
#line 174 "mon_cli.y"
{ fprintf(mon_output, "Display IO registers\n"); ;
    break;}
case 72:
#line 175 "mon_cli.y"
{ fprintf(mon_output, "Continue until RTS/RTI\n"); ;
    break;}
case 73:
#line 176 "mon_cli.y"
{ puts("Dump machine state."); ;
    break;}
case 74:
#line 177 "mon_cli.y"
{ puts("Undump machine state."); ;
    break;}
case 75:
#line 180 "mon_cli.y"
{ display_memory(yyvsp[-1].dt, yyvsp[0].arange); ;
    break;}
case 76:
#line 181 "mon_cli.y"
{ display_memory(0, yyvsp[0].arange); ;
    break;}
case 77:
#line 182 "mon_cli.y"
{ display_memory(0,new_range(bad_addr,bad_addr)); ;
    break;}
case 78:
#line 185 "mon_cli.y"
{ fprintf(mon_output, "Setting default readspace to %s\n",SPACESTRING(yyvsp[0].i)); 
                                      default_readspace = yyvsp[0].i; ;
    break;}
case 79:
#line 187 "mon_cli.y"
{ fprintf(mon_output, "Default readspace is %s\n",SPACESTRING(default_readspace)); ;
    break;}
case 80:
#line 188 "mon_cli.y"
{ fprintf(mon_output, "Setting default writespace to %s\n", SPACESTRING(yyvsp[0].i)); 
                                       default_writespace = yyvsp[0].i; ;
    break;}
case 81:
#line 190 "mon_cli.y"
{ fprintf(mon_output,"Default writespace is %s\n",SPACESTRING(default_writespace)); ;
    break;}
case 82:
#line 191 "mon_cli.y"
{ fprintf(mon_output,"Setting default device to %s\n", SPACESTRING(yyvsp[0].i)); 
                                 default_readspace = default_writespace = yyvsp[0].i; ;
    break;}
case 83:
#line 195 "mon_cli.y"
{ print_registers(e_default_space); ;
    break;}
case 84:
#line 196 "mon_cli.y"
{ print_registers(yyvsp[0].i); ;
    break;}
case 88:
#line 204 "mon_cli.y"
{ set_reg_val(yyvsp[-2].reg, default_writespace, yyvsp[0].i); ;
    break;}
case 89:
#line 205 "mon_cli.y"
{ set_reg_val(yyvsp[-2].reg, yyvsp[-4].i, yyvsp[0].i); ;
    break;}
case 90:
#line 208 "mon_cli.y"
{ mon_load_file(yyvsp[-1].str,yyvsp[0].a); ;
    break;}
case 91:
#line 209 "mon_cli.y"
{ mon_save_file(yyvsp[-1].str,yyvsp[0].arange); ;
    break;}
case 92:
#line 210 "mon_cli.y"
{ mon_verify_file(yyvsp[-1].str,yyvsp[0].a); ;
    break;}
case 93:
#line 213 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 94:
#line 214 "mon_cli.y"
{ yyval.i = -1; ;
    break;}
case 95:
#line 217 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 96:
#line 218 "mon_cli.y"
{ yyval.i = -1; ;
    break;}
case 97:
#line 221 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 98:
#line 224 "mon_cli.y"
{ block_cmd(0,yyvsp[-2].i,yyvsp[-1].i,yyvsp[0].a); ;
    break;}
case 99:
#line 225 "mon_cli.y"
{ block_cmd(1,yyvsp[-2].i,yyvsp[-1].i,yyvsp[0].a); ;
    break;}
case 100:
#line 228 "mon_cli.y"
{ yyval.arange = new_range(yyvsp[-1].a,yyvsp[0].a); ;
    break;}
case 101:
#line 229 "mon_cli.y"
{ yyval.arange = new_range(yyvsp[-3].a,new_addr(e_default_space,addr_location(yyvsp[-3].a)+yyvsp[0].i)); ;
    break;}
case 102:
#line 230 "mon_cli.y"
{ yyval.arange = new_range(yyvsp[-3].a,new_addr(e_default_space,addr_location(yyvsp[-3].a)-yyvsp[0].i)); ;
    break;}
case 103:
#line 233 "mon_cli.y"
{ yyval.arange = new_range(yyvsp[-1].a,yyvsp[0].a); ;
    break;}
case 104:
#line 235 "mon_cli.y"
{ yyval.a = yyvsp[0].a; ;
    break;}
case 105:
#line 236 "mon_cli.y"
{ yyval.a = bad_addr; ;
    break;}
case 106:
#line 239 "mon_cli.y"
{ yyval.a = new_addr(e_default_space,yyvsp[0].i); if (opt_asm) new_cmd = asm_mode = 1; ;
    break;}
case 107:
#line 240 "mon_cli.y"
{ yyval.a = new_addr(yyvsp[-2].i,yyvsp[0].i); if (opt_asm) new_cmd = asm_mode = 1; ;
    break;}
case 108:
#line 242 "mon_cli.y"
{ yyval.i = e_comp_space; ;
    break;}
case 109:
#line 243 "mon_cli.y"
{ yyval.i = e_disk_space; ;
    break;}
case 110:
#line 246 "mon_cli.y"
{ yyval.i = check_addr_limits(yyvsp[0].i); ;
    break;}
case 111:
#line 249 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 112:
#line 251 "mon_cli.y"
{ yyval.i = yyvsp[-2].i + yyvsp[0].i; ;
    break;}
case 113:
#line 252 "mon_cli.y"
{ yyval.i = yyvsp[-2].i - yyvsp[0].i; ;
    break;}
case 114:
#line 253 "mon_cli.y"
{ yyval.i = yyvsp[-2].i * yyvsp[0].i; ;
    break;}
case 115:
#line 254 "mon_cli.y"
{ yyval.i = (yyvsp[0].i) ? (yyvsp[-2].i / yyvsp[0].i) : 1; ;
    break;}
case 116:
#line 255 "mon_cli.y"
{ yyval.i = yyvsp[-1].i; ;
    break;}
case 117:
#line 256 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 118:
#line 259 "mon_cli.y"
{
              yyval.cond_node = new_cond;
              yyval.cond_node->child1 = yyvsp[-2].cond_node; yyval.cond_node->child2 = yyvsp[0].cond_node; yyval.cond_node->operation = yyvsp[-1].cond_op; ;
    break;}
case 119:
#line 264 "mon_cli.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV; 
                            yyval.cond_node->reg_num = yyvsp[0].reg; yyval.cond_node->is_reg = default_readspace; ;
    break;}
case 120:
#line 266 "mon_cli.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV; 
                            yyval.cond_node->reg_num = yyvsp[0].reg; yyval.cond_node->is_reg = yyvsp[-2].i; ;
    break;}
case 121:
#line 268 "mon_cli.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV;
                            yyval.cond_node->value = yyvsp[0].i; yyval.cond_node->is_reg = 0; ;
    break;}
case 124:
#line 276 "mon_cli.y"
{ add_number_to_buffer(yyvsp[0].i); ;
    break;}
case 125:
#line 277 "mon_cli.y"
{ add_string_to_buffer(yyvsp[0].str); ;
    break;}
case 126:
#line 280 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 127:
#line 281 "mon_cli.y"
{ yyval.i = get_reg_val(default_readspace, yyvsp[0].reg); ;
    break;}
case 128:
#line 282 "mon_cli.y"
{ yyval.i = get_reg_val(yyvsp[-2].i, yyvsp[0].reg); ;
    break;}
case 129:
#line 285 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 130:
#line 286 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 131:
#line 287 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 132:
#line 288 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 133:
#line 291 "mon_cli.y"
{ yyval.i = new_mode(IMMEDIATE,yyvsp[0].i); ;
    break;}
case 134:
#line 292 "mon_cli.y"
{ if (yyvsp[0].i < 0x100)
                              yyval.i = new_mode(ZERO_PAGE,yyvsp[0].i);
                           else
                              yyval.i = new_mode(ABSOLUTE,yyvsp[0].i);
                         ;
    break;}
case 135:
#line 297 "mon_cli.y"
{ if (yyvsp[-2].i < 0x100)
                                           yyval.i = new_mode(ZERO_PAGE_X,yyvsp[-2].i);
                                        else
                                           yyval.i = new_mode(ABSOLUTE_X,yyvsp[-2].i);
                                      ;
    break;}
case 136:
#line 302 "mon_cli.y"
{ if (yyvsp[-2].i < 0x100)
                                           yyval.i = new_mode(ZERO_PAGE_Y,yyvsp[-2].i);
                                        else
                                           yyval.i = new_mode(ABSOLUTE_Y,yyvsp[-2].i);
                                      ;
    break;}
case 137:
#line 307 "mon_cli.y"
{ yyval.i = new_mode(ABS_INDIRECT,yyvsp[-1].i); ;
    break;}
case 138:
#line 308 "mon_cli.y"
{ yyval.i = new_mode(INDIRECT_X,yyvsp[-3].i); ;
    break;}
case 139:
#line 309 "mon_cli.y"
{ yyval.i = new_mode(INDIRECT_Y,yyvsp[-3].i); ;
    break;}
case 140:
#line 310 "mon_cli.y"
{ yyval.i = new_mode(IMPLIED,0); ;
    break;}
case 141:
#line 311 "mon_cli.y"
{ yyval.i = new_mode(ACCUMULATOR,0); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "/usr/lib/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 315 "mon_cli.y"
 

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

