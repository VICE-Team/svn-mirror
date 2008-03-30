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
     CMD_SEP = 274,
     REG_ASGN_SEP = 275,
     EQUALS = 276,
     CMD_SIDEFX = 277,
     CMD_RETURN = 278,
     CMD_BLOCK_READ = 279,
     CMD_BLOCK_WRITE = 280,
     CMD_UP = 281,
     CMD_DOWN = 282,
     CMD_LOAD = 283,
     CMD_SAVE = 284,
     CMD_VERIFY = 285,
     CMD_IGNORE = 286,
     CMD_HUNT = 287,
     CMD_FILL = 288,
     CMD_MOVE = 289,
     CMD_GOTO = 290,
     CMD_REGISTERS = 291,
     CMD_READSPACE = 292,
     CMD_WRITESPACE = 293,
     CMD_RADIX = 294,
     CMD_MEM_DISPLAY = 295,
     CMD_BREAK = 296,
     CMD_TRACE = 297,
     CMD_IO = 298,
     CMD_BRMON = 299,
     CMD_COMPARE = 300,
     CMD_DUMP = 301,
     CMD_UNDUMP = 302,
     CMD_EXIT = 303,
     CMD_DELETE = 304,
     CMD_CONDITION = 305,
     CMD_COMMAND = 306,
     CMD_ASSEMBLE = 307,
     CMD_DISASSEMBLE = 308,
     CMD_NEXT = 309,
     CMD_STEP = 310,
     CMD_PRINT = 311,
     CMD_DEVICE = 312,
     CMD_HELP = 313,
     CMD_WATCH = 314,
     CMD_DISK = 315,
     CMD_SYSTEM = 316,
     CMD_QUIT = 317,
     CMD_CHDIR = 318,
     CMD_BANK = 319,
     CMD_LOAD_LABELS = 320,
     CMD_SAVE_LABELS = 321,
     CMD_ADD_LABEL = 322,
     CMD_DEL_LABEL = 323,
     CMD_SHOW_LABELS = 324,
     CMD_RECORD = 325,
     CMD_STOP = 326,
     CMD_PLAYBACK = 327,
     CMD_CHAR_DISPLAY = 328,
     CMD_SPRITE_DISPLAY = 329,
     CMD_TEXT_DISPLAY = 330,
     CMD_ENTER_DATA = 331,
     CMD_ENTER_BIN_DATA = 332,
     CMD_KEYBUF = 333,
     CMD_BLOAD = 334,
     CMD_BSAVE = 335,
     CMD_SCREEN = 336,
     CMD_UNTIL = 337,
     CMD_CPU = 338,
     L_PAREN = 339,
     R_PAREN = 340,
     ARG_IMMEDIATE = 341,
     REG_A = 342,
     REG_X = 343,
     REG_Y = 344,
     COMMA = 345,
     INST_SEP = 346,
     REG_B = 347,
     REG_C = 348,
     REG_D = 349,
     REG_E = 350,
     REG_H = 351,
     REG_L = 352,
     REG_AF = 353,
     REG_BC = 354,
     REG_DE = 355,
     REG_HL = 356,
     REG_IX = 357,
     REG_IY = 358,
     REG_SP = 359,
     REG_IXH = 360,
     REG_IXL = 361,
     REG_IYH = 362,
     REG_IYL = 363,
     CPUTYPE_6502 = 364,
     CPUTYPE_Z80 = 365,
     STRING = 366,
     FILENAME = 367,
     R_O_L = 368,
     OPCODE = 369,
     LABEL = 370,
     BANKNAME = 371,
     CPUTYPE = 372,
     REGISTER = 373,
     COMPARE_OP = 374,
     RADIX_TYPE = 375,
     INPUT_SPEC = 376,
     CMD_CHECKPT_ON = 377,
     CMD_CHECKPT_OFF = 378,
     TOGGLE = 379
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
#define CMD_SEP 274
#define REG_ASGN_SEP 275
#define EQUALS 276
#define CMD_SIDEFX 277
#define CMD_RETURN 278
#define CMD_BLOCK_READ 279
#define CMD_BLOCK_WRITE 280
#define CMD_UP 281
#define CMD_DOWN 282
#define CMD_LOAD 283
#define CMD_SAVE 284
#define CMD_VERIFY 285
#define CMD_IGNORE 286
#define CMD_HUNT 287
#define CMD_FILL 288
#define CMD_MOVE 289
#define CMD_GOTO 290
#define CMD_REGISTERS 291
#define CMD_READSPACE 292
#define CMD_WRITESPACE 293
#define CMD_RADIX 294
#define CMD_MEM_DISPLAY 295
#define CMD_BREAK 296
#define CMD_TRACE 297
#define CMD_IO 298
#define CMD_BRMON 299
#define CMD_COMPARE 300
#define CMD_DUMP 301
#define CMD_UNDUMP 302
#define CMD_EXIT 303
#define CMD_DELETE 304
#define CMD_CONDITION 305
#define CMD_COMMAND 306
#define CMD_ASSEMBLE 307
#define CMD_DISASSEMBLE 308
#define CMD_NEXT 309
#define CMD_STEP 310
#define CMD_PRINT 311
#define CMD_DEVICE 312
#define CMD_HELP 313
#define CMD_WATCH 314
#define CMD_DISK 315
#define CMD_SYSTEM 316
#define CMD_QUIT 317
#define CMD_CHDIR 318
#define CMD_BANK 319
#define CMD_LOAD_LABELS 320
#define CMD_SAVE_LABELS 321
#define CMD_ADD_LABEL 322
#define CMD_DEL_LABEL 323
#define CMD_SHOW_LABELS 324
#define CMD_RECORD 325
#define CMD_STOP 326
#define CMD_PLAYBACK 327
#define CMD_CHAR_DISPLAY 328
#define CMD_SPRITE_DISPLAY 329
#define CMD_TEXT_DISPLAY 330
#define CMD_ENTER_DATA 331
#define CMD_ENTER_BIN_DATA 332
#define CMD_KEYBUF 333
#define CMD_BLOAD 334
#define CMD_BSAVE 335
#define CMD_SCREEN 336
#define CMD_UNTIL 337
#define CMD_CPU 338
#define L_PAREN 339
#define R_PAREN 340
#define ARG_IMMEDIATE 341
#define REG_A 342
#define REG_X 343
#define REG_Y 344
#define COMMA 345
#define INST_SEP 346
#define REG_B 347
#define REG_C 348
#define REG_D 349
#define REG_E 350
#define REG_H 351
#define REG_L 352
#define REG_AF 353
#define REG_BC 354
#define REG_DE 355
#define REG_HL 356
#define REG_IX 357
#define REG_IY 358
#define REG_SP 359
#define REG_IXH 360
#define REG_IXL 361
#define REG_IYH 362
#define REG_IYL 363
#define CPUTYPE_6502 364
#define CPUTYPE_Z80 365
#define STRING 366
#define FILENAME 367
#define R_O_L 368
#define OPCODE 369
#define LABEL 370
#define BANKNAME 371
#define CPUTYPE 372
#define REGISTER 373
#define COMPARE_OP 374
#define RADIX_TYPE 375
#define INPUT_SPEC 376
#define CMD_CHECKPT_ON 377
#define CMD_CHECKPT_OFF 378
#define TOGGLE 379




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
#line 411 "mon_parse.c"
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
#line 432 "mon_parse.c"

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
#define YYFINAL  222
#define YYLAST   1046

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  131
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  46
/* YYNRULES -- Number of rules. */
#define YYNRULES  206
/* YYNRULES -- Number of states. */
#define YYNSTATES  402

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   379

#define YYTRANSLATE(X) \
  ((unsigned)(X) <= YYMAXUTOK ? yytranslate[X] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     129,   130,   127,   125,     2,   126,     2,   128,     2,     2,
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
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124
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
     454,   456,   458,   460,   464,   468,   472,   476,   480,   484,
     486,   490,   494,   498,   502,   504,   506,   508,   511,   513,
     515,   517,   519,   521,   523,   525,   527,   529,   531,   533,
     535,   537,   539,   541,   543,   545,   549,   553,   556,   559,
     561,   563,   566,   568,   572,   576,   580,   586,   592,   596,
     600,   604,   608,   612,   616,   622,   628,   634,   640,   641,
     643,   645,   647,   649,   651,   653,   655,   657,   659,   661,
     663,   665,   667,   669,   671,   673,   675
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     132,     0,    -1,   133,    -1,   174,    12,    -1,    12,    -1,
     135,    -1,   133,   135,    -1,    19,    -1,    12,    -1,     1,
      -1,   136,    -1,   138,    -1,   141,    -1,   139,    -1,   142,
      -1,   143,    -1,   144,    -1,   145,    -1,   146,    -1,   147,
      -1,   148,    -1,    13,    -1,    64,   160,   150,   134,    -1,
      35,   159,   134,    -1,    43,   134,    -1,    83,   117,   134,
      -1,    23,   134,    -1,    46,   151,   134,    -1,    47,   151,
     134,    -1,    55,   156,   134,    -1,    54,   156,   134,    -1,
      26,   156,   134,    -1,    27,   156,   134,    -1,    81,   134,
      -1,   137,    -1,    36,   134,    -1,    36,   161,   134,    -1,
      36,   154,   134,    -1,    65,   160,   151,   134,    -1,    66,
     160,   151,   134,    -1,    67,   159,   115,   134,    -1,    68,
     160,   115,   134,    -1,    69,   160,   134,    -1,    -1,    52,
     159,   140,   175,   134,    -1,    52,   159,   134,    -1,    53,
     159,   158,   134,    -1,    53,   134,    -1,    34,   159,   159,
     159,   134,    -1,    45,   159,   159,   159,   134,    -1,    33,
     159,   159,   167,   134,    -1,    32,   159,   159,   167,   134,
      -1,    40,   120,   159,   158,   134,    -1,    40,   159,   158,
     134,    -1,    40,   134,    -1,    73,   159,   158,   134,    -1,
      73,   134,    -1,    74,   159,   158,   134,    -1,    74,   134,
      -1,    75,   159,   158,   134,    -1,    75,   134,    -1,    41,
     159,   158,   134,    -1,    82,   159,   158,   134,    -1,    41,
     159,   158,    15,   165,   134,    -1,    59,   152,   159,   158,
     134,    -1,    42,   159,   158,   134,    -1,    41,   134,    -1,
      82,   134,    -1,    42,   134,    -1,    59,   134,    -1,   122,
     157,   134,    -1,   123,   157,   134,    -1,    31,   157,   156,
     134,    -1,    49,   157,   134,    -1,    49,   134,    -1,    50,
     157,    15,   165,   134,    -1,    51,   157,   111,   134,    -1,
      51,   157,     1,   134,    -1,    22,   124,   134,    -1,    22,
     134,    -1,    39,   120,   134,    -1,    39,   134,    -1,    57,
     161,   134,    -1,    62,   134,    -1,    48,   134,    -1,    60,
     149,   134,    -1,    56,   164,   134,    -1,    58,   134,    -1,
      58,   149,   134,    -1,    61,   149,   134,    -1,     7,   164,
     134,    -1,    63,   149,   134,    -1,    78,   149,   134,    -1,
      28,   151,   164,   158,   134,    -1,    79,   151,   164,   158,
     134,    -1,    29,   151,   164,   159,   159,   134,    -1,    80,
     151,   164,   159,   159,   134,    -1,    30,   151,   164,   159,
     134,    -1,    24,   164,   164,   158,   134,    -1,    25,   164,
     164,   159,   134,    -1,    70,   151,   134,    -1,    71,   134,
      -1,    72,   151,   134,    -1,    76,   159,   167,   134,    -1,
      77,   134,    -1,   113,    -1,   116,    -1,    -1,   112,    -1,
       1,    -1,    14,    -1,    -1,   118,    -1,   161,   118,    -1,
     154,    20,   155,    -1,   155,    -1,   153,    21,   172,    -1,
     164,    -1,    -1,   170,    -1,     1,    -1,   159,    -1,    -1,
     162,    -1,   161,   162,    -1,   115,    -1,   161,    -1,    -1,
      16,    -1,    17,    -1,    18,    -1,   163,    -1,   172,    -1,
     164,   125,   164,    -1,   164,   126,   164,    -1,   164,   127,
     164,    -1,   164,   128,   164,    -1,   129,   164,   130,    -1,
     129,   164,     1,    -1,   169,    -1,   165,   119,   165,    -1,
     165,   119,     1,    -1,    84,   165,    85,    -1,    84,   165,
       1,    -1,   166,    -1,   153,    -1,   172,    -1,   167,   168,
      -1,   168,    -1,   172,    -1,   111,    -1,   172,    -1,   153,
      -1,     4,    -1,    11,    -1,    10,    -1,     9,    -1,    11,
      -1,    10,    -1,     9,    -1,     3,    -1,     4,    -1,     5,
      -1,     6,    -1,   171,    -1,   173,    91,   174,    -1,   174,
      91,   174,    -1,   174,    91,    -1,   114,   176,    -1,   174,
      -1,   173,    -1,    86,   172,    -1,   172,    -1,   172,    90,
      88,    -1,   172,    90,    89,    -1,    84,   172,    85,    -1,
      84,   172,    90,    88,    85,    -1,    84,   172,    85,    90,
      89,    -1,    84,    99,    85,    -1,    84,   100,    85,    -1,
      84,   101,    85,    -1,    84,   102,    85,    -1,    84,   103,
      85,    -1,    84,   104,    85,    -1,    84,   172,    85,    90,
      87,    -1,    84,   172,    85,    90,   101,    -1,    84,   172,
      85,    90,   102,    -1,    84,   172,    85,    90,   103,    -1,
      -1,    87,    -1,    92,    -1,    93,    -1,    94,    -1,    95,
      -1,    96,    -1,   105,    -1,   107,    -1,    97,    -1,   106,
      -1,   108,    -1,    98,    -1,    99,    -1,   100,    -1,   101,
      -1,   102,    -1,   103,    -1,   104,    -1
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
     476,   483,   485,   487,   488,   489,   490,   491,   492,   493,
     496,   501,   503,   505,   507,   511,   516,   523,   524,   527,
     528,   531,   532,   535,   536,   537,   538,   541,   542,   543,
     546,   547,   548,   549,   550,   553,   554,   555,   558,   568,
     569,   572,   576,   581,   586,   591,   593,   595,   597,   598,
     599,   600,   601,   602,   603,   605,   607,   609,   611,   612,
     613,   614,   615,   616,   617,   618,   619,   620,   621,   622,
     623,   624,   625,   626,   627,   628,   629
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
  "MEM_DISK8", "MEM_DISK9", "CMD_SEP", "REG_ASGN_SEP", "EQUALS", 
  "CMD_SIDEFX", "CMD_RETURN", "CMD_BLOCK_READ", "CMD_BLOCK_WRITE", 
  "CMD_UP", "CMD_DOWN", "CMD_LOAD", "CMD_SAVE", "CMD_VERIFY", 
  "CMD_IGNORE", "CMD_HUNT", "CMD_FILL", "CMD_MOVE", "CMD_GOTO", 
  "CMD_REGISTERS", "CMD_READSPACE", "CMD_WRITESPACE", "CMD_RADIX", 
  "CMD_MEM_DISPLAY", "CMD_BREAK", "CMD_TRACE", "CMD_IO", "CMD_BRMON", 
  "CMD_COMPARE", "CMD_DUMP", "CMD_UNDUMP", "CMD_EXIT", "CMD_DELETE", 
  "CMD_CONDITION", "CMD_COMMAND", "CMD_ASSEMBLE", "CMD_DISASSEMBLE", 
  "CMD_NEXT", "CMD_STEP", "CMD_PRINT", "CMD_DEVICE", "CMD_HELP", 
  "CMD_WATCH", "CMD_DISK", "CMD_SYSTEM", "CMD_QUIT", "CMD_CHDIR", 
  "CMD_BANK", "CMD_LOAD_LABELS", "CMD_SAVE_LABELS", "CMD_ADD_LABEL", 
  "CMD_DEL_LABEL", "CMD_SHOW_LABELS", "CMD_RECORD", "CMD_STOP", 
  "CMD_PLAYBACK", "CMD_CHAR_DISPLAY", "CMD_SPRITE_DISPLAY", 
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
     375,   376,   377,   378,   379,    43,    45,    42,    47,    40,
      41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   131,   132,   132,   132,   133,   133,   134,   134,   134,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   137,   137,   137,   138,   138,
     138,   138,   138,   140,   139,   139,   139,   139,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   142,   142,   142,   142,   142,   142,   142,   142,   142,
     143,   143,   143,   143,   143,   143,   143,   143,   144,   144,
     144,   144,   144,   144,   144,   145,   145,   145,   145,   145,
     145,   145,   145,   146,   146,   146,   146,   146,   146,   146,
     147,   147,   147,   148,   148,   149,   150,   150,   151,   151,
     152,   152,   153,   153,   154,   154,   155,   156,   156,   157,
     157,   158,   158,   159,   159,   159,   160,   160,   161,   161,
     161,   162,   163,   164,   164,   164,   164,   164,   164,   164,
     165,   165,   165,   165,   165,   166,   166,   167,   167,   168,
     168,   169,   169,   170,   170,   170,   170,   171,   171,   171,
     172,   172,   172,   172,   172,   173,   173,   173,   174,   175,
     175,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176
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
       1,     1,     1,     3,     3,     3,     3,     3,     3,     1,
       3,     3,     3,     3,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     2,     2,     1,
       1,     2,     1,     3,     3,     3,     5,     5,     3,     3,
       3,     3,     3,     3,     5,     5,     5,     5,     0,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1
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
       0,     0,     0,   188,     0,     0,     0,     2,     5,    10,
      34,    11,    13,    12,    14,    15,    16,    17,    18,    19,
      20,     0,   160,   161,   162,   163,   159,   158,   157,   128,
     129,   130,   112,     0,   152,     0,     0,   139,   164,   151,
       9,     8,     7,     0,    79,    26,     0,     0,     0,   117,
       0,   109,   108,     0,     0,     0,   120,   153,   156,   155,
     154,   118,   119,   125,     0,     0,   123,   131,   132,     0,
       0,     0,    35,     0,     0,   115,     0,     0,    81,     0,
      54,   122,    66,   122,    68,   122,    24,     0,     0,     0,
      84,     9,    74,     0,     0,     0,     0,    47,   122,     0,
       0,     0,     0,   105,    87,     0,   110,    69,     0,     0,
       0,    83,     0,   107,   126,     0,     0,     0,     0,     0,
       0,   101,     0,    56,   122,    58,   122,    60,   122,     0,
     104,     0,     0,     0,    33,    67,   122,     0,     0,     0,
     189,   190,   191,   192,   193,   194,   197,   200,   201,   202,
     203,   204,   205,   206,   195,   198,   196,   199,   172,   168,
       0,     0,     1,     6,     3,     0,   113,     0,     0,     0,
       0,    90,    78,   122,     0,    31,    32,   122,     0,     0,
       0,     0,   124,     0,     0,    23,     0,     0,    37,    36,
      80,   122,     0,   121,     0,     0,     0,    27,    28,    73,
       0,     0,     0,    45,     0,     0,    30,    29,    86,    82,
      88,   122,    85,    89,    91,   106,     0,     0,     0,     0,
       0,    42,   100,   102,     0,     0,     0,   150,     0,   148,
     149,    92,   122,     0,     0,    25,     0,     0,     0,     0,
       0,     0,     0,   171,     0,    70,    71,   138,   137,   133,
     134,   135,   136,     0,     0,     0,     0,     0,    72,     0,
       0,     0,   116,   114,     0,    53,     0,    61,    65,     0,
       0,   145,     0,   144,   146,    77,    76,   170,   169,     0,
      46,     0,    22,    38,    39,    40,    41,    55,    57,    59,
     103,   147,     0,     0,    62,   178,   179,   180,   181,   182,
     183,   175,     0,   173,   174,    98,    99,    93,     0,    97,
      51,    50,    48,    52,     0,    49,     0,     0,    75,     0,
     167,    44,    64,    94,     0,     0,     0,    95,    63,   143,
     142,   141,   140,   165,   166,    96,   184,   177,   185,   186,
     187,   176
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    66,    67,   104,    68,    69,    70,    71,    72,   264,
      73,    74,    75,    76,    77,    78,    79,    80,   165,   276,
     113,   168,    94,   134,   135,   108,   121,   252,   253,   173,
     125,   126,   127,   109,   332,   333,   288,   289,    97,   122,
      98,   128,   337,    81,   339,   219
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -303
static const short yypact[] =
{
     725,   340,  -303,  -303,    18,   133,   340,   340,   340,   340,
       3,     3,     3,   258,   526,   526,   526,   526,   275,    42,
     472,   613,   613,   133,   526,     3,     3,   133,   319,   258,
     258,   526,   613,   340,   340,   340,    55,    95,   585,   -73,
     -73,   133,   -73,    55,    55,    55,   526,    55,    55,     3,
     133,     3,   613,   613,   613,   526,   133,   -73,     3,     3,
     133,   613,   -75,   832,   258,   258,    44,   827,  -303,  -303,
    -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,
    -303,    38,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,
    -303,  -303,  -303,   340,  -303,   -53,    45,  -303,  -303,  -303,
    -303,  -303,  -303,   133,  -303,  -303,    77,    77,   133,   -26,
     133,  -303,  -303,   340,   340,   340,  -303,  -303,  -303,  -303,
    -303,   340,  -303,  -303,   526,   444,  -303,  -303,  -303,   526,
     526,   133,  -303,    32,   108,  -303,    65,   133,  -303,   526,
    -303,   526,  -303,   526,  -303,   526,  -303,   526,   133,   133,
    -303,   149,  -303,   133,    61,     1,    73,  -303,   526,   133,
     133,    45,   133,  -303,  -303,   133,  -303,  -303,   526,   133,
     133,  -303,   133,   -27,  -303,     3,     3,   -11,    -7,   133,
     133,  -303,   133,  -303,   526,  -303,   526,  -303,   526,   712,
    -303,   133,   340,   340,  -303,  -303,   526,   133,   942,   444,
    -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,
    -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,    16,  -303,
     133,   133,  -303,  -303,  -303,    11,  -303,   340,   340,   340,
     340,  -303,  -303,   399,   399,  -303,  -303,   399,   399,   399,
     133,   712,  -303,   712,   526,  -303,   444,   164,  -303,  -303,
    -303,   526,   133,  -303,   134,   133,   526,  -303,  -303,  -303,
     549,   133,   133,  -303,     2,   133,  -303,  -303,  -303,  -303,
    -303,   526,  -303,  -303,  -303,  -303,   133,   133,   133,   133,
     133,  -303,  -303,  -303,   133,   133,   133,  -303,   701,  -303,
    -303,  -303,   399,   399,   133,  -303,    36,    48,    62,    78,
      81,    84,   -82,  -303,   -10,  -303,  -303,  -303,  -303,   -65,
     -65,  -303,  -303,   133,   133,   133,   526,   133,  -303,   701,
     701,   133,  -303,  -303,   133,  -303,   549,  -303,  -303,   133,
     549,  -303,    40,  -303,  -303,  -303,  -303,    34,    39,   133,
    -303,   133,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,
    -303,  -303,   133,   526,  -303,  -303,  -303,  -303,  -303,  -303,
    -303,    50,    70,  -303,  -303,  -303,  -303,  -303,   133,  -303,
    -303,  -303,  -303,  -303,    40,  -303,     5,   491,  -303,     2,
       2,  -303,  -303,  -303,   133,   141,    94,  -303,  -303,  -303,
    -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,
    -303,  -303
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -303,  -303,  -303,    -5,   121,  -303,  -303,  -303,  -303,  -303,
    -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,   197,  -303,
       9,  -303,    -9,  -303,   -58,    -8,   181,    33,   410,   328,
     353,    69,  -303,     4,  -302,  -303,  -212,  -250,  -303,  -303,
    -303,   192,  -303,  -257,  -303,  -303
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, parse error.  */
#define YYTABLE_NINF -121
static const short yytable[] =
{
     105,   110,   261,   361,   111,    96,   389,   338,   362,   133,
     106,   107,   307,   132,   138,   140,   142,   144,   146,   100,
     114,   115,   150,   152,   374,   159,   160,   157,   376,   319,
     101,   320,   164,   167,   148,   149,   171,   102,   351,   161,
     163,   100,   197,   100,   222,   181,   100,   183,   185,   187,
     224,   190,   101,   246,   101,   194,   195,   101,   180,   102,
     182,   102,   229,   230,   102,   226,   100,   192,   193,   351,
     351,    89,    90,    91,   100,   392,   260,   101,   363,   364,
      82,    83,    84,    85,   102,   101,    86,    87,    88,   275,
     390,   231,   102,    89,    90,    91,   100,   225,   232,   227,
     228,   229,   230,   235,   279,   236,   304,   101,   280,   100,
     233,   234,   262,   240,   102,   112,    63,   237,   238,   239,
     101,   355,   393,   394,   377,   379,   245,   102,   247,   248,
     380,   249,   250,   356,   100,   100,   227,   228,   229,   230,
     385,   308,   103,   257,   258,   101,   101,   357,   259,   326,
    -120,   263,   102,   102,   266,   267,   268,   269,   386,   377,
     270,  -120,   137,   358,   272,   273,   359,   274,  -120,   360,
     227,   228,   229,   230,   281,   282,   254,   283,   255,   401,
      89,    90,    91,   226,   277,   278,   291,   -43,   223,   323,
       0,   265,   295,    99,   242,    92,   292,   293,    99,    99,
      99,    99,   227,   228,   229,   230,    93,     0,   163,   153,
     154,   155,     0,     0,     0,   305,   306,   284,     0,   285,
       0,   286,     0,     0,     0,    99,    99,    99,   396,   294,
     397,   309,   310,   311,   312,   318,   169,   170,   133,   172,
       0,     0,   398,   399,   400,   220,   221,   325,     0,   327,
     328,   331,     0,     0,   191,   218,   335,   336,     0,   116,
     340,     0,   117,     0,     0,     0,   313,   118,   119,   120,
     315,   342,   343,   344,   345,   346,   100,     0,     0,   347,
     348,   349,    92,   350,   324,    99,     0,   101,     0,   354,
       0,    89,    90,    91,   102,     0,     0,     0,    99,    99,
       0,     0,     0,     0,   341,    99,    99,    99,   365,   366,
     367,     0,   369,    99,   370,   371,   372,   331,     0,   373,
     151,   331,     0,   117,   375,   352,     0,   378,   118,   119,
     120,   101,     0,     0,   381,     0,   382,     0,   102,     0,
       0,     0,     0,    82,    83,    84,    85,   383,     0,    86,
      87,    88,     0,     0,    95,     0,    89,    90,    91,    95,
      95,    95,    95,   387,     0,     0,     0,     0,   331,   388,
       0,   136,   175,   176,     0,   178,   179,     0,     0,   395,
       0,   290,     0,     0,    99,    99,    95,    95,    95,   162,
     302,   303,     0,    92,     0,     0,   174,   174,   174,     0,
     174,   174,    82,    83,    84,    85,     0,     0,    86,    87,
      88,     0,     0,     0,     0,    89,    90,    91,     0,    99,
      99,    99,    99,     0,   124,   129,   130,   131,     0,     0,
     141,   143,   145,   290,   147,   290,     0,     0,   322,     0,
       0,   156,   158,     0,     0,     0,    95,    82,    83,    84,
      85,     0,   334,    86,    87,    88,   177,     0,    92,    95,
      95,     0,   184,   186,   188,   189,    95,    95,    95,    93,
       0,   196,     0,   100,    95,    82,    83,    84,    85,     0,
     290,    86,    87,    88,   101,     0,     0,     0,    89,    90,
      91,   102,   391,     0,    82,    83,    84,    85,     0,     0,
      86,    87,    88,     0,     0,     0,     0,    89,    90,    91,
       0,   290,   290,     0,   123,     0,     0,     0,   334,     0,
       0,     0,   334,     0,   227,   228,   229,   230,     0,    82,
      83,    84,    85,     0,   241,    86,    87,    88,     0,   243,
     244,     0,    89,    90,    91,    95,    95,     0,     0,   251,
       0,     0,    82,    83,    84,    85,     0,   256,    86,    87,
      88,     0,     0,     0,     0,    89,    90,    91,     0,   334,
       0,     0,     0,     0,     0,   330,     0,     0,   271,     0,
      95,    95,    95,    95,     0,     0,   100,   123,  -111,  -111,
    -111,  -111,   139,     0,  -111,  -111,  -111,   101,     0,   166,
      95,  -111,  -111,  -111,   102,     0,     0,     0,     0,    92,
       0,     0,     0,    95,   100,     0,    82,    83,    84,    85,
       0,     0,    86,    87,    88,   101,     0,     0,     0,    89,
      90,    91,   102,   330,     0,     0,     0,     0,     0,     0,
       0,   123,     0,     0,   314,     0,     0,     0,   316,   317,
       0,     0,     0,     0,   321,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   329,    92,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
       0,     0,     0,    95,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -111,     0,   100,   353,    82,    83,    84,    85,     0,     0,
      86,    87,    88,   101,     0,    82,    83,    84,    85,     0,
     102,    86,    87,    88,     0,     0,   368,     0,   123,     0,
      95,     0,     1,     0,     0,     0,     0,     2,     3,     0,
       0,     0,     0,     0,     0,     0,     0,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,     0,   384,    19,    20,    21,    22,    23,     0,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,     0,
       0,     0,   287,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   287,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     1,    82,    83,    84,    85,    63,
       3,    86,    87,    88,     0,     0,     0,    64,    65,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,     0,     0,    19,    20,    21,    22,
      23,     0,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,     0,     0,     0,     0,     0,   198,     0,   199,   200,
       0,     0,     0,     0,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,     0,     0,     0,     0,    82,    83,    84,    85,    64,
      65,    86,    87,    88,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   296,   297,   298,   299,   300,   301
};

static const short yycheck[] =
{
       5,     9,     1,    85,     1,     1,     1,   264,    90,    18,
       6,     7,     1,    18,    19,    20,    21,    22,    23,     1,
      11,    12,    27,    28,   326,    33,    34,    32,   330,   241,
      12,   243,    37,    38,    25,    26,    41,    19,   288,    35,
     113,     1,   117,     1,     0,    50,     1,    52,    53,    54,
      12,    56,    12,    21,    12,    60,    61,    12,    49,    19,
      51,    19,   127,   128,    19,   118,     1,    58,    59,   319,
     320,    16,    17,    18,     1,   377,    15,    12,    88,    89,
       3,     4,     5,     6,    19,    12,     9,    10,    11,   116,
      85,    96,    19,    16,    17,    18,     1,    93,   103,   125,
     126,   127,   128,   108,   115,   110,    90,    12,   115,     1,
     106,   107,   111,   121,    19,   112,   114,   113,   114,   115,
      12,    85,   379,   380,   119,    91,   131,    19,    20,   134,
      91,   136,   137,    85,     1,     1,   125,   126,   127,   128,
      90,   130,   124,   148,   149,    12,    12,    85,   153,    15,
       1,   156,    19,    19,   159,   160,   161,   162,    88,   119,
     165,    12,   120,    85,   169,   170,    85,   172,    19,    85,
     125,   126,   127,   128,   179,   180,   143,   182,   145,    85,
      16,    17,    18,   118,   175,   176,   191,   114,    67,   247,
      -1,   158,   197,     1,   125,   118,   192,   193,     6,     7,
       8,     9,   125,   126,   127,   128,   129,    -1,   113,    28,
      29,    30,    -1,    -1,    -1,   220,   221,   184,    -1,   186,
      -1,   188,    -1,    -1,    -1,    33,    34,    35,    87,   196,
      89,   227,   228,   229,   230,   240,    39,    40,   247,    42,
      -1,    -1,   101,   102,   103,    64,    65,   252,    -1,   254,
     255,   260,    -1,    -1,    57,    63,   261,   262,    -1,     1,
     265,    -1,     4,    -1,    -1,    -1,   233,     9,    10,    11,
     237,   276,   277,   278,   279,   280,     1,    -1,    -1,   284,
     285,   286,   118,   288,   251,    93,    -1,    12,    -1,   294,
      -1,    16,    17,    18,    19,    -1,    -1,    -1,   106,   107,
      -1,    -1,    -1,    -1,   271,   113,   114,   115,   313,   314,
     315,    -1,   317,   121,   319,   320,   321,   326,    -1,   324,
       1,   330,    -1,     4,   329,   292,    -1,   332,     9,    10,
      11,    12,    -1,    -1,   339,    -1,   341,    -1,    19,    -1,
      -1,    -1,    -1,     3,     4,     5,     6,   352,    -1,     9,
      10,    11,    -1,    -1,     1,    -1,    16,    17,    18,     6,
       7,     8,     9,   368,    -1,    -1,    -1,    -1,   377,   374,
      -1,    18,    44,    45,    -1,    47,    48,    -1,    -1,   384,
      -1,   189,    -1,    -1,   192,   193,    33,    34,    35,    36,
     198,   199,    -1,   118,    -1,    -1,    43,    44,    45,    -1,
      47,    48,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    -1,    -1,    -1,    -1,    16,    17,    18,    -1,   227,
     228,   229,   230,    -1,    14,    15,    16,    17,    -1,    -1,
      20,    21,    22,   241,    24,   243,    -1,    -1,   246,    -1,
      -1,    31,    32,    -1,    -1,    -1,    93,     3,     4,     5,
       6,    -1,   260,     9,    10,    11,    46,    -1,   118,   106,
     107,    -1,    52,    53,    54,    55,   113,   114,   115,   129,
      -1,    61,    -1,     1,   121,     3,     4,     5,     6,    -1,
     288,     9,    10,    11,    12,    -1,    -1,    -1,    16,    17,
      18,    19,     1,    -1,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    -1,    -1,    -1,    -1,    16,    17,    18,
      -1,   319,   320,    -1,   115,    -1,    -1,    -1,   326,    -1,
      -1,    -1,   330,    -1,   125,   126,   127,   128,    -1,     3,
       4,     5,     6,    -1,   124,     9,    10,    11,    -1,   129,
     130,    -1,    16,    17,    18,   192,   193,    -1,    -1,   139,
      -1,    -1,     3,     4,     5,     6,    -1,   147,     9,    10,
      11,    -1,    -1,    -1,    -1,    16,    17,    18,    -1,   377,
      -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,   168,    -1,
     227,   228,   229,   230,    -1,    -1,     1,   115,     3,     4,
       5,     6,   120,    -1,     9,    10,    11,    12,    -1,    14,
     247,    16,    17,    18,    19,    -1,    -1,    -1,    -1,   118,
      -1,    -1,    -1,   260,     1,    -1,     3,     4,     5,     6,
      -1,    -1,     9,    10,    11,    12,    -1,    -1,    -1,    16,
      17,    18,    19,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   115,    -1,    -1,   234,    -1,    -1,    -1,   238,   239,
      -1,    -1,    -1,    -1,   244,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   256,   118,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   326,
      -1,    -1,    -1,   330,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     115,    -1,     1,   293,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    12,    -1,     3,     4,     5,     6,    -1,
      19,     9,    10,    11,    -1,    -1,   316,    -1,   115,    -1,
     377,    -1,     7,    -1,    -1,    -1,    -1,    12,    13,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    -1,   353,    39,    40,    41,    42,    43,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    -1,
      -1,    -1,   111,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   111,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     7,     3,     4,     5,     6,   114,
      13,     9,    10,    11,    -1,    -1,    -1,   122,   123,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    -1,    -1,    39,    40,    41,    42,
      43,    -1,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    -1,    -1,    -1,    -1,    -1,    84,    -1,    86,    87,
      -1,    -1,    -1,    -1,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,    -1,    -1,    -1,    -1,     3,     4,     5,     6,   122,
     123,     9,    10,    11,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    99,   100,   101,   102,   103,   104
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     7,    12,    13,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    39,
      40,    41,    42,    43,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,   114,   122,   123,   132,   133,   135,   136,
     137,   138,   139,   141,   142,   143,   144,   145,   146,   147,
     148,   174,     3,     4,     5,     6,     9,    10,    11,    16,
      17,    18,   118,   129,   153,   161,   164,   169,   171,   172,
       1,    12,    19,   124,   134,   134,   164,   164,   156,   164,
     156,     1,   112,   151,   151,   151,     1,     4,     9,    10,
      11,   157,   170,   115,   159,   161,   162,   163,   172,   159,
     159,   159,   134,   153,   154,   155,   161,   120,   134,   120,
     134,   159,   134,   159,   134,   159,   134,   159,   151,   151,
     134,     1,   134,   157,   157,   157,   159,   134,   159,   156,
     156,   164,   161,   113,   134,   149,    14,   134,   152,   149,
     149,   134,   149,   160,   161,   160,   160,   159,   160,   160,
     151,   134,   151,   134,   159,   134,   159,   134,   159,   159,
     134,   149,   151,   151,   134,   134,   159,   117,    84,    86,
      87,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   172,   176,
     157,   157,     0,   135,    12,   164,   118,   125,   126,   127,
     128,   134,   134,   164,   164,   134,   134,   164,   164,   164,
     156,   159,   162,   159,   159,   134,    21,    20,   134,   134,
     134,   159,   158,   159,   158,   158,   159,   134,   134,   134,
      15,     1,   111,   134,   140,   158,   134,   134,   134,   134,
     134,   159,   134,   134,   134,   116,   150,   151,   151,   115,
     115,   134,   134,   134,   158,   158,   158,   111,   167,   168,
     172,   134,   164,   164,   158,   134,    99,   100,   101,   102,
     103,   104,   172,   172,    90,   134,   134,     1,   130,   164,
     164,   164,   164,   158,   159,   158,   159,   159,   134,   167,
     167,   159,   172,   155,   158,   134,    15,   134,   134,   159,
      84,   153,   165,   166,   172,   134,   134,   173,   174,   175,
     134,   158,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   168,   158,   159,   134,    85,    85,    85,    85,    85,
      85,    85,    90,    88,    89,   134,   134,   134,   159,   134,
     134,   134,   134,   134,   165,   134,   165,   119,   134,    91,
      91,   134,   134,   134,   159,    90,    88,   134,   134,     1,
      85,     1,   165,   174,   174,   134,    87,    89,   101,   102,
     103,    85
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
#line 483 "mon_parse.y"
    { yyval.i = yyvsp[0].i; if (!CHECK_ADDR(yyvsp[0].i)) return ERR_ADDR_TOO_BIG; }
    break;

  case 132:
#line 485 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 133:
#line 487 "mon_parse.y"
    { yyval.i = yyvsp[-2].i + yyvsp[0].i; }
    break;

  case 134:
#line 488 "mon_parse.y"
    { yyval.i = yyvsp[-2].i - yyvsp[0].i; }
    break;

  case 135:
#line 489 "mon_parse.y"
    { yyval.i = yyvsp[-2].i * yyvsp[0].i; }
    break;

  case 136:
#line 490 "mon_parse.y"
    { yyval.i = (yyvsp[0].i) ? (yyvsp[-2].i / yyvsp[0].i) : 1; }
    break;

  case 137:
#line 491 "mon_parse.y"
    { yyval.i = yyvsp[-1].i; }
    break;

  case 138:
#line 492 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 139:
#line 493 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 140:
#line 497 "mon_parse.y"
    {
               yyval.cond_node = new_cond; yyval.cond_node->is_parenthized = FALSE;
               yyval.cond_node->child1 = yyvsp[-2].cond_node; yyval.cond_node->child2 = yyvsp[0].cond_node; yyval.cond_node->operation = yyvsp[-1].cond_op;
           }
    break;

  case 141:
#line 502 "mon_parse.y"
    { return ERR_INCOMPLETE_COMPARE_OP; }
    break;

  case 142:
#line 504 "mon_parse.y"
    { yyval.cond_node = yyvsp[-1].cond_node; yyval.cond_node->is_parenthized = TRUE; }
    break;

  case 143:
#line 506 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 144:
#line 508 "mon_parse.y"
    { yyval.cond_node = yyvsp[0].cond_node; }
    break;

  case 145:
#line 511 "mon_parse.y"
    { yyval.cond_node = new_cond;
                            yyval.cond_node->operation = e_INV;
                            yyval.cond_node->is_parenthized = FALSE;
                            yyval.cond_node->reg_num = yyvsp[0].i; yyval.cond_node->is_reg = TRUE;
                          }
    break;

  case 146:
#line 516 "mon_parse.y"
    { yyval.cond_node = new_cond;
                            yyval.cond_node->operation = e_INV;
                            yyval.cond_node->is_parenthized = FALSE;
                            yyval.cond_node->value = yyvsp[0].i; yyval.cond_node->is_reg = FALSE;
                          }
    break;

  case 149:
#line 527 "mon_parse.y"
    { mon_add_number_to_buffer(yyvsp[0].i); }
    break;

  case 150:
#line 528 "mon_parse.y"
    { mon_add_string_to_buffer(yyvsp[0].str); }
    break;

  case 151:
#line 531 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 152:
#line 532 "mon_parse.y"
    { yyval.i = (monitor_cpu_type.mon_register_get_val)(reg_memspace(yyvsp[0].i), reg_regid(yyvsp[0].i)); }
    break;

  case 153:
#line 535 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 154:
#line 536 "mon_parse.y"
    { yyval.i = strtol(yyvsp[0].str, NULL, 10); }
    break;

  case 155:
#line 537 "mon_parse.y"
    { yyval.i = strtol(yyvsp[0].str, NULL, 10); }
    break;

  case 156:
#line 538 "mon_parse.y"
    { yyval.i = strtol(yyvsp[0].str, NULL, 10); }
    break;

  case 157:
#line 541 "mon_parse.y"
    { yyval.i = resolve_datatype(B_NUMBER,yyvsp[0].str); }
    break;

  case 158:
#line 542 "mon_parse.y"
    { yyval.i = resolve_datatype(O_NUMBER,yyvsp[0].str); }
    break;

  case 159:
#line 543 "mon_parse.y"
    { yyval.i = resolve_datatype(D_NUMBER,yyvsp[0].str); }
    break;

  case 160:
#line 546 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 161:
#line 547 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 162:
#line 548 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 163:
#line 549 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 164:
#line 550 "mon_parse.y"
    { yyval.i = yyvsp[0].i; }
    break;

  case 168:
#line 558 "mon_parse.y"
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

  case 170:
#line 569 "mon_parse.y"
    { asm_mode = 0; }
    break;

  case 171:
#line 572 "mon_parse.y"
    { if (yyvsp[0].i > 0xff)
                          yyval.i = join_ints(ASM_ADDR_MODE_IMMEDIATE_16,yyvsp[0].i);
                        else
                          yyval.i = join_ints(ASM_ADDR_MODE_IMMEDIATE,yyvsp[0].i); }
    break;

  case 172:
#line 576 "mon_parse.y"
    { if (yyvsp[0].i < 0x100)
               yyval.i = join_ints(ASM_ADDR_MODE_ZERO_PAGE,yyvsp[0].i);
             else
               yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE,yyvsp[0].i);
           }
    break;

  case 173:
#line 581 "mon_parse.y"
    { if (yyvsp[-2].i < 0x100)
                            yyval.i = join_ints(ASM_ADDR_MODE_ZERO_PAGE_X,yyvsp[-2].i);
                          else
                            yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE_X,yyvsp[-2].i);
                        }
    break;

  case 174:
#line 586 "mon_parse.y"
    { if (yyvsp[-2].i < 0x100)
                            yyval.i = join_ints(ASM_ADDR_MODE_ZERO_PAGE_Y,yyvsp[-2].i);
                          else
                            yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE_Y,yyvsp[-2].i);
                        }
    break;

  case 175:
#line 592 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_ABS_INDIRECT,yyvsp[-1].i); }
    break;

  case 176:
#line 594 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_INDIRECT_X,yyvsp[-3].i); }
    break;

  case 177:
#line 596 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_INDIRECT_Y,yyvsp[-3].i); }
    break;

  case 178:
#line 597 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IND_BC,0); }
    break;

  case 179:
#line 598 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IND_DE,0); }
    break;

  case 180:
#line 599 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IND_HL,0); }
    break;

  case 181:
#line 600 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IND_IX,0); }
    break;

  case 182:
#line 601 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IND_IY,0); }
    break;

  case 183:
#line 602 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IND_SP,0); }
    break;

  case 184:
#line 604 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE_A,yyvsp[-3].i); }
    break;

  case 185:
#line 606 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE_HL,yyvsp[-3].i); }
    break;

  case 186:
#line 608 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE_IX,yyvsp[-3].i); }
    break;

  case 187:
#line 610 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_ABSOLUTE_IY,yyvsp[-3].i); }
    break;

  case 188:
#line 611 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_IMPLIED,0); }
    break;

  case 189:
#line 612 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_ACCUMULATOR,0); }
    break;

  case 190:
#line 613 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_B,0); }
    break;

  case 191:
#line 614 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_C,0); }
    break;

  case 192:
#line 615 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_D,0); }
    break;

  case 193:
#line 616 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_E,0); }
    break;

  case 194:
#line 617 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_H,0); }
    break;

  case 195:
#line 618 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IXH,0); }
    break;

  case 196:
#line 619 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IYH,0); }
    break;

  case 197:
#line 620 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_L,0); }
    break;

  case 198:
#line 621 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IXL,0); }
    break;

  case 199:
#line 622 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IYL,0); }
    break;

  case 200:
#line 623 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_AF,0); }
    break;

  case 201:
#line 624 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_BC,0); }
    break;

  case 202:
#line 625 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_DE,0); }
    break;

  case 203:
#line 626 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_HL,0); }
    break;

  case 204:
#line 627 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IX,0); }
    break;

  case 205:
#line 628 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_IY,0); }
    break;

  case 206:
#line 629 "mon_parse.y"
    { yyval.i = join_ints(ASM_ADDR_MODE_REG_SP,0); }
    break;


    }

/* Line 1016 of /usr/share/bison/yacc.c.  */
#line 2742 "mon_parse.c"

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


#line 633 "mon_parse.y"


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


