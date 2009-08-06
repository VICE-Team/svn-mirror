/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

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
     H_RANGE_GUESS = 264,
     D_NUMBER_GUESS = 265,
     O_NUMBER_GUESS = 266,
     B_NUMBER_GUESS = 267,
     BAD_CMD = 268,
     MEM_OP = 269,
     IF = 270,
     MEM_COMP = 271,
     MEM_DISK8 = 272,
     MEM_DISK9 = 273,
     MEM_DISK10 = 274,
     MEM_DISK11 = 275,
     EQUALS = 276,
     TRAIL = 277,
     CMD_SEP = 278,
     LABEL_ASGN_COMMENT = 279,
     CMD_SIDEFX = 280,
     CMD_RETURN = 281,
     CMD_BLOCK_READ = 282,
     CMD_BLOCK_WRITE = 283,
     CMD_UP = 284,
     CMD_DOWN = 285,
     CMD_LOAD = 286,
     CMD_SAVE = 287,
     CMD_VERIFY = 288,
     CMD_IGNORE = 289,
     CMD_HUNT = 290,
     CMD_FILL = 291,
     CMD_MOVE = 292,
     CMD_GOTO = 293,
     CMD_REGISTERS = 294,
     CMD_READSPACE = 295,
     CMD_WRITESPACE = 296,
     CMD_RADIX = 297,
     CMD_MEM_DISPLAY = 298,
     CMD_BREAK = 299,
     CMD_TRACE = 300,
     CMD_IO = 301,
     CMD_BRMON = 302,
     CMD_COMPARE = 303,
     CMD_DUMP = 304,
     CMD_UNDUMP = 305,
     CMD_EXIT = 306,
     CMD_DELETE = 307,
     CMD_CONDITION = 308,
     CMD_COMMAND = 309,
     CMD_ASSEMBLE = 310,
     CMD_DISASSEMBLE = 311,
     CMD_NEXT = 312,
     CMD_STEP = 313,
     CMD_PRINT = 314,
     CMD_DEVICE = 315,
     CMD_HELP = 316,
     CMD_WATCH = 317,
     CMD_DISK = 318,
     CMD_SYSTEM = 319,
     CMD_QUIT = 320,
     CMD_CHDIR = 321,
     CMD_BANK = 322,
     CMD_LOAD_LABELS = 323,
     CMD_SAVE_LABELS = 324,
     CMD_ADD_LABEL = 325,
     CMD_DEL_LABEL = 326,
     CMD_SHOW_LABELS = 327,
     CMD_RECORD = 328,
     CMD_MON_STOP = 329,
     CMD_PLAYBACK = 330,
     CMD_CHAR_DISPLAY = 331,
     CMD_SPRITE_DISPLAY = 332,
     CMD_TEXT_DISPLAY = 333,
     CMD_SCREENCODE_DISPLAY = 334,
     CMD_ENTER_DATA = 335,
     CMD_ENTER_BIN_DATA = 336,
     CMD_KEYBUF = 337,
     CMD_BLOAD = 338,
     CMD_BSAVE = 339,
     CMD_SCREEN = 340,
     CMD_UNTIL = 341,
     CMD_CPU = 342,
     CMD_YYDEBUG = 343,
     CMD_BACKTRACE = 344,
     CMD_SCREENSHOT = 345,
     CMD_PWD = 346,
     CMD_DIR = 347,
     CMD_RESOURCE_GET = 348,
     CMD_RESOURCE_SET = 349,
     CMD_ATTACH = 350,
     CMD_DETACH = 351,
     CMD_MON_RESET = 352,
     CMD_TAPECTRL = 353,
     CMD_CARTFREEZE = 354,
     CMD_CPUHISTORY = 355,
     CMD_MEMMAPZAP = 356,
     CMD_MEMMAPSHOW = 357,
     CMD_MEMMAPSAVE = 358,
     CMD_COMMENT = 359,
     CMD_LABEL_ASGN = 360,
     L_PAREN = 361,
     R_PAREN = 362,
     ARG_IMMEDIATE = 363,
     REG_A = 364,
     REG_X = 365,
     REG_Y = 366,
     COMMA = 367,
     INST_SEP = 368,
     REG_B = 369,
     REG_C = 370,
     REG_D = 371,
     REG_E = 372,
     REG_H = 373,
     REG_L = 374,
     REG_AF = 375,
     REG_BC = 376,
     REG_DE = 377,
     REG_HL = 378,
     REG_IX = 379,
     REG_IY = 380,
     REG_SP = 381,
     REG_IXH = 382,
     REG_IXL = 383,
     REG_IYH = 384,
     REG_IYL = 385,
     STRING = 386,
     FILENAME = 387,
     R_O_L = 388,
     OPCODE = 389,
     LABEL = 390,
     BANKNAME = 391,
     CPUTYPE = 392,
     MON_REGISTER = 393,
     COMPARE_OP = 394,
     RADIX_TYPE = 395,
     INPUT_SPEC = 396,
     CMD_CHECKPT_ON = 397,
     CMD_CHECKPT_OFF = 398,
     TOGGLE = 399,
     MASK = 400
   };
#endif
/* Tokens.  */
#define H_NUMBER 258
#define D_NUMBER 259
#define O_NUMBER 260
#define B_NUMBER 261
#define CONVERT_OP 262
#define B_DATA 263
#define H_RANGE_GUESS 264
#define D_NUMBER_GUESS 265
#define O_NUMBER_GUESS 266
#define B_NUMBER_GUESS 267
#define BAD_CMD 268
#define MEM_OP 269
#define IF 270
#define MEM_COMP 271
#define MEM_DISK8 272
#define MEM_DISK9 273
#define MEM_DISK10 274
#define MEM_DISK11 275
#define EQUALS 276
#define TRAIL 277
#define CMD_SEP 278
#define LABEL_ASGN_COMMENT 279
#define CMD_SIDEFX 280
#define CMD_RETURN 281
#define CMD_BLOCK_READ 282
#define CMD_BLOCK_WRITE 283
#define CMD_UP 284
#define CMD_DOWN 285
#define CMD_LOAD 286
#define CMD_SAVE 287
#define CMD_VERIFY 288
#define CMD_IGNORE 289
#define CMD_HUNT 290
#define CMD_FILL 291
#define CMD_MOVE 292
#define CMD_GOTO 293
#define CMD_REGISTERS 294
#define CMD_READSPACE 295
#define CMD_WRITESPACE 296
#define CMD_RADIX 297
#define CMD_MEM_DISPLAY 298
#define CMD_BREAK 299
#define CMD_TRACE 300
#define CMD_IO 301
#define CMD_BRMON 302
#define CMD_COMPARE 303
#define CMD_DUMP 304
#define CMD_UNDUMP 305
#define CMD_EXIT 306
#define CMD_DELETE 307
#define CMD_CONDITION 308
#define CMD_COMMAND 309
#define CMD_ASSEMBLE 310
#define CMD_DISASSEMBLE 311
#define CMD_NEXT 312
#define CMD_STEP 313
#define CMD_PRINT 314
#define CMD_DEVICE 315
#define CMD_HELP 316
#define CMD_WATCH 317
#define CMD_DISK 318
#define CMD_SYSTEM 319
#define CMD_QUIT 320
#define CMD_CHDIR 321
#define CMD_BANK 322
#define CMD_LOAD_LABELS 323
#define CMD_SAVE_LABELS 324
#define CMD_ADD_LABEL 325
#define CMD_DEL_LABEL 326
#define CMD_SHOW_LABELS 327
#define CMD_RECORD 328
#define CMD_MON_STOP 329
#define CMD_PLAYBACK 330
#define CMD_CHAR_DISPLAY 331
#define CMD_SPRITE_DISPLAY 332
#define CMD_TEXT_DISPLAY 333
#define CMD_SCREENCODE_DISPLAY 334
#define CMD_ENTER_DATA 335
#define CMD_ENTER_BIN_DATA 336
#define CMD_KEYBUF 337
#define CMD_BLOAD 338
#define CMD_BSAVE 339
#define CMD_SCREEN 340
#define CMD_UNTIL 341
#define CMD_CPU 342
#define CMD_YYDEBUG 343
#define CMD_BACKTRACE 344
#define CMD_SCREENSHOT 345
#define CMD_PWD 346
#define CMD_DIR 347
#define CMD_RESOURCE_GET 348
#define CMD_RESOURCE_SET 349
#define CMD_ATTACH 350
#define CMD_DETACH 351
#define CMD_MON_RESET 352
#define CMD_TAPECTRL 353
#define CMD_CARTFREEZE 354
#define CMD_CPUHISTORY 355
#define CMD_MEMMAPZAP 356
#define CMD_MEMMAPSHOW 357
#define CMD_MEMMAPSAVE 358
#define CMD_COMMENT 359
#define CMD_LABEL_ASGN 360
#define L_PAREN 361
#define R_PAREN 362
#define ARG_IMMEDIATE 363
#define REG_A 364
#define REG_X 365
#define REG_Y 366
#define COMMA 367
#define INST_SEP 368
#define REG_B 369
#define REG_C 370
#define REG_D 371
#define REG_E 372
#define REG_H 373
#define REG_L 374
#define REG_AF 375
#define REG_BC 376
#define REG_DE 377
#define REG_HL 378
#define REG_IX 379
#define REG_IY 380
#define REG_SP 381
#define REG_IXH 382
#define REG_IXL 383
#define REG_IYH 384
#define REG_IYL 385
#define STRING 386
#define FILENAME 387
#define R_O_L 388
#define OPCODE 389
#define LABEL 390
#define BANKNAME 391
#define CPUTYPE 392
#define MON_REGISTER 393
#define COMPARE_OP 394
#define RADIX_TYPE 395
#define INPUT_SPEC 396
#define CMD_CHECKPT_ON 397
#define CMD_CHECKPT_OFF 398
#define TOGGLE 399
#define MASK 400




/* Copy the first part of user declarations.  */
#line 1 "mon_parse.y"

/* -*- C -*-
 *
 * mon_parse.y - Parser for the VICE built-in monitor.
 *
 * Written by
 *  Daniel Sladic <sladic@eecg.toronto.edu>
 *  Andreas Boose <viceteam@t-online.de>
 *  Thomas Giesel <skoe@directbox.com>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#ifndef MINIXVMD
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
#endif /* MINIXVMD */

/* SunOS 4.x specific stuff */
#if defined(sun) || defined(__sun)
#  if !defined(__SVR4) && !defined(__svr4__)
#    ifdef __sparc__
#      define YYFREE
#    endif
#  endif
#endif

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
static int resolve_datatype(unsigned guess_type, const char *num);
static int resolve_range(enum t_memspace memspace, MON_ADDR range[2], 
                         const char *num);
static void mon_quit(void);

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
#define ERR_EXPECT_DEVICE_NUM 14
#define ERR_EXPECT_ADDRESS 15

#define BAD_ADDR (new_addr(e_invalid_space, 0))
#define CHECK_ADDR(x) ((x) == addr_mask(x))

#define YYDEBUG 1



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

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 127 "mon_parse.y"
{
    MON_ADDR a;
    MON_ADDR range[2];
    int i;
    REG_ID reg;
    CONDITIONAL cond_op;
    cond_node_t *cond_node;
    RADIXTYPE rt;
    ACTION action;
    char *str;
}
/* Line 187 of yacc.c.  */
#line 524 "mon_parse.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 537 "mon_parse.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
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
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  283
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1591

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  152
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  51
/* YYNRULES -- Number of rules.  */
#define YYNRULES  262
/* YYNRULES -- Number of states.  */
#define YYNSTATES  517

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   400

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     150,   151,   148,   146,     2,   147,     2,   149,     2,     2,
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
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    15,    17,    19,
      21,    23,    25,    27,    29,    31,    33,    35,    37,    39,
      41,    43,    45,    47,    50,    54,    58,    64,    68,    71,
      74,    78,    81,    86,    89,    93,    97,   100,   105,   108,
     113,   116,   121,   124,   129,   132,   134,   137,   141,   145,
     151,   155,   161,   165,   171,   175,   181,   185,   188,   193,
     199,   200,   206,   210,   214,   217,   223,   229,   235,   241,
     247,   251,   254,   258,   261,   265,   268,   272,   275,   279,
     282,   285,   288,   293,   299,   305,   309,   313,   319,   325,
     329,   332,   335,   338,   341,   345,   349,   353,   359,   363,
     366,   372,   378,   383,   387,   390,   394,   397,   401,   404,
     407,   411,   415,   418,   422,   426,   430,   434,   438,   441,
     445,   448,   452,   458,   462,   467,   470,   475,   480,   483,
     487,   493,   499,   505,   509,   514,   520,   525,   531,   536,
     542,   548,   553,   557,   561,   564,   568,   573,   576,   579,
     581,   583,   584,   586,   588,   590,   592,   594,   595,   597,
     600,   604,   606,   610,   612,   614,   616,   618,   622,   624,
     628,   631,   632,   634,   638,   640,   642,   643,   645,   647,
     649,   651,   653,   655,   657,   661,   665,   669,   673,   677,
     681,   683,   687,   691,   695,   699,   701,   703,   705,   709,
     711,   713,   715,   718,   720,   722,   724,   726,   728,   730,
     732,   734,   736,   738,   740,   742,   744,   746,   748,   750,
     752,   754,   758,   762,   765,   768,   770,   772,   775,   777,
     781,   785,   789,   795,   801,   805,   809,   813,   817,   821,
     825,   831,   837,   843,   849,   850,   852,   854,   856,   858,
     860,   862,   864,   866,   868,   870,   872,   874,   876,   878,
     880,   882,   884
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     153,     0,    -1,   154,    -1,   200,    22,    -1,    22,    -1,
     156,    -1,   154,   156,    -1,    23,    -1,    22,    -1,     1,
      -1,   157,    -1,   159,    -1,   162,    -1,   160,    -1,   163,
      -1,   164,    -1,   165,    -1,   166,    -1,   167,    -1,   168,
      -1,   169,    -1,   170,    -1,    13,    -1,    67,   155,    -1,
      67,   185,   155,    -1,    67,   136,   155,    -1,    67,   185,
     184,   136,   155,    -1,    38,   183,   155,    -1,    46,   155,
      -1,    87,   155,    -1,    87,   137,   155,    -1,   100,   155,
      -1,   100,   184,   188,   155,    -1,    26,   155,    -1,    49,
     173,   155,    -1,    50,   173,   155,    -1,    58,   155,    -1,
      58,   184,   188,   155,    -1,    57,   155,    -1,    57,   184,
     188,   155,    -1,    29,   155,    -1,    29,   184,   188,   155,
      -1,    30,   155,    -1,    30,   184,   188,   155,    -1,    85,
     155,    -1,   158,    -1,    39,   155,    -1,    39,   185,   155,
      -1,    39,   177,   155,    -1,    68,   185,   184,   173,   155,
      -1,    68,   173,   155,    -1,    69,   185,   184,   173,   155,
      -1,    69,   173,   155,    -1,    70,   183,   184,   135,   155,
      -1,    71,   135,   155,    -1,    71,   185,   184,   135,   155,
      -1,    72,   185,   155,    -1,    72,   155,    -1,   105,    21,
     183,   155,    -1,   105,    21,   183,    24,   155,    -1,    -1,
      55,   183,   161,   201,   155,    -1,    55,   183,   155,    -1,
      56,   180,   155,    -1,    56,   155,    -1,    37,   181,   184,
     183,   155,    -1,    48,   181,   184,   183,   155,    -1,    36,
     181,   184,   191,   155,    -1,    35,   181,   184,   193,   155,
      -1,    43,   140,   184,   180,   155,    -1,    43,   180,   155,
      -1,    43,   155,    -1,    76,   180,   155,    -1,    76,   155,
      -1,    77,   180,   155,    -1,    77,   155,    -1,    78,   180,
     155,    -1,    78,   155,    -1,    79,   180,   155,    -1,    79,
     155,    -1,   101,   155,    -1,   102,   155,    -1,   102,   184,
     188,   155,    -1,   102,   184,   188,   180,   155,    -1,   103,
     173,   184,   188,   155,    -1,    44,   180,   155,    -1,    86,
     180,   155,    -1,    44,   180,    15,   189,   155,    -1,    62,
     175,   184,   180,   155,    -1,    45,   180,   155,    -1,    44,
     155,    -1,    86,   155,    -1,    45,   155,    -1,    62,   155,
      -1,   142,   179,   155,    -1,   143,   179,   155,    -1,    34,
     179,   155,    -1,    34,   179,   184,   188,   155,    -1,    52,
     179,   155,    -1,    52,   155,    -1,    53,   179,    15,   189,
     155,    -1,    54,   179,   184,   131,   155,    -1,    54,   179,
       1,   155,    -1,    25,   144,   155,    -1,    25,   155,    -1,
      42,   140,   155,    -1,    42,   155,    -1,    60,   185,   155,
      -1,    65,   155,    -1,    51,   155,    -1,    63,   171,   155,
      -1,    59,   188,   155,    -1,    61,   155,    -1,    61,   171,
     155,    -1,    64,   171,   155,    -1,     7,   188,   155,    -1,
      66,   171,   155,    -1,    82,   171,   155,    -1,    89,   155,
      -1,    92,   172,   155,    -1,    91,   155,    -1,    90,   173,
     155,    -1,    90,   173,   184,   188,   155,    -1,    93,   131,
     155,    -1,    94,   131,   131,   155,    -1,    97,   155,    -1,
      97,   184,   188,   155,    -1,    98,   184,   188,   155,    -1,
      99,   155,    -1,   104,   172,   155,    -1,    31,   173,   174,
     182,   155,    -1,    83,   173,   174,   182,   155,    -1,    32,
     173,   174,   181,   155,    -1,    32,   173,     1,    -1,    32,
     173,   174,     1,    -1,    84,   173,   174,   181,   155,    -1,
      84,   173,   174,     1,    -1,    33,   173,   174,   183,   155,
      -1,    33,   173,   174,     1,    -1,    27,   188,   188,   182,
     155,    -1,    28,   188,   188,   183,   155,    -1,    95,   173,
     188,   155,    -1,    96,   188,   155,    -1,    73,   173,   155,
      -1,    74,   155,    -1,    75,   173,   155,    -1,    80,   183,
     191,   155,    -1,    81,   155,    -1,    88,   155,    -1,   133,
      -1,   133,    -1,    -1,   132,    -1,     1,    -1,   188,    -1,
       1,    -1,    14,    -1,    -1,   138,    -1,   185,   138,    -1,
     177,   112,   178,    -1,   178,    -1,   176,    21,   198,    -1,
     196,    -1,     1,    -1,   181,    -1,   183,    -1,   183,   184,
     183,    -1,     9,    -1,   185,   184,     9,    -1,   184,   183,
      -1,    -1,   186,    -1,   185,   184,   186,    -1,   135,    -1,
     112,    -1,    -1,    16,    -1,    17,    -1,    18,    -1,    19,
      -1,    20,    -1,   187,    -1,   198,    -1,   188,   146,   188,
      -1,   188,   147,   188,    -1,   188,   148,   188,    -1,   188,
     149,   188,    -1,   150,   188,   151,    -1,   150,   188,     1,
      -1,   195,    -1,   189,   139,   189,    -1,   189,   139,     1,
      -1,   106,   189,   107,    -1,   106,   189,     1,    -1,   190,
      -1,   176,    -1,   198,    -1,   191,   184,   192,    -1,   192,
      -1,   198,    -1,   131,    -1,   193,   194,    -1,   194,    -1,
     198,    -1,   145,    -1,   131,    -1,   198,    -1,   176,    -1,
       4,    -1,    12,    -1,    11,    -1,    10,    -1,    12,    -1,
      11,    -1,    10,    -1,     3,    -1,     4,    -1,     5,    -1,
       6,    -1,   197,    -1,   199,   113,   200,    -1,   200,   113,
     200,    -1,   200,   113,    -1,   134,   202,    -1,   200,    -1,
     199,    -1,   108,   198,    -1,   198,    -1,   198,   112,   110,
      -1,   198,   112,   111,    -1,   106,   198,   107,    -1,   106,
     198,   112,   110,   107,    -1,   106,   198,   107,   112,   111,
      -1,   106,   121,   107,    -1,   106,   122,   107,    -1,   106,
     123,   107,    -1,   106,   124,   107,    -1,   106,   125,   107,
      -1,   106,   126,   107,    -1,   106,   198,   107,   112,   109,
      -1,   106,   198,   107,   112,   123,    -1,   106,   198,   107,
     112,   124,    -1,   106,   198,   107,   112,   125,    -1,    -1,
     109,    -1,   114,    -1,   115,    -1,   116,    -1,   117,    -1,
     118,    -1,   127,    -1,   129,    -1,   119,    -1,   128,    -1,
     130,    -1,   120,    -1,   121,    -1,   122,    -1,   123,    -1,
     124,    -1,   125,    -1,   126,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   185,   185,   186,   187,   190,   191,   194,   195,   196,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   214,   216,   218,   220,   222,   224,   226,
     228,   230,   232,   234,   236,   238,   240,   242,   244,   246,
     248,   250,   252,   254,   256,   258,   261,   263,   265,   268,
     273,   278,   280,   282,   284,   286,   288,   290,   292,   296,
     303,   302,   305,   307,   309,   313,   315,   317,   319,   321,
     323,   325,   327,   329,   331,   333,   335,   337,   339,   341,
     343,   345,   347,   349,   351,   355,   360,   365,   371,   377,
     382,   384,   386,   388,   393,   395,   397,   399,   401,   403,
     405,   407,   409,   413,   415,   420,   422,   440,   442,   444,
     448,   450,   452,   454,   456,   458,   460,   462,   464,   466,
     468,   470,   472,   474,   476,   478,   480,   482,   484,   486,
     490,   492,   494,   496,   498,   500,   502,   504,   506,   508,
     510,   512,   514,   518,   520,   522,   526,   528,   532,   536,
     539,   540,   543,   544,   547,   548,   551,   552,   555,   556,
     559,   560,   563,   567,   568,   571,   572,   575,   576,   578,
     582,   583,   586,   591,   596,   606,   607,   610,   611,   612,
     613,   614,   617,   619,   621,   622,   623,   624,   625,   626,
     627,   630,   635,   637,   639,   641,   645,   651,   659,   660,
     663,   664,   667,   668,   671,   672,   673,   676,   677,   680,
     681,   682,   683,   686,   687,   688,   691,   692,   693,   694,
     695,   698,   699,   700,   703,   713,   714,   717,   721,   726,
     731,   736,   738,   740,   742,   743,   744,   745,   746,   747,
     748,   750,   752,   754,   756,   757,   758,   759,   760,   761,
     762,   763,   764,   765,   766,   767,   768,   769,   770,   771,
     772,   773,   774
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "H_NUMBER", "D_NUMBER", "O_NUMBER",
  "B_NUMBER", "CONVERT_OP", "B_DATA", "H_RANGE_GUESS", "D_NUMBER_GUESS",
  "O_NUMBER_GUESS", "B_NUMBER_GUESS", "BAD_CMD", "MEM_OP", "IF",
  "MEM_COMP", "MEM_DISK8", "MEM_DISK9", "MEM_DISK10", "MEM_DISK11",
  "EQUALS", "TRAIL", "CMD_SEP", "LABEL_ASGN_COMMENT", "CMD_SIDEFX",
  "CMD_RETURN", "CMD_BLOCK_READ", "CMD_BLOCK_WRITE", "CMD_UP", "CMD_DOWN",
  "CMD_LOAD", "CMD_SAVE", "CMD_VERIFY", "CMD_IGNORE", "CMD_HUNT",
  "CMD_FILL", "CMD_MOVE", "CMD_GOTO", "CMD_REGISTERS", "CMD_READSPACE",
  "CMD_WRITESPACE", "CMD_RADIX", "CMD_MEM_DISPLAY", "CMD_BREAK",
  "CMD_TRACE", "CMD_IO", "CMD_BRMON", "CMD_COMPARE", "CMD_DUMP",
  "CMD_UNDUMP", "CMD_EXIT", "CMD_DELETE", "CMD_CONDITION", "CMD_COMMAND",
  "CMD_ASSEMBLE", "CMD_DISASSEMBLE", "CMD_NEXT", "CMD_STEP", "CMD_PRINT",
  "CMD_DEVICE", "CMD_HELP", "CMD_WATCH", "CMD_DISK", "CMD_SYSTEM",
  "CMD_QUIT", "CMD_CHDIR", "CMD_BANK", "CMD_LOAD_LABELS",
  "CMD_SAVE_LABELS", "CMD_ADD_LABEL", "CMD_DEL_LABEL", "CMD_SHOW_LABELS",
  "CMD_RECORD", "CMD_MON_STOP", "CMD_PLAYBACK", "CMD_CHAR_DISPLAY",
  "CMD_SPRITE_DISPLAY", "CMD_TEXT_DISPLAY", "CMD_SCREENCODE_DISPLAY",
  "CMD_ENTER_DATA", "CMD_ENTER_BIN_DATA", "CMD_KEYBUF", "CMD_BLOAD",
  "CMD_BSAVE", "CMD_SCREEN", "CMD_UNTIL", "CMD_CPU", "CMD_YYDEBUG",
  "CMD_BACKTRACE", "CMD_SCREENSHOT", "CMD_PWD", "CMD_DIR",
  "CMD_RESOURCE_GET", "CMD_RESOURCE_SET", "CMD_ATTACH", "CMD_DETACH",
  "CMD_MON_RESET", "CMD_TAPECTRL", "CMD_CARTFREEZE", "CMD_CPUHISTORY",
  "CMD_MEMMAPZAP", "CMD_MEMMAPSHOW", "CMD_MEMMAPSAVE", "CMD_COMMENT",
  "CMD_LABEL_ASGN", "L_PAREN", "R_PAREN", "ARG_IMMEDIATE", "REG_A",
  "REG_X", "REG_Y", "COMMA", "INST_SEP", "REG_B", "REG_C", "REG_D",
  "REG_E", "REG_H", "REG_L", "REG_AF", "REG_BC", "REG_DE", "REG_HL",
  "REG_IX", "REG_IY", "REG_SP", "REG_IXH", "REG_IXL", "REG_IYH", "REG_IYL",
  "STRING", "FILENAME", "R_O_L", "OPCODE", "LABEL", "BANKNAME", "CPUTYPE",
  "MON_REGISTER", "COMPARE_OP", "RADIX_TYPE", "INPUT_SPEC",
  "CMD_CHECKPT_ON", "CMD_CHECKPT_OFF", "TOGGLE", "MASK", "'+'", "'-'",
  "'*'", "'/'", "'('", "')'", "$accept", "top_level", "command_list",
  "end_cmd", "command", "machine_state_rules", "register_mod",
  "symbol_table_rules", "asm_rules", "@1", "memory_rules",
  "checkpoint_rules", "checkpoint_control_rules", "monitor_state_rules",
  "monitor_misc_rules", "disk_rules", "cmd_file_rules", "data_entry_rules",
  "monitor_debug_rules", "rest_of_line", "opt_rest_of_line", "filename",
  "device_num", "opt_mem_op", "register", "reg_list", "reg_asgn",
  "breakpt_num", "address_opt_range", "address_range", "opt_address",
  "address", "opt_sep", "memspace", "memloc", "memaddr", "expression",
  "cond_expr", "compare_operand", "data_list", "data_element", "hunt_list",
  "hunt_element", "value", "d_number", "guess_default", "number",
  "assembly_instr_list", "assembly_instruction", "post_assemble",
  "asm_operand_mode", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
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
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,    43,    45,    42,    47,
      40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   152,   153,   153,   153,   154,   154,   155,   155,   155,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   158,   158,   158,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     161,   160,   160,   160,   160,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   163,   163,   163,   163,   163,
     163,   163,   163,   163,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   165,   165,   165,   165,   165,   165,   165,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   168,   168,   168,   169,   169,   170,   171,
     172,   172,   173,   173,   174,   174,   175,   175,   176,   176,
     177,   177,   178,   179,   179,   180,   180,   181,   181,   181,
     182,   182,   183,   183,   183,   184,   184,   185,   185,   185,
     185,   185,   186,   187,   188,   188,   188,   188,   188,   188,
     188,   189,   189,   189,   189,   189,   190,   190,   191,   191,
     192,   192,   193,   193,   194,   194,   194,   195,   195,   196,
     196,   196,   196,   197,   197,   197,   198,   198,   198,   198,
     198,   199,   199,   199,   200,   201,   201,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     3,     5,     3,     2,     2,
       3,     2,     4,     2,     3,     3,     2,     4,     2,     4,
       2,     4,     2,     4,     2,     1,     2,     3,     3,     5,
       3,     5,     3,     5,     3,     5,     3,     2,     4,     5,
       0,     5,     3,     3,     2,     5,     5,     5,     5,     5,
       3,     2,     3,     2,     3,     2,     3,     2,     3,     2,
       2,     2,     4,     5,     5,     3,     3,     5,     5,     3,
       2,     2,     2,     2,     3,     3,     3,     5,     3,     2,
       5,     5,     4,     3,     2,     3,     2,     3,     2,     2,
       3,     3,     2,     3,     3,     3,     3,     3,     2,     3,
       2,     3,     5,     3,     4,     2,     4,     4,     2,     3,
       5,     5,     5,     3,     4,     5,     4,     5,     4,     5,
       5,     4,     3,     3,     2,     3,     4,     2,     2,     1,
       1,     0,     1,     1,     1,     1,     1,     0,     1,     2,
       3,     1,     3,     1,     1,     1,     1,     3,     1,     3,
       2,     0,     1,     3,     1,     1,     0,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       1,     3,     3,     3,     3,     1,     1,     1,     3,     1,
       1,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     2,     2,     1,     1,     2,     1,     3,
       3,     3,     5,     5,     3,     3,     3,     3,     3,     3,
       5,     5,     5,     5,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,    22,     4,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   151,     0,
       0,     0,     0,     0,   176,     0,     0,     0,     0,     0,
     151,     0,   244,     0,     0,     0,     2,     5,    10,    45,
      11,    13,    12,    14,    15,    16,    17,    18,    19,    20,
      21,     0,   216,   217,   218,   219,   215,   214,   213,   177,
     178,   179,   180,   181,   158,     0,   208,     0,     0,   190,
     220,   207,     9,     8,     7,     0,   104,    33,     0,     0,
     175,    40,     0,    42,     0,   153,   152,     0,     0,     0,
     164,   209,   212,   211,   210,     0,   163,   168,   174,   176,
     176,   176,   172,   182,   183,   176,   176,     0,   176,    46,
       0,     0,   161,     0,     0,   106,   176,    71,     0,   165,
     176,    90,     0,    92,     0,    28,   176,     0,     0,   109,
       9,    99,     0,     0,     0,     0,    64,     0,    38,     0,
      36,     0,     0,     0,   149,   112,     0,   156,    93,   176,
       0,     0,   108,     0,     0,    23,     0,     0,   176,     0,
     176,   176,     0,   176,    57,     0,     0,   144,     0,    73,
       0,    75,     0,    77,     0,    79,     0,     0,   147,     0,
       0,     0,    44,    91,     0,     0,    29,   148,   118,     0,
     120,   150,     0,     0,     0,     0,     0,   125,     0,     0,
     128,    31,     0,    80,    81,     0,   176,     0,     0,     0,
       0,   245,   246,   247,   248,   249,   250,   253,   256,   257,
     258,   259,   260,   261,   262,   251,   254,   252,   255,   228,
     224,     0,     0,     1,     6,     3,     0,   159,     0,     0,
       0,     0,   115,   103,   176,     0,     0,     0,   155,   176,
     154,   133,     0,     0,    96,     0,     0,     0,     0,     0,
       0,    27,     0,     0,     0,    48,    47,   105,     0,    70,
       0,    85,    89,     0,    34,    35,    98,     0,     0,     0,
      62,     0,    63,     0,     0,   111,   107,   113,     0,   110,
     114,   116,    25,    24,     0,    50,     0,    52,     0,     0,
      54,     0,    56,   143,   145,    72,    74,    76,    78,   201,
       0,   199,   200,   117,   176,     0,    86,    30,   121,     0,
     119,   123,     0,     0,   142,     0,     0,     0,     0,     0,
     129,     0,     0,     0,     0,     0,     0,     0,     0,   227,
       0,    94,    95,   189,   188,   184,   185,   186,   187,     0,
       0,     0,    41,    43,     0,   134,     0,   138,     0,     0,
     206,   205,     0,   203,   204,   167,   169,   173,     0,     0,
     162,   160,     0,     0,   196,     0,   195,   197,     0,     0,
     102,     0,   226,   225,     0,    39,    37,     0,     0,     0,
       0,     0,     0,   146,     0,     0,   136,     0,     0,   124,
     141,   126,   127,    32,    82,     0,     0,     0,    58,   234,
     235,   236,   237,   238,   239,   231,     0,   229,   230,   139,
     170,   140,   130,   132,   137,    97,    68,   202,    67,    65,
      69,     0,     0,    87,    66,   100,   101,     0,   223,    61,
      88,    26,    49,    51,    53,    55,   198,   131,   135,   122,
      83,    84,    59,     0,     0,   194,   193,   192,   191,   221,
     222,   240,   233,   241,   242,   243,   232
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    85,    86,   126,    87,    88,    89,    90,    91,   331,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   196,
     242,   137,   299,   199,   116,   161,   162,   145,   168,   169,
     399,   170,   400,   117,   152,   153,   300,   425,   426,   360,
     361,   412,   413,   119,   146,   120,   154,   432,   101,   434,
     280
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -324
static const yytype_int16 yypact[] =
{
    1081,   825,  -324,  -324,    17,   233,   825,   825,   259,   259,
      22,    22,    22,   231,  1418,  1418,  1418,  1439,   382,    98,
     961,  1052,  1052,   233,  1418,    22,    22,   233,   769,   231,
     231,  1439,  1052,   259,   259,   825,   568,   191,  1335,  -121,
    -121,   233,  -121,   600,   282,   282,  1439,   433,   678,    22,
     233,    22,  1052,  1052,  1052,  1052,  1439,   233,  -121,    22,
      22,   233,  1052,   112,   233,   233,    22,   233,   -92,   -88,
     -66,    22,   825,   259,   -64,   233,   259,   233,   259,    22,
     -92,    48,  1461,   231,   231,    76,  1200,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,    51,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,   825,  -324,   -35,    73,  -324,
    -324,  -324,  -324,  -324,  -324,   233,  -324,  -324,   803,   803,
    -324,  -324,   825,  -324,   825,  -324,  -324,    74,   428,    74,
    -324,  -324,  -324,  -324,  -324,   259,  -324,  -324,  -324,   -64,
     -64,   -64,  -324,  -324,  -324,   -64,   -64,   233,   -64,  -324,
      86,   291,  -324,   107,   233,  -324,   -64,  -324,   233,  -324,
     251,  -324,   271,  -324,   233,  -324,   -64,   233,   233,  -324,
     306,  -324,   233,    95,    87,   172,  -324,   233,  -324,   825,
    -324,   825,    73,   233,  -324,  -324,   233,  -324,  -324,   -64,
     233,   233,  -324,   233,   233,  -324,   125,   233,   -64,   233,
     -64,   -64,   233,   -64,  -324,   233,   233,  -324,   233,  -324,
     233,  -324,   233,  -324,   233,  -324,   233,   624,  -324,   233,
      74,    74,  -324,  -324,   233,   233,  -324,  -324,  -324,   259,
    -324,  -324,   233,   233,   -20,   825,    73,  -324,   825,   825,
    -324,  -324,   825,  -324,  -324,   825,   -64,   233,  1439,   915,
     868,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,     0,
    -324,   233,   233,  -324,  -324,  -324,    34,  -324,   825,   825,
     825,   825,  -324,  -324,   245,   896,    73,    73,  -324,   394,
     354,  1192,  1358,  1400,  -324,   825,    41,  1439,   784,   624,
    1439,  -324,   868,   868,   407,  -324,  -324,  -324,  1418,  -324,
    1303,  -324,  -324,  1439,  -324,  -324,  -324,  1303,   233,   -13,
    -324,   -15,  -324,    73,    73,  -324,  -324,  -324,  1418,  -324,
    -324,  -324,  -324,  -324,   -12,  -324,    22,  -324,    22,    -3,
    -324,     5,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,
     845,  -324,  -324,  -324,   394,  1378,  -324,  -324,  -324,   825,
    -324,  -324,   233,    73,  -324,    73,    73,    73,   651,   825,
    -324,   186,    35,    39,    44,    46,    49,    55,   -83,  -324,
     -80,  -324,  -324,  -324,  -324,   -94,   -94,  -324,  -324,   233,
    1439,   233,  -324,  -324,   233,  -324,   233,  -324,   233,    73,
    -324,  -324,    10,  -324,  -324,  -324,  -324,  -324,   845,   233,
    -324,  -324,   233,  1303,  -324,   100,  -324,  -324,   233,   100,
    -324,   233,    45,    50,   233,  -324,  -324,   233,   233,   233,
     233,   233,   233,  -324,   624,   233,  -324,   233,    73,  -324,
    -324,  -324,  -324,  -324,  -324,   233,    73,   233,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,    53,    58,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,    18,   984,  -324,  -324,  -324,  -324,   -15,   -15,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,   439,    62,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -324,  -324,  -324,   312,    85,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,   419,
      99,    38,   -33,  -324,   -17,  -324,  -125,    32,     4,    -7,
    -262,   160,    -6,   559,  -270,  -324,   643,  -323,  -324,  -113,
    -243,  -324,  -209,  -324,  -324,  -324,    -1,  -324,  -321,  -324,
    -324
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -177
static const yytype_int16 yytable[] =
{
     121,   160,   132,   134,   429,   121,   121,   149,   155,   156,
     433,   122,   194,   102,   103,   104,   105,   176,   122,   505,
     106,   107,   108,   135,   465,   172,   174,   189,   191,   466,
     467,   468,   123,   124,   121,   393,   187,   404,   417,   123,
     124,   241,   417,   243,   102,   103,   104,   105,   130,   138,
     139,   106,   107,   108,   290,   291,   220,   222,   224,   226,
     182,   183,   184,   177,   178,   244,   234,   248,   249,   258,
     252,   121,   255,   285,   122,   298,   283,   102,   103,   104,
     105,   279,   207,   209,   106,   107,   108,   216,   328,   218,
     109,   110,   111,   112,   113,   123,   124,   230,   231,   122,
     481,   122,   445,   287,   239,   302,   303,   313,   122,   245,
     327,   372,   390,   122,   121,   281,   282,   256,   431,    82,
     123,   124,   123,   124,   438,   506,   122,   121,   121,   123,
     124,   121,   441,   121,   123,   124,   121,   121,   121,   305,
     442,   410,   459,   306,   307,   308,   460,   123,   124,   309,
     310,   461,   312,   462,   136,   411,   463,   482,   487,   508,
     318,   125,   464,   488,   307,   503,   509,   510,   504,   516,
     323,   284,   410,   122,   150,   150,   150,   157,   329,   257,
     288,   289,   290,   291,   150,   394,   411,   122,   121,   421,
     121,   185,   122,   338,   123,   124,   418,   364,   365,   130,
     344,   496,   346,   477,   348,   349,   211,   351,   123,   124,
     457,     0,   114,   123,   124,     0,   227,     0,  -176,   288,
     289,   290,   291,     0,   115,     0,   362,     0,     0,   121,
     121,     0,   140,   369,   122,   141,     0,   130,   164,   482,
       0,   142,   143,   144,   121,   287,  -171,   121,   121,   235,
     379,   121,  -166,     0,   121,   123,   124,     0,   388,   389,
     122,  -176,  -176,  -176,  -176,  -176,  -166,  -171,  -171,  -176,
    -176,  -176,   122,  -166,  -166,  -176,  -176,  -176,  -176,  -176,
       0,   123,   124,   135,     0,     0,   320,   121,   121,   121,
     121,     0,   122,   123,   124,   406,     0,   160,   109,   110,
     111,   112,   113,   424,   121,   414,   -60,  -164,   362,     0,
     424,     0,   420,   123,   124,     0,     0,   127,     0,   427,
     131,   133,   422,     0,   194,     0,   427,     0,  -164,  -164,
     159,   165,   167,   171,   173,   175,     0,     0,     0,   179,
     181,     0,   437,     0,   186,   188,   190,     0,     0,   195,
     198,     0,     0,   202,   444,   205,     0,   130,   447,     0,
     214,     0,   217,   130,   219,   221,   223,   225,   121,   228,
       0,   130,     0,   232,   233,   236,   237,   238,   121,   240,
       0,     0,   455,   122,   439,   247,   440,   250,   251,   253,
     254,   288,   289,   290,   291,  -171,     0,  -176,   109,   110,
     111,   112,   113,   314,   123,   124,   424,     0,     0,  -176,
       0,   414,   444,     0,   136,     0,  -171,  -171,   381,     0,
       0,     0,   427,   109,   110,   111,   112,   113,     0,   301,
     292,   102,   103,   104,   105,     0,     0,   293,   106,   107,
     108,     0,     0,   362,   109,   110,   111,   112,   113,   109,
     110,   111,   112,   113,     0,   401,     0,   304,   200,   201,
       0,   203,   150,   408,     0,   424,     0,   415,     0,   311,
     419,     0,     0,   315,     0,   316,   317,   229,     0,     0,
     319,   427,     0,   428,   321,     0,   322,     0,     0,   324,
     325,     0,     0,     0,   326,     0,     0,   330,     0,   332,
     288,   289,   290,   291,   335,   336,   130,     0,   337,     0,
       0,     0,   339,   340,     0,   341,   342,     0,   343,   345,
     114,   347,     0,     0,   350,   150,     0,   352,   353,     0,
     354,     0,   355,     0,   356,     0,   357,     0,   358,     0,
       0,   363,     0,     0,     0,   114,   366,   367,   511,     0,
     512,   368,     0,     0,   370,   371,     0,     0,   374,     0,
     470,     0,   513,   514,   515,     0,   114,     0,   212,   380,
       0,     0,     0,   151,   151,   151,   158,   163,   115,   151,
     151,   151,     0,   151,   109,   110,   111,   112,   113,     0,
     158,   151,     0,   391,   392,   193,     0,     0,     0,     0,
       0,   122,   206,   208,   210,   158,   213,   215,   402,   403,
       0,   151,   151,   151,   151,   158,   109,   110,   111,   112,
     113,   151,   123,   124,     0,     0,     0,   102,   103,   104,
     105,     0,     0,     0,   106,   107,   108,     0,     0,     0,
     430,     0,     0,     0,   118,   435,   436,     0,     0,   128,
     129,     0,   122,     0,   102,   103,   104,   105,     0,     0,
     147,   106,   107,   108,     0,     0,     0,   109,   110,   111,
     112,   113,   443,   123,   124,     0,     0,     0,   192,   122,
       0,     0,     0,     0,   449,   450,     0,   451,   452,   453,
     454,     0,     0,   458,   109,   110,   111,   112,   113,     0,
     123,   124,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   469,     0,   471,     0,   246,   472,     0,   473,     0,
     474,   475,     0,     0,   476,     0,     0,     0,     0,     0,
     478,   479,     0,     0,   480,     0,   204,   483,     0,     0,
     484,   485,     0,   486,     0,     0,   489,     0,     0,   490,
     491,   492,   493,   494,   495,   359,     0,   497,   286,   498,
     499,     0,     0,     0,     0,     0,     0,   500,   501,   502,
     180,   294,   295,   141,     0,   296,     0,   297,     0,   142,
     143,   144,     0,     0,     0,     0,   148,   102,   103,   104,
     105,   123,   124,   416,   106,   107,   108,   288,   289,   290,
     291,     0,     0,     0,     0,     0,   102,   103,   104,   105,
       0,     0,     0,   106,   107,   108,     0,   158,     0,   109,
     110,   111,   112,   113,     0,     0,     0,     0,   102,   103,
     104,   105,   333,     0,   334,   106,   107,   108,     0,     0,
       0,   109,   110,   111,   112,   113,   122,     0,  -176,  -176,
    -176,  -176,     0,     0,   158,  -176,  -176,  -176,     0,     0,
       0,   151,   158,     0,     0,     0,   158,   123,   124,   158,
       0,   102,   103,   104,   105,     0,     0,   151,   106,   107,
     108,     0,   158,     0,     0,     0,     0,     0,   373,     0,
       0,   375,   376,     0,     0,   377,     0,   151,   378,   102,
     103,   104,   105,     0,     0,     0,   106,   107,   108,     0,
       0,     0,   109,   110,   111,   112,   113,     0,   102,   103,
     104,   105,     0,     0,   151,   106,   107,   108,     0,     0,
       0,   395,   396,   397,   398,     0,     0,   151,     0,     0,
       0,   114,     0,     0,     0,     0,     0,     0,   409,   288,
     289,   290,   291,   115,     0,     0,     0,   130,     0,   158,
       0,     0,   122,   114,   102,   103,   104,   105,     0,     0,
     147,   106,   107,   108,     0,   115,  -176,   109,   110,   111,
     112,   113,     0,   123,   124,   507,     0,   102,   103,   104,
     105,     0,     0,     0,   106,   107,   108,     0,     0,     0,
     109,   110,   111,   112,   113,     0,     0,     0,     0,     0,
       0,     0,   448,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   456,     0,     0,     0,     0,     0,     0,     0,
       0,   148,     0,     0,     0,     0,   382,   383,   384,   385,
     386,   387,   288,   289,   290,   291,     0,     0,     0,     0,
       0,     0,     0,   122,     0,   102,   103,   104,   105,     0,
       0,   147,   106,   107,   108,     0,     0,     0,   109,   110,
     111,   112,   113,     0,   123,   124,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     1,     0,
     423,     0,     0,     0,     2,     0,   148,     0,     0,     0,
       0,   166,     0,     3,     0,     0,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,     0,   114,    19,    20,    21,    22,    23,     0,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,   148,     0,     0,
       0,     0,     0,  -155,     0,  -155,  -155,  -155,  -155,     0,
       0,  -155,  -155,  -155,  -155,     0,     0,     1,  -155,  -155,
    -155,  -155,  -155,     2,     0,    82,     0,     0,     0,     0,
       0,     0,     0,    83,    84,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
       0,     0,    19,    20,    21,    22,    23,     0,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,   102,   103,   104,   105,
       0,     0,     0,   106,   107,   108,     0,     0,     0,   109,
     110,   111,   112,   113,     0,     0,     0,  -155,     0,     0,
       0,     0,     0,     0,     0,     0,   122,     0,  -157,  -157,
    -157,  -157,    83,    84,  -157,  -157,  -157,  -157,     0,   197,
       0,  -157,  -157,  -157,  -157,  -157,     0,   123,   124,   405,
       0,   102,   103,   104,   105,     0,     0,   147,   106,   107,
     108,     0,     0,     0,   109,   110,   111,   112,   113,   446,
       0,   102,   103,   104,   105,     0,     0,   147,   106,   107,
     108,     0,     0,     0,   109,   110,   111,   112,   113,     0,
       0,   407,     0,   102,   103,   104,   105,     0,     0,   423,
     106,   107,   108,     0,     0,     0,   109,   110,   111,   112,
     113,   102,   103,   104,   105,     0,     0,   147,   106,   107,
     108,     0,     0,     0,   109,   110,   111,   112,   113,     0,
       0,   114,   102,   103,   104,   105,     0,  -157,     0,   106,
     107,   108,     0,     0,     0,   109,   110,   111,   112,   113,
       0,     0,     0,     0,   102,   103,   104,   105,     0,     0,
    -157,   106,   107,   108,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   148,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   148,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   148,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   148,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   259,     0,   260,
     261,     0,     0,     0,   148,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278
};

static const yytype_int16 yycheck[] =
{
       1,    18,     8,     9,   327,     6,     7,    14,    15,    16,
     331,     1,   133,     3,     4,     5,     6,    24,     1,     1,
      10,    11,    12,     1,   107,    21,    22,    33,    34,   112,
     110,   111,    22,    23,    35,     1,    32,   299,   308,    22,
      23,   133,   312,   131,     3,     4,     5,     6,   112,    11,
      12,    10,    11,    12,   148,   149,    52,    53,    54,    55,
      28,    29,    30,    25,    26,   131,    62,    73,    74,    21,
      76,    72,    78,    22,     1,     1,     0,     3,     4,     5,
       6,    82,    44,    45,    10,    11,    12,    49,     1,    51,
      16,    17,    18,    19,    20,    22,    23,    59,    60,     1,
     423,     1,   364,   138,    66,   138,   139,    21,     1,    71,
      15,   131,   112,     1,   115,    83,    84,    79,   131,   134,
      22,    23,    22,    23,   136,   107,     1,   128,   129,    22,
      23,   132,   135,   134,    22,    23,   137,   138,   139,   145,
     135,   131,   107,   149,   150,   151,   107,    22,    23,   155,
     156,   107,   158,   107,   132,   145,   107,   139,   113,   482,
     166,   144,   107,   113,   170,   112,   487,   488,   110,   107,
     176,    86,   131,     1,    14,    15,    16,    17,   184,    80,
     146,   147,   148,   149,    24,   151,   145,     1,   189,   314,
     191,    31,     1,   199,    22,    23,   309,   230,   231,   112,
     206,   444,   208,   412,   210,   211,    46,   213,    22,    23,
      24,    -1,   138,    22,    23,    -1,    56,    -1,   131,   146,
     147,   148,   149,    -1,   150,    -1,   227,    -1,    -1,   230,
     231,    -1,     1,   239,     1,     4,    -1,   112,   140,   139,
      -1,    10,    11,    12,   245,   138,     1,   248,   249,   137,
     256,   252,     1,    -1,   255,    22,    23,    -1,   259,   260,
       1,   136,     3,     4,     5,     6,    15,    22,    23,    10,
      11,    12,     1,    22,    23,    16,    17,    18,    19,    20,
      -1,    22,    23,     1,    -1,    -1,    15,   288,   289,   290,
     291,    -1,     1,    22,    23,   302,    -1,   314,    16,    17,
      18,    19,    20,   320,   305,   306,   134,     1,   309,    -1,
     327,    -1,   313,    22,    23,    -1,    -1,     5,    -1,   320,
       8,     9,   318,    -1,   133,    -1,   327,    -1,    22,    23,
      18,    19,    20,    21,    22,    23,    -1,    -1,    -1,    27,
      28,    -1,   338,    -1,    32,    33,    34,    -1,    -1,    37,
      38,    -1,    -1,    41,   360,    43,    -1,   112,   365,    -1,
      48,    -1,    50,   112,    52,    53,    54,    55,   369,    57,
      -1,   112,    -1,    61,    62,    63,    64,    65,   379,    67,
      -1,    -1,   378,     1,   346,    73,   348,    75,    76,    77,
      78,   146,   147,   148,   149,     1,    -1,   138,    16,    17,
      18,    19,    20,   112,    22,    23,   423,    -1,    -1,   150,
      -1,   412,   418,    -1,   132,    -1,    22,    23,   258,    -1,
      -1,    -1,   423,    16,    17,    18,    19,    20,    -1,     1,
     118,     3,     4,     5,     6,    -1,    -1,   125,    10,    11,
      12,    -1,    -1,   444,    16,    17,    18,    19,    20,    16,
      17,    18,    19,    20,    -1,   295,    -1,   145,    39,    40,
      -1,    42,   302,   303,    -1,   482,    -1,   307,    -1,   157,
     310,    -1,    -1,   161,    -1,   163,   164,    58,    -1,    -1,
     168,   482,    -1,   323,   172,    -1,   174,    -1,    -1,   177,
     178,    -1,    -1,    -1,   182,    -1,    -1,   185,    -1,   187,
     146,   147,   148,   149,   192,   193,   112,    -1,   196,    -1,
      -1,    -1,   200,   201,    -1,   203,   204,    -1,   206,   207,
     138,   209,    -1,    -1,   212,   365,    -1,   215,   216,    -1,
     218,    -1,   220,    -1,   222,    -1,   224,    -1,   226,    -1,
      -1,   229,    -1,    -1,    -1,   138,   234,   235,   109,    -1,
     111,   239,    -1,    -1,   242,   243,    -1,    -1,   246,    -1,
     400,    -1,   123,   124,   125,    -1,   138,    -1,   135,   257,
      -1,    -1,    -1,    14,    15,    16,    17,    18,   150,    20,
      21,    22,    -1,    24,    16,    17,    18,    19,    20,    -1,
      31,    32,    -1,   281,   282,    36,    -1,    -1,    -1,    -1,
      -1,     1,    43,    44,    45,    46,    47,    48,   296,   297,
      -1,    52,    53,    54,    55,    56,    16,    17,    18,    19,
      20,    62,    22,    23,    -1,    -1,    -1,     3,     4,     5,
       6,    -1,    -1,    -1,    10,    11,    12,    -1,    -1,    -1,
     328,    -1,    -1,    -1,     1,   333,   334,    -1,    -1,     6,
       7,    -1,     1,    -1,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    12,    -1,    -1,    -1,    16,    17,    18,
      19,    20,   360,    22,    23,    -1,    -1,    -1,    35,     1,
      -1,    -1,    -1,    -1,   372,   373,    -1,   375,   376,   377,
     378,    -1,    -1,   381,    16,    17,    18,    19,    20,    -1,
      22,    23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   399,    -1,   401,    -1,    72,   404,    -1,   406,    -1,
     408,   409,    -1,    -1,   412,    -1,    -1,    -1,    -1,    -1,
     418,   419,    -1,    -1,   422,    -1,   136,   425,    -1,    -1,
     428,   429,    -1,   431,    -1,    -1,   434,    -1,    -1,   437,
     438,   439,   440,   441,   442,   131,    -1,   445,   115,   447,
     448,    -1,    -1,    -1,    -1,    -1,    -1,   455,   456,   457,
       1,   128,   129,     4,    -1,   132,    -1,   134,    -1,    10,
      11,    12,    -1,    -1,    -1,    -1,   135,     3,     4,     5,
       6,    22,    23,     9,    10,    11,    12,   146,   147,   148,
     149,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    -1,    -1,    10,    11,    12,    -1,   258,    -1,    16,
      17,    18,    19,    20,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,   189,    -1,   191,    10,    11,    12,    -1,    -1,
      -1,    16,    17,    18,    19,    20,     1,    -1,     3,     4,
       5,     6,    -1,    -1,   295,    10,    11,    12,    -1,    -1,
      -1,   302,   303,    -1,    -1,    -1,   307,    22,    23,   310,
      -1,     3,     4,     5,     6,    -1,    -1,   318,    10,    11,
      12,    -1,   323,    -1,    -1,    -1,    -1,    -1,   245,    -1,
      -1,   248,   249,    -1,    -1,   252,    -1,   338,   255,     3,
       4,     5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,
      -1,    -1,    16,    17,    18,    19,    20,    -1,     3,     4,
       5,     6,    -1,    -1,   365,    10,    11,    12,    -1,    -1,
      -1,   288,   289,   290,   291,    -1,    -1,   378,    -1,    -1,
      -1,   138,    -1,    -1,    -1,    -1,    -1,    -1,   305,   146,
     147,   148,   149,   150,    -1,    -1,    -1,   112,    -1,   400,
      -1,    -1,     1,   138,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    12,    -1,   150,   131,    16,    17,    18,
      19,    20,    -1,    22,    23,     1,    -1,     3,     4,     5,
       6,    -1,    -1,    -1,    10,    11,    12,    -1,    -1,    -1,
      16,    17,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   369,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   379,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   135,    -1,    -1,    -1,    -1,   121,   122,   123,   124,
     125,   126,   146,   147,   148,   149,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,    -1,
      -1,     9,    10,    11,    12,    -1,    -1,    -1,    16,    17,
      18,    19,    20,    -1,    22,    23,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
     106,    -1,    -1,    -1,    13,    -1,   135,    -1,    -1,    -1,
      -1,   140,    -1,    22,    -1,    -1,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    -1,   138,    42,    43,    44,    45,    46,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   135,    -1,    -1,
      -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,    -1,
      -1,     9,    10,    11,    12,    -1,    -1,     7,    16,    17,
      18,    19,    20,    13,    -1,   134,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   142,   143,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      -1,    -1,    42,    43,    44,    45,    46,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,     3,     4,     5,     6,
      -1,    -1,    -1,    10,    11,    12,    -1,    -1,    -1,    16,
      17,    18,    19,    20,    -1,    -1,    -1,   135,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,     3,     4,
       5,     6,   142,   143,     9,    10,    11,    12,    -1,    14,
      -1,    16,    17,    18,    19,    20,    -1,    22,    23,     1,
      -1,     3,     4,     5,     6,    -1,    -1,     9,    10,    11,
      12,    -1,    -1,    -1,    16,    17,    18,    19,    20,     1,
      -1,     3,     4,     5,     6,    -1,    -1,     9,    10,    11,
      12,    -1,    -1,    -1,    16,    17,    18,    19,    20,    -1,
      -1,     1,    -1,     3,     4,     5,     6,    -1,    -1,   106,
      10,    11,    12,    -1,    -1,    -1,    16,    17,    18,    19,
      20,     3,     4,     5,     6,    -1,    -1,     9,    10,    11,
      12,    -1,    -1,    -1,    16,    17,    18,    19,    20,    -1,
      -1,   138,     3,     4,     5,     6,    -1,   112,    -1,    10,
      11,    12,    -1,    -1,    -1,    16,    17,    18,    19,    20,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,    -1,
     135,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   135,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   135,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   135,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   135,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   106,    -1,   108,
     109,    -1,    -1,    -1,   135,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     7,    13,    22,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    42,
      43,    44,    45,    46,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   134,   142,   143,   153,   154,   156,   157,   158,
     159,   160,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   200,     3,     4,     5,     6,    10,    11,    12,    16,
      17,    18,    19,    20,   138,   150,   176,   185,   188,   195,
     197,   198,     1,    22,    23,   144,   155,   155,   188,   188,
     112,   155,   184,   155,   184,     1,   132,   173,   173,   173,
       1,     4,    10,    11,    12,   179,   196,     9,   135,   181,
     183,   185,   186,   187,   198,   181,   181,   183,   185,   155,
     176,   177,   178,   185,   140,   155,   140,   155,   180,   181,
     183,   155,   180,   155,   180,   155,   181,   173,   173,   155,
       1,   155,   179,   179,   179,   183,   155,   180,   155,   184,
     155,   184,   188,   185,   133,   155,   171,    14,   155,   175,
     171,   171,   155,   171,   136,   155,   185,   173,   185,   173,
     185,   183,   135,   185,   155,   185,   173,   155,   173,   155,
     180,   155,   180,   155,   180,   155,   180,   183,   155,   171,
     173,   173,   155,   155,   180,   137,   155,   155,   155,   173,
     155,   133,   172,   131,   131,   173,   188,   155,   184,   184,
     155,   155,   184,   155,   155,   184,   173,   172,    21,   106,
     108,   109,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   198,
     202,   179,   179,     0,   156,    22,   188,   138,   146,   147,
     148,   149,   155,   155,   188,   188,   188,   188,     1,   174,
     188,     1,   174,   174,   155,   184,   184,   184,   184,   184,
     184,   155,   184,    21,   112,   155,   155,   155,   184,   155,
      15,   155,   155,   184,   155,   155,   155,    15,     1,   184,
     155,   161,   155,   188,   188,   155,   155,   155,   184,   155,
     155,   155,   155,   155,   184,   155,   184,   155,   184,   184,
     155,   184,   155,   155,   155,   155,   155,   155,   155,   131,
     191,   192,   198,   155,   174,   174,   155,   155,   155,   184,
     155,   155,   131,   188,   155,   188,   188,   188,   188,   184,
     155,   183,   121,   122,   123,   124,   125,   126,   198,   198,
     112,   155,   155,     1,   151,   188,   188,   188,   188,   182,
     184,   183,   155,   155,   182,     1,   181,     1,   183,   188,
     131,   145,   193,   194,   198,   183,     9,   186,   191,   183,
     198,   178,   180,   106,   176,   189,   190,   198,   183,   189,
     155,   131,   199,   200,   201,   155,   155,   180,   136,   173,
     173,   135,   135,   155,   184,   182,     1,   181,   188,   155,
     155,   155,   155,   155,   155,   180,   188,    24,   155,   107,
     107,   107,   107,   107,   107,   107,   112,   110,   111,   155,
     183,   155,   155,   155,   155,   155,   155,   194,   155,   155,
     155,   189,   139,   155,   155,   155,   155,   113,   113,   155,
     155,   155,   155,   155,   155,   155,   192,   155,   155,   155,
     155,   155,   155,   112,   110,     1,   107,     1,   189,   200,
     200,   109,   111,   123,   124,   125,   107
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


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
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

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
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

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
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
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

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 185 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 3:
#line 186 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 4:
#line 187 "mon_parse.y"
    { new_cmd = 1; asm_mode = 0;  (yyval.i) = 0; }
    break;

  case 9:
#line 196 "mon_parse.y"
    { return ERR_EXPECT_END_CMD; }
    break;

  case 22:
#line 211 "mon_parse.y"
    { return ERR_BAD_CMD; }
    break;

  case 23:
#line 215 "mon_parse.y"
    { mon_bank(e_default_space, NULL); }
    break;

  case 24:
#line 217 "mon_parse.y"
    { mon_bank((yyvsp[(2) - (3)].i), NULL); }
    break;

  case 25:
#line 219 "mon_parse.y"
    { mon_bank(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 26:
#line 221 "mon_parse.y"
    { mon_bank((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 27:
#line 223 "mon_parse.y"
    { mon_jump((yyvsp[(2) - (3)].a)); }
    break;

  case 28:
#line 225 "mon_parse.y"
    { mon_display_io_regs(); }
    break;

  case 29:
#line 227 "mon_parse.y"
    { monitor_cpu_type_set(""); }
    break;

  case 30:
#line 229 "mon_parse.y"
    { monitor_cpu_type_set((yyvsp[(2) - (3)].str)); }
    break;

  case 31:
#line 231 "mon_parse.y"
    { mon_cpuhistory(-1); }
    break;

  case 32:
#line 233 "mon_parse.y"
    { mon_cpuhistory((yyvsp[(3) - (4)].i)); }
    break;

  case 33:
#line 235 "mon_parse.y"
    { mon_instruction_return(); }
    break;

  case 34:
#line 237 "mon_parse.y"
    { machine_write_snapshot((yyvsp[(2) - (3)].str),0,0,0); /* FIXME */ }
    break;

  case 35:
#line 239 "mon_parse.y"
    { machine_read_snapshot((yyvsp[(2) - (3)].str), 0); }
    break;

  case 36:
#line 241 "mon_parse.y"
    { mon_instructions_step(-1); }
    break;

  case 37:
#line 243 "mon_parse.y"
    { mon_instructions_step((yyvsp[(3) - (4)].i)); }
    break;

  case 38:
#line 245 "mon_parse.y"
    { mon_instructions_next(-1); }
    break;

  case 39:
#line 247 "mon_parse.y"
    { mon_instructions_next((yyvsp[(3) - (4)].i)); }
    break;

  case 40:
#line 249 "mon_parse.y"
    { mon_stack_up(-1); }
    break;

  case 41:
#line 251 "mon_parse.y"
    { mon_stack_up((yyvsp[(3) - (4)].i)); }
    break;

  case 42:
#line 253 "mon_parse.y"
    { mon_stack_down(-1); }
    break;

  case 43:
#line 255 "mon_parse.y"
    { mon_stack_down((yyvsp[(3) - (4)].i)); }
    break;

  case 44:
#line 257 "mon_parse.y"
    { mon_display_screen(); }
    break;

  case 46:
#line 262 "mon_parse.y"
    { (monitor_cpu_for_memspace[default_memspace]->mon_register_print)(default_memspace); }
    break;

  case 47:
#line 264 "mon_parse.y"
    { (monitor_cpu_for_memspace[(yyvsp[(2) - (3)].i)]->mon_register_print)((yyvsp[(2) - (3)].i)); }
    break;

  case 49:
#line 269 "mon_parse.y"
    {
                        /* What about the memspace? */
                        mon_playback_init((yyvsp[(4) - (5)].str));
                    }
    break;

  case 50:
#line 274 "mon_parse.y"
    {
                        /* What about the memspace? */
                        mon_playback_init((yyvsp[(2) - (3)].str));
                    }
    break;

  case 51:
#line 279 "mon_parse.y"
    { mon_save_symbols((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 52:
#line 281 "mon_parse.y"
    { mon_save_symbols(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 53:
#line 283 "mon_parse.y"
    { mon_add_name_to_symbol_table((yyvsp[(2) - (5)].a), (yyvsp[(4) - (5)].str)); }
    break;

  case 54:
#line 285 "mon_parse.y"
    { mon_remove_name_from_symbol_table(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 55:
#line 287 "mon_parse.y"
    { mon_remove_name_from_symbol_table((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 56:
#line 289 "mon_parse.y"
    { mon_print_symbol_table((yyvsp[(2) - (3)].i)); }
    break;

  case 57:
#line 291 "mon_parse.y"
    { mon_print_symbol_table(e_default_space); }
    break;

  case 58:
#line 293 "mon_parse.y"
    {
                        mon_add_name_to_symbol_table((yyvsp[(3) - (4)].a), mon_prepend_dot_to_name((yyvsp[(1) - (4)].str)));
                    }
    break;

  case 59:
#line 297 "mon_parse.y"
    {
                        mon_add_name_to_symbol_table((yyvsp[(3) - (5)].a), mon_prepend_dot_to_name((yyvsp[(1) - (5)].str)));
                    }
    break;

  case 60:
#line 303 "mon_parse.y"
    { mon_start_assemble_mode((yyvsp[(2) - (2)].a), NULL); }
    break;

  case 61:
#line 304 "mon_parse.y"
    { }
    break;

  case 62:
#line 306 "mon_parse.y"
    { mon_start_assemble_mode((yyvsp[(2) - (3)].a), NULL); }
    break;

  case 63:
#line 308 "mon_parse.y"
    { mon_disassemble_lines((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1]); }
    break;

  case 64:
#line 310 "mon_parse.y"
    { mon_disassemble_lines(BAD_ADDR, BAD_ADDR); }
    break;

  case 65:
#line 314 "mon_parse.y"
    { mon_memory_move((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], (yyvsp[(4) - (5)].a)); }
    break;

  case 66:
#line 316 "mon_parse.y"
    { mon_memory_compare((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[0], (yyvsp[(4) - (5)].a)); }
    break;

  case 67:
#line 318 "mon_parse.y"
    { mon_memory_fill((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1],(unsigned char *)(yyvsp[(4) - (5)].str)); }
    break;

  case 68:
#line 320 "mon_parse.y"
    { mon_memory_hunt((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1],(unsigned char *)(yyvsp[(4) - (5)].str)); }
    break;

  case 69:
#line 322 "mon_parse.y"
    { mon_memory_display((yyvsp[(2) - (5)].rt), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], DF_PETSCII); }
    break;

  case 70:
#line 324 "mon_parse.y"
    { mon_memory_display(default_radix, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_PETSCII); }
    break;

  case 71:
#line 326 "mon_parse.y"
    { mon_memory_display(default_radix, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
    break;

  case 72:
#line 328 "mon_parse.y"
    { mon_memory_display_data((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], 8, 8); }
    break;

  case 73:
#line 330 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 8, 8); }
    break;

  case 74:
#line 332 "mon_parse.y"
    { mon_memory_display_data((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], 24, 21); }
    break;

  case 75:
#line 334 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 24, 21); }
    break;

  case 76:
#line 336 "mon_parse.y"
    { mon_memory_display(0, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_PETSCII); }
    break;

  case 77:
#line 338 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
    break;

  case 78:
#line 340 "mon_parse.y"
    { mon_memory_display(0, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_SCREEN_CODE); }
    break;

  case 79:
#line 342 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_SCREEN_CODE); }
    break;

  case 80:
#line 344 "mon_parse.y"
    { mon_memmap_zap(); }
    break;

  case 81:
#line 346 "mon_parse.y"
    { mon_memmap_show(-1,BAD_ADDR,BAD_ADDR); }
    break;

  case 82:
#line 348 "mon_parse.y"
    { mon_memmap_show((yyvsp[(3) - (4)].i),BAD_ADDR,BAD_ADDR); }
    break;

  case 83:
#line 350 "mon_parse.y"
    { mon_memmap_show((yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].range)[0],(yyvsp[(4) - (5)].range)[1]); }
    break;

  case 84:
#line 352 "mon_parse.y"
    { mon_memmap_save((yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].i)); }
    break;

  case 85:
#line 356 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], FALSE, FALSE,
                                                    FALSE, FALSE);
                  }
    break;

  case 86:
#line 361 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], FALSE, FALSE,
                                                    FALSE, TRUE);
                  }
    break;

  case 87:
#line 366 "mon_parse.y"
    {
                      temp = mon_breakpoint_add_checkpoint((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], FALSE,
                                                           FALSE, FALSE, FALSE);
                      mon_breakpoint_set_checkpoint_condition(temp, (yyvsp[(4) - (5)].cond_node));
                  }
    break;

  case 88:
#line 372 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], FALSE,
                      ((yyvsp[(2) - (5)].i) == e_load || (yyvsp[(2) - (5)].i) == e_load_store),
                      ((yyvsp[(2) - (5)].i) == e_store || (yyvsp[(2) - (5)].i) == e_load_store), FALSE);
                  }
    break;

  case 89:
#line 378 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], TRUE, FALSE, FALSE,
                                                    FALSE);
                  }
    break;

  case 90:
#line 383 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 91:
#line 385 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 92:
#line 387 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 93:
#line 389 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 94:
#line 394 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_ON, (yyvsp[(2) - (3)].i)); }
    break;

  case 95:
#line 396 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_OFF, (yyvsp[(2) - (3)].i)); }
    break;

  case 96:
#line 398 "mon_parse.y"
    { mon_breakpoint_set_ignore_count((yyvsp[(2) - (3)].i), -1); }
    break;

  case 97:
#line 400 "mon_parse.y"
    { mon_breakpoint_set_ignore_count((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].i)); }
    break;

  case 98:
#line 402 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint((yyvsp[(2) - (3)].i)); }
    break;

  case 99:
#line 404 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint(-1); }
    break;

  case 100:
#line 406 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_condition((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].cond_node)); }
    break;

  case 101:
#line 408 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_command((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 102:
#line 410 "mon_parse.y"
    { return ERR_EXPECT_STRING; }
    break;

  case 103:
#line 414 "mon_parse.y"
    { sidefx = (((yyvsp[(2) - (3)].action) == e_TOGGLE) ? (sidefx ^ 1) : (yyvsp[(2) - (3)].action)); }
    break;

  case 104:
#line 416 "mon_parse.y"
    {
                         mon_out("I/O side effects are %s\n",
                                   sidefx ? "enabled" : "disabled");
                     }
    break;

  case 105:
#line 421 "mon_parse.y"
    { default_radix = (yyvsp[(2) - (3)].rt); }
    break;

  case 106:
#line 423 "mon_parse.y"
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

  case 107:
#line 441 "mon_parse.y"
    { monitor_change_device((yyvsp[(2) - (3)].i)); }
    break;

  case 108:
#line 443 "mon_parse.y"
    { mon_quit(); YYACCEPT; }
    break;

  case 109:
#line 445 "mon_parse.y"
    { exit_mon = 1; YYACCEPT; }
    break;

  case 110:
#line 449 "mon_parse.y"
    { mon_drive_execute_disk_cmd((yyvsp[(2) - (3)].str)); }
    break;

  case 111:
#line 451 "mon_parse.y"
    { mon_out("\t%d\n",(yyvsp[(2) - (3)].i)); }
    break;

  case 112:
#line 453 "mon_parse.y"
    { mon_command_print_help(NULL); }
    break;

  case 113:
#line 455 "mon_parse.y"
    { mon_command_print_help((yyvsp[(2) - (3)].str)); }
    break;

  case 114:
#line 457 "mon_parse.y"
    { printf("SYSTEM COMMAND: %s\n",(yyvsp[(2) - (3)].str)); }
    break;

  case 115:
#line 459 "mon_parse.y"
    { mon_print_convert((yyvsp[(2) - (3)].i)); }
    break;

  case 116:
#line 461 "mon_parse.y"
    { mon_change_dir((yyvsp[(2) - (3)].str)); }
    break;

  case 117:
#line 463 "mon_parse.y"
    { mon_keyboard_feed((yyvsp[(2) - (3)].str)); }
    break;

  case 118:
#line 465 "mon_parse.y"
    { mon_backtrace(); }
    break;

  case 119:
#line 467 "mon_parse.y"
    { mon_show_dir((yyvsp[(2) - (3)].str)); }
    break;

  case 120:
#line 469 "mon_parse.y"
    { mon_show_pwd(); }
    break;

  case 121:
#line 471 "mon_parse.y"
    { mon_screenshot_save((yyvsp[(2) - (3)].str),-1); }
    break;

  case 122:
#line 473 "mon_parse.y"
    { mon_screenshot_save((yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].i)); }
    break;

  case 123:
#line 475 "mon_parse.y"
    { mon_resource_get((yyvsp[(2) - (3)].str)); }
    break;

  case 124:
#line 477 "mon_parse.y"
    { mon_resource_set((yyvsp[(2) - (4)].str),(yyvsp[(3) - (4)].str)); }
    break;

  case 125:
#line 479 "mon_parse.y"
    { mon_reset_machine(-1); }
    break;

  case 126:
#line 481 "mon_parse.y"
    { mon_reset_machine((yyvsp[(3) - (4)].i)); }
    break;

  case 127:
#line 483 "mon_parse.y"
    { mon_tape_ctrl((yyvsp[(3) - (4)].i)); }
    break;

  case 128:
#line 485 "mon_parse.y"
    { mon_cart_freeze(); }
    break;

  case 129:
#line 487 "mon_parse.y"
    { }
    break;

  case 130:
#line 491 "mon_parse.y"
    { mon_file_load((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].a), FALSE); }
    break;

  case 131:
#line 493 "mon_parse.y"
    { mon_file_load((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].a), TRUE); }
    break;

  case 132:
#line 495 "mon_parse.y"
    { mon_file_save((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], FALSE); }
    break;

  case 133:
#line 497 "mon_parse.y"
    { return ERR_EXPECT_DEVICE_NUM; }
    break;

  case 134:
#line 499 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 135:
#line 501 "mon_parse.y"
    { mon_file_save((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], TRUE); }
    break;

  case 136:
#line 503 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 137:
#line 505 "mon_parse.y"
    { mon_file_verify((yyvsp[(2) - (5)].str),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 138:
#line 507 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 139:
#line 509 "mon_parse.y"
    { mon_drive_block_cmd(0,(yyvsp[(2) - (5)].i),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 140:
#line 511 "mon_parse.y"
    { mon_drive_block_cmd(1,(yyvsp[(2) - (5)].i),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 141:
#line 513 "mon_parse.y"
    { mon_attach((yyvsp[(2) - (4)].str),(yyvsp[(3) - (4)].i)); }
    break;

  case 142:
#line 515 "mon_parse.y"
    { mon_detach((yyvsp[(2) - (3)].i)); }
    break;

  case 143:
#line 519 "mon_parse.y"
    { mon_record_commands((yyvsp[(2) - (3)].str)); }
    break;

  case 144:
#line 521 "mon_parse.y"
    { mon_end_recording(); }
    break;

  case 145:
#line 523 "mon_parse.y"
    { mon_playback_init((yyvsp[(2) - (3)].str)); }
    break;

  case 146:
#line 527 "mon_parse.y"
    { mon_memory_fill((yyvsp[(2) - (4)].a), BAD_ADDR, (unsigned char *)(yyvsp[(3) - (4)].str)); }
    break;

  case 147:
#line 529 "mon_parse.y"
    { printf("Not yet.\n"); }
    break;

  case 148:
#line 533 "mon_parse.y"
    { yydebug = 1; }
    break;

  case 149:
#line 536 "mon_parse.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 150:
#line 539 "mon_parse.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 151:
#line 540 "mon_parse.y"
    { (yyval.str) = NULL; }
    break;

  case 153:
#line 544 "mon_parse.y"
    { return ERR_EXPECT_FILENAME; }
    break;

  case 155:
#line 548 "mon_parse.y"
    { return ERR_EXPECT_DEVICE_NUM; }
    break;

  case 156:
#line 551 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 157:
#line 552 "mon_parse.y"
    { (yyval.i) = e_load_store; }
    break;

  case 158:
#line 555 "mon_parse.y"
    { (yyval.i) = new_reg(default_memspace, (yyvsp[(1) - (1)].reg)); }
    break;

  case 159:
#line 556 "mon_parse.y"
    { (yyval.i) = new_reg((yyvsp[(1) - (2)].i), (yyvsp[(2) - (2)].reg)); }
    break;

  case 162:
#line 564 "mon_parse.y"
    { (monitor_cpu_for_memspace[reg_memspace((yyvsp[(1) - (3)].i))]->mon_register_set_val)(reg_memspace((yyvsp[(1) - (3)].i)), reg_regid((yyvsp[(1) - (3)].i)), (WORD) (yyvsp[(3) - (3)].i)); }
    break;

  case 163:
#line 567 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 164:
#line 568 "mon_parse.y"
    { return ERR_EXPECT_BRKNUM; }
    break;

  case 166:
#line 572 "mon_parse.y"
    { (yyval.range)[0] = (yyvsp[(1) - (1)].a); (yyval.range)[1] = BAD_ADDR; }
    break;

  case 167:
#line 575 "mon_parse.y"
    { (yyval.range)[0] = (yyvsp[(1) - (3)].a); (yyval.range)[1] = (yyvsp[(3) - (3)].a); }
    break;

  case 168:
#line 577 "mon_parse.y"
    { if (resolve_range(e_default_space, (yyval.range), (yyvsp[(1) - (1)].str))) return ERR_ADDR_TOO_BIG; }
    break;

  case 169:
#line 579 "mon_parse.y"
    { if (resolve_range((yyvsp[(1) - (3)].i), (yyval.range), (yyvsp[(3) - (3)].str))) return ERR_ADDR_TOO_BIG; }
    break;

  case 170:
#line 582 "mon_parse.y"
    { (yyval.a) = (yyvsp[(2) - (2)].a); }
    break;

  case 171:
#line 583 "mon_parse.y"
    { (yyval.a) = BAD_ADDR; }
    break;

  case 172:
#line 587 "mon_parse.y"
    {
             (yyval.a) = new_addr(e_default_space,(yyvsp[(1) - (1)].i));
             if (opt_asm) new_cmd = asm_mode = 1; 
         }
    break;

  case 173:
#line 592 "mon_parse.y"
    {
             (yyval.a) = new_addr((yyvsp[(1) - (3)].i), (yyvsp[(3) - (3)].i));
             if (opt_asm) new_cmd = asm_mode = 1; 
         }
    break;

  case 174:
#line 597 "mon_parse.y"
    {
             temp = mon_symbol_table_lookup_addr(e_default_space, (yyvsp[(1) - (1)].str));
             if (temp >= 0)
                 (yyval.a) = new_addr(e_default_space, temp);
             else
                 return ERR_UNDEFINED_LABEL;
         }
    break;

  case 177:
#line 610 "mon_parse.y"
    { (yyval.i) = e_comp_space; }
    break;

  case 178:
#line 611 "mon_parse.y"
    { (yyval.i) = e_disk8_space; }
    break;

  case 179:
#line 612 "mon_parse.y"
    { (yyval.i) = e_disk9_space; }
    break;

  case 180:
#line 613 "mon_parse.y"
    { (yyval.i) = e_disk10_space; }
    break;

  case 181:
#line 614 "mon_parse.y"
    { (yyval.i) = e_disk11_space; }
    break;

  case 182:
#line 617 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); if (!CHECK_ADDR((yyvsp[(1) - (1)].i))) return ERR_ADDR_TOO_BIG; }
    break;

  case 183:
#line 619 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 184:
#line 621 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) + (yyvsp[(3) - (3)].i); }
    break;

  case 185:
#line 622 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) - (yyvsp[(3) - (3)].i); }
    break;

  case 186:
#line 623 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) * (yyvsp[(3) - (3)].i); }
    break;

  case 187:
#line 624 "mon_parse.y"
    { (yyval.i) = ((yyvsp[(3) - (3)].i)) ? ((yyvsp[(1) - (3)].i) / (yyvsp[(3) - (3)].i)) : 1; }
    break;

  case 188:
#line 625 "mon_parse.y"
    { (yyval.i) = (yyvsp[(2) - (3)].i); }
    break;

  case 189:
#line 626 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 190:
#line 627 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 191:
#line 631 "mon_parse.y"
    {
               (yyval.cond_node) = new_cond; (yyval.cond_node)->is_parenthized = FALSE;
               (yyval.cond_node)->child1 = (yyvsp[(1) - (3)].cond_node); (yyval.cond_node)->child2 = (yyvsp[(3) - (3)].cond_node); (yyval.cond_node)->operation = (yyvsp[(2) - (3)].cond_op);
           }
    break;

  case 192:
#line 636 "mon_parse.y"
    { return ERR_INCOMPLETE_COMPARE_OP; }
    break;

  case 193:
#line 638 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(2) - (3)].cond_node); (yyval.cond_node)->is_parenthized = TRUE; }
    break;

  case 194:
#line 640 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 195:
#line 642 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(1) - (1)].cond_node); }
    break;

  case 196:
#line 645 "mon_parse.y"
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->reg_num = (yyvsp[(1) - (1)].i); (yyval.cond_node)->is_reg = TRUE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
    break;

  case 197:
#line 651 "mon_parse.y"
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->value = (yyvsp[(1) - (1)].i); (yyval.cond_node)->is_reg = FALSE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
    break;

  case 200:
#line 663 "mon_parse.y"
    { mon_add_number_to_buffer((yyvsp[(1) - (1)].i)); }
    break;

  case 201:
#line 664 "mon_parse.y"
    { mon_add_string_to_buffer((yyvsp[(1) - (1)].str)); }
    break;

  case 204:
#line 671 "mon_parse.y"
    { mon_add_number_to_buffer((yyvsp[(1) - (1)].i)); }
    break;

  case 205:
#line 672 "mon_parse.y"
    { mon_add_number_masked_to_buffer((yyvsp[(1) - (1)].i), 0x00); }
    break;

  case 206:
#line 673 "mon_parse.y"
    { mon_add_string_to_buffer((yyvsp[(1) - (1)].str)); }
    break;

  case 207:
#line 676 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 208:
#line 677 "mon_parse.y"
    { (yyval.i) = (monitor_cpu_for_memspace[reg_memspace((yyvsp[(1) - (1)].i))]->mon_register_get_val)(reg_memspace((yyvsp[(1) - (1)].i)), reg_regid((yyvsp[(1) - (1)].i))); }
    break;

  case 209:
#line 680 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 210:
#line 681 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 211:
#line 682 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 212:
#line 683 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 213:
#line 686 "mon_parse.y"
    { (yyval.i) = resolve_datatype(B_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 214:
#line 687 "mon_parse.y"
    { (yyval.i) = resolve_datatype(O_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 215:
#line 688 "mon_parse.y"
    { (yyval.i) = resolve_datatype(D_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 216:
#line 691 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 217:
#line 692 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 218:
#line 693 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 219:
#line 694 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 220:
#line 695 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 224:
#line 703 "mon_parse.y"
    { (yyval.i) = 0;
                                                if ((yyvsp[(1) - (2)].str)) {
                                                    (monitor_cpu_for_memspace[default_memspace]->mon_assemble_instr)((yyvsp[(1) - (2)].str), (yyvsp[(2) - (2)].i));
                                                } else {
                                                    new_cmd = 1;
                                                    asm_mode = 0;
                                                }
                                                opt_asm = 0;
                                              }
    break;

  case 226:
#line 714 "mon_parse.y"
    { asm_mode = 0; }
    break;

  case 227:
#line 717 "mon_parse.y"
    { if ((yyvsp[(2) - (2)].i) > 0xff)
                          (yyval.i) = join_ints(ASM_ADDR_MODE_IMMEDIATE_16,(yyvsp[(2) - (2)].i));
                        else
                          (yyval.i) = join_ints(ASM_ADDR_MODE_IMMEDIATE,(yyvsp[(2) - (2)].i)); }
    break;

  case 228:
#line 721 "mon_parse.y"
    { if ((yyvsp[(1) - (1)].i) < 0x100)
               (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE,(yyvsp[(1) - (1)].i));
             else
               (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE,(yyvsp[(1) - (1)].i));
           }
    break;

  case 229:
#line 726 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100)
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE_X,(yyvsp[(1) - (3)].i));
                          else
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_X,(yyvsp[(1) - (3)].i));
                        }
    break;

  case 230:
#line 731 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100)
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE_Y,(yyvsp[(1) - (3)].i));
                          else
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_Y,(yyvsp[(1) - (3)].i));
                        }
    break;

  case 231:
#line 737 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABS_INDIRECT,(yyvsp[(2) - (3)].i)); }
    break;

  case 232:
#line 739 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_INDIRECT_X,(yyvsp[(2) - (5)].i)); }
    break;

  case 233:
#line 741 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_INDIRECT_Y,(yyvsp[(2) - (5)].i)); }
    break;

  case 234:
#line 742 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_BC,0); }
    break;

  case 235:
#line 743 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_DE,0); }
    break;

  case 236:
#line 744 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_HL,0); }
    break;

  case 237:
#line 745 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_IX,0); }
    break;

  case 238:
#line 746 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_IY,0); }
    break;

  case 239:
#line 747 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_SP,0); }
    break;

  case 240:
#line 749 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_A,(yyvsp[(2) - (5)].i)); }
    break;

  case 241:
#line 751 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_HL,(yyvsp[(2) - (5)].i)); }
    break;

  case 242:
#line 753 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_IX,(yyvsp[(2) - (5)].i)); }
    break;

  case 243:
#line 755 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_IY,(yyvsp[(2) - (5)].i)); }
    break;

  case 244:
#line 756 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_IMPLIED,0); }
    break;

  case 245:
#line 757 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ACCUMULATOR,0); }
    break;

  case 246:
#line 758 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_B,0); }
    break;

  case 247:
#line 759 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_C,0); }
    break;

  case 248:
#line 760 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_D,0); }
    break;

  case 249:
#line 761 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_E,0); }
    break;

  case 250:
#line 762 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_H,0); }
    break;

  case 251:
#line 763 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IXH,0); }
    break;

  case 252:
#line 764 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IYH,0); }
    break;

  case 253:
#line 765 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_L,0); }
    break;

  case 254:
#line 766 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IXL,0); }
    break;

  case 255:
#line 767 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IYL,0); }
    break;

  case 256:
#line 768 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_AF,0); }
    break;

  case 257:
#line 769 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_BC,0); }
    break;

  case 258:
#line 770 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_DE,0); }
    break;

  case 259:
#line 771 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_HL,0); }
    break;

  case 260:
#line 772 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IX,0); }
    break;

  case 261:
#line 773 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IY,0); }
    break;

  case 262:
#line 774 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_SP,0); }
    break;


/* Line 1267 of yacc.c.  */
#line 3693 "mon_parse.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

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
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 778 "mon_parse.y"


void parse_and_execute_line(char *input)
{
   char *temp_buf;
   int i, rc;

   temp_buf = lib_malloc(strlen(input) + 3);
   strcpy(temp_buf,input);
   i = (int)strlen(input);
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
         case ERR_EXPECT_DEVICE_NUM:
           mon_out("Expecting a device number.\n");
           break;
         case ERR_EXPECT_ADDRESS:
           mon_out("Expecting an address.\n");
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
   lib_free(temp_buf);
   free_buffer();
}

static int yyerror(char *s)
{
   fprintf(stderr, "ERR:%s\n", s);
   return 0;
}

static int resolve_datatype(unsigned guess_type, const char *num)
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

/*
 * Resolve a character sequence containing 8 hex digits like "08001000". 
 * This could be a lazy version of "0800 1000". If the default radix is not
 * hexadecimal, we handle it like a ordinary number, in the latter case there
 * is only one number in the range.
 */
static int resolve_range(enum t_memspace memspace, MON_ADDR range[2], 
                         const char *num)
{
    char start[5];
    char end[5];
    long sa;

    range[1] = BAD_ADDR;

    switch (default_radix)
    {
    case e_hexadecimal:
        /* checked twice, but as the code must have exactly 8 digits: */
        if (strlen(num) == 8) {
            memcpy(start, num, 4);
            start[4] = '\0';
            memcpy(end, num + 4, 4);
            end[4] = '\0';
            sa = strtol(start, NULL, 16);
            range[1] = new_addr(memspace, strtol(end, NULL, 16));
        }
        else
            sa = strtol(num, NULL, 16);
        break;

    case e_decimal:
       sa = strtol(num, NULL, 10);
       break;

    case e_octal:
       sa = strtol(num, NULL, 8);
       break;

    default:
       sa = strtol(num, NULL, 2);
    }

    if (!CHECK_ADDR(sa))
        return ERR_ADDR_TOO_BIG;

    range[0] = new_addr(memspace, sa);
    return 0;
}

/* If we want 'quit' for OS/2 I couldn't leave the emulator by calling exit(0)
   So I decided to skip this (I think it's unnecessary for OS/2 */
static void mon_quit(void)
{
#ifdef OS2
    /* same as "quit" */
    exit_mon = 1; 
#else
    exit_mon = 2;
#endif
}


