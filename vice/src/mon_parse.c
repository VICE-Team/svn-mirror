
/*  A Bison parser, made from ../../src/mon_parse.y
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
#define	MEM_DISK8	269
#define	MEM_DISK9	270
#define	CMD_SEP	271
#define	REG_ASGN_SEP	272
#define	EQUALS	273
#define	CMD_SIDEFX	274
#define	CMD_RETURN	275
#define	CMD_BLOCK_READ	276
#define	CMD_BLOCK_WRITE	277
#define	CMD_UP	278
#define	CMD_DOWN	279
#define	CMD_LOAD	280
#define	CMD_SAVE	281
#define	CMD_VERIFY	282
#define	CMD_IGNORE	283
#define	CMD_HUNT	284
#define	CMD_FILL	285
#define	CMD_MOVE	286
#define	CMD_GOTO	287
#define	CMD_REGISTERS	288
#define	CMD_READSPACE	289
#define	CMD_WRITESPACE	290
#define	CMD_RADIX	291
#define	CMD_MEM_DISPLAY	292
#define	CMD_BREAK	293
#define	CMD_TRACE	294
#define	CMD_IO	295
#define	CMD_BRMON	296
#define	CMD_COMPARE	297
#define	CMD_DUMP	298
#define	CMD_UNDUMP	299
#define	CMD_EXIT	300
#define	CMD_DELETE	301
#define	CMD_CONDITION	302
#define	CMD_COMMAND	303
#define	CMD_ASSEMBLE	304
#define	CMD_DISASSEMBLE	305
#define	CMD_NEXT	306
#define	CMD_STEP	307
#define	CMD_PRINT	308
#define	CMD_DEVICE	309
#define	CMD_HELP	310
#define	CMD_WATCH	311
#define	CMD_DISK	312
#define	CMD_SYSTEM	313
#define	CMD_QUIT	314
#define	CMD_CHDIR	315
#define	CMD_BANK	316
#define	CMD_LOAD_LABELS	317
#define	CMD_SAVE_LABELS	318
#define	CMD_ADD_LABEL	319
#define	CMD_DEL_LABEL	320
#define	CMD_SHOW_LABELS	321
#define	CMD_RECORD	322
#define	CMD_STOP	323
#define	CMD_PLAYBACK	324
#define	CMD_CHAR_DISPLAY	325
#define	CMD_SPRITE_DISPLAY	326
#define	CMD_TEXT_DISPLAY	327
#define	CMD_ENTER_DATA	328
#define	CMD_ENTER_BIN_DATA	329
#define	CMD_BLOAD	330
#define	CMD_BSAVE	331
#define	L_PAREN	332
#define	R_PAREN	333
#define	ARG_IMMEDIATE	334
#define	REG_A	335
#define	REG_X	336
#define	REG_Y	337
#define	COMMA	338
#define	INST_SEP	339
#define	STRING	340
#define	FILENAME	341
#define	R_O_L	342
#define	OPCODE	343
#define	LABEL	344
#define	BANKNAME	345
#define	REGISTER	346
#define	COMPARE_OP	347
#define	RADIX_TYPE	348
#define	INPUT_SPEC	349
#define	CMD_CHECKPT_ONOFF	350
#define	TOGGLE	351

#line 28 "../../src/mon_parse.y"

#include "vice.h"
#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#endif
#include "types.h"
#include "asm.h"
#include "utils.h"
#include "mon.h"

#define join_ints(x,y) (LO16_TO_HI16(x)|y)
#define separate_int1(x) (HI16_TO_LO16(x))
#define separate_int2(x) (LO16(x))

static int yyerror(char *s);
static int temp;

/* Defined in the lexer */
extern int new_cmd, opt_asm;
extern void free_buffer(void);
extern void make_buffer(char *str);
extern int yylex(void);
extern int cur_len, last_len;

#define ERR_ILLEGAL_INPUT 1     /* Generic error as returned by yacc.  */
#define ERR_RANGE_BAD_START 2
#define ERR_RANGE_BAD_END 3
#define ERR_BAD_CMD 4
#define ERR_EXPECT_BRKNUM 5
#define ERR_EXPECT_END_CMD 6
#define ERR_MISSING_CLOSE_PAREN 7
#define ERR_INCOMPLETE_COMPARE_OP 8
#define ERR_EXPECT_FILENAME 9
#define ERR_ADDR_TOO_BIG 10
#define ERR_IMM_TOO_BIG 11

#define BAD_ADDR (new_addr(e_invalid_space, 0))
#define CHECK_ADDR(x) ((x) == LO16(x))


#line 71 "../../src/mon_parse.y"
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



#define	YYFINAL		336
#define	YYFLAG		-32768
#define	YYNTBASE	103

#define YYTRANSLATE(x) ((unsigned)(x) <= 351 ? yytranslate[x] : 146)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,   101,
   102,    99,    97,     2,    98,     2,   100,     2,     2,     2,
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
    86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
    96
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     7,     9,    12,    14,    16,    18,    20,
    22,    24,    26,    28,    30,    32,    34,    36,    38,    40,
    42,    47,    51,    54,    57,    60,    63,    67,    71,    75,
    79,    81,    84,    88,    92,    97,   102,   107,   112,   116,
   117,   123,   127,   132,   135,   141,   147,   153,   159,   165,
   170,   173,   178,   181,   186,   189,   194,   197,   202,   209,
   215,   220,   223,   226,   229,   233,   238,   242,   245,   251,
   256,   260,   263,   267,   270,   274,   277,   280,   284,   288,
   291,   295,   299,   303,   307,   312,   317,   323,   329,   334,
   340,   346,   350,   353,   357,   362,   365,   367,   369,   370,
   372,   374,   376,   377,   379,   382,   386,   388,   392,   394,
   395,   397,   399,   401,   402,   404,   407,   409,   411,   412,
   414,   416,   418,   420,   422,   426,   430,   434,   438,   442,
   446,   448,   452,   456,   460,   464,   466,   468,   470,   473,
   475,   477,   479,   481,   483,   485,   487,   489,   491,   495,
   499,   502,   504,   506,   509,   511,   515,   519,   523,   529,
   535,   536
};

static const short yyrhs[] = {   104,
     0,   143,     9,     0,     9,     0,   106,     0,   104,   106,
     0,    16,     0,     9,     0,     1,     0,   107,     0,   109,
     0,   112,     0,   110,     0,   113,     0,   114,     0,   115,
     0,   116,     0,   117,     0,   118,     0,   119,     0,    10,
     0,    61,   131,   121,   105,     0,    32,   130,   105,     0,
    40,   105,     0,    20,   105,     0,    43,   105,     0,    44,
   105,     0,    52,   127,   105,     0,    51,   127,   105,     0,
    23,   127,   105,     0,    24,   127,   105,     0,   108,     0,
    33,   105,     0,    33,   132,   105,     0,    33,   125,   105,
     0,    62,   131,   122,   105,     0,    63,   131,   122,   105,
     0,    64,   130,    89,   105,     0,    65,   131,    89,   105,
     0,    66,   131,   105,     0,     0,    49,   130,   111,   144,
   105,     0,    49,   130,   105,     0,    50,   130,   129,   105,
     0,    50,   105,     0,    31,   130,   130,   130,   105,     0,
    42,   130,   130,   130,   105,     0,    30,   130,   130,   138,
   105,     0,    29,   130,   130,   138,   105,     0,    37,    93,
   130,   129,   105,     0,    37,   130,   129,   105,     0,    37,
   105,     0,    70,   130,   129,   105,     0,    70,   105,     0,
    71,   130,   129,   105,     0,    71,   105,     0,    72,   130,
   129,   105,     0,    72,   105,     0,    38,   130,   129,   105,
     0,    38,   130,   129,    12,   136,   105,     0,    56,   123,
   130,   129,   105,     0,    39,   130,   129,   105,     0,    38,
   105,     0,    39,   105,     0,    56,   105,     0,    95,   128,
   105,     0,    28,   128,   127,   105,     0,    46,   128,   105,
     0,    46,   105,     0,    47,   128,    12,   136,   105,     0,
    48,   128,    85,   105,     0,    19,    96,   105,     0,    19,
   105,     0,    36,    93,   105,     0,    36,   105,     0,    54,
   132,   105,     0,    59,   105,     0,    45,   105,     0,    57,
   120,   105,     0,    53,   135,   105,     0,    55,   105,     0,
    55,   120,   105,     0,    58,   120,   105,     0,     7,   135,
   105,     0,    60,   120,   105,     0,    25,   122,   129,   105,
     0,    75,   122,   129,   105,     0,    26,   122,   130,   130,
   105,     0,    76,   122,   130,   130,   105,     0,    27,   122,
   130,   105,     0,    21,   135,   135,   129,   105,     0,    22,
   135,   135,   130,   105,     0,    67,   122,   105,     0,    68,
   105,     0,    69,   122,   105,     0,    73,   130,   138,   105,
     0,    74,   105,     0,    87,     0,    90,     0,     0,    86,
     0,     1,     0,    11,     0,     0,    91,     0,   132,    91,
     0,   125,    17,   126,     0,   126,     0,   124,    18,   141,
     0,   135,     0,     0,     4,     0,     1,     0,   130,     0,
     0,   133,     0,   132,   133,     0,    89,     0,   132,     0,
     0,    13,     0,    14,     0,    15,     0,   134,     0,   141,
     0,   135,    97,   135,     0,   135,    98,   135,     0,   135,
    99,   135,     0,   135,   100,   135,     0,   101,   135,   102,
     0,   101,   135,     1,     0,   140,     0,   136,    92,   136,
     0,   136,    92,     1,     0,    77,   136,    78,     0,    77,
   136,     1,     0,   137,     0,   124,     0,   141,     0,   138,
   139,     0,   139,     0,   141,     0,    85,     0,   141,     0,
   124,     0,     3,     0,     4,     0,     5,     0,     6,     0,
   142,    84,   143,     0,   143,    84,   143,     0,    88,   145,
     0,   143,     0,   142,     0,    79,   141,     0,   141,     0,
   141,    83,    81,     0,   141,    83,    82,     0,    77,   141,
    78,     0,    77,   141,    83,    81,    78,     0,    77,   141,
    78,    83,    82,     0,     0,    80,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   121,   122,   123,   126,   127,   130,   131,   132,   135,   136,
   137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
   149,   150,   151,   152,   153,   154,   155,   156,   157,   158,
   159,   162,   163,   164,   167,   168,   169,   170,   171,   174,
   174,   175,   176,   177,   180,   181,   182,   183,   184,   185,
   186,   187,   188,   189,   190,   191,   192,   195,   196,   199,
   201,   202,   203,   204,   208,   209,   210,   211,   212,   213,
   216,   217,   218,   219,   237,   239,   240,   243,   244,   245,
   246,   247,   248,   249,   252,   253,   254,   255,   256,   257,
   258,   261,   262,   263,   266,   267,   270,   273,   274,   277,
   278,   281,   282,   285,   286,   289,   290,   293,   296,   297,
   300,   301,   304,   305,   308,   309,   310,   317,   318,   321,
   322,   323,   326,   329,   331,   332,   333,   334,   335,   336,
   337,   340,   342,   343,   344,   345,   348,   350,   354,   355,
   358,   359,   362,   363,   366,   367,   368,   369,   372,   373,
   376,   386,   387,   390,   392,   397,   402,   407,   408,   409,
   410,   411
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","H_NUMBER",
"D_NUMBER","O_NUMBER","B_NUMBER","CONVERT_OP","B_DATA","TRAIL","BAD_CMD","MEM_OP",
"IF","MEM_COMP","MEM_DISK8","MEM_DISK9","CMD_SEP","REG_ASGN_SEP","EQUALS","CMD_SIDEFX",
"CMD_RETURN","CMD_BLOCK_READ","CMD_BLOCK_WRITE","CMD_UP","CMD_DOWN","CMD_LOAD",
"CMD_SAVE","CMD_VERIFY","CMD_IGNORE","CMD_HUNT","CMD_FILL","CMD_MOVE","CMD_GOTO",
"CMD_REGISTERS","CMD_READSPACE","CMD_WRITESPACE","CMD_RADIX","CMD_MEM_DISPLAY",
"CMD_BREAK","CMD_TRACE","CMD_IO","CMD_BRMON","CMD_COMPARE","CMD_DUMP","CMD_UNDUMP",
"CMD_EXIT","CMD_DELETE","CMD_CONDITION","CMD_COMMAND","CMD_ASSEMBLE","CMD_DISASSEMBLE",
"CMD_NEXT","CMD_STEP","CMD_PRINT","CMD_DEVICE","CMD_HELP","CMD_WATCH","CMD_DISK",
"CMD_SYSTEM","CMD_QUIT","CMD_CHDIR","CMD_BANK","CMD_LOAD_LABELS","CMD_SAVE_LABELS",
"CMD_ADD_LABEL","CMD_DEL_LABEL","CMD_SHOW_LABELS","CMD_RECORD","CMD_STOP","CMD_PLAYBACK",
"CMD_CHAR_DISPLAY","CMD_SPRITE_DISPLAY","CMD_TEXT_DISPLAY","CMD_ENTER_DATA",
"CMD_ENTER_BIN_DATA","CMD_BLOAD","CMD_BSAVE","L_PAREN","R_PAREN","ARG_IMMEDIATE",
"REG_A","REG_X","REG_Y","COMMA","INST_SEP","STRING","FILENAME","R_O_L","OPCODE",
"LABEL","BANKNAME","REGISTER","COMPARE_OP","RADIX_TYPE","INPUT_SPEC","CMD_CHECKPT_ONOFF",
"TOGGLE","'+'","'-'","'*'","'/'","'('","')'","top_level","command_list","end_cmd",
"command","machine_state_rules","register_mod","symbol_table_rules","asm_rules",
"@1","memory_rules","checkpoint_rules","checkpoint_control_rules","monitor_state_rules",
"monitor_misc_rules","disk_rules","cmd_file_rules","data_entry_rules","rest_of_line",
"opt_bankname","filename","opt_mem_op","register","reg_list","reg_asgn","opt_count",
"breakpt_num","opt_address","address","opt_memspace","memspace","memloc","memaddr",
"expression","cond_expr","compare_operand","data_list","data_element","value",
"number","assembly_instr_list","assembly_instruction","post_assemble","asm_operand_mode", NULL
};
#endif

static const short yyr1[] = {     0,
   103,   103,   103,   104,   104,   105,   105,   105,   106,   106,
   106,   106,   106,   106,   106,   106,   106,   106,   106,   106,
   107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
   107,   108,   108,   108,   109,   109,   109,   109,   109,   111,
   110,   110,   110,   110,   112,   112,   112,   112,   112,   112,
   112,   112,   112,   112,   112,   112,   112,   113,   113,   113,
   113,   113,   113,   113,   114,   114,   114,   114,   114,   114,
   115,   115,   115,   115,   115,   115,   115,   116,   116,   116,
   116,   116,   116,   116,   117,   117,   117,   117,   117,   117,
   117,   118,   118,   118,   119,   119,   120,   121,   121,   122,
   122,   123,   123,   124,   124,   125,   125,   126,   127,   127,
   128,   128,   129,   129,   130,   130,   130,   131,   131,   132,
   132,   132,   133,   134,   135,   135,   135,   135,   135,   135,
   135,   136,   136,   136,   136,   136,   137,   137,   138,   138,
   139,   139,   140,   140,   141,   141,   141,   141,   142,   142,
   143,   144,   144,   145,   145,   145,   145,   145,   145,   145,
   145,   145
};

static const short yyr2[] = {     0,
     1,     2,     1,     1,     2,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     4,     3,     2,     2,     2,     2,     3,     3,     3,     3,
     1,     2,     3,     3,     4,     4,     4,     4,     3,     0,
     5,     3,     4,     2,     5,     5,     5,     5,     5,     4,
     2,     4,     2,     4,     2,     4,     2,     4,     6,     5,
     4,     2,     2,     2,     3,     4,     3,     2,     5,     4,
     3,     2,     3,     2,     3,     2,     2,     3,     3,     2,
     3,     3,     3,     3,     4,     4,     5,     5,     4,     5,
     5,     3,     2,     3,     4,     2,     1,     1,     0,     1,
     1,     1,     0,     1,     2,     3,     1,     3,     1,     0,
     1,     1,     1,     0,     1,     2,     1,     1,     0,     1,
     1,     1,     1,     1,     3,     3,     3,     3,     3,     3,
     1,     3,     3,     3,     3,     1,     1,     1,     2,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
     2,     1,     1,     2,     1,     3,     3,     3,     5,     5,
     0,     1
};

static const short yydefact[] = {     0,
     0,     3,    20,     0,     0,     0,     0,   110,   110,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   110,   110,     0,     0,     0,     0,     0,     0,
     0,     0,   119,   119,   119,     0,   119,   119,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   161,     0,
     1,     4,     9,    31,    10,    12,    11,    13,    14,    15,
    16,    17,    18,    19,     0,   145,   146,   147,   148,   120,
   121,   122,   104,     0,   144,     0,     0,   131,   143,     8,
     7,     6,     0,    72,    24,     0,     0,     0,   109,     0,
   101,   100,   114,     0,     0,   112,   111,   110,   117,     0,
     0,   115,   123,   124,     0,     0,     0,    32,     0,     0,
   107,     0,     0,    74,     0,    51,   114,    62,   114,    63,
   114,    23,     0,    25,    26,    77,     8,    68,     0,     0,
     0,     0,    44,   114,     0,     0,     0,     0,    97,    80,
     0,   102,    64,     0,     0,     0,    76,     0,    99,   118,
     0,     0,     0,     0,     0,     0,    93,     0,    53,   114,
    55,   114,    57,   114,     0,    96,   114,     0,     0,     0,
   162,   155,   151,     0,     5,     2,     0,   105,     0,     0,
     0,     0,    83,    71,   114,     0,    29,    30,     0,   113,
     0,     0,     0,     0,   116,     0,     0,    22,     0,     0,
    34,    33,    73,   114,     0,     0,     0,     0,    67,     0,
     0,    42,     0,     0,    28,    27,    79,    75,    81,   114,
    78,    82,    84,    98,     0,     0,     0,     0,     0,    39,
    92,    94,     0,     0,     0,   142,     0,   140,   141,     0,
     0,     0,   154,     0,    65,   130,   129,   125,   126,   127,
   128,     0,     0,    85,     0,    89,    66,     0,     0,     0,
   108,   106,     0,    50,     0,    58,    61,     0,     0,   137,
     0,   136,   138,    70,   153,   152,     0,    43,     0,    21,
    35,    36,    37,    38,    52,    54,    56,    95,   139,    86,
     0,   158,     0,   156,   157,    90,    91,    87,    48,    47,
    45,    49,     0,    46,     0,     0,    69,     0,     0,    41,
    60,    88,     0,     0,    59,   135,   134,   133,   132,   149,
   150,   160,   159,     0,     0,     0
};

static const short yydefgoto[] = {   334,
    61,    94,    62,    63,    64,    65,    66,   223,    67,    68,
    69,    70,    71,    72,    73,    74,   151,   235,   103,   154,
    85,   120,   121,    98,   108,   199,   200,   159,   111,   112,
   113,    99,   281,   282,   247,   248,    88,   114,   285,    75,
   287,   183
};

static const short yypact[] = {   600,
   235,-32768,-32768,    22,   195,   235,   235,   235,   235,    36,
    36,    36,     9,   504,   504,   504,   504,    62,    38,   445,
   305,   305,   195,   504,   195,   195,   195,   176,     9,     9,
   504,   305,   235,   235,   235,   130,   160,   464,   -68,   -68,
   195,   -68,   130,   130,   130,   504,   130,   130,    36,   195,
    36,   305,   305,   305,   504,   195,    36,    36,   144,     9,
   677,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    16,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   235,-32768,   -55,    11,-32768,-32768,-32768,
-32768,-32768,   195,-32768,-32768,   385,   385,   195,   243,   195,
-32768,-32768,   504,   504,   504,-32768,-32768,   235,-32768,   504,
   413,-32768,-32768,-32768,   504,   504,   195,-32768,    39,   216,
-32768,    72,   195,-32768,   504,-32768,   504,-32768,   504,-32768,
   504,-32768,   504,-32768,-32768,-32768,   226,-32768,   195,    50,
    -3,   105,-32768,   504,   195,   195,    11,   195,-32768,-32768,
   195,-32768,-32768,   504,   195,   195,-32768,   195,    33,-32768,
    36,    36,    23,    35,   195,   195,-32768,   195,-32768,   504,
-32768,   504,-32768,   504,   124,-32768,   504,   504,   413,   413,
-32768,    75,-32768,   195,-32768,-32768,     0,-32768,   235,   235,
   235,   235,-32768,-32768,   341,   341,-32768,-32768,   195,-32768,
   504,   195,   195,   124,-32768,   124,   504,-32768,   413,    71,
-32768,-32768,-32768,   504,   195,   242,   195,   504,-32768,   420,
   195,-32768,    77,   195,-32768,-32768,-32768,-32768,-32768,   504,
-32768,-32768,-32768,-32768,   195,   195,   195,   195,   195,-32768,
-32768,-32768,   195,   195,   195,-32768,   487,-32768,-32768,   195,
   504,   -43,-32768,   -36,-32768,-32768,-32768,   -34,   -34,-32768,
-32768,   195,   195,-32768,   195,-32768,-32768,   487,   487,   195,
-32768,-32768,   195,-32768,   420,-32768,-32768,   195,   420,-32768,
     2,-32768,-32768,-32768,     5,    76,   195,-32768,   195,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   195,    92,    78,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,     2,-32768,     1,   289,-32768,    77,    77,-32768,
-32768,-32768,   100,   106,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   183,   194,-32768
};

static const short yypgoto[] = {-32768,
-32768,   114,   134,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   249,-32768,    10,-32768,
     6,-32768,    -8,    -5,    20,   -57,   390,   252,     8,    95,
-32768,   180,  -215,-32768,   -50,  -178,-32768,    -1,-32768,  -193,
-32768,-32768
};


#define	YYLAST		772


static const short yytable[] = {    89,
   256,   326,    90,   100,    89,    89,    89,    89,    86,   106,
    91,    90,   107,    86,    86,    86,    86,    92,   149,    91,
   104,   105,    90,   119,   186,   122,    92,   145,   146,   286,
    91,    89,    89,    89,   302,   188,   101,    92,    90,   303,
    86,    86,    86,   148,   304,   305,    91,   139,   140,   141,
   160,   160,   160,    92,   160,   160,   209,   182,   166,   313,
   168,   220,    90,   315,   191,   192,   177,   178,   299,   215,
    91,   216,    90,   217,    80,    81,    82,    92,   327,   184,
    91,   221,    89,    80,    81,    82,   224,    92,   318,   299,
   299,    86,   316,   316,    89,    89,   189,   190,   191,   192,
   329,   257,   203,    86,    86,    90,    89,   189,   190,   191,
   192,   238,   243,    91,   244,    86,   245,    93,    95,   250,
    92,   102,   234,   239,   330,   331,    76,    77,    78,    79,
   123,   118,   124,   126,   128,   130,   132,   262,   134,   135,
   136,   138,    80,    81,    82,   143,    76,    77,    78,    79,
   150,   153,    83,   268,   157,   269,   273,   254,   324,   319,
    90,    83,   188,   167,    59,   169,   171,   173,    91,   176,
   236,   237,   289,   249,   323,    92,   137,   252,   253,   107,
    87,   332,   335,   333,    91,    96,    97,    89,    89,    89,
    89,    92,   -40,   336,   185,    90,    86,    86,    86,    86,
   193,   272,   249,    91,   249,   205,   194,   271,   246,     0,
    92,   197,     0,   198,   147,   119,    90,    86,   283,     0,
   179,     0,   180,   181,    91,   280,  -112,    86,     0,     0,
   208,    92,   210,   211,  -112,   212,   213,    76,    77,    78,
    79,  -112,    90,     0,     0,   249,   149,    80,    81,    82,
    91,     0,   219,   275,     0,   222,     0,    92,   225,   226,
   227,   228,     0,   187,   229,     0,   249,   249,   231,   232,
     0,   233,     0,   283,     0,   195,   196,   283,   240,   241,
   280,   242,    86,     0,   280,     0,    86,   155,   156,   328,
   158,    76,    77,    78,    79,   161,   162,   255,   164,   165,
     0,    80,    81,    82,     0,    90,     0,    76,    77,    78,
    79,     0,   264,    91,   283,   266,   267,    80,    81,    82,
    92,   280,     0,    86,     0,    83,     0,     0,   274,   276,
   277,     0,     0,     0,   284,    84,     0,   288,     0,   189,
   190,   191,   192,    76,    77,    78,    79,     0,   290,   291,
   292,   293,   294,    80,    81,    82,   295,   296,   297,     0,
   298,     0,     0,   300,     0,   279,     0,     0,   258,   259,
   260,   261,     0,     0,     0,   306,   307,     0,   308,    83,
     0,   309,   310,   311,     0,     0,   312,    76,    77,    78,
    79,   314,     0,   109,   317,     0,     0,    80,    81,    82,
   320,     0,   321,   110,   115,   116,   117,     0,     0,   127,
   129,   131,     0,   133,   322,    76,    77,    78,    79,     0,
   142,   144,    76,    77,    78,    79,   325,     0,     0,   109,
     0,     0,    80,    81,    82,   163,     0,   189,   190,   191,
   192,   170,   172,   174,   175,    90,     0,    76,    77,    78,
    79,     0,     0,    91,     0,     0,     0,    80,    81,    82,
    92,     0,     0,     0,    90,     0,  -103,  -103,  -103,  -103,
     0,     0,    91,     0,   152,    83,  -103,  -103,  -103,    92,
     0,   189,   190,   191,   192,    84,     0,    90,     0,    76,
    77,    78,    79,   201,   202,    91,   279,     0,     0,   204,
     0,     0,    92,     0,   206,   207,    76,    77,    78,    79,
    83,     0,     0,     0,   214,     0,    80,    81,    82,     0,
     0,     0,   218,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   109,     0,     0,     0,   125,     0,     0,
     0,     0,     0,   230,     0,     0,     0,     0,     0,     0,
     0,     0,  -103,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   251,     0,     0,
     0,   246,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   263,     0,     0,     0,     0,
   265,     0,   109,     0,     0,     0,   270,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     1,   278,     2,     3,
     0,     0,     0,     0,     0,     0,     0,     0,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,     0,     0,    19,    20,    21,    22,    23,
   301,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    56,    57,    58,     0,     0,     0,     0,
     0,     0,     0,     1,     0,     0,     3,    59,     0,     0,
     0,     0,     0,     0,    60,     4,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
     0,     0,    19,    20,    21,    22,    23,     0,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56,    57,    58,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    60
};

static const short yycheck[] = {     1,
     1,     1,     1,     9,     6,     7,     8,     9,     1,     1,
     9,     1,     4,     6,     7,     8,     9,    16,    87,     9,
    11,    12,     1,    18,     9,    18,    16,    33,    34,   223,
     9,    33,    34,    35,    78,    91,     1,    16,     1,    83,
    33,    34,    35,    36,    81,    82,     9,    28,    29,    30,
    43,    44,    45,    16,    47,    48,    18,    59,    49,   275,
    51,    12,     1,   279,    99,   100,    57,    58,   247,   127,
     9,   129,     1,   131,    13,    14,    15,    16,    78,    60,
     9,    85,    84,    13,    14,    15,   144,    16,    84,   268,
   269,    84,    92,    92,    96,    97,    97,    98,    99,   100,
   316,   102,   108,    96,    97,     1,   108,    97,    98,    99,
   100,    89,   170,     9,   172,   108,   174,    96,     5,   177,
    16,    86,    90,    89,   318,   319,     3,     4,     5,     6,
    93,    18,    19,    20,    21,    22,    23,   195,    25,    26,
    27,    28,    13,    14,    15,    32,     3,     4,     5,     6,
    37,    38,    91,   204,    41,   206,   214,    83,    81,    84,
     1,    91,    91,    50,    88,    52,    53,    54,     9,    56,
   161,   162,   230,   175,    83,    16,     1,   179,   180,     4,
     1,    82,     0,    78,     9,     6,     7,   189,   190,   191,
   192,    16,    88,     0,    61,     1,   189,   190,   191,   192,
    87,   210,   204,     9,   206,   111,    93,   209,    85,    -1,
    16,    98,    -1,   100,    35,   210,     1,   210,   220,    -1,
    77,    -1,    79,    80,     9,   220,     1,   220,    -1,    -1,
   117,    16,    17,   120,     9,   122,   123,     3,     4,     5,
     6,    16,     1,    -1,    -1,   247,    87,    13,    14,    15,
     9,    -1,   139,    12,    -1,   142,    -1,    16,   145,   146,
   147,   148,    -1,    84,   151,    -1,   268,   269,   155,   156,
    -1,   158,    -1,   275,    -1,    96,    97,   279,   165,   166,
   275,   168,   275,    -1,   279,    -1,   279,    39,    40,     1,
    42,     3,     4,     5,     6,    44,    45,   184,    47,    48,
    -1,    13,    14,    15,    -1,     1,    -1,     3,     4,     5,
     6,    -1,   199,     9,   316,   202,   203,    13,    14,    15,
    16,   316,    -1,   316,    -1,    91,    -1,    -1,   215,   216,
   217,    -1,    -1,    -1,   221,   101,    -1,   224,    -1,    97,
    98,    99,   100,     3,     4,     5,     6,    -1,   235,   236,
   237,   238,   239,    13,    14,    15,   243,   244,   245,    -1,
   247,    -1,    -1,   250,    -1,    77,    -1,    -1,   189,   190,
   191,   192,    -1,    -1,    -1,   262,   263,    -1,   265,    91,
    -1,   268,   269,   270,    -1,    -1,   273,     3,     4,     5,
     6,   278,    -1,    89,   281,    -1,    -1,    13,    14,    15,
   287,    -1,   289,    14,    15,    16,    17,    -1,    -1,    20,
    21,    22,    -1,    24,   301,     3,     4,     5,     6,    -1,
    31,    32,     3,     4,     5,     6,   313,    -1,    -1,    89,
    -1,    -1,    13,    14,    15,    46,    -1,    97,    98,    99,
   100,    52,    53,    54,    55,     1,    -1,     3,     4,     5,
     6,    -1,    -1,     9,    -1,    -1,    -1,    13,    14,    15,
    16,    -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,
    -1,    -1,     9,    -1,    11,    91,    13,    14,    15,    16,
    -1,    97,    98,    99,   100,   101,    -1,     1,    -1,     3,
     4,     5,     6,   104,   105,     9,    77,    -1,    -1,   110,
    -1,    -1,    16,    -1,   115,   116,     3,     4,     5,     6,
    91,    -1,    -1,    -1,   125,    -1,    13,    14,    15,    -1,
    -1,    -1,   133,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    89,    -1,    -1,    -1,    93,    -1,    -1,
    -1,    -1,    -1,   154,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   178,    -1,    -1,
    -1,    85,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   196,    -1,    -1,    -1,    -1,
   201,    -1,    89,    -1,    -1,    -1,   207,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     7,   218,     9,    10,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    -1,    -1,    36,    37,    38,    39,    40,
   251,    42,    43,    44,    45,    46,    47,    48,    49,    50,
    51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
    61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
    71,    72,    73,    74,    75,    76,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,     7,    -1,    -1,    10,    88,    -1,    -1,
    -1,    -1,    -1,    -1,    95,    19,    20,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
    -1,    -1,    36,    37,    38,    39,    40,    -1,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
    64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
    74,    75,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    95
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

case 1:
#line 121 "../../src/mon_parse.y"
{ yyval.i = 0; ;
    break;}
case 2:
#line 122 "../../src/mon_parse.y"
{ yyval.i = 0; ;
    break;}
case 3:
#line 123 "../../src/mon_parse.y"
{ new_cmd = 1; asm_mode = 0;  yyval.i = 0; ;
    break;}
case 8:
#line 132 "../../src/mon_parse.y"
{ return ERR_EXPECT_END_CMD; ;
    break;}
case 20:
#line 146 "../../src/mon_parse.y"
{ return ERR_BAD_CMD; ;
    break;}
case 21:
#line 149 "../../src/mon_parse.y"
{ mon_bank(yyvsp[-2].i,yyvsp[-1].str); ;
    break;}
case 22:
#line 150 "../../src/mon_parse.y"
{ mon_jump(yyvsp[-1].a); ;
    break;}
case 23:
#line 151 "../../src/mon_parse.y"
{ fprintf(mon_output, "Display IO registers\n"); ;
    break;}
case 24:
#line 152 "../../src/mon_parse.y"
{ mon_instruction_return(); ;
    break;}
case 25:
#line 153 "../../src/mon_parse.y"
{ puts("Dump machine state."); ;
    break;}
case 26:
#line 154 "../../src/mon_parse.y"
{ puts("Undump machine state."); ;
    break;}
case 27:
#line 155 "../../src/mon_parse.y"
{ mon_instructions_step(yyvsp[-1].i); ;
    break;}
case 28:
#line 156 "../../src/mon_parse.y"
{ mon_instructions_next(yyvsp[-1].i); ;
    break;}
case 29:
#line 157 "../../src/mon_parse.y"
{ mon_stack_up(yyvsp[-1].i); ;
    break;}
case 30:
#line 158 "../../src/mon_parse.y"
{ mon_stack_down(yyvsp[-1].i); ;
    break;}
case 32:
#line 162 "../../src/mon_parse.y"
{ mon_print_registers(default_memspace); ;
    break;}
case 33:
#line 163 "../../src/mon_parse.y"
{ mon_print_registers(yyvsp[-1].i); ;
    break;}
case 35:
#line 167 "../../src/mon_parse.y"
{ mon_load_symbols(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 36:
#line 168 "../../src/mon_parse.y"
{ mon_save_symbols(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 37:
#line 169 "../../src/mon_parse.y"
{ mon_add_name_to_symbol_table(yyvsp[-2].a, yyvsp[-1].str); ;
    break;}
case 38:
#line 170 "../../src/mon_parse.y"
{ mon_remove_name_from_symbol_table(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 39:
#line 171 "../../src/mon_parse.y"
{ mon_print_symbol_table(yyvsp[-1].i); ;
    break;}
case 40:
#line 174 "../../src/mon_parse.y"
{ mon_start_assemble_mode(yyvsp[0].a, NULL); ;
    break;}
case 42:
#line 175 "../../src/mon_parse.y"
{ mon_start_assemble_mode(yyvsp[-1].a, NULL); ;
    break;}
case 43:
#line 176 "../../src/mon_parse.y"
{ mon_disassemble_lines(yyvsp[-2].a,yyvsp[-1].a); ;
    break;}
case 44:
#line 177 "../../src/mon_parse.y"
{ mon_disassemble_lines(BAD_ADDR, BAD_ADDR); ;
    break;}
case 45:
#line 180 "../../src/mon_parse.y"
{ mon_move_memory(yyvsp[-3].a, yyvsp[-2].a, yyvsp[-1].a); ;
    break;}
case 46:
#line 181 "../../src/mon_parse.y"
{ mon_compare_memory(yyvsp[-3].a, yyvsp[-2].a, yyvsp[-1].a); ;
    break;}
case 47:
#line 182 "../../src/mon_parse.y"
{ mon_fill_memory(yyvsp[-3].a, yyvsp[-2].a, yyvsp[-1].str); ;
    break;}
case 48:
#line 183 "../../src/mon_parse.y"
{ mon_hunt_memory(yyvsp[-3].a, yyvsp[-2].a, yyvsp[-1].str); ;
    break;}
case 49:
#line 184 "../../src/mon_parse.y"
{ mon_display_memory(yyvsp[-3].rt, yyvsp[-2].a, yyvsp[-1].a); ;
    break;}
case 50:
#line 185 "../../src/mon_parse.y"
{ mon_display_memory(default_radix, yyvsp[-2].a, yyvsp[-1].a); ;
    break;}
case 51:
#line 186 "../../src/mon_parse.y"
{ mon_display_memory(default_radix, BAD_ADDR, BAD_ADDR); ;
    break;}
case 52:
#line 187 "../../src/mon_parse.y"
{ mon_display_data(yyvsp[-2].a, yyvsp[-1].a, 8, 8); ;
    break;}
case 53:
#line 188 "../../src/mon_parse.y"
{ mon_display_data(BAD_ADDR, BAD_ADDR, 8, 8); ;
    break;}
case 54:
#line 189 "../../src/mon_parse.y"
{ mon_display_data(yyvsp[-2].a, yyvsp[-1].a, 24, 21); ;
    break;}
case 55:
#line 190 "../../src/mon_parse.y"
{ mon_display_data(BAD_ADDR, BAD_ADDR, 24, 21); ;
    break;}
case 56:
#line 191 "../../src/mon_parse.y"
{ mon_display_memory(0, yyvsp[-2].a, yyvsp[-1].a); ;
    break;}
case 57:
#line 192 "../../src/mon_parse.y"
{ mon_display_memory(0, BAD_ADDR, BAD_ADDR); ;
    break;}
case 58:
#line 195 "../../src/mon_parse.y"
{ mon_add_checkpoint(yyvsp[-2].a, yyvsp[-1].a, FALSE, FALSE, FALSE); ;
    break;}
case 59:
#line 196 "../../src/mon_parse.y"
{
                          temp = mon_add_checkpoint(yyvsp[-4].a, yyvsp[-3].a, FALSE, FALSE, FALSE);
                          mon_set_checkpoint_condition(temp, yyvsp[-1].cond_node); ;
    break;}
case 60:
#line 199 "../../src/mon_parse.y"
{ mon_add_checkpoint(yyvsp[-2].a, yyvsp[-1].a, FALSE,
                              (yyvsp[-3].i == e_load || yyvsp[-3].i == e_load_store), (yyvsp[-3].i == e_store || yyvsp[-3].i == e_load_store)); ;
    break;}
case 61:
#line 201 "../../src/mon_parse.y"
{ mon_add_checkpoint(yyvsp[-2].a, yyvsp[-1].a, TRUE, FALSE, FALSE); ;
    break;}
case 62:
#line 202 "../../src/mon_parse.y"
{ mon_print_checkpoints(); ;
    break;}
case 63:
#line 203 "../../src/mon_parse.y"
{ mon_print_checkpoints(); ;
    break;}
case 64:
#line 204 "../../src/mon_parse.y"
{ mon_print_checkpoints(); ;
    break;}
case 65:
#line 208 "../../src/mon_parse.y"
{ mon_switch_checkpoint(yyvsp[-2].action, yyvsp[-1].i); ;
    break;}
case 66:
#line 209 "../../src/mon_parse.y"
{ mon_set_ignore_count(yyvsp[-2].i, yyvsp[-1].i); ;
    break;}
case 67:
#line 210 "../../src/mon_parse.y"
{ mon_delete_checkpoint(yyvsp[-1].i); ;
    break;}
case 68:
#line 211 "../../src/mon_parse.y"
{ mon_delete_checkpoint(-1); ;
    break;}
case 69:
#line 212 "../../src/mon_parse.y"
{ mon_set_checkpoint_condition(yyvsp[-3].i, yyvsp[-1].cond_node); ;
    break;}
case 70:
#line 213 "../../src/mon_parse.y"
{ mon_set_checkpoint_command(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 71:
#line 216 "../../src/mon_parse.y"
{ sidefx = ((yyvsp[-1].action==e_TOGGLE)?(sidefx^1):yyvsp[-1].action); ;
    break;}
case 72:
#line 217 "../../src/mon_parse.y"
{ fprintf(mon_output, "I/O side effects are %s\n",sidefx ? "enabled" : "disabled"); ;
    break;}
case 73:
#line 218 "../../src/mon_parse.y"
{ default_radix = yyvsp[-1].rt; ;
    break;}
case 74:
#line 220 "../../src/mon_parse.y"
{
                         const char *p;

                         if (default_radix == e_hexadecimal)
                             p = "Hexadecimal";
                         else if (default_radix == e_decimal)
                             p = "Decimal";
                         else if (default_radix == e_octal)
                             p = "Octal";
                         else if (default_radix == e_binary)
                             p = "Binary";
                         else
                             p = "Unknown";

                         fprintf(mon_output, "Default radix is %s\n", p);
                     ;
    break;}
case 75:
#line 237 "../../src/mon_parse.y"
{ fprintf(mon_output,"Setting default device to `%s'\n",
                                                         _mon_space_strings[(int) yyvsp[-1].i]); default_memspace = yyvsp[-1].i; ;
    break;}
case 76:
#line 239 "../../src/mon_parse.y"
{ exit_mon = 2; YYACCEPT; ;
    break;}
case 77:
#line 240 "../../src/mon_parse.y"
{ exit_mon = 1; YYACCEPT; ;
    break;}
case 78:
#line 243 "../../src/mon_parse.y"
{ mon_execute_disk_command(yyvsp[-1].str); ;
    break;}
case 79:
#line 244 "../../src/mon_parse.y"
{ fprintf(mon_output, "\t%d\n",yyvsp[-1].i); ;
    break;}
case 80:
#line 245 "../../src/mon_parse.y"
{ mon_print_help(NULL); ;
    break;}
case 81:
#line 246 "../../src/mon_parse.y"
{ mon_print_help(yyvsp[-1].str); ;
    break;}
case 82:
#line 247 "../../src/mon_parse.y"
{ printf("SYSTEM COMMAND: %s\n",yyvsp[-1].str); ;
    break;}
case 83:
#line 248 "../../src/mon_parse.y"
{ mon_print_convert(yyvsp[-1].i); ;
    break;}
case 84:
#line 249 "../../src/mon_parse.y"
{ mon_change_dir(yyvsp[-1].str); ;
    break;}
case 85:
#line 252 "../../src/mon_parse.y"
{ mon_load_file(yyvsp[-2].str,yyvsp[-1].a); ;
    break;}
case 86:
#line 253 "../../src/mon_parse.y"
{ mon_bload_file(yyvsp[-2].str,yyvsp[-1].a); ;
    break;}
case 87:
#line 254 "../../src/mon_parse.y"
{ mon_save_file(yyvsp[-3].str,yyvsp[-2].a,yyvsp[-1].a); ;
    break;}
case 88:
#line 255 "../../src/mon_parse.y"
{ mon_bsave_file(yyvsp[-3].str,yyvsp[-2].a,yyvsp[-1].a); ;
    break;}
case 89:
#line 256 "../../src/mon_parse.y"
{ mon_verify_file(yyvsp[-2].str,yyvsp[-1].a); ;
    break;}
case 90:
#line 257 "../../src/mon_parse.y"
{ mon_block_cmd(0,yyvsp[-3].i,yyvsp[-2].i,yyvsp[-1].a); ;
    break;}
case 91:
#line 258 "../../src/mon_parse.y"
{ mon_block_cmd(1,yyvsp[-3].i,yyvsp[-2].i,yyvsp[-1].a); ;
    break;}
case 92:
#line 261 "../../src/mon_parse.y"
{ mon_record_commands(yyvsp[-1].str); ;
    break;}
case 93:
#line 262 "../../src/mon_parse.y"
{ mon_end_recording(); ;
    break;}
case 94:
#line 263 "../../src/mon_parse.y"
{ playback=TRUE; playback_name = yyvsp[-1].str; ;
    break;}
case 95:
#line 266 "../../src/mon_parse.y"
{ mon_fill_memory(yyvsp[-2].a, BAD_ADDR, yyvsp[-1].str); ;
    break;}
case 96:
#line 267 "../../src/mon_parse.y"
{ printf("Not yet.\n"); ;
    break;}
case 97:
#line 270 "../../src/mon_parse.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 99:
#line 274 "../../src/mon_parse.y"
{ yyval.str = NULL; ;
    break;}
case 101:
#line 278 "../../src/mon_parse.y"
{ return ERR_EXPECT_FILENAME; ;
    break;}
case 102:
#line 281 "../../src/mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 103:
#line 282 "../../src/mon_parse.y"
{ yyval.i = e_load_store; ;
    break;}
case 104:
#line 285 "../../src/mon_parse.y"
{ yyval.i = new_reg(default_memspace, yyvsp[0].reg); ;
    break;}
case 105:
#line 286 "../../src/mon_parse.y"
{ yyval.i = new_reg(yyvsp[-1].i, yyvsp[0].reg); ;
    break;}
case 108:
#line 293 "../../src/mon_parse.y"
{ mon_set_reg_val(reg_memspace(yyvsp[-2].i), reg_regid(yyvsp[-2].i), yyvsp[0].i); ;
    break;}
case 109:
#line 296 "../../src/mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 110:
#line 297 "../../src/mon_parse.y"
{ yyval.i = -1; ;
    break;}
case 111:
#line 300 "../../src/mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 112:
#line 301 "../../src/mon_parse.y"
{ return ERR_EXPECT_BRKNUM; ;
    break;}
case 113:
#line 304 "../../src/mon_parse.y"
{ yyval.a = yyvsp[0].a; ;
    break;}
case 114:
#line 305 "../../src/mon_parse.y"
{ yyval.a = BAD_ADDR; ;
    break;}
case 115:
#line 308 "../../src/mon_parse.y"
{ yyval.a = new_addr(e_default_space,yyvsp[0].i); if (opt_asm) new_cmd = asm_mode = 1; ;
    break;}
case 116:
#line 309 "../../src/mon_parse.y"
{ yyval.a = new_addr(yyvsp[-1].i,yyvsp[0].i); if (opt_asm) new_cmd = asm_mode = 1; ;
    break;}
case 117:
#line 310 "../../src/mon_parse.y"
{ temp = mon_symbol_table_lookup_addr(e_default_space, yyvsp[0].str); 
                 if (temp >= 0)
                    yyval.a = new_addr(e_default_space, temp);
                 else
                    printf("ERRR\n"); ;
    break;}
case 118:
#line 317 "../../src/mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 119:
#line 318 "../../src/mon_parse.y"
{ yyval.i = e_default_space; ;
    break;}
case 120:
#line 321 "../../src/mon_parse.y"
{ yyval.i = e_comp_space; ;
    break;}
case 121:
#line 322 "../../src/mon_parse.y"
{ yyval.i = e_disk8_space; ;
    break;}
case 122:
#line 323 "../../src/mon_parse.y"
{ yyval.i = e_disk9_space; ;
    break;}
case 123:
#line 326 "../../src/mon_parse.y"
{ yyval.i = yyvsp[0].i; if (!CHECK_ADDR(yyvsp[0].i)) return ERR_ADDR_TOO_BIG; ;
    break;}
case 124:
#line 329 "../../src/mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 125:
#line 331 "../../src/mon_parse.y"
{ yyval.i = yyvsp[-2].i + yyvsp[0].i; ;
    break;}
case 126:
#line 332 "../../src/mon_parse.y"
{ yyval.i = yyvsp[-2].i - yyvsp[0].i; ;
    break;}
case 127:
#line 333 "../../src/mon_parse.y"
{ yyval.i = yyvsp[-2].i * yyvsp[0].i; ;
    break;}
case 128:
#line 334 "../../src/mon_parse.y"
{ yyval.i = (yyvsp[0].i) ? (yyvsp[-2].i / yyvsp[0].i) : 1; ;
    break;}
case 129:
#line 335 "../../src/mon_parse.y"
{ yyval.i = yyvsp[-1].i; ;
    break;}
case 130:
#line 336 "../../src/mon_parse.y"
{ return ERR_MISSING_CLOSE_PAREN; ;
    break;}
case 131:
#line 337 "../../src/mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 132:
#line 340 "../../src/mon_parse.y"
{ yyval.cond_node = new_cond; yyval.cond_node->is_parenthized = FALSE;
                                            yyval.cond_node->child1 = yyvsp[-2].cond_node; yyval.cond_node->child2 = yyvsp[0].cond_node; yyval.cond_node->operation = yyvsp[-1].cond_op; ;
    break;}
case 133:
#line 342 "../../src/mon_parse.y"
{ return ERR_INCOMPLETE_COMPARE_OP; ;
    break;}
case 134:
#line 343 "../../src/mon_parse.y"
{ yyval.cond_node = yyvsp[-1].cond_node; yyval.cond_node->is_parenthized = TRUE; ;
    break;}
case 135:
#line 344 "../../src/mon_parse.y"
{ return ERR_MISSING_CLOSE_PAREN; ;
    break;}
case 136:
#line 345 "../../src/mon_parse.y"
{ yyval.cond_node = yyvsp[0].cond_node; ;
    break;}
case 137:
#line 348 "../../src/mon_parse.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV; yyval.cond_node->is_parenthized = FALSE;
                            yyval.cond_node->reg_num = yyvsp[0].i; yyval.cond_node->is_reg = TRUE; ;
    break;}
case 138:
#line 350 "../../src/mon_parse.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV; yyval.cond_node->is_parenthized = FALSE;
                            yyval.cond_node->value = yyvsp[0].i; yyval.cond_node->is_reg = FALSE; ;
    break;}
case 141:
#line 358 "../../src/mon_parse.y"
{ mon_add_number_to_buffer(yyvsp[0].i); ;
    break;}
case 142:
#line 359 "../../src/mon_parse.y"
{ mon_add_string_to_buffer(yyvsp[0].str); ;
    break;}
case 143:
#line 362 "../../src/mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 144:
#line 363 "../../src/mon_parse.y"
{ yyval.i = mon_get_reg_val(reg_memspace(yyvsp[0].i), reg_regid(yyvsp[0].i)); ;
    break;}
case 145:
#line 366 "../../src/mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 146:
#line 367 "../../src/mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 147:
#line 368 "../../src/mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 148:
#line 369 "../../src/mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 151:
#line 376 "../../src/mon_parse.y"
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
case 153:
#line 387 "../../src/mon_parse.y"
{ asm_mode = 0; ;
    break;}
case 154:
#line 390 "../../src/mon_parse.y"
{ if (yyvsp[0].i > 0xff) return ERR_IMM_TOO_BIG;
                                         yyval.i = join_ints(ASM_ADDR_MODE_IMMEDIATE,yyvsp[0].i); ;
    break;}
case 155:
#line 392 "../../src/mon_parse.y"
{ if (yyvsp[0].i < 0x100)
                              yyval.i = join_ints(ASM_ADDR_MODE_ZERO_PAGE,yyvsp[0].i);
                           else
                              yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE,yyvsp[0].i);
                         ;
    break;}
case 156:
#line 397 "../../src/mon_parse.y"
{ if (yyvsp[-2].i < 0x100)
                                           yyval.i = join_ints(ASM_ADDR_MODE_ZERO_PAGE_X,yyvsp[-2].i);
                                        else
                                           yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE_X,yyvsp[-2].i);
                                      ;
    break;}
case 157:
#line 402 "../../src/mon_parse.y"
{ if (yyvsp[-2].i < 0x100)
                                           yyval.i = join_ints(ASM_ADDR_MODE_ZERO_PAGE_Y,yyvsp[-2].i);
                                        else
                                           yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE_Y,yyvsp[-2].i);
                                      ;
    break;}
case 158:
#line 407 "../../src/mon_parse.y"
{ yyval.i = join_ints(ASM_ADDR_MODE_ABS_INDIRECT,yyvsp[-1].i); ;
    break;}
case 159:
#line 408 "../../src/mon_parse.y"
{ yyval.i = join_ints(ASM_ADDR_MODE_INDIRECT_X,yyvsp[-3].i); ;
    break;}
case 160:
#line 409 "../../src/mon_parse.y"
{ yyval.i = join_ints(ASM_ADDR_MODE_INDIRECT_Y,yyvsp[-3].i); ;
    break;}
case 161:
#line 410 "../../src/mon_parse.y"
{ yyval.i = join_ints(ASM_ADDR_MODE_IMPLIED,0); ;
    break;}
case 162:
#line 411 "../../src/mon_parse.y"
{ yyval.i = join_ints(ASM_ADDR_MODE_ACCUMULATOR,0); ;
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
#line 415 "../../src/mon_parse.y"


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
       fprintf(mon_output, "ERROR -- ");
       switch(rc) {
           case ERR_BAD_CMD:
               fprintf(mon_output, "Bad command:\n");
               break;
           case ERR_RANGE_BAD_START:
               fprintf(mon_output, "Bad first address in range:\n");
               break;
           case ERR_RANGE_BAD_END:
               fprintf(mon_output, "Bad second address in range:\n");
               break;
           case ERR_EXPECT_BRKNUM:
               fprintf(mon_output, "Checkpoint number expected:\n");
               break;
           case ERR_EXPECT_END_CMD:
               fprintf(mon_output, "Unexpected token:\n");
               break;
           case ERR_MISSING_CLOSE_PAREN:
               fprintf(mon_output, "')' expected:\n");
               break;
           case ERR_INCOMPLETE_COMPARE_OP:
               fprintf(mon_output, "Compare operation missing an operand:\n");
               break;
           case ERR_EXPECT_FILENAME:
               fprintf(mon_output, "Expecting a filename:\n");
               break;
           case ERR_ADDR_TOO_BIG:
               fprintf(mon_output, "Address too large:\n");
               break;
           case ERR_IMM_TOO_BIG:
               fprintf(mon_output, "Immediate argument too large:\n");
               break;
           case ERR_ILLEGAL_INPUT:
           default:
               fprintf(mon_output, "Wrong syntax:\n");
       }
       fprintf(mon_output, "  %s\n", input);
       for (i = 0; i < last_len; i++)
           fprintf(mon_output, " ");
       fprintf(mon_output, "  ^\n");
       asm_mode = 0;
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

