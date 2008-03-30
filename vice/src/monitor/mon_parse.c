/* A Bison parser, made from mon_parse.y, by GNU bison 1.75.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON	1

/* Pure parsers.  */
#define YYPURE	0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     H_NUMBER = 258,
     D_NUMBER = 259,
     O_NUMBER = 260,
     B_NUMBER = 261,
     CONVERT_OP = 262,
     B_DATA = 263,
     D_NUMBER_GUESS = 264,
     O_NUMBER_GUESS = 265,
     B_NUMBER_GUESS = 266,
     TRAIL = 267,
     BAD_CMD = 268,
     MEM_OP = 269,
     IF = 270,
     MEM_COMP = 271,
     MEM_DISK8 = 272,
     MEM_DISK9 = 273,
     MEM_DISK10 = 274,
     MEM_DISK11 = 275,
     CMD_SEP = 276,
     REG_ASGN_SEP = 277,
     EQUALS = 278,
     CMD_SIDEFX = 279,
     CMD_RETURN = 280,
     CMD_BLOCK_READ = 281,
     CMD_BLOCK_WRITE = 282,
     CMD_UP = 283,
     CMD_DOWN = 284,
     CMD_LOAD = 285,
     CMD_SAVE = 286,
     CMD_VERIFY = 287,
     CMD_IGNORE = 288,
     CMD_HUNT = 289,
     CMD_FILL = 290,
     CMD_MOVE = 291,
     CMD_GOTO = 292,
     CMD_REGISTERS = 293,
     CMD_READSPACE = 294,
     CMD_WRITESPACE = 295,
     CMD_RADIX = 296,
     CMD_MEM_DISPLAY = 297,
     CMD_BREAK = 298,
     CMD_TRACE = 299,
     CMD_IO = 300,
     CMD_BRMON = 301,
     CMD_COMPARE = 302,
     CMD_DUMP = 303,
     CMD_UNDUMP = 304,
     CMD_EXIT = 305,
     CMD_DELETE = 306,
     CMD_CONDITION = 307,
     CMD_COMMAND = 308,
     CMD_ASSEMBLE = 309,
     CMD_DISASSEMBLE = 310,
     CMD_NEXT = 311,
     CMD_STEP = 312,
     CMD_PRINT = 313,
     CMD_DEVICE = 314,
     CMD_HELP = 315,
     CMD_WATCH = 316,
     CMD_DISK = 317,
     CMD_SYSTEM = 318,
     CMD_QUIT = 319,
     CMD_CHDIR = 320,
     CMD_BANK = 321,
     CMD_LOAD_LABELS = 322,
     CMD_SAVE_LABELS = 323,
     CMD_ADD_LABEL = 324,
     CMD_DEL_LABEL = 325,
     CMD_SHOW_LABELS = 326,
     CMD_RECORD = 327,
     CMD_STOP = 328,
     CMD_PLAYBACK = 329,
     CMD_CHAR_DISPLAY = 330,
     CMD_SPRITE_DISPLAY = 331,
     CMD_TEXT_DISPLAY = 332,
     CMD_ENTER_DATA = 333,
     CMD_ENTER_BIN_DATA = 334,
     CMD_KEYBUF = 335,
     CMD_BLOAD = 336,
     CMD_BSAVE = 337,
     CMD_SCREEN = 338,
     CMD_UNTIL = 339,
     CMD_CPU = 340,
     L_PAREN = 341,
     R_PAREN = 342,
     ARG_IMMEDIATE = 343,
     REG_A = 344,
     REG_X = 345,
     REG_Y = 346,
     COMMA = 347,
     INST_SEP = 348,
     REG_B = 349,
     REG_C = 350,
     REG_D = 351,
     REG_E = 352,
     REG_H = 353,
     REG_L = 354,
     REG_AF = 355,
     REG_BC = 356,
     REG_DE = 357,
     REG_HL = 358,
     REG_IX = 359,
     REG_IY = 360,
     REG_SP = 361,
     REG_IXH = 362,
     REG_IXL = 363,
     REG_IYH = 364,
     REG_IYL = 365,
     CPUTYPE_6502 = 366,
     CPUTYPE_Z80 = 367,
     STRING = 368,
     FILENAME = 369,
     R_O_L = 370,
     OPCODE = 371,
     LABEL = 372,
     BANKNAME = 373,
     CPUTYPE = 374,
     REGISTER = 375,
     COMPARE_OP = 376,
     RADIX_TYPE = 377,
     INPUT_SPEC = 378,
     CMD_CHECKPT_ON = 379,
     CMD_CHECKPT_OFF = 380,
     TOGGLE = 381
   };
#endif
#define H_NUMBER 258
#define D_NUMBER 259
#define O_NUMBER 260
#define B_NUMBER 261
#define CONVERT_OP 262
#define B_DATA 263
#define D_NUMBER_GUESS 264
#define O_NUMBER_GUESS 265
#define B_NUMBER_GUESS 266
#define TRAIL 267
#define BAD_CMD 268
#define MEM_OP 269
#define IF 270
#define MEM_COMP 271
#define MEM_DISK8 272
#define MEM_DISK9 273
#define MEM_DISK10 274
#define MEM_DISK11 275
#define CMD_SEP 276
#define REG_ASGN_SEP 277
#define EQUALS 278
#define CMD_SIDEFX 279
#define CMD_RETURN 280
#define CMD_BLOCK_READ 281
#define CMD_BLOCK_WRITE 282
#define CMD_UP 283
#define CMD_DOWN 284
#define CMD_LOAD 285
#define CMD_SAVE 286
#define CMD_VERIFY 287
#define CMD_IGNORE 288
#define CMD_HUNT 289
#define CMD_FILL 290
#define CMD_MOVE 291
#define CMD_GOTO 292
#define CMD_REGISTERS 293
#define CMD_READSPACE 294
#define CMD_WRITESPACE 295
#define CMD_RADIX 296
#define CMD_MEM_DISPLAY 297
#define CMD_BREAK 298
#define CMD_TRACE 299
#define CMD_IO 300
#define CMD_BRMON 301
#define CMD_COMPARE 302
#define CMD_DUMP 303
#define CMD_UNDUMP 304
#define CMD_EXIT 305
#define CMD_DELETE 306
#define CMD_CONDITION 307
#define CMD_COMMAND 308
#define CMD_ASSEMBLE 309
#define CMD_DISASSEMBLE 310
#define CMD_NEXT 311
#define CMD_STEP 312
#define CMD_PRINT 313
#define CMD_DEVICE 314
#define CMD_HELP 315
#define CMD_WATCH 316
#define CMD_DISK 317
#define CMD_SYSTEM 318
#define CMD_QUIT 319
#define CMD_CHDIR 320
#define CMD_BANK 321
#define CMD_LOAD_LABELS 322
#define CMD_SAVE_LABELS 323
#define CMD_ADD_LABEL 324
#define CMD_DEL_LABEL 325
#define CMD_SHOW_LABELS 326
#define CMD_RECORD 327
#define CMD_STOP 328
#define CMD_PLAYBACK 329
#define CMD_CHAR_DISPLAY 330
#define CMD_SPRITE_DISPLAY 331
#define CMD_TEXT_DISPLAY 332
#define CMD_ENTER_DATA 333
#define CMD_ENTER_BIN_DATA 334
#define CMD_KEYBUF 335
#define CMD_BLOAD 336
#define CMD_BSAVE 337
#define CMD_SCREEN 338
#define CMD_UNTIL 339
#define CMD_CPU 340
#define L_PAREN 341
#define R_PAREN 342
#define ARG_IMMEDIATE 343
#define REG_A 344
#define REG_X 345
#define REG_Y 346
#define COMMA 347
#define INST_SEP 348
#define REG_B 349
#define REG_C 350
#define REG_D 351
#define REG_E 352
#define REG_H 353
#define REG_L 354
#define REG_AF 355
#define REG_BC 356
#define REG_DE 357
#define REG_HL 358
#define REG_IX 359
#define REG_IY 360
#define REG_SP 361
#define REG_IXH 362
#define REG_IXL 363
#define REG_IYH 364
#define REG_IYL 365
#define CPUTYPE_6502 366
#define CPUTYPE_Z80 367
#define STRING 368
#define FILENAME 369
#define R_O_L 370
#define OPCODE 371
#define LABEL 372
#define BANKNAME 373
#define CPUTYPE 374
#define REGISTER 375
#define COMPARE_OP 376
#define RADIX_TYPE 377
#define INPUT_SPEC 378
#define CMD_CHECKPT_ON 379
#define CMD_CHECKPT_OFF 380
#define TOGGLE 381




/* Copy the first part of user declarations.  */
#line 29 "mon_parse.y"

#include "vice.h"

#ifdef __GNUC__
#undef alloca
#define        alloca(n)       __builtin_alloca (n)
#else
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#else  /* Not HAVE_ALLOCA_H.  */
#if !defined(_AIX) && !defined(WINCE)
#ifndef _MSC_VER
extern char *alloca();
#else
#define alloca(n)   _alloca(n)
#endif  /* MSVC */
#endif /* Not AIX and not WINCE.  */
#endif /* HAVE_ALLOCA_H.  */
#endif /* GCC.  */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "asm.h"
#include "console.h"
#include "lib.h"
#include "machine.h"
#include "mon_breakpoint.h"
#include "mon_command.h"
#include "mon_disassemble.h"
#include "mon_drive.h"
#include "mon_file.h"
#include "mon_memory.h"
#include "mon_util.h"
#include "montypes.h"
#include "types.h"
#include "uimon.h"


#define join_ints(x,y) (LO16_TO_HI16(x)|y)
#define separate_int1(x) (HI16_TO_LO16(x))
#define separate_int2(x) (LO16(x))

static int yyerror(char *s);
static int temp;
static int resolve_datatype(unsigned guess_type, char *num);

#ifdef __IBMC__
static void __yy_memcpy (char *to, char *from, int count);
#endif

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
#define ERR_EXPECT_STRING 12
#define ERR_UNDEFINED_LABEL 13

#define BAD_ADDR (new_addr(e_invalid_space, 0))
#define CHECK_ADDR(x) ((x) == LO16(x))



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#ifndef YYSTYPE
#line 108 "mon_parse.y"
typedef union {
    MON_ADDR a;
    int i;
    REG_ID reg;
    CONDITIONAL cond_op;
    cond_node_t *cond_node;
    RADIXTYPE rt;
    ACTION action;
    char *str;
} yystype;
/* Line 193 of /usr/share/bison/yacc.c.  */
#line 415 "mon_parse.c"
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif

#ifndef YYLTYPE
typedef struct yyltype
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} yyltype;
# define YYLTYPE yyltype
# define YYLTYPE_IS_TRIVIAL 1
#endif

/* Copy the second part of user declarations.  */


/* Line 213 of /usr/share/bison/yacc.c.  */
#line 436 "mon_parse.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];	\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  224
#define YYLAST   1119

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  133
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  46
/* YYNRULES -- Number of rules. */
#define YYNRULES  208
/* YYNRULES -- Number of states. */
#define YYNSTATES  404

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   381

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     131,   132,   129,   127,     2,   128,     2,   130,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    15,    17,    19,
      21,    23,    25,    27,    29,    31,    33,    35,    37,    39,
      41,    43,    45,    50,    54,    57,    61,    64,    68,    72,
      76,    80,    84,    88,    91,    93,    96,   100,   104,   109,
     114,   119,   124,   128,   129,   135,   139,   144,   147,   153,
     159,   165,   171,   177,   182,   185,   190,   193,   198,   201,
     206,   209,   214,   219,   226,   232,   237,   240,   243,   246,
     249,   253,   257,   262,   266,   269,   275,   280,   285,   289,
     292,   296,   299,   303,   306,   309,   313,   317,   320,   324,
     328,   332,   336,   340,   346,   352,   359,   366,   372,   378,
     384,   388,   391,   395,   400,   403,   405,   407,   408,   410,
     412,   414,   415,   417,   420,   424,   426,   430,   432,   433,
     435,   437,   439,   440,   442,   445,   447,   449,   450,   452,
     454,   456,   458,   460,   462,   464,   468,   472,   476,   480,
     484,   488,   490,   494,   498,   502,   506,   508,   510,   512,
     515,   517,   519,   521,   523,   525,   527,   529,   531,   533,
     535,   537,   539,   541,   543,   545,   547,   549,   553,   557,
     560,   563,   565,   567,   570,   572,   576,   580,   584,   590,
     596,   600,   604,   608,   612,   616,   620,   626,   632,   638,
     644,   645,   647,   649,   651,   653,   655,   657,   659,   661,
     663,   665,   667,   669,   671,   673,   675,   677,   679
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     134,     0,    -1,   135,    -1,   176,    12,    -1,    12,    -1,
     137,    -1,   135,   137,    -1,    21,    -1,    12,    -1,     1,
      -1,   138,    -1,   140,    -1,   143,    -1,   141,    -1,   144,
      -1,   145,    -1,   146,    -1,   147,    -1,   148,    -1,   149,
      -1,   150,    -1,    13,    -1,    66,   162,   152,   136,    -1,
      37,   161,   136,    -1,    45,   136,    -1,    85,   119,   136,
      -1,    25,   136,    -1,    48,   153,   136,    -1,    49,   153,
     136,    -1,    57,   158,   136,    -1,    56,   158,   136,    -1,
      28,   158,   136,    -1,    29,   158,   136,    -1,    83,   136,
      -1,   139,    -1,    38,   136,    -1,    38,   163,   136,    -1,
      38,   156,   136,    -1,    67,   162,   153,   136,    -1,    68,
     162,   153,   136,    -1,    69,   161,   117,   136,    -1,    70,
     162,   117,   136,    -1,    71,   162,   136,    -1,    -1,    54,
     161,   142,   177,   136,    -1,    54,   161,   136,    -1,    55,
     161,   160,   136,    -1,    55,   136,    -1,    36,   161,   161,
     161,   136,    -1,    47,   161,   161,   161,   136,    -1,    35,
     161,   161,   169,   136,    -1,    34,   161,   161,   169,   136,
      -1,    42,   122,   161,   160,   136,    -1,    42,   161,   160,
     136,    -1,    42,   136,    -1,    75,   161,   160,   136,    -1,
      75,   136,    -1,    76,   161,   160,   136,    -1,    76,   136,
      -1,    77,   161,   160,   136,    -1,    77,   136,    -1,    43,
     161,   160,   136,    -1,    84,   161,   160,   136,    -1,    43,
     161,   160,    15,   167,   136,    -1,    61,   154,   161,   160,
     136,    -1,    44,   161,   160,   136,    -1,    43,   136,    -1,
      84,   136,    -1,    44,   136,    -1,    61,   136,    -1,   124,
     159,   136,    -1,   125,   159,   136,    -1,    33,   159,   158,
     136,    -1,    51,   159,   136,    -1,    51,   136,    -1,    52,
     159,    15,   167,   136,    -1,    53,   159,   113,   136,    -1,
      53,   159,     1,   136,    -1,    24,   126,   136,    -1,    24,
     136,    -1,    41,   122,   136,    -1,    41,   136,    -1,    59,
     163,   136,    -1,    64,   136,    -1,    50,   136,    -1,    62,
     151,   136,    -1,    58,   166,   136,    -1,    60,   136,    -1,
      60,   151,   136,    -1,    63,   151,   136,    -1,     7,   166,
     136,    -1,    65,   151,   136,    -1,    80,   151,   136,    -1,
      30,   153,   166,   160,   136,    -1,    81,   153,   166,   160,
     136,    -1,    31,   153,   166,   161,   161,   136,    -1,    82,
     153,   166,   161,   161,   136,    -1,    32,   153,   166,   161,
     136,    -1,    26,   166,   166,   160,   136,    -1,    27,   166,
     166,   161,   136,    -1,    72,   153,   136,    -1,    73,   136,
      -1,    74,   153,   136,    -1,    78,   161,   169,   136,    -1,
      79,   136,    -1,   115,    -1,   118,    -1,    -1,   114,    -1,
       1,    -1,    14,    -1,    -1,   120,    -1,   163,   120,    -1,
     156,    22,   157,    -1,   157,    -1,   155,    23,   174,    -1,
     166,    -1,    -1,   172,    -1,     1,    -1,   161,    -1,    -1,
     164,    -1,   163,   164,    -1,   117,    -1,   163,    -1,    -1,
      16,    -1,    17,    -1,    18,    -1,    19,    -1,    20,    -1,
     165,    -1,   174,    -1,   166,   127,   166,    -1,   166,   128,
     166,    -1,   166,   129,   166,    -1,   166,   130,   166,    -1,
     131,   166,   132,    -1,   131,   166,     1,    -1,   171,    -1,
     167,   121,   167,    -1,   167,   121,     1,    -1,    86,   167,
      87,    -1,    86,   167,     1,    -1,   168,    -1,   155,    -1,
     174,    -1,   169,   170,    -1,   170,    -1,   174,    -1,   113,
      -1,   174,    -1,   155,    -1,     4,    -1,    11,    -1,    10,
      -1,     9,    -1,    11,    -1,    10,    -1,     9,    -1,     3,
      -1,     4,    -1,     5,    -1,     6,    -1,   173,    -1,   175,
      93,   176,    -1,   176,    93,   176,    -1,   176,    93,    -1,
     116,   178,    -1,   176,    -1,   175,    -1,    88,   174,    -1,
     174,    -1,   174,    92,    90,    -1,   174,    92,    91,    -1,
      86,   174,    87,    -1,    86,   174,    92,    90,    87,    -1,
      86,   174,    87,    92,    91,    -1,    86,   101,    87,    -1,
      86,   102,    87,    -1,    86,   103,    87,    -1,    86,   104,
      87,    -1,    86,   105,    87,    -1,    86,   106,    87,    -1,
      86,   174,    87,    92,    89,    -1,    86,   174,    87,    92,
     103,    -1,    86,   174,    87,    92,   104,    -1,    86,   174,
      87,    92,   105,    -1,    -1,    89,    -1,    94,    -1,    95,
      -1,    96,    -1,    97,    -1,    98,    -1,   107,    -1,   109,
      -1,    99,    -1,   108,    -1,   110,    -1,   100,    -1,   101,
      -1,   102,    -1,   103,    -1,   104,    -1,   105,    -1,   106,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   163,   163,   164,   165,   168,   169,   172,   173,   174,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   191,   193,   195,   197,   199,   201,   203,   205,
     207,   209,   211,   213,   215,   218,   220,   222,   225,   230,
     232,   234,   236,   241,   240,   242,   244,   246,   250,   252,
     254,   256,   258,   260,   262,   264,   266,   268,   270,   272,
     274,   278,   283,   288,   294,   300,   305,   307,   309,   311,
     316,   318,   320,   322,   324,   326,   328,   330,   334,   336,
     341,   343,   361,   366,   368,   372,   374,   376,   378,   380,
     382,   384,   386,   390,   392,   394,   396,   398,   400,   402,
     406,   408,   410,   414,   416,   420,   423,   424,   427,   428,
     431,   432,   435,   436,   439,   440,   443,   446,   447,   450,
     451,   454,   455,   458,   460,   462,   470,   471,   474,   475,
     476,   477,   478,   485,   487,   489,   490,   491,   492,   493,
     494,   495,   498,   503,   505,   507,   509,   513,   519,   527,
     528,   531,   532,   535,   536,   539,   540,   541,   542,   545,
     546,   547,   550,   551,   552,   553,   554,   557,   558,   559,
     562,   572,   573,   576,   580,   585,   590,   595,   597,   599,
     601,   602,   603,   604,   605,   606,   607,   609,   611,   613,
     615,   616,   617,   618,   619,   620,   621,   622,   623,   624,
     625,   626,   627,   628,   629,   630,   631,   632,   633
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "H_NUMBER", "D_NUMBER", "O_NUMBER", 
  "B_NUMBER", "CONVERT_OP", "B_DATA", "D_NUMBER_GUESS", "O_NUMBER_GUESS", 
  "B_NUMBER_GUESS", "TRAIL", "BAD_CMD", "MEM_OP", "IF", "MEM_COMP", 
  "MEM_DISK8", "MEM_DISK9", "MEM_DISK10", "MEM_DISK11", "CMD_SEP", 
  "REG_ASGN_SEP", "EQUALS", "CMD_SIDEFX", "CMD_RETURN", "CMD_BLOCK_READ", 
  "CMD_BLOCK_WRITE", "CMD_UP", "CMD_DOWN", "CMD_LOAD", "CMD_SAVE", 
  "CMD_VERIFY", "CMD_IGNORE", "CMD_HUNT", "CMD_FILL", "CMD_MOVE", 
  "CMD_GOTO", "CMD_REGISTERS", "CMD_READSPACE", "CMD_WRITESPACE", 
  "CMD_RADIX", "CMD_MEM_DISPLAY", "CMD_BREAK", "CMD_TRACE", "CMD_IO", 
  "CMD_BRMON", "CMD_COMPARE", "CMD_DUMP", "CMD_UNDUMP", "CMD_EXIT", 
  "CMD_DELETE", "CMD_CONDITION", "CMD_COMMAND", "CMD_ASSEMBLE", 
  "CMD_DISASSEMBLE", "CMD_NEXT", "CMD_STEP", "CMD_PRINT", "CMD_DEVICE", 
  "CMD_HELP", "CMD_WATCH", "CMD_DISK", "CMD_SYSTEM", "CMD_QUIT", 
  "CMD_CHDIR", "CMD_BANK", "CMD_LOAD_LABELS", "CMD_SAVE_LABELS", 
  "CMD_ADD_LABEL", "CMD_DEL_LABEL", "CMD_SHOW_LABELS", "CMD_RECORD", 
  "CMD_STOP", "CMD_PLAYBACK", "CMD_CHAR_DISPLAY", "CMD_SPRITE_DISPLAY", 
  "CMD_TEXT_DISPLAY", "CMD_ENTER_DATA", "CMD_ENTER_BIN_DATA", 
  "CMD_KEYBUF", "CMD_BLOAD", "CMD_BSAVE", "CMD_SCREEN", "CMD_UNTIL", 
  "CMD_CPU", "L_PAREN", "R_PAREN", "ARG_IMMEDIATE", "REG_A", "REG_X", 
  "REG_Y", "COMMA", "INST_SEP", "REG_B", "REG_C", "REG_D", "REG_E", 
  "REG_H", "REG_L", "REG_AF", "REG_BC", "REG_DE", "REG_HL", "REG_IX", 
  "REG_IY", "REG_SP", "REG_IXH", "REG_IXL", "REG_IYH", "REG_IYL", 
  "CPUTYPE_6502", "CPUTYPE_Z80", "STRING", "FILENAME", "R_O_L", "OPCODE", 
  "LABEL", "BANKNAME", "CPUTYPE", "REGISTER", "COMPARE_OP", "RADIX_TYPE", 
  "INPUT_SPEC", "CMD_CHECKPT_ON", "CMD_CHECKPT_OFF", "TOGGLE", "'+'", 
  "'-'", "'*'", "'/'", "'('", "')'", "$accept", "top_level", 
  "command_list", "end_cmd", "command", "machine_state_rules", 
  "register_mod", "symbol_table_rules", "asm_rules", "@1", "memory_rules", 
  "checkpoint_rules", "checkpoint_control_rules", "monitor_state_rules", 
  "monitor_misc_rules", "disk_rules", "cmd_file_rules", 
  "data_entry_rules", "rest_of_line", "opt_bankname", "filename", 
  "opt_mem_op", "register", "reg_list", "reg_asgn", "opt_count", 
  "breakpt_num", "opt_address", "address", "opt_memspace", "memspace", 
  "memloc", "memaddr", "expression", "cond_expr", "compare_operand", 
  "data_list", "data_element", "value", "d_number", "guess_default", 
  "number", "assembly_instr_list", "assembly_instruction", 
  "post_assemble", "asm_operand_mode", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,    43,    45,    42,
      47,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   133,   134,   134,   134,   135,   135,   136,   136,   136,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   139,   139,   139,   140,   140,
     140,   140,   140,   142,   141,   141,   141,   141,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     145,   145,   145,   145,   145,   145,   145,   145,   146,   146,
     146,   146,   146,   146,   146,   147,   147,   147,   147,   147,
     147,   147,   147,   148,   148,   148,   148,   148,   148,   148,
     149,   149,   149,   150,   150,   151,   152,   152,   153,   153,
     154,   154,   155,   155,   156,   156,   157,   158,   158,   159,
     159,   160,   160,   161,   161,   161,   162,   162,   163,   163,
     163,   163,   163,   164,   165,   166,   166,   166,   166,   166,
     166,   166,   167,   167,   167,   167,   167,   168,   168,   169,
     169,   170,   170,   171,   171,   172,   172,   172,   172,   173,
     173,   173,   174,   174,   174,   174,   174,   175,   175,   175,
     176,   177,   177,   178,   178,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     2,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     3,     2,     3,     2,     3,     3,     3,
       3,     3,     3,     2,     1,     2,     3,     3,     4,     4,
       4,     4,     3,     0,     5,     3,     4,     2,     5,     5,
       5,     5,     5,     4,     2,     4,     2,     4,     2,     4,
       2,     4,     4,     6,     5,     4,     2,     2,     2,     2,
       3,     3,     4,     3,     2,     5,     4,     4,     3,     2,
       3,     2,     3,     2,     2,     3,     3,     2,     3,     3,
       3,     3,     3,     5,     5,     6,     6,     5,     5,     5,
       3,     2,     3,     4,     2,     1,     1,     0,     1,     1,
       1,     0,     1,     2,     3,     1,     3,     1,     0,     1,
       1,     1,     0,     1,     2,     1,     1,     0,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     1,     3,     3,     3,     3,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     2,
       2,     1,     1,     2,     1,     3,     3,     3,     5,     5,
       3,     3,     3,     3,     3,     3,     5,     5,     5,     5,
       0,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,     4,    21,     0,     0,     0,     0,   118,   118,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   118,   118,     0,     0,     0,     0,     0,
       0,     0,     0,   127,   127,   127,     0,   127,   127,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   190,     0,     0,     0,     2,     5,    10,
      34,    11,    13,    12,    14,    15,    16,    17,    18,    19,
      20,     0,   162,   163,   164,   165,   161,   160,   159,   128,
     129,   130,   131,   132,   112,     0,   154,     0,     0,   141,
     166,   153,     9,     8,     7,     0,    79,    26,     0,     0,
       0,   117,     0,   109,   108,     0,     0,     0,   120,   155,
     158,   157,   156,   118,   119,   125,     0,     0,   123,   133,
     134,     0,     0,     0,    35,     0,     0,   115,     0,     0,
      81,     0,    54,   122,    66,   122,    68,   122,    24,     0,
       0,     0,    84,     9,    74,     0,     0,     0,     0,    47,
     122,     0,     0,     0,     0,   105,    87,     0,   110,    69,
       0,     0,     0,    83,     0,   107,   126,     0,     0,     0,
       0,     0,     0,   101,     0,    56,   122,    58,   122,    60,
     122,     0,   104,     0,     0,     0,    33,    67,   122,     0,
       0,     0,   191,   192,   193,   194,   195,   196,   199,   202,
     203,   204,   205,   206,   207,   208,   197,   200,   198,   201,
     174,   170,     0,     0,     1,     6,     3,     0,   113,     0,
       0,     0,     0,    90,    78,   122,     0,    31,    32,   122,
       0,     0,     0,     0,   124,     0,     0,    23,     0,     0,
      37,    36,    80,   122,     0,   121,     0,     0,     0,    27,
      28,    73,     0,     0,     0,    45,     0,     0,    30,    29,
      86,    82,    88,   122,    85,    89,    91,   106,     0,     0,
       0,     0,     0,    42,   100,   102,     0,     0,     0,   152,
       0,   150,   151,    92,   122,     0,     0,    25,     0,     0,
       0,     0,     0,     0,     0,   173,     0,    70,    71,   140,
     139,   135,   136,   137,   138,     0,     0,     0,     0,     0,
      72,     0,     0,     0,   116,   114,     0,    53,     0,    61,
      65,     0,     0,   147,     0,   146,   148,    77,    76,   172,
     171,     0,    46,     0,    22,    38,    39,    40,    41,    55,
      57,    59,   103,   149,     0,     0,    62,   180,   181,   182,
     183,   184,   185,   177,     0,   175,   176,    98,    99,    93,
       0,    97,    51,    50,    48,    52,     0,    49,     0,     0,
      75,     0,   169,    44,    64,    94,     0,     0,     0,    95,
      63,   145,   144,   143,   142,   167,   168,    96,   186,   179,
     187,   188,   189,   178
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    66,    67,   106,    68,    69,    70,    71,    72,   266,
      73,    74,    75,    76,    77,    78,    79,    80,   167,   278,
     115,   170,    96,   136,   137,   110,   123,   254,   255,   175,
     127,   128,   129,   111,   334,   335,   290,   291,    99,   124,
     100,   130,   339,    81,   341,   221
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -292
static const short yypact[] =
{
     784,   442,  -292,  -292,     0,   142,   442,   442,   442,   442,
       6,     6,     6,   186,   409,   409,   409,   409,    45,     7,
     630,   885,   885,   142,   409,     6,     6,   142,   323,   186,
     186,   409,   885,   442,   442,   442,   334,    58,   762,  -109,
    -109,   142,  -109,   334,   334,   334,   409,   334,   334,     6,
     142,     6,   885,   885,   885,   409,   142,  -109,     6,     6,
     142,   885,   -85,  1009,   186,   186,    10,   886,  -292,  -292,
    -292,  -292,  -292,  -292,  -292,  -292,  -292,  -292,  -292,  -292,
    -292,    38,  -292,  -292,  -292,  -292,  -292,  -292,  -292,  -292,
    -292,  -292,  -292,  -292,  -292,   442,  -292,   -66,   101,  -292,
    -292,  -292,  -292,  -292,  -292,   142,  -292,  -292,   499,   499,
     142,   118,   142,  -292,  -292,   442,   442,   442,  -292,  -292,
    -292,  -292,  -292,   442,  -292,  -292,   409,   210,  -292,  -292,
    -292,   409,   409,   142,  -292,    61,    82,  -292,    19,   142,
    -292,   409,  -292,   409,  -292,   409,  -292,   409,  -292,   409,
     142,   142,  -292,   159,  -292,   142,    72,     3,    70,  -292,
     409,   142,   142,   101,   142,  -292,  -292,   142,  -292,  -292,
     409,   142,   142,  -292,   142,   -37,  -292,     6,     6,   -27,
     -21,   142,   142,  -292,   142,  -292,   409,  -292,   409,  -292,
     409,   261,  -292,   142,   442,   442,  -292,  -292,   409,   142,
     554,   210,  -292,  -292,  -292,  -292,  -292,  -292,  -292,  -292,
    -292,  -292,  -292,  -292,  -292,  -292,  -292,  -292,  -292,  -292,
      14,  -292,   142,   142,  -292,  -292,  -292,    79,  -292,   442,
     442,   442,   442,  -292,  -292,   535,   535,  -292,  -292,   535,
     535,   535,   142,   261,  -292,   261,   409,  -292,   210,   286,
    -292,  -292,  -292,   409,   142,  -292,   120,   142,   409,  -292,
    -292,  -292,   969,   142,   142,  -292,   -18,   142,  -292,  -292,
    -292,  -292,  -292,   409,  -292,  -292,  -292,  -292,   142,   142,
     142,   142,   142,  -292,  -292,  -292,   142,   142,   142,  -292,
     336,  -292,  -292,  -292,   535,   535,   142,  -292,    12,    21,
      24,    25,    28,    37,    22,  -292,   -65,  -292,  -292,  -292,
    -292,   -61,   -61,  -292,  -292,   142,   142,   142,   409,   142,
    -292,   336,   336,   142,  -292,  -292,   142,  -292,   969,  -292,
    -292,   142,   969,  -292,    23,  -292,  -292,  -292,  -292,    32,
      34,   142,  -292,   142,  -292,  -292,  -292,  -292,  -292,  -292,
    -292,  -292,  -292,  -292,   142,   409,  -292,  -292,  -292,  -292,
    -292,  -292,  -292,    50,    40,  -292,  -292,  -292,  -292,  -292,
     142,  -292,  -292,  -292,  -292,  -292,    23,  -292,     2,   675,
    -292,   -18,   -18,  -292,  -292,  -292,   142,   189,    60,  -292,
    -292,  -292,  -292,  -292,  -292,  -292,  -292,  -292,  -292,  -292,
    -292,  -292,  -292,  -292
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -292,  -292,  -292,    -5,    84,  -292,  -292,  -292,  -292,  -292,
    -292,  -292,  -292,  -292,  -292,  -292,  -292,  -292,    35,  -292,
      27,  -292,    -9,  -292,  -112,    -4,   204,   -50,   475,   191,
     355,    41,  -292,    66,  -291,  -292,  -185,  -279,  -292,  -292,
    -292,   192,  -292,  -264,  -292,  -292
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -121
static const short yytable[] =
{
     107,   102,   340,   391,   263,   112,   165,   113,   102,   135,
     224,   353,   103,   134,   140,   142,   144,   146,   148,   103,
     102,   104,   152,   154,   102,   365,   366,   159,   104,   161,
     162,   103,   166,   169,   199,   103,   173,   376,   116,   117,
     104,   378,   353,   353,   104,   183,   102,   185,   187,   189,
     226,   192,   150,   151,   228,   196,   197,   103,   321,   102,
     322,    89,    90,    91,    92,    93,   104,    98,   231,   232,
     103,   102,   108,   109,   171,   172,   182,   174,   184,   104,
     309,   277,   103,   102,   248,   194,   195,   262,   394,   392,
     281,   104,   193,   233,   103,   256,   282,   257,    63,   357,
     234,   163,   102,   104,   249,   237,   306,   238,   358,   363,
     267,   359,   360,   103,   364,   361,   264,   395,   396,   242,
     114,   102,   104,   379,   362,   381,   105,   382,   247,   139,
     388,   250,   103,   251,   252,   328,   286,   325,   287,   228,
     288,   104,   387,   102,   379,   259,   260,   403,   296,     0,
     261,   225,     0,   265,   103,     0,   268,   269,   270,   271,
    -120,   227,   272,   104,     0,    94,   274,   275,   244,   276,
       0,  -120,     0,   165,   235,   236,   283,   284,     0,   285,
    -120,   239,   240,   241,     0,   315,   -43,   118,   293,   317,
     119,     0,     0,   101,   297,   120,   121,   122,   101,   101,
     101,   101,     0,   326,   279,   280,   229,   230,   231,   232,
       0,   310,     0,    82,    83,    84,    85,   307,   308,    86,
      87,    88,     0,   343,     0,   101,   101,   101,   229,   230,
     231,   232,   155,   156,   157,   177,   178,   320,   180,   181,
     135,     0,     0,     0,   354,   229,   230,   231,   232,   327,
       0,   329,   330,   333,     0,   220,     0,     0,   337,   338,
     294,   295,   342,     0,    82,    83,    84,    85,   222,   223,
      86,    87,    88,   344,   345,   346,   347,   348,   398,     0,
     399,   349,   350,   351,     0,   352,     0,   101,     0,     0,
       0,   356,   400,   401,   402,   311,   312,   313,   314,     0,
     101,   101,    89,    90,    91,    92,    93,   101,   101,   101,
     367,   368,   369,     0,   371,   101,   372,   373,   374,   333,
       0,   375,     0,   333,   153,     0,   377,   119,     0,   380,
       0,     0,   120,   121,   122,   103,   383,   102,   384,    82,
      83,    84,    85,     0,   104,    86,    87,    88,   103,   385,
      89,    90,    91,    92,    93,     0,    97,   104,     0,     0,
       0,    97,    97,    97,    97,   389,     0,     0,     0,     0,
     333,   390,     0,   138,   289,     0,     0,     0,     0,     0,
       0,   397,     0,   292,     0,     0,   101,   101,    97,    97,
      97,   164,   304,   305,     0,     0,     0,     0,   176,   176,
     176,     0,   176,   176,     0,     0,    94,     0,     0,     0,
       0,     0,    82,    83,    84,    85,     0,     0,    86,    87,
      88,   101,   101,   101,   101,    89,    90,    91,    92,    93,
       0,     0,     0,     0,     0,   292,     0,   292,     0,     0,
     324,     0,     0,     0,     0,    82,    83,    84,    85,   289,
      97,    86,    87,    88,   336,     0,     0,     0,    89,    90,
      91,    92,    93,    97,    97,     0,     0,     0,     0,     0,
      97,    97,    97,     0,     0,     0,     0,     0,    97,     0,
       0,     0,   292,     0,     0,     0,     0,     0,     0,   126,
     131,   132,   133,     0,     0,   143,   145,   147,     0,   149,
       0,     0,    82,    83,    84,    85,   158,   160,    86,    87,
      88,     0,     0,   292,   292,    89,    90,    91,    92,    93,
     336,   179,     0,     0,   336,     0,   125,   186,   188,   190,
     191,     0,     0,     0,     0,     0,   198,     0,    82,    83,
      84,    85,     0,     0,    86,    87,    88,     0,     0,    97,
      97,    89,    90,    91,    92,    93,     0,    82,    83,    84,
      85,     0,    94,    86,    87,    88,     0,     0,     0,     0,
       0,   336,     0,    95,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    97,    97,    97,    97,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   243,     0,     0,    97,     0,   245,   246,     0,     0,
       0,     0,     0,     0,     0,     0,   253,    97,     0,    94,
       0,     0,     0,     0,   258,     0,   229,   230,   231,   232,
      95,   102,     0,    82,    83,    84,    85,     0,     0,    86,
      87,    88,   103,     0,     0,   273,    89,    90,    91,    92,
      93,   104,   125,     0,     0,   298,   299,   300,   301,   302,
     303,     0,   229,   230,   231,   232,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   393,     0,    82,    83,
      84,    85,     0,    97,    86,    87,    88,    97,     0,     0,
       0,    89,    90,    91,    92,    93,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   316,     0,     0,     0,   318,   319,     0,     0,     0,
       0,   323,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   331,    97,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   125,     0,     0,
       0,     0,   141,     0,     0,     0,     0,     0,     0,     0,
       0,   332,     0,   102,     0,  -111,  -111,  -111,  -111,     0,
     355,  -111,  -111,  -111,   103,     0,   168,     0,  -111,  -111,
    -111,  -111,  -111,   104,     0,     0,     0,     0,     0,     0,
       0,     1,     0,   370,     0,    94,     2,     3,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,     0,     0,    19,    20,    21,    22,    23,
     386,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -111,
       0,     0,     0,     0,     0,     0,   102,     0,    82,    83,
      84,    85,     0,     1,    86,    87,    88,   103,     0,     3,
      63,    89,    90,    91,    92,    93,   104,     0,    64,    65,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,     0,     0,    19,    20,    21,
      22,    23,     0,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    82,    83,    84,    85,     0,     0,    86,    87,
      88,     0,     0,     0,     0,    89,    90,    91,    92,    93,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   125,     0,     0,     0,     0,     0,     0,     0,
      64,    65,    82,    83,    84,    85,     0,     0,    86,    87,
      88,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   332,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    94,
       0,     0,     0,     0,     0,   200,     0,   201,   202,     0,
       0,     0,     0,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219
};

static const short yycheck[] =
{
       5,     1,   266,     1,     1,     9,   115,     1,     1,    18,
       0,   290,    12,    18,    19,    20,    21,    22,    23,    12,
       1,    21,    27,    28,     1,    90,    91,    32,    21,    33,
      34,    12,    37,    38,   119,    12,    41,   328,    11,    12,
      21,   332,   321,   322,    21,    50,     1,    52,    53,    54,
      12,    56,    25,    26,   120,    60,    61,    12,   243,     1,
     245,    16,    17,    18,    19,    20,    21,     1,   129,   130,
      12,     1,     6,     7,    39,    40,    49,    42,    51,    21,
       1,   118,    12,     1,    23,    58,    59,    15,   379,    87,
     117,    21,    57,    98,    12,   145,   117,   147,   116,    87,
     105,    35,     1,    21,    22,   110,    92,   112,    87,    87,
     160,    87,    87,    12,    92,    87,   113,   381,   382,   123,
     114,     1,    21,   121,    87,    93,   126,    93,   133,   122,
      90,   136,    12,   138,   139,    15,   186,   249,   188,   120,
     190,    21,    92,     1,   121,   150,   151,    87,   198,    -1,
     155,    67,    -1,   158,    12,    -1,   161,   162,   163,   164,
       1,    95,   167,    21,    -1,   120,   171,   172,   127,   174,
      -1,    12,    -1,   115,   108,   109,   181,   182,    -1,   184,
      21,   115,   116,   117,    -1,   235,   116,     1,   193,   239,
       4,    -1,    -1,     1,   199,     9,    10,    11,     6,     7,
       8,     9,    -1,   253,   177,   178,   127,   128,   129,   130,
      -1,   132,    -1,     3,     4,     5,     6,   222,   223,     9,
      10,    11,    -1,   273,    -1,    33,    34,    35,   127,   128,
     129,   130,    28,    29,    30,    44,    45,   242,    47,    48,
     249,    -1,    -1,    -1,   294,   127,   128,   129,   130,   254,
      -1,   256,   257,   262,    -1,    63,    -1,    -1,   263,   264,
     194,   195,   267,    -1,     3,     4,     5,     6,    64,    65,
       9,    10,    11,   278,   279,   280,   281,   282,    89,    -1,
      91,   286,   287,   288,    -1,   290,    -1,    95,    -1,    -1,
      -1,   296,   103,   104,   105,   229,   230,   231,   232,    -1,
     108,   109,    16,    17,    18,    19,    20,   115,   116,   117,
     315,   316,   317,    -1,   319,   123,   321,   322,   323,   328,
      -1,   326,    -1,   332,     1,    -1,   331,     4,    -1,   334,
      -1,    -1,     9,    10,    11,    12,   341,     1,   343,     3,
       4,     5,     6,    -1,    21,     9,    10,    11,    12,   354,
      16,    17,    18,    19,    20,    -1,     1,    21,    -1,    -1,
      -1,     6,     7,     8,     9,   370,    -1,    -1,    -1,    -1,
     379,   376,    -1,    18,   113,    -1,    -1,    -1,    -1,    -1,
      -1,   386,    -1,   191,    -1,    -1,   194,   195,    33,    34,
      35,    36,   200,   201,    -1,    -1,    -1,    -1,    43,    44,
      45,    -1,    47,    48,    -1,    -1,   120,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,   229,   230,   231,   232,    16,    17,    18,    19,    20,
      -1,    -1,    -1,    -1,    -1,   243,    -1,   245,    -1,    -1,
     248,    -1,    -1,    -1,    -1,     3,     4,     5,     6,   113,
      95,     9,    10,    11,   262,    -1,    -1,    -1,    16,    17,
      18,    19,    20,   108,   109,    -1,    -1,    -1,    -1,    -1,
     115,   116,   117,    -1,    -1,    -1,    -1,    -1,   123,    -1,
      -1,    -1,   290,    -1,    -1,    -1,    -1,    -1,    -1,    14,
      15,    16,    17,    -1,    -1,    20,    21,    22,    -1,    24,
      -1,    -1,     3,     4,     5,     6,    31,    32,     9,    10,
      11,    -1,    -1,   321,   322,    16,    17,    18,    19,    20,
     328,    46,    -1,    -1,   332,    -1,   117,    52,    53,    54,
      55,    -1,    -1,    -1,    -1,    -1,    61,    -1,     3,     4,
       5,     6,    -1,    -1,     9,    10,    11,    -1,    -1,   194,
     195,    16,    17,    18,    19,    20,    -1,     3,     4,     5,
       6,    -1,   120,     9,    10,    11,    -1,    -1,    -1,    -1,
      -1,   379,    -1,   131,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   229,   230,   231,   232,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   126,    -1,    -1,   249,    -1,   131,   132,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   141,   262,    -1,   120,
      -1,    -1,    -1,    -1,   149,    -1,   127,   128,   129,   130,
     131,     1,    -1,     3,     4,     5,     6,    -1,    -1,     9,
      10,    11,    12,    -1,    -1,   170,    16,    17,    18,    19,
      20,    21,   117,    -1,    -1,   101,   102,   103,   104,   105,
     106,    -1,   127,   128,   129,   130,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,     3,     4,
       5,     6,    -1,   328,     9,    10,    11,   332,    -1,    -1,
      -1,    16,    17,    18,    19,    20,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   236,    -1,    -1,    -1,   240,   241,    -1,    -1,    -1,
      -1,   246,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   258,   379,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,
      -1,    -1,   122,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,     1,    -1,     3,     4,     5,     6,    -1,
     295,     9,    10,    11,    12,    -1,    14,    -1,    16,    17,
      18,    19,    20,    21,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     7,    -1,   318,    -1,   120,    12,    13,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    -1,    -1,    41,    42,    43,    44,    45,
     355,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,
      -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,     3,     4,
       5,     6,    -1,     7,     9,    10,    11,    12,    -1,    13,
     116,    16,    17,    18,    19,    20,    21,    -1,   124,   125,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,    -1,    41,    42,    43,
      44,    45,    -1,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    -1,    -1,    -1,    -1,    16,    17,    18,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   117,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     124,   125,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    88,    89,    -1,
      -1,    -1,    -1,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     7,    12,    13,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    41,
      42,    43,    44,    45,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,   116,   124,   125,   134,   135,   137,   138,
     139,   140,   141,   143,   144,   145,   146,   147,   148,   149,
     150,   176,     3,     4,     5,     6,     9,    10,    11,    16,
      17,    18,    19,    20,   120,   131,   155,   163,   166,   171,
     173,   174,     1,    12,    21,   126,   136,   136,   166,   166,
     158,   166,   158,     1,   114,   153,   153,   153,     1,     4,
       9,    10,    11,   159,   172,   117,   161,   163,   164,   165,
     174,   161,   161,   161,   136,   155,   156,   157,   163,   122,
     136,   122,   136,   161,   136,   161,   136,   161,   136,   161,
     153,   153,   136,     1,   136,   159,   159,   159,   161,   136,
     161,   158,   158,   166,   163,   115,   136,   151,    14,   136,
     154,   151,   151,   136,   151,   162,   163,   162,   162,   161,
     162,   162,   153,   136,   153,   136,   161,   136,   161,   136,
     161,   161,   136,   151,   153,   153,   136,   136,   161,   119,
      86,    88,    89,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     174,   178,   159,   159,     0,   137,    12,   166,   120,   127,
     128,   129,   130,   136,   136,   166,   166,   136,   136,   166,
     166,   166,   158,   161,   164,   161,   161,   136,    23,    22,
     136,   136,   136,   161,   160,   161,   160,   160,   161,   136,
     136,   136,    15,     1,   113,   136,   142,   160,   136,   136,
     136,   136,   136,   161,   136,   136,   136,   118,   152,   153,
     153,   117,   117,   136,   136,   136,   160,   160,   160,   113,
     169,   170,   174,   136,   166,   166,   160,   136,   101,   102,
     103,   104,   105,   106,   174,   174,    92,   136,   136,     1,
     132,   166,   166,   166,   166,   160,   161,   160,   161,   161,
     136,   169,   169,   161,   174,   157,   160,   136,    15,   136,
     136,   161,    86,   155,   167,   168,   174,   136,   136,   175,
     176,   177,   136,   160,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   170,   160,   161,   136,    87,    87,    87,
      87,    87,    87,    87,    92,    90,    91,   136,   136,   136,
     161,   136,   136,   136,   136,   136,   167,   136,   167,   121,
     136,    93,    93,   136,   136,   136,   161,    92,    90,   136,
     136,     1,    87,     1,   167,   176,   176,   136,    89,    91,
     103,   104,   105,    87
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)           \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#define YYLEX	yylex ()

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*-----------------------------.
| Print this symbol on YYOUT.  |
`-----------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yysymprint (FILE* yyout, int yytype, YYSTYPE yyvalue)
#else
yysymprint (yyout, yytype, yyvalue)
    FILE* yyout;
    int yytype;
    YYSTYPE yyvalue;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyout, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyout, yytoknum[yytype], yyvalue);
# endif
    }
  else
    YYFPRINTF (yyout, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyout, ")");
}
#endif /* YYDEBUG. */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
#if defined (__STDC__) || defined (__cplusplus)
yydestruct (int yytype, YYSTYPE yyvalue)
#else
yydestruct (yytype, yyvalue)
    int yytype;
    YYSTYPE yyvalue;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvalue;

  switch (yytype)
    {
      default:
        break;
    }
}



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of parse errors so far.  */
int yynerrs;


int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with.  */

  if (yychar <= 0)		/* This means end of input.  */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more.  */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

      /* We have to keep this `#if YYDEBUG', since we use variables
	 which are defined only if `YYDEBUG' is set.  */
      YYDPRINTF ((stderr, "Next token is "));
      YYDSYMPRINT ((stderr, yychar1, yylval));
      YYDPRINTF ((stderr, "\n"));
    }

  /* If the proper action on seeing token YYCHAR1 is to reduce or to
     detect an error, take that action.  */
  yyn += yychar1;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yychar1)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];



#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn - 1, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] >= 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif
  switch (yyn)
    {
        case 2:
#line 163 "mon_parse.y"
    { yyval.i = 0; }
    break;

  case 3:
#line 164 "mon_parse.y"
    { yyval.i = 0; }
    break;

  case 4:
#line 165 "mon_parse.y"
    { new_cmd = 1; asm_mode = 0;  yyval.i = 0; }
    break;

  case 9:
#line 174 "mon_parse.y"
    { return ERR_EXPECT_END_CMD; }
    break;

  case 21:
#line 188 "mon_parse.y"
    { return ERR_BAD_CMD; }
    break;

  case 22:
#line 192 "mon_parse.y"
    { mon_bank(yyvsp[-2].i,yyvsp[-1].str); }
    break;

  case 23:
#line 194 "mon_parse.y"
    { mon_jump(yyvsp[-1].a); }
    break;

  case 24:
#line 196 "mon_parse.y"
    { mon_display_io_regs(); }
    break;

  case 25:
#line 198 "mon_parse.y"
    { monitor_cpu_type_set(yyvsp[-1].str); }
    break;

  case 26:
#line 200 "mon_parse.y"
    { mon_instruction_return(); }
    break;

  case 27:
#line 202 "mon_parse.y"
    { machine_write_snapshot(yyvsp[-1].str,0,0,0); /* FIXME */ }
    break;

  case 28:
#line 204 "mon_parse.y"
    { machine_read_snapshot(yyvsp[-1].str, 0); }
    break;

  case 29:
#line 206 "mon_parse.y"
    { mon_instructions_step(yyvsp[-1].i); }
    break;

  case 30:
#line 208 "mon_parse.y"
    { mon_instructions_next(yyvsp[-1].i); }
    break;

  case 31:
#line 210 "mon_parse.y"
    { mon_stack_up(yyvsp[-1].i); }
    break;

  case 32:
#line 212 "mon_parse.y"
    { mon_stack_down(yyvsp[-1].i); }
    break;

  case 33:
#line 214 "mon_parse.y"
    { mon_display_screen(); }
    break;

  case 35:
#line 219 "mon_parse.y"
    { (monitor_cpu_type.mon_register_print)(default_memspace); }
    break;

  case 36:
#line 221 "mon_parse.y"
    { (monitor_cpu_type.mon_register_print)(yyvsp[-1].i); }
    break;

  case 38:
#line 226 "mon_parse.y"
    {
                        playback = TRUE; playback_name = yyvsp[-1].str;
                        /*mon_load_symbols(, );*/
                    }
    break;

  case 39:
#line 231 "mon_parse.y"
    { mon_save_symbols(yyvsp[-2].i, yyvsp[-1].str); }
    break;

  case 40:
#line 233 "mon_parse.y"
    { mon_add_name_to_symbol_table(yyvsp[-2].a, yyvsp[-1].str); }
    break;

  case 41:
#line 235 "mon_parse.y"
    { mon_remove_name_from_symbol_table(yyvsp[-2].i, yyvsp[-1].str); }
    break;

  case 42:
#line 237 "mon_parse.y"
    { mon_print_symbol_table(yyvsp[-1].i); }
    break;

  case 43:
#line 241 "mon_parse.y"
    { mon_start_assemble_mode(yyvsp[0].a, NULL); }
    break;

  case 45:
#line 243 "mon_parse.y"
    { mon_start_assemble_mode(yyvsp[-1].a, NULL); }
    break;

  case 46:
#line 245 "mon_parse.y"
    { mon_disassemble_lines(yyvsp[-2].a,yyvsp[-1].a); }
    break;

  case 47:
#line 247 "mon_parse.y"
    { mon_disassemble_lines(BAD_ADDR, BAD_ADDR); }
    break;

  case 48:
#line 251 "mon_parse.y"
    { mon_memory_move(yyvsp[-3].a, yyvsp[-2].a, yyvsp[-1].a); }
    break;

  case 49:
#line 253 "mon_parse.y"
    { mon_memory_compare(yyvsp[-3].a, yyvsp[-2].a, yyvsp[-1].a); }
    break;

  case 50:
#line 255 "mon_parse.y"
    { mon_memory_fill(yyvsp[-3].a,yyvsp[-2].a,(unsigned char *)yyvsp[-1].str); }
    break;

  case 51:
#line 257 "mon_parse.y"
    { mon_memory_hunt(yyvsp[-3].a,yyvsp[-2].a,(unsigned char *)yyvsp[-1].str); }
    break;

  case 52:
#line 259 "mon_parse.y"
    { mon_memory_display(yyvsp[-3].rt, yyvsp[-2].a, yyvsp[-1].a); }
    break;

  case 53:
#line 261 "mon_parse.y"
    { mon_memory_display(default_radix, yyvsp[-2].a, yyvsp[-1].a); }
    break;

  case 54:
#line 263 "mon_parse.y"
    { mon_memory_display(default_radix, BAD_ADDR, BAD_ADDR); }
    break;

  case 55:
#line 265 "mon_parse.y"
    { mon_memory_display_data(yyvsp[-2].a, yyvsp[-1].a, 8, 8); }
    break;

  case 56:
#line 267 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 8, 8); }
    break;

  case 57:
#line 269 "mon_parse.y"
    { mon_memory_display_data(yyvsp[-2].a, yyvsp[-1].a, 24, 21); }
    break;

  case 58:
#line 271 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 24, 21); }
    break;

  case 59:
#line 273 "mon_parse.y"
    { mon_memory_display(0, yyvsp[-2].a, yyvsp[-1].a); }
    break;

  case 60:
#line 275 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR); }
    break;

  case 61:
#line 279 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint(yyvsp[-2].a, yyvsp[-1].a, FALSE, FALSE,
                                                    FALSE, FALSE);
                  }
    break;

  case 62:
#line 284 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint(yyvsp[-2].a, yyvsp[-1].a, FALSE, FALSE,
                                                    FALSE, TRUE);
                  }
    break;

  case 63:
#line 289 "mon_parse.y"
    {
                      temp = mon_breakpoint_add_checkpoint(yyvsp[-4].a, yyvsp[-3].a, FALSE,
                                                           FALSE, FALSE, FALSE);
                      mon_breakpoint_set_checkpoint_condition(temp, yyvsp[-1].cond_node);
                  }
    break;

  case 64:
#line 295 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint(yyvsp[-2].a, yyvsp[-1].a, FALSE,
                      (yyvsp[-3].i == e_load || yyvsp[-3].i == e_load_store),
                      (yyvsp[-3].i == e_store || yyvsp[-3].i == e_load_store), FALSE);
                  }
    break;

  case 65:
#line 301 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint(yyvsp[-2].a, yyvsp[-1].a, TRUE, FALSE, FALSE,
                                                    FALSE);
                  }
    break;

  case 66:
#line 306 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 67:
#line 308 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 68:
#line 310 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 69:
#line 312 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 70:
#line 317 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_ON, yyvsp[-1].i); }
    break;

  case 71:
#line 319 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_OFF, yyvsp[-1].i); }
    break;

  case 72:
#line 321 "mon_parse.y"
    { mon_breakpoint_set_ignore_count(yyvsp[-2].i, yyvsp[-1].i); }
    break;

  case 73:
#line 323 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint(yyvsp[-1].i); }
    break;

  case 74:
#line 325 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint(-1); }
    break;

  case 75:
#line 327 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_condition(yyvsp[-3].i, yyvsp[-1].cond_node); }
    break;

  case 76:
#line 329 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_command(yyvsp[-2].i, yyvsp[-1].str); }
    break;

  case 77:
#line 331 "mon_parse.y"
    { return ERR_EXPECT_STRING; }
    break;

  case 78:
#line 335 "mon_parse.y"
    { sidefx = ((yyvsp[-1].action == e_TOGGLE) ? (sidefx ^ 1) : yyvsp[-1].action); }
    break;

  case 79:
#line 337 "mon_parse.y"
    {
                         mon_out("I/O side effects are %s\n",
                                   sidefx ? "enabled" : "disabled");
                     }
    break;

  case 80:
#line 342 "mon_parse.y"
    { default_radix = yyvsp[-1].rt; }
    break;

  case 81:
#line 344 "mon_parse.y"
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

                         mon_out("Default radix is %s\n", p);
                     }
    break;

  case 82:
#line 362 "mon_parse.y"
    {
                         mon_out("Setting default device to `%s'\n",
                         _mon_space_strings[(int) yyvsp[-1].i]); default_memspace = yyvsp[-1].i;
                     }
    break;

  case 83:
#line 367 "mon_parse.y"
    { exit_mon = 2; YYACCEPT; }
    break;

  case 84:
#line 369 "mon_parse.y"
    { exit_mon = 1; YYACCEPT; }
    break;

  case 85:
#line 373 "mon_parse.y"
    { mon_drive_execute_disk_cmd(yyvsp[-1].str); }
    break;

  case 86:
#line 375 "mon_parse.y"
    { mon_out("\t%d\n",yyvsp[-1].i); }
    break;

  case 87:
#line 377 "mon_parse.y"
    { mon_command_print_help(NULL); }
    break;

  case 88:
#line 379 "mon_parse.y"
    { mon_command_print_help(yyvsp[-1].str); }
    break;

  case 89:
#line 381 "mon_parse.y"
    { printf("SYSTEM COMMAND: %s\n",yyvsp[-1].str); }
    break;

  case 90:
#line 383 "mon_parse.y"
    { mon_print_convert(yyvsp[-1].i); }
    break;

  case 91:
#line 385 "mon_parse.y"
    { mon_change_dir(yyvsp[-1].str); }
    break;

  case 92:
#line 387 "mon_parse.y"
    { mon_keyboard_feed(yyvsp[-1].str); }
    break;

  case 93:
#line 391 "mon_parse.y"
    { mon_file_load(yyvsp[-3].str,yyvsp[-2].i,yyvsp[-1].a,FALSE); }
    break;

  case 94:
#line 393 "mon_parse.y"
    { mon_file_load(yyvsp[-3].str,yyvsp[-2].i,yyvsp[-1].a,TRUE); }
    break;

  case 95:
#line 395 "mon_parse.y"
    { mon_file_save(yyvsp[-4].str,yyvsp[-3].i,yyvsp[-2].a,yyvsp[-1].a,FALSE); }
    break;

  case 96:
#line 397 "mon_parse.y"
    { mon_file_save(yyvsp[-4].str,yyvsp[-3].i,yyvsp[-2].a,yyvsp[-1].a,TRUE); }
    break;

  case 97:
#line 399 "mon_parse.y"
    { mon_file_verify(yyvsp[-3].str,yyvsp[-2].i,yyvsp[-1].a); }
    break;

  case 98:
#line 401 "mon_parse.y"
    { mon_drive_block_cmd(0,yyvsp[-3].i,yyvsp[-2].i,yyvsp[-1].a); }
    break;

  case 99:
#line 403 "mon_parse.y"
    { mon_drive_block_cmd(1,yyvsp[-3].i,yyvsp[-2].i,yyvsp[-1].a); }
    break;

  case 100:
#line 407 "mon_parse.y"
    { mon_record_commands(yyvsp[-1].str); }
    break;

  case 101:
#line 409 "mon_parse.y"
    { mon_end_recording(); }
    break;

  case 102:
#line 411 "mon_parse.y"
    { playback=TRUE; playback_name = yyvsp[-1].str; }
    break;

  case 103:
#line 415 "mon_parse.y"
    { mon_memory_fill(yyvsp[-2].a, BAD_ADDR, (unsigned char *)yyvsp[-1].str); }
    break;

  case 104:
#line 417 "mon_parse.y"
    { printf("Not yet.\n"); }
    break;

  case 105:
#line 420 "mon_parse.y"
    { yyval.str = yyvsp[0].str; }
    break;

  case 107:
#line 424 "mon_parse.y"
    { yyval.str = NULL; }
    break;

  case 109:
#line 428 "mon_parse.y"
    { return ERR_EXPECT_FILENAME; }
    break;

  case 110:
#line 431 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 111:
#line 432 "mon_parse.y"
    { yyval.i = e_load_store; }
    break;

  case 112:
#line 435 "mon_parse.y"
    { yyval.i = new_reg(default_memspace, yyvsp[0].reg); }
    break;

  case 113:
#line 436 "mon_parse.y"
    { yyval.i = new_reg(yyvsp[-1].i, yyvsp[0].reg); }
    break;

  case 116:
#line 443 "mon_parse.y"
    { (monitor_cpu_type.mon_register_set_val)(reg_memspace(yyvsp[-2].i), reg_regid(yyvsp[-2].i), yyvsp[0].i); }
    break;

  case 117:
#line 446 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 118:
#line 447 "mon_parse.y"
    { yyval.i = -1; }
    break;

  case 119:
#line 450 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 120:
#line 451 "mon_parse.y"
    { return ERR_EXPECT_BRKNUM; }
    break;

  case 121:
#line 454 "mon_parse.y"
    { yyval.a = yyvsp[0].a; }
    break;

  case 122:
#line 455 "mon_parse.y"
    { yyval.a = BAD_ADDR; }
    break;

  case 123:
#line 458 "mon_parse.y"
    { yyval.a = new_addr(e_default_space,yyvsp[0].i);
                  if (opt_asm) new_cmd = asm_mode = 1; }
    break;

  case 124:
#line 460 "mon_parse.y"
    { yyval.a = new_addr(yyvsp[-1].i,yyvsp[0].i);
                           if (opt_asm) new_cmd = asm_mode = 1; }
    break;

  case 125:
#line 462 "mon_parse.y"
    { temp = mon_symbol_table_lookup_addr(e_default_space, yyvsp[0].str);
                 if (temp >= 0)
                    yyval.a = new_addr(e_default_space, temp);
                 else
                    return ERR_UNDEFINED_LABEL;
               }
    break;

  case 126:
#line 470 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 127:
#line 471 "mon_parse.y"
    { yyval.i = e_default_space; }
    break;

  case 128:
#line 474 "mon_parse.y"
    { yyval.i = e_comp_space; }
    break;

  case 129:
#line 475 "mon_parse.y"
    { yyval.i = e_disk8_space; }
    break;

  case 130:
#line 476 "mon_parse.y"
    { yyval.i = e_disk9_space; }
    break;

  case 131:
#line 477 "mon_parse.y"
    { yyval.i = e_disk10_space; }
    break;

  case 132:
#line 478 "mon_parse.y"
    { yyval.i = e_disk11_space; }
    break;

  case 133:
#line 485 "mon_parse.y"
    { yyval.i = yyvsp[0].i; if (!CHECK_ADDR(yyvsp[0].i)) return ERR_ADDR_TOO_BIG; }
    break;

  case 134:
#line 487 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 135:
#line 489 "mon_parse.y"
    { yyval.i = yyvsp[-2].i + yyvsp[0].i; }
    break;

  case 136:
#line 490 "mon_parse.y"
    { yyval.i = yyvsp[-2].i - yyvsp[0].i; }
    break;

  case 137:
#line 491 "mon_parse.y"
    { yyval.i = yyvsp[-2].i * yyvsp[0].i; }
    break;

  case 138:
#line 492 "mon_parse.y"
    { yyval.i = (yyvsp[0].i) ? (yyvsp[-2].i / yyvsp[0].i) : 1; }
    break;

  case 139:
#line 493 "mon_parse.y"
    { yyval.i = yyvsp[-1].i; }
    break;

  case 140:
#line 494 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 141:
#line 495 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 142:
#line 499 "mon_parse.y"
    {
               yyval.cond_node = new_cond; yyval.cond_node->is_parenthized = FALSE;
               yyval.cond_node->child1 = yyvsp[-2].cond_node; yyval.cond_node->child2 = yyvsp[0].cond_node; yyval.cond_node->operation = yyvsp[-1].cond_op;
           }
    break;

  case 143:
#line 504 "mon_parse.y"
    { return ERR_INCOMPLETE_COMPARE_OP; }
    break;

  case 144:
#line 506 "mon_parse.y"
    { yyval.cond_node = yyvsp[-1].cond_node; yyval.cond_node->is_parenthized = TRUE; }
    break;

  case 145:
#line 508 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 146:
#line 510 "mon_parse.y"
    { yyval.cond_node = yyvsp[0].cond_node; }
    break;

  case 147:
#line 513 "mon_parse.y"
    { yyval.cond_node = new_cond;
                            yyval.cond_node->operation = e_INV;
                            yyval.cond_node->is_parenthized = FALSE;
                            yyval.cond_node->reg_num = yyvsp[0].i; yyval.cond_node->is_reg = TRUE;
                            yyval.cond_node->child1 = NULL; yyval.cond_node->child2 = NULL;
                          }
    break;

  case 148:
#line 519 "mon_parse.y"
    { yyval.cond_node = new_cond;
                            yyval.cond_node->operation = e_INV;
                            yyval.cond_node->is_parenthized = FALSE;
                            yyval.cond_node->value = yyvsp[0].i; yyval.cond_node->is_reg = FALSE;
                            yyval.cond_node->child1 = NULL; yyval.cond_node->child2 = NULL;
                          }
    break;

  case 151:
#line 531 "mon_parse.y"
    { mon_add_number_to_buffer(yyvsp[0].i); }
    break;

  case 152:
#line 532 "mon_parse.y"
    { mon_add_string_to_buffer(yyvsp[0].str); }
    break;

  case 153:
#line 535 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 154:
#line 536 "mon_parse.y"
    { yyval.i = (monitor_cpu_type.mon_register_get_val)(reg_memspace(yyvsp[0].i), reg_regid(yyvsp[0].i)); }
    break;

  case 155:
#line 539 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 156:
#line 540 "mon_parse.y"
    { yyval.i = strtol(yyvsp[0].str, NULL, 10); }
    break;

  case 157:
#line 541 "mon_parse.y"
    { yyval.i = strtol(yyvsp[0].str, NULL, 10); }
    break;

  case 158:
#line 542 "mon_parse.y"
    { yyval.i = strtol(yyvsp[0].str, NULL, 10); }
    break;

  case 159:
#line 545 "mon_parse.y"
    { yyval.i = resolve_datatype(B_NUMBER,yyvsp[0].str); }
    break;

  case 160:
#line 546 "mon_parse.y"
    { yyval.i = resolve_datatype(O_NUMBER,yyvsp[0].str); }
    break;

  case 161:
#line 547 "mon_parse.y"
    { yyval.i = resolve_datatype(D_NUMBER,yyvsp[0].str); }
    break;

  case 162:
#line 550 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 163:
#line 551 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 164:
#line 552 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 165:
#line 553 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 166:
#line 554 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 170:
#line 562 "mon_parse.y"
    { yyval.i = 0;
                                                if (yyvsp[-1].str) {
                                                    (monitor_cpu_type.mon_assemble_instr)(yyvsp[-1].str, yyvsp[0].i);
                                                } else {
                                                    new_cmd = 1;
                                                    asm_mode = 0;
                                                }
                                                opt_asm = 0;
                                              }
    break;

  case 172:
#line 573 "mon_parse.y"
    { asm_mode = 0; }
    break;

  case 173:
#line 576 "mon_parse.y"
    { if (yyvsp[0].i > 0xff)
                          yyval.i = join_ints(ASM_ADDR_MODE_IMMEDIATE_16,yyvsp[0].i);
                        else
                          yyval.i = join_ints(ASM_ADDR_MODE_IMMEDIATE,yyvsp[0].i); }
    break;

  case 174:
#line 580 "mon_parse.y"
    { if (yyvsp[0].i < 0x100)
               yyval.i = join_ints(ASM_ADDR_MODE_ZERO_PAGE,yyvsp[0].i);
             else
               yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE,yyvsp[0].i);
           }
    break;

  case 175:
#line 585 "mon_parse.y"
    { if (yyvsp[-2].i < 0x100)
                            yyval.i = join_ints(ASM_ADDR_MODE_ZERO_PAGE_X,yyvsp[-2].i);
                          else
                            yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE_X,yyvsp[-2].i);
                        }
    break;

  case 176:
#line 590 "mon_parse.y"
    { if (yyvsp[-2].i < 0x100)
                            yyval.i = join_ints(ASM_ADDR_MODE_ZERO_PAGE_Y,yyvsp[-2].i);
                          else
                            yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE_Y,yyvsp[-2].i);
                        }
    break;

  case 177:
#line 596 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_ABS_INDIRECT,yyvsp[-1].i); }
    break;

  case 178:
#line 598 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_INDIRECT_X,yyvsp[-3].i); }
    break;

  case 179:
#line 600 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_INDIRECT_Y,yyvsp[-3].i); }
    break;

  case 180:
#line 601 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IND_BC,0); }
    break;

  case 181:
#line 602 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IND_DE,0); }
    break;

  case 182:
#line 603 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IND_HL,0); }
    break;

  case 183:
#line 604 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IND_IX,0); }
    break;

  case 184:
#line 605 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IND_IY,0); }
    break;

  case 185:
#line 606 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IND_SP,0); }
    break;

  case 186:
#line 608 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE_A,yyvsp[-3].i); }
    break;

  case 187:
#line 610 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE_HL,yyvsp[-3].i); }
    break;

  case 188:
#line 612 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE_IX,yyvsp[-3].i); }
    break;

  case 189:
#line 614 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE_IY,yyvsp[-3].i); }
    break;

  case 190:
#line 615 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_IMPLIED,0); }
    break;

  case 191:
#line 616 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_ACCUMULATOR,0); }
    break;

  case 192:
#line 617 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_B,0); }
    break;

  case 193:
#line 618 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_C,0); }
    break;

  case 194:
#line 619 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_D,0); }
    break;

  case 195:
#line 620 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_E,0); }
    break;

  case 196:
#line 621 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_H,0); }
    break;

  case 197:
#line 622 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IXH,0); }
    break;

  case 198:
#line 623 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IYH,0); }
    break;

  case 199:
#line 624 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_L,0); }
    break;

  case 200:
#line 625 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IXL,0); }
    break;

  case 201:
#line 626 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IYL,0); }
    break;

  case 202:
#line 627 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_AF,0); }
    break;

  case 203:
#line 628 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_BC,0); }
    break;

  case 204:
#line 629 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_DE,0); }
    break;

  case 205:
#line 630 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_HL,0); }
    break;

  case 206:
#line 631 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IX,0); }
    break;

  case 207:
#line 632 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IY,0); }
    break;

  case 208:
#line 633 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_SP,0); }
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 2774 "mon_parse.c"

  yyvsp -= yylen;
  yyssp -= yylen;


#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyssp > yyss)
	    {
	      YYDPRINTF ((stderr, "Error: popping "));
	      YYDSYMPRINT ((stderr,
			    yystos[*yyssp],
			    *yyvsp));
	      YYDPRINTF ((stderr, "\n"));
	      yydestruct (yystos[*yyssp], *yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yydestruct (yychar1, yylval);
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDPRINTF ((stderr, "Error: popping "));
      YYDSYMPRINT ((stderr,
		    yystos[*yyssp], *yyvsp));
      YYDPRINTF ((stderr, "\n"));

      yydestruct (yystos[yystate], *yyvsp);
      yyvsp--;
      yystate = *--yyssp;


#if YYDEBUG
      if (yydebug)
	{
	  short *yyssp1 = yyss - 1;
	  YYFPRINTF (stderr, "Error: state stack now");
	  while (yyssp1 != yyssp)
	    YYFPRINTF (stderr, " %d", *++yyssp1);
	  YYFPRINTF (stderr, "\n");
	}
#endif
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 637 "mon_parse.y"


void parse_and_execute_line(char *input)
{
   char *temp_buf;
   int i, rc;

   temp_buf = (char *)lib_malloc(strlen(input) + 3);
   strcpy(temp_buf,input);
   i = strlen(input);
   temp_buf[i++] = '\n';
   temp_buf[i++] = '\0';
   temp_buf[i++] = '\0';

   make_buffer(temp_buf);
   if ( (rc =yyparse()) != 0) {
       mon_out("ERROR -- ");
       switch(rc) {
         case ERR_BAD_CMD:
           mon_out("Bad command:\n");
           break;
         case ERR_RANGE_BAD_START:
           mon_out("Bad first address in range:\n");
           break;
         case ERR_RANGE_BAD_END:
           mon_out("Bad second address in range:\n");
           break;
         case ERR_EXPECT_BRKNUM:
           mon_out("Checkpoint number expected:\n");
           break;
         case ERR_EXPECT_END_CMD:
           mon_out("Unexpected token:\n");
           break;
         case ERR_MISSING_CLOSE_PAREN:
           mon_out("')' expected:\n");
           break;
         case ERR_INCOMPLETE_COMPARE_OP:
           mon_out("Compare operation missing an operand:\n");
           break;
         case ERR_EXPECT_FILENAME:
           mon_out("Expecting a filename:\n");
           break;
         case ERR_ADDR_TOO_BIG:
           mon_out("Address too large:\n");
           break;
         case ERR_IMM_TOO_BIG:
           mon_out("Immediate argument too large:\n");
           break;
         case ERR_EXPECT_STRING:
           mon_out("Expecting a string.\n");
           break;
         case ERR_UNDEFINED_LABEL:
           mon_out("Found an undefined label.\n");
           break;
         case ERR_ILLEGAL_INPUT:
         default:
           mon_out("Wrong syntax:\n");
       }
       mon_out("  %s\n", input);
       for (i = 0; i < last_len; i++)
           mon_out(" ");
       mon_out("  ^\n");
       asm_mode = 0;
       new_cmd = 1;
   }
   free_buffer();
}

static int yyerror(char *s)
{
   fprintf(stderr, "ERR:%s\n", s);
   return 0;
}

static int resolve_datatype(unsigned guess_type, char *num)
{
   /* FIXME: Handle cases when default type is non-numerical */
   if (default_radix == e_hexadecimal) {
       return strtol(num, NULL, 16);
   }

   if ((guess_type == D_NUMBER) || (default_radix == e_decimal)) {
       return strtol(num, NULL, 10);
   }

   if ((guess_type == O_NUMBER) || (default_radix == e_octal)) {
       return strtol(num, NULL, 8);
   }

   return strtol(num, NULL, 2);
}


