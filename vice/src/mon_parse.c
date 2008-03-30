
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
#define	CMD_TEXT_DISPLAY	326
#define	CMD_ENTER_DATA	327
#define	CMD_ENTER_BIN_DATA	328
#define	L_PAREN	329
#define	R_PAREN	330
#define	ARG_IMMEDIATE	331
#define	REG_A	332
#define	REG_X	333
#define	REG_Y	334
#define	COMMA	335
#define	INST_SEP	336
#define	STRING	337
#define	FILENAME	338
#define	R_O_L	339
#define	OPCODE	340
#define	LABEL	341
#define	REGISTER	342
#define	COMPARE_OP	343
#define	RADIX_TYPE	344
#define	INPUT_SPEC	345
#define	CMD_CHECKPT_ONOFF	346
#define	TOGGLE	347

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
#define ERR_ADDR_TOO_BIG 9

#define BAD_ADDR (new_addr(e_invalid_space, 0))
#define CHECK_ADDR(x) ((x) == LO16(x))


#line 41 "mon_parse.y"
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



#define	YYFINAL		319
#define	YYFLAG		-32768
#define	YYNTBASE	99

#define YYTRANSLATE(x) ((unsigned)(x) <= 347 ? yytranslate[x] : 141)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    97,
    98,    95,    93,     2,    94,     2,    96,     2,     2,     2,
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
    86,    87,    88,    89,    90,    91,    92
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     7,     9,    12,    14,    16,    18,    20,
    22,    24,    26,    28,    30,    32,    34,    36,    38,    40,
    42,    45,    49,    52,    55,    58,    61,    65,    69,    73,
    77,    79,    82,    86,    90,    95,   100,   105,   110,   114,
   115,   121,   125,   130,   133,   139,   145,   151,   157,   163,
   168,   171,   176,   179,   184,   187,   192,   197,   204,   210,
   215,   218,   221,   224,   228,   233,   237,   240,   246,   251,
   255,   258,   262,   265,   269,   272,   275,   279,   283,   286,
   290,   294,   298,   302,   307,   313,   318,   323,   328,   332,
   335,   339,   344,   347,   349,   351,   353,   355,   356,   358,
   361,   365,   367,   371,   373,   374,   376,   378,   380,   381,
   383,   386,   388,   389,   391,   393,   395,   397,   401,   405,
   409,   413,   417,   421,   423,   427,   431,   435,   439,   441,
   443,   445,   448,   450,   452,   454,   456,   458,   460,   462,
   464,   466,   470,   474,   477,   479,   481,   484,   486,   490,
   494,   498,   504,   510,   511
};

static const short yyrhs[] = {   100,
     0,   138,     9,     0,     9,     0,   102,     0,   100,   102,
     0,    15,     0,     9,     0,     1,     0,   103,     0,   105,
     0,   108,     0,   106,     0,   109,     0,   110,     0,   111,
     0,   112,     0,   113,     0,   114,     0,   115,     0,    10,
     0,    60,   101,     0,    31,   125,   101,     0,    39,   101,
     0,    19,   101,     0,    42,   101,     0,    43,   101,     0,
    51,   122,   101,     0,    50,   122,   101,     0,    22,   122,
   101,     0,    23,   122,   101,     0,   104,     0,    32,   101,
     0,    32,   127,   101,     0,    32,   120,   101,     0,    61,
   126,   117,   101,     0,    62,   126,   117,   101,     0,    63,
   125,    86,   101,     0,    64,   126,    86,   101,     0,    65,
   126,   101,     0,     0,    48,   125,   107,   139,   101,     0,
    48,   125,   101,     0,    49,   125,   124,   101,     0,    49,
   101,     0,    30,   125,   125,   125,   101,     0,    41,   125,
   125,   125,   101,     0,    29,   125,   125,   133,   101,     0,
    28,   125,   125,   133,   101,     0,    36,    89,   125,   124,
   101,     0,    36,   125,   124,   101,     0,    36,   101,     0,
    69,   125,   124,   101,     0,    69,   101,     0,    70,   125,
   124,   101,     0,    70,   101,     0,    71,   125,   124,   101,
     0,    37,   125,   124,   101,     0,    37,   125,   124,    12,
   131,   101,     0,    55,   118,   125,   124,   101,     0,    38,
   125,   124,   101,     0,    37,   101,     0,    38,   101,     0,
    55,   101,     0,    91,   123,   101,     0,    27,   123,   122,
   101,     0,    45,   123,   101,     0,    45,   101,     0,    46,
   123,    12,   131,   101,     0,    47,   123,    82,   101,     0,
    18,    92,   101,     0,    18,   101,     0,    35,    89,   101,
     0,    35,   101,     0,    53,   127,   101,     0,    58,   101,
     0,    44,   101,     0,    56,   116,   101,     0,    52,   130,
   101,     0,    54,   101,     0,    54,   116,   101,     0,    57,
   116,   101,     0,     7,   130,   101,     0,    59,   116,   101,
     0,    24,   117,   125,   101,     0,    25,   117,   125,   125,
   101,     0,    26,   117,   125,   101,     0,    20,   130,   130,
   124,     0,    21,   130,   130,   125,     0,    66,   117,   101,
     0,    67,   101,     0,    68,   117,   101,     0,    72,   125,
   133,   101,     0,    73,   101,     0,    84,     0,    83,     0,
     1,     0,    11,     0,     0,    87,     0,   127,    87,     0,
   120,    16,   121,     0,   121,     0,   119,    17,   136,     0,
   130,     0,     0,   136,     0,     1,     0,   125,     0,     0,
   128,     0,   127,   128,     0,   127,     0,     0,    13,     0,
    14,     0,   129,     0,   136,     0,   130,    93,   130,     0,
   130,    94,   130,     0,   130,    95,   130,     0,   130,    96,
   130,     0,    97,   130,    98,     0,    97,   130,     1,     0,
   135,     0,   131,    88,   131,     0,   131,    88,     1,     0,
    74,   131,    75,     0,    74,   131,     1,     0,   132,     0,
   119,     0,   136,     0,   133,   134,     0,   134,     0,   136,
     0,    82,     0,   136,     0,   119,     0,     3,     0,     4,
     0,     5,     0,     6,     0,   137,    81,   138,     0,   138,
    81,   138,     0,    85,   140,     0,   138,     0,   137,     0,
    76,   136,     0,   136,     0,   136,    80,    78,     0,   136,
    80,    79,     0,    74,   136,    75,     0,    74,   136,    80,
    78,    75,     0,    74,   136,    75,    80,    79,     0,     0,
    77,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    90,    91,    92,    95,    96,    99,   100,   101,   104,   105,
   106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
   118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
   128,   131,   132,   133,   136,   137,   138,   139,   140,   143,
   143,   144,   145,   146,   149,   150,   151,   152,   153,   154,
   155,   156,   157,   158,   159,   160,   163,   164,   167,   169,
   170,   171,   172,   176,   177,   178,   179,   180,   181,   184,
   185,   186,   187,   189,   191,   192,   195,   196,   197,   198,
   199,   200,   201,   204,   205,   206,   207,   208,   211,   212,
   213,   216,   217,   220,   223,   224,   227,   228,   231,   232,
   235,   236,   239,   242,   243,   246,   247,   250,   251,   254,
   255,   258,   259,   262,   263,   266,   269,   271,   272,   273,
   274,   275,   276,   277,   280,   282,   283,   284,   285,   288,
   290,   294,   295,   298,   299,   302,   303,   306,   307,   308,
   309,   312,   313,   316,   326,   327,   330,   331,   336,   341,
   346,   347,   348,   349,   350
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
"CMD_SPRITE_DISPLAY","CMD_TEXT_DISPLAY","CMD_ENTER_DATA","CMD_ENTER_BIN_DATA",
"L_PAREN","R_PAREN","ARG_IMMEDIATE","REG_A","REG_X","REG_Y","COMMA","INST_SEP",
"STRING","FILENAME","R_O_L","OPCODE","LABEL","REGISTER","COMPARE_OP","RADIX_TYPE",
"INPUT_SPEC","CMD_CHECKPT_ONOFF","TOGGLE","'+'","'-'","'*'","'/'","'('","')'",
"top_level","command_list","end_cmd","command","machine_state_rules","register_mod",
"symbol_table_rules","asm_rules","@1","memory_rules","checkpoint_rules","checkpoint_control_rules",
"monitor_state_rules","monitor_misc_rules","disk_rules","cmd_file_rules","data_entry_rules",
"rest_of_line","filename","opt_mem_op","register","reg_list","reg_asgn","opt_count",
"breakpt_num","opt_address","address","opt_memspace","memspace","memloc","memaddr",
"expression","cond_expr","compare_operand","data_list","data_element","value",
"number","assembly_instr_list","assembly_instruction","post_assemble","asm_operand_mode", NULL
};
#endif

static const short yyr1[] = {     0,
    99,    99,    99,   100,   100,   101,   101,   101,   102,   102,
   102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
   103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
   103,   104,   104,   104,   105,   105,   105,   105,   105,   107,
   106,   106,   106,   106,   108,   108,   108,   108,   108,   108,
   108,   108,   108,   108,   108,   108,   109,   109,   109,   109,
   109,   109,   109,   110,   110,   110,   110,   110,   110,   111,
   111,   111,   111,   111,   111,   111,   112,   112,   112,   112,
   112,   112,   112,   113,   113,   113,   113,   113,   114,   114,
   114,   115,   115,   116,   117,   117,   118,   118,   119,   119,
   120,   120,   121,   122,   122,   123,   123,   124,   124,   125,
   125,   126,   126,   127,   127,   128,   129,   130,   130,   130,
   130,   130,   130,   130,   131,   131,   131,   131,   131,   132,
   132,   133,   133,   134,   134,   135,   135,   136,   136,   136,
   136,   137,   137,   138,   139,   139,   140,   140,   140,   140,
   140,   140,   140,   140,   140
};

static const short yyr2[] = {     0,
     1,     2,     1,     1,     2,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     2,     3,     2,     2,     2,     2,     3,     3,     3,     3,
     1,     2,     3,     3,     4,     4,     4,     4,     3,     0,
     5,     3,     4,     2,     5,     5,     5,     5,     5,     4,
     2,     4,     2,     4,     2,     4,     4,     6,     5,     4,
     2,     2,     2,     3,     4,     3,     2,     5,     4,     3,
     2,     3,     2,     3,     2,     2,     3,     3,     2,     3,
     3,     3,     3,     4,     5,     4,     4,     4,     3,     2,
     3,     4,     2,     1,     1,     1,     1,     0,     1,     2,
     3,     1,     3,     1,     0,     1,     1,     1,     0,     1,
     2,     1,     0,     1,     1,     1,     1,     3,     3,     3,
     3,     3,     3,     1,     3,     3,     3,     3,     1,     1,
     1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     3,     3,     2,     1,     1,     2,     1,     3,     3,
     3,     5,     5,     0,     1
};

static const short yydefact[] = {     0,
     0,     3,    20,     0,     0,     0,     0,   105,   105,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   105,   105,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   113,   113,     0,   113,   113,     0,     0,
     0,     0,     0,     0,     0,     0,   154,     0,     1,     4,
     9,    31,    10,    12,    11,    13,    14,    15,    16,    17,
    18,    19,     0,   138,   139,   140,   141,   114,   115,    99,
     0,   137,     0,     0,   124,   136,     8,     7,     6,     0,
    71,    24,     0,     0,     0,   104,     0,    96,    95,     0,
     0,     0,   107,   105,   106,     0,     0,   110,   116,   117,
     0,     0,     0,    32,     0,     0,   102,     0,     0,    73,
     0,    51,   109,    61,   109,    62,   109,    23,     0,    25,
    26,    76,     8,    67,     0,     0,     0,     0,    44,   109,
     0,     0,     0,     0,    94,    79,     0,    97,    63,     0,
     0,     0,    75,     0,    21,     0,   112,     0,     0,     0,
     0,     0,    90,     0,    53,   109,    55,   109,   109,     0,
    93,     0,     0,   155,   148,   144,     0,     5,     2,     0,
   100,     0,     0,     0,     0,    82,    70,   109,     0,    29,
    30,     0,     0,     0,     0,     0,   111,     0,     0,    22,
     0,     0,    34,    33,    72,   109,     0,   108,     0,     0,
     0,    66,     0,     0,    42,     0,     0,    28,    27,    78,
    74,    80,   109,    77,    81,    83,     0,     0,     0,     0,
    39,    89,    91,     0,     0,     0,   135,     0,   133,   134,
     0,   147,     0,    64,   123,   122,   118,   119,   120,   121,
    87,    88,    84,     0,    86,    65,     0,     0,     0,   103,
   101,     0,    50,     0,    57,    60,     0,     0,   130,     0,
   129,   131,    69,   146,   145,     0,    43,     0,    35,    36,
    37,    38,    52,    54,    56,    92,   132,   151,     0,   149,
   150,    85,    48,    47,    45,    49,     0,    46,     0,     0,
    68,     0,     0,    41,    59,     0,     0,    58,   128,   127,
   126,   125,   142,   143,   153,   152,     0,     0,     0
};

static const short yydefgoto[] = {   317,
    59,    91,    60,    61,    62,    63,    64,   216,    65,    66,
    67,    68,    69,    70,    71,    72,   147,   100,   150,    82,
   116,   117,    95,   104,   207,   208,   156,   107,   108,   109,
    96,   270,   271,   238,   239,    85,   110,   274,    73,   276,
   176
};

static const short yypact[] = {   646,
   118,-32768,-32768,    62,   158,   118,   118,   118,   118,    36,
    36,    36,   389,   324,   324,   324,   324,   165,    16,   279,
   535,   535,   158,   324,   158,   158,   158,   348,   389,   389,
   324,   535,   118,   118,   118,    51,   146,   520,   -73,   -73,
   158,   -73,   158,    51,    51,   324,    51,    51,    36,   158,
    36,   535,   535,   324,   324,   158,   229,   389,   720,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    27,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   118,-32768,   -48,    57,-32768,-32768,-32768,-32768,-32768,   158,
-32768,-32768,   311,   311,   158,   215,   158,-32768,-32768,   324,
   324,   324,-32768,   118,-32768,   324,   406,-32768,-32768,-32768,
   324,   324,   158,-32768,    38,   148,-32768,   105,   158,-32768,
   324,-32768,   324,-32768,   324,-32768,   324,-32768,   324,-32768,
-32768,-32768,   212,-32768,   158,    37,     9,    85,-32768,   324,
   158,   158,    57,   158,-32768,-32768,   158,-32768,-32768,   324,
   158,   158,-32768,   158,-32768,    36,-32768,    36,    12,    13,
   158,   158,-32768,   158,-32768,   324,-32768,   324,   324,   204,
-32768,   406,   406,-32768,    46,-32768,   158,-32768,-32768,    15,
-32768,   118,   118,   118,   118,-32768,-32768,   130,   130,-32768,
-32768,   158,   324,   158,   158,   204,-32768,   204,   324,-32768,
   406,   -11,-32768,-32768,-32768,   324,   158,-32768,   189,   158,
   324,-32768,   375,   158,-32768,    10,   158,-32768,-32768,-32768,
-32768,-32768,   324,-32768,-32768,-32768,   158,   158,   158,   158,
-32768,-32768,-32768,   158,   158,   158,-32768,   507,-32768,-32768,
   -30,-32768,    -5,-32768,-32768,-32768,    80,    80,-32768,-32768,
-32768,-32768,-32768,   158,-32768,-32768,   507,   507,   158,-32768,
-32768,   158,-32768,   375,-32768,-32768,   158,   375,-32768,    74,
-32768,-32768,-32768,    31,    35,   158,-32768,   158,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    49,    52,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,    74,-32768,    29,   342,
-32768,    10,    10,-32768,-32768,    77,    66,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   168,   177,-32768
};

static const short yypgoto[] = {-32768,
-32768,   223,   132,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   106,     2,-32768,     1,
-32768,   -37,    14,    32,   -81,   452,    92,    34,    82,-32768,
     3,  -246,-32768,  -172,  -237,-32768,    -1,-32768,  -201,-32768,
-32768
};


#define	YYLAST		811


static const short yytable[] = {    86,
   287,    78,    79,    84,    86,    86,    86,    86,    93,    94,
   145,   105,   101,   102,   275,   245,    87,   297,   115,   287,
   287,   299,    97,   257,    88,   258,   105,   105,   105,   309,
    89,    86,    86,    86,    83,   179,    98,   143,   181,    83,
    83,    83,    83,   209,   288,   210,   141,   142,   213,   289,
   162,   118,   164,   312,   201,   175,   105,    87,   217,   135,
   136,   137,    87,    78,    79,    88,    83,    83,    83,   144,
    88,    89,   290,   291,    87,    80,    89,   157,   157,    86,
   157,   157,    88,   180,   234,    87,   235,   236,    89,   177,
   214,    86,    86,    88,    57,   188,   189,   229,   230,    89,
   313,   314,    86,   310,   119,    87,   251,   182,   183,   184,
   185,   302,   246,    88,    83,   303,   300,   195,    99,    89,
    74,    75,    76,    77,   262,   243,    83,    83,   306,   307,
    78,    79,    74,    75,    76,    77,   158,    83,   160,   161,
   316,   278,    78,    79,   151,   152,    87,   154,    87,   182,
   183,   184,   185,    90,    88,   315,    88,   227,    87,   228,
    89,   300,    89,   202,   261,    87,    88,   318,   240,   -40,
   241,   242,    89,    88,   184,   185,   319,    78,    79,    89,
    86,    86,    86,    86,   247,   248,   249,   250,   197,    87,
   178,   181,     0,     0,   240,     0,   240,    88,     0,   260,
   264,     0,   115,    89,    80,     0,    74,    75,    76,    77,
     0,   272,  -107,   269,    81,    83,    83,    83,    83,     0,
  -107,     0,   182,   183,   184,   185,  -107,    92,     0,   145,
     0,    74,    75,    76,    77,    83,   240,     0,     0,     0,
   114,   120,   122,   124,   126,   128,    83,   130,   131,   132,
   134,    80,     0,     0,   139,   240,   240,     0,     0,   146,
   149,     0,   272,   153,   269,   155,   272,     0,   269,     0,
     0,     0,   163,     0,   165,   167,     0,     0,   171,    87,
     0,    74,    75,    76,    77,   237,     0,    88,     0,     0,
     0,    78,    79,    89,     0,     0,     0,    83,   272,     0,
   269,    83,   172,     0,   173,   174,   186,   182,   183,   184,
   185,     0,   187,    74,    75,    76,    77,   190,     0,   191,
     0,     0,     0,    78,    79,     0,    74,    75,    76,    77,
     0,     0,     0,    83,     0,   200,    78,    79,   203,     0,
   204,   205,   311,     0,    74,    75,    76,    77,   133,     0,
    74,    75,    76,    77,    78,    79,    88,   212,     0,     0,
   215,     0,    89,   218,   219,   220,   221,   121,     0,   222,
     0,     0,     0,   224,   225,     0,   226,    74,    75,    76,
    77,     0,     0,   231,   232,     0,   233,    78,    79,   103,
     0,    74,    75,    76,    77,     0,     0,    80,     0,   244,
     0,     0,     0,   182,   183,   184,   185,    81,    74,    75,
    76,    77,     0,     0,   253,   268,   255,   256,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    80,   263,
     0,   265,   266,     0,     0,     0,   273,     0,     0,   277,
     0,     0,     0,     0,     0,     0,     0,     0,   268,   279,
   280,   281,   282,     0,     0,     0,   283,   284,   285,     0,
   286,    80,     0,     0,     0,   106,   111,   112,   113,     0,
     0,   123,   125,   127,     0,   129,   292,     0,     0,   293,
   294,   295,   138,   140,   296,     0,     0,     0,     0,   298,
     0,     0,   301,     0,     0,     0,     0,   159,   304,     0,
   305,     0,     0,   166,   168,   169,   170,    87,     0,    74,
    75,    76,    77,     0,     0,    88,     0,     0,     0,   308,
    87,    89,   -98,   -98,   -98,   -98,     0,     0,    88,     0,
   148,     0,   -98,   -98,    89,    87,     0,    74,    75,    76,
    77,     0,     0,    88,     0,     0,     0,    78,    79,    89,
     0,   192,   193,   194,     0,     0,     0,   196,     0,     0,
     0,     0,   198,   199,     0,     0,     0,     0,     0,     0,
     0,     0,   206,     0,     0,     0,     0,     0,     0,     0,
   211,     0,     0,     0,     0,     0,     0,     0,   237,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   223,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   252,     0,     0,     0,   254,     0,     0,     0,     0,     0,
   259,     0,     1,     0,     2,     3,     0,     0,     0,     0,
     0,     0,   267,     4,     5,     6,     7,     8,     9,    10,
    11,    12,    13,    14,    15,    16,    17,    18,     0,     0,
    19,    20,    21,    22,    23,     0,    24,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    55,    56,     0,
     0,     0,     0,     0,     0,     0,     1,     0,     0,     3,
    57,     0,     0,     0,     0,     0,    58,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,     0,     0,    19,    20,    21,    22,    23,     0,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
    54,    55,    56,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    58
};

static const short yycheck[] = {     1,
   238,    13,    14,     1,     6,     7,     8,     9,     6,     7,
    84,    13,    11,    12,   216,     1,     1,   264,    18,   257,
   258,   268,     9,   196,     9,   198,    28,    29,    30,     1,
    15,    33,    34,    35,     1,     9,     1,    35,    87,     6,
     7,     8,     9,   125,    75,   127,    33,    34,    12,    80,
    49,    18,    51,   300,    17,    57,    58,     1,   140,    28,
    29,    30,     1,    13,    14,     9,    33,    34,    35,    36,
     9,    15,    78,    79,     1,    87,    15,    44,    45,    81,
    47,    48,     9,    81,   166,     1,   168,   169,    15,    58,
    82,    93,    94,     9,    85,    93,    94,    86,    86,    15,
   302,   303,   104,    75,    89,     1,   188,    93,    94,    95,
    96,    81,    98,     9,    81,    81,    88,   104,    83,    15,
     3,     4,     5,     6,   206,    80,    93,    94,    80,    78,
    13,    14,     3,     4,     5,     6,    45,   104,    47,    48,
    75,   223,    13,    14,    39,    40,     1,    42,     1,    93,
    94,    95,    96,    92,     9,    79,     9,   156,     1,   158,
    15,    88,    15,    16,   202,     1,     9,     0,   170,    85,
   172,   173,    15,     9,    95,    96,     0,    13,    14,    15,
   182,   183,   184,   185,   182,   183,   184,   185,   107,     1,
    59,    87,    -1,    -1,   196,    -1,   198,     9,    -1,   201,
    12,    -1,   202,    15,    87,    -1,     3,     4,     5,     6,
    -1,   213,     1,   213,    97,   182,   183,   184,   185,    -1,
     9,    -1,    93,    94,    95,    96,    15,     5,    -1,    84,
    -1,     3,     4,     5,     6,   202,   238,    -1,    -1,    -1,
    18,    19,    20,    21,    22,    23,   213,    25,    26,    27,
    28,    87,    -1,    -1,    32,   257,   258,    -1,    -1,    37,
    38,    -1,   264,    41,   264,    43,   268,    -1,   268,    -1,
    -1,    -1,    50,    -1,    52,    53,    -1,    -1,    56,     1,
    -1,     3,     4,     5,     6,    82,    -1,     9,    -1,    -1,
    -1,    13,    14,    15,    -1,    -1,    -1,   264,   300,    -1,
   300,   268,    74,    -1,    76,    77,    84,    93,    94,    95,
    96,    -1,    90,     3,     4,     5,     6,    95,    -1,    97,
    -1,    -1,    -1,    13,    14,    -1,     3,     4,     5,     6,
    -1,    -1,    -1,   300,    -1,   113,    13,    14,   116,    -1,
   118,   119,     1,    -1,     3,     4,     5,     6,     1,    -1,
     3,     4,     5,     6,    13,    14,     9,   135,    -1,    -1,
   138,    -1,    15,   141,   142,   143,   144,    89,    -1,   147,
    -1,    -1,    -1,   151,   152,    -1,   154,     3,     4,     5,
     6,    -1,    -1,   161,   162,    -1,   164,    13,    14,     1,
    -1,     3,     4,     5,     6,    -1,    -1,    87,    -1,   177,
    -1,    -1,    -1,    93,    94,    95,    96,    97,     3,     4,
     5,     6,    -1,    -1,   192,    74,   194,   195,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,   207,
    -1,   209,   210,    -1,    -1,    -1,   214,    -1,    -1,   217,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    74,   227,
   228,   229,   230,    -1,    -1,    -1,   234,   235,   236,    -1,
   238,    87,    -1,    -1,    -1,    14,    15,    16,    17,    -1,
    -1,    20,    21,    22,    -1,    24,   254,    -1,    -1,   257,
   258,   259,    31,    32,   262,    -1,    -1,    -1,    -1,   267,
    -1,    -1,   270,    -1,    -1,    -1,    -1,    46,   276,    -1,
   278,    -1,    -1,    52,    53,    54,    55,     1,    -1,     3,
     4,     5,     6,    -1,    -1,     9,    -1,    -1,    -1,   297,
     1,    15,     3,     4,     5,     6,    -1,    -1,     9,    -1,
    11,    -1,    13,    14,    15,     1,    -1,     3,     4,     5,
     6,    -1,    -1,     9,    -1,    -1,    -1,    13,    14,    15,
    -1,   100,   101,   102,    -1,    -1,    -1,   106,    -1,    -1,
    -1,    -1,   111,   112,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   121,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   129,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   150,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   189,    -1,    -1,    -1,   193,    -1,    -1,    -1,    -1,    -1,
   199,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
    -1,    -1,   211,    18,    19,    20,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
    35,    36,    37,    38,    39,    -1,    41,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,    10,
    85,    -1,    -1,    -1,    -1,    -1,    91,    18,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    -1,    -1,    35,    36,    37,    38,    39,    -1,
    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
    51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
    61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
    71,    72,    73,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    91
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
#line 92 "mon_parse.y"
{ new_cmd = 1; asm_mode = 0; ;
    break;}
case 8:
#line 101 "mon_parse.y"
{ return ERR_EXPECT_END_CMD; ;
    break;}
case 20:
#line 115 "mon_parse.y"
{ return ERR_BAD_CMD; ;
    break;}
case 21:
#line 118 "mon_parse.y"
{ fprintf(mon_output, "Bank command not done yet.\n"); ;
    break;}
case 22:
#line 119 "mon_parse.y"
{ mon_jump(yyvsp[-1].a); ;
    break;}
case 23:
#line 120 "mon_parse.y"
{ fprintf(mon_output, "Display IO registers\n"); ;
    break;}
case 24:
#line 121 "mon_parse.y"
{ fprintf(mon_output, "Continue until RTS/RTI\n"); ;
    break;}
case 25:
#line 122 "mon_parse.y"
{ puts("Dump machine state."); ;
    break;}
case 26:
#line 123 "mon_parse.y"
{ puts("Undump machine state."); ;
    break;}
case 27:
#line 124 "mon_parse.y"
{ mon_instructions_step(yyvsp[-1].i); ;
    break;}
case 28:
#line 125 "mon_parse.y"
{ mon_instructions_next(yyvsp[-1].i); ;
    break;}
case 29:
#line 126 "mon_parse.y"
{ mon_stack_up(yyvsp[-1].i); ;
    break;}
case 30:
#line 127 "mon_parse.y"
{ mon_stack_down(yyvsp[-1].i); ;
    break;}
case 32:
#line 131 "mon_parse.y"
{ mon_print_registers(default_memspace); ;
    break;}
case 33:
#line 132 "mon_parse.y"
{ mon_print_registers(yyvsp[-1].i); ;
    break;}
case 35:
#line 136 "mon_parse.y"
{ mon_load_symbols(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 36:
#line 137 "mon_parse.y"
{ mon_save_symbols(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 37:
#line 138 "mon_parse.y"
{ mon_add_name_to_symbol_table(yyvsp[-2].a, yyvsp[-1].str); ;
    break;}
case 38:
#line 139 "mon_parse.y"
{ mon_remove_name_from_symbol_table(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 39:
#line 140 "mon_parse.y"
{ mon_print_symbol_table(yyvsp[-1].i); ;
    break;}
case 40:
#line 143 "mon_parse.y"
{ mon_start_assemble_mode(yyvsp[0].a, NULL); ;
    break;}
case 42:
#line 144 "mon_parse.y"
{ mon_start_assemble_mode(yyvsp[-1].a, NULL); ;
    break;}
case 43:
#line 145 "mon_parse.y"
{ mon_disassemble_lines(yyvsp[-2].a,yyvsp[-1].a); ;
    break;}
case 44:
#line 146 "mon_parse.y"
{ mon_disassemble_lines(BAD_ADDR, BAD_ADDR); ;
    break;}
case 45:
#line 149 "mon_parse.y"
{ mon_move_memory(yyvsp[-3].a, yyvsp[-2].a, yyvsp[-1].a); ;
    break;}
case 46:
#line 150 "mon_parse.y"
{ mon_compare_memory(yyvsp[-3].a, yyvsp[-2].a, yyvsp[-1].a); ;
    break;}
case 47:
#line 151 "mon_parse.y"
{ mon_fill_memory(yyvsp[-3].a, yyvsp[-2].a, yyvsp[-1].str); ;
    break;}
case 48:
#line 152 "mon_parse.y"
{ mon_hunt_memory(yyvsp[-3].a, yyvsp[-2].a, yyvsp[-1].str); ;
    break;}
case 49:
#line 153 "mon_parse.y"
{ mon_display_memory(yyvsp[-3].rt, yyvsp[-2].a, yyvsp[-1].a); ;
    break;}
case 50:
#line 154 "mon_parse.y"
{ mon_display_memory(default_radix, yyvsp[-2].a, yyvsp[-1].a); ;
    break;}
case 51:
#line 155 "mon_parse.y"
{ mon_display_memory(default_radix, BAD_ADDR, BAD_ADDR); ;
    break;}
case 52:
#line 156 "mon_parse.y"
{ mon_display_data(yyvsp[-2].a, yyvsp[-1].a, 8, 8); ;
    break;}
case 53:
#line 157 "mon_parse.y"
{ mon_display_data(BAD_ADDR, BAD_ADDR, 8, 8); ;
    break;}
case 54:
#line 158 "mon_parse.y"
{ mon_display_data(yyvsp[-2].a, yyvsp[-1].a, 24, 21); ;
    break;}
case 55:
#line 159 "mon_parse.y"
{ mon_display_data(BAD_ADDR, BAD_ADDR, 24, 21); ;
    break;}
case 56:
#line 160 "mon_parse.y"
{ mon_display_memory(0, yyvsp[-2].a, yyvsp[-1].a); ;
    break;}
case 57:
#line 163 "mon_parse.y"
{ mon_add_checkpoint(yyvsp[-2].a, yyvsp[-1].a, FALSE, FALSE, FALSE); ;
    break;}
case 58:
#line 164 "mon_parse.y"
{ 
                          temp = mon_add_checkpoint(yyvsp[-4].a, yyvsp[-3].a, FALSE, FALSE, FALSE); 
                          mon_set_checkpoint_condition(temp, yyvsp[-1].cond_node); ;
    break;}
case 59:
#line 167 "mon_parse.y"
{ mon_add_checkpoint(yyvsp[-2].a, yyvsp[-1].a, FALSE, 
                              (yyvsp[-3].i == e_load || yyvsp[-3].i == e_load_store), (yyvsp[-3].i == e_store || yyvsp[-3].i == e_load_store)); ;
    break;}
case 60:
#line 169 "mon_parse.y"
{ mon_add_checkpoint(yyvsp[-2].a, yyvsp[-1].a, TRUE, FALSE, FALSE); ;
    break;}
case 61:
#line 170 "mon_parse.y"
{ mon_print_checkpoints(); ;
    break;}
case 62:
#line 171 "mon_parse.y"
{ mon_print_checkpoints(); ;
    break;}
case 63:
#line 172 "mon_parse.y"
{ mon_print_checkpoints(); ;
    break;}
case 64:
#line 176 "mon_parse.y"
{ mon_switch_checkpoint(yyvsp[-2].action, yyvsp[-1].i); ;
    break;}
case 65:
#line 177 "mon_parse.y"
{ mon_set_ignore_count(yyvsp[-2].i, yyvsp[-1].i); ;
    break;}
case 66:
#line 178 "mon_parse.y"
{ mon_delete_checkpoint(yyvsp[-1].i); ;
    break;}
case 67:
#line 179 "mon_parse.y"
{ mon_delete_checkpoint(-1); ;
    break;}
case 68:
#line 180 "mon_parse.y"
{ mon_set_checkpoint_condition(yyvsp[-3].i, yyvsp[-1].cond_node); ;
    break;}
case 69:
#line 181 "mon_parse.y"
{ mon_set_checkpoint_command(yyvsp[-2].i, yyvsp[-1].str); ;
    break;}
case 70:
#line 184 "mon_parse.y"
{ sidefx = ((yyvsp[-1].action==e_TOGGLE)?(sidefx^1):yyvsp[-1].action); ;
    break;}
case 71:
#line 185 "mon_parse.y"
{ fprintf(mon_output, "sidefx %d\n",sidefx); ;
    break;}
case 72:
#line 186 "mon_parse.y"
{ default_radix = yyvsp[-1].rt; ;
    break;}
case 73:
#line 187 "mon_parse.y"
{ fprintf(mon_output, "Default radix is %d\n", 
                                                         default_radix); ;
    break;}
case 74:
#line 189 "mon_parse.y"
{ fprintf(mon_output,"Setting default device to %s\n", 
                                                         SPACESTRING(yyvsp[-1].i)); default_memspace = yyvsp[-1].i; ;
    break;}
case 75:
#line 191 "mon_parse.y"
{ fprintf(mon_output, "Quit.\n"); exit(-1); exit(0); ;
    break;}
case 76:
#line 192 "mon_parse.y"
{ exit_mon = 1; YYACCEPT; ;
    break;}
case 77:
#line 195 "mon_parse.y"
{ mon_execute_disk_command(yyvsp[-1].str); ;
    break;}
case 78:
#line 196 "mon_parse.y"
{ fprintf(mon_output, "\t%d\n",yyvsp[-1].i); ;
    break;}
case 79:
#line 197 "mon_parse.y"
{ mon_print_help(NULL); ;
    break;}
case 80:
#line 198 "mon_parse.y"
{ mon_print_help(yyvsp[-1].str); ;
    break;}
case 81:
#line 199 "mon_parse.y"
{ printf("SYSTEM COMMAND: %s\n",yyvsp[-1].str); ;
    break;}
case 82:
#line 200 "mon_parse.y"
{ mon_print_convert(yyvsp[-1].i); ;
    break;}
case 83:
#line 201 "mon_parse.y"
{ mon_change_dir(yyvsp[-1].str); ;
    break;}
case 84:
#line 204 "mon_parse.y"
{ mon_load_file(yyvsp[-2].str,yyvsp[-1].a); ;
    break;}
case 85:
#line 205 "mon_parse.y"
{ mon_save_file(yyvsp[-3].str,yyvsp[-2].a,yyvsp[-1].a); ;
    break;}
case 86:
#line 206 "mon_parse.y"
{ mon_verify_file(yyvsp[-2].str,yyvsp[-1].a); ;
    break;}
case 87:
#line 207 "mon_parse.y"
{ mon_block_cmd(0,yyvsp[-2].i,yyvsp[-1].i,yyvsp[0].a); ;
    break;}
case 88:
#line 208 "mon_parse.y"
{ mon_block_cmd(1,yyvsp[-2].i,yyvsp[-1].i,yyvsp[0].a); ;
    break;}
case 89:
#line 211 "mon_parse.y"
{ mon_record_commands(yyvsp[-1].str); ;
    break;}
case 90:
#line 212 "mon_parse.y"
{ mon_end_recording(); ;
    break;}
case 91:
#line 213 "mon_parse.y"
{ playback=TRUE; playback_name = yyvsp[-1].str; ;
    break;}
case 92:
#line 216 "mon_parse.y"
{ mon_fill_memory(yyvsp[-2].a, BAD_ADDR, yyvsp[-1].str); ;
    break;}
case 93:
#line 217 "mon_parse.y"
{ printf("Not yet.\n"); ;
    break;}
case 94:
#line 220 "mon_parse.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 96:
#line 224 "mon_parse.y"
{ return ERR_EXPECT_FILENAME; ;
    break;}
case 97:
#line 227 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 98:
#line 228 "mon_parse.y"
{ yyval.i = e_load_store; ;
    break;}
case 99:
#line 231 "mon_parse.y"
{ yyval.i = new_reg(default_memspace, yyvsp[0].reg); ;
    break;}
case 100:
#line 232 "mon_parse.y"
{ yyval.i = new_reg(yyvsp[-1].i, yyvsp[0].reg); ;
    break;}
case 103:
#line 239 "mon_parse.y"
{ mon_set_reg_val(reg_memspace(yyvsp[-2].i), reg_regid(yyvsp[-2].i), yyvsp[0].i); ;
    break;}
case 104:
#line 242 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 105:
#line 243 "mon_parse.y"
{ yyval.i = -1; ;
    break;}
case 106:
#line 246 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 107:
#line 247 "mon_parse.y"
{ return ERR_EXPECT_BRKNUM; ;
    break;}
case 108:
#line 250 "mon_parse.y"
{ yyval.a = yyvsp[0].a; ;
    break;}
case 109:
#line 251 "mon_parse.y"
{ yyval.a = BAD_ADDR; ;
    break;}
case 110:
#line 254 "mon_parse.y"
{ yyval.a = new_addr(e_default_space,yyvsp[0].i); if (opt_asm) new_cmd = asm_mode = 1; ;
    break;}
case 111:
#line 255 "mon_parse.y"
{ yyval.a = new_addr(yyvsp[-1].i,yyvsp[0].i); if (opt_asm) new_cmd = asm_mode = 1; ;
    break;}
case 112:
#line 258 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 113:
#line 259 "mon_parse.y"
{ yyval.i = e_default_space; ;
    break;}
case 114:
#line 262 "mon_parse.y"
{ yyval.i = e_comp_space; ;
    break;}
case 115:
#line 263 "mon_parse.y"
{ yyval.i = e_disk_space; ;
    break;}
case 116:
#line 266 "mon_parse.y"
{ yyval.i = yyvsp[0].i; if (!CHECK_ADDR(yyvsp[0].i)) return ERR_ADDR_TOO_BIG; ;
    break;}
case 117:
#line 269 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 118:
#line 271 "mon_parse.y"
{ yyval.i = yyvsp[-2].i + yyvsp[0].i; ;
    break;}
case 119:
#line 272 "mon_parse.y"
{ yyval.i = yyvsp[-2].i - yyvsp[0].i; ;
    break;}
case 120:
#line 273 "mon_parse.y"
{ yyval.i = yyvsp[-2].i * yyvsp[0].i; ;
    break;}
case 121:
#line 274 "mon_parse.y"
{ yyval.i = (yyvsp[0].i) ? (yyvsp[-2].i / yyvsp[0].i) : 1; ;
    break;}
case 122:
#line 275 "mon_parse.y"
{ yyval.i = yyvsp[-1].i; ;
    break;}
case 123:
#line 276 "mon_parse.y"
{ return ERR_MISSING_CLOSE_PAREN; ;
    break;}
case 124:
#line 277 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 125:
#line 280 "mon_parse.y"
{ yyval.cond_node = new_cond; yyval.cond_node->is_parenthized = FALSE;
                                            yyval.cond_node->child1 = yyvsp[-2].cond_node; yyval.cond_node->child2 = yyvsp[0].cond_node; yyval.cond_node->operation = yyvsp[-1].cond_op; ;
    break;}
case 126:
#line 282 "mon_parse.y"
{ return ERR_INCOMPLETE_COMPARE_OP; ;
    break;}
case 127:
#line 283 "mon_parse.y"
{ yyval.cond_node = yyvsp[-1].cond_node; yyval.cond_node->is_parenthized = TRUE; ;
    break;}
case 128:
#line 284 "mon_parse.y"
{ return ERR_MISSING_CLOSE_PAREN; ;
    break;}
case 129:
#line 285 "mon_parse.y"
{ yyval.cond_node = yyvsp[0].cond_node; ;
    break;}
case 130:
#line 288 "mon_parse.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV; yyval.cond_node->is_parenthized = FALSE;
                            yyval.cond_node->reg_num = yyvsp[0].i; yyval.cond_node->is_reg = TRUE; ;
    break;}
case 131:
#line 290 "mon_parse.y"
{ yyval.cond_node = new_cond; yyval.cond_node->operation = e_INV; yyval.cond_node->is_parenthized = FALSE;
                            yyval.cond_node->value = yyvsp[0].i; yyval.cond_node->is_reg = FALSE; ;
    break;}
case 134:
#line 298 "mon_parse.y"
{ mon_add_number_to_buffer(yyvsp[0].i); ;
    break;}
case 135:
#line 299 "mon_parse.y"
{ mon_add_string_to_buffer(yyvsp[0].str); ;
    break;}
case 136:
#line 302 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 137:
#line 303 "mon_parse.y"
{ yyval.i = mon_get_reg_val(reg_memspace(yyvsp[0].i), reg_regid(yyvsp[0].i)); ;
    break;}
case 138:
#line 306 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 139:
#line 307 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 140:
#line 308 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 141:
#line 309 "mon_parse.y"
{ yyval.i = yyvsp[0].i; ;
    break;}
case 144:
#line 316 "mon_parse.y"
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
case 146:
#line 327 "mon_parse.y"
{ asm_mode = 0; ;
    break;}
case 147:
#line 330 "mon_parse.y"
{ yyval.i = join_ints(IMMEDIATE,yyvsp[0].i); ;
    break;}
case 148:
#line 331 "mon_parse.y"
{ if (yyvsp[0].i < 0x100)
                              yyval.i = join_ints(ZERO_PAGE,yyvsp[0].i);
                           else
                              yyval.i = join_ints(ABSOLUTE,yyvsp[0].i);
                         ;
    break;}
case 149:
#line 336 "mon_parse.y"
{ if (yyvsp[-2].i < 0x100)
                                           yyval.i = join_ints(ZERO_PAGE_X,yyvsp[-2].i);
                                        else
                                           yyval.i = join_ints(ABSOLUTE_X,yyvsp[-2].i);
                                      ;
    break;}
case 150:
#line 341 "mon_parse.y"
{ if (yyvsp[-2].i < 0x100)
                                           yyval.i = join_ints(ZERO_PAGE_Y,yyvsp[-2].i);
                                        else
                                           yyval.i = join_ints(ABSOLUTE_Y,yyvsp[-2].i);
                                      ;
    break;}
case 151:
#line 346 "mon_parse.y"
{ yyval.i = join_ints(ABS_INDIRECT,yyvsp[-1].i); ;
    break;}
case 152:
#line 347 "mon_parse.y"
{ yyval.i = join_ints(INDIRECT_X,yyvsp[-3].i); ;
    break;}
case 153:
#line 348 "mon_parse.y"
{ yyval.i = join_ints(INDIRECT_Y,yyvsp[-3].i); ;
    break;}
case 154:
#line 349 "mon_parse.y"
{ yyval.i = join_ints(IMPLIED,0); ;
    break;}
case 155:
#line 350 "mon_parse.y"
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
#line 354 "mon_parse.y"
 

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
       fprintf(mon_output, "ERROR: ");
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
           case ERR_ADDR_TOO_BIG:
               fprintf(mon_output, "Address too large\n  %s\n", input);
               break;
           default:
               fprintf(mon_output, "Illegal input:\n  %s\n", input);
       }
       for (i=0;i<last_len;i++)   fprintf(mon_output, " ");
       fprintf(mon_output, "  ^\n");
       new_cmd = 1;
       asm_mode = 0;
   }
   free_buffer();
}

static int yyerror(char *s)
{
   YYABORT;
   fprintf(stderr, "ERR:%s\n",s);
   return 0;
}

