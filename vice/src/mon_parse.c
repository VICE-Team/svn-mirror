
/*  A Bison parser, made from mon_cli.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	H_NUMBER	258
#define	D_NUMBER	259
#define	O_NUMBER	260
#define	B_NUMBER	261
#define	CONVERT_OP	262
#define	B_DATA	263
#define	TRAIL	264
#define	BAD_CMD	265
#define	MEM_OP	266
#define	IF	267
#define	MEM_COMP	268
#define	MEM_DISK	269
#define	CMD_SEP	270
#define	REG_ASGN_SEP	271
#define	EQUALS	272
#define	CMD_SIDEFX	273
#define	CMD_RETURN	274
#define	CMD_BLOCK_READ	275
#define	CMD_BLOCK_WRITE	276
#define	CMD_UP	277
#define	CMD_DOWN	278
#define	CMD_LOAD	279
#define	CMD_SAVE	280
#define	CMD_VERIFY	281
#define	CMD_IGNORE	282
#define	CMD_HUNT	283
#define	CMD_FILL	284
#define	CMD_MOVE	285
#define	CMD_GOTO	286
#define	CMD_REGISTERS	287
#define	CMD_READSPACE	288
#define	CMD_WRITESPACE	289
#define	CMD_DISPLAYTYPE	290
#define	CMD_MEM_DISPLAY	291
#define	CMD_BREAK	292
#define	CMD_TRACE	293
#define	CMD_IO	294
#define	CMD_BRMON	295
#define	CMD_COMPARE	296
#define	CMD_DUMP	297
#define	CMD_UNDUMP	298
#define	CMD_EXIT	299
#define	CMD_DELETE	300
#define	CMD_CONDITION	301
#define	CMD_COMMAND	302
#define	CMD_ASSEMBLE	303
#define	CMD_DISASSEMBLE	304
#define	CMD_NEXT	305
#define	CMD_STEP	306
#define	CMD_PRINT	307
#define	CMD_DEVICE	308
#define	CMD_HELP	309
#define	CMD_WATCH	310
#define	CMD_DISK	311
#define	CMD_SYSTEM	312
#define	CMD_QUIT	313
#define	CMD_CHDIR	314
#define	CMD_BANK	315
#define	CMD_LOAD_LABELS	316
#define	CMD_SAVE_LABELS	317
#define	CMD_ADD_LABEL	318
#define	CMD_DEL_LABEL	319
#define	CMD_SHOW_LABELS	320
#define	CMD_RECORD	321
#define	CMD_STOP	322
#define	CMD_PLAYBACK	323
#define	L_PAREN	324
#define	R_PAREN	325
#define	ARG_IMMEDIATE	326
#define	REG_A	327
#define	REG_X	328
#define	REG_Y	329
#define	COMMA	330
#define	INST_SEP	331
#define	STRING	332
#define	FILENAME	333
#define	R_O_L	334
#define	OPCODE	335
#define	LABEL	336
#define	REGISTER	337
#define	COMPARE_OP	338
#define	DATA_TYPE	339
#define	INPUT_SPEC	340
#define	CMD_CHECKPT_ONOFF	341
#define	TOGGLE	342

#line 1 "mon_cli.y"

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


#line 29 "mon_cli.y"
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



#define	YYFINAL		298
#define	YYFLAG		-32768
#define	YYNTBASE	96

#define YYTRANSLATE(x) ((unsigned)(x) <= 342 ? yytranslate[x] : 138)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    94,
    95,    90,    88,     2,    89,     2,    91,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    92,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    93,     2,     2,     2,     2,     2,     2,
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
    76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
    86,    87
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     7,     9,    12,    14,    16,    19,    21,
    25,    28,    31,    34,    37,    41,    45,    49,    53,    55,
    57,    59,    61,    63,    65,    67,    69,    71,    73,    75,
    80,    85,    90,    95,    99,   100,   106,   110,   114,   117,
   122,   127,   132,   137,   142,   146,   149,   153,   159,   164,
   168,   171,   174,   177,   181,   186,   190,   196,   201,   205,
   208,   212,   215,   219,   222,   226,   229,   233,   236,   239,
   243,   247,   250,   254,   258,   262,   266,   271,   276,   281,
   285,   288,   292,   294,   296,   297,   299,   302,   305,   309,
   311,   315,   320,   322,   323,   325,   326,   328,   333,   338,
   341,   346,   351,   354,   356,   357,   359,   362,   364,   365,
   367,   369,   371,   373,   377,   381,   385,   389,   393,   395,
   399,   403,   405,   407,   410,   412,   415,   417,   419,   421,
   423,   425,   428,   430,   432,   434,   436,   440,   444,   447,
   449,   451,   454,   456,   460,   464,   468,   474,   480,   481
};

static const short yyrhs[] = {    97,
     0,   135,     9,     0,     9,     0,    99,     0,    97,    99,
     0,    15,     0,     9,     0,    60,    98,     0,   112,     0,
    31,   122,    98,     0,    39,    98,     0,    19,    98,     0,
    42,    98,     0,    43,    98,     0,    51,   115,    98,     0,
    50,   115,    98,     0,    22,   115,    98,     0,    23,   115,
    98,     0,   118,     0,   100,     0,   103,     0,   101,     0,
   104,     0,   105,     0,   106,     0,   107,     0,   108,     0,
   109,     0,    10,     0,    61,   123,    78,    98,     0,    62,
   123,    78,    98,     0,    63,   122,    81,    98,     0,    64,
   123,    81,    98,     0,    65,   123,    98,     0,     0,    48,
   122,   102,   136,    98,     0,    48,   122,    98,     0,    49,
   120,    98,     0,    49,    98,     0,    30,   119,   122,    98,
     0,    41,   119,   122,    98,     0,    29,   119,   130,    98,
     0,    28,   119,   130,    98,     0,    36,    84,   120,    98,
     0,    36,   120,    98,     0,    36,    98,     0,    37,   120,
    98,     0,    37,   120,    12,   128,    98,     0,    55,   111,
   120,    98,     0,    38,   120,    98,     0,    37,    98,     0,
    38,    98,     0,    55,    98,     0,    86,   117,    98,     0,
    27,   117,   115,    98,     0,    45,   116,    98,     0,    46,
   117,    12,   128,    98,     0,    47,   117,    77,    98,     0,
    18,    87,    98,     0,    18,    98,     0,    35,    84,    98,
     0,    35,    98,     0,    33,   124,    98,     0,    33,    98,
     0,    34,   124,    98,     0,    34,    98,     0,    53,   124,
    98,     0,    58,    98,     0,    44,    98,     0,    56,   110,
    98,     0,    52,   127,    98,     0,    54,    98,     0,    54,
   110,    98,     0,    57,   110,    98,     0,     7,   127,    98,
     0,    59,   110,    98,     0,    24,    78,   122,    98,     0,
    25,    78,   119,    98,     0,    26,    78,   122,    98,     0,
    66,    78,    98,     0,    67,    98,     0,    68,    78,    98,
     0,    79,     0,    11,     0,     0,    32,     0,    32,   124,
     0,    32,   113,     0,   113,    16,   114,     0,   114,     0,
    82,    17,   133,     0,   124,    82,    92,   133,     0,   127,
     0,     0,   117,     0,     0,     4,     0,    20,   127,   127,
   121,     0,    21,   127,   127,   122,     0,   122,   122,     0,
   122,    93,    88,   133,     0,   122,    93,    89,   133,     0,
   122,   121,     0,   122,     0,     0,   125,     0,   124,   125,
     0,   124,     0,     0,    13,     0,    14,     0,   126,     0,
   133,     0,   127,    88,   127,     0,   127,    89,   127,     0,
   127,    90,   127,     0,   127,    91,   127,     0,    94,   127,
    95,     0,   132,     0,   128,    83,   128,     0,    69,   128,
    70,     0,   129,     0,    82,     0,   124,    82,     0,   133,
     0,   130,   131,     0,   131,     0,   133,     0,    77,     0,
   133,     0,    82,     0,   124,    82,     0,     3,     0,     4,
     0,     5,     0,     6,     0,   134,    76,   135,     0,   135,
    76,   135,     0,    80,   137,     0,   135,     0,   134,     0,
    71,   133,     0,   133,     0,   133,    75,    73,     0,   133,
    75,    74,     0,    69,   133,    70,     0,    69,   133,    75,
    73,    70,     0,    69,   133,    70,    75,    74,     0,     0,
    72,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    78,    79,    80,    83,    84,    87,    88,    91,    92,    93,
    94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
   104,   105,   106,   107,   108,   109,   110,   111,   112,   115,
   116,   117,   118,   119,   122,   122,   123,   124,   125,   128,
   129,   130,   131,   132,   133,   134,   137,   138,   140,   142,
   143,   144,   145,   149,   150,   151,   152,   153,   156,   157,
   158,   159,   161,   163,   165,   167,   169,   171,   172,   175,
   176,   177,   178,   179,   180,   181,   184,   185,   186,   189,
   190,   191,   194,   197,   198,   200,   201,   202,   205,   206,
   209,   210,   213,   214,   217,   218,   221,   224,   225,   228,
   229,   230,   233,   235,   236,   239,   240,   243,   244,   247,
   248,   251,   254,   256,   257,   258,   259,   260,   261,   264,
   267,   268,   271,   273,   275,   279,   280,   283,   284,   287,
   288,   289,   292,   293,   294,   295,   298,   299,   302,   312,
   313,   316,   317,   322,   327,   332,   333,   334,   335,   336
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","H_NUMBER",
"D_NUMBER","O_NUMBER","B_NUMBER","CONVERT_OP","B_DATA","TRAIL","BAD_CMD","MEM_OP",
"IF","MEM_COMP","MEM_DISK","CMD_SEP","REG_ASGN_SEP","EQUALS","CMD_SIDEFX","CMD_RETURN",
"CMD_BLOCK_READ","CMD_BLOCK_WRITE","CMD_UP","CMD_DOWN","CMD_LOAD","CMD_SAVE",
"CMD_VERIFY","CMD_IGNORE","CMD_HUNT","CMD_FILL","CMD_MOVE","CMD_GOTO","CMD_REGISTERS",
"CMD_READSPACE","CMD_WRITESPACE","CMD_DISPLAYTYPE","CMD_MEM_DISPLAY","CMD_BREAK",
"CMD_TRACE","CMD_IO","CMD_BRMON","CMD_COMPARE","CMD_DUMP","CMD_UNDUMP","CMD_EXIT",
"CMD_DELETE","CMD_CONDITION","CMD_COMMAND","CMD_ASSEMBLE","CMD_DISASSEMBLE",
"CMD_NEXT","CMD_STEP","CMD_PRINT","CMD_DEVICE","CMD_HELP","CMD_WATCH","CMD_DISK",
"CMD_SYSTEM","CMD_QUIT","CMD_CHDIR","CMD_BANK","CMD_LOAD_LABELS","CMD_SAVE_LABELS",
"CMD_ADD_LABEL","CMD_DEL_LABEL","CMD_SHOW_LABELS","CMD_RECORD","CMD_STOP","CMD_PLAYBACK",
"L_PAREN","R_PAREN","ARG_IMMEDIATE","REG_A","REG_X","REG_Y","COMMA","INST_SEP",
"STRING","FILENAME","R_O_L","OPCODE","LABEL","REGISTER","COMPARE_OP","DATA_TYPE",
"INPUT_SPEC","CMD_CHECKPT_ONOFF","TOGGLE","'+'","'-'","'*'","'/'","'='","'|'",
"'('","')'","top_level","command_list","end_cmd","command","symbol_table_rules",
"asm_rules","@1","memory_rules","checkpoint_rules","checkpoint_control_rules",
"monitor_state_rules","monitor_misc_rules","file_rules","cmd_file_rules","rest_of_line",
"opt_mem_op","register_mod","reg_list","reg_asgn","opt_count","opt_brknum","breakpt_num",
"block_cmd","address_range","address_opt_range","opt_address","address","opt_memspace",
"memspace","memloc","memaddr","expression","cond_expr","compare_operand","data_list",
"data_element","value","number","assembly_instr_list","assembly_instruction",
"post_assemble","asm_operand_mode", NULL
};
#endif

static const short yyr1[] = {     0,
    96,    96,    96,    97,    97,    98,    98,    99,    99,    99,
    99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
    99,    99,    99,    99,    99,    99,    99,    99,    99,   100,
   100,   100,   100,   100,   102,   101,   101,   101,   101,   103,
   103,   103,   103,   103,   103,   103,   104,   104,   104,   104,
   104,   104,   104,   105,   105,   105,   105,   105,   106,   106,
   106,   106,   106,   106,   106,   106,   106,   106,   106,   107,
   107,   107,   107,   107,   107,   107,   108,   108,   108,   109,
   109,   109,   110,   111,   111,   112,   112,   112,   113,   113,
   114,   114,   115,   115,   116,   116,   117,   118,   118,   119,
   119,   119,   120,   121,   121,   122,   122,   123,   123,   124,
   124,   125,   126,   127,   127,   127,   127,   127,   127,   128,
   128,   128,   129,   129,   129,   130,   130,   131,   131,   132,
   132,   132,   133,   133,   133,   133,   134,   134,   135,   136,
   136,   137,   137,   137,   137,   137,   137,   137,   137,   137
};

static const short yyr2[] = {     0,
     1,     2,     1,     1,     2,     1,     1,     2,     1,     3,
     2,     2,     2,     2,     3,     3,     3,     3,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     4,
     4,     4,     4,     3,     0,     5,     3,     3,     2,     4,
     4,     4,     4,     4,     3,     2,     3,     5,     4,     3,
     2,     2,     2,     3,     4,     3,     5,     4,     3,     2,
     3,     2,     3,     2,     3,     2,     3,     2,     2,     3,
     3,     2,     3,     3,     3,     3,     4,     4,     4,     3,
     2,     3,     1,     1,     0,     1,     2,     2,     3,     1,
     3,     4,     1,     0,     1,     0,     1,     4,     4,     2,
     4,     4,     2,     1,     0,     1,     2,     1,     0,     1,
     1,     1,     1,     3,     3,     3,     3,     3,     1,     3,
     3,     1,     1,     2,     1,     2,     1,     1,     1,     1,
     1,     2,     1,     1,     1,     1,     3,     3,     2,     1,
     1,     2,     1,     3,     3,     3,     5,     5,     0,     1
};

static const short yydefact[] = {     0,
     0,     3,    29,     0,     0,     0,     0,    94,    94,     0,
     0,     0,     0,     0,     0,     0,     0,    86,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    96,
     0,     0,     0,     0,    94,    94,     0,     0,     0,    85,
     0,     0,     0,     0,     0,   109,   109,     0,   109,   109,
     0,     0,     0,   149,     0,     1,     4,    20,    22,    21,
    23,    24,    25,    26,    27,    28,     9,    19,     0,   133,
   134,   135,   136,   110,   111,   131,     0,     0,     0,   119,
   130,     7,     6,     0,    60,    12,     0,     0,     0,    93,
     0,     0,     0,     0,    97,    94,     0,     0,     0,   106,
   112,   113,     0,     0,     0,     0,    88,    90,    87,    64,
     0,    66,     0,     0,    62,     0,    46,     0,   105,    51,
     0,    52,     0,    11,     0,    13,    14,    69,     0,    95,
     0,     0,    35,    39,     0,     0,     0,     0,     0,    83,
    72,     0,    84,    53,     0,     0,     0,    68,     0,     8,
     0,   108,     0,     0,     0,     0,     0,    81,     0,     0,
     0,   150,   143,   139,     0,     5,     2,     0,   132,     0,
     0,     0,     0,    75,    59,   105,     0,    17,    18,     0,
     0,     0,     0,   129,     0,   127,   128,     0,   100,   107,
     0,     0,    10,     0,     0,     0,    63,    65,    61,     0,
    45,   103,   104,     0,    47,    50,     0,    56,     0,     0,
    37,     0,    38,    16,    15,    71,    67,    73,     0,    70,
    74,    76,     0,     0,     0,     0,    34,    80,    82,     0,
   142,     0,    54,   118,   114,   115,   116,   117,    98,    99,
    77,    78,    79,    55,    43,   126,     0,     0,    42,    40,
    91,    89,     0,     0,    44,     0,   123,     0,     0,   122,
   125,    41,     0,    58,   141,   140,     0,    49,    30,    31,
    32,    33,   146,     0,   144,   145,   101,   102,    92,     0,
   124,     0,    48,    57,     0,     0,    36,     0,     0,   121,
   120,   137,   138,   148,   147,     0,     0,     0
};

static const short yydefgoto[] = {   296,
    56,    85,    57,    58,    59,   212,    60,    61,    62,    63,
    64,    65,    66,   142,   145,    67,   107,   108,    89,   129,
    96,    68,    97,   118,   202,   119,   151,    99,   100,   101,
    90,   259,   260,   185,   186,    80,   102,   265,    69,   267,
   164
};

static const short yypact[] = {   490,
    42,-32768,-32768,    48,   153,    42,    42,    42,    42,   -75,
   -51,   -39,    47,   196,   196,   196,   196,    36,   216,   216,
    63,   209,   306,   306,   153,   196,   153,   153,   153,    47,
    47,    47,   196,   306,    42,    42,    42,   108,    73,   177,
   -11,   -11,   153,   -11,   153,   108,   108,   196,   108,   108,
    11,   153,    29,   231,    47,   559,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    72,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,    42,    28,    60,-32768,
-32768,-32768,-32768,   153,-32768,-32768,    15,    15,   153,   197,
   153,   196,   196,   196,-32768,    42,   174,   128,   321,-32768,
-32768,-32768,   174,   196,   153,   100,   122,-32768,    71,-32768,
   153,-32768,   153,   153,-32768,   196,-32768,   153,   196,-32768,
   283,-32768,   153,-32768,   196,-32768,-32768,-32768,   153,-32768,
   133,    81,   153,-32768,   153,   153,   153,    60,   153,-32768,
-32768,   153,-32768,-32768,   196,   153,   153,-32768,   153,-32768,
    85,-32768,    89,   117,   123,   153,   153,-32768,   153,   321,
   321,-32768,   106,-32768,   153,-32768,-32768,   181,-32768,    42,
    42,    42,    42,-32768,-32768,   151,   151,-32768,-32768,   153,
   153,   153,   153,-32768,   110,-32768,-32768,   -24,-32768,-32768,
   110,   153,-32768,   321,    36,    97,-32768,-32768,-32768,   153,
-32768,-32768,-32768,    10,-32768,-32768,   153,-32768,    10,   153,
-32768,   111,-32768,-32768,-32768,-32768,-32768,-32768,   153,-32768,
-32768,-32768,   153,   153,   153,   153,-32768,-32768,-32768,   -53,
-32768,   175,-32768,-32768,   136,   136,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   321,   321,-32768,-32768,
-32768,-32768,    71,   321,-32768,    10,-32768,   134,    43,-32768,
-32768,-32768,    43,-32768,   144,   167,   153,-32768,-32768,-32768,
-32768,-32768,   170,   179,-32768,-32768,-32768,-32768,-32768,   -29,
-32768,    10,-32768,-32768,   111,   111,-32768,   176,   195,-32768,
-32768,-32768,-32768,-32768,-32768,   274,   275,-32768
};

static const short yypgoto[] = {-32768,
-32768,   239,   221,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   141,-32768,-32768,-32768,    88,     2,-32768,
    53,-32768,    51,   -22,   118,   113,   207,    24,   198,-32768,
     3,  -116,-32768,   193,   -48,-32768,    -1,-32768,  -186,-32768,
-32768
};


#define	YYLAST		645


static const short yytable[] = {    81,
   121,   123,    92,    79,    81,    81,    81,    81,    87,    88,
    91,   135,    70,    71,    72,    73,   273,    70,    71,    72,
    73,   274,    74,    75,    78,   266,    93,    74,    75,    78,
    78,    78,    78,    81,    81,    81,   136,   137,    94,   138,
   290,   109,   111,   113,    70,    71,    72,    73,    74,    75,
    95,    82,   163,   282,    74,    75,    82,    83,    78,    78,
    78,   139,    83,   247,   248,   103,   104,   140,    82,   152,
   152,    82,   152,   152,    83,    81,   125,    83,   256,   168,
   167,    82,   130,   131,   132,    81,    81,    83,   157,   176,
   177,   257,   263,   200,    81,   187,    76,   183,   292,   293,
    78,   187,   170,   171,   172,   173,   159,   165,    77,   169,
    78,    78,    70,    71,    72,    73,   194,   106,    82,    78,
    74,    75,   219,    76,    83,   282,    98,    98,    98,   105,
    70,    71,    72,    73,    84,    77,   246,   195,    98,   280,
    74,    75,   246,   181,   209,   133,   114,   170,   171,   172,
   173,   140,   196,    70,    71,    72,    73,   210,   230,   231,
   154,    82,   223,    74,    75,   291,   224,    83,    81,    81,
    81,    81,   235,   236,   237,   238,    70,    71,    72,    73,
   232,   146,   147,   187,   149,    82,   184,   143,   254,   187,
    54,    83,   251,    78,    78,    78,    78,   225,    70,    71,
    72,    73,   261,   226,   180,    98,   182,   261,    74,    75,
   189,    70,    71,    72,    73,   281,   192,    82,   253,   285,
   188,    74,    75,    83,    82,   172,   173,   258,    74,    75,
    83,   203,   258,    70,    71,    72,    73,   207,   170,   171,
   172,   173,   286,    86,   288,   277,   278,   275,   276,   294,
   184,   289,   279,   153,   261,   155,   156,   110,   112,   115,
   117,   120,   122,   124,   295,   126,   127,   128,   170,   171,
   172,   173,   134,   297,   298,   234,   166,   141,   144,   258,
   261,   148,   252,   150,   170,   171,   172,   173,   203,   240,
   158,    82,   116,   239,   204,   191,   190,    83,     0,   160,
     0,   161,   162,     0,     0,   258,     0,     0,    70,    71,
    72,    73,     0,     0,    82,     0,     0,   174,    74,    75,
    83,     0,   175,    70,    71,    72,    73,   178,     0,   179,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   193,     0,     0,     0,     0,     0,   197,
     0,   198,   199,     0,     0,     0,   201,     0,     0,   205,
     0,   206,     0,     0,     0,     0,     0,   208,     0,     0,
     0,   211,     0,   213,   214,   215,   216,   217,     0,     0,
   218,     0,     0,     0,   220,   221,     0,   222,     0,     0,
     0,     0,     0,     0,   227,   228,     0,   229,     0,     0,
     0,     0,     0,   233,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   241,   242,
   243,   244,     0,   245,     0,     0,     0,     0,     0,   249,
   250,     0,     0,     0,     0,     0,     0,     0,   255,     0,
     0,     0,     0,     0,     0,   262,     0,     0,   264,     0,
     0,     0,     0,     0,     0,     0,     0,   268,     0,     0,
     0,   269,   270,   271,   272,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     1,   283,     2,     3,
     0,   284,     0,     0,     0,   287,     0,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,     0,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,     0,     0,
     0,     0,     0,     0,     0,     1,     0,     0,     3,    54,
     0,     0,     0,     0,     0,    55,     4,     5,     6,     7,
     8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
    18,    19,    20,    21,    22,    23,    24,    25,     0,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    55
};

static const short yycheck[] = {     1,
    23,    24,    78,     1,     6,     7,     8,     9,     6,     7,
     9,    34,     3,     4,     5,     6,    70,     3,     4,     5,
     6,    75,    13,    14,     1,   212,    78,    13,    14,     6,
     7,     8,     9,    35,    36,    37,    35,    36,    78,    37,
    70,    18,    19,    20,     3,     4,     5,     6,    13,    14,
     4,     9,    54,    83,    13,    14,     9,    15,    35,    36,
    37,    38,    15,    88,    89,    15,    16,    79,     9,    46,
    47,     9,    49,    50,    15,    77,    26,    15,    69,    77,
     9,     9,    30,    31,    32,    87,    88,    15,    78,    87,
    88,    82,   209,   116,    96,    97,    82,    96,   285,   286,
    77,   103,    88,    89,    90,    91,    78,    55,    94,    82,
    87,    88,     3,     4,     5,     6,    17,    82,     9,    96,
    13,    14,   145,    82,    15,    83,    14,    15,    16,    17,
     3,     4,     5,     6,    87,    94,   185,    16,    26,   256,
    13,    14,   191,    93,    12,    33,    84,    88,    89,    90,
    91,    79,    82,     3,     4,     5,     6,    77,   160,   161,
    48,     9,    78,    13,    14,   282,    78,    15,   170,   171,
   172,   173,   170,   171,   172,   173,     3,     4,     5,     6,
    75,    41,    42,   185,    44,     9,    77,    11,    92,   191,
    80,    15,   194,   170,   171,   172,   173,    81,     3,     4,
     5,     6,   204,    81,    92,    93,    94,   209,    13,    14,
    98,     3,     4,     5,     6,    82,   104,     9,   195,    76,
    93,    13,    14,    15,     9,    90,    91,   204,    13,    14,
    15,   119,   209,     3,     4,     5,     6,   125,    88,    89,
    90,    91,    76,     5,    75,   247,   248,    73,    74,    74,
    77,    73,   254,    47,   256,    49,    50,    19,    20,    21,
    22,    23,    24,    25,    70,    27,    28,    29,    88,    89,
    90,    91,    34,     0,     0,    95,    56,    39,    40,   256,
   282,    43,   195,    45,    88,    89,    90,    91,   176,   177,
    52,     9,    84,   176,    12,   103,    99,    15,    -1,    69,
    -1,    71,    72,    -1,    -1,   282,    -1,    -1,     3,     4,
     5,     6,    -1,    -1,     9,    -1,    -1,    79,    13,    14,
    15,    -1,    84,     3,     4,     5,     6,    89,    -1,    91,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   105,    -1,    -1,    -1,    -1,    -1,   111,
    -1,   113,   114,    -1,    -1,    -1,   118,    -1,    -1,   121,
    -1,   123,    -1,    -1,    -1,    -1,    -1,   129,    -1,    -1,
    -1,   133,    -1,   135,   136,   137,   138,   139,    -1,    -1,
   142,    -1,    -1,    -1,   146,   147,    -1,   149,    -1,    -1,
    -1,    -1,    -1,    -1,   156,   157,    -1,   159,    -1,    -1,
    -1,    -1,    -1,   165,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   180,   181,
   182,   183,    -1,   185,    -1,    -1,    -1,    -1,    -1,   191,
   192,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   200,    -1,
    -1,    -1,    -1,    -1,    -1,   207,    -1,    -1,   210,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   219,    -1,    -1,
    -1,   223,   224,   225,   226,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     7,   259,     9,    10,
    -1,   263,    -1,    -1,    -1,   267,    -1,    18,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    -1,
    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
    51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
    61,    62,    63,    64,    65,    66,    67,    68,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,    10,    80,
    -1,    -1,    -1,    -1,    -1,    86,    18,    19,    20,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    -1,    41,
    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
    62,    63,    64,    65,    66,    67,    68,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    86
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
#line 80 "mon_cli.y"
{ new_cmd = 1; asm_mode = 0; ;
    break;}
case 8:
#line 91 "mon_cli.y"
{ fprintf(mon_output, "Bank command not done yet.\n"); ;
    break;}
case 10:
#line 93 "mon_cli.y"
{ jump(yyvsp[-1].a); ;
    break;}
case 11:
#line 94 "mon_cli.y"
{ fprintf(mon_output, "Display IO registers\n"); ;
    break;}
case 12:
#line 95 "mon_cli.y"
{ fprintf(mon_output, "Continue until RTS/RTI\n"); ;
    break;}
case 13:
#line 96 "mon_cli.y"
{ puts("Dump machine state."); ;
    break;}
case 14:
#line 97 "mon_cli.y"
{ puts("Undump machine state."); ;
    break;}
case 15:
#line 98 "mon_cli.y"
{ instructions_step(yyvsp[-1].i); ;
    break;}
case 16:
#line 99 "mon_cli.y"
{ instructions_next(yyvsp[-1].i); ;
    break;}
case 17:
#line 100 "mon_cli.y"
{ stack_up(yyvsp[-1].i); ;
    break;}
case 18:
#line 101 "mon_cli.y"
{ stack_down(yyvsp[-1].i); ;
    break;}
case 29:
#line 112 "mon_cli.y"
{ YYABORT; ;
    break;}
case 30:
#line 115 "mon_cli.y"
{ mon_load_symbols(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 31:
#line 116 "mon_cli.y"
{ mon_save_symbols(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 32:
#line 117 "mon_cli.y"
{ add_name_to_symbol_table(yyvsp[-2].a, yyvsp[-1].str); ;
    break;}
case 33:
#line 118 "mon_cli.y"
{ remove_name_from_symbol_table(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 34:
#line 119 "mon_cli.y"
{ print_symbol_table(yyvsp[-1].i); ;
    break;}
case 35:
#line 122 "mon_cli.y"
{ start_assemble_mode(yyvsp[0].a, NULL); ;
    break;}
case 37:
#line 123 "mon_cli.y"
{ start_assemble_mode(yyvsp[-1].a, NULL); ;
    break;}
case 38:
#line 124 "mon_cli.y"
{ disassemble_lines(yyvsp[-1].arange); ;
    break;}
case 39:
#line 125 "mon_cli.y"
{ disassemble_lines(new_range(bad_addr,bad_addr)); ;
    break;}
case 40:
#line 128 "mon_cli.y"
{ move_memory(yyvsp[-2].arange, yyvsp[-1].a); ;
    break;}
case 41:
#line 129 "mon_cli.y"
{ compare_memory(yyvsp[-2].arange, yyvsp[-1].a); ;
    break;}
case 42:
#line 130 "mon_cli.y"
{ fill_memory(yyvsp[-2].arange, yyvsp[-1].str); ;
    break;}
case 43:
#line 131 "mon_cli.y"
{ hunt_memory(yyvsp[-2].arange, yyvsp[-1].str); ;
    break;}
case 44:
#line 132 "mon_cli.y"
{ display_memory(yyvsp[-2].dt, yyvsp[-1].arange); ;
    break;}
case 45:
#line 133 "mon_cli.y"
{ display_memory(0, yyvsp[-1].arange); ;
    break;}
case 46:
#line 134 "mon_cli.y"
{ display_memory(0,new_range(bad_addr,bad_addr)); ;
    break;}
case 47:
#line 137 "mon_cli.y"
{ add_breakpoint(yyvsp[-1].arange, FALSE, FALSE, FALSE); ;
    break;}
case 48:
#line 138 "mon_cli.y"
{ temp = add_breakpoint(yyvsp[-3].arange, FALSE, FALSE, FALSE); 
                                                              set_brkpt_condition(temp, yyvsp[-1].cond_node); ;
    break;}
case 49:
#line 140 "mon_cli.y"
{ add_breakpoint(yyvsp[-1].arange, FALSE, 
                              (yyvsp[-2].i == e_load || yyvsp[-2].i == e_load_store), (yyvsp[-2].i == e_store || yyvsp[-2].i == e_load_store)); ;
    break;}
case 50:
#line 142 "mon_cli.y"
{ add_breakpoint(yyvsp[-1].arange, TRUE, FALSE, FALSE); ;
    break;}
case 51:
#line 143 "mon_cli.y"
{ print_breakpts(); ;
    break;}
case 52:
#line 144 "mon_cli.y"
{ print_breakpts(); ;
    break;}
case 53:
#line 145 "mon_cli.y"
{ print_breakpts(); ;
    break;}
case 54:
#line 149 "mon_cli.y"
{ switch_breakpt(yyvsp[-2].action, yyvsp[-1].i); ;
    break;}
case 55:
#line 150 "mon_cli.y"
{ set_ignore_count(yyvsp[-2].i, yyvsp[-1].i); ;
    break;}
case 56:
#line 151 "mon_cli.y"
{ delete_breakpoint(yyvsp[-1].i); ;
    break;}
case 57:
#line 152 "mon_cli.y"
{ set_brkpt_condition(yyvsp[-3].i, yyvsp[-1].cond_node); ;
    break;}
case 58:
#line 153 "mon_cli.y"
{ set_breakpt_command(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 59:
#line 156 "mon_cli.y"
{ sidefx = ((yyvsp[-1].action==e_TOGGLE)?(sidefx^1):yyvsp[-1].action); ;
    break;}
case 60:
#line 157 "mon_cli.y"
{ fprintf(mon_output, "sidefx %d\n",sidefx); ;
    break;}
case 61:
#line 158 "mon_cli.y"
{ default_datatype = yyvsp[-1].dt; ;
    break;}
case 62:
#line 159 "mon_cli.y"
{ fprintf(mon_output, "Default datatype is %s\n", 
                                      datatype_string[default_datatype]); ;
    break;}
case 63:
#line 161 "mon_cli.y"
{ fprintf(mon_output, "Setting default readspace to %s\n",
                                              SPACESTRING(yyvsp[-1].i)); default_readspace = yyvsp[-1].i; ;
    break;}
case 64:
#line 163 "mon_cli.y"
{ fprintf(mon_output, "Default readspace is %s\n",
                                     SPACESTRING(default_readspace)); ;
    break;}
case 65:
#line 165 "mon_cli.y"
{ fprintf(mon_output, "Setting default writespace to %s\n", 
                                               SPACESTRING(yyvsp[-1].i)); default_writespace = yyvsp[-1].i; ;
    break;}
case 66:
#line 167 "mon_cli.y"
{ fprintf(mon_output,"Default writespace is %s\n",
                                      SPACESTRING(default_writespace)); ;
    break;}
case 67:
#line 169 "mon_cli.y"
{ fprintf(mon_output,"Setting default device to %s\n", 
                                           SPACESTRING(yyvsp[-1].i)); default_readspace = default_writespace = yyvsp[-1].i; ;
    break;}
case 68:
#line 171 "mon_cli.y"
{ printf("Quit.\n"); exit(-1); exit(0); ;
    break;}
case 69:
#line 172 "mon_cli.y"
{ exit_mon = 1; YYACCEPT; ;
    break;}
case 70:
#line 175 "mon_cli.y"
{ execute_disk_command(yyvsp[-1].str); ;
    break;}
case 71:
#line 176 "mon_cli.y"
{ fprintf(mon_output, "\t%d\n",yyvsp[-1].i); ;
    break;}
case 72:
#line 177 "mon_cli.y"
{ print_help(-1); ;
    break;}
case 73:
#line 178 "mon_cli.y"
{ print_help(cmd_lookup_index(yyvsp[-1].str)); ;
    break;}
case 74:
#line 179 "mon_cli.y"
{ printf("SYSTEM COMMAND: %s\n",yyvsp[-1].str); ;
    break;}
case 75:
#line 180 "mon_cli.y"
{ print_convert(yyvsp[-1].i); ;
    break;}
case 76:
#line 181 "mon_cli.y"
{ change_dir(yyvsp[-1].str); ;
    break;}
case 77:
#line 184 "mon_cli.y"
{ mon_load_file(yyvsp[-2].str,yyvsp[-1].a); ;
    break;}
case 78:
#line 185 "mon_cli.y"
{ mon_save_file(yyvsp[-2].str,yyvsp[-1].arange); ;
    break;}
case 79:
#line 186 "mon_cli.y"
{ mon_verify_file(yyvsp[-2].str,yyvsp[-1].a); ;
    break;}
case 80:
#line 189 "mon_cli.y"
{ record_commands(yyvsp[-1].str); ;
    break;}
case 81:
#line 190 "mon_cli.y"
{ end_recording(); ;
    break;}
case 82:
#line 191 "mon_cli.y"
{ playback=TRUE; playback_name = yyvsp[-1].str; ;
    break;}
case 83:
#line 194 "mon_cli.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 84:
#line 197 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 85:
#line 198 "mon_cli.y"
{ yyval.i = e_load_store; ;
    break;}
case 86:
#line 200 "mon_cli.y"
{ print_registers(e_default_space); ;
    break;}
case 87:
#line 201 "mon_cli.y"
{ print_registers(yyvsp[0].i); ;
    break;}
case 91:
#line 209 "mon_cli.y"
{ set_reg_val(yyvsp[-2].reg, default_writespace, yyvsp[0].i); ;
    break;}
case 92:
#line 210 "mon_cli.y"
{ set_reg_val(yyvsp[-2].reg, yyvsp[-3].i, yyvsp[0].i); ;
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
{ yyval.a = new_addr(yyvsp[-1].i,yyvsp[0].i); if (opt_asm) new_cmd = asm_mode = 1; ;
    break;}
case 108:
#line 243 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 109:
#line 244 "mon_cli.y"
{ yyval.i = e_default_space; ;
    break;}
case 110:
#line 247 "mon_cli.y"
{ yyval.i = e_comp_space; ;
    break;}
case 111:
#line 248 "mon_cli.y"
{ yyval.i = e_disk_space; ;
    break;}
case 112:
#line 251 "mon_cli.y"
{ yyval.i = check_addr_limits(yyvsp[0].i); ;
    break;}
case 113:
#line 254 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 114:
#line 256 "mon_cli.y"
{ yyval.i = yyvsp[-2].i + yyvsp[0].i; ;
    break;}
case 115:
#line 257 "mon_cli.y"
{ yyval.i = yyvsp[-2].i - yyvsp[0].i; ;
    break;}
case 116:
#line 258 "mon_cli.y"
{ yyval.i = yyvsp[-2].i * yyvsp[0].i; ;
    break;}
case 117:
#line 259 "mon_cli.y"
{ yyval.i = (yyvsp[0].i) ? (yyvsp[-2].i / yyvsp[0].i) : 1; ;
    break;}
case 118:
#line 260 "mon_cli.y"
{ yyval.i = yyvsp[-1].i; ;
    break;}
case 119:
#line 261 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 120:
#line 264 "mon_cli.y"
{
              yyval.cond_node = new_cond; yyval.cond_node->is_parenthized = FALSE;
              yyval.cond_node->child1 = yyvsp[-2].cond_node; yyval.cond_node->child2 = yyvsp[0].cond_node; yyval.cond_node->operation = yyvsp[-1].cond_op; ;
    break;}
case 121:
#line 267 "mon_cli.y"
{ yyval.cond_node = yyvsp[-1].cond_node; yyval.cond_node->is_parenthized = TRUE; ;
    break;}
case 122:
#line 268 "mon_cli.y"
{ yyval.cond_node = yyvsp[0].cond_node; ;
    break;}
case 123:
#line 271 "mon_cli.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV; yyval.cond_node->is_parenthized = FALSE;
                            yyval.cond_node->reg_num = yyvsp[0].reg; yyval.cond_node->is_reg = TRUE; ;
    break;}
case 124:
#line 273 "mon_cli.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV;  yyval.cond_node->is_parenthized = FALSE;
                            yyval.cond_node->reg_num = yyvsp[0].reg; yyval.cond_node->is_reg = TRUE; ;
    break;}
case 125:
#line 275 "mon_cli.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV; yyval.cond_node->is_parenthized = FALSE;
                            yyval.cond_node->value = yyvsp[0].i; yyval.cond_node->is_reg = FALSE; ;
    break;}
case 128:
#line 283 "mon_cli.y"
{ add_number_to_buffer(yyvsp[0].i); ;
    break;}
case 129:
#line 284 "mon_cli.y"
{ add_string_to_buffer(yyvsp[0].str); ;
    break;}
case 130:
#line 287 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 131:
#line 288 "mon_cli.y"
{ yyval.i = get_reg_val(default_readspace, yyvsp[0].reg); ;
    break;}
case 132:
#line 289 "mon_cli.y"
{ yyval.i = get_reg_val(yyvsp[-1].i, yyvsp[0].reg); ;
    break;}
case 133:
#line 292 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 134:
#line 293 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 135:
#line 294 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 136:
#line 295 "mon_cli.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 139:
#line 302 "mon_cli.y"
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
case 141:
#line 313 "mon_cli.y"
{ asm_mode = 0; ;
    break;}
case 142:
#line 316 "mon_cli.y"
{ yyval.i = new_mode(IMMEDIATE,yyvsp[0].i); ;
    break;}
case 143:
#line 317 "mon_cli.y"
{ if (yyvsp[0].i < 0x100)
                              yyval.i = new_mode(ZERO_PAGE,yyvsp[0].i);
                           else
                              yyval.i = new_mode(ABSOLUTE,yyvsp[0].i);
                         ;
    break;}
case 144:
#line 322 "mon_cli.y"
{ if (yyvsp[-2].i < 0x100)
                                           yyval.i = new_mode(ZERO_PAGE_X,yyvsp[-2].i);
                                        else
                                           yyval.i = new_mode(ABSOLUTE_X,yyvsp[-2].i);
                                      ;
    break;}
case 145:
#line 327 "mon_cli.y"
{ if (yyvsp[-2].i < 0x100)
                                           yyval.i = new_mode(ZERO_PAGE_Y,yyvsp[-2].i);
                                        else
                                           yyval.i = new_mode(ABSOLUTE_Y,yyvsp[-2].i);
                                      ;
    break;}
case 146:
#line 332 "mon_cli.y"
{ yyval.i = new_mode(ABS_INDIRECT,yyvsp[-1].i); ;
    break;}
case 147:
#line 333 "mon_cli.y"
{ yyval.i = new_mode(INDIRECT_X,yyvsp[-3].i); ;
    break;}
case 148:
#line 334 "mon_cli.y"
{ yyval.i = new_mode(INDIRECT_Y,yyvsp[-3].i); ;
    break;}
case 149:
#line 335 "mon_cli.y"
{ yyval.i = new_mode(IMPLIED,0); ;
    break;}
case 150:
#line 336 "mon_cli.y"
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
#line 340 "mon_cli.y"
 

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

