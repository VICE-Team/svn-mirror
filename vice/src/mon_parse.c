
/*  A Bison parser, made from mon_parse.y
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

#line 1 "mon_parse.y"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vice.h"
#include "types.h"
#include "asm.h"
#include "utils.h"

#undef M_ADDR
#include "mon.h"

#define join_ints(x,y) (LO16_TO_HI16(x)|y)
#define separate_int1(x) (HI16_TO_LO16(x))
#define separate_int2(x) (LO16(x))

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

#define ERR_ILLEGAL_INPUT 0
#define ERR_RANGE_BAD_START 1
#define ERR_RANGE_BAD_END 2
#define ERR_BAD_CMD 3
#define ERR_EXPECT_BRKNUM 4
#define ERR_EXPECT_END_CMD 5
#define ERR_MISSING_CLOSE_PAREN 6
#define ERR_INCOMPLETE_COMPARE_OP 7
#define ERR_EXPECT_FILENAME 8


#line 42 "mon_parse.y"
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



#define	YYFINAL		303
#define	YYFLAG		-32768
#define	YYNTBASE	95

#define YYTRANSLATE(x) ((unsigned)(x) <= 342 ? yytranslate[x] : 138)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    93,
    94,    90,    88,     2,    89,     2,    91,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    92,     2,     2,     2,     2,     2,     2,
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
     0,     2,     5,     7,     9,    12,    14,    16,    18,    20,
    22,    24,    26,    28,    30,    32,    34,    36,    38,    40,
    43,    47,    50,    53,    56,    59,    63,    67,    71,    75,
    77,    79,    82,    85,    90,    95,   100,   105,   109,   110,
   116,   120,   124,   127,   132,   137,   142,   147,   152,   156,
   159,   163,   169,   174,   178,   181,   184,   187,   191,   196,
   200,   203,   209,   214,   218,   221,   225,   228,   232,   235,
   239,   242,   246,   249,   252,   256,   260,   263,   267,   271,
   275,   279,   284,   289,   294,   299,   304,   308,   311,   315,
   317,   319,   321,   323,   324,   326,   329,   333,   335,   339,
   341,   342,   344,   346,   349,   354,   359,   361,   364,   367,
   369,   370,   372,   375,   377,   378,   380,   382,   384,   386,
   390,   394,   398,   402,   406,   410,   412,   416,   420,   424,
   428,   430,   432,   434,   437,   439,   441,   443,   445,   447,
   449,   451,   453,   455,   459,   463,   466,   468,   470,   473,
   475,   479,   483,   487,   493,   499,   500
};

static const short yyrhs[] = {    96,
     0,   135,     9,     0,     9,     0,    98,     0,    96,    98,
     0,    15,     0,     9,     0,     1,     0,    99,     0,   101,
     0,   104,     0,   102,     0,   105,     0,   106,     0,   107,
     0,   108,     0,   109,     0,   110,     0,    10,     0,    60,
    97,     0,    31,   122,    97,     0,    39,    97,     0,    19,
    97,     0,    42,    97,     0,    43,    97,     0,    51,   117,
    97,     0,    50,   117,    97,     0,    22,   117,    97,     0,
    23,   117,    97,     0,   100,     0,    32,     0,    32,   124,
     0,    32,   115,     0,    61,   123,   112,    97,     0,    62,
   123,   112,    97,     0,    63,   122,    81,    97,     0,    64,
   123,    81,    97,     0,    65,   123,    97,     0,     0,    48,
   122,   103,   136,    97,     0,    48,   122,    97,     0,    49,
   120,    97,     0,    49,    97,     0,    30,   119,   122,    97,
     0,    41,   119,   122,    97,     0,    29,   119,   130,    97,
     0,    28,   119,   130,    97,     0,    36,    84,   120,    97,
     0,    36,   120,    97,     0,    36,    97,     0,    37,   120,
    97,     0,    37,   120,    12,   128,    97,     0,    55,   113,
   120,    97,     0,    38,   120,    97,     0,    37,    97,     0,
    38,    97,     0,    55,    97,     0,    86,   118,    97,     0,
    27,   118,   117,    97,     0,    45,   118,    97,     0,    45,
    97,     0,    46,   118,    12,   128,    97,     0,    47,   118,
    77,    97,     0,    18,    87,    97,     0,    18,    97,     0,
    35,    84,    97,     0,    35,    97,     0,    33,   124,    97,
     0,    33,    97,     0,    34,   124,    97,     0,    34,    97,
     0,    53,   124,    97,     0,    58,    97,     0,    44,    97,
     0,    56,   111,    97,     0,    52,   127,    97,     0,    54,
    97,     0,    54,   111,    97,     0,    57,   111,    97,     0,
     7,   127,    97,     0,    59,   111,    97,     0,    24,   112,
   122,    97,     0,    25,   112,   119,    97,     0,    26,   112,
   122,    97,     0,    20,   127,   127,   121,     0,    21,   127,
   127,   122,     0,    66,   112,    97,     0,    67,    97,     0,
    68,   112,    97,     0,    79,     0,    78,     0,     1,     0,
    11,     0,     0,    82,     0,   124,    82,     0,   115,    16,
   116,     0,   116,     0,   114,    17,   133,     0,   127,     0,
     0,   133,     0,     1,     0,   122,   122,     0,   122,    92,
    88,   133,     0,   122,    92,    89,   133,     0,     1,     0,
   122,     1,     0,   122,   121,     0,   122,     0,     0,   125,
     0,   124,   125,     0,   124,     0,     0,    13,     0,    14,
     0,   126,     0,   133,     0,   127,    88,   127,     0,   127,
    89,   127,     0,   127,    90,   127,     0,   127,    91,   127,
     0,    93,   127,    94,     0,    93,   127,     1,     0,   132,
     0,   128,    83,   128,     0,   128,    83,     1,     0,    69,
   128,    70,     0,    69,   128,     1,     0,   129,     0,   114,
     0,   133,     0,   130,   131,     0,   131,     0,   133,     0,
    77,     0,   133,     0,   114,     0,     3,     0,     4,     0,
     5,     0,     6,     0,   134,    76,   135,     0,   135,    76,
   135,     0,    80,   137,     0,   135,     0,   134,     0,    71,
   133,     0,   133,     0,   133,    75,    73,     0,   133,    75,
    74,     0,    69,   133,    70,     0,    69,   133,    75,    73,
    70,     0,    69,   133,    70,    75,    74,     0,     0,    72,
     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    92,    93,    94,    97,    98,   101,   102,   103,   106,   107,
   108,   109,   110,   111,   112,   113,   114,   115,   116,   119,
   120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
   132,   133,   134,   137,   138,   139,   140,   141,   144,   144,
   145,   146,   147,   150,   151,   152,   153,   154,   155,   156,
   159,   160,   162,   164,   165,   166,   167,   171,   172,   173,
   174,   175,   176,   179,   180,   181,   182,   184,   186,   188,
   190,   192,   194,   195,   198,   199,   200,   201,   202,   203,
   204,   207,   208,   209,   210,   211,   214,   215,   216,   219,
   222,   223,   226,   227,   230,   231,   234,   235,   238,   241,
   242,   245,   246,   249,   250,   251,   252,   253,   256,   258,
   259,   262,   263,   266,   267,   270,   271,   274,   277,   279,
   280,   281,   282,   283,   284,   285,   288,   291,   292,   293,
   294,   297,   299,   303,   304,   307,   308,   311,   312,   315,
   316,   317,   318,   321,   322,   325,   335,   336,   339,   340,
   345,   350,   355,   356,   357,   358,   359
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
"INPUT_SPEC","CMD_CHECKPT_ONOFF","TOGGLE","'+'","'-'","'*'","'/'","'|'","'('",
"')'","top_level","command_list","end_cmd","command","machine_state_rules","register_mod",
"symbol_table_rules","asm_rules","@1","memory_rules","checkpoint_rules","checkpoint_control_rules",
"monitor_state_rules","monitor_misc_rules","disk_rules","cmd_file_rules","rest_of_line",
"filename","opt_mem_op","register","reg_list","reg_asgn","opt_count","breakpt_num",
"address_range","address_opt_range","opt_address","address","opt_memspace","memspace",
"memloc","memaddr","expression","cond_expr","compare_operand","data_list","data_element",
"value","number","assembly_instr_list","assembly_instruction","post_assemble",
"asm_operand_mode", NULL
};
#endif

static const short yyr1[] = {     0,
    95,    95,    95,    96,    96,    97,    97,    97,    98,    98,
    98,    98,    98,    98,    98,    98,    98,    98,    98,    99,
    99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
   100,   100,   100,   101,   101,   101,   101,   101,   103,   102,
   102,   102,   102,   104,   104,   104,   104,   104,   104,   104,
   105,   105,   105,   105,   105,   105,   105,   106,   106,   106,
   106,   106,   106,   107,   107,   107,   107,   107,   107,   107,
   107,   107,   107,   107,   108,   108,   108,   108,   108,   108,
   108,   109,   109,   109,   109,   109,   110,   110,   110,   111,
   112,   112,   113,   113,   114,   114,   115,   115,   116,   117,
   117,   118,   118,   119,   119,   119,   119,   119,   120,   121,
   121,   122,   122,   123,   123,   124,   124,   125,   126,   127,
   127,   127,   127,   127,   127,   127,   128,   128,   128,   128,
   128,   129,   129,   130,   130,   131,   131,   132,   132,   133,
   133,   133,   133,   134,   134,   135,   136,   136,   137,   137,
   137,   137,   137,   137,   137,   137,   137
};

static const short yyr2[] = {     0,
     1,     2,     1,     1,     2,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
     3,     2,     2,     2,     2,     3,     3,     3,     3,     1,
     1,     2,     2,     4,     4,     4,     4,     3,     0,     5,
     3,     3,     2,     4,     4,     4,     4,     4,     3,     2,
     3,     5,     4,     3,     2,     2,     2,     3,     4,     3,
     2,     5,     4,     3,     2,     3,     2,     3,     2,     3,
     2,     3,     2,     2,     3,     3,     2,     3,     3,     3,
     3,     4,     4,     4,     4,     4,     3,     2,     3,     1,
     1,     1,     1,     0,     1,     2,     3,     1,     3,     1,
     0,     1,     1,     2,     4,     4,     1,     2,     2,     1,
     0,     1,     2,     1,     0,     1,     1,     1,     1,     3,
     3,     3,     3,     3,     3,     1,     3,     3,     3,     3,
     1,     1,     1,     2,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     3,     3,     2,     1,     1,     2,     1,
     3,     3,     3,     5,     5,     0,     1
};

static const short yydefact[] = {     0,
     0,     3,    19,     0,     0,     0,     0,   101,   101,     0,
     0,     0,     0,     0,     0,     0,     0,    31,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   101,   101,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   115,   115,     0,   115,   115,
     0,     0,     0,   156,     0,     1,     4,     9,    30,    10,
    12,    11,    13,    14,    15,    16,    17,    18,     0,   140,
   141,   142,   143,   116,   117,    95,     0,   139,     0,     0,
   126,   138,     8,     7,     6,     0,    65,    23,     0,     0,
     0,   100,     0,    92,    91,     0,     0,     0,   103,   101,
   102,   107,     0,     0,     0,   112,   118,   119,     0,     0,
     0,     0,    33,    98,    32,    69,     0,    71,     0,     0,
    67,     0,    50,     0,   111,    55,     0,    56,     0,    22,
     0,    24,    25,    74,     8,    61,     0,     0,     0,     0,
    43,     0,     0,     0,     0,     0,    90,    77,     0,    93,
    57,     0,     0,     0,    73,     0,    20,     0,   114,     0,
     0,     0,     0,     0,    88,     0,     0,     0,   157,   150,
   146,     0,     5,     2,     0,    96,     0,     0,     0,     0,
    80,    64,   111,     0,    28,    29,     0,     0,     0,     0,
   137,     0,   135,   136,   108,     0,   104,   113,     0,     0,
    21,     0,     0,    68,    70,    66,     0,    49,   109,   110,
     0,    51,    54,     0,    60,     0,     0,    41,     0,    42,
    27,    26,    76,    72,    78,     0,    75,    79,    81,     0,
     0,     0,     0,    38,    87,    89,     0,   149,     0,    58,
   125,   124,   120,   121,   122,   123,    85,    86,    82,    83,
    84,    59,    47,   134,     0,     0,    46,    44,    99,    97,
    48,     0,   132,     0,   131,   133,    45,     0,    63,   148,
   147,     0,    53,    34,    35,    36,    37,   153,     0,   151,
   152,   105,   106,     0,     0,    52,    62,     0,     0,    40,
     0,     0,   130,   129,   128,   127,   144,   145,   155,   154,
     0,     0,     0
};

static const short yydefgoto[] = {   301,
    56,    87,    57,    58,    59,    60,    61,   219,    62,    63,
    64,    65,    66,    67,    68,   149,    96,   152,    78,   113,
   114,    91,   100,   103,   124,   209,   125,   158,   105,   106,
   107,    92,   264,   265,   192,   193,    81,   108,   270,    69,
   272,   171
};

static const short yypact[] = {   504,
   243,-32768,-32768,    18,   250,   243,   243,   243,   243,    14,
    14,    14,   311,   572,   572,   572,   425,     9,   150,   150,
   116,   391,   483,   483,   250,   572,   250,   250,   250,   500,
   311,   311,   425,   483,   243,   243,   243,   115,   173,   468,
   -77,   -77,   250,   -77,   250,   115,   115,   425,   115,   115,
    14,   250,    14,   374,   311,   573,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    -5,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   243,-32768,   -66,   131,
-32768,-32768,-32768,-32768,-32768,   250,-32768,-32768,    80,    80,
   250,   253,   250,-32768,-32768,   425,   572,   425,-32768,   243,
-32768,-32768,   228,    42,   383,-32768,-32768,-32768,   228,   425,
   250,     3,    12,-32768,   -66,-32768,   250,-32768,   250,   250,
-32768,   425,-32768,   250,   425,-32768,   121,-32768,   250,-32768,
   425,-32768,-32768,-32768,   257,-32768,   250,    26,   -36,     2,
-32768,   250,   250,   250,   131,   250,-32768,-32768,   250,-32768,
-32768,   425,   250,   250,-32768,   250,-32768,    14,-32768,    14,
   -37,   -29,   250,   250,-32768,   250,   383,   383,-32768,   -11,
-32768,   250,-32768,-32768,    25,-32768,   243,   243,   243,   243,
-32768,-32768,   276,   276,-32768,-32768,   250,   250,   250,   250,
-32768,   417,-32768,-32768,-32768,    50,-32768,-32768,   417,   250,
-32768,   383,     9,-32768,-32768,-32768,   250,-32768,-32768,-32768,
   355,-32768,-32768,   250,-32768,   355,   250,-32768,    -8,-32768,
-32768,-32768,-32768,-32768,-32768,   250,-32768,-32768,-32768,   250,
   250,   250,   250,-32768,-32768,-32768,   -33,-32768,   107,-32768,
-32768,-32768,    93,    93,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   383,   383,-32768,-32768,-32768,-32768,
-32768,   355,-32768,    72,-32768,-32768,-32768,    72,-32768,     1,
    28,   250,-32768,-32768,-32768,-32768,-32768,     0,    27,-32768,
-32768,-32768,-32768,    20,   293,-32768,-32768,    -8,    -8,-32768,
    37,    48,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   124,   135,-32768
};

static const short yypgoto[] = {-32768,
-32768,   184,    92,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   103,    -2,-32768,   -17,-32768,
   -51,    23,   255,    24,   -10,   -26,   337,    13,    60,    56,
-32768,    64,  -142,-32768,    63,  -174,-32768,    -1,-32768,  -162,
-32768,-32768
};


#define	YYLAST		659


static const short yytable[] = {    82,
   112,   147,    83,   174,    82,    82,    82,    82,    97,    98,
    84,   101,   127,   129,    94,   176,    85,   254,    83,   202,
   293,    74,    75,   142,   254,   241,    84,   203,   101,   101,
   101,    93,    85,    82,    82,    82,   278,   216,   109,   110,
   217,   279,   195,   232,    70,    71,    72,    73,   164,   131,
   166,   233,   170,   101,    74,    75,   271,   143,   144,   160,
    79,   162,   163,   239,    80,    79,    79,    79,    79,    89,
    90,    54,    83,   268,   291,    82,   288,   115,   117,   119,
    84,   -39,    70,    71,    72,    73,    85,    82,    82,   294,
    76,    95,    74,    75,    79,    79,    79,   146,    82,   292,
   145,   194,   285,   289,    86,   159,   159,   194,   159,   159,
   299,   207,   177,   178,   179,   180,    83,   300,   242,   284,
   188,    83,   190,   302,    84,   297,   298,    74,    75,    84,
    85,    83,   211,   196,   303,    85,    79,   255,   256,    84,
   175,   226,   296,   153,   154,    85,   156,   173,    79,    79,
    83,   260,   183,   184,   285,   230,   247,   231,    84,    79,
   198,    76,    74,    75,    85,   237,   238,   177,   178,   179,
   180,   199,    77,    83,     0,    82,    82,    82,    82,   280,
   281,    84,   179,   180,     0,   112,     0,    85,    88,     0,
   194,     0,     0,   263,     0,     0,     0,   194,   263,   120,
   259,     0,   116,   118,   121,   123,   126,   128,   130,   266,
   132,   133,   134,   136,   266,     0,     0,   141,   177,   178,
   179,   180,   148,   151,     0,     0,   155,     0,   157,     0,
    70,    71,    72,    73,     0,   165,    79,    79,    79,    79,
   243,   244,   245,   246,   263,    70,    71,    72,    73,     0,
    83,   147,     0,   282,   283,    74,    75,  -103,    84,     0,
   266,     0,    79,   181,    85,  -103,     0,   263,     0,   182,
    79,  -103,     0,     0,   185,    79,   186,     0,    70,    71,
    72,    73,     0,   266,   137,   138,   139,     0,    74,    75,
     0,     0,     0,   295,   201,    70,    71,    72,    73,     0,
   204,     0,   205,   206,   191,    74,    75,   208,     0,   172,
   212,    99,   213,    70,    71,    72,    73,     0,     0,     0,
   215,    79,     0,   218,    76,   220,   221,   222,   223,   224,
     0,     0,   225,     0,     0,    77,   227,   228,     0,   229,
   177,   178,   179,   180,    79,     0,   234,   235,     0,   236,
   104,   104,   104,   111,     0,   240,     0,    70,    71,    72,
    73,   262,   104,   177,   178,   179,   180,    74,    75,   140,
   249,   250,   251,   252,    76,   253,    70,    71,    72,    73,
     0,     0,   257,   258,   161,    70,    71,    72,    73,     0,
   261,    83,     0,    70,    71,    72,    73,   267,     0,    84,
   269,     0,     0,    74,    75,    85,     0,     0,     0,   273,
     0,     0,     0,   274,   275,   276,   277,    83,     0,    70,
    71,    72,    73,   262,     0,    84,     0,    70,    71,    72,
    73,    85,   187,   104,   189,     0,    76,    74,    75,     0,
   197,     0,   167,     0,   168,   169,   200,   286,     0,     0,
     0,   287,     0,     0,     0,   290,     0,     0,     0,     0,
     0,   210,     0,     0,     0,     0,     0,   214,    83,     0,
   -94,   -94,   -94,   -94,   122,     0,    84,     0,   150,     0,
   -94,   -94,    85,    83,     0,    70,    71,    72,    73,     0,
     0,    84,     0,   191,     0,    74,    75,    85,     0,     0,
   135,     0,    70,    71,    72,    73,     0,     0,    84,     0,
     1,     0,     2,     3,    85,     0,     0,     0,     0,   210,
   248,     4,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
    23,    24,    25,     0,    26,    27,    28,    29,    30,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    52,    53,   102,     0,    70,    71,    72,    73,     0,     1,
     0,     0,     3,    54,    74,    75,     0,     0,     0,    55,
     4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,     0,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    55
};

static const short yycheck[] = {     1,
    18,    79,     1,     9,     6,     7,     8,     9,    11,    12,
     9,    13,    23,    24,     1,    82,    15,   192,     1,    17,
     1,    13,    14,    34,   199,     1,     9,    16,    30,    31,
    32,     9,    15,    35,    36,    37,    70,    12,    15,    16,
    77,    75,     1,    81,     3,     4,     5,     6,    51,    26,
    53,    81,    54,    55,    13,    14,   219,    35,    36,    47,
     1,    49,    50,    75,     1,     6,     7,     8,     9,     6,
     7,    80,     1,   216,    75,    77,    76,    18,    19,    20,
     9,    80,     3,     4,     5,     6,    15,    89,    90,    70,
    82,    78,    13,    14,    35,    36,    37,    38,   100,    73,
    37,   103,    83,    76,    87,    46,    47,   109,    49,    50,
    74,   122,    88,    89,    90,    91,     1,    70,    94,   262,
    97,     1,   100,     0,     9,   288,   289,    13,    14,     9,
    15,     1,    12,    92,     0,    15,    77,    88,    89,     9,
    77,   152,   285,    41,    42,    15,    44,    56,    89,    90,
     1,   203,    89,    90,    83,   158,   183,   160,     9,   100,
   105,    82,    13,    14,    15,   167,   168,    88,    89,    90,
    91,   109,    93,     1,    -1,   177,   178,   179,   180,    73,
    74,     9,    90,    91,    -1,   203,    -1,    15,     5,    -1,
   192,    -1,    -1,   211,    -1,    -1,    -1,   199,   216,    84,
   202,    -1,    19,    20,    21,    22,    23,    24,    25,   211,
    27,    28,    29,    30,   216,    -1,    -1,    34,    88,    89,
    90,    91,    39,    40,    -1,    -1,    43,    -1,    45,    -1,
     3,     4,     5,     6,    -1,    52,   177,   178,   179,   180,
   177,   178,   179,   180,   262,     3,     4,     5,     6,    -1,
     1,    79,    -1,   255,   256,    13,    14,     1,     9,    -1,
   262,    -1,   203,    80,    15,     9,    -1,   285,    -1,    86,
   211,    15,    -1,    -1,    91,   216,    93,    -1,     3,     4,
     5,     6,    -1,   285,    30,    31,    32,    -1,    13,    14,
    -1,    -1,    -1,     1,   111,     3,     4,     5,     6,    -1,
   117,    -1,   119,   120,    77,    13,    14,   124,    -1,    55,
   127,     1,   129,     3,     4,     5,     6,    -1,    -1,    -1,
   137,   262,    -1,   140,    82,   142,   143,   144,   145,   146,
    -1,    -1,   149,    -1,    -1,    93,   153,   154,    -1,   156,
    88,    89,    90,    91,   285,    -1,   163,   164,    -1,   166,
    14,    15,    16,    17,    -1,   172,    -1,     3,     4,     5,
     6,    69,    26,    88,    89,    90,    91,    13,    14,    33,
   187,   188,   189,   190,    82,   192,     3,     4,     5,     6,
    -1,    -1,   199,   200,    48,     3,     4,     5,     6,    -1,
   207,     1,    -1,     3,     4,     5,     6,   214,    -1,     9,
   217,    -1,    -1,    13,    14,    15,    -1,    -1,    -1,   226,
    -1,    -1,    -1,   230,   231,   232,   233,     1,    -1,     3,
     4,     5,     6,    69,    -1,     9,    -1,     3,     4,     5,
     6,    15,    96,    97,    98,    -1,    82,    13,    14,    -1,
   104,    -1,    69,    -1,    71,    72,   110,   264,    -1,    -1,
    -1,   268,    -1,    -1,    -1,   272,    -1,    -1,    -1,    -1,
    -1,   125,    -1,    -1,    -1,    -1,    -1,   131,     1,    -1,
     3,     4,     5,     6,    84,    -1,     9,    -1,    11,    -1,
    13,    14,    15,     1,    -1,     3,     4,     5,     6,    -1,
    -1,     9,    -1,    77,    -1,    13,    14,    15,    -1,    -1,
     1,    -1,     3,     4,     5,     6,    -1,    -1,     9,    -1,
     7,    -1,     9,    10,    15,    -1,    -1,    -1,    -1,   183,
   184,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    -1,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,     1,    -1,     3,     4,     5,     6,    -1,     7,
    -1,    -1,    10,    80,    13,    14,    -1,    -1,    -1,    86,
    18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    -1,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
    58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86
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
#line 94 "mon_parse.y"
{ new_cmd = 1; asm_mode = 0; ;
    break;}
case 8:
#line 103 "mon_parse.y"
{ return ERR_EXPECT_END_CMD; ;
    break;}
case 19:
#line 116 "mon_parse.y"
{ return ERR_BAD_CMD; ;
    break;}
case 20:
#line 119 "mon_parse.y"
{ fprintf(mon_output, "Bank command not done yet.\n"); ;
    break;}
case 21:
#line 120 "mon_parse.y"
{ jump(yyvsp[-1].a); ;
    break;}
case 22:
#line 121 "mon_parse.y"
{ fprintf(mon_output, "Display IO registers\n"); ;
    break;}
case 23:
#line 122 "mon_parse.y"
{ fprintf(mon_output, "Continue until RTS/RTI\n"); ;
    break;}
case 24:
#line 123 "mon_parse.y"
{ puts("Dump machine state."); ;
    break;}
case 25:
#line 124 "mon_parse.y"
{ puts("Undump machine state."); ;
    break;}
case 26:
#line 125 "mon_parse.y"
{ instructions_step(yyvsp[-1].i); ;
    break;}
case 27:
#line 126 "mon_parse.y"
{ instructions_next(yyvsp[-1].i); ;
    break;}
case 28:
#line 127 "mon_parse.y"
{ stack_up(yyvsp[-1].i); ;
    break;}
case 29:
#line 128 "mon_parse.y"
{ stack_down(yyvsp[-1].i); ;
    break;}
case 31:
#line 132 "mon_parse.y"
{ print_registers(e_default_space); ;
    break;}
case 32:
#line 133 "mon_parse.y"
{ print_registers(yyvsp[0].i); ;
    break;}
case 34:
#line 137 "mon_parse.y"
{ mon_load_symbols(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 35:
#line 138 "mon_parse.y"
{ mon_save_symbols(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 36:
#line 139 "mon_parse.y"
{ add_name_to_symbol_table(yyvsp[-2].a, yyvsp[-1].str); ;
    break;}
case 37:
#line 140 "mon_parse.y"
{ remove_name_from_symbol_table(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 38:
#line 141 "mon_parse.y"
{ print_symbol_table(yyvsp[-1].i); ;
    break;}
case 39:
#line 144 "mon_parse.y"
{ start_assemble_mode(yyvsp[0].a, NULL); ;
    break;}
case 41:
#line 145 "mon_parse.y"
{ start_assemble_mode(yyvsp[-1].a, NULL); ;
    break;}
case 42:
#line 146 "mon_parse.y"
{ disassemble_lines(yyvsp[-1].arange); ;
    break;}
case 43:
#line 147 "mon_parse.y"
{ disassemble_lines(new_range(bad_addr,bad_addr)); ;
    break;}
case 44:
#line 150 "mon_parse.y"
{ move_memory(yyvsp[-2].arange, yyvsp[-1].a); ;
    break;}
case 45:
#line 151 "mon_parse.y"
{ compare_memory(yyvsp[-2].arange, yyvsp[-1].a); ;
    break;}
case 46:
#line 152 "mon_parse.y"
{ fill_memory(yyvsp[-2].arange, yyvsp[-1].str); ;
    break;}
case 47:
#line 153 "mon_parse.y"
{ hunt_memory(yyvsp[-2].arange, yyvsp[-1].str); ;
    break;}
case 48:
#line 154 "mon_parse.y"
{ display_memory(yyvsp[-2].dt, yyvsp[-1].arange); ;
    break;}
case 49:
#line 155 "mon_parse.y"
{ display_memory(0, yyvsp[-1].arange); ;
    break;}
case 50:
#line 156 "mon_parse.y"
{ display_memory(0,new_range(bad_addr,bad_addr)); ;
    break;}
case 51:
#line 159 "mon_parse.y"
{ add_breakpoint(yyvsp[-1].arange, FALSE, FALSE, FALSE); ;
    break;}
case 52:
#line 160 "mon_parse.y"
{ temp = add_breakpoint(yyvsp[-3].arange, FALSE, FALSE, FALSE); 
                                                              set_brkpt_condition(temp, yyvsp[-1].cond_node); ;
    break;}
case 53:
#line 162 "mon_parse.y"
{ add_breakpoint(yyvsp[-1].arange, FALSE, 
                              (yyvsp[-2].i == e_load || yyvsp[-2].i == e_load_store), (yyvsp[-2].i == e_store || yyvsp[-2].i == e_load_store)); ;
    break;}
case 54:
#line 164 "mon_parse.y"
{ add_breakpoint(yyvsp[-1].arange, TRUE, FALSE, FALSE); ;
    break;}
case 55:
#line 165 "mon_parse.y"
{ print_breakpts(); ;
    break;}
case 56:
#line 166 "mon_parse.y"
{ print_breakpts(); ;
    break;}
case 57:
#line 167 "mon_parse.y"
{ print_breakpts(); ;
    break;}
case 58:
#line 171 "mon_parse.y"
{ switch_breakpt(yyvsp[-2].action, yyvsp[-1].i); ;
    break;}
case 59:
#line 172 "mon_parse.y"
{ set_ignore_count(yyvsp[-2].i, yyvsp[-1].i); ;
    break;}
case 60:
#line 173 "mon_parse.y"
{ delete_breakpoint(yyvsp[-1].i); ;
    break;}
case 61:
#line 174 "mon_parse.y"
{ delete_breakpoint(-1); ;
    break;}
case 62:
#line 175 "mon_parse.y"
{ set_brkpt_condition(yyvsp[-3].i, yyvsp[-1].cond_node); ;
    break;}
case 63:
#line 176 "mon_parse.y"
{ set_breakpt_command(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 64:
#line 179 "mon_parse.y"
{ sidefx = ((yyvsp[-1].action==e_TOGGLE)?(sidefx^1):yyvsp[-1].action); ;
    break;}
case 65:
#line 180 "mon_parse.y"
{ fprintf(mon_output, "sidefx %d\n",sidefx); ;
    break;}
case 66:
#line 181 "mon_parse.y"
{ default_datatype = yyvsp[-1].dt; ;
    break;}
case 67:
#line 182 "mon_parse.y"
{ fprintf(mon_output, "Default datatype is %s\n", 
                                      datatype_string[default_datatype]); ;
    break;}
case 68:
#line 184 "mon_parse.y"
{ fprintf(mon_output, "Setting default readspace to %s\n",
                                              SPACESTRING(yyvsp[-1].i)); default_readspace = yyvsp[-1].i; ;
    break;}
case 69:
#line 186 "mon_parse.y"
{ fprintf(mon_output, "Default readspace is %s\n",
                                     SPACESTRING(default_readspace)); ;
    break;}
case 70:
#line 188 "mon_parse.y"
{ fprintf(mon_output, "Setting default writespace to %s\n", 
                                               SPACESTRING(yyvsp[-1].i)); default_writespace = yyvsp[-1].i; ;
    break;}
case 71:
#line 190 "mon_parse.y"
{ fprintf(mon_output,"Default writespace is %s\n",
                                      SPACESTRING(default_writespace)); ;
    break;}
case 72:
#line 192 "mon_parse.y"
{ fprintf(mon_output,"Setting default device to %s\n", 
                                           SPACESTRING(yyvsp[-1].i)); default_readspace = default_writespace = yyvsp[-1].i; ;
    break;}
case 73:
#line 194 "mon_parse.y"
{ printf("Quit.\n"); exit(-1); exit(0); ;
    break;}
case 74:
#line 195 "mon_parse.y"
{ exit_mon = 1; YYACCEPT; ;
    break;}
case 75:
#line 198 "mon_parse.y"
{ execute_disk_command(yyvsp[-1].str); ;
    break;}
case 76:
#line 199 "mon_parse.y"
{ fprintf(mon_output, "\t%d\n",yyvsp[-1].i); ;
    break;}
case 77:
#line 200 "mon_parse.y"
{ print_help(-1); ;
    break;}
case 78:
#line 201 "mon_parse.y"
{ print_help(cmd_lookup_index(yyvsp[-1].str)); ;
    break;}
case 79:
#line 202 "mon_parse.y"
{ printf("SYSTEM COMMAND: %s\n",yyvsp[-1].str); ;
    break;}
case 80:
#line 203 "mon_parse.y"
{ print_convert(yyvsp[-1].i); ;
    break;}
case 81:
#line 204 "mon_parse.y"
{ change_dir(yyvsp[-1].str); ;
    break;}
case 82:
#line 207 "mon_parse.y"
{ mon_load_file(yyvsp[-2].str,yyvsp[-1].a); ;
    break;}
case 83:
#line 208 "mon_parse.y"
{ mon_save_file(yyvsp[-2].str,yyvsp[-1].arange); ;
    break;}
case 84:
#line 209 "mon_parse.y"
{ mon_verify_file(yyvsp[-2].str,yyvsp[-1].a); ;
    break;}
case 85:
#line 210 "mon_parse.y"
{ block_cmd(0,yyvsp[-2].i,yyvsp[-1].i,yyvsp[0].a); ;
    break;}
case 86:
#line 211 "mon_parse.y"
{ block_cmd(1,yyvsp[-2].i,yyvsp[-1].i,yyvsp[0].a); ;
    break;}
case 87:
#line 214 "mon_parse.y"
{ record_commands(yyvsp[-1].str); ;
    break;}
case 88:
#line 215 "mon_parse.y"
{ end_recording(); ;
    break;}
case 89:
#line 216 "mon_parse.y"
{ playback=TRUE; playback_name = yyvsp[-1].str; ;
    break;}
case 90:
#line 219 "mon_parse.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 92:
#line 223 "mon_parse.y"
{ return ERR_EXPECT_FILENAME; ;
    break;}
case 93:
#line 226 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 94:
#line 227 "mon_parse.y"
{ yyval.i = e_load_store; ;
    break;}
case 95:
#line 230 "mon_parse.y"
{ yyval.i = join_ints(default_readspace, yyvsp[0].reg); ;
    break;}
case 96:
#line 231 "mon_parse.y"
{ yyval.i = join_ints(yyvsp[-1].i, yyvsp[0].reg); ;
    break;}
case 99:
#line 238 "mon_parse.y"
{ set_reg_val(separate_int1(yyvsp[-2].i), separate_int2(yyvsp[-2].i), yyvsp[0].i); ;
    break;}
case 100:
#line 241 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 101:
#line 242 "mon_parse.y"
{ yyval.i = -1; ;
    break;}
case 102:
#line 245 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 103:
#line 246 "mon_parse.y"
{ return ERR_EXPECT_BRKNUM; ;
    break;}
case 104:
#line 249 "mon_parse.y"
{ yyval.arange = new_range(yyvsp[-1].a,yyvsp[0].a); ;
    break;}
case 105:
#line 250 "mon_parse.y"
{ yyval.arange = new_range(yyvsp[-3].a,new_addr(e_default_space,addr_location(yyvsp[-3].a)+yyvsp[0].i)); ;
    break;}
case 106:
#line 251 "mon_parse.y"
{ yyval.arange = new_range(yyvsp[-3].a,new_addr(e_default_space,addr_location(yyvsp[-3].a)-yyvsp[0].i)); ;
    break;}
case 107:
#line 252 "mon_parse.y"
{ return ERR_RANGE_BAD_START; ;
    break;}
case 108:
#line 253 "mon_parse.y"
{ return ERR_RANGE_BAD_END; ;
    break;}
case 109:
#line 256 "mon_parse.y"
{ yyval.arange = new_range(yyvsp[-1].a,yyvsp[0].a); ;
    break;}
case 110:
#line 258 "mon_parse.y"
{ yyval.a = yyvsp[0].a; ;
    break;}
case 111:
#line 259 "mon_parse.y"
{ yyval.a = bad_addr; ;
    break;}
case 112:
#line 262 "mon_parse.y"
{ yyval.a = new_addr(e_default_space,yyvsp[0].i); if (opt_asm) new_cmd = asm_mode = 1; ;
    break;}
case 113:
#line 263 "mon_parse.y"
{ yyval.a = new_addr(yyvsp[-1].i,yyvsp[0].i); if (opt_asm) new_cmd = asm_mode = 1; ;
    break;}
case 114:
#line 266 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 115:
#line 267 "mon_parse.y"
{ yyval.i = e_default_space; ;
    break;}
case 116:
#line 270 "mon_parse.y"
{ yyval.i = e_comp_space; ;
    break;}
case 117:
#line 271 "mon_parse.y"
{ yyval.i = e_disk_space; ;
    break;}
case 118:
#line 274 "mon_parse.y"
{ yyval.i = check_addr_limits(yyvsp[0].i); ;
    break;}
case 119:
#line 277 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 120:
#line 279 "mon_parse.y"
{ yyval.i = yyvsp[-2].i + yyvsp[0].i; ;
    break;}
case 121:
#line 280 "mon_parse.y"
{ yyval.i = yyvsp[-2].i - yyvsp[0].i; ;
    break;}
case 122:
#line 281 "mon_parse.y"
{ yyval.i = yyvsp[-2].i * yyvsp[0].i; ;
    break;}
case 123:
#line 282 "mon_parse.y"
{ yyval.i = (yyvsp[0].i) ? (yyvsp[-2].i / yyvsp[0].i) : 1; ;
    break;}
case 124:
#line 283 "mon_parse.y"
{ yyval.i = yyvsp[-1].i; ;
    break;}
case 125:
#line 284 "mon_parse.y"
{ return ERR_MISSING_CLOSE_PAREN; ;
    break;}
case 126:
#line 285 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 127:
#line 288 "mon_parse.y"
{
              yyval.cond_node = new_cond; yyval.cond_node->is_parenthized = FALSE;
              yyval.cond_node->child1 = yyvsp[-2].cond_node; yyval.cond_node->child2 = yyvsp[0].cond_node; yyval.cond_node->operation = yyvsp[-1].cond_op; ;
    break;}
case 128:
#line 291 "mon_parse.y"
{ return ERR_INCOMPLETE_COMPARE_OP; ;
    break;}
case 129:
#line 292 "mon_parse.y"
{ yyval.cond_node = yyvsp[-1].cond_node; yyval.cond_node->is_parenthized = TRUE; ;
    break;}
case 130:
#line 293 "mon_parse.y"
{ return ERR_MISSING_CLOSE_PAREN; ;
    break;}
case 131:
#line 294 "mon_parse.y"
{ yyval.cond_node = yyvsp[0].cond_node; ;
    break;}
case 132:
#line 297 "mon_parse.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV; yyval.cond_node->is_parenthized = FALSE;
                            yyval.cond_node->reg_num = separate_int2(yyvsp[0].i); yyval.cond_node->is_reg = TRUE; ;
    break;}
case 133:
#line 299 "mon_parse.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV; yyval.cond_node->is_parenthized = FALSE;
                            yyval.cond_node->value = yyvsp[0].i; yyval.cond_node->is_reg = FALSE; ;
    break;}
case 136:
#line 307 "mon_parse.y"
{ add_number_to_buffer(yyvsp[0].i); ;
    break;}
case 137:
#line 308 "mon_parse.y"
{ add_string_to_buffer(yyvsp[0].str); ;
    break;}
case 138:
#line 311 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 139:
#line 312 "mon_parse.y"
{ yyval.i = get_reg_val(separate_int1(yyvsp[0].i), separate_int2(yyvsp[0].i)); ;
    break;}
case 140:
#line 315 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 141:
#line 316 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 142:
#line 317 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 143:
#line 318 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 146:
#line 325 "mon_parse.y"
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
case 148:
#line 336 "mon_parse.y"
{ asm_mode = 0; ;
    break;}
case 149:
#line 339 "mon_parse.y"
{ yyval.i = join_ints(IMMEDIATE,yyvsp[0].i); ;
    break;}
case 150:
#line 340 "mon_parse.y"
{ if (yyvsp[0].i < 0x100)
                              yyval.i = join_ints(ZERO_PAGE,yyvsp[0].i);
                           else
                              yyval.i = join_ints(ABSOLUTE,yyvsp[0].i);
                         ;
    break;}
case 151:
#line 345 "mon_parse.y"
{ if (yyvsp[-2].i < 0x100)
                                           yyval.i = join_ints(ZERO_PAGE_X,yyvsp[-2].i);
                                        else
                                           yyval.i = join_ints(ABSOLUTE_X,yyvsp[-2].i);
                                      ;
    break;}
case 152:
#line 350 "mon_parse.y"
{ if (yyvsp[-2].i < 0x100)
                                           yyval.i = join_ints(ZERO_PAGE_Y,yyvsp[-2].i);
                                        else
                                           yyval.i = join_ints(ABSOLUTE_Y,yyvsp[-2].i);
                                      ;
    break;}
case 153:
#line 355 "mon_parse.y"
{ yyval.i = join_ints(ABS_INDIRECT,yyvsp[-1].i); ;
    break;}
case 154:
#line 356 "mon_parse.y"
{ yyval.i = join_ints(INDIRECT_X,yyvsp[-3].i); ;
    break;}
case 155:
#line 357 "mon_parse.y"
{ yyval.i = join_ints(INDIRECT_Y,yyvsp[-3].i); ;
    break;}
case 156:
#line 358 "mon_parse.y"
{ yyval.i = join_ints(IMPLIED,0); ;
    break;}
case 157:
#line 359 "mon_parse.y"
{ yyval.i = join_ints(ACCUMULATOR,0); ;
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
#line 363 "mon_parse.y"
 

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
       switch(rc) {
           case ERR_BAD_CMD:
               fprintf(mon_output, "Bad command:\n  %s\n", input);
               break;
           case ERR_RANGE_BAD_START:
               fprintf(mon_output, "Bad first address in range:\n  %s\n", input);
               break;
           case ERR_RANGE_BAD_END:
               fprintf(mon_output, "Bad second address in range:\n  %s\n", input);
               break;
           case ERR_EXPECT_BRKNUM:
               fprintf(mon_output, "Checkpoint number expected:\n  %s\n", input);
               break;
           case ERR_EXPECT_END_CMD:
               fprintf(mon_output, "Newline or ';'  expected:\n  %s\n", input);
               break;
           case ERR_MISSING_CLOSE_PAREN:
               fprintf(mon_output, "')' expected:\n  %s\n", input);
               break;
           case ERR_INCOMPLETE_COMPARE_OP:
               fprintf(mon_output, "Compare operation missing an operand\n  %s\n", input);
               break;
           case ERR_EXPECT_FILENAME:
               fprintf(mon_output, "Expecting a filename\n  %s\n", input);
               break;
           default:
               fprintf(mon_output, "Illegal input:\n  %s\n", input);
       }
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

