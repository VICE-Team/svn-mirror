
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
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



/* Line 189 of yacc.c  */
#line 200 "mon_parse.c"

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
     CMD_LIST = 360,
     CMD_LABEL_ASGN = 361,
     L_PAREN = 362,
     R_PAREN = 363,
     ARG_IMMEDIATE = 364,
     REG_A = 365,
     REG_X = 366,
     REG_Y = 367,
     COMMA = 368,
     INST_SEP = 369,
     REG_B = 370,
     REG_C = 371,
     REG_D = 372,
     REG_E = 373,
     REG_H = 374,
     REG_L = 375,
     REG_AF = 376,
     REG_BC = 377,
     REG_DE = 378,
     REG_HL = 379,
     REG_IX = 380,
     REG_IY = 381,
     REG_SP = 382,
     REG_IXH = 383,
     REG_IXL = 384,
     REG_IYH = 385,
     REG_IYL = 386,
     STRING = 387,
     FILENAME = 388,
     R_O_L = 389,
     OPCODE = 390,
     LABEL = 391,
     BANKNAME = 392,
     CPUTYPE = 393,
     MON_REGISTER = 394,
     COMPARE_OP = 395,
     RADIX_TYPE = 396,
     INPUT_SPEC = 397,
     CMD_CHECKPT_ON = 398,
     CMD_CHECKPT_OFF = 399,
     TOGGLE = 400,
     MASK = 401
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
#define CMD_LIST 360
#define CMD_LABEL_ASGN 361
#define L_PAREN 362
#define R_PAREN 363
#define ARG_IMMEDIATE 364
#define REG_A 365
#define REG_X 366
#define REG_Y 367
#define COMMA 368
#define INST_SEP 369
#define REG_B 370
#define REG_C 371
#define REG_D 372
#define REG_E 373
#define REG_H 374
#define REG_L 375
#define REG_AF 376
#define REG_BC 377
#define REG_DE 378
#define REG_HL 379
#define REG_IX 380
#define REG_IY 381
#define REG_SP 382
#define REG_IXH 383
#define REG_IXL 384
#define REG_IYH 385
#define REG_IYL 386
#define STRING 387
#define FILENAME 388
#define R_O_L 389
#define OPCODE 390
#define LABEL 391
#define BANKNAME 392
#define CPUTYPE 393
#define MON_REGISTER 394
#define COMPARE_OP 395
#define RADIX_TYPE 396
#define INPUT_SPEC 397
#define CMD_CHECKPT_ON 398
#define CMD_CHECKPT_OFF 399
#define TOGGLE 400
#define MASK 401




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 127 "mon_parse.y"

    MON_ADDR a;
    MON_ADDR range[2];
    int i;
    REG_ID reg;
    CONDITIONAL cond_op;
    cond_node_t *cond_node;
    RADIXTYPE rt;
    ACTION action;
    char *str;



/* Line 214 of yacc.c  */
#line 542 "mon_parse.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 554 "mon_parse.c"

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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  289
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1713

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  153
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  51
/* YYNRULES -- Number of rules.  */
#define YYNRULES  265
/* YYNRULES -- Number of states.  */
#define YYNSTATES  524

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   401

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     151,   152,   149,   147,     2,   148,     2,   150,     2,     2,
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
     145,   146
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    15,    17,    19,
      21,    23,    25,    27,    29,    31,    33,    35,    37,    39,
      41,    43,    45,    47,    50,    54,    58,    64,    68,    71,
      75,    78,    82,    85,    90,    93,    97,   101,   104,   109,
     112,   117,   120,   125,   128,   133,   136,   138,   141,   145,
     149,   155,   159,   165,   169,   175,   179,   185,   189,   192,
     197,   203,   204,   210,   214,   218,   221,   227,   233,   239,
     245,   251,   255,   258,   262,   265,   269,   272,   276,   279,
     283,   286,   289,   292,   297,   303,   309,   313,   317,   323,
     329,   333,   336,   339,   342,   345,   349,   353,   357,   363,
     367,   370,   376,   382,   387,   391,   394,   398,   401,   405,
     408,   411,   415,   419,   422,   426,   430,   434,   438,   442,
     445,   449,   452,   456,   462,   466,   471,   474,   479,   484,
     487,   491,   497,   503,   509,   513,   518,   524,   529,   535,
     540,   546,   552,   555,   559,   564,   568,   572,   575,   579,
     584,   587,   590,   592,   594,   595,   597,   599,   601,   603,
     605,   606,   608,   611,   615,   617,   621,   623,   625,   627,
     629,   633,   635,   639,   642,   643,   645,   649,   651,   653,
     654,   656,   658,   660,   662,   664,   666,   668,   672,   676,
     680,   684,   688,   692,   694,   698,   702,   706,   710,   712,
     714,   716,   720,   722,   724,   726,   729,   731,   733,   735,
     737,   739,   741,   743,   745,   747,   749,   751,   753,   755,
     757,   759,   761,   763,   765,   769,   773,   776,   779,   781,
     783,   786,   788,   792,   796,   800,   806,   812,   816,   820,
     824,   828,   832,   836,   842,   848,   854,   860,   861,   863,
     865,   867,   869,   871,   873,   875,   877,   879,   881,   883,
     885,   887,   889,   891,   893,   895
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     154,     0,    -1,   155,    -1,   201,    22,    -1,    22,    -1,
     157,    -1,   155,   157,    -1,    23,    -1,    22,    -1,     1,
      -1,   158,    -1,   160,    -1,   163,    -1,   161,    -1,   164,
      -1,   165,    -1,   166,    -1,   167,    -1,   168,    -1,   169,
      -1,   170,    -1,   171,    -1,    13,    -1,    67,   156,    -1,
      67,   186,   156,    -1,    67,   137,   156,    -1,    67,   186,
     185,   137,   156,    -1,    38,   184,   156,    -1,    46,   156,
      -1,    46,   184,   156,    -1,    87,   156,    -1,    87,   138,
     156,    -1,   100,   156,    -1,   100,   185,   189,   156,    -1,
      26,   156,    -1,    49,   174,   156,    -1,    50,   174,   156,
      -1,    58,   156,    -1,    58,   185,   189,   156,    -1,    57,
     156,    -1,    57,   185,   189,   156,    -1,    29,   156,    -1,
      29,   185,   189,   156,    -1,    30,   156,    -1,    30,   185,
     189,   156,    -1,    85,   156,    -1,   159,    -1,    39,   156,
      -1,    39,   186,   156,    -1,    39,   178,   156,    -1,    68,
     186,   185,   174,   156,    -1,    68,   174,   156,    -1,    69,
     186,   185,   174,   156,    -1,    69,   174,   156,    -1,    70,
     184,   185,   136,   156,    -1,    71,   136,   156,    -1,    71,
     186,   185,   136,   156,    -1,    72,   186,   156,    -1,    72,
     156,    -1,   106,    21,   184,   156,    -1,   106,    21,   184,
      24,   156,    -1,    -1,    55,   184,   162,   202,   156,    -1,
      55,   184,   156,    -1,    56,   181,   156,    -1,    56,   156,
      -1,    37,   182,   185,   184,   156,    -1,    48,   182,   185,
     184,   156,    -1,    36,   182,   185,   192,   156,    -1,    35,
     182,   185,   194,   156,    -1,    43,   141,   185,   181,   156,
      -1,    43,   181,   156,    -1,    43,   156,    -1,    76,   181,
     156,    -1,    76,   156,    -1,    77,   181,   156,    -1,    77,
     156,    -1,    78,   181,   156,    -1,    78,   156,    -1,    79,
     181,   156,    -1,    79,   156,    -1,   101,   156,    -1,   102,
     156,    -1,   102,   185,   189,   156,    -1,   102,   185,   189,
     181,   156,    -1,   103,   174,   185,   189,   156,    -1,    44,
     181,   156,    -1,    86,   181,   156,    -1,    44,   181,    15,
     190,   156,    -1,    62,   176,   185,   181,   156,    -1,    45,
     181,   156,    -1,    44,   156,    -1,    86,   156,    -1,    45,
     156,    -1,    62,   156,    -1,   143,   180,   156,    -1,   144,
     180,   156,    -1,    34,   180,   156,    -1,    34,   180,   185,
     189,   156,    -1,    52,   180,   156,    -1,    52,   156,    -1,
      53,   180,    15,   190,   156,    -1,    54,   180,   185,   132,
     156,    -1,    54,   180,     1,   156,    -1,    25,   145,   156,
      -1,    25,   156,    -1,    42,   141,   156,    -1,    42,   156,
      -1,    60,   186,   156,    -1,    65,   156,    -1,    51,   156,
      -1,    63,   172,   156,    -1,    59,   189,   156,    -1,    61,
     156,    -1,    61,   172,   156,    -1,    64,   172,   156,    -1,
       7,   189,   156,    -1,    66,   172,   156,    -1,    82,   172,
     156,    -1,    89,   156,    -1,    92,   173,   156,    -1,    91,
     156,    -1,    90,   174,   156,    -1,    90,   174,   185,   189,
     156,    -1,    93,   132,   156,    -1,    94,   132,   132,   156,
      -1,    97,   156,    -1,    97,   185,   189,   156,    -1,    98,
     185,   189,   156,    -1,    99,   156,    -1,   104,   173,   156,
      -1,    31,   174,   175,   183,   156,    -1,    83,   174,   175,
     183,   156,    -1,    32,   174,   175,   182,   156,    -1,    32,
     174,     1,    -1,    32,   174,   175,     1,    -1,    84,   174,
     175,   182,   156,    -1,    84,   174,   175,     1,    -1,    33,
     174,   175,   184,   156,    -1,    33,   174,   175,     1,    -1,
      27,   189,   189,   183,   156,    -1,    28,   189,   189,   184,
     156,    -1,   105,   156,    -1,   105,   175,   156,    -1,    95,
     174,   189,   156,    -1,    96,   189,   156,    -1,    73,   174,
     156,    -1,    74,   156,    -1,    75,   174,   156,    -1,    80,
     184,   192,   156,    -1,    81,   156,    -1,    88,   156,    -1,
     134,    -1,   134,    -1,    -1,   133,    -1,     1,    -1,   189,
      -1,     1,    -1,    14,    -1,    -1,   139,    -1,   186,   139,
      -1,   178,   113,   179,    -1,   179,    -1,   177,    21,   199,
      -1,   197,    -1,     1,    -1,   182,    -1,   184,    -1,   184,
     185,   184,    -1,     9,    -1,   186,   185,     9,    -1,   185,
     184,    -1,    -1,   187,    -1,   186,   185,   187,    -1,   136,
      -1,   113,    -1,    -1,    16,    -1,    17,    -1,    18,    -1,
      19,    -1,    20,    -1,   188,    -1,   199,    -1,   189,   147,
     189,    -1,   189,   148,   189,    -1,   189,   149,   189,    -1,
     189,   150,   189,    -1,   151,   189,   152,    -1,   151,   189,
       1,    -1,   196,    -1,   190,   140,   190,    -1,   190,   140,
       1,    -1,   107,   190,   108,    -1,   107,   190,     1,    -1,
     191,    -1,   177,    -1,   199,    -1,   192,   185,   193,    -1,
     193,    -1,   199,    -1,   132,    -1,   194,   195,    -1,   195,
      -1,   199,    -1,   146,    -1,   132,    -1,   199,    -1,   177,
      -1,     4,    -1,    12,    -1,    11,    -1,    10,    -1,    12,
      -1,    11,    -1,    10,    -1,     3,    -1,     4,    -1,     5,
      -1,     6,    -1,   198,    -1,   200,   114,   201,    -1,   201,
     114,   201,    -1,   201,   114,    -1,   135,   203,    -1,   201,
      -1,   200,    -1,   109,   199,    -1,   199,    -1,   199,   113,
     111,    -1,   199,   113,   112,    -1,   107,   199,   108,    -1,
     107,   199,   113,   111,   108,    -1,   107,   199,   108,   113,
     112,    -1,   107,   122,   108,    -1,   107,   123,   108,    -1,
     107,   124,   108,    -1,   107,   125,   108,    -1,   107,   126,
     108,    -1,   107,   127,   108,    -1,   107,   199,   108,   113,
     110,    -1,   107,   199,   108,   113,   124,    -1,   107,   199,
     108,   113,   125,    -1,   107,   199,   108,   113,   126,    -1,
      -1,   110,    -1,   115,    -1,   116,    -1,   117,    -1,   118,
      -1,   119,    -1,   128,    -1,   130,    -1,   120,    -1,   129,
      -1,   131,    -1,   121,    -1,   122,    -1,   123,    -1,   124,
      -1,   125,    -1,   126,    -1,   127,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   185,   185,   186,   187,   190,   191,   194,   195,   196,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   214,   216,   218,   220,   222,   224,   226,
     228,   230,   232,   234,   236,   238,   240,   242,   244,   246,
     248,   250,   252,   254,   256,   258,   260,   263,   265,   267,
     270,   275,   280,   282,   284,   286,   288,   290,   292,   294,
     298,   305,   304,   307,   309,   311,   315,   317,   319,   321,
     323,   325,   327,   329,   331,   333,   335,   337,   339,   341,
     343,   345,   347,   349,   351,   353,   357,   362,   367,   373,
     379,   384,   386,   388,   390,   395,   397,   399,   401,   403,
     405,   407,   409,   411,   415,   417,   422,   424,   442,   444,
     446,   450,   452,   454,   456,   458,   460,   462,   464,   466,
     468,   470,   472,   474,   476,   478,   480,   482,   484,   486,
     488,   492,   494,   496,   498,   500,   502,   504,   506,   508,
     510,   512,   514,   516,   518,   520,   524,   526,   528,   532,
     534,   538,   542,   545,   546,   549,   550,   553,   554,   557,
     558,   561,   562,   565,   566,   569,   573,   574,   577,   578,
     581,   582,   584,   588,   589,   592,   597,   602,   612,   613,
     616,   617,   618,   619,   620,   623,   625,   627,   628,   629,
     630,   631,   632,   633,   636,   641,   643,   645,   647,   651,
     657,   665,   666,   669,   670,   673,   674,   677,   678,   679,
     682,   683,   686,   687,   688,   689,   692,   693,   694,   697,
     698,   699,   700,   701,   704,   705,   706,   709,   719,   720,
     723,   727,   732,   737,   742,   744,   746,   748,   749,   750,
     751,   752,   753,   754,   756,   758,   760,   762,   763,   764,
     765,   766,   767,   768,   769,   770,   771,   772,   773,   774,
     775,   776,   777,   778,   779,   780
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
  "CMD_LIST", "CMD_LABEL_ASGN", "L_PAREN", "R_PAREN", "ARG_IMMEDIATE",
  "REG_A", "REG_X", "REG_Y", "COMMA", "INST_SEP", "REG_B", "REG_C",
  "REG_D", "REG_E", "REG_H", "REG_L", "REG_AF", "REG_BC", "REG_DE",
  "REG_HL", "REG_IX", "REG_IY", "REG_SP", "REG_IXH", "REG_IXL", "REG_IYH",
  "REG_IYL", "STRING", "FILENAME", "R_O_L", "OPCODE", "LABEL", "BANKNAME",
  "CPUTYPE", "MON_REGISTER", "COMPARE_OP", "RADIX_TYPE", "INPUT_SPEC",
  "CMD_CHECKPT_ON", "CMD_CHECKPT_OFF", "TOGGLE", "MASK", "'+'", "'-'",
  "'*'", "'/'", "'('", "')'", "$accept", "top_level", "command_list",
  "end_cmd", "command", "machine_state_rules", "register_mod",
  "symbol_table_rules", "asm_rules", "$@1", "memory_rules",
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
     395,   396,   397,   398,   399,   400,   401,    43,    45,    42,
      47,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   153,   154,   154,   154,   155,   155,   156,   156,   156,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   159,   159,   159,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   162,   161,   161,   161,   161,   163,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   166,   166,   166,   166,   166,   166,
     166,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   168,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   169,   169,   169,   170,
     170,   171,   172,   173,   173,   174,   174,   175,   175,   176,
     176,   177,   177,   178,   178,   179,   180,   180,   181,   181,
     182,   182,   182,   183,   183,   184,   184,   184,   185,   185,
     186,   186,   186,   186,   186,   187,   188,   189,   189,   189,
     189,   189,   189,   189,   190,   190,   190,   190,   190,   191,
     191,   192,   192,   193,   193,   194,   194,   195,   195,   195,
     196,   196,   197,   197,   197,   197,   198,   198,   198,   199,
     199,   199,   199,   199,   200,   200,   200,   201,   202,   202,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     3,     5,     3,     2,     3,
       2,     3,     2,     4,     2,     3,     3,     2,     4,     2,
       4,     2,     4,     2,     4,     2,     1,     2,     3,     3,
       5,     3,     5,     3,     5,     3,     5,     3,     2,     4,
       5,     0,     5,     3,     3,     2,     5,     5,     5,     5,
       5,     3,     2,     3,     2,     3,     2,     3,     2,     3,
       2,     2,     2,     4,     5,     5,     3,     3,     5,     5,
       3,     2,     2,     2,     2,     3,     3,     3,     5,     3,
       2,     5,     5,     4,     3,     2,     3,     2,     3,     2,
       2,     3,     3,     2,     3,     3,     3,     3,     3,     2,
       3,     2,     3,     5,     3,     4,     2,     4,     4,     2,
       3,     5,     5,     5,     3,     4,     5,     4,     5,     4,
       5,     5,     2,     3,     4,     3,     3,     2,     3,     4,
       2,     2,     1,     1,     0,     1,     1,     1,     1,     1,
       0,     1,     2,     3,     1,     3,     1,     1,     1,     1,
       3,     1,     3,     2,     0,     1,     3,     1,     1,     0,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     1,     3,     3,     3,     3,     1,     1,
       1,     3,     1,     1,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     2,     2,     1,     1,
       2,     1,     3,     3,     3,     5,     5,     3,     3,     3,
       3,     3,     3,     5,     5,     5,     5,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1
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
       0,     0,     0,     0,     0,     0,     0,     0,   154,     0,
       0,     0,     0,     0,   179,     0,     0,     0,     0,     0,
     154,     0,     0,   247,     0,     0,     0,     2,     5,    10,
      46,    11,    13,    12,    14,    15,    16,    17,    18,    19,
      20,    21,     0,   219,   220,   221,   222,   218,   217,   216,
     180,   181,   182,   183,   184,   161,     0,   211,     0,     0,
     193,   223,   210,     9,     8,     7,     0,   105,    34,     0,
       0,   178,    41,     0,    43,     0,   156,   155,     0,     0,
       0,   167,   212,   215,   214,   213,     0,   166,   171,   177,
     179,   179,   179,   175,   185,   186,   179,   179,     0,   179,
      47,     0,     0,   164,     0,     0,   107,   179,    72,     0,
     168,   179,    91,     0,    93,     0,    28,     0,   179,     0,
       0,   110,     9,   100,     0,     0,     0,     0,    65,     0,
      39,     0,    37,     0,     0,     0,   152,   113,     0,   159,
      94,   179,     0,     0,   109,     0,     0,    23,     0,     0,
     179,     0,   179,   179,     0,   179,    58,     0,     0,   147,
       0,    74,     0,    76,     0,    78,     0,    80,     0,     0,
     150,     0,     0,     0,    45,    92,     0,     0,    30,   151,
     119,     0,   121,   153,     0,     0,     0,     0,     0,   126,
       0,     0,   129,    32,     0,    81,    82,     0,   179,     0,
       9,   142,     0,   157,     0,     0,     0,   248,   249,   250,
     251,   252,   253,   256,   259,   260,   261,   262,   263,   264,
     265,   254,   257,   255,   258,   231,   227,     0,     0,     1,
       6,     3,     0,   162,     0,     0,     0,     0,   116,   104,
     179,     0,     0,     0,   158,   179,   134,     0,     0,    97,
       0,     0,     0,     0,     0,     0,    27,     0,     0,     0,
      49,    48,   106,     0,    71,     0,    86,    90,    29,     0,
      35,    36,    99,     0,     0,     0,    63,     0,    64,     0,
       0,   112,   108,   114,     0,   111,   115,   117,    25,    24,
       0,    51,     0,    53,     0,     0,    55,     0,    57,   146,
     148,    73,    75,    77,    79,   204,     0,   202,   203,   118,
     179,     0,    87,    31,   122,     0,   120,   124,     0,     0,
     145,     0,     0,     0,     0,     0,   130,   143,     0,     0,
       0,     0,     0,     0,     0,     0,   230,     0,    95,    96,
     192,   191,   187,   188,   189,   190,     0,     0,     0,    42,
      44,     0,   135,     0,   139,     0,     0,   209,   208,     0,
     206,   207,   170,   172,   176,     0,     0,   165,   163,     0,
       0,   199,     0,   198,   200,     0,     0,   103,     0,   229,
     228,     0,    40,    38,     0,     0,     0,     0,     0,     0,
     149,     0,     0,   137,     0,     0,   125,   144,   127,   128,
      33,    83,     0,     0,     0,    59,   237,   238,   239,   240,
     241,   242,   234,     0,   232,   233,   140,   173,   141,   131,
     133,   138,    98,    69,   205,    68,    66,    70,     0,     0,
      88,    67,   101,   102,     0,   226,    62,    89,    26,    50,
      52,    54,    56,   201,   132,   136,   123,    84,    85,    60,
       0,     0,   197,   196,   195,   194,   224,   225,   243,   236,
     244,   245,   246,   235
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    86,    87,   127,    88,    89,    90,    91,    92,   337,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   198,
     244,   138,   262,   201,   117,   162,   163,   146,   169,   170,
     406,   171,   407,   118,   153,   154,   263,   432,   433,   366,
     367,   419,   420,   120,   147,   121,   155,   439,   102,   441,
     286
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -330
static const yytype_int16 yypact[] =
{
    1130,   913,  -330,  -330,    13,   232,   913,   913,    86,    86,
      36,    36,    36,    14,  1530,  1530,  1530,  1553,   612,    18,
     997,  1409,  1409,  1432,  1530,    36,    36,   232,   870,    14,
      14,  1553,  1409,    86,    86,   913,   580,   165,  1386,  -122,
    -122,   232,  -122,   416,   287,   287,  1553,   405,   691,    36,
     232,    36,  1409,  1409,  1409,  1409,  1553,   232,  -122,    36,
      36,   232,  1409,    37,   232,   232,    36,   232,  -112,  -103,
    -101,    36,   913,    86,   -74,   232,    86,   232,    86,    36,
    -112,   663,    25,  1575,    14,    14,    48,  1250,  -330,  -330,
    -330,  -330,  -330,  -330,  -330,  -330,  -330,  -330,  -330,  -330,
    -330,  -330,    39,  -330,  -330,  -330,  -330,  -330,  -330,  -330,
    -330,  -330,  -330,  -330,  -330,  -330,   913,  -330,   -70,   259,
    -330,  -330,  -330,  -330,  -330,  -330,   232,  -330,  -330,   846,
     846,  -330,  -330,   913,  -330,   913,  -330,  -330,    46,   788,
      46,  -330,  -330,  -330,  -330,  -330,    86,  -330,  -330,  -330,
     -74,   -74,   -74,  -330,  -330,  -330,   -74,   -74,   232,   -74,
    -330,    57,   217,  -330,    10,   232,  -330,   -74,  -330,   232,
    -330,   297,  -330,   204,  -330,   232,  -330,   232,   -74,   232,
     232,  -330,   257,  -330,   232,    70,    76,   219,  -330,   232,
    -330,   913,  -330,   913,   259,   232,  -330,  -330,   232,  -330,
    -330,   -74,   232,   232,  -330,   232,   232,  -330,    99,   232,
     -74,   232,   -74,   -74,   232,   -74,  -330,   232,   232,  -330,
     232,  -330,   232,  -330,   232,  -330,   232,  -330,   232,   636,
    -330,   232,    46,    46,  -330,  -330,   232,   232,  -330,  -330,
    -330,    86,  -330,  -330,   232,   232,   -46,   913,   259,  -330,
     913,   913,  -330,  -330,   913,  -330,  -330,   913,   -74,   232,
     269,  -330,   232,   -37,  1553,  1586,   885,  -330,  -330,  -330,
    -330,  -330,  -330,  -330,  -330,  -330,  -330,  -330,  -330,  -330,
    -330,  -330,  -330,  -330,  -330,   -25,  -330,   232,   232,  -330,
    -330,  -330,    29,  -330,   913,   913,   913,   913,  -330,  -330,
     125,   970,   259,   259,  -330,   439,  1242,  1459,  1507,  -330,
     913,   211,  1553,   806,   636,  1553,  -330,   885,   885,   370,
    -330,  -330,  -330,  1530,  -330,  1354,  -330,  -330,  -330,  1553,
    -330,  -330,  -330,  1354,   232,   -39,  -330,   -41,  -330,   259,
     259,  -330,  -330,  -330,  1530,  -330,  -330,  -330,  -330,  -330,
     -30,  -330,    36,  -330,    36,    -9,  -330,    -5,  -330,  -330,
    -330,  -330,  -330,  -330,  -330,  -330,  1124,  -330,  -330,  -330,
     439,  1485,  -330,  -330,  -330,   913,  -330,  -330,   232,   259,
    -330,   259,   259,   259,   820,   913,  -330,  -330,   444,    33,
      34,    35,    62,    65,    66,   -29,  -330,   -91,  -330,  -330,
    -330,  -330,   -95,   -95,  -330,  -330,   232,  1553,   232,  -330,
    -330,   232,  -330,   232,  -330,   232,   259,  -330,  -330,   113,
    -330,  -330,  -330,  -330,  -330,  1124,   232,  -330,  -330,   232,
    1354,  -330,    22,  -330,  -330,   232,    22,  -330,   232,    16,
      19,   232,  -330,  -330,   232,   232,   232,   232,   232,   232,
    -330,   636,   232,  -330,   232,   259,  -330,  -330,  -330,  -330,
    -330,  -330,   232,   259,   232,  -330,  -330,  -330,  -330,  -330,
    -330,  -330,    69,    72,  -330,  -330,  -330,  -330,  -330,  -330,
    -330,  -330,  -330,  -330,  -330,  -330,  -330,  -330,    12,  1038,
    -330,  -330,  -330,  -330,   -41,   -41,  -330,  -330,  -330,  -330,
    -330,  -330,  -330,  -330,  -330,  -330,  -330,  -330,  -330,  -330,
     353,    83,  -330,  -330,  -330,  -330,  -330,  -330,  -330,  -330,
    -330,  -330,  -330,  -330
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -330,  -330,  -330,   318,    97,  -330,  -330,  -330,  -330,  -330,
    -330,  -330,  -330,  -330,  -330,  -330,  -330,  -330,  -330,   289,
     114,   218,   195,  -330,   -17,  -330,  -126,   372,    21,    -7,
    -289,   140,    -6,   571,  -218,  -330,   655,  -329,  -330,  -116,
    -251,  -330,  -216,  -330,  -330,  -330,    -1,  -330,  -327,  -330,
    -330
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -180
static const yytype_int16 yytable[] =
{
     122,   161,   133,   135,   436,   122,   122,   150,   156,   157,
     440,   123,   196,   512,   123,   141,   411,   178,   142,   123,
     474,   475,   243,   123,   143,   144,   145,   191,   193,   245,
     400,   246,   124,   125,   122,   124,   125,   136,   123,   131,
     124,   125,   173,   175,   124,   125,   264,   304,   289,   103,
     104,   105,   106,   189,   296,   297,   107,   108,   109,   124,
     125,   291,   110,   111,   112,   113,   114,   250,   251,   293,
     254,   122,   257,   222,   224,   226,   228,   334,   318,   472,
     122,   452,   285,   236,   473,   333,   378,   123,   397,  -179,
    -179,  -179,  -179,   438,    83,   424,  -179,  -179,  -179,   424,
     123,   488,  -179,  -179,  -179,  -179,  -179,   445,   124,   125,
     294,   295,   296,   297,   123,   122,   103,   104,   105,   106,
     513,   124,   125,   107,   108,   109,  -174,   448,   122,   122,
     494,   449,   122,   495,   122,   124,   125,   122,   122,   122,
     310,   466,   467,   468,   311,   312,   313,  -174,  -174,   293,
     314,   315,   489,   317,   151,   151,   151,   158,   126,   165,
     515,   323,   489,   177,   151,   312,   123,   516,   517,   137,
     469,   187,   329,   470,   471,   237,   294,   295,   296,   297,
     335,   401,   510,   511,   290,   115,   213,   124,   125,   131,
     122,   523,   122,   428,   259,   344,   229,   116,   425,   131,
     503,     0,   350,   484,   352,   123,   354,   355,  -179,   357,
       0,     0,   131,     0,   103,   104,   105,   106,   123,   325,
     123,   107,   108,   109,     0,  -179,   124,   125,   368,   139,
     140,   122,   122,   123,     0,   375,  -179,  -179,   131,   124,
     125,   124,   125,   179,   180,   417,   122,     0,     0,   122,
     122,     0,   385,   122,   124,   125,   122,     0,  -167,   418,
     123,     0,   209,   211,   395,   396,     0,   218,     0,   220,
    -158,     0,   294,   295,   296,   297,     0,   232,   233,  -167,
    -167,   124,   125,     0,   241,     0,     0,     0,   136,   247,
       0,  -158,  -158,   122,   122,   122,   122,   258,  -169,   196,
     413,     0,   161,   110,   111,   112,   113,   114,   431,   122,
     421,     0,  -169,   368,     0,     0,   431,   427,     0,  -169,
    -169,     0,     0,   128,   434,     0,   132,   134,   202,   203,
     319,   205,   434,   305,   307,   308,   160,   166,   168,   172,
     174,   176,     0,   417,   429,   181,   183,   231,     0,     0,
     188,   190,   192,     0,   -61,   197,   200,   418,     0,   204,
     451,   207,     0,     0,   454,   444,   216,     0,   219,     0,
     221,   223,   225,   227,   122,   230,     0,     0,     0,   234,
     235,   238,   239,   240,   122,   242,   110,   111,   112,   113,
     114,   249,     0,   252,   253,   255,   256,     0,     0,   261,
     184,   185,   186,     0,   388,   462,   294,   295,   296,   297,
     131,     0,     0,   431,     0,     0,     0,   123,   421,   451,
     137,   110,   111,   112,   113,   114,     0,   370,   371,   434,
       0,     0,   110,   111,   112,   113,   114,   298,   124,   125,
    -174,   408,     0,     0,   299,   123,     0,   151,   415,     0,
     368,     0,   422,     0,     0,   426,   287,   288,     0,     0,
       0,  -174,  -174,   518,   309,   519,   124,   125,   464,   435,
       0,     0,   431,     0,     0,     0,   316,   520,   521,   522,
     320,     0,   321,   322,     0,     0,     0,   324,   434,     0,
       0,   326,     0,   327,     0,   328,     0,   330,   331,     0,
       0,     0,   332,     0,     0,   336,     0,   338,     0,   115,
       0,   151,   341,   342,     0,     0,   343,     0,     0,     0,
     345,   346,     0,   347,   348,     0,   349,   351,     0,   353,
       0,     0,   356,     0,     0,   358,   359,     0,   360,     0,
     361,   214,   362,     0,   363,     0,   364,   477,     0,   369,
       0,     0,   131,   206,   372,   373,     0,     0,     0,   374,
       0,     0,   376,   377,     0,     0,   380,     0,     0,     0,
     446,     0,   447,     0,     0,     0,     0,   386,     0,     0,
     387,     0,     0,     0,     0,   152,   152,   152,   159,   164,
       0,   152,   152,   152,   159,   152,   110,   111,   112,   113,
     114,     0,   159,   152,     0,   398,   399,   195,     0,     0,
       0,     0,     0,   123,   208,   210,   212,   159,   215,   217,
     409,   410,     0,   152,   152,   152,   152,   159,   110,   111,
     112,   113,   114,   152,   124,   125,     0,     0,     0,   103,
     104,   105,   106,     0,     0,     0,   107,   108,   109,     0,
       0,     0,   437,     0,     0,     0,   119,   442,   443,     0,
       0,   129,   130,     0,   260,     0,   103,   104,   105,   106,
       0,     0,     0,   107,   108,   109,     0,     0,     0,   110,
     111,   112,   113,   114,   450,   124,   125,     0,     0,     0,
     194,     0,   123,     0,     0,     0,   456,   457,     0,   458,
     459,   460,   461,     0,     0,     0,   465,   110,   111,   112,
     113,   114,     0,   124,   125,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   476,     0,   478,   248,     0,   479,
       0,   480,     0,   481,   482,     0,     0,   483,     0,     0,
       0,     0,     0,   485,   486,     0,     0,   487,     0,     0,
     490,   115,     0,   491,   492,     0,   493,     0,     0,   496,
       0,     0,   497,   498,   499,   500,   501,   502,   365,     0,
     504,   292,   505,   506,     0,     0,     0,     0,     0,     0,
     507,   508,   509,     0,   300,   301,     0,     0,   302,   306,
     303,   103,   104,   105,   106,     0,     0,     0,   107,   108,
     109,     0,   115,     0,   110,   111,   112,   113,   114,   103,
     104,   105,   106,     0,   116,   423,   107,   108,   109,     0,
       0,   123,     0,   103,   104,   105,   106,     0,     0,   148,
     107,   108,   109,     0,     0,   159,   110,   111,   112,   113,
     114,     0,   124,   125,     0,     0,   339,     0,   340,   103,
     104,   105,   106,     0,     0,     0,   107,   108,   109,     0,
       0,     0,   110,   111,   112,   113,   114,     0,     0,     0,
       0,   182,   159,     0,   142,     0,     0,     0,   152,   159,
     143,   144,   145,   159,     0,     0,   159,     0,   103,   104,
     105,   106,   124,   125,   152,   107,   108,   109,     0,     0,
     159,     0,   379,     0,     0,   381,   382,     0,     0,   383,
       0,     0,   384,     0,     0,   152,   103,   104,   105,   106,
       0,     0,     0,   107,   108,   109,     0,   115,     0,   110,
     111,   112,   113,   114,     0,     0,     0,     0,     0,   116,
       0,     0,   152,     0,     0,     0,     0,     0,     0,   402,
     403,   404,   405,     0,     0,   152,   149,     0,     0,     0,
       0,     0,     0,     0,     0,   416,     0,   294,   295,   296,
     297,     0,     0,   103,   104,   105,   106,     0,   159,     0,
     107,   108,   109,     0,     0,   115,   110,   111,   112,   113,
     114,     0,     0,   294,   295,   296,   297,   116,   123,     0,
     103,   104,   105,   106,     0,     0,   148,   107,   108,   109,
       0,     0,     0,   110,   111,   112,   113,   114,     0,   124,
     125,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     455,     0,     0,     0,     0,     0,     0,     0,     0,   514,
     463,   103,   104,   105,   106,     0,     0,     0,   107,   108,
     109,     0,   115,     0,   110,   111,   112,   113,   114,     0,
       0,     0,     0,     0,   116,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   149,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   294,   295,   296,
     297,     0,     0,     0,     0,   123,     0,  -179,  -179,  -179,
    -179,     0,     0,   149,  -179,  -179,  -179,     1,   167,     0,
       0,     0,     0,     2,     0,   430,   124,   125,     0,     0,
       0,     0,     3,     0,     0,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
       0,     0,    19,    20,    21,    22,    23,   115,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,   131,     0,     0,
       0,     0,     0,  -158,     0,  -158,  -158,  -158,  -158,     0,
       0,  -158,  -158,  -158,  -158,     0,  -179,     1,  -158,  -158,
    -158,  -158,  -158,     2,     0,    83,     0,     0,     0,     0,
       0,     0,     0,    84,    85,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
       0,     0,    19,    20,    21,    22,    23,     0,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,   103,   104,   105,
     106,     0,     0,     0,   107,   108,   109,     0,     0,     0,
     110,   111,   112,   113,   114,     0,     0,     0,  -158,     0,
       0,     0,     0,     0,     0,     0,     0,   123,     0,  -160,
    -160,  -160,  -160,    84,    85,  -160,  -160,  -160,  -160,     0,
     199,     0,  -160,  -160,  -160,  -160,  -160,     0,   124,   125,
     123,     0,   103,   104,   105,   106,     0,     0,   148,   107,
     108,   109,     0,     0,     0,   110,   111,   112,   113,   114,
       0,   124,   125,   123,     0,   103,   104,   105,   106,     0,
       0,     0,   107,   108,   109,     0,     0,     0,   110,   111,
     112,   113,   114,     0,   124,   125,     0,     0,     0,     0,
     412,   430,   103,   104,   105,   106,     0,     0,   148,   107,
     108,   109,     0,     0,     0,   110,   111,   112,   113,   114,
       0,     0,     0,     0,     0,     0,   453,     0,   103,   104,
     105,   106,     0,   115,   148,   107,   108,   109,     0,  -160,
       0,   110,   111,   112,   113,   114,     0,     0,   414,     0,
     103,   104,   105,   106,     0,     0,     0,   107,   108,   109,
       0,     0,  -160,   110,   111,   112,   113,   114,     0,     0,
       0,     0,     0,   103,   104,   105,   106,     0,     0,   148,
     107,   108,   109,     0,     0,   149,   110,   111,   112,   113,
     114,     0,     0,     0,     0,     0,   103,   104,   105,   106,
       0,     0,     0,   107,   108,   109,     0,     0,   149,   110,
     111,   112,   113,   114,     0,     0,     0,     0,   103,   104,
     105,   106,     0,     0,     0,   107,   108,   109,     0,   103,
     104,   105,   106,     0,     0,   149,   107,   108,   109,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   149,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   149,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   149,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   265,     0,   266,   267,     0,     0,     0,   149,
     268,   269,   270,   271,   272,   273,   274,   275,   276,   277,
     278,   279,   280,   281,   282,   283,   284,     0,   389,   390,
     391,   392,   393,   394
};

static const yytype_int16 yycheck[] =
{
       1,    18,     8,     9,   333,     6,     7,    14,    15,    16,
     337,     1,   134,     1,     1,     1,   305,    24,     4,     1,
     111,   112,   134,     1,    10,    11,    12,    33,    34,   132,
       1,   132,    22,    23,    35,    22,    23,     1,     1,   113,
      22,    23,    21,    22,    22,    23,    21,     1,     0,     3,
       4,     5,     6,    32,   149,   150,    10,    11,    12,    22,
      23,    22,    16,    17,    18,    19,    20,    73,    74,   139,
      76,    72,    78,    52,    53,    54,    55,     1,    21,   108,
      81,   370,    83,    62,   113,    15,   132,     1,   113,     3,
       4,     5,     6,   132,   135,   313,    10,    11,    12,   317,
       1,   430,    16,    17,    18,    19,    20,   137,    22,    23,
     147,   148,   149,   150,     1,   116,     3,     4,     5,     6,
     108,    22,    23,    10,    11,    12,     1,   136,   129,   130,
     114,   136,   133,   114,   135,    22,    23,   138,   139,   140,
     146,   108,   108,   108,   150,   151,   152,    22,    23,   139,
     156,   157,   140,   159,    14,    15,    16,    17,   145,   141,
     489,   167,   140,    23,    24,   171,     1,   494,   495,   133,
     108,    31,   178,   108,   108,   138,   147,   148,   149,   150,
     186,   152,   113,   111,    87,   139,    46,    22,    23,   113,
     191,   108,   193,   319,    80,   201,    56,   151,   314,   113,
     451,    -1,   208,   419,   210,     1,   212,   213,   132,   215,
      -1,    -1,   113,    -1,     3,     4,     5,     6,     1,    15,
       1,    10,    11,    12,    -1,   139,    22,    23,   229,    11,
      12,   232,   233,     1,    -1,   241,   137,   151,   113,    22,
      23,    22,    23,    25,    26,   132,   247,    -1,    -1,   250,
     251,    -1,   258,   254,    22,    23,   257,    -1,     1,   146,
       1,    -1,    44,    45,   265,   266,    -1,    49,    -1,    51,
       1,    -1,   147,   148,   149,   150,    -1,    59,    60,    22,
      23,    22,    23,    -1,    66,    -1,    -1,    -1,     1,    71,
      -1,    22,    23,   294,   295,   296,   297,    79,     1,   134,
     307,    -1,   319,    16,    17,    18,    19,    20,   325,   310,
     311,    -1,    15,   314,    -1,    -1,   333,   318,    -1,    22,
      23,    -1,    -1,     5,   325,    -1,     8,     9,    39,    40,
     113,    42,   333,   138,   139,   140,    18,    19,    20,    21,
      22,    23,    -1,   132,   323,    27,    28,    58,    -1,    -1,
      32,    33,    34,    -1,   135,    37,    38,   146,    -1,    41,
     366,    43,    -1,    -1,   371,   344,    48,    -1,    50,    -1,
      52,    53,    54,    55,   375,    57,    -1,    -1,    -1,    61,
      62,    63,    64,    65,   385,    67,    16,    17,    18,    19,
      20,    73,    -1,    75,    76,    77,    78,    -1,    -1,    81,
      28,    29,    30,    -1,   264,   384,   147,   148,   149,   150,
     113,    -1,    -1,   430,    -1,    -1,    -1,     1,   419,   425,
     133,    16,    17,    18,    19,    20,    -1,   232,   233,   430,
      -1,    -1,    16,    17,    18,    19,    20,   119,    22,    23,
       1,   301,    -1,    -1,   126,     1,    -1,   307,   308,    -1,
     451,    -1,   312,    -1,    -1,   315,    84,    85,    -1,    -1,
      -1,    22,    23,   110,   146,   112,    22,    23,    24,   329,
      -1,    -1,   489,    -1,    -1,    -1,   158,   124,   125,   126,
     162,    -1,   164,   165,    -1,    -1,    -1,   169,   489,    -1,
      -1,   173,    -1,   175,    -1,   177,    -1,   179,   180,    -1,
      -1,    -1,   184,    -1,    -1,   187,    -1,   189,    -1,   139,
      -1,   371,   194,   195,    -1,    -1,   198,    -1,    -1,    -1,
     202,   203,    -1,   205,   206,    -1,   208,   209,    -1,   211,
      -1,    -1,   214,    -1,    -1,   217,   218,    -1,   220,    -1,
     222,   136,   224,    -1,   226,    -1,   228,   407,    -1,   231,
      -1,    -1,   113,   137,   236,   237,    -1,    -1,    -1,   241,
      -1,    -1,   244,   245,    -1,    -1,   248,    -1,    -1,    -1,
     352,    -1,   354,    -1,    -1,    -1,    -1,   259,    -1,    -1,
     262,    -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,
      -1,    20,    21,    22,    23,    24,    16,    17,    18,    19,
      20,    -1,    31,    32,    -1,   287,   288,    36,    -1,    -1,
      -1,    -1,    -1,     1,    43,    44,    45,    46,    47,    48,
     302,   303,    -1,    52,    53,    54,    55,    56,    16,    17,
      18,    19,    20,    62,    22,    23,    -1,    -1,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,
      -1,    -1,   334,    -1,    -1,    -1,     1,   339,   340,    -1,
      -1,     6,     7,    -1,     1,    -1,     3,     4,     5,     6,
      -1,    -1,    -1,    10,    11,    12,    -1,    -1,    -1,    16,
      17,    18,    19,    20,   366,    22,    23,    -1,    -1,    -1,
      35,    -1,     1,    -1,    -1,    -1,   378,   379,    -1,   381,
     382,   383,   384,    -1,    -1,    -1,   388,    16,    17,    18,
      19,    20,    -1,    22,    23,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   406,    -1,   408,    72,    -1,   411,
      -1,   413,    -1,   415,   416,    -1,    -1,   419,    -1,    -1,
      -1,    -1,    -1,   425,   426,    -1,    -1,   429,    -1,    -1,
     432,   139,    -1,   435,   436,    -1,   438,    -1,    -1,   441,
      -1,    -1,   444,   445,   446,   447,   448,   449,   132,    -1,
     452,   116,   454,   455,    -1,    -1,    -1,    -1,    -1,    -1,
     462,   463,   464,    -1,   129,   130,    -1,    -1,   133,     1,
     135,     3,     4,     5,     6,    -1,    -1,    -1,    10,    11,
      12,    -1,   139,    -1,    16,    17,    18,    19,    20,     3,
       4,     5,     6,    -1,   151,     9,    10,    11,    12,    -1,
      -1,     1,    -1,     3,     4,     5,     6,    -1,    -1,     9,
      10,    11,    12,    -1,    -1,   264,    16,    17,    18,    19,
      20,    -1,    22,    23,    -1,    -1,   191,    -1,   193,     3,
       4,     5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,
      -1,    -1,    16,    17,    18,    19,    20,    -1,    -1,    -1,
      -1,     1,   301,    -1,     4,    -1,    -1,    -1,   307,   308,
      10,    11,    12,   312,    -1,    -1,   315,    -1,     3,     4,
       5,     6,    22,    23,   323,    10,    11,    12,    -1,    -1,
     329,    -1,   247,    -1,    -1,   250,   251,    -1,    -1,   254,
      -1,    -1,   257,    -1,    -1,   344,     3,     4,     5,     6,
      -1,    -1,    -1,    10,    11,    12,    -1,   139,    -1,    16,
      17,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,   151,
      -1,    -1,   371,    -1,    -1,    -1,    -1,    -1,    -1,   294,
     295,   296,   297,    -1,    -1,   384,   136,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   310,    -1,   147,   148,   149,
     150,    -1,    -1,     3,     4,     5,     6,    -1,   407,    -1,
      10,    11,    12,    -1,    -1,   139,    16,    17,    18,    19,
      20,    -1,    -1,   147,   148,   149,   150,   151,     1,    -1,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      -1,    -1,    -1,    16,    17,    18,    19,    20,    -1,    22,
      23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     375,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,
     385,     3,     4,     5,     6,    -1,    -1,    -1,    10,    11,
      12,    -1,   139,    -1,    16,    17,    18,    19,    20,    -1,
      -1,    -1,    -1,    -1,   151,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   136,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   147,   148,   149,
     150,    -1,    -1,    -1,    -1,     1,    -1,     3,     4,     5,
       6,    -1,    -1,   136,    10,    11,    12,     7,   141,    -1,
      -1,    -1,    -1,    13,    -1,   107,    22,    23,    -1,    -1,
      -1,    -1,    22,    -1,    -1,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      -1,    -1,    42,    43,    44,    45,    46,   139,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   113,    -1,    -1,
      -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,    -1,
      -1,     9,    10,    11,    12,    -1,   132,     7,    16,    17,
      18,    19,    20,    13,    -1,   135,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   143,   144,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      -1,    -1,    42,    43,    44,    45,    46,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,     3,     4,     5,
       6,    -1,    -1,    -1,    10,    11,    12,    -1,    -1,    -1,
      16,    17,    18,    19,    20,    -1,    -1,    -1,   136,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,     3,
       4,     5,     6,   143,   144,     9,    10,    11,    12,    -1,
      14,    -1,    16,    17,    18,    19,    20,    -1,    22,    23,
       1,    -1,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    -1,    -1,    -1,    16,    17,    18,    19,    20,
      -1,    22,    23,     1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    10,    11,    12,    -1,    -1,    -1,    16,    17,
      18,    19,    20,    -1,    22,    23,    -1,    -1,    -1,    -1,
       1,   107,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    -1,    -1,    -1,    16,    17,    18,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,     3,     4,
       5,     6,    -1,   139,     9,    10,    11,    12,    -1,   113,
      -1,    16,    17,    18,    19,    20,    -1,    -1,     1,    -1,
       3,     4,     5,     6,    -1,    -1,    -1,    10,    11,    12,
      -1,    -1,   136,    16,    17,    18,    19,    20,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,     6,    -1,    -1,     9,
      10,    11,    12,    -1,    -1,   136,    16,    17,    18,    19,
      20,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    -1,    -1,    10,    11,    12,    -1,    -1,   136,    16,
      17,    18,    19,    20,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,     3,
       4,     5,     6,    -1,    -1,   136,    10,    11,    12,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   136,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   136,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   136,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,    -1,   109,   110,    -1,    -1,    -1,   136,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,    -1,   122,   123,
     124,   125,   126,   127
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
     104,   105,   106,   135,   143,   144,   154,   155,   157,   158,
     159,   160,   161,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   201,     3,     4,     5,     6,    10,    11,    12,
      16,    17,    18,    19,    20,   139,   151,   177,   186,   189,
     196,   198,   199,     1,    22,    23,   145,   156,   156,   189,
     189,   113,   156,   185,   156,   185,     1,   133,   174,   174,
     174,     1,     4,    10,    11,    12,   180,   197,     9,   136,
     182,   184,   186,   187,   188,   199,   182,   182,   184,   186,
     156,   177,   178,   179,   186,   141,   156,   141,   156,   181,
     182,   184,   156,   181,   156,   181,   156,   184,   182,   174,
     174,   156,     1,   156,   180,   180,   180,   184,   156,   181,
     156,   185,   156,   185,   189,   186,   134,   156,   172,    14,
     156,   176,   172,   172,   156,   172,   137,   156,   186,   174,
     186,   174,   186,   184,   136,   186,   156,   186,   174,   156,
     174,   156,   181,   156,   181,   156,   181,   156,   181,   184,
     156,   172,   174,   174,   156,   156,   181,   138,   156,   156,
     156,   174,   156,   134,   173,   132,   132,   174,   189,   156,
     185,   185,   156,   156,   185,   156,   156,   185,   174,   173,
       1,   156,   175,   189,    21,   107,   109,   110,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   199,   203,   180,   180,     0,
     157,    22,   189,   139,   147,   148,   149,   150,   156,   156,
     189,   189,   189,   189,     1,   175,     1,   175,   175,   156,
     185,   185,   185,   185,   185,   185,   156,   185,    21,   113,
     156,   156,   156,   185,   156,    15,   156,   156,   156,   185,
     156,   156,   156,    15,     1,   185,   156,   162,   156,   189,
     189,   156,   156,   156,   185,   156,   156,   156,   156,   156,
     185,   156,   185,   156,   185,   185,   156,   185,   156,   156,
     156,   156,   156,   156,   156,   132,   192,   193,   199,   156,
     175,   175,   156,   156,   156,   185,   156,   156,   132,   189,
     156,   189,   189,   189,   189,   185,   156,   156,   184,   122,
     123,   124,   125,   126,   127,   199,   199,   113,   156,   156,
       1,   152,   189,   189,   189,   189,   183,   185,   184,   156,
     156,   183,     1,   182,     1,   184,   189,   132,   146,   194,
     195,   199,   184,     9,   187,   192,   184,   199,   179,   181,
     107,   177,   190,   191,   199,   184,   190,   156,   132,   200,
     201,   202,   156,   156,   181,   137,   174,   174,   136,   136,
     156,   185,   183,     1,   182,   189,   156,   156,   156,   156,
     156,   156,   181,   189,    24,   156,   108,   108,   108,   108,
     108,   108,   108,   113,   111,   112,   156,   184,   156,   156,
     156,   156,   156,   156,   195,   156,   156,   156,   190,   140,
     156,   156,   156,   156,   114,   114,   156,   156,   156,   156,
     156,   156,   156,   193,   156,   156,   156,   156,   156,   156,
     113,   111,     1,   108,     1,   190,   201,   201,   110,   112,
     124,   125,   126,   108
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
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
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

/* Line 1455 of yacc.c  */
#line 185 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 186 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 187 "mon_parse.y"
    { new_cmd = 1; asm_mode = 0;  (yyval.i) = 0; }
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 196 "mon_parse.y"
    { return ERR_EXPECT_END_CMD; }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 211 "mon_parse.y"
    { return ERR_BAD_CMD; }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 215 "mon_parse.y"
    { mon_bank(e_default_space, NULL); }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 217 "mon_parse.y"
    { mon_bank((yyvsp[(2) - (3)].i), NULL); }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 219 "mon_parse.y"
    { mon_bank(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 221 "mon_parse.y"
    { mon_bank((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 223 "mon_parse.y"
    { mon_jump((yyvsp[(2) - (3)].a)); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 225 "mon_parse.y"
    { mon_display_io_regs(0); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 227 "mon_parse.y"
    { mon_display_io_regs((yyvsp[(2) - (3)].a)); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 229 "mon_parse.y"
    { monitor_cpu_type_set(""); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 231 "mon_parse.y"
    { monitor_cpu_type_set((yyvsp[(2) - (3)].str)); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 233 "mon_parse.y"
    { mon_cpuhistory(-1); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 235 "mon_parse.y"
    { mon_cpuhistory((yyvsp[(3) - (4)].i)); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 237 "mon_parse.y"
    { mon_instruction_return(); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 239 "mon_parse.y"
    { machine_write_snapshot((yyvsp[(2) - (3)].str),0,0,0); /* FIXME */ }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 241 "mon_parse.y"
    { machine_read_snapshot((yyvsp[(2) - (3)].str), 0); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 243 "mon_parse.y"
    { mon_instructions_step(-1); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 245 "mon_parse.y"
    { mon_instructions_step((yyvsp[(3) - (4)].i)); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 247 "mon_parse.y"
    { mon_instructions_next(-1); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 249 "mon_parse.y"
    { mon_instructions_next((yyvsp[(3) - (4)].i)); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 251 "mon_parse.y"
    { mon_stack_up(-1); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 253 "mon_parse.y"
    { mon_stack_up((yyvsp[(3) - (4)].i)); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 255 "mon_parse.y"
    { mon_stack_down(-1); }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 257 "mon_parse.y"
    { mon_stack_down((yyvsp[(3) - (4)].i)); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 259 "mon_parse.y"
    { mon_display_screen(); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 264 "mon_parse.y"
    { (monitor_cpu_for_memspace[default_memspace]->mon_register_print)(default_memspace); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 266 "mon_parse.y"
    { (monitor_cpu_for_memspace[(yyvsp[(2) - (3)].i)]->mon_register_print)((yyvsp[(2) - (3)].i)); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 271 "mon_parse.y"
    {
                        /* What about the memspace? */
                        mon_playback_init((yyvsp[(4) - (5)].str));
                    }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 276 "mon_parse.y"
    {
                        /* What about the memspace? */
                        mon_playback_init((yyvsp[(2) - (3)].str));
                    }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 281 "mon_parse.y"
    { mon_save_symbols((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 283 "mon_parse.y"
    { mon_save_symbols(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 285 "mon_parse.y"
    { mon_add_name_to_symbol_table((yyvsp[(2) - (5)].a), (yyvsp[(4) - (5)].str)); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 287 "mon_parse.y"
    { mon_remove_name_from_symbol_table(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 289 "mon_parse.y"
    { mon_remove_name_from_symbol_table((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 291 "mon_parse.y"
    { mon_print_symbol_table((yyvsp[(2) - (3)].i)); }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 293 "mon_parse.y"
    { mon_print_symbol_table(e_default_space); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 295 "mon_parse.y"
    {
                        mon_add_name_to_symbol_table((yyvsp[(3) - (4)].a), mon_prepend_dot_to_name((yyvsp[(1) - (4)].str)));
                    }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 299 "mon_parse.y"
    {
                        mon_add_name_to_symbol_table((yyvsp[(3) - (5)].a), mon_prepend_dot_to_name((yyvsp[(1) - (5)].str)));
                    }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 305 "mon_parse.y"
    { mon_start_assemble_mode((yyvsp[(2) - (2)].a), NULL); }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 306 "mon_parse.y"
    { }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 308 "mon_parse.y"
    { mon_start_assemble_mode((yyvsp[(2) - (3)].a), NULL); }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 310 "mon_parse.y"
    { mon_disassemble_lines((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1]); }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 312 "mon_parse.y"
    { mon_disassemble_lines(BAD_ADDR, BAD_ADDR); }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 316 "mon_parse.y"
    { mon_memory_move((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], (yyvsp[(4) - (5)].a)); }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 318 "mon_parse.y"
    { mon_memory_compare((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[0], (yyvsp[(4) - (5)].a)); }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 320 "mon_parse.y"
    { mon_memory_fill((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1],(unsigned char *)(yyvsp[(4) - (5)].str)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 322 "mon_parse.y"
    { mon_memory_hunt((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1],(unsigned char *)(yyvsp[(4) - (5)].str)); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 324 "mon_parse.y"
    { mon_memory_display((yyvsp[(2) - (5)].rt), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], DF_PETSCII); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 326 "mon_parse.y"
    { mon_memory_display(default_radix, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_PETSCII); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 328 "mon_parse.y"
    { mon_memory_display(default_radix, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 330 "mon_parse.y"
    { mon_memory_display_data((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], 8, 8); }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 332 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 8, 8); }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 334 "mon_parse.y"
    { mon_memory_display_data((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], 24, 21); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 336 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 24, 21); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 338 "mon_parse.y"
    { mon_memory_display(0, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_PETSCII); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 340 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 342 "mon_parse.y"
    { mon_memory_display(0, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_SCREEN_CODE); }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 344 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_SCREEN_CODE); }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 346 "mon_parse.y"
    { mon_memmap_zap(); }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 348 "mon_parse.y"
    { mon_memmap_show(-1,BAD_ADDR,BAD_ADDR); }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 350 "mon_parse.y"
    { mon_memmap_show((yyvsp[(3) - (4)].i),BAD_ADDR,BAD_ADDR); }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 352 "mon_parse.y"
    { mon_memmap_show((yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].range)[0],(yyvsp[(4) - (5)].range)[1]); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 354 "mon_parse.y"
    { mon_memmap_save((yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].i)); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 358 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], FALSE, FALSE,
                                                    FALSE, FALSE);
                  }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 363 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], FALSE, FALSE,
                                                    FALSE, TRUE);
                  }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 368 "mon_parse.y"
    {
                      temp = mon_breakpoint_add_checkpoint((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], FALSE,
                                                           FALSE, FALSE, FALSE);
                      mon_breakpoint_set_checkpoint_condition(temp, (yyvsp[(4) - (5)].cond_node));
                  }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 374 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], FALSE,
                      ((yyvsp[(2) - (5)].i) == e_load || (yyvsp[(2) - (5)].i) == e_load_store),
                      ((yyvsp[(2) - (5)].i) == e_store || (yyvsp[(2) - (5)].i) == e_load_store), FALSE);
                  }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 380 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], TRUE, FALSE, FALSE,
                                                    FALSE);
                  }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 385 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 387 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 389 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 391 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 396 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_ON, (yyvsp[(2) - (3)].i)); }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 398 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_OFF, (yyvsp[(2) - (3)].i)); }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 400 "mon_parse.y"
    { mon_breakpoint_set_ignore_count((yyvsp[(2) - (3)].i), -1); }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 402 "mon_parse.y"
    { mon_breakpoint_set_ignore_count((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].i)); }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 404 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint((yyvsp[(2) - (3)].i)); }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 406 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint(-1); }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 408 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_condition((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].cond_node)); }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 410 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_command((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 412 "mon_parse.y"
    { return ERR_EXPECT_STRING; }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 416 "mon_parse.y"
    { sidefx = (((yyvsp[(2) - (3)].action) == e_TOGGLE) ? (sidefx ^ 1) : (yyvsp[(2) - (3)].action)); }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 418 "mon_parse.y"
    {
                         mon_out("I/O side effects are %s\n",
                                   sidefx ? "enabled" : "disabled");
                     }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 423 "mon_parse.y"
    { default_radix = (yyvsp[(2) - (3)].rt); }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 425 "mon_parse.y"
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

  case 108:

/* Line 1455 of yacc.c  */
#line 443 "mon_parse.y"
    { monitor_change_device((yyvsp[(2) - (3)].i)); }
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 445 "mon_parse.y"
    { mon_quit(); YYACCEPT; }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 447 "mon_parse.y"
    { exit_mon = 1; YYACCEPT; }
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 451 "mon_parse.y"
    { mon_drive_execute_disk_cmd((yyvsp[(2) - (3)].str)); }
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 453 "mon_parse.y"
    { mon_out("\t%d\n",(yyvsp[(2) - (3)].i)); }
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 455 "mon_parse.y"
    { mon_command_print_help(NULL); }
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 457 "mon_parse.y"
    { mon_command_print_help((yyvsp[(2) - (3)].str)); }
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 459 "mon_parse.y"
    { printf("SYSTEM COMMAND: %s\n",(yyvsp[(2) - (3)].str)); }
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 461 "mon_parse.y"
    { mon_print_convert((yyvsp[(2) - (3)].i)); }
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 463 "mon_parse.y"
    { mon_change_dir((yyvsp[(2) - (3)].str)); }
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 465 "mon_parse.y"
    { mon_keyboard_feed((yyvsp[(2) - (3)].str)); }
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 467 "mon_parse.y"
    { mon_backtrace(); }
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 469 "mon_parse.y"
    { mon_show_dir((yyvsp[(2) - (3)].str)); }
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 471 "mon_parse.y"
    { mon_show_pwd(); }
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 473 "mon_parse.y"
    { mon_screenshot_save((yyvsp[(2) - (3)].str),-1); }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 475 "mon_parse.y"
    { mon_screenshot_save((yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].i)); }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 477 "mon_parse.y"
    { mon_resource_get((yyvsp[(2) - (3)].str)); }
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 479 "mon_parse.y"
    { mon_resource_set((yyvsp[(2) - (4)].str),(yyvsp[(3) - (4)].str)); }
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 481 "mon_parse.y"
    { mon_reset_machine(-1); }
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 483 "mon_parse.y"
    { mon_reset_machine((yyvsp[(3) - (4)].i)); }
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 485 "mon_parse.y"
    { mon_tape_ctrl((yyvsp[(3) - (4)].i)); }
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 487 "mon_parse.y"
    { mon_cart_freeze(); }
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 489 "mon_parse.y"
    { }
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 493 "mon_parse.y"
    { mon_file_load((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].a), FALSE); }
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 495 "mon_parse.y"
    { mon_file_load((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].a), TRUE); }
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 497 "mon_parse.y"
    { mon_file_save((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], FALSE); }
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 499 "mon_parse.y"
    { return ERR_EXPECT_DEVICE_NUM; }
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 501 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 503 "mon_parse.y"
    { mon_file_save((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], TRUE); }
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 505 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 507 "mon_parse.y"
    { mon_file_verify((yyvsp[(2) - (5)].str),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 509 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 511 "mon_parse.y"
    { mon_drive_block_cmd(0,(yyvsp[(2) - (5)].i),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 513 "mon_parse.y"
    { mon_drive_block_cmd(1,(yyvsp[(2) - (5)].i),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 515 "mon_parse.y"
    { mon_drive_list(-1); }
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 517 "mon_parse.y"
    { mon_drive_list((yyvsp[(2) - (3)].i)); }
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 519 "mon_parse.y"
    { mon_attach((yyvsp[(2) - (4)].str),(yyvsp[(3) - (4)].i)); }
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 521 "mon_parse.y"
    { mon_detach((yyvsp[(2) - (3)].i)); }
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 525 "mon_parse.y"
    { mon_record_commands((yyvsp[(2) - (3)].str)); }
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 527 "mon_parse.y"
    { mon_end_recording(); }
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 529 "mon_parse.y"
    { mon_playback_init((yyvsp[(2) - (3)].str)); }
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 533 "mon_parse.y"
    { mon_memory_fill((yyvsp[(2) - (4)].a), BAD_ADDR, (unsigned char *)(yyvsp[(3) - (4)].str)); }
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 535 "mon_parse.y"
    { printf("Not yet.\n"); }
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 539 "mon_parse.y"
    { yydebug = 1; }
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 542 "mon_parse.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 545 "mon_parse.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 546 "mon_parse.y"
    { (yyval.str) = NULL; }
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 550 "mon_parse.y"
    { return ERR_EXPECT_FILENAME; }
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 554 "mon_parse.y"
    { return ERR_EXPECT_DEVICE_NUM; }
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 557 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 558 "mon_parse.y"
    { (yyval.i) = e_load_store; }
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 561 "mon_parse.y"
    { (yyval.i) = new_reg(default_memspace, (yyvsp[(1) - (1)].reg)); }
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 562 "mon_parse.y"
    { (yyval.i) = new_reg((yyvsp[(1) - (2)].i), (yyvsp[(2) - (2)].reg)); }
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 570 "mon_parse.y"
    { (monitor_cpu_for_memspace[reg_memspace((yyvsp[(1) - (3)].i))]->mon_register_set_val)(reg_memspace((yyvsp[(1) - (3)].i)), reg_regid((yyvsp[(1) - (3)].i)), (WORD) (yyvsp[(3) - (3)].i)); }
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 573 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 574 "mon_parse.y"
    { return ERR_EXPECT_BRKNUM; }
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 578 "mon_parse.y"
    { (yyval.range)[0] = (yyvsp[(1) - (1)].a); (yyval.range)[1] = BAD_ADDR; }
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 581 "mon_parse.y"
    { (yyval.range)[0] = (yyvsp[(1) - (3)].a); (yyval.range)[1] = (yyvsp[(3) - (3)].a); }
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 583 "mon_parse.y"
    { if (resolve_range(e_default_space, (yyval.range), (yyvsp[(1) - (1)].str))) return ERR_ADDR_TOO_BIG; }
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 585 "mon_parse.y"
    { if (resolve_range((yyvsp[(1) - (3)].i), (yyval.range), (yyvsp[(3) - (3)].str))) return ERR_ADDR_TOO_BIG; }
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 588 "mon_parse.y"
    { (yyval.a) = (yyvsp[(2) - (2)].a); }
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 589 "mon_parse.y"
    { (yyval.a) = BAD_ADDR; }
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 593 "mon_parse.y"
    {
             (yyval.a) = new_addr(e_default_space,(yyvsp[(1) - (1)].i));
             if (opt_asm) new_cmd = asm_mode = 1; 
         }
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 598 "mon_parse.y"
    {
             (yyval.a) = new_addr((yyvsp[(1) - (3)].i), (yyvsp[(3) - (3)].i));
             if (opt_asm) new_cmd = asm_mode = 1; 
         }
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 603 "mon_parse.y"
    {
             temp = mon_symbol_table_lookup_addr(e_default_space, (yyvsp[(1) - (1)].str));
             if (temp >= 0)
                 (yyval.a) = new_addr(e_default_space, temp);
             else
                 return ERR_UNDEFINED_LABEL;
         }
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 616 "mon_parse.y"
    { (yyval.i) = e_comp_space; }
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 617 "mon_parse.y"
    { (yyval.i) = e_disk8_space; }
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 618 "mon_parse.y"
    { (yyval.i) = e_disk9_space; }
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 619 "mon_parse.y"
    { (yyval.i) = e_disk10_space; }
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 620 "mon_parse.y"
    { (yyval.i) = e_disk11_space; }
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 623 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); if (!CHECK_ADDR((yyvsp[(1) - (1)].i))) return ERR_ADDR_TOO_BIG; }
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 625 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 627 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) + (yyvsp[(3) - (3)].i); }
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 628 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) - (yyvsp[(3) - (3)].i); }
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 629 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) * (yyvsp[(3) - (3)].i); }
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 630 "mon_parse.y"
    { (yyval.i) = ((yyvsp[(3) - (3)].i)) ? ((yyvsp[(1) - (3)].i) / (yyvsp[(3) - (3)].i)) : 1; }
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 631 "mon_parse.y"
    { (yyval.i) = (yyvsp[(2) - (3)].i); }
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 632 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 633 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 637 "mon_parse.y"
    {
               (yyval.cond_node) = new_cond; (yyval.cond_node)->is_parenthized = FALSE;
               (yyval.cond_node)->child1 = (yyvsp[(1) - (3)].cond_node); (yyval.cond_node)->child2 = (yyvsp[(3) - (3)].cond_node); (yyval.cond_node)->operation = (yyvsp[(2) - (3)].cond_op);
           }
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 642 "mon_parse.y"
    { return ERR_INCOMPLETE_COMPARE_OP; }
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 644 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(2) - (3)].cond_node); (yyval.cond_node)->is_parenthized = TRUE; }
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 646 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 648 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(1) - (1)].cond_node); }
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 651 "mon_parse.y"
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->reg_num = (yyvsp[(1) - (1)].i); (yyval.cond_node)->is_reg = TRUE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 657 "mon_parse.y"
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->value = (yyvsp[(1) - (1)].i); (yyval.cond_node)->is_reg = FALSE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 669 "mon_parse.y"
    { mon_add_number_to_buffer((yyvsp[(1) - (1)].i)); }
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 670 "mon_parse.y"
    { mon_add_string_to_buffer((yyvsp[(1) - (1)].str)); }
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 677 "mon_parse.y"
    { mon_add_number_to_buffer((yyvsp[(1) - (1)].i)); }
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 678 "mon_parse.y"
    { mon_add_number_masked_to_buffer((yyvsp[(1) - (1)].i), 0x00); }
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 679 "mon_parse.y"
    { mon_add_string_to_buffer((yyvsp[(1) - (1)].str)); }
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 682 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 683 "mon_parse.y"
    { (yyval.i) = (monitor_cpu_for_memspace[reg_memspace((yyvsp[(1) - (1)].i))]->mon_register_get_val)(reg_memspace((yyvsp[(1) - (1)].i)), reg_regid((yyvsp[(1) - (1)].i))); }
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 686 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 687 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 688 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 689 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 692 "mon_parse.y"
    { (yyval.i) = resolve_datatype(B_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 693 "mon_parse.y"
    { (yyval.i) = resolve_datatype(O_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 694 "mon_parse.y"
    { (yyval.i) = resolve_datatype(D_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 697 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 698 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 699 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 700 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 701 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 709 "mon_parse.y"
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

  case 229:

/* Line 1455 of yacc.c  */
#line 720 "mon_parse.y"
    { asm_mode = 0; }
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 723 "mon_parse.y"
    { if ((yyvsp[(2) - (2)].i) > 0xff)
                          (yyval.i) = join_ints(ASM_ADDR_MODE_IMMEDIATE_16,(yyvsp[(2) - (2)].i));
                        else
                          (yyval.i) = join_ints(ASM_ADDR_MODE_IMMEDIATE,(yyvsp[(2) - (2)].i)); }
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 727 "mon_parse.y"
    { if ((yyvsp[(1) - (1)].i) < 0x100)
               (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE,(yyvsp[(1) - (1)].i));
             else
               (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE,(yyvsp[(1) - (1)].i));
           }
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 732 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100)
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE_X,(yyvsp[(1) - (3)].i));
                          else
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_X,(yyvsp[(1) - (3)].i));
                        }
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 737 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100)
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE_Y,(yyvsp[(1) - (3)].i));
                          else
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_Y,(yyvsp[(1) - (3)].i));
                        }
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 743 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABS_INDIRECT,(yyvsp[(2) - (3)].i)); }
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 745 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_INDIRECT_X,(yyvsp[(2) - (5)].i)); }
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 747 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_INDIRECT_Y,(yyvsp[(2) - (5)].i)); }
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 748 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_BC,0); }
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 749 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_DE,0); }
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 750 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_HL,0); }
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 751 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_IX,0); }
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 752 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_IY,0); }
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 753 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_SP,0); }
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 755 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_A,(yyvsp[(2) - (5)].i)); }
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 757 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_HL,(yyvsp[(2) - (5)].i)); }
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 759 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_IX,(yyvsp[(2) - (5)].i)); }
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 761 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_IY,(yyvsp[(2) - (5)].i)); }
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 762 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_IMPLIED,0); }
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 763 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ACCUMULATOR,0); }
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 764 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_B,0); }
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 765 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_C,0); }
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 766 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_D,0); }
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 767 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_E,0); }
    break;

  case 253:

/* Line 1455 of yacc.c  */
#line 768 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_H,0); }
    break;

  case 254:

/* Line 1455 of yacc.c  */
#line 769 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IXH,0); }
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 770 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IYH,0); }
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 771 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_L,0); }
    break;

  case 257:

/* Line 1455 of yacc.c  */
#line 772 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IXL,0); }
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 773 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IYL,0); }
    break;

  case 259:

/* Line 1455 of yacc.c  */
#line 774 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_AF,0); }
    break;

  case 260:

/* Line 1455 of yacc.c  */
#line 775 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_BC,0); }
    break;

  case 261:

/* Line 1455 of yacc.c  */
#line 776 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_DE,0); }
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 777 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_HL,0); }
    break;

  case 263:

/* Line 1455 of yacc.c  */
#line 778 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IX,0); }
    break;

  case 264:

/* Line 1455 of yacc.c  */
#line 779 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IY,0); }
    break;

  case 265:

/* Line 1455 of yacc.c  */
#line 780 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_SP,0); }
    break;



/* Line 1455 of yacc.c  */
#line 4213 "mon_parse.c"
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
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
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



/* Line 1675 of yacc.c  */
#line 784 "mon_parse.y"


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


