
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
#define	CMD_SIDEFX	269
#define	CMD_RETURN	270
#define	CMD_BLOCK_READ	271
#define	CMD_BLOCK_WRITE	272
#define	CMD_UP	273
#define	CMD_DOWN	274
#define	CMD_LOAD	275
#define	CMD_SAVE	276
#define	CMD_VERIFY	277
#define	CMD_IGNORE	278
#define	CMD_HUNT	279
#define	CMD_FILL	280
#define	CMD_MOVE	281
#define	CMD_GOTO	282
#define	CMD_REGISTERS	283
#define	CMD_READSPACE	284
#define	CMD_WRITESPACE	285
#define	CMD_DISPLAYTYPE	286
#define	CMD_MEM_DISPLAY	287
#define	CMD_BREAK	288
#define	CMD_TRACE	289
#define	CMD_IO	290
#define	CMD_BRMON	291
#define	CMD_COMPARE	292
#define	CMD_DUMP	293
#define	CMD_UNDUMP	294
#define	CMD_EXIT	295
#define	CMD_DELETE	296
#define	CMD_CONDITION	297
#define	CMD_COMMAND	298
#define	CMD_ASSEMBLE	299
#define	CMD_DISASSEMBLE	300
#define	CMD_NEXT	301
#define	CMD_STEP	302
#define	CMD_PRINT	303
#define	CMD_DEVICE	304
#define	CMD_HELP	305
#define	CMD_WATCH	306
#define	CMD_DISK	307
#define	CMD_SYSTEM	308
#define	CMD_QUIT	309
#define	CMD_CHDIR	310
#define	STRING	311
#define	FILENAME	312
#define	R_O_L	313
#define	ASM_LINE	314
#define	REGISTER	315
#define	COMPARE_OP	316
#define	DATA_TYPE	317
#define	INPUT_SPEC	318
#define	CMD_BREAKPT_ONOFF	319
#define	TOGGLE	320

#line 1 "mon_cli.y"

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


#line 19 "mon_cli.y"
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



#define	YYFINAL		181
#define	YYFLAG		-32768
#define	YYNTBASE	78

#define YYTRANSLATE(x) ((unsigned)(x) <= 320 ? yytranslate[x] : 115)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    76,
    77,    68,    66,    70,    67,     2,    69,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    73,     2,     2,
    71,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    74,    75,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    72,     2,     2,     2,     2,     2,     2,
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
    56,    57,    58,    59,    60,    61,    62,    63,    64,    65
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     7,    10,    12,    14,    16,    18,    20,    22,
    24,    26,    28,    31,    35,    39,    43,    46,    49,    52,
    54,    56,    59,    62,    65,    68,    70,    72,    74,    76,
    78,    82,    86,    90,    94,    97,    99,   102,   105,   108,
   110,   113,   115,   117,   120,   122,   126,   129,   132,   134,
   137,   139,   142,   144,   146,   147,   151,   153,   156,   158,
   160,   162,   164,   166,   170,   173,   175,   178,   180,   183,
   185,   188,   190,   193,   196,   200,   202,   206,   210,   214,
   218,   220,   221,   223,   224,   226,   231,   236,   239,   244,
   249,   252,   254,   255,   257,   261,   263,   265,   267,   269,
   273,   277,   281,   285,   289,   291,   295,   297,   299,   302,
   304,   306,   308,   310,   312,   314,   316,   318
};

static const short yyrhs[] = {    79,
    10,     0,    78,    79,    10,     0,    59,    10,     0,    10,
     0,    80,     0,     7,     0,    85,     0,    93,     0,   100,
     0,    86,     0,    88,     0,    89,     0,    64,    99,     0,
    23,    99,    97,     0,    26,   101,   104,     0,    37,   101,
   104,     0,    27,   104,     0,    41,    98,     0,    40,    10,
     0,    82,     0,    90,     0,    47,    97,     0,    46,    97,
     0,    18,    97,     0,    19,    97,     0,    96,     0,    92,
     0,    91,     0,    83,     0,    84,     0,    42,    99,   109,
     0,    43,    99,    56,     0,    25,   101,   111,     0,    24,
   101,   111,     0,    48,   108,     0,    50,     0,    52,    81,
     0,    53,    81,     0,     8,   108,     0,    54,     0,    55,
    81,     0,    11,     0,    58,     0,    31,    62,     0,    31,
     0,    44,   104,    81,     0,    44,   104,     0,    45,   102,
     0,    45,     0,    14,    65,     0,    14,     0,    33,   102,
     0,    33,     0,    12,     0,     0,    51,    87,   102,     0,
    51,     0,    34,   102,     0,    34,     0,    35,     0,    15,
     0,    38,     0,    39,     0,    32,    62,   102,     0,    32,
   102,     0,    32,     0,    29,   105,     0,    29,     0,    30,
   105,     0,    30,     0,    49,   105,     0,    28,     0,    28,
   105,     0,    28,    94,     0,    94,    70,    95,     0,    95,
     0,    60,    71,   114,     0,    20,    57,   104,     0,    21,
    57,   101,     0,    22,    57,   104,     0,   108,     0,     0,
    99,     0,     0,   114,     0,    16,   108,   108,   103,     0,
    17,   108,   108,   104,     0,   104,   104,     0,   104,    72,
    66,   114,     0,   104,    72,    67,   114,     0,   104,   103,
     0,   104,     0,     0,   106,     0,   105,    73,   106,     0,
    74,     0,    75,     0,   107,     0,   114,     0,   108,    66,
   108,     0,   108,    67,   108,     0,   108,    68,   108,     0,
   108,    69,   108,     0,    76,   108,    77,     0,   113,     0,
   110,    61,   110,     0,    60,     0,   114,     0,   111,   112,
     0,   112,     0,   114,     0,    56,     0,   114,     0,    60,
     0,     3,     0,     4,     0,     5,     0,     6,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    62,    63,    64,    72,    75,    77,    78,    79,    80,    81,
    82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
    92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
   102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
   112,   113,   116,   118,   119,   123,   124,   127,   128,   131,
   132,   134,   135,   138,   139,   141,   143,   146,   147,   150,
   151,   152,   153,   156,   157,   158,   161,   163,   164,   166,
   167,   171,   172,   173,   176,   177,   180,   183,   184,   185,
   188,   189,   192,   193,   196,   199,   200,   203,   204,   205,
   208,   210,   211,   214,   215,   217,   218,   221,   224,   226,
   227,   228,   229,   230,   231,   234,   239,   241,   245,   246,
   249,   250,   253,   254,   256,   257,   258,   259
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","H_NUMBER",
"D_NUMBER","O_NUMBER","B_NUMBER","COMMAND_NAME","CONVERT_OP","B_DATA","TRAIL",
"BAD_CMD","MEM_OP","IF","CMD_SIDEFX","CMD_RETURN","CMD_BLOCK_READ","CMD_BLOCK_WRITE",
"CMD_UP","CMD_DOWN","CMD_LOAD","CMD_SAVE","CMD_VERIFY","CMD_IGNORE","CMD_HUNT",
"CMD_FILL","CMD_MOVE","CMD_GOTO","CMD_REGISTERS","CMD_READSPACE","CMD_WRITESPACE",
"CMD_DISPLAYTYPE","CMD_MEM_DISPLAY","CMD_BREAK","CMD_TRACE","CMD_IO","CMD_BRMON",
"CMD_COMPARE","CMD_DUMP","CMD_UNDUMP","CMD_EXIT","CMD_DELETE","CMD_CONDITION",
"CMD_COMMAND","CMD_ASSEMBLE","CMD_DISASSEMBLE","CMD_NEXT","CMD_STEP","CMD_PRINT",
"CMD_DEVICE","CMD_HELP","CMD_WATCH","CMD_DISK","CMD_SYSTEM","CMD_QUIT","CMD_CHDIR",
"STRING","FILENAME","R_O_L","ASM_LINE","REGISTER","COMPARE_OP","DATA_TYPE","INPUT_SPEC",
"CMD_BREAKPT_ONOFF","TOGGLE","'+'","'-'","'*'","'/'","','","'='","'|'","':'",
"'c'","'d'","'('","')'","command_list","meta_command","command","rest_of_line",
"display_type","assemble","disassemble","handle_read_sidefx","set_list_breakpts",
"opt_mem_op","set_list_watchpts","set_list_tracepts","no_arg_cmds","memory_display",
"space_mod","register_mod","reg_list","reg_asgn","file_cmd","opt_count","opt_brknum",
"breakpt_num","block_cmd","address_range","address_opt_range","opt_address",
"address","memspace","memloc","memaddr","expression","cond_expr","compare_operand",
"data_list","data_element","value","number", NULL
};
#endif

static const short yyr1[] = {     0,
    78,    78,    78,    78,    79,    80,    80,    80,    80,    80,
    80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
    80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
    80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
    80,    80,    81,    82,    82,    83,    83,    84,    84,    85,
    85,    86,    86,    87,    87,    88,    88,    89,    89,    90,
    90,    90,    90,    91,    91,    91,    92,    92,    92,    92,
    92,    93,    93,    93,    94,    94,    95,    96,    96,    96,
    97,    97,    98,    98,    99,   100,   100,   101,   101,   101,
   102,   103,   103,   104,   104,   105,   105,   106,   107,   108,
   108,   108,   108,   108,   108,   109,   110,   110,   111,   111,
   112,   112,   113,   113,   114,   114,   114,   114
};

static const short yyr2[] = {     0,
     2,     3,     2,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     2,     3,     3,     3,     2,     2,     2,     1,
     1,     2,     2,     2,     2,     1,     1,     1,     1,     1,
     3,     3,     3,     3,     2,     1,     2,     2,     2,     1,
     2,     1,     1,     2,     1,     3,     2,     2,     1,     2,
     1,     2,     1,     1,     0,     3,     1,     2,     1,     1,
     1,     1,     1,     3,     2,     1,     2,     1,     2,     1,
     2,     1,     2,     2,     3,     1,     3,     3,     3,     3,
     1,     0,     1,     0,     1,     4,     4,     2,     4,     4,
     2,     1,     0,     1,     3,     1,     1,     1,     1,     3,
     3,     3,     3,     3,     1,     3,     1,     1,     2,     1,
     1,     1,     1,     1,     1,     1,     1,     1
};

static const short yydefact[] = {     0,
     6,     0,     4,    42,    51,    61,     0,     0,    82,    82,
     0,     0,     0,     0,     0,     0,     0,     0,    72,    68,
    70,    45,    66,    53,    59,    60,     0,    62,    63,     0,
    84,     0,     0,     0,    49,    82,    82,     0,     0,    36,
    55,     0,     0,    40,     0,     0,     0,     0,     0,     5,
    20,    29,    30,     7,    10,    11,    12,    21,    28,    27,
     8,    26,     9,   115,   116,   117,   118,   114,     0,    39,
   105,   113,    50,     0,     0,    24,    81,    25,     0,     0,
     0,    82,    85,    96,    97,     0,     0,     0,    94,    98,
    99,     0,     0,    17,     0,    74,    76,    73,    67,    69,
    44,     0,    65,    93,    52,    58,     0,    19,    18,    83,
     0,     0,    47,    48,    23,    22,    35,    71,    54,     0,
    43,    37,    38,    41,     3,    13,     0,     1,     0,     0,
     0,     0,     0,    93,     0,    78,    79,    80,    14,   112,
    34,   110,   111,     0,    88,     0,    33,    15,     0,     0,
    64,    91,    92,    16,   107,    31,     0,   108,    32,    46,
    56,     2,   104,   100,   101,   102,   103,    86,    87,   109,
     0,     0,    95,    77,    75,     0,    89,    90,   106,     0,
     0
};

static const short yydefgoto[] = {    48,
    49,    50,   122,    51,    52,    53,    54,    55,   120,    56,
    57,    58,    59,    60,    61,    96,    97,    62,    76,   109,
    82,    63,    86,   103,   152,   104,    88,    89,    90,    77,
   156,   157,   141,   142,    71,    91
};

static const short yypact[] = {   258,
-32768,    15,-32768,-32768,   -64,-32768,    15,    15,    15,    15,
   -18,   -16,    -9,   151,    97,    97,    97,    97,   -28,   -58,
   -58,    -4,    91,    97,    97,-32768,    97,-32768,-32768,    55,
   151,   151,   151,    97,    97,    15,    15,    15,   -58,-32768,
    16,    10,    10,-32768,    10,    59,   151,   207,    60,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    15,   113,
-32768,-32768,-32768,    46,    46,-32768,   113,-32768,    97,    97,
    97,    15,-32768,-32768,-32768,   187,   137,     6,-32768,-32768,
-32768,   187,    97,-32768,    11,     4,-32768,-32768,-32768,-32768,
-32768,    97,-32768,    97,-32768,-32768,    97,-32768,-32768,-32768,
   156,    25,    10,-32768,-32768,-32768,   113,-32768,-32768,    97,
-32768,-32768,-32768,-32768,-32768,-32768,    75,-32768,   117,    15,
    15,    15,    15,    50,    50,-32768,-32768,-32768,-32768,-32768,
   187,-32768,-32768,   -23,-32768,   151,   187,-32768,   151,    33,
-32768,-32768,-32768,-32768,-32768,-32768,    38,-32768,-32768,-32768,
-32768,-32768,-32768,    -8,    -8,-32768,-32768,-32768,-32768,-32768,
   151,   151,-32768,-32768,-32768,   156,-32768,-32768,-32768,   104,
-32768
};

static const short yypgoto[] = {-32768,
    63,-32768,    14,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   -43,-32768,     1,-32768,
    31,-32768,   -14,   -10,   -26,    71,     3,   -25,-32768,     2,
-32768,   -56,    34,  -114,-32768,    -2
};


#define	YYLAST		322


static const short yytable[] = {    72,
    73,    92,    93,    70,    72,    72,    72,    72,    74,    75,
    78,    83,   107,   105,   106,    84,    85,    64,    65,    66,
    67,    98,    99,   100,   114,   -57,   170,   119,    83,    83,
    83,    95,   170,    72,    72,    72,   115,   116,    79,   117,
    80,   118,   171,   172,    83,    84,    85,    81,    64,    65,
    66,    67,    64,    65,    66,    67,   123,   101,   124,   132,
   133,   110,   111,   112,   108,   137,    72,   121,   125,   128,
   129,    72,    72,   150,    68,   134,   135,   126,   146,    72,
   159,   149,   139,   143,   162,    87,    87,    87,    94,   143,
    69,   151,    95,    64,    65,    66,    67,    87,   176,    64,
    65,    66,    67,   181,   113,    68,   175,   168,   158,   161,
   127,   130,   131,   132,   133,   130,   131,   132,   133,   179,
   173,    69,     0,    84,    85,   147,   160,    72,    72,    72,
    72,   164,   165,   166,   167,     0,     0,     0,   143,    64,
    65,    66,    67,     0,   143,     0,   174,     0,     0,   136,
    87,   138,   102,    64,    65,    66,    67,   145,    64,    65,
    66,    67,     0,   148,    84,    85,     0,     0,   177,   178,
    84,    85,     0,   158,   153,     0,     0,   154,   130,   131,
   132,   133,   130,   131,   132,   133,     0,     0,     0,    64,
    65,    66,    67,   163,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   153,   169,   180,     0,   144,     0,
    84,    85,     0,     1,     2,   155,     0,     4,     0,     0,
     5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
    25,    26,   140,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
    44,    45,     0,     0,     1,     2,     0,     3,     4,     0,
    47,     5,     6,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    26,     0,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
    43,    44,    45,     0,     0,     0,    46,     0,     0,     0,
     0,    47
};

static const short yycheck[] = {     2,
    65,    16,    17,     2,     7,     8,     9,    10,     7,     8,
    10,    14,    27,    24,    25,    74,    75,     3,     4,     5,
     6,    19,    20,    21,    35,    10,   141,    12,    31,    32,
    33,    60,   147,    36,    37,    38,    36,    37,    57,    38,
    57,    39,    66,    67,    47,    74,    75,    57,     3,     4,
     5,     6,     3,     4,     5,     6,    43,    62,    45,    68,
    69,    31,    32,    33,    10,    80,    69,    58,    10,    10,
    69,    74,    75,    70,    60,    74,    75,    47,    73,    82,
    56,    71,    82,    86,    10,    15,    16,    17,    18,    92,
    76,   102,    60,     3,     4,     5,     6,    27,    61,     3,
     4,     5,     6,     0,    34,    60,   150,   134,   111,   120,
    48,    66,    67,    68,    69,    66,    67,    68,    69,   176,
   146,    76,    -1,    74,    75,    92,   113,   130,   131,   132,
   133,   130,   131,   132,   133,    -1,    -1,    -1,   141,     3,
     4,     5,     6,    -1,   147,    -1,   149,    -1,    -1,    79,
    80,    81,    62,     3,     4,     5,     6,    87,     3,     4,
     5,     6,    -1,    93,    74,    75,    -1,    -1,   171,   172,
    74,    75,    -1,   176,   104,    -1,    -1,   107,    66,    67,
    68,    69,    66,    67,    68,    69,    -1,    -1,    -1,     3,
     4,     5,     6,    77,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   134,   135,     0,    -1,    72,    -1,
    74,    75,    -1,     7,     8,    60,    -1,    11,    -1,    -1,
    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    56,    37,    38,    39,    40,    41,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
    54,    55,    -1,    -1,     7,     8,    -1,    10,    11,    -1,
    64,    14,    15,    16,    17,    18,    19,    20,    21,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    -1,    37,    38,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    -1,    -1,    -1,    59,    -1,    -1,    -1,
    -1,    64
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

case 3:
#line 64 "mon_cli.y"
{ yyval.i = 0; 
                               if (yyvsp[-1].str) {
                                  assemble_line(yyvsp[-1].str);
                               } else {
                                  new_cmd = 1;
                                  end_assemble_mode();
                               }
                             ;
    break;}
case 4:
#line 72 "mon_cli.y"
{ new_cmd = 1; end_assemble_mode(); ;
    break;}
case 6:
#line 77 "mon_cli.y"
{puts("Unsupported command"); ;
    break;}
case 13:
#line 84 "mon_cli.y"
{ switch_breakpt(yyvsp[-1].action, yyvsp[0].i); ;
    break;}
case 14:
#line 85 "mon_cli.y"
{ set_ignore_count(yyvsp[-1].i, yyvsp[0].i); ;
    break;}
case 15:
#line 86 "mon_cli.y"
{ move_memory(yyvsp[-1].arange, yyvsp[0].a); ;
    break;}
case 16:
#line 87 "mon_cli.y"
{ compare_memory(yyvsp[-1].arange, yyvsp[0].a); ;
    break;}
case 17:
#line 88 "mon_cli.y"
{ jump(yyvsp[0].a); ;
    break;}
case 18:
#line 89 "mon_cli.y"
{ delete_breakpoint(yyvsp[0].i); ;
    break;}
case 19:
#line 90 "mon_cli.y"
{ exit_mon = 1; YYACCEPT; ;
    break;}
case 22:
#line 93 "mon_cli.y"
{ instructions_step(yyvsp[0].i); ;
    break;}
case 23:
#line 94 "mon_cli.y"
{ instructions_next(yyvsp[0].i); ;
    break;}
case 24:
#line 95 "mon_cli.y"
{ stack_up(yyvsp[0].i); ;
    break;}
case 25:
#line 96 "mon_cli.y"
{ stack_down(yyvsp[0].i); ;
    break;}
case 31:
#line 102 "mon_cli.y"
{ set_brkpt_condition(yyvsp[-1].i, yyvsp[0].cond_node); ;
    break;}
case 32:
#line 103 "mon_cli.y"
{ set_breakpt_command(yyvsp[-1].i, yyvsp[0].str); ;
    break;}
case 33:
#line 104 "mon_cli.y"
{ fill_memory(yyvsp[-1].arange, yyvsp[0].str); ;
    break;}
case 34:
#line 105 "mon_cli.y"
{ hunt_memory(yyvsp[-1].arange, yyvsp[0].str); ;
    break;}
case 35:
#line 106 "mon_cli.y"
{ fprintf(mon_output, "\t%d\n",yyvsp[0].i); ;
    break;}
case 36:
#line 107 "mon_cli.y"
{ print_help(); ;
    break;}
case 37:
#line 108 "mon_cli.y"
{ printf("DISK COMMAND: %s\n",yyvsp[0].str); ;
    break;}
case 38:
#line 109 "mon_cli.y"
{ printf("SYSTEM COMMAND: %s\n",yyvsp[0].str); ;
    break;}
case 39:
#line 110 "mon_cli.y"
{ print_convert(yyvsp[0].i); ;
    break;}
case 40:
#line 111 "mon_cli.y"
{ printf("Quit.\n"); exit(-1); exit(0); ;
    break;}
case 41:
#line 112 "mon_cli.y"
{ change_dir(yyvsp[0].str); ;
    break;}
case 42:
#line 113 "mon_cli.y"
{ YYABORT; ;
    break;}
case 43:
#line 116 "mon_cli.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 44:
#line 118 "mon_cli.y"
{ default_datatype = yyvsp[0].dt; ;
    break;}
case 45:
#line 119 "mon_cli.y"
{ fprintf(mon_output, "Default datatype is %s\n", 
                                        datatype_string[default_datatype]); ;
    break;}
case 46:
#line 123 "mon_cli.y"
{ start_assemble_mode(yyvsp[-1].a, yyvsp[0].str); ;
    break;}
case 47:
#line 124 "mon_cli.y"
{ start_assemble_mode(yyvsp[0].a, NULL); ;
    break;}
case 48:
#line 127 "mon_cli.y"
{ disassemble_lines(yyvsp[0].arange); ;
    break;}
case 49:
#line 128 "mon_cli.y"
{ disassemble_lines(new_range(bad_addr,bad_addr)); ;
    break;}
case 50:
#line 131 "mon_cli.y"
{ sidefx = ((yyvsp[0].action==e_TOGGLE)?(sidefx^1):yyvsp[0].action); ;
    break;}
case 51:
#line 132 "mon_cli.y"
{ fprintf(mon_output, "sidefx %d\n",sidefx); ;
    break;}
case 52:
#line 134 "mon_cli.y"
{ add_breakpoint(yyvsp[0].arange, FALSE, FALSE, FALSE); ;
    break;}
case 53:
#line 135 "mon_cli.y"
{ print_breakpts(); ;
    break;}
case 54:
#line 138 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 55:
#line 139 "mon_cli.y"
{ yyval.i = e_load_store; ;
    break;}
case 56:
#line 141 "mon_cli.y"
{ add_breakpoint(yyvsp[0].arange, FALSE, 
                              (yyvsp[-1].i == e_load || yyvsp[-1].i == e_load_store), (yyvsp[-1].i == e_store || yyvsp[-1].i == e_load_store)); ;
    break;}
case 57:
#line 143 "mon_cli.y"
{ print_breakpts(); ;
    break;}
case 58:
#line 146 "mon_cli.y"
{ add_breakpoint(yyvsp[0].arange, TRUE, FALSE, FALSE); ;
    break;}
case 59:
#line 147 "mon_cli.y"
{ print_breakpts(); ;
    break;}
case 60:
#line 150 "mon_cli.y"
{ fprintf(mon_output, "Display IO registers\n"); ;
    break;}
case 61:
#line 151 "mon_cli.y"
{ fprintf(mon_output, "Continue until RTS/RTI\n"); ;
    break;}
case 62:
#line 152 "mon_cli.y"
{ puts("Dump machine state."); ;
    break;}
case 63:
#line 153 "mon_cli.y"
{ puts("Undump machine state."); ;
    break;}
case 64:
#line 156 "mon_cli.y"
{ display_memory(yyvsp[-1].dt, yyvsp[0].arange); ;
    break;}
case 65:
#line 157 "mon_cli.y"
{ display_memory(0, yyvsp[0].arange); ;
    break;}
case 66:
#line 158 "mon_cli.y"
{ display_memory(0,new_range(bad_addr,bad_addr)); ;
    break;}
case 67:
#line 161 "mon_cli.y"
{ fprintf(mon_output, "Setting default readspace to %s\n",SPACESTRING(yyvsp[0].i)); 
                                      default_readspace = yyvsp[0].i; ;
    break;}
case 68:
#line 163 "mon_cli.y"
{ fprintf(mon_output, "Default readspace is %s\n",SPACESTRING(default_readspace)); ;
    break;}
case 69:
#line 164 "mon_cli.y"
{ fprintf(mon_output, "Setting default writespace to %s\n", SPACESTRING(yyvsp[0].i)); 
                                       default_writespace = yyvsp[0].i; ;
    break;}
case 70:
#line 166 "mon_cli.y"
{ fprintf(mon_output,"Default writespace is %s\n",SPACESTRING(default_writespace)); ;
    break;}
case 71:
#line 167 "mon_cli.y"
{ fprintf(mon_output,"Setting default device to %s\n", SPACESTRING(yyvsp[0].i)); 
                                 default_readspace = default_writespace = yyvsp[0].i; ;
    break;}
case 72:
#line 171 "mon_cli.y"
{ print_registers(e_default_space); ;
    break;}
case 73:
#line 172 "mon_cli.y"
{ print_registers(yyvsp[0].i); ;
    break;}
case 77:
#line 180 "mon_cli.y"
{ set_reg_val(yyvsp[-2].reg,yyvsp[0].i); ;
    break;}
case 78:
#line 183 "mon_cli.y"
{ mon_load_file(yyvsp[-1].str,yyvsp[0].a); ;
    break;}
case 79:
#line 184 "mon_cli.y"
{ mon_save_file(yyvsp[-1].str,yyvsp[0].arange); ;
    break;}
case 80:
#line 185 "mon_cli.y"
{ mon_verify_file(yyvsp[-1].str,yyvsp[0].a); ;
    break;}
case 81:
#line 188 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 82:
#line 189 "mon_cli.y"
{ yyval.i = -1; ;
    break;}
case 83:
#line 192 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 84:
#line 193 "mon_cli.y"
{ yyval.i = -1; ;
    break;}
case 85:
#line 196 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 86:
#line 199 "mon_cli.y"
{ block_cmd(0,yyvsp[-2].i,yyvsp[-1].i,yyvsp[0].a); ;
    break;}
case 87:
#line 200 "mon_cli.y"
{ block_cmd(1,yyvsp[-2].i,yyvsp[-1].i,yyvsp[0].a); ;
    break;}
case 88:
#line 203 "mon_cli.y"
{ yyval.arange = new_range(yyvsp[-1].a,yyvsp[0].a); ;
    break;}
case 89:
#line 204 "mon_cli.y"
{ yyval.arange = new_range(yyvsp[-3].a,new_addr(e_default_space,addr_location(yyvsp[-3].a)+yyvsp[0].i)); ;
    break;}
case 90:
#line 205 "mon_cli.y"
{ yyval.arange = new_range(yyvsp[-3].a,new_addr(e_default_space,addr_location(yyvsp[-3].a)-yyvsp[0].i)); ;
    break;}
case 91:
#line 208 "mon_cli.y"
{ yyval.arange = new_range(yyvsp[-1].a,yyvsp[0].a); ;
    break;}
case 92:
#line 210 "mon_cli.y"
{ yyval.a = yyvsp[0].a; ;
    break;}
case 93:
#line 211 "mon_cli.y"
{ yyval.a = bad_addr; ;
    break;}
case 94:
#line 214 "mon_cli.y"
{ yyval.a = new_addr(e_default_space,yyvsp[0].i); if (opt_asm) rol = 1; ;
    break;}
case 95:
#line 215 "mon_cli.y"
{ yyval.a = new_addr(yyvsp[-2].i,yyvsp[0].i); if (opt_asm) rol = 1; ;
    break;}
case 96:
#line 217 "mon_cli.y"
{ yyval.i = e_comp_space; ;
    break;}
case 97:
#line 218 "mon_cli.y"
{ yyval.i = e_disk_space; ;
    break;}
case 98:
#line 221 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 99:
#line 224 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 100:
#line 226 "mon_cli.y"
{ yyval.i = yyvsp[-2].i + yyvsp[0].i; ;
    break;}
case 101:
#line 227 "mon_cli.y"
{ yyval.i = yyvsp[-2].i - yyvsp[0].i; ;
    break;}
case 102:
#line 228 "mon_cli.y"
{ yyval.i = yyvsp[-2].i * yyvsp[0].i; ;
    break;}
case 103:
#line 229 "mon_cli.y"
{ yyval.i = (yyvsp[0].i) ? (yyvsp[-2].i / yyvsp[0].i) : 1; ;
    break;}
case 104:
#line 230 "mon_cli.y"
{ yyval.i = yyvsp[-1].i; ;
    break;}
case 105:
#line 231 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 106:
#line 234 "mon_cli.y"
{
              yyval.cond_node = new_cond;
              yyval.cond_node->child1 = yyvsp[-2].cond_node; yyval.cond_node->child2 = yyvsp[0].cond_node; yyval.cond_node->operation = yyvsp[-1].cond_op; ;
    break;}
case 107:
#line 239 "mon_cli.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV; 
                            yyval.cond_node->reg_num = yyvsp[0].reg; yyval.cond_node->is_reg = 1; ;
    break;}
case 108:
#line 241 "mon_cli.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV;
                            yyval.cond_node->value = yyvsp[0].i; yyval.cond_node->is_reg = 0; ;
    break;}
case 111:
#line 249 "mon_cli.y"
{ add_number_to_buffer(yyvsp[0].i); ;
    break;}
case 112:
#line 250 "mon_cli.y"
{ add_string_to_buffer(yyvsp[0].str); ;
    break;}
case 113:
#line 253 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 114:
#line 254 "mon_cli.y"
{ yyval.i = get_reg_val(e_comp_space, yyvsp[0].reg); /* FIXME */ ;
    break;}
case 115:
#line 256 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 116:
#line 257 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 117:
#line 258 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 118:
#line 259 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
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
#line 262 "mon_cli.y"
 

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

