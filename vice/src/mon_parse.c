
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
#define	CMD_RADIX	290
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
#define	CMD_CHAR_DISPLAY	324
#define	CMD_SPRITE_DISPLAY	325
#define	L_PAREN	326
#define	R_PAREN	327
#define	ARG_IMMEDIATE	328
#define	REG_A	329
#define	REG_X	330
#define	REG_Y	331
#define	COMMA	332
#define	INST_SEP	333
#define	STRING	334
#define	FILENAME	335
#define	R_O_L	336
#define	OPCODE	337
#define	LABEL	338
#define	REGISTER	339
#define	COMPARE_OP	340
#define	RADIX_TYPE	341
#define	INPUT_SPEC	342
#define	CMD_CHECKPT_ONOFF	343
#define	TOGGLE	344

#line 1 "mon_parse.y"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vice.h"
#include "types.h"
#include "asm.h"
#include "utils.h"
#include "mon.h"

#define join_ints(x,y) (LO16_TO_HI16(x)|y)
#define separate_int1(x) (HI16_TO_LO16(x))
#define separate_int2(x) (LO16(x))

static int yyerror(char *s);
static unsigned check_addr_limits(ADDRESS val);
static int temp;

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

#define BAD_ADDR (new_addr(e_invalid_space, 0))


#line 40 "mon_parse.y"
typedef union {
	MON_ADDR a;
        int i;
        REG_ID reg;
        CONDITIONAL cond_op;
        CONDITIONAL_NODE *cond_node;
        RADIXTYPE rt;
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



#define	YYFINAL		305
#define	YYFLAG		-32768
#define	YYNTBASE	96

#define YYTRANSLATE(x) ((unsigned)(x) <= 344 ? yytranslate[x] : 137)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    94,
    95,    92,    90,     2,    91,     2,    93,     2,     2,     2,
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
    86,    87,    88,    89
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     7,     9,    12,    14,    16,    18,    20,
    22,    24,    26,    28,    30,    32,    34,    36,    38,    40,
    43,    47,    50,    53,    56,    59,    63,    67,    71,    75,
    77,    79,    82,    85,    90,    95,   100,   105,   109,   110,
   116,   120,   125,   128,   134,   140,   146,   152,   158,   163,
   166,   171,   174,   179,   182,   187,   194,   200,   205,   208,
   211,   214,   218,   223,   227,   230,   236,   241,   245,   248,
   252,   255,   259,   262,   265,   269,   273,   276,   280,   284,
   288,   292,   297,   303,   308,   313,   318,   322,   325,   329,
   331,   333,   335,   337,   338,   340,   343,   347,   349,   353,
   355,   356,   358,   360,   362,   363,   365,   368,   370,   371,
   373,   375,   377,   379,   383,   387,   391,   395,   399,   403,
   405,   409,   413,   417,   421,   423,   425,   427,   430,   432,
   434,   436,   438,   440,   442,   444,   446,   448,   452,   456,
   459,   461,   463,   466,   468,   472,   476,   480,   486,   492,
   493
};

static const short yyrhs[] = {    97,
     0,   134,     9,     0,     9,     0,    99,     0,    97,    99,
     0,    15,     0,     9,     0,     1,     0,   100,     0,   102,
     0,   105,     0,   103,     0,   106,     0,   107,     0,   108,
     0,   109,     0,   110,     0,   111,     0,    10,     0,    60,
    98,     0,    31,   121,    98,     0,    39,    98,     0,    19,
    98,     0,    42,    98,     0,    43,    98,     0,    51,   118,
    98,     0,    50,   118,    98,     0,    22,   118,    98,     0,
    23,   118,    98,     0,   101,     0,    32,     0,    32,   123,
     0,    32,   116,     0,    61,   122,   113,    98,     0,    62,
   122,   113,    98,     0,    63,   121,    83,    98,     0,    64,
   122,    83,    98,     0,    65,   122,    98,     0,     0,    48,
   121,   104,   135,    98,     0,    48,   121,    98,     0,    49,
   121,   120,    98,     0,    49,    98,     0,    30,   121,   121,
   121,    98,     0,    41,   121,   121,   121,    98,     0,    29,
   121,   121,   129,    98,     0,    28,   121,   121,   129,    98,
     0,    36,    86,   121,   120,    98,     0,    36,   121,   120,
    98,     0,    36,    98,     0,    69,   121,   120,    98,     0,
    69,    98,     0,    70,   121,   120,    98,     0,    70,    98,
     0,    37,   121,   120,    98,     0,    37,   121,   120,    12,
   127,    98,     0,    55,   114,   121,   120,    98,     0,    38,
   121,   120,    98,     0,    37,    98,     0,    38,    98,     0,
    55,    98,     0,    88,   119,    98,     0,    27,   119,   118,
    98,     0,    45,   119,    98,     0,    45,    98,     0,    46,
   119,    12,   127,    98,     0,    47,   119,    79,    98,     0,
    18,    89,    98,     0,    18,    98,     0,    35,    86,    98,
     0,    35,    98,     0,    53,   123,    98,     0,    58,    98,
     0,    44,    98,     0,    56,   112,    98,     0,    52,   126,
    98,     0,    54,    98,     0,    54,   112,    98,     0,    57,
   112,    98,     0,     7,   126,    98,     0,    59,   112,    98,
     0,    24,   113,   121,    98,     0,    25,   113,   121,   121,
    98,     0,    26,   113,   121,    98,     0,    20,   126,   126,
   120,     0,    21,   126,   126,   121,     0,    66,   113,    98,
     0,    67,    98,     0,    68,   113,    98,     0,    81,     0,
    80,     0,     1,     0,    11,     0,     0,    84,     0,   123,
    84,     0,   116,    16,   117,     0,   117,     0,   115,    17,
   132,     0,   126,     0,     0,   132,     0,     1,     0,   121,
     0,     0,   124,     0,   123,   124,     0,   123,     0,     0,
    13,     0,    14,     0,   125,     0,   132,     0,   126,    90,
   126,     0,   126,    91,   126,     0,   126,    92,   126,     0,
   126,    93,   126,     0,    94,   126,    95,     0,    94,   126,
     1,     0,   131,     0,   127,    85,   127,     0,   127,    85,
     1,     0,    71,   127,    72,     0,    71,   127,     1,     0,
   128,     0,   115,     0,   132,     0,   129,   130,     0,   130,
     0,   132,     0,    79,     0,   132,     0,   115,     0,     3,
     0,     4,     0,     5,     0,     6,     0,   133,    78,   134,
     0,   134,    78,   134,     0,    82,   136,     0,   134,     0,
   133,     0,    73,   132,     0,   132,     0,   132,    77,    75,
     0,   132,    77,    76,     0,    71,   132,    72,     0,    71,
   132,    77,    75,    72,     0,    71,   132,    72,    77,    76,
     0,     0,    74,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    88,    89,    90,    93,    94,    97,    98,    99,   102,   103,
   104,   105,   106,   107,   108,   109,   110,   111,   112,   115,
   116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
   128,   129,   130,   133,   134,   135,   136,   137,   140,   140,
   141,   142,   143,   146,   147,   148,   149,   150,   151,   152,
   153,   154,   155,   156,   159,   160,   163,   165,   166,   167,
   168,   172,   173,   174,   175,   176,   177,   180,   181,   182,
   183,   185,   187,   188,   191,   192,   193,   194,   195,   196,
   197,   200,   201,   202,   203,   204,   207,   208,   209,   212,
   215,   216,   219,   220,   223,   224,   227,   228,   231,   234,
   235,   238,   239,   242,   243,   246,   247,   250,   251,   254,
   255,   258,   261,   263,   264,   265,   266,   267,   268,   269,
   272,   274,   275,   276,   277,   280,   282,   286,   287,   290,
   291,   294,   295,   298,   299,   300,   301,   304,   305,   308,
   318,   319,   322,   323,   328,   333,   338,   339,   340,   341,
   342
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","H_NUMBER",
"D_NUMBER","O_NUMBER","B_NUMBER","CONVERT_OP","B_DATA","TRAIL","BAD_CMD","MEM_OP",
"IF","MEM_COMP","MEM_DISK","CMD_SEP","REG_ASGN_SEP","EQUALS","CMD_SIDEFX","CMD_RETURN",
"CMD_BLOCK_READ","CMD_BLOCK_WRITE","CMD_UP","CMD_DOWN","CMD_LOAD","CMD_SAVE",
"CMD_VERIFY","CMD_IGNORE","CMD_HUNT","CMD_FILL","CMD_MOVE","CMD_GOTO","CMD_REGISTERS",
"CMD_READSPACE","CMD_WRITESPACE","CMD_RADIX","CMD_MEM_DISPLAY","CMD_BREAK","CMD_TRACE",
"CMD_IO","CMD_BRMON","CMD_COMPARE","CMD_DUMP","CMD_UNDUMP","CMD_EXIT","CMD_DELETE",
"CMD_CONDITION","CMD_COMMAND","CMD_ASSEMBLE","CMD_DISASSEMBLE","CMD_NEXT","CMD_STEP",
"CMD_PRINT","CMD_DEVICE","CMD_HELP","CMD_WATCH","CMD_DISK","CMD_SYSTEM","CMD_QUIT",
"CMD_CHDIR","CMD_BANK","CMD_LOAD_LABELS","CMD_SAVE_LABELS","CMD_ADD_LABEL","CMD_DEL_LABEL",
"CMD_SHOW_LABELS","CMD_RECORD","CMD_STOP","CMD_PLAYBACK","CMD_CHAR_DISPLAY",
"CMD_SPRITE_DISPLAY","L_PAREN","R_PAREN","ARG_IMMEDIATE","REG_A","REG_X","REG_Y",
"COMMA","INST_SEP","STRING","FILENAME","R_O_L","OPCODE","LABEL","REGISTER","COMPARE_OP",
"RADIX_TYPE","INPUT_SPEC","CMD_CHECKPT_ONOFF","TOGGLE","'+'","'-'","'*'","'/'",
"'('","')'","top_level","command_list","end_cmd","command","machine_state_rules",
"register_mod","symbol_table_rules","asm_rules","@1","memory_rules","checkpoint_rules",
"checkpoint_control_rules","monitor_state_rules","monitor_misc_rules","disk_rules",
"cmd_file_rules","rest_of_line","filename","opt_mem_op","register","reg_list",
"reg_asgn","opt_count","breakpt_num","opt_address","address","opt_memspace",
"memspace","memloc","memaddr","expression","cond_expr","compare_operand","data_list",
"data_element","value","number","assembly_instr_list","assembly_instruction",
"post_assemble","asm_operand_mode", NULL
};
#endif

static const short yyr1[] = {     0,
    96,    96,    96,    97,    97,    98,    98,    98,    99,    99,
    99,    99,    99,    99,    99,    99,    99,    99,    99,   100,
   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
   101,   101,   101,   102,   102,   102,   102,   102,   104,   103,
   103,   103,   103,   105,   105,   105,   105,   105,   105,   105,
   105,   105,   105,   105,   106,   106,   106,   106,   106,   106,
   106,   107,   107,   107,   107,   107,   107,   108,   108,   108,
   108,   108,   108,   108,   109,   109,   109,   109,   109,   109,
   109,   110,   110,   110,   110,   110,   111,   111,   111,   112,
   113,   113,   114,   114,   115,   115,   116,   116,   117,   118,
   118,   119,   119,   120,   120,   121,   121,   122,   122,   123,
   123,   124,   125,   126,   126,   126,   126,   126,   126,   126,
   127,   127,   127,   127,   127,   128,   128,   129,   129,   130,
   130,   131,   131,   132,   132,   132,   132,   133,   133,   134,
   135,   135,   136,   136,   136,   136,   136,   136,   136,   136,
   136
};

static const short yyr2[] = {     0,
     1,     2,     1,     1,     2,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
     3,     2,     2,     2,     2,     3,     3,     3,     3,     1,
     1,     2,     2,     4,     4,     4,     4,     3,     0,     5,
     3,     4,     2,     5,     5,     5,     5,     5,     4,     2,
     4,     2,     4,     2,     4,     6,     5,     4,     2,     2,
     2,     3,     4,     3,     2,     5,     4,     3,     2,     3,
     2,     3,     2,     2,     3,     3,     2,     3,     3,     3,
     3,     4,     5,     4,     4,     4,     3,     2,     3,     1,
     1,     1,     1,     0,     1,     2,     3,     1,     3,     1,
     0,     1,     1,     1,     0,     1,     2,     1,     0,     1,
     1,     1,     1,     3,     3,     3,     3,     3,     3,     1,
     3,     3,     3,     3,     1,     1,     1,     2,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     3,     3,     2,
     1,     1,     2,     1,     3,     3,     3,     5,     5,     0,
     1
};

static const short yydefact[] = {     0,
     0,     3,    19,     0,     0,     0,     0,   101,   101,     0,
     0,     0,     0,     0,     0,     0,     0,    31,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   101,   101,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   109,   109,     0,   109,   109,     0,     0,
     0,     0,     0,   150,     0,     1,     4,     9,    30,    10,
    12,    11,    13,    14,    15,    16,    17,    18,     0,   134,
   135,   136,   137,   110,   111,    95,     0,   133,     0,     0,
   120,   132,     8,     7,     6,     0,    69,    23,     0,     0,
     0,   100,     0,    92,    91,     0,     0,     0,   103,   101,
   102,     0,     0,   106,   112,   113,     0,     0,     0,     0,
    33,    98,    32,     0,    71,     0,    50,   105,    59,   105,
    60,   105,    22,     0,    24,    25,    74,     8,    65,     0,
     0,     0,     0,    43,   105,     0,     0,     0,     0,    90,
    77,     0,    93,    61,     0,     0,     0,    73,     0,    20,
     0,   108,     0,     0,     0,     0,     0,    88,     0,    52,
   105,    54,   105,     0,     0,   151,   144,   140,     0,     5,
     2,     0,    96,     0,     0,     0,     0,    80,    68,   105,
     0,    28,    29,     0,     0,     0,     0,     0,   107,     0,
     0,    21,     0,     0,    70,   105,     0,   104,     0,     0,
     0,    64,     0,     0,    41,     0,     0,    27,    26,    76,
    72,    78,   105,    75,    79,    81,     0,     0,     0,     0,
    38,    87,    89,     0,     0,     0,   143,     0,    62,   119,
   118,   114,   115,   116,   117,    85,    86,    82,     0,    84,
    63,   131,     0,   129,   130,     0,     0,    99,    97,     0,
    49,     0,    55,    58,     0,     0,   126,     0,   125,   127,
    67,   142,   141,     0,    42,     0,    34,    35,    36,    37,
    51,    53,   147,     0,   145,   146,    83,    47,   128,    46,
    44,    48,     0,    45,     0,     0,    66,     0,     0,    40,
    57,     0,     0,    56,   124,   123,   122,   121,   138,   139,
   149,   148,     0,     0,     0
};

static const short yydefgoto[] = {   303,
    56,    87,    57,    58,    59,    60,    61,   206,    62,    63,
    64,    65,    66,    67,    68,   142,    96,   145,    78,   111,
   112,    91,   100,   197,   198,   151,   103,   104,   105,    92,
   258,   259,   243,   244,    81,   106,   262,    69,   264,   168
};

static const short yypact[] = {   647,
    67,-32768,-32768,    15,   125,    67,    67,    67,    67,    16,
    16,    16,   379,   330,   330,   330,   330,    33,   140,   153,
   385,   385,   125,   330,   125,   125,   125,   316,   379,   379,
   330,   385,    67,    67,    67,    48,   120,   358,   -46,   -46,
   125,   -46,   125,    48,    48,   330,    48,    48,    16,   125,
    16,   385,   385,   217,   379,   718,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    32,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,    67,-32768,   -16,    10,
-32768,-32768,-32768,-32768,-32768,   125,-32768,-32768,   119,   119,
   125,   106,   125,-32768,-32768,   330,   330,   330,-32768,    67,
-32768,   330,   212,-32768,-32768,-32768,   330,   330,   125,    92,
    68,-32768,   -16,   125,-32768,   330,-32768,   330,-32768,   330,
-32768,   330,-32768,   330,-32768,-32768,-32768,   323,-32768,   125,
   100,    35,   248,-32768,   330,   125,   125,    10,   125,-32768,
-32768,   125,-32768,-32768,   330,   125,   125,-32768,   125,-32768,
    16,-32768,    16,    45,    65,   125,   125,-32768,   125,-32768,
   330,-32768,   330,   212,   212,-32768,    94,-32768,   125,-32768,
-32768,     0,-32768,    67,    67,    67,    67,-32768,-32768,    46,
    46,-32768,-32768,   125,   330,   125,   125,    52,-32768,    52,
   330,-32768,   212,    33,-32768,   330,   125,-32768,   298,   125,
   330,-32768,   270,   125,-32768,    87,   125,-32768,-32768,-32768,
-32768,-32768,   330,-32768,-32768,-32768,   125,   125,   125,   125,
-32768,-32768,-32768,   125,   125,   -59,-32768,   -54,-32768,-32768,
-32768,    13,    13,-32768,-32768,-32768,-32768,-32768,   125,-32768,
-32768,-32768,   300,-32768,-32768,   300,   125,-32768,-32768,   125,
-32768,   270,-32768,-32768,   125,   270,-32768,   179,-32768,-32768,
-32768,    99,   101,   125,-32768,   125,-32768,-32768,-32768,-32768,
-32768,-32768,   104,    95,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   179,-32768,    93,   266,-32768,    87,    87,-32768,
-32768,   114,   121,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   191,   200,-32768
};

static const short yypgoto[] = {-32768,
-32768,   209,   152,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    76,    -9,-32768,    -8,-32768,
    25,    11,   117,   -53,   462,   105,    30,   122,-32768,     8,
  -173,-32768,    37,  -220,-32768,    -1,-32768,  -202,-32768,-32768
};


#define	YYLAST		806


static const short yytable[] = {    82,
   230,    97,    98,   263,    82,    82,    82,    82,    80,   110,
    83,   101,   273,    89,    90,    83,    94,   274,    84,    93,
   275,   276,   279,    84,    85,   279,   101,   101,   101,    85,
    79,    82,    82,    82,   140,    79,    79,    79,    79,   157,
   171,   159,   138,   136,   137,    74,    75,   113,    70,    71,
    72,    73,   167,   101,    70,    71,    72,    73,    74,    75,
    74,    75,    79,    79,    79,   139,   199,   173,   200,    70,
    71,    72,    73,   152,   152,    82,   152,   152,   283,    74,
    75,   207,   285,   194,   172,   299,   300,    82,    82,   174,
   175,   176,   177,   295,   231,    95,   180,   181,    82,   174,
   175,   176,   177,    86,   176,   177,    79,   224,   193,   225,
   187,   203,   298,   204,   146,   147,    76,   149,    79,    79,
    83,    70,    71,    72,    73,    83,   236,   219,    84,    79,
   242,    74,    75,    84,    85,   174,   175,   176,   177,    85,
    83,   217,   250,   218,   130,   131,   132,   220,    84,   153,
    76,   155,   156,    83,    85,    70,    71,    72,    73,   266,
    77,    84,   226,   227,   296,    74,    75,    85,    54,   293,
   228,   169,    82,    82,    82,    82,   288,   286,   289,    83,
   292,   232,   233,   234,   235,   110,   245,    84,   245,   301,
   304,   248,   302,    85,   257,   174,   175,   176,   177,   305,
   140,   260,    76,    79,    79,    79,    79,   170,   174,   175,
   176,   177,    77,    88,    70,    71,    72,    73,   249,    70,
    71,    72,    73,    79,   189,   114,   246,   115,   117,   119,
   121,   123,    79,   125,   126,   127,   129,     0,   116,     0,
   134,   245,     0,   257,   245,   141,   144,   257,    83,   148,
   260,   150,     0,     0,   260,     0,    84,     0,   158,     0,
   160,   162,    85,   286,     0,     0,   297,     0,    70,    71,
    72,    73,    70,    71,    72,    73,     0,   257,    74,    75,
     0,    79,    74,    75,   260,    79,     0,   164,   178,   165,
   166,     0,     0,     0,   179,     0,     0,     0,    83,   182,
    83,   183,    70,    71,    72,    73,    84,     0,    84,   252,
     0,     0,    85,     0,    85,    79,   128,   192,    70,    71,
    72,    73,   195,  -103,    84,     0,     0,     0,     0,   -39,
    85,  -103,    70,    71,    72,    73,   256,  -103,   202,     0,
   256,   205,    74,    75,   208,   209,   210,   211,     0,    76,
   212,     0,     0,    76,   214,   215,     0,   216,    83,     0,
   -94,   -94,   -94,   -94,   221,   222,    84,   223,   143,     0,
   -94,   -94,    85,     0,     0,     0,     0,   229,   242,    99,
     0,    70,    71,    72,    73,    83,     0,    70,    71,    72,
    73,     0,   238,    84,   240,   241,     0,    74,    75,    85,
     0,     0,     0,     0,     0,   251,     0,   253,   254,     0,
     0,     0,   261,     0,     0,   265,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   267,   268,   269,   270,     0,
     0,     0,   271,   272,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   277,     0,     0,
     0,   278,     0,     0,   280,   281,     0,     0,   282,     0,
     0,     0,     0,   284,     0,     0,   287,     0,     0,     0,
     0,     0,   290,     0,   291,   102,   107,   108,   109,     0,
     0,   118,   120,   122,     0,   124,     0,     0,     0,     0,
     0,   294,   133,   135,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   154,     0,     0,
     0,     0,     0,   161,   163,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   184,   185,   186,
     0,     0,     0,   188,     0,     0,     0,     0,   190,   191,
     0,     0,     0,     0,     0,     0,     0,   196,     0,     0,
     0,     0,     0,     0,     0,   201,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   213,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   237,     0,     0,     0,   239,     0,     0,     0,
     0,     0,   247,     1,     0,     2,     3,     0,     0,     0,
     0,     0,   255,     0,     4,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,    15,    16,    17,    18,     0,
     0,    19,    20,    21,    22,    23,     0,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,     0,     0,     0,
     0,     0,     0,     0,     1,     0,     0,     3,    54,     0,
     0,     0,     0,     0,    55,     4,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
     0,     0,    19,    20,    21,    22,    23,     0,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    55
};

static const short yycheck[] = {     1,
     1,    11,    12,   206,     6,     7,     8,     9,     1,    18,
     1,    13,    72,     6,     7,     1,     1,    77,     9,     9,
    75,    76,   243,     9,    15,   246,    28,    29,    30,    15,
     1,    33,    34,    35,    81,     6,     7,     8,     9,    49,
     9,    51,    35,    33,    34,    13,    14,    18,     3,     4,
     5,     6,    54,    55,     3,     4,     5,     6,    13,    14,
    13,    14,    33,    34,    35,    36,   120,    84,   122,     3,
     4,     5,     6,    44,    45,    77,    47,    48,   252,    13,
    14,   135,   256,    16,    77,   288,   289,    89,    90,    90,
    91,    92,    93,     1,    95,    80,    89,    90,   100,    90,
    91,    92,    93,    89,    92,    93,    77,   161,    17,   163,
   100,    12,   286,    79,    39,    40,    84,    42,    89,    90,
     1,     3,     4,     5,     6,     1,   180,    83,     9,   100,
    79,    13,    14,     9,    15,    90,    91,    92,    93,    15,
     1,   151,   196,   153,    28,    29,    30,    83,     9,    45,
    84,    47,    48,     1,    15,     3,     4,     5,     6,   213,
    94,     9,   164,   165,    72,    13,    14,    15,    82,    75,
    77,    55,   174,   175,   176,   177,    78,    85,    78,     1,
    77,   174,   175,   176,   177,   194,   188,     9,   190,    76,
     0,   193,    72,    15,   203,    90,    91,    92,    93,     0,
    81,   203,    84,   174,   175,   176,   177,    56,    90,    91,
    92,    93,    94,     5,     3,     4,     5,     6,   194,     3,
     4,     5,     6,   194,   103,    86,   190,    19,    20,    21,
    22,    23,   203,    25,    26,    27,    28,    -1,    86,    -1,
    32,   243,    -1,   252,   246,    37,    38,   256,     1,    41,
   252,    43,    -1,    -1,   256,    -1,     9,    -1,    50,    -1,
    52,    53,    15,    85,    -1,    -1,     1,    -1,     3,     4,
     5,     6,     3,     4,     5,     6,    -1,   286,    13,    14,
    -1,   252,    13,    14,   286,   256,    -1,    71,    80,    73,
    74,    -1,    -1,    -1,    86,    -1,    -1,    -1,     1,    91,
     1,    93,     3,     4,     5,     6,     9,    -1,     9,    12,
    -1,    -1,    15,    -1,    15,   286,     1,   109,     3,     4,
     5,     6,   114,     1,     9,    -1,    -1,    -1,    -1,    82,
    15,     9,     3,     4,     5,     6,    71,    15,   130,    -1,
    71,   133,    13,    14,   136,   137,   138,   139,    -1,    84,
   142,    -1,    -1,    84,   146,   147,    -1,   149,     1,    -1,
     3,     4,     5,     6,   156,   157,     9,   159,    11,    -1,
    13,    14,    15,    -1,    -1,    -1,    -1,   169,    79,     1,
    -1,     3,     4,     5,     6,     1,    -1,     3,     4,     5,
     6,    -1,   184,     9,   186,   187,    -1,    13,    14,    15,
    -1,    -1,    -1,    -1,    -1,   197,    -1,   199,   200,    -1,
    -1,    -1,   204,    -1,    -1,   207,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   217,   218,   219,   220,    -1,
    -1,    -1,   224,   225,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   239,    -1,    -1,
    -1,   243,    -1,    -1,   246,   247,    -1,    -1,   250,    -1,
    -1,    -1,    -1,   255,    -1,    -1,   258,    -1,    -1,    -1,
    -1,    -1,   264,    -1,   266,    14,    15,    16,    17,    -1,
    -1,    20,    21,    22,    -1,    24,    -1,    -1,    -1,    -1,
    -1,   283,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,
    -1,    -1,    -1,    52,    53,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    96,    97,    98,
    -1,    -1,    -1,   102,    -1,    -1,    -1,    -1,   107,   108,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   116,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   124,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   145,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   181,    -1,    -1,    -1,   185,    -1,    -1,    -1,
    -1,    -1,   191,     7,    -1,     9,    10,    -1,    -1,    -1,
    -1,    -1,   201,    -1,    18,    19,    20,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
    -1,    35,    36,    37,    38,    39,    -1,    41,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
    64,    65,    66,    67,    68,    69,    70,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,     7,    -1,    -1,    10,    82,    -1,
    -1,    -1,    -1,    -1,    88,    18,    19,    20,    21,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    -1,    -1,    35,    36,    37,    38,    39,    -1,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
    63,    64,    65,    66,    67,    68,    69,    70,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    88
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
#line 90 "mon_parse.y"
{ new_cmd = 1; asm_mode = 0; ;
    break;}
case 8:
#line 99 "mon_parse.y"
{ return ERR_EXPECT_END_CMD; ;
    break;}
case 19:
#line 112 "mon_parse.y"
{ return ERR_BAD_CMD; ;
    break;}
case 20:
#line 115 "mon_parse.y"
{ fprintf(mon_output, "Bank command not done yet.\n"); ;
    break;}
case 21:
#line 116 "mon_parse.y"
{ mon_jump(yyvsp[-1].a); ;
    break;}
case 22:
#line 117 "mon_parse.y"
{ fprintf(mon_output, "Display IO registers\n"); ;
    break;}
case 23:
#line 118 "mon_parse.y"
{ fprintf(mon_output, "Continue until RTS/RTI\n"); ;
    break;}
case 24:
#line 119 "mon_parse.y"
{ puts("Dump machine state."); ;
    break;}
case 25:
#line 120 "mon_parse.y"
{ puts("Undump machine state."); ;
    break;}
case 26:
#line 121 "mon_parse.y"
{ mon_instructions_step(yyvsp[-1].i); ;
    break;}
case 27:
#line 122 "mon_parse.y"
{ mon_instructions_next(yyvsp[-1].i); ;
    break;}
case 28:
#line 123 "mon_parse.y"
{ mon_stack_up(yyvsp[-1].i); ;
    break;}
case 29:
#line 124 "mon_parse.y"
{ mon_stack_down(yyvsp[-1].i); ;
    break;}
case 31:
#line 128 "mon_parse.y"
{ mon_print_registers(default_memspace); ;
    break;}
case 32:
#line 129 "mon_parse.y"
{ mon_print_registers(yyvsp[0].i); ;
    break;}
case 34:
#line 133 "mon_parse.y"
{ mon_load_symbols(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 35:
#line 134 "mon_parse.y"
{ mon_save_symbols(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 36:
#line 135 "mon_parse.y"
{ mon_add_name_to_symbol_table(yyvsp[-2].a, yyvsp[-1].str); ;
    break;}
case 37:
#line 136 "mon_parse.y"
{ mon_remove_name_from_symbol_table(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 38:
#line 137 "mon_parse.y"
{ mon_print_symbol_table(yyvsp[-1].i); ;
    break;}
case 39:
#line 140 "mon_parse.y"
{ mon_start_assemble_mode(yyvsp[0].a, NULL); ;
    break;}
case 41:
#line 141 "mon_parse.y"
{ mon_start_assemble_mode(yyvsp[-1].a, NULL); ;
    break;}
case 42:
#line 142 "mon_parse.y"
{ mon_disassemble_lines(yyvsp[-2].a,yyvsp[-1].a); ;
    break;}
case 43:
#line 143 "mon_parse.y"
{ mon_disassemble_lines(BAD_ADDR, BAD_ADDR); ;
    break;}
case 44:
#line 146 "mon_parse.y"
{ mon_move_memory(yyvsp[-3].a, yyvsp[-2].a, yyvsp[-1].a); ;
    break;}
case 45:
#line 147 "mon_parse.y"
{ mon_compare_memory(yyvsp[-3].a, yyvsp[-2].a, yyvsp[-1].a); ;
    break;}
case 46:
#line 148 "mon_parse.y"
{ mon_fill_memory(yyvsp[-3].a, yyvsp[-2].a, yyvsp[-1].str); ;
    break;}
case 47:
#line 149 "mon_parse.y"
{ mon_hunt_memory(yyvsp[-3].a, yyvsp[-2].a, yyvsp[-1].str); ;
    break;}
case 48:
#line 150 "mon_parse.y"
{ mon_display_memory(yyvsp[-3].rt, yyvsp[-2].a, yyvsp[-1].a); ;
    break;}
case 49:
#line 151 "mon_parse.y"
{ mon_display_memory(0, yyvsp[-2].a, yyvsp[-1].a); ;
    break;}
case 50:
#line 152 "mon_parse.y"
{ mon_display_memory(0, BAD_ADDR, BAD_ADDR); ;
    break;}
case 51:
#line 153 "mon_parse.y"
{ mon_display_data(0, yyvsp[-2].a, yyvsp[-1].a, 8, 8); ;
    break;}
case 52:
#line 154 "mon_parse.y"
{ mon_display_data(0, BAD_ADDR, BAD_ADDR, 8, 8); ;
    break;}
case 53:
#line 155 "mon_parse.y"
{ mon_display_data(0, yyvsp[-2].a, yyvsp[-1].a, 24, 21); ;
    break;}
case 54:
#line 156 "mon_parse.y"
{ mon_display_data(0, BAD_ADDR, BAD_ADDR, 24, 21); ;
    break;}
case 55:
#line 159 "mon_parse.y"
{ mon_add_checkpoint(yyvsp[-2].a, yyvsp[-1].a, FALSE, FALSE, FALSE); ;
    break;}
case 56:
#line 160 "mon_parse.y"
{ 
                          temp = mon_add_checkpoint(yyvsp[-4].a, yyvsp[-3].a, FALSE, FALSE, FALSE); 
                          mon_set_checkpoint_condition(temp, yyvsp[-1].cond_node); ;
    break;}
case 57:
#line 163 "mon_parse.y"
{ mon_add_checkpoint(yyvsp[-2].a, yyvsp[-1].a, FALSE, 
                              (yyvsp[-3].i == e_load || yyvsp[-3].i == e_load_store), (yyvsp[-3].i == e_store || yyvsp[-3].i == e_load_store)); ;
    break;}
case 58:
#line 165 "mon_parse.y"
{ mon_add_checkpoint(yyvsp[-2].a, yyvsp[-1].a, TRUE, FALSE, FALSE); ;
    break;}
case 59:
#line 166 "mon_parse.y"
{ mon_print_checkpoints(); ;
    break;}
case 60:
#line 167 "mon_parse.y"
{ mon_print_checkpoints(); ;
    break;}
case 61:
#line 168 "mon_parse.y"
{ mon_print_checkpoints(); ;
    break;}
case 62:
#line 172 "mon_parse.y"
{ mon_switch_checkpoint(yyvsp[-2].action, yyvsp[-1].i); ;
    break;}
case 63:
#line 173 "mon_parse.y"
{ mon_set_ignore_count(yyvsp[-2].i, yyvsp[-1].i); ;
    break;}
case 64:
#line 174 "mon_parse.y"
{ mon_delete_checkpoint(yyvsp[-1].i); ;
    break;}
case 65:
#line 175 "mon_parse.y"
{ mon_delete_checkpoint(-1); ;
    break;}
case 66:
#line 176 "mon_parse.y"
{ mon_set_checkpoint_condition(yyvsp[-3].i, yyvsp[-1].cond_node); ;
    break;}
case 67:
#line 177 "mon_parse.y"
{ mon_set_checkpoint_command(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 68:
#line 180 "mon_parse.y"
{ sidefx = ((yyvsp[-1].action==e_TOGGLE)?(sidefx^1):yyvsp[-1].action); ;
    break;}
case 69:
#line 181 "mon_parse.y"
{ fprintf(mon_output, "sidefx %d\n",sidefx); ;
    break;}
case 70:
#line 182 "mon_parse.y"
{ default_radix = yyvsp[-1].rt; ;
    break;}
case 71:
#line 183 "mon_parse.y"
{ fprintf(mon_output, "Default radix is %d\n", 
                                                         default_radix); ;
    break;}
case 72:
#line 185 "mon_parse.y"
{ fprintf(mon_output,"Setting default device to %s\n", 
                                                         SPACESTRING(yyvsp[-1].i)); default_memspace = yyvsp[-1].i; ;
    break;}
case 73:
#line 187 "mon_parse.y"
{ fprintf(mon_output, "Quit.\n"); exit(-1); exit(0); ;
    break;}
case 74:
#line 188 "mon_parse.y"
{ exit_mon = 1; YYACCEPT; ;
    break;}
case 75:
#line 191 "mon_parse.y"
{ mon_execute_disk_command(yyvsp[-1].str); ;
    break;}
case 76:
#line 192 "mon_parse.y"
{ fprintf(mon_output, "\t%d\n",yyvsp[-1].i); ;
    break;}
case 77:
#line 193 "mon_parse.y"
{ mon_print_help(NULL); ;
    break;}
case 78:
#line 194 "mon_parse.y"
{ mon_print_help(yyvsp[-1].str); ;
    break;}
case 79:
#line 195 "mon_parse.y"
{ printf("SYSTEM COMMAND: %s\n",yyvsp[-1].str); ;
    break;}
case 80:
#line 196 "mon_parse.y"
{ mon_print_convert(yyvsp[-1].i); ;
    break;}
case 81:
#line 197 "mon_parse.y"
{ mon_change_dir(yyvsp[-1].str); ;
    break;}
case 82:
#line 200 "mon_parse.y"
{ mon_load_file(yyvsp[-2].str,yyvsp[-1].a); ;
    break;}
case 83:
#line 201 "mon_parse.y"
{ mon_save_file(yyvsp[-3].str,yyvsp[-2].a,yyvsp[-1].a); ;
    break;}
case 84:
#line 202 "mon_parse.y"
{ mon_verify_file(yyvsp[-2].str,yyvsp[-1].a); ;
    break;}
case 85:
#line 203 "mon_parse.y"
{ mon_block_cmd(0,yyvsp[-2].i,yyvsp[-1].i,yyvsp[0].a); ;
    break;}
case 86:
#line 204 "mon_parse.y"
{ mon_block_cmd(1,yyvsp[-2].i,yyvsp[-1].i,yyvsp[0].a); ;
    break;}
case 87:
#line 207 "mon_parse.y"
{ mon_record_commands(yyvsp[-1].str); ;
    break;}
case 88:
#line 208 "mon_parse.y"
{ mon_end_recording(); ;
    break;}
case 89:
#line 209 "mon_parse.y"
{ playback=TRUE; playback_name = yyvsp[-1].str; ;
    break;}
case 90:
#line 212 "mon_parse.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 92:
#line 216 "mon_parse.y"
{ return ERR_EXPECT_FILENAME; ;
    break;}
case 93:
#line 219 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 94:
#line 220 "mon_parse.y"
{ yyval.i = e_load_store; ;
    break;}
case 95:
#line 223 "mon_parse.y"
{ yyval.i = new_reg(default_memspace, yyvsp[0].reg); ;
    break;}
case 96:
#line 224 "mon_parse.y"
{ yyval.i = new_reg(yyvsp[-1].i, yyvsp[0].reg); ;
    break;}
case 99:
#line 231 "mon_parse.y"
{ mon_set_reg_val(reg_memspace(yyvsp[-2].i), reg_regid(yyvsp[-2].i), yyvsp[0].i); ;
    break;}
case 100:
#line 234 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 101:
#line 235 "mon_parse.y"
{ yyval.i = -1; ;
    break;}
case 102:
#line 238 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 103:
#line 239 "mon_parse.y"
{ return ERR_EXPECT_BRKNUM; ;
    break;}
case 104:
#line 242 "mon_parse.y"
{ yyval.a = yyvsp[0].a; ;
    break;}
case 105:
#line 243 "mon_parse.y"
{ yyval.a = BAD_ADDR; ;
    break;}
case 106:
#line 246 "mon_parse.y"
{ yyval.a = new_addr(e_default_space,yyvsp[0].i); if (opt_asm) new_cmd = asm_mode = 1; ;
    break;}
case 107:
#line 247 "mon_parse.y"
{ yyval.a = new_addr(yyvsp[-1].i,yyvsp[0].i); if (opt_asm) new_cmd = asm_mode = 1; ;
    break;}
case 108:
#line 250 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 109:
#line 251 "mon_parse.y"
{ yyval.i = e_default_space; ;
    break;}
case 110:
#line 254 "mon_parse.y"
{ yyval.i = e_comp_space; ;
    break;}
case 111:
#line 255 "mon_parse.y"
{ yyval.i = e_disk_space; ;
    break;}
case 112:
#line 258 "mon_parse.y"
{ yyval.i = check_addr_limits(yyvsp[0].i); ;
    break;}
case 113:
#line 261 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 114:
#line 263 "mon_parse.y"
{ yyval.i = yyvsp[-2].i + yyvsp[0].i; ;
    break;}
case 115:
#line 264 "mon_parse.y"
{ yyval.i = yyvsp[-2].i - yyvsp[0].i; ;
    break;}
case 116:
#line 265 "mon_parse.y"
{ yyval.i = yyvsp[-2].i * yyvsp[0].i; ;
    break;}
case 117:
#line 266 "mon_parse.y"
{ yyval.i = (yyvsp[0].i) ? (yyvsp[-2].i / yyvsp[0].i) : 1; ;
    break;}
case 118:
#line 267 "mon_parse.y"
{ yyval.i = yyvsp[-1].i; ;
    break;}
case 119:
#line 268 "mon_parse.y"
{ return ERR_MISSING_CLOSE_PAREN; ;
    break;}
case 120:
#line 269 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 121:
#line 272 "mon_parse.y"
{ yyval.cond_node = new_cond; yyval.cond_node->is_parenthized = FALSE;
                                            yyval.cond_node->child1 = yyvsp[-2].cond_node; yyval.cond_node->child2 = yyvsp[0].cond_node; yyval.cond_node->operation = yyvsp[-1].cond_op; ;
    break;}
case 122:
#line 274 "mon_parse.y"
{ return ERR_INCOMPLETE_COMPARE_OP; ;
    break;}
case 123:
#line 275 "mon_parse.y"
{ yyval.cond_node = yyvsp[-1].cond_node; yyval.cond_node->is_parenthized = TRUE; ;
    break;}
case 124:
#line 276 "mon_parse.y"
{ return ERR_MISSING_CLOSE_PAREN; ;
    break;}
case 125:
#line 277 "mon_parse.y"
{ yyval.cond_node = yyvsp[0].cond_node; ;
    break;}
case 126:
#line 280 "mon_parse.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV; yyval.cond_node->is_parenthized = FALSE;
                            yyval.cond_node->reg_num = yyvsp[0].i; yyval.cond_node->is_reg = TRUE; ;
    break;}
case 127:
#line 282 "mon_parse.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV; yyval.cond_node->is_parenthized = FALSE;
                            yyval.cond_node->value = yyvsp[0].i; yyval.cond_node->is_reg = FALSE; ;
    break;}
case 130:
#line 290 "mon_parse.y"
{ mon_add_number_to_buffer(yyvsp[0].i); ;
    break;}
case 131:
#line 291 "mon_parse.y"
{ mon_add_string_to_buffer(yyvsp[0].str); ;
    break;}
case 132:
#line 294 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 133:
#line 295 "mon_parse.y"
{ yyval.i = mon_get_reg_val(reg_memspace(yyvsp[0].i), reg_regid(yyvsp[0].i)); ;
    break;}
case 134:
#line 298 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 135:
#line 299 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 136:
#line 300 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 137:
#line 301 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 140:
#line 308 "mon_parse.y"
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
case 142:
#line 319 "mon_parse.y"
{ asm_mode = 0; ;
    break;}
case 143:
#line 322 "mon_parse.y"
{ yyval.i = join_ints(IMMEDIATE,yyvsp[0].i); ;
    break;}
case 144:
#line 323 "mon_parse.y"
{ if (yyvsp[0].i < 0x100)
                              yyval.i = join_ints(ZERO_PAGE,yyvsp[0].i);
                           else
                              yyval.i = join_ints(ABSOLUTE,yyvsp[0].i);
                         ;
    break;}
case 145:
#line 328 "mon_parse.y"
{ if (yyvsp[-2].i < 0x100)
                                           yyval.i = join_ints(ZERO_PAGE_X,yyvsp[-2].i);
                                        else
                                           yyval.i = join_ints(ABSOLUTE_X,yyvsp[-2].i);
                                      ;
    break;}
case 146:
#line 333 "mon_parse.y"
{ if (yyvsp[-2].i < 0x100)
                                           yyval.i = join_ints(ZERO_PAGE_Y,yyvsp[-2].i);
                                        else
                                           yyval.i = join_ints(ABSOLUTE_Y,yyvsp[-2].i);
                                      ;
    break;}
case 147:
#line 338 "mon_parse.y"
{ yyval.i = join_ints(ABS_INDIRECT,yyvsp[-1].i); ;
    break;}
case 148:
#line 339 "mon_parse.y"
{ yyval.i = join_ints(INDIRECT_X,yyvsp[-3].i); ;
    break;}
case 149:
#line 340 "mon_parse.y"
{ yyval.i = join_ints(INDIRECT_Y,yyvsp[-3].i); ;
    break;}
case 150:
#line 341 "mon_parse.y"
{ yyval.i = join_ints(IMPLIED,0); ;
    break;}
case 151:
#line 342 "mon_parse.y"
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
#line 346 "mon_parse.y"
 

static unsigned check_addr_limits(ADDRESS val)
{
   if (val != LO16(val))
   {
      fprintf(mon_output, "Overflow warning: $%x -> $ffff\n", val);
      return 0xffff;
   }

   return val;
}

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

static int yyerror(char *s)
{
   YYABORT;
   fprintf(stderr, "ERR:%s\n",s);
   return 0;
}

