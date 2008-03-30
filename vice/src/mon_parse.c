
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
#define	OPCODE	311
#define	STRING	312
#define	FILENAME	313
#define	R_O_L	314
#define	ASM_LINE	315
#define	REGISTER	316
#define	COMPARE_OP	317
#define	DATA_TYPE	318
#define	INPUT_SPEC	319
#define	CMD_BREAKPT_ONOFF	320
#define	TOGGLE	321

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
extern int rol, new_cmd, opt_asm;
extern void free_buffer(void);
extern void make_buffer(char *str);
extern int yylex(void);

#define new_mode(x,y) (LO16_TO_HI16(x)|y)
#define BAD_MODE        14


#line 25 "mon_cli.y"
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



#define	YYFINAL		198
#define	YYFLAG		-32768
#define	YYNTBASE	80

#define YYTRANSLATE(x) ((unsigned)(x) <= 321 ? yytranslate[x] : 120)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,    79,     2,     2,     2,     2,    77,
    78,    69,    67,    71,    68,     2,    70,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    74,     2,     2,
    72,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    75,    76,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    73,     2,     2,     2,     2,     2,     2,
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
    66
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     7,    10,    12,    15,    17,    19,    21,    23,
    25,    27,    29,    31,    34,    38,    42,    46,    49,    52,
    55,    57,    59,    62,    65,    68,    71,    73,    75,    77,
    79,    81,    85,    89,    93,    97,   100,   102,   105,   108,
   111,   113,   116,   118,   120,   123,   125,   126,   131,   134,
   137,   139,   142,   144,   147,   149,   151,   152,   156,   158,
   161,   163,   165,   167,   169,   171,   175,   178,   180,   183,
   185,   188,   190,   193,   195,   198,   201,   205,   207,   211,
   215,   219,   223,   225,   226,   228,   229,   231,   236,   241,
   244,   249,   254,   257,   259,   260,   262,   266,   268,   270,
   272,   274,   278,   282,   286,   290,   294,   296,   300,   302,
   304,   307,   309,   311,   313,   315,   317,   319,   321,   323,
   325,   328,   330,   334,   338,   344,   350,   351
};

static const short yyrhs[] = {    82,
    10,     0,    80,    82,    10,     0,    81,    10,     0,    10,
     0,    60,   119,     0,    83,     0,     7,     0,    89,     0,
    97,     0,   104,     0,    90,     0,    92,     0,    93,     0,
    65,   103,     0,    23,   103,   101,     0,    26,   105,   108,
     0,    37,   105,   108,     0,    27,   108,     0,    41,   102,
     0,    40,    10,     0,    85,     0,    94,     0,    47,   101,
     0,    46,   101,     0,    18,   101,     0,    19,   101,     0,
   100,     0,    96,     0,    95,     0,    86,     0,    88,     0,
    42,   103,   113,     0,    43,   103,    57,     0,    25,   105,
   115,     0,    24,   105,   115,     0,    48,   112,     0,    50,
     0,    52,    84,     0,    53,    84,     0,     8,   112,     0,
    54,     0,    55,    84,     0,    11,     0,    59,     0,    31,
    63,     0,    31,     0,     0,    44,   108,    87,    81,     0,
    44,   108,     0,    45,   106,     0,    45,     0,    14,    66,
     0,    14,     0,    33,   106,     0,    33,     0,    12,     0,
     0,    51,    91,   106,     0,    51,     0,    34,   106,     0,
    34,     0,    35,     0,    15,     0,    38,     0,    39,     0,
    32,    63,   106,     0,    32,   106,     0,    32,     0,    29,
   109,     0,    29,     0,    30,   109,     0,    30,     0,    49,
   109,     0,    28,     0,    28,   109,     0,    28,    98,     0,
    98,    71,    99,     0,    99,     0,    61,    72,   118,     0,
    20,    58,   108,     0,    21,    58,   105,     0,    22,    58,
   108,     0,   112,     0,     0,   103,     0,     0,   118,     0,
    16,   112,   112,   107,     0,    17,   112,   112,   108,     0,
   108,   108,     0,   108,    73,    67,   118,     0,   108,    73,
    68,   118,     0,   108,   107,     0,   108,     0,     0,   110,
     0,   109,    74,   110,     0,    75,     0,    76,     0,   111,
     0,   118,     0,   112,    67,   112,     0,   112,    68,   112,
     0,   112,    69,   112,     0,   112,    70,   112,     0,    77,
   112,    78,     0,   117,     0,   114,    62,   114,     0,    61,
     0,   118,     0,   115,   116,     0,   116,     0,   118,     0,
    57,     0,   118,     0,    61,     0,     3,     0,     4,     0,
     5,     0,     6,     0,    79,   118,     0,   118,     0,   118,
    71,    61,     0,    77,   118,    78,     0,    77,   118,    71,
    61,    78,     0,    77,   118,    78,    71,    61,     0,     0,
    61,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    68,    69,    70,    71,    74,    84,    86,    87,    88,    89,
    90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
   100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
   110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
   120,   121,   122,   125,   127,   128,   132,   133,   133,   136,
   137,   140,   141,   143,   144,   147,   148,   150,   152,   155,
   156,   159,   160,   161,   162,   165,   166,   167,   170,   172,
   173,   175,   176,   180,   181,   182,   185,   186,   189,   192,
   193,   194,   197,   198,   201,   202,   205,   208,   209,   212,
   213,   214,   217,   219,   220,   223,   224,   226,   227,   230,
   233,   235,   236,   237,   238,   239,   240,   243,   248,   250,
   254,   255,   258,   259,   262,   263,   265,   266,   267,   268,
   271,   272,   277,   291,   292,   299,   306,   307
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
"OPCODE","STRING","FILENAME","R_O_L","ASM_LINE","REGISTER","COMPARE_OP","DATA_TYPE",
"INPUT_SPEC","CMD_BREAKPT_ONOFF","TOGGLE","'+'","'-'","'*'","'/'","','","'='",
"'|'","':'","'c'","'d'","'('","')'","'#'","command_list","assembly_instruction",
"meta_command","command","rest_of_line","display_type","assemble","@1","disassemble",
"handle_read_sidefx","set_list_breakpts","opt_mem_op","set_list_watchpts","set_list_tracepts",
"no_arg_cmds","memory_display","space_mod","register_mod","reg_list","reg_asgn",
"file_cmd","opt_count","opt_brknum","breakpt_num","block_cmd","address_range",
"address_opt_range","opt_address","address","memspace","memloc","memaddr","expression",
"cond_expr","compare_operand","data_list","data_element","value","number","asm_operand_mode", NULL
};
#endif

static const short yyr1[] = {     0,
    80,    80,    80,    80,    81,    82,    83,    83,    83,    83,
    83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
    83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
    83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
    83,    83,    83,    84,    85,    85,    87,    86,    86,    88,
    88,    89,    89,    90,    90,    91,    91,    92,    92,    93,
    93,    94,    94,    94,    94,    95,    95,    95,    96,    96,
    96,    96,    96,    97,    97,    97,    98,    98,    99,   100,
   100,   100,   101,   101,   102,   102,   103,   104,   104,   105,
   105,   105,   106,   107,   107,   108,   108,   109,   109,   110,
   111,   112,   112,   112,   112,   112,   112,   113,   114,   114,
   115,   115,   116,   116,   117,   117,   118,   118,   118,   118,
   119,   119,   119,   119,   119,   119,   119,   119
};

static const short yyr2[] = {     0,
     2,     3,     2,     1,     2,     1,     1,     1,     1,     1,
     1,     1,     1,     2,     3,     3,     3,     2,     2,     2,
     1,     1,     2,     2,     2,     2,     1,     1,     1,     1,
     1,     3,     3,     3,     3,     2,     1,     2,     2,     2,
     1,     2,     1,     1,     2,     1,     0,     4,     2,     2,
     1,     2,     1,     2,     1,     1,     0,     3,     1,     2,
     1,     1,     1,     1,     1,     3,     2,     1,     2,     1,
     2,     1,     2,     1,     2,     2,     3,     1,     3,     3,
     3,     3,     1,     0,     1,     0,     1,     4,     4,     2,
     4,     4,     2,     1,     0,     1,     3,     1,     1,     1,
     1,     3,     3,     3,     3,     3,     1,     3,     1,     1,
     2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     2,     1,     3,     3,     5,     5,     0,     1
};

static const short yydefact[] = {     0,
     7,     0,     4,    43,    53,    63,     0,     0,    84,    84,
     0,     0,     0,     0,     0,     0,     0,     0,    74,    70,
    72,    46,    68,    55,    61,    62,     0,    64,    65,     0,
    86,     0,     0,     0,    51,    84,    84,     0,     0,    37,
    57,     0,     0,    41,     0,   127,     0,     0,     0,     0,
     6,    21,    30,    31,     8,    11,    12,    13,    22,    29,
    28,     9,    27,    10,   117,   118,   119,   120,   116,     0,
    40,   107,   115,    52,     0,     0,    25,    83,    26,     0,
     0,     0,    84,    87,    98,    99,     0,     0,     0,    96,
   100,   101,     0,     0,    18,     0,    76,    78,    75,    69,
    71,    45,     0,    67,    95,    54,    60,     0,    20,    19,
    85,     0,     0,    47,    50,    24,    23,    36,    73,    56,
     0,    44,    38,    39,    42,   128,     0,     0,   122,     5,
    14,     0,     3,     1,     0,     0,     0,     0,     0,    95,
     0,    80,    81,    82,    15,   114,    35,   112,   113,     0,
    90,     0,    34,    16,     0,     0,    66,    93,    94,    17,
   109,    32,     0,   110,    33,     0,    58,     0,   121,     0,
     2,   106,   102,   103,   104,   105,    88,    89,   111,     0,
     0,    97,    79,    77,     0,    48,     0,   124,   123,    91,
    92,   108,     0,     0,   125,   126,     0,     0
};

static const short yydefgoto[] = {    48,
    49,    50,    51,   123,    52,    53,   166,    54,    55,    56,
   121,    57,    58,    59,    60,    61,    62,    97,    98,    63,
    77,   110,    83,    64,    87,   104,   158,   105,    89,    90,
    91,    78,   162,   163,   147,   148,    72,    92,   130
};

static const short yypact[] = {   273,
-32768,    21,-32768,-32768,   -64,-32768,    21,    21,    21,    21,
   -35,   -25,   -15,   102,    47,    47,    47,    47,    72,   -56,
   -56,    -1,    97,    47,    47,-32768,    47,-32768,-32768,    66,
   102,   102,   102,    47,    47,    21,    21,    21,   -56,-32768,
    -9,    11,    11,-32768,    11,    10,   102,   221,    69,    70,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    21,
   119,-32768,-32768,-32768,    51,    51,-32768,   119,-32768,    47,
    47,    47,    21,-32768,-32768,-32768,    60,   158,     9,-32768,
-32768,-32768,    60,    47,-32768,    25,    38,-32768,-32768,-32768,
-32768,-32768,    47,-32768,    47,-32768,-32768,    47,-32768,-32768,
-32768,    55,    42,   103,-32768,-32768,-32768,   119,-32768,-32768,
    47,-32768,-32768,-32768,-32768,-32768,   102,   102,    44,-32768,
-32768,   114,-32768,-32768,    99,    21,    21,    21,    21,   126,
   126,-32768,-32768,-32768,-32768,-32768,    60,-32768,-32768,   -26,
-32768,   102,    60,-32768,   102,    83,-32768,-32768,-32768,-32768,
-32768,-32768,    65,-32768,-32768,    86,-32768,   -32,-32768,    88,
-32768,-32768,    73,    73,-32768,-32768,-32768,-32768,-32768,   102,
   102,-32768,-32768,-32768,    55,-32768,    89,    81,-32768,-32768,
-32768,-32768,    96,   115,-32768,-32768,   175,-32768
};

static const short yypgoto[] = {-32768,
    14,   133,-32768,    45,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    34,-32768,
     1,-32768,   123,-32768,     5,    -7,    52,    77,    28,    32,
-32768,     2,-32768,     6,   104,   -78,-32768,    -2,-32768
};


#define	YYLAST		338


static const short yytable[] = {    73,
   -59,    74,   120,    71,    73,    73,    73,    73,    75,    76,
    79,    84,    65,    66,    67,    68,   106,   107,    85,    86,
    93,    94,    80,    65,    66,    67,    68,   115,    84,    84,
    84,   108,    81,    73,    73,    73,   116,   117,   187,   118,
   180,   181,    82,   129,    84,   188,    99,   100,   101,    65,
    66,    67,    68,    65,    66,    67,    68,    65,    66,    67,
    68,   102,    65,    66,    67,    68,   119,    73,   179,   122,
   126,   135,    73,    73,   179,   109,   140,   141,   133,   134,
    73,    69,   152,   145,   149,   143,   127,   124,   128,   125,
   149,    88,    88,    88,    95,   157,   155,    70,   165,    65,
    66,    67,    68,    88,    65,    66,    67,    68,   156,   164,
   114,    69,   -49,   167,   170,   161,   146,   136,   137,   138,
   139,    85,    86,   171,   168,   169,   185,    70,    65,    66,
    67,    68,    96,    73,    73,    73,    73,   173,   174,   175,
   176,   138,   139,    96,   149,    46,    85,    86,   189,   193,
   149,   194,   183,   111,   112,   113,   142,    88,   144,   103,
    65,    66,    67,    68,   151,   136,   137,   138,   139,   131,
   154,    85,    86,   195,   198,   196,   172,   190,   191,   186,
   132,   159,   164,   182,   160,   136,   137,   138,   139,   184,
   192,   177,   136,   137,   138,   139,   153,     0,     0,     0,
    85,    86,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   159,   178,     0,     0,
   197,     0,     0,     0,     0,     0,     0,     1,     2,     0,
   150,     4,    85,    86,     5,     6,     7,     8,     9,    10,
    11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
    21,    22,    23,    24,    25,    26,     0,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    44,    45,     0,     0,     0,     1,
     2,     0,     3,     4,     0,    47,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,    21,    22,    23,    24,    25,    26,     0,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    41,    42,    43,    44,    45,     0,     0,
     0,     0,    46,     0,     0,     0,     0,    47
};

static const short yycheck[] = {     2,
    10,    66,    12,     2,     7,     8,     9,    10,     7,     8,
    10,    14,     3,     4,     5,     6,    24,    25,    75,    76,
    16,    17,    58,     3,     4,     5,     6,    35,    31,    32,
    33,    27,    58,    36,    37,    38,    36,    37,    71,    38,
    67,    68,    58,    46,    47,    78,    19,    20,    21,     3,
     4,     5,     6,     3,     4,     5,     6,     3,     4,     5,
     6,    63,     3,     4,     5,     6,    39,    70,   147,    59,
    61,    70,    75,    76,   153,    10,    75,    76,    10,    10,
    83,    61,    74,    83,    87,    81,    77,    43,    79,    45,
    93,    15,    16,    17,    18,   103,    72,    77,    57,     3,
     4,     5,     6,    27,     3,     4,     5,     6,    71,   112,
    34,    61,    10,   121,    71,    61,    57,    67,    68,    69,
    70,    75,    76,    10,   127,   128,    62,    77,     3,     4,
     5,     6,    61,   136,   137,   138,   139,   136,   137,   138,
   139,    69,    70,    61,   147,    60,    75,    76,    61,    61,
   153,    71,   155,    31,    32,    33,    80,    81,    82,    63,
     3,     4,     5,     6,    88,    67,    68,    69,    70,    47,
    94,    75,    76,    78,     0,    61,    78,   180,   181,   166,
    48,   105,   185,   152,   108,    67,    68,    69,    70,   156,
   185,   140,    67,    68,    69,    70,    93,    -1,    -1,    -1,
    75,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   140,   141,    -1,    -1,
     0,    -1,    -1,    -1,    -1,    -1,    -1,     7,     8,    -1,
    73,    11,    75,    76,    14,    15,    16,    17,    18,    19,
    20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    -1,    37,    38,    39,
    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
    50,    51,    52,    53,    54,    55,    -1,    -1,    -1,     7,
     8,    -1,    10,    11,    -1,    65,    14,    15,    16,    17,
    18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    -1,    37,
    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    55,    -1,    -1,
    -1,    -1,    60,    -1,    -1,    -1,    -1,    65
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
#line 71 "mon_cli.y"
{ new_cmd = 1; end_assemble_mode(); ;
    break;}
case 5:
#line 74 "mon_cli.y"
{ yyval.i = 0; 
                               if (yyvsp[-1].str) {
                                  mon_assemble_instr(yyvsp[-1].str, yyvsp[0].i);
                               } else {
                                  new_cmd = 1;
                                  end_assemble_mode();
                               }
                               opt_asm = 0;
                             ;
    break;}
case 7:
#line 86 "mon_cli.y"
{puts("Unsupported command"); ;
    break;}
case 14:
#line 93 "mon_cli.y"
{ switch_breakpt(yyvsp[-1].action, yyvsp[0].i); ;
    break;}
case 15:
#line 94 "mon_cli.y"
{ set_ignore_count(yyvsp[-1].i, yyvsp[0].i); ;
    break;}
case 16:
#line 95 "mon_cli.y"
{ move_memory(yyvsp[-1].arange, yyvsp[0].a); ;
    break;}
case 17:
#line 96 "mon_cli.y"
{ compare_memory(yyvsp[-1].arange, yyvsp[0].a); ;
    break;}
case 18:
#line 97 "mon_cli.y"
{ jump(yyvsp[0].a); ;
    break;}
case 19:
#line 98 "mon_cli.y"
{ delete_breakpoint(yyvsp[0].i); ;
    break;}
case 20:
#line 99 "mon_cli.y"
{ exit_mon = 1; YYACCEPT; ;
    break;}
case 23:
#line 102 "mon_cli.y"
{ instructions_step(yyvsp[0].i); ;
    break;}
case 24:
#line 103 "mon_cli.y"
{ instructions_next(yyvsp[0].i); ;
    break;}
case 25:
#line 104 "mon_cli.y"
{ stack_up(yyvsp[0].i); ;
    break;}
case 26:
#line 105 "mon_cli.y"
{ stack_down(yyvsp[0].i); ;
    break;}
case 32:
#line 111 "mon_cli.y"
{ set_brkpt_condition(yyvsp[-1].i, yyvsp[0].cond_node); ;
    break;}
case 33:
#line 112 "mon_cli.y"
{ set_breakpt_command(yyvsp[-1].i, yyvsp[0].str); ;
    break;}
case 34:
#line 113 "mon_cli.y"
{ fill_memory(yyvsp[-1].arange, yyvsp[0].str); ;
    break;}
case 35:
#line 114 "mon_cli.y"
{ hunt_memory(yyvsp[-1].arange, yyvsp[0].str); ;
    break;}
case 36:
#line 115 "mon_cli.y"
{ fprintf(mon_output, "\t%d\n",yyvsp[0].i); ;
    break;}
case 37:
#line 116 "mon_cli.y"
{ print_help(); ;
    break;}
case 38:
#line 117 "mon_cli.y"
{ printf("DISK COMMAND: %s\n",yyvsp[0].str); ;
    break;}
case 39:
#line 118 "mon_cli.y"
{ printf("SYSTEM COMMAND: %s\n",yyvsp[0].str); ;
    break;}
case 40:
#line 119 "mon_cli.y"
{ print_convert(yyvsp[0].i); ;
    break;}
case 41:
#line 120 "mon_cli.y"
{ printf("Quit.\n"); exit(-1); exit(0); ;
    break;}
case 42:
#line 121 "mon_cli.y"
{ change_dir(yyvsp[0].str); ;
    break;}
case 43:
#line 122 "mon_cli.y"
{ YYABORT; ;
    break;}
case 44:
#line 125 "mon_cli.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 45:
#line 127 "mon_cli.y"
{ default_datatype = yyvsp[0].dt; ;
    break;}
case 46:
#line 128 "mon_cli.y"
{ fprintf(mon_output, "Default datatype is %s\n", 
                                        datatype_string[default_datatype]); ;
    break;}
case 47:
#line 132 "mon_cli.y"
{ start_assemble_mode(yyvsp[0].a, NULL); ;
    break;}
case 49:
#line 133 "mon_cli.y"
{ start_assemble_mode(yyvsp[0].a, NULL); ;
    break;}
case 50:
#line 136 "mon_cli.y"
{ disassemble_lines(yyvsp[0].arange); ;
    break;}
case 51:
#line 137 "mon_cli.y"
{ disassemble_lines(new_range(bad_addr,bad_addr)); ;
    break;}
case 52:
#line 140 "mon_cli.y"
{ sidefx = ((yyvsp[0].action==e_TOGGLE)?(sidefx^1):yyvsp[0].action); ;
    break;}
case 53:
#line 141 "mon_cli.y"
{ fprintf(mon_output, "sidefx %d\n",sidefx); ;
    break;}
case 54:
#line 143 "mon_cli.y"
{ add_breakpoint(yyvsp[0].arange, FALSE, FALSE, FALSE); ;
    break;}
case 55:
#line 144 "mon_cli.y"
{ print_breakpts(); ;
    break;}
case 56:
#line 147 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 57:
#line 148 "mon_cli.y"
{ yyval.i = e_load_store; ;
    break;}
case 58:
#line 150 "mon_cli.y"
{ add_breakpoint(yyvsp[0].arange, FALSE, 
                              (yyvsp[-1].i == e_load || yyvsp[-1].i == e_load_store), (yyvsp[-1].i == e_store || yyvsp[-1].i == e_load_store)); ;
    break;}
case 59:
#line 152 "mon_cli.y"
{ print_breakpts(); ;
    break;}
case 60:
#line 155 "mon_cli.y"
{ add_breakpoint(yyvsp[0].arange, TRUE, FALSE, FALSE); ;
    break;}
case 61:
#line 156 "mon_cli.y"
{ print_breakpts(); ;
    break;}
case 62:
#line 159 "mon_cli.y"
{ fprintf(mon_output, "Display IO registers\n"); ;
    break;}
case 63:
#line 160 "mon_cli.y"
{ fprintf(mon_output, "Continue until RTS/RTI\n"); ;
    break;}
case 64:
#line 161 "mon_cli.y"
{ puts("Dump machine state."); ;
    break;}
case 65:
#line 162 "mon_cli.y"
{ puts("Undump machine state."); ;
    break;}
case 66:
#line 165 "mon_cli.y"
{ display_memory(yyvsp[-1].dt, yyvsp[0].arange); ;
    break;}
case 67:
#line 166 "mon_cli.y"
{ display_memory(0, yyvsp[0].arange); ;
    break;}
case 68:
#line 167 "mon_cli.y"
{ display_memory(0,new_range(bad_addr,bad_addr)); ;
    break;}
case 69:
#line 170 "mon_cli.y"
{ fprintf(mon_output, "Setting default readspace to %s\n",SPACESTRING(yyvsp[0].i)); 
                                      default_readspace = yyvsp[0].i; ;
    break;}
case 70:
#line 172 "mon_cli.y"
{ fprintf(mon_output, "Default readspace is %s\n",SPACESTRING(default_readspace)); ;
    break;}
case 71:
#line 173 "mon_cli.y"
{ fprintf(mon_output, "Setting default writespace to %s\n", SPACESTRING(yyvsp[0].i)); 
                                       default_writespace = yyvsp[0].i; ;
    break;}
case 72:
#line 175 "mon_cli.y"
{ fprintf(mon_output,"Default writespace is %s\n",SPACESTRING(default_writespace)); ;
    break;}
case 73:
#line 176 "mon_cli.y"
{ fprintf(mon_output,"Setting default device to %s\n", SPACESTRING(yyvsp[0].i)); 
                                 default_readspace = default_writespace = yyvsp[0].i; ;
    break;}
case 74:
#line 180 "mon_cli.y"
{ print_registers(e_default_space); ;
    break;}
case 75:
#line 181 "mon_cli.y"
{ print_registers(yyvsp[0].i); ;
    break;}
case 79:
#line 189 "mon_cli.y"
{ set_reg_val(yyvsp[-2].reg,yyvsp[0].i); ;
    break;}
case 80:
#line 192 "mon_cli.y"
{ mon_load_file(yyvsp[-1].str,yyvsp[0].a); ;
    break;}
case 81:
#line 193 "mon_cli.y"
{ mon_save_file(yyvsp[-1].str,yyvsp[0].arange); ;
    break;}
case 82:
#line 194 "mon_cli.y"
{ mon_verify_file(yyvsp[-1].str,yyvsp[0].a); ;
    break;}
case 83:
#line 197 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 84:
#line 198 "mon_cli.y"
{ yyval.i = -1; ;
    break;}
case 85:
#line 201 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 86:
#line 202 "mon_cli.y"
{ yyval.i = -1; ;
    break;}
case 87:
#line 205 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 88:
#line 208 "mon_cli.y"
{ block_cmd(0,yyvsp[-2].i,yyvsp[-1].i,yyvsp[0].a); ;
    break;}
case 89:
#line 209 "mon_cli.y"
{ block_cmd(1,yyvsp[-2].i,yyvsp[-1].i,yyvsp[0].a); ;
    break;}
case 90:
#line 212 "mon_cli.y"
{ yyval.arange = new_range(yyvsp[-1].a,yyvsp[0].a); ;
    break;}
case 91:
#line 213 "mon_cli.y"
{ yyval.arange = new_range(yyvsp[-3].a,new_addr(e_default_space,addr_location(yyvsp[-3].a)+yyvsp[0].i)); ;
    break;}
case 92:
#line 214 "mon_cli.y"
{ yyval.arange = new_range(yyvsp[-3].a,new_addr(e_default_space,addr_location(yyvsp[-3].a)-yyvsp[0].i)); ;
    break;}
case 93:
#line 217 "mon_cli.y"
{ yyval.arange = new_range(yyvsp[-1].a,yyvsp[0].a); ;
    break;}
case 94:
#line 219 "mon_cli.y"
{ yyval.a = yyvsp[0].a; ;
    break;}
case 95:
#line 220 "mon_cli.y"
{ yyval.a = bad_addr; ;
    break;}
case 96:
#line 223 "mon_cli.y"
{ yyval.a = new_addr(e_default_space,yyvsp[0].i); if (opt_asm) new_cmd = asm_mode = 1; ;
    break;}
case 97:
#line 224 "mon_cli.y"
{ yyval.a = new_addr(yyvsp[-2].i,yyvsp[0].i); if (opt_asm) new_cmd = asm_mode = 1; ;
    break;}
case 98:
#line 226 "mon_cli.y"
{ yyval.i = e_comp_space; ;
    break;}
case 99:
#line 227 "mon_cli.y"
{ yyval.i = e_disk_space; ;
    break;}
case 100:
#line 230 "mon_cli.y"
{ yyval.i = check_addr_limits(yyvsp[0].i); ;
    break;}
case 101:
#line 233 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 102:
#line 235 "mon_cli.y"
{ yyval.i = yyvsp[-2].i + yyvsp[0].i; ;
    break;}
case 103:
#line 236 "mon_cli.y"
{ yyval.i = yyvsp[-2].i - yyvsp[0].i; ;
    break;}
case 104:
#line 237 "mon_cli.y"
{ yyval.i = yyvsp[-2].i * yyvsp[0].i; ;
    break;}
case 105:
#line 238 "mon_cli.y"
{ yyval.i = (yyvsp[0].i) ? (yyvsp[-2].i / yyvsp[0].i) : 1; ;
    break;}
case 106:
#line 239 "mon_cli.y"
{ yyval.i = yyvsp[-1].i; ;
    break;}
case 107:
#line 240 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 108:
#line 243 "mon_cli.y"
{
              yyval.cond_node = new_cond;
              yyval.cond_node->child1 = yyvsp[-2].cond_node; yyval.cond_node->child2 = yyvsp[0].cond_node; yyval.cond_node->operation = yyvsp[-1].cond_op; ;
    break;}
case 109:
#line 248 "mon_cli.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV; 
                            yyval.cond_node->reg_num = yyvsp[0].reg; yyval.cond_node->is_reg = 1; ;
    break;}
case 110:
#line 250 "mon_cli.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV;
                            yyval.cond_node->value = yyvsp[0].i; yyval.cond_node->is_reg = 0; ;
    break;}
case 113:
#line 258 "mon_cli.y"
{ add_number_to_buffer(yyvsp[0].i); ;
    break;}
case 114:
#line 259 "mon_cli.y"
{ add_string_to_buffer(yyvsp[0].str); ;
    break;}
case 115:
#line 262 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 116:
#line 263 "mon_cli.y"
{ yyval.i = get_reg_val(e_comp_space, yyvsp[0].reg); /* FIXME */ ;
    break;}
case 117:
#line 265 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 118:
#line 266 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 119:
#line 267 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 120:
#line 268 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 121:
#line 271 "mon_cli.y"
{ yyval.i = new_mode(IMMEDIATE,yyvsp[0].i); ;
    break;}
case 122:
#line 272 "mon_cli.y"
{ if (yyvsp[0].i < 0x100)
                              yyval.i = new_mode(ZERO_PAGE,yyvsp[0].i);
                           else
                              yyval.i = new_mode(ABSOLUTE,yyvsp[0].i);
                         ;
    break;}
case 123:
#line 277 "mon_cli.y"
{ if (yyvsp[0].reg == e_X) {
                                            if (yyvsp[-2].i < 0x100)
                                               yyval.i = new_mode(ZERO_PAGE_X,yyvsp[-2].i);
                                            else
                                               yyval.i = new_mode(ABSOLUTE_X,yyvsp[-2].i);
                                         } else if (yyvsp[0].reg == e_Y) {
                                            if (yyvsp[-2].i < 0x100)
                                               yyval.i = new_mode(ZERO_PAGE_Y,yyvsp[-2].i);
                                            else
                                               yyval.i = new_mode(ABSOLUTE_Y,yyvsp[-2].i);
                                         } else {
                                            printf("Invalid index register.\n");
                                         }
                                       ;
    break;}
case 124:
#line 291 "mon_cli.y"
{ yyval.i = new_mode(ABS_INDIRECT,yyvsp[-1].i); ;
    break;}
case 125:
#line 292 "mon_cli.y"
{ if (yyvsp[-1].reg == e_X)
                                                   yyval.i = new_mode(INDIRECT_X,yyvsp[-3].i);
                                                else {
                                                   printf("Invalid index register.\n");
                                                   yyval.i = new_mode(BAD_MODE,0);
                                                }
                                              ;
    break;}
case 126:
#line 299 "mon_cli.y"
{ if (yyvsp[0].reg == e_Y)
                                                   yyval.i = new_mode(INDIRECT_Y,yyvsp[-3].i);
                                                else {
                                                   printf("Invalid index register.\n");
                                                   yyval.i = new_mode(BAD_MODE,0);
                                                }
                                              ;
    break;}
case 127:
#line 306 "mon_cli.y"
{ yyval.i = new_mode(IMPLIED,0); ;
    break;}
case 128:
#line 307 "mon_cli.y"
{ if (yyvsp[0].reg == e_A)
                                yyval.i = new_mode(ACCUMULATOR,yyvsp[0].reg);
                             else {
                                printf("Invalid register.\n");
                                yyval.i = new_mode(BAD_MODE,0);
                             }
                           ;
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
#line 317 "mon_cli.y"
 

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

