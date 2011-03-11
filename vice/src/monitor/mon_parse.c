
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
#define ERR_EXPECT_CHECKNUM 5
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
     CMD_EXPORT = 361,
     CMD_LABEL_ASGN = 362,
     L_PAREN = 363,
     R_PAREN = 364,
     ARG_IMMEDIATE = 365,
     REG_A = 366,
     REG_X = 367,
     REG_Y = 368,
     COMMA = 369,
     INST_SEP = 370,
     REG_B = 371,
     REG_C = 372,
     REG_D = 373,
     REG_E = 374,
     REG_H = 375,
     REG_L = 376,
     REG_AF = 377,
     REG_BC = 378,
     REG_DE = 379,
     REG_HL = 380,
     REG_IX = 381,
     REG_IY = 382,
     REG_SP = 383,
     REG_IXH = 384,
     REG_IXL = 385,
     REG_IYH = 386,
     REG_IYL = 387,
     STRING = 388,
     FILENAME = 389,
     R_O_L = 390,
     OPCODE = 391,
     LABEL = 392,
     BANKNAME = 393,
     CPUTYPE = 394,
     MON_REGISTER = 395,
     COMPARE_OP = 396,
     RADIX_TYPE = 397,
     INPUT_SPEC = 398,
     CMD_CHECKPT_ON = 399,
     CMD_CHECKPT_OFF = 400,
     TOGGLE = 401,
     MASK = 402
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
#define CMD_EXPORT 361
#define CMD_LABEL_ASGN 362
#define L_PAREN 363
#define R_PAREN 364
#define ARG_IMMEDIATE 365
#define REG_A 366
#define REG_X 367
#define REG_Y 368
#define COMMA 369
#define INST_SEP 370
#define REG_B 371
#define REG_C 372
#define REG_D 373
#define REG_E 374
#define REG_H 375
#define REG_L 376
#define REG_AF 377
#define REG_BC 378
#define REG_DE 379
#define REG_HL 380
#define REG_IX 381
#define REG_IY 382
#define REG_SP 383
#define REG_IXH 384
#define REG_IXL 385
#define REG_IYH 386
#define REG_IYL 387
#define STRING 388
#define FILENAME 389
#define R_O_L 390
#define OPCODE 391
#define LABEL 392
#define BANKNAME 393
#define CPUTYPE 394
#define MON_REGISTER 395
#define COMPARE_OP 396
#define RADIX_TYPE 397
#define INPUT_SPEC 398
#define CMD_CHECKPT_ON 399
#define CMD_CHECKPT_OFF 400
#define TOGGLE 401
#define MASK 402




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
#line 544 "mon_parse.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 556 "mon_parse.c"

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
#define YYFINAL  291
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1656

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  154
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  51
/* YYNRULES -- Number of rules.  */
#define YYNRULES  266
/* YYNRULES -- Number of states.  */
#define YYNSTATES  525

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   402

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     152,   153,   150,   148,     2,   149,     2,   151,     2,     2,
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
     145,   146,   147
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
     328,   332,   335,   338,   341,   344,   348,   352,   356,   362,
     366,   369,   375,   381,   386,   390,   393,   397,   400,   404,
     407,   410,   413,   417,   421,   424,   428,   432,   436,   440,
     444,   447,   451,   454,   458,   464,   468,   473,   476,   481,
     486,   489,   493,   499,   505,   511,   515,   520,   526,   531,
     537,   542,   548,   554,   557,   561,   566,   570,   574,   577,
     581,   586,   589,   592,   594,   596,   597,   599,   601,   603,
     605,   607,   608,   610,   613,   617,   619,   623,   625,   627,
     629,   631,   635,   637,   641,   644,   645,   647,   651,   653,
     655,   656,   658,   660,   662,   664,   666,   668,   670,   674,
     678,   682,   686,   690,   694,   696,   700,   704,   708,   712,
     714,   716,   718,   722,   724,   726,   728,   731,   733,   735,
     737,   739,   741,   743,   745,   747,   749,   751,   753,   755,
     757,   759,   761,   763,   765,   767,   771,   775,   778,   781,
     783,   785,   788,   790,   794,   798,   802,   808,   814,   818,
     822,   826,   830,   834,   838,   844,   850,   856,   862,   863,
     865,   867,   869,   871,   873,   875,   877,   879,   881,   883,
     885,   887,   889,   891,   893,   895,   897
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     155,     0,    -1,   156,    -1,   202,    22,    -1,    22,    -1,
     158,    -1,   156,   158,    -1,    23,    -1,    22,    -1,     1,
      -1,   159,    -1,   161,    -1,   164,    -1,   162,    -1,   165,
      -1,   166,    -1,   167,    -1,   168,    -1,   169,    -1,   170,
      -1,   171,    -1,   172,    -1,    13,    -1,    67,   157,    -1,
      67,   187,   157,    -1,    67,   138,   157,    -1,    67,   187,
     186,   138,   157,    -1,    38,   185,   157,    -1,    46,   157,
      -1,    46,   185,   157,    -1,    87,   157,    -1,    87,   139,
     157,    -1,   100,   157,    -1,   100,   186,   190,   157,    -1,
      26,   157,    -1,    49,   175,   157,    -1,    50,   175,   157,
      -1,    58,   157,    -1,    58,   186,   190,   157,    -1,    57,
     157,    -1,    57,   186,   190,   157,    -1,    29,   157,    -1,
      29,   186,   190,   157,    -1,    30,   157,    -1,    30,   186,
     190,   157,    -1,    85,   157,    -1,   160,    -1,    39,   157,
      -1,    39,   187,   157,    -1,    39,   179,   157,    -1,    68,
     187,   186,   175,   157,    -1,    68,   175,   157,    -1,    69,
     187,   186,   175,   157,    -1,    69,   175,   157,    -1,    70,
     185,   186,   137,   157,    -1,    71,   137,   157,    -1,    71,
     187,   186,   137,   157,    -1,    72,   187,   157,    -1,    72,
     157,    -1,   107,    21,   185,   157,    -1,   107,    21,   185,
      24,   157,    -1,    -1,    55,   185,   163,   203,   157,    -1,
      55,   185,   157,    -1,    56,   182,   157,    -1,    56,   157,
      -1,    37,   183,   186,   185,   157,    -1,    48,   183,   186,
     185,   157,    -1,    36,   183,   186,   193,   157,    -1,    35,
     183,   186,   195,   157,    -1,    43,   142,   186,   182,   157,
      -1,    43,   182,   157,    -1,    43,   157,    -1,    76,   182,
     157,    -1,    76,   157,    -1,    77,   182,   157,    -1,    77,
     157,    -1,    78,   182,   157,    -1,    78,   157,    -1,    79,
     182,   157,    -1,    79,   157,    -1,   101,   157,    -1,   102,
     157,    -1,   102,   186,   190,   157,    -1,   102,   186,   190,
     182,   157,    -1,   103,   175,   186,   190,   157,    -1,    44,
     182,   157,    -1,    86,   182,   157,    -1,    44,   182,    15,
     191,   157,    -1,    62,   177,   182,   157,    -1,    45,   182,
     157,    -1,    44,   157,    -1,    86,   157,    -1,    45,   157,
      -1,    62,   157,    -1,   144,   181,   157,    -1,   145,   181,
     157,    -1,    34,   181,   157,    -1,    34,   181,   186,   190,
     157,    -1,    52,   181,   157,    -1,    52,   157,    -1,    53,
     181,    15,   191,   157,    -1,    54,   181,   186,   133,   157,
      -1,    54,   181,     1,   157,    -1,    25,   146,   157,    -1,
      25,   157,    -1,    42,   142,   157,    -1,    42,   157,    -1,
      60,   187,   157,    -1,   106,   157,    -1,    65,   157,    -1,
      51,   157,    -1,    63,   173,   157,    -1,    59,   190,   157,
      -1,    61,   157,    -1,    61,   173,   157,    -1,    64,   173,
     157,    -1,     7,   190,   157,    -1,    66,   173,   157,    -1,
      82,   173,   157,    -1,    89,   157,    -1,    92,   174,   157,
      -1,    91,   157,    -1,    90,   175,   157,    -1,    90,   175,
     186,   190,   157,    -1,    93,   133,   157,    -1,    94,   133,
     133,   157,    -1,    97,   157,    -1,    97,   186,   190,   157,
      -1,    98,   186,   190,   157,    -1,    99,   157,    -1,   104,
     174,   157,    -1,    31,   175,   176,   184,   157,    -1,    83,
     175,   176,   184,   157,    -1,    32,   175,   176,   183,   157,
      -1,    32,   175,     1,    -1,    32,   175,   176,     1,    -1,
      84,   175,   176,   183,   157,    -1,    84,   175,   176,     1,
      -1,    33,   175,   176,   185,   157,    -1,    33,   175,   176,
       1,    -1,    27,   190,   190,   184,   157,    -1,    28,   190,
     190,   185,   157,    -1,   105,   157,    -1,   105,   176,   157,
      -1,    95,   175,   190,   157,    -1,    96,   190,   157,    -1,
      73,   175,   157,    -1,    74,   157,    -1,    75,   175,   157,
      -1,    80,   185,   193,   157,    -1,    81,   157,    -1,    88,
     157,    -1,   135,    -1,   135,    -1,    -1,   134,    -1,     1,
      -1,   190,    -1,     1,    -1,    14,    -1,    -1,   140,    -1,
     187,   140,    -1,   179,   114,   180,    -1,   180,    -1,   178,
      21,   200,    -1,   198,    -1,     1,    -1,   183,    -1,   185,
      -1,   185,   186,   185,    -1,     9,    -1,   187,   186,     9,
      -1,   186,   185,    -1,    -1,   188,    -1,   187,   186,   188,
      -1,   137,    -1,   114,    -1,    -1,    16,    -1,    17,    -1,
      18,    -1,    19,    -1,    20,    -1,   189,    -1,   200,    -1,
     190,   148,   190,    -1,   190,   149,   190,    -1,   190,   150,
     190,    -1,   190,   151,   190,    -1,   152,   190,   153,    -1,
     152,   190,     1,    -1,   197,    -1,   191,   141,   191,    -1,
     191,   141,     1,    -1,   108,   191,   109,    -1,   108,   191,
       1,    -1,   192,    -1,   178,    -1,   200,    -1,   193,   186,
     194,    -1,   194,    -1,   200,    -1,   133,    -1,   195,   196,
      -1,   196,    -1,   200,    -1,   147,    -1,   133,    -1,   200,
      -1,   178,    -1,     4,    -1,    12,    -1,    11,    -1,    10,
      -1,    12,    -1,    11,    -1,    10,    -1,     3,    -1,     4,
      -1,     5,    -1,     6,    -1,   199,    -1,   201,   115,   202,
      -1,   202,   115,   202,    -1,   202,   115,    -1,   136,   204,
      -1,   202,    -1,   201,    -1,   110,   200,    -1,   200,    -1,
     200,   114,   112,    -1,   200,   114,   113,    -1,   108,   200,
     109,    -1,   108,   200,   114,   112,   109,    -1,   108,   200,
     109,   114,   113,    -1,   108,   123,   109,    -1,   108,   124,
     109,    -1,   108,   125,   109,    -1,   108,   126,   109,    -1,
     108,   127,   109,    -1,   108,   128,   109,    -1,   108,   200,
     109,   114,   111,    -1,   108,   200,   109,   114,   125,    -1,
     108,   200,   109,   114,   126,    -1,   108,   200,   109,   114,
     127,    -1,    -1,   111,    -1,   116,    -1,   117,    -1,   118,
      -1,   119,    -1,   120,    -1,   129,    -1,   131,    -1,   121,
      -1,   130,    -1,   132,    -1,   122,    -1,   123,    -1,   124,
      -1,   125,    -1,   126,    -1,   127,    -1,   128,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   186,   186,   187,   188,   191,   192,   195,   196,   197,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   215,   217,   219,   221,   223,   225,   227,
     229,   231,   233,   235,   237,   239,   241,   243,   245,   247,
     249,   251,   253,   255,   257,   259,   261,   264,   266,   268,
     271,   276,   281,   283,   285,   287,   289,   291,   293,   295,
     299,   306,   305,   308,   310,   312,   316,   318,   320,   322,
     324,   326,   328,   330,   332,   334,   336,   338,   340,   342,
     344,   346,   348,   350,   352,   354,   358,   362,   366,   371,
     375,   380,   382,   384,   386,   391,   393,   395,   397,   399,
     401,   403,   405,   407,   411,   413,   418,   420,   438,   440,
     442,   444,   448,   450,   452,   454,   456,   458,   460,   462,
     464,   466,   468,   470,   472,   474,   476,   478,   480,   482,
     484,   486,   490,   492,   494,   496,   498,   500,   502,   504,
     506,   508,   510,   512,   514,   516,   518,   522,   524,   526,
     530,   532,   536,   540,   543,   544,   547,   548,   551,   552,
     555,   556,   559,   560,   563,   564,   567,   571,   572,   575,
     576,   579,   580,   582,   586,   587,   590,   595,   600,   610,
     611,   614,   615,   616,   617,   618,   621,   623,   625,   626,
     627,   628,   629,   630,   631,   634,   639,   641,   643,   645,
     649,   655,   663,   664,   667,   668,   671,   672,   675,   676,
     677,   680,   681,   684,   685,   686,   687,   690,   691,   692,
     695,   696,   697,   698,   699,   702,   703,   704,   707,   717,
     718,   721,   725,   730,   735,   740,   742,   744,   746,   747,
     748,   749,   750,   751,   752,   754,   756,   758,   760,   761,
     762,   763,   764,   765,   766,   767,   768,   769,   770,   771,
     772,   773,   774,   775,   776,   777,   778
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
  "CMD_LIST", "CMD_EXPORT", "CMD_LABEL_ASGN", "L_PAREN", "R_PAREN",
  "ARG_IMMEDIATE", "REG_A", "REG_X", "REG_Y", "COMMA", "INST_SEP", "REG_B",
  "REG_C", "REG_D", "REG_E", "REG_H", "REG_L", "REG_AF", "REG_BC",
  "REG_DE", "REG_HL", "REG_IX", "REG_IY", "REG_SP", "REG_IXH", "REG_IXL",
  "REG_IYH", "REG_IYL", "STRING", "FILENAME", "R_O_L", "OPCODE", "LABEL",
  "BANKNAME", "CPUTYPE", "MON_REGISTER", "COMPARE_OP", "RADIX_TYPE",
  "INPUT_SPEC", "CMD_CHECKPT_ON", "CMD_CHECKPT_OFF", "TOGGLE", "MASK",
  "'+'", "'-'", "'*'", "'/'", "'('", "')'", "$accept", "top_level",
  "command_list", "end_cmd", "command", "machine_state_rules",
  "register_mod", "symbol_table_rules", "asm_rules", "$@1", "memory_rules",
  "checkpoint_rules", "checkpoint_control_rules", "monitor_state_rules",
  "monitor_misc_rules", "disk_rules", "cmd_file_rules", "data_entry_rules",
  "monitor_debug_rules", "rest_of_line", "opt_rest_of_line", "filename",
  "device_num", "opt_mem_op", "register", "reg_list", "reg_asgn",
  "checkpt_num", "address_opt_range", "address_range", "opt_address",
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
     395,   396,   397,   398,   399,   400,   401,   402,    43,    45,
      42,    47,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   154,   155,   155,   155,   156,   156,   157,   157,   157,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   160,   160,   160,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   163,   162,   162,   162,   162,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   167,   167,   167,   167,   167,   167,
     167,   167,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   170,   170,   170,
     171,   171,   172,   173,   174,   174,   175,   175,   176,   176,
     177,   177,   178,   178,   179,   179,   180,   181,   181,   182,
     182,   183,   183,   183,   184,   184,   185,   185,   185,   186,
     186,   187,   187,   187,   187,   187,   188,   189,   190,   190,
     190,   190,   190,   190,   190,   191,   191,   191,   191,   191,
     192,   192,   193,   193,   194,   194,   195,   195,   196,   196,
     196,   197,   197,   198,   198,   198,   198,   199,   199,   199,
     200,   200,   200,   200,   200,   201,   201,   201,   202,   203,
     203,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   204
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
       2,     2,     2,     4,     5,     5,     3,     3,     5,     4,
       3,     2,     2,     2,     2,     3,     3,     3,     5,     3,
       2,     5,     5,     4,     3,     2,     3,     2,     3,     2,
       2,     2,     3,     3,     2,     3,     3,     3,     3,     3,
       2,     3,     2,     3,     5,     3,     4,     2,     4,     4,
       2,     3,     5,     5,     5,     3,     4,     5,     4,     5,
       4,     5,     5,     2,     3,     4,     3,     3,     2,     3,
       4,     2,     2,     1,     1,     0,     1,     1,     1,     1,
       1,     0,     1,     2,     3,     1,     3,     1,     1,     1,
       1,     3,     1,     3,     2,     0,     1,     3,     1,     1,
       0,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     3,     1,     3,     3,     3,     3,     1,
       1,     1,     3,     1,     1,     1,     2,     1,     1,     1,
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
static const yytype_uint16 yydefact[] =
{
       0,     0,    22,     4,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   155,     0,
       0,     0,     0,     0,   180,     0,     0,     0,     0,     0,
     155,     0,     0,     0,   248,     0,     0,     0,     2,     5,
      10,    46,    11,    13,    12,    14,    15,    16,    17,    18,
      19,    20,    21,     0,   220,   221,   222,   223,   219,   218,
     217,   181,   182,   183,   184,   185,   162,     0,   212,     0,
       0,   194,   224,   211,     9,     8,     7,     0,   105,    34,
       0,     0,   179,    41,     0,    43,     0,   157,   156,     0,
       0,     0,   168,   213,   216,   215,   214,     0,   167,   172,
     178,   180,   180,   180,   176,   186,   187,   180,   180,     0,
     180,    47,     0,     0,   165,     0,     0,   107,   180,    72,
       0,   169,   180,    91,     0,    93,     0,    28,     0,   180,
       0,     0,   111,     9,   100,     0,     0,     0,     0,    65,
       0,    39,     0,    37,     0,     0,     0,   153,   114,     0,
     160,    94,     0,     0,     0,   110,     0,     0,    23,     0,
       0,   180,     0,   180,   180,     0,   180,    58,     0,     0,
     148,     0,    74,     0,    76,     0,    78,     0,    80,     0,
       0,   151,     0,     0,     0,    45,    92,     0,     0,    30,
     152,   120,     0,   122,   154,     0,     0,     0,     0,     0,
     127,     0,     0,   130,    32,     0,    81,    82,     0,   180,
       0,     9,   143,     0,   158,   109,     0,     0,     0,   249,
     250,   251,   252,   253,   254,   257,   260,   261,   262,   263,
     264,   265,   266,   255,   258,   256,   259,   232,   228,     0,
       0,     1,     6,     3,     0,   163,     0,     0,     0,     0,
     117,   104,   180,     0,     0,     0,   159,   180,   135,     0,
       0,    97,     0,     0,     0,     0,     0,     0,    27,     0,
       0,     0,    49,    48,   106,     0,    71,     0,    86,    90,
      29,     0,    35,    36,    99,     0,     0,     0,    63,     0,
      64,     0,     0,   113,   108,   115,     0,   112,   116,   118,
      25,    24,     0,    51,     0,    53,     0,     0,    55,     0,
      57,   147,   149,    73,    75,    77,    79,   205,     0,   203,
     204,   119,   180,     0,    87,    31,   123,     0,   121,   125,
       0,     0,   146,     0,     0,     0,     0,     0,   131,   144,
       0,     0,     0,     0,     0,     0,     0,     0,   231,     0,
      95,    96,   193,   192,   188,   189,   190,   191,     0,     0,
       0,    42,    44,     0,   136,     0,   140,     0,     0,   210,
     209,     0,   207,   208,   171,   173,   177,     0,     0,   166,
     164,     0,     0,   200,     0,   199,   201,     0,     0,   103,
       0,   230,   229,     0,    40,    38,    89,     0,     0,     0,
       0,     0,   150,     0,     0,   138,     0,     0,   126,   145,
     128,   129,    33,    83,     0,     0,     0,    59,   238,   239,
     240,   241,   242,   243,   235,     0,   233,   234,   141,   174,
     142,   132,   134,   139,    98,    69,   206,    68,    66,    70,
       0,     0,    88,    67,   101,   102,     0,   227,    62,    26,
      50,    52,    54,    56,   202,   133,   137,   124,    84,    85,
      60,     0,     0,   198,   197,   196,   195,   225,   226,   244,
     237,   245,   246,   247,   236
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    87,    88,   128,    89,    90,    91,    92,    93,   339,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   199,
     245,   139,   263,   202,   118,   163,   164,   147,   170,   171,
     408,   172,   409,   119,   154,   155,   264,   434,   435,   368,
     369,   421,   422,   121,   148,   122,   156,   441,   103,   443,
     288
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -338
static const yytype_int16 yypact[] =
{
    1168,   944,  -338,  -338,     3,   217,   944,   944,   827,   827,
       7,     7,     7,   248,  1160,  1160,  1160,  1274,    45,     6,
    1031,  1396,  1396,  1419,  1160,     7,     7,   217,   573,   248,
     248,  1274,  1396,   827,   827,   944,   628,    54,  1139,  -120,
    -120,   217,  -120,   663,   324,   324,  1274,   190,   708,     7,
     217,     7,  1396,  1396,  1396,  1396,  1274,   217,  -120,     7,
       7,   217,  1396,    31,   217,   217,     7,   217,  -111,   -77,
     -75,     7,   944,   827,   -71,   217,   827,   217,   827,     7,
    -111,   270,   217,    51,  1524,   248,   248,    69,  1289,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,    53,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,   944,  -338,   -62,
     114,  -338,  -338,  -338,  -338,  -338,  -338,   217,  -338,  -338,
     858,   858,  -338,  -338,   944,  -338,   944,  -338,  -338,   429,
     597,   429,  -338,  -338,  -338,  -338,  -338,   827,  -338,  -338,
    -338,   -71,   -71,   -71,  -338,  -338,  -338,   -71,   -71,   217,
     -71,  -338,    61,   103,  -338,    17,   217,  -338,   -71,  -338,
     217,  -338,    80,  -338,   177,  -338,   217,  -338,   217,   -71,
     217,   217,  -338,   220,  -338,   217,    71,     9,    78,  -338,
     217,  -338,   944,  -338,   944,   114,   217,  -338,  -338,   217,
    -338,  -338,  1160,   217,   217,  -338,   217,   217,  -338,    29,
     217,   -71,   217,   -71,   -71,   217,   -71,  -338,   217,   217,
    -338,   217,  -338,   217,  -338,   217,  -338,   217,  -338,   217,
     318,  -338,   217,   429,   429,  -338,  -338,   217,   217,  -338,
    -338,  -338,   827,  -338,  -338,   217,   217,   -42,   944,   114,
    -338,   944,   944,  -338,  -338,   944,  -338,  -338,   944,   -71,
     217,   255,  -338,   217,   151,  -338,  1274,   848,   787,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,   -20,  -338,   217,
     217,  -338,  -338,  -338,     2,  -338,   944,   944,   944,   944,
    -338,  -338,    87,   985,   114,   114,  -338,   165,  1442,  1462,
    1502,  -338,   944,   170,  1274,   902,   318,  1274,  -338,   787,
     787,   407,  -338,  -338,  -338,  1160,  -338,  1052,  -338,  -338,
    -338,  1274,  -338,  -338,  -338,  1052,   217,   -35,  -338,   -31,
    -338,   114,   114,  -338,  -338,  -338,   217,  -338,  -338,  -338,
    -338,  -338,   -21,  -338,     7,  -338,     7,   -19,  -338,   -16,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,   696,  -338,
    -338,  -338,   165,  1482,  -338,  -338,  -338,   944,  -338,  -338,
     217,   114,  -338,   114,   114,   114,   880,   944,  -338,  -338,
      89,    15,    19,    22,    23,    25,    35,   -76,  -338,   -90,
    -338,  -338,  -338,  -338,  -109,  -109,  -338,  -338,   217,  1274,
     217,  -338,  -338,   217,  -338,   217,  -338,   217,   114,  -338,
    -338,   458,  -338,  -338,  -338,  -338,  -338,   696,   217,  -338,
    -338,   217,  1052,  -338,    13,  -338,  -338,   217,    13,  -338,
     217,     5,    32,   217,  -338,  -338,  -338,   217,   217,   217,
     217,   217,  -338,   318,   217,  -338,   217,   114,  -338,  -338,
    -338,  -338,  -338,  -338,   217,   114,   217,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,     8,    34,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
      36,  1075,  -338,  -338,  -338,  -338,   -31,   -31,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,   360,    47,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -338,  -338,  -338,   330,    73,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,   274,
      92,    48,   249,  -338,   -17,  -338,  -137,   331,    -5,    -3,
    -276,   188,    11,   607,  -249,  -338,   688,  -326,  -338,  -130,
    -258,  -338,  -225,  -338,  -338,  -338,    -1,  -338,  -337,  -338,
    -338
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -181
static const yytype_int16 yytable[] =
{
     123,   162,   442,   402,   124,   123,   123,   124,   137,   438,
     336,   151,   157,   158,   124,   197,   174,   176,   124,   134,
     136,   179,   476,   477,   244,   125,   126,   190,   125,   126,
     124,   413,   124,   474,   123,   125,   126,   513,   475,   125,
     126,   298,   299,   132,   192,   194,   124,   223,   225,   227,
     229,   125,   126,   125,   126,   124,   246,   237,   247,   140,
     141,   111,   112,   113,   114,   115,   426,   125,   126,   291,
     426,   123,   266,   180,   181,   293,   125,   126,   295,   124,
     123,  -170,   320,   287,   251,   252,   335,   255,  -175,   258,
     124,   380,   210,   212,   399,  -170,   454,   219,   440,   221,
     125,   126,  -170,  -170,   124,    84,   490,   233,   234,  -175,
    -175,   125,   126,   466,   242,   124,   123,   447,   450,   248,
     496,   451,   511,   132,   468,   125,   126,   259,   469,   123,
     123,   470,   471,   123,   472,   123,   125,   126,   123,   123,
     123,   138,  -180,   132,   473,   514,   512,   497,   166,   127,
     296,   297,   298,   299,   491,   403,   524,   295,   312,   517,
     518,   292,   313,   314,   315,   516,  -175,  -180,   316,   317,
     238,   319,   260,   104,   105,   106,   107,   491,   124,   325,
     108,   109,   110,   314,   430,   116,   427,  -175,  -175,   197,
     331,   123,   327,   123,   132,   504,   486,   346,   337,   125,
     126,   132,   152,   152,   152,   159,   111,   112,   113,   114,
     115,   178,   152,     0,   -61,     0,     0,   321,   124,   188,
     352,  -168,   354,     0,   356,   357,     0,   359,     0,   370,
       0,     0,   123,   123,   214,   296,   297,   298,   299,   125,
     126,     0,  -168,  -168,   230,     0,     0,   123,     0,   142,
     123,   123,   143,   377,   123,     0,  -159,   123,   144,   145,
     146,     0,   296,   297,   298,   299,   397,   398,     0,     0,
     387,   261,     0,   104,   105,   106,   107,  -159,  -159,   132,
     108,   109,   110,     0,     0,     0,   111,   112,   113,   114,
     115,     0,   125,   126,     0,   123,   123,   123,   123,   296,
     297,   298,   299,   419,   162,     0,   415,     0,     0,     0,
     433,   123,   423,   203,   204,   370,   206,   420,   433,   429,
     431,   104,   105,   106,   107,   137,   436,   215,   108,   109,
     110,     0,   232,     0,   436,   129,     0,     0,   133,   135,
     111,   112,   113,   114,   115,     0,     0,     0,   161,   167,
     169,   173,   175,   177,     0,     0,     0,   182,   184,   185,
     186,   187,   189,   191,   193,     0,     0,   198,   201,     0,
     456,   205,     0,   208,     0,     0,   123,     0,   217,   453,
     220,   464,   222,   224,   226,   228,   123,   231,   307,   309,
     310,   235,   236,   239,   240,   241,     0,   243,     0,     0,
       0,     0,   448,   250,   449,   253,   254,   256,   257,     0,
     116,   262,   265,     0,     0,   433,   289,   290,     0,     0,
     423,     0,   117,   111,   112,   113,   114,   115,     0,     0,
     306,   436,   104,   105,   106,   107,     0,     0,   453,   108,
     109,   110,     0,     0,     0,   111,   112,   113,   114,   115,
     300,   367,   370,     0,   390,     0,     0,   301,   138,   124,
       0,   104,   105,   106,   107,     0,     0,     0,   108,   109,
     110,   519,     0,   520,   433,     0,     0,   311,     0,     0,
     125,   126,   372,   373,     0,   521,   522,   523,     0,   318,
     436,   410,     0,   322,     0,   323,   324,   152,   417,     0,
     326,     0,   424,     0,   328,   428,   329,     0,   330,     0,
     332,   333,     0,     0,     0,   334,     0,     0,   338,   437,
     340,     0,     0,     0,     0,   343,   344,     0,     0,   345,
       0,     0,     0,   347,   348,     0,   349,   350,     0,   351,
     353,     0,   355,     0,     0,   358,     0,   116,   360,   361,
       0,   362,     0,   363,     0,   364,     0,   365,     0,   366,
       0,   152,   371,     0,     0,     0,     0,   374,   375,   116,
       0,     0,   376,     0,   183,   378,   379,   143,     0,   382,
       0,   117,     0,   144,   145,   146,     0,     0,     0,     0,
     388,   419,     0,   389,     0,   125,   126,   479,   308,     0,
     104,   105,   106,   107,     0,   420,     0,   108,   109,   110,
       0,     0,     0,   111,   112,   113,   114,   115,     0,   400,
     401,   153,   153,   153,   160,   165,     0,   153,   153,   153,
     160,   153,     0,     0,   411,   412,     0,     0,   160,   153,
       0,     0,     0,   196,   111,   112,   113,   114,   115,     0,
     209,   211,   213,   160,   216,   218,     0,     0,     0,   153,
     153,   153,   153,   160,   124,     0,   439,     0,     0,   153,
       0,   444,   445,     0,     0,     0,   446,     0,     0,   111,
     112,   113,   114,   115,     0,   125,   126,     0,     0,   120,
       0,     0,     0,     0,   130,   131,     0,   124,   452,  -180,
    -180,  -180,  -180,     0,     0,     0,  -180,  -180,  -180,   124,
     458,   459,     0,   460,   461,   462,   463,     0,   125,   126,
     467,     0,     0,   195,   111,   112,   113,   114,   115,     0,
     125,   126,     0,     0,     0,     0,     0,   116,   478,     0,
     480,     0,     0,   481,     0,   482,     0,   483,   484,   117,
       0,   485,     0,     0,     0,     0,     0,   487,   488,     0,
     249,   489,     0,     0,   492,     0,     0,   493,   494,     0,
     495,     0,     0,   498,     0,     0,     0,   499,   500,   501,
     502,   503,     0,     0,   505,     0,   506,   507,     0,     0,
     104,   105,   106,   107,   508,   509,   510,   108,   109,   110,
       0,   207,     0,     0,     0,   294,     0,     0,     0,   153,
     132,     0,     0,     0,     0,     0,     0,     0,   302,   303,
       0,     0,   304,     0,   305,     0,     0,     0,   124,  -180,
    -180,  -180,  -180,  -180,     0,     0,     0,  -180,  -180,  -180,
       0,     0,     0,  -180,  -180,  -180,  -180,  -180,     0,   125,
     126,   104,   105,   106,   107,     0,     0,     0,   108,   109,
     110,   104,   105,   106,   107,     0,     0,     0,   108,   109,
     110,     0,     0,   160,   111,   112,   113,   114,   115,     0,
     341,   124,   342,   104,   105,   106,   107,     0,     0,   149,
     108,   109,   110,     0,     0,     0,   111,   112,   113,   114,
     115,     0,   125,   126,     0,   104,   105,   106,   107,     0,
     160,   425,   108,   109,   110,     0,   153,   160,     0,     0,
       0,   160,     0,     0,   160,     0,     0,     0,     0,     0,
       0,     0,   153,     0,     0,     0,   381,     0,   160,   383,
     384,   132,     0,   385,     0,     0,   386,   104,   105,   106,
     107,     0,     0,     0,   108,   109,   110,     0,     0,     0,
     111,   112,   113,   114,   115,     0,     0,  -180,     0,     0,
       0,   391,   392,   393,   394,   395,   396,     0,     0,  -180,
     153,     0,     0,     0,   404,   405,   406,   407,   104,   105,
     106,   107,     0,   153,     0,   108,   109,   110,   116,     0,
     418,   111,   112,   113,   114,   115,   296,   297,   298,   299,
     117,     0,     0,     0,     0,     0,   160,   150,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   296,   297,
     298,   299,   124,     0,   104,   105,   106,   107,     0,     0,
     149,   108,   109,   110,     0,     0,     0,   111,   112,   113,
     114,   115,     0,   125,   126,   104,   105,   106,   107,     0,
       0,     0,   108,   109,   110,   457,     0,     0,   111,   112,
     113,   114,   115,     0,     0,   465,   515,     0,   104,   105,
     106,   107,     0,     0,   116,   108,   109,   110,     0,     0,
       0,   111,   112,   113,   114,   115,   117,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   150,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   296,   297,   298,   299,     0,     0,     0,
     124,     0,  -161,  -161,  -161,  -161,     0,     0,  -161,  -161,
    -161,  -161,     0,   200,     0,  -161,  -161,  -161,  -161,  -161,
     432,   125,   126,   104,   105,   106,   107,     0,   150,   149,
     108,   109,   110,   168,     0,     1,   111,   112,   113,   114,
     115,     2,     0,   432,     0,     0,     0,     0,     0,     0,
       3,     0,   116,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,     0,     0,
      19,    20,    21,    22,    23,   116,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,  -161,   104,   105,   106,
     107,     0,     0,     0,   108,   109,   110,     0,     0,     0,
     111,   112,   113,   114,   115,     0,     1,   150,     0,     0,
       0,     0,     2,     0,    84,     0,     0,     0,     0,     0,
       0,     0,    85,    86,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,     0,
       0,    19,    20,    21,    22,    23,     0,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,   124,     0,   104,
     105,   106,   107,     0,     0,   149,   108,   109,   110,     0,
       0,   150,   111,   112,   113,   114,   115,     0,   125,   126,
     124,     0,   104,   105,   106,   107,     0,     0,     0,   108,
     109,   110,     0,    85,    86,   111,   112,   113,   114,   115,
       0,   125,   126,  -159,     0,  -159,  -159,  -159,  -159,     0,
       0,  -159,  -159,  -159,  -159,     0,     0,     0,  -159,  -159,
    -159,  -159,  -159,   414,     0,   104,   105,   106,   107,     0,
       0,   149,   108,   109,   110,     0,     0,     0,   111,   112,
     113,   114,   115,   455,     0,   104,   105,   106,   107,     0,
       0,   149,   108,   109,   110,     0,     0,     0,   111,   112,
     113,   114,   115,   416,     0,   104,   105,   106,   107,     0,
       0,     0,   108,   109,   110,     0,     0,     0,   111,   112,
     113,   114,   115,     0,     0,     0,     0,   104,   105,   106,
     107,     0,     0,   150,   108,   109,   110,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   150,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -159,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   150,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   150,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   267,     0,   268,   269,     0,     0,     0,   150,
     270,   271,   272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286
};

static const yytype_int16 yycheck[] =
{
       1,    18,   339,     1,     1,     6,     7,     1,     1,   335,
       1,    14,    15,    16,     1,   135,    21,    22,     1,     8,
       9,    24,   112,   113,   135,    22,    23,    32,    22,    23,
       1,   307,     1,   109,    35,    22,    23,     1,   114,    22,
      23,   150,   151,   114,    33,    34,     1,    52,    53,    54,
      55,    22,    23,    22,    23,     1,   133,    62,   133,    11,
      12,    16,    17,    18,    19,    20,   315,    22,    23,     0,
     319,    72,    21,    25,    26,    22,    22,    23,   140,     1,
      81,     1,    21,    84,    73,    74,    15,    76,     1,    78,
       1,   133,    44,    45,   114,    15,   372,    49,   133,    51,
      22,    23,    22,    23,     1,   136,   432,    59,    60,    22,
      23,    22,    23,    24,    66,     1,   117,   138,   137,    71,
     115,   137,   114,   114,   109,    22,    23,    79,   109,   130,
     131,   109,   109,   134,   109,   136,    22,    23,   139,   140,
     141,   134,   133,   114,   109,   109,   112,   115,   142,   146,
     148,   149,   150,   151,   141,   153,   109,   140,   147,   496,
     497,    88,   151,   152,   153,   491,     1,   138,   157,   158,
     139,   160,    80,     3,     4,     5,     6,   141,     1,   168,
      10,    11,    12,   172,   321,   140,   316,    22,    23,   135,
     179,   192,    15,   194,   114,   453,   421,   202,   187,    22,
      23,   114,    14,    15,    16,    17,    16,    17,    18,    19,
      20,    23,    24,    -1,   136,    -1,    -1,   114,     1,    31,
     209,     1,   211,    -1,   213,   214,    -1,   216,    -1,   230,
      -1,    -1,   233,   234,    46,   148,   149,   150,   151,    22,
      23,    -1,    22,    23,    56,    -1,    -1,   248,    -1,     1,
     251,   252,     4,   242,   255,    -1,     1,   258,    10,    11,
      12,    -1,   148,   149,   150,   151,   267,   268,    -1,    -1,
     259,     1,    -1,     3,     4,     5,     6,    22,    23,   114,
      10,    11,    12,    -1,    -1,    -1,    16,    17,    18,    19,
      20,    -1,    22,    23,    -1,   296,   297,   298,   299,   148,
     149,   150,   151,   133,   321,    -1,   309,    -1,    -1,    -1,
     327,   312,   313,    39,    40,   316,    42,   147,   335,   320,
     325,     3,     4,     5,     6,     1,   327,   137,    10,    11,
      12,    -1,    58,    -1,   335,     5,    -1,    -1,     8,     9,
      16,    17,    18,    19,    20,    -1,    -1,    -1,    18,    19,
      20,    21,    22,    23,    -1,    -1,    -1,    27,    28,    28,
      29,    30,    32,    33,    34,    -1,    -1,    37,    38,    -1,
     373,    41,    -1,    43,    -1,    -1,   377,    -1,    48,   368,
      50,   386,    52,    53,    54,    55,   387,    57,   139,   140,
     141,    61,    62,    63,    64,    65,    -1,    67,    -1,    -1,
      -1,    -1,   354,    73,   356,    75,    76,    77,    78,    -1,
     140,    81,    82,    -1,    -1,   432,    85,    86,    -1,    -1,
     421,    -1,   152,    16,    17,    18,    19,    20,    -1,    -1,
       1,   432,     3,     4,     5,     6,    -1,    -1,   427,    10,
      11,    12,    -1,    -1,    -1,    16,    17,    18,    19,    20,
     120,   133,   453,    -1,   266,    -1,    -1,   127,   134,     1,
      -1,     3,     4,     5,     6,    -1,    -1,    -1,    10,    11,
      12,   111,    -1,   113,   491,    -1,    -1,   147,    -1,    -1,
      22,    23,   233,   234,    -1,   125,   126,   127,    -1,   159,
     491,   303,    -1,   163,    -1,   165,   166,   309,   310,    -1,
     170,    -1,   314,    -1,   174,   317,   176,    -1,   178,    -1,
     180,   181,    -1,    -1,    -1,   185,    -1,    -1,   188,   331,
     190,    -1,    -1,    -1,    -1,   195,   196,    -1,    -1,   199,
      -1,    -1,    -1,   203,   204,    -1,   206,   207,    -1,   209,
     210,    -1,   212,    -1,    -1,   215,    -1,   140,   218,   219,
      -1,   221,    -1,   223,    -1,   225,    -1,   227,    -1,   229,
      -1,   373,   232,    -1,    -1,    -1,    -1,   237,   238,   140,
      -1,    -1,   242,    -1,     1,   245,   246,     4,    -1,   249,
      -1,   152,    -1,    10,    11,    12,    -1,    -1,    -1,    -1,
     260,   133,    -1,   263,    -1,    22,    23,   409,     1,    -1,
       3,     4,     5,     6,    -1,   147,    -1,    10,    11,    12,
      -1,    -1,    -1,    16,    17,    18,    19,    20,    -1,   289,
     290,    14,    15,    16,    17,    18,    -1,    20,    21,    22,
      23,    24,    -1,    -1,   304,   305,    -1,    -1,    31,    32,
      -1,    -1,    -1,    36,    16,    17,    18,    19,    20,    -1,
      43,    44,    45,    46,    47,    48,    -1,    -1,    -1,    52,
      53,    54,    55,    56,     1,    -1,   336,    -1,    -1,    62,
      -1,   341,   342,    -1,    -1,    -1,   346,    -1,    -1,    16,
      17,    18,    19,    20,    -1,    22,    23,    -1,    -1,     1,
      -1,    -1,    -1,    -1,     6,     7,    -1,     1,   368,     3,
       4,     5,     6,    -1,    -1,    -1,    10,    11,    12,     1,
     380,   381,    -1,   383,   384,   385,   386,    -1,    22,    23,
     390,    -1,    -1,    35,    16,    17,    18,    19,    20,    -1,
      22,    23,    -1,    -1,    -1,    -1,    -1,   140,   408,    -1,
     410,    -1,    -1,   413,    -1,   415,    -1,   417,   418,   152,
      -1,   421,    -1,    -1,    -1,    -1,    -1,   427,   428,    -1,
      72,   431,    -1,    -1,   434,    -1,    -1,   437,   438,    -1,
     440,    -1,    -1,   443,    -1,    -1,    -1,   447,   448,   449,
     450,   451,    -1,    -1,   454,    -1,   456,   457,    -1,    -1,
       3,     4,     5,     6,   464,   465,   466,    10,    11,    12,
      -1,   138,    -1,    -1,    -1,   117,    -1,    -1,    -1,   202,
     114,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   130,   131,
      -1,    -1,   134,    -1,   136,    -1,    -1,    -1,     1,   133,
       3,     4,     5,     6,    -1,    -1,    -1,    10,    11,    12,
      -1,    -1,    -1,    16,    17,    18,    19,    20,    -1,    22,
      23,     3,     4,     5,     6,    -1,    -1,    -1,    10,    11,
      12,     3,     4,     5,     6,    -1,    -1,    -1,    10,    11,
      12,    -1,    -1,   266,    16,    17,    18,    19,    20,    -1,
     192,     1,   194,     3,     4,     5,     6,    -1,    -1,     9,
      10,    11,    12,    -1,    -1,    -1,    16,    17,    18,    19,
      20,    -1,    22,    23,    -1,     3,     4,     5,     6,    -1,
     303,     9,    10,    11,    12,    -1,   309,   310,    -1,    -1,
      -1,   314,    -1,    -1,   317,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   325,    -1,    -1,    -1,   248,    -1,   331,   251,
     252,   114,    -1,   255,    -1,    -1,   258,     3,     4,     5,
       6,    -1,    -1,    -1,    10,    11,    12,    -1,    -1,    -1,
      16,    17,    18,    19,    20,    -1,    -1,   140,    -1,    -1,
      -1,   123,   124,   125,   126,   127,   128,    -1,    -1,   152,
     373,    -1,    -1,    -1,   296,   297,   298,   299,     3,     4,
       5,     6,    -1,   386,    -1,    10,    11,    12,   140,    -1,
     312,    16,    17,    18,    19,    20,   148,   149,   150,   151,
     152,    -1,    -1,    -1,    -1,    -1,   409,   137,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   148,   149,
     150,   151,     1,    -1,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    12,    -1,    -1,    -1,    16,    17,    18,
      19,    20,    -1,    22,    23,     3,     4,     5,     6,    -1,
      -1,    -1,    10,    11,    12,   377,    -1,    -1,    16,    17,
      18,    19,    20,    -1,    -1,   387,     1,    -1,     3,     4,
       5,     6,    -1,    -1,   140,    10,    11,    12,    -1,    -1,
      -1,    16,    17,    18,    19,    20,   152,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   137,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   148,   149,   150,   151,    -1,    -1,    -1,
       1,    -1,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    -1,    14,    -1,    16,    17,    18,    19,    20,
     108,    22,    23,     3,     4,     5,     6,    -1,   137,     9,
      10,    11,    12,   142,    -1,     7,    16,    17,    18,    19,
      20,    13,    -1,   108,    -1,    -1,    -1,    -1,    -1,    -1,
      22,    -1,   140,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    -1,    -1,
      42,    43,    44,    45,    46,   140,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   137,     3,     4,     5,
       6,    -1,    -1,    -1,    10,    11,    12,    -1,    -1,    -1,
      16,    17,    18,    19,    20,    -1,     7,   137,    -1,    -1,
      -1,    -1,    13,    -1,   136,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   144,   145,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    -1,
      -1,    42,    43,    44,    45,    46,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,     1,    -1,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    12,    -1,
      -1,   137,    16,    17,    18,    19,    20,    -1,    22,    23,
       1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,    10,
      11,    12,    -1,   144,   145,    16,    17,    18,    19,    20,
      -1,    22,    23,     1,    -1,     3,     4,     5,     6,    -1,
      -1,     9,    10,    11,    12,    -1,    -1,    -1,    16,    17,
      18,    19,    20,     1,    -1,     3,     4,     5,     6,    -1,
      -1,     9,    10,    11,    12,    -1,    -1,    -1,    16,    17,
      18,    19,    20,     1,    -1,     3,     4,     5,     6,    -1,
      -1,     9,    10,    11,    12,    -1,    -1,    -1,    16,    17,
      18,    19,    20,     1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    10,    11,    12,    -1,    -1,    -1,    16,    17,
      18,    19,    20,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,    -1,    -1,   137,    10,    11,    12,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   137,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   137,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   137,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   137,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   108,    -1,   110,   111,    -1,    -1,    -1,   137,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132
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
     104,   105,   106,   107,   136,   144,   145,   155,   156,   158,
     159,   160,   161,   162,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   202,     3,     4,     5,     6,    10,    11,
      12,    16,    17,    18,    19,    20,   140,   152,   178,   187,
     190,   197,   199,   200,     1,    22,    23,   146,   157,   157,
     190,   190,   114,   157,   186,   157,   186,     1,   134,   175,
     175,   175,     1,     4,    10,    11,    12,   181,   198,     9,
     137,   183,   185,   187,   188,   189,   200,   183,   183,   185,
     187,   157,   178,   179,   180,   187,   142,   157,   142,   157,
     182,   183,   185,   157,   182,   157,   182,   157,   185,   183,
     175,   175,   157,     1,   157,   181,   181,   181,   185,   157,
     182,   157,   186,   157,   186,   190,   187,   135,   157,   173,
      14,   157,   177,   173,   173,   157,   173,   138,   157,   187,
     175,   187,   175,   187,   185,   137,   187,   157,   187,   175,
     157,   175,   157,   182,   157,   182,   157,   182,   157,   182,
     185,   157,   173,   175,   175,   157,   157,   182,   139,   157,
     157,   157,   175,   157,   135,   174,   133,   133,   175,   190,
     157,   186,   186,   157,   157,   186,   157,   157,   186,   175,
     174,     1,   157,   176,   190,   157,    21,   108,   110,   111,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   200,   204,   181,
     181,     0,   158,    22,   190,   140,   148,   149,   150,   151,
     157,   157,   190,   190,   190,   190,     1,   176,     1,   176,
     176,   157,   186,   186,   186,   186,   186,   186,   157,   186,
      21,   114,   157,   157,   157,   186,   157,    15,   157,   157,
     157,   186,   157,   157,   157,    15,     1,   186,   157,   163,
     157,   190,   190,   157,   157,   157,   182,   157,   157,   157,
     157,   157,   186,   157,   186,   157,   186,   186,   157,   186,
     157,   157,   157,   157,   157,   157,   157,   133,   193,   194,
     200,   157,   176,   176,   157,   157,   157,   186,   157,   157,
     133,   190,   157,   190,   190,   190,   190,   186,   157,   157,
     185,   123,   124,   125,   126,   127,   128,   200,   200,   114,
     157,   157,     1,   153,   190,   190,   190,   190,   184,   186,
     185,   157,   157,   184,     1,   183,     1,   185,   190,   133,
     147,   195,   196,   200,   185,     9,   188,   193,   185,   200,
     180,   182,   108,   178,   191,   192,   200,   185,   191,   157,
     133,   201,   202,   203,   157,   157,   157,   138,   175,   175,
     137,   137,   157,   186,   184,     1,   183,   190,   157,   157,
     157,   157,   157,   157,   182,   190,    24,   157,   109,   109,
     109,   109,   109,   109,   109,   114,   112,   113,   157,   185,
     157,   157,   157,   157,   157,   157,   196,   157,   157,   157,
     191,   141,   157,   157,   157,   157,   115,   115,   157,   157,
     157,   157,   157,   157,   194,   157,   157,   157,   157,   157,
     157,   114,   112,     1,   109,     1,   191,   202,   202,   111,
     113,   125,   126,   127,   109
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
#line 186 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 187 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 188 "mon_parse.y"
    { new_cmd = 1; asm_mode = 0;  (yyval.i) = 0; }
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 197 "mon_parse.y"
    { return ERR_EXPECT_END_CMD; }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 212 "mon_parse.y"
    { return ERR_BAD_CMD; }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 216 "mon_parse.y"
    { mon_bank(e_default_space, NULL); }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 218 "mon_parse.y"
    { mon_bank((yyvsp[(2) - (3)].i), NULL); }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 220 "mon_parse.y"
    { mon_bank(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 222 "mon_parse.y"
    { mon_bank((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 224 "mon_parse.y"
    { mon_jump((yyvsp[(2) - (3)].a)); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 226 "mon_parse.y"
    { mon_display_io_regs(0); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 228 "mon_parse.y"
    { mon_display_io_regs((yyvsp[(2) - (3)].a)); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 230 "mon_parse.y"
    { monitor_cpu_type_set(""); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 232 "mon_parse.y"
    { monitor_cpu_type_set((yyvsp[(2) - (3)].str)); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 234 "mon_parse.y"
    { mon_cpuhistory(-1); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 236 "mon_parse.y"
    { mon_cpuhistory((yyvsp[(3) - (4)].i)); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 238 "mon_parse.y"
    { mon_instruction_return(); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 240 "mon_parse.y"
    { machine_write_snapshot((yyvsp[(2) - (3)].str),0,0,0); /* FIXME */ }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 242 "mon_parse.y"
    { machine_read_snapshot((yyvsp[(2) - (3)].str), 0); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 244 "mon_parse.y"
    { mon_instructions_step(-1); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 246 "mon_parse.y"
    { mon_instructions_step((yyvsp[(3) - (4)].i)); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 248 "mon_parse.y"
    { mon_instructions_next(-1); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 250 "mon_parse.y"
    { mon_instructions_next((yyvsp[(3) - (4)].i)); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 252 "mon_parse.y"
    { mon_stack_up(-1); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 254 "mon_parse.y"
    { mon_stack_up((yyvsp[(3) - (4)].i)); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 256 "mon_parse.y"
    { mon_stack_down(-1); }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 258 "mon_parse.y"
    { mon_stack_down((yyvsp[(3) - (4)].i)); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 260 "mon_parse.y"
    { mon_display_screen(); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 265 "mon_parse.y"
    { (monitor_cpu_for_memspace[default_memspace]->mon_register_print)(default_memspace); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 267 "mon_parse.y"
    { (monitor_cpu_for_memspace[(yyvsp[(2) - (3)].i)]->mon_register_print)((yyvsp[(2) - (3)].i)); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 272 "mon_parse.y"
    {
                        /* What about the memspace? */
                        mon_playback_init((yyvsp[(4) - (5)].str));
                    }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 277 "mon_parse.y"
    {
                        /* What about the memspace? */
                        mon_playback_init((yyvsp[(2) - (3)].str));
                    }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 282 "mon_parse.y"
    { mon_save_symbols((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 284 "mon_parse.y"
    { mon_save_symbols(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 286 "mon_parse.y"
    { mon_add_name_to_symbol_table((yyvsp[(2) - (5)].a), (yyvsp[(4) - (5)].str)); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 288 "mon_parse.y"
    { mon_remove_name_from_symbol_table(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 290 "mon_parse.y"
    { mon_remove_name_from_symbol_table((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 292 "mon_parse.y"
    { mon_print_symbol_table((yyvsp[(2) - (3)].i)); }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 294 "mon_parse.y"
    { mon_print_symbol_table(e_default_space); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 296 "mon_parse.y"
    {
                        mon_add_name_to_symbol_table((yyvsp[(3) - (4)].a), mon_prepend_dot_to_name((yyvsp[(1) - (4)].str)));
                    }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 300 "mon_parse.y"
    {
                        mon_add_name_to_symbol_table((yyvsp[(3) - (5)].a), mon_prepend_dot_to_name((yyvsp[(1) - (5)].str)));
                    }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 306 "mon_parse.y"
    { mon_start_assemble_mode((yyvsp[(2) - (2)].a), NULL); }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 307 "mon_parse.y"
    { }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 309 "mon_parse.y"
    { mon_start_assemble_mode((yyvsp[(2) - (3)].a), NULL); }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 311 "mon_parse.y"
    { mon_disassemble_lines((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1]); }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 313 "mon_parse.y"
    { mon_disassemble_lines(BAD_ADDR, BAD_ADDR); }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 317 "mon_parse.y"
    { mon_memory_move((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], (yyvsp[(4) - (5)].a)); }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 319 "mon_parse.y"
    { mon_memory_compare((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], (yyvsp[(4) - (5)].a)); }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 321 "mon_parse.y"
    { mon_memory_fill((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1],(unsigned char *)(yyvsp[(4) - (5)].str)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 323 "mon_parse.y"
    { mon_memory_hunt((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1],(unsigned char *)(yyvsp[(4) - (5)].str)); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 325 "mon_parse.y"
    { mon_memory_display((yyvsp[(2) - (5)].rt), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], DF_PETSCII); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 327 "mon_parse.y"
    { mon_memory_display(default_radix, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_PETSCII); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 329 "mon_parse.y"
    { mon_memory_display(default_radix, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 331 "mon_parse.y"
    { mon_memory_display_data((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], 8, 8); }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 333 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 8, 8); }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 335 "mon_parse.y"
    { mon_memory_display_data((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], 24, 21); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 337 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 24, 21); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 339 "mon_parse.y"
    { mon_memory_display(0, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_PETSCII); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 341 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 343 "mon_parse.y"
    { mon_memory_display(0, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_SCREEN_CODE); }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 345 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_SCREEN_CODE); }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 347 "mon_parse.y"
    { mon_memmap_zap(); }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 349 "mon_parse.y"
    { mon_memmap_show(-1,BAD_ADDR,BAD_ADDR); }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 351 "mon_parse.y"
    { mon_memmap_show((yyvsp[(3) - (4)].i),BAD_ADDR,BAD_ADDR); }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 353 "mon_parse.y"
    { mon_memmap_show((yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].range)[0],(yyvsp[(4) - (5)].range)[1]); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 355 "mon_parse.y"
    { mon_memmap_save((yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].i)); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 359 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], TRUE, e_exec, FALSE);
                  }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 363 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], TRUE, e_exec, TRUE);
                  }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 367 "mon_parse.y"
    {
                      temp = mon_breakpoint_add_checkpoint((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], TRUE, e_exec, FALSE);
                      mon_breakpoint_set_checkpoint_condition(temp, (yyvsp[(4) - (5)].cond_node));
                  }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 372 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(3) - (4)].range)[0], (yyvsp[(3) - (4)].range)[1], TRUE, (yyvsp[(2) - (4)].i), FALSE);
                  }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 376 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], FALSE, e_load | e_store | e_exec,
                                                    FALSE);
                  }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 381 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 383 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 385 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 387 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 392 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_ON, (yyvsp[(2) - (3)].i)); }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 394 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_OFF, (yyvsp[(2) - (3)].i)); }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 396 "mon_parse.y"
    { mon_breakpoint_set_ignore_count((yyvsp[(2) - (3)].i), -1); }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 398 "mon_parse.y"
    { mon_breakpoint_set_ignore_count((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].i)); }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 400 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint((yyvsp[(2) - (3)].i)); }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 402 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint(-1); }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 404 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_condition((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].cond_node)); }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 406 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_command((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 408 "mon_parse.y"
    { return ERR_EXPECT_STRING; }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 412 "mon_parse.y"
    { sidefx = (((yyvsp[(2) - (3)].action) == e_TOGGLE) ? (sidefx ^ 1) : (yyvsp[(2) - (3)].action)); }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 414 "mon_parse.y"
    {
                         mon_out("I/O side effects are %s\n",
                                   sidefx ? "enabled" : "disabled");
                     }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 419 "mon_parse.y"
    { default_radix = (yyvsp[(2) - (3)].rt); }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 421 "mon_parse.y"
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
#line 439 "mon_parse.y"
    { monitor_change_device((yyvsp[(2) - (3)].i)); }
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 441 "mon_parse.y"
    { mon_export(); }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 443 "mon_parse.y"
    { mon_quit(); YYACCEPT; }
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 445 "mon_parse.y"
    { exit_mon = 1; YYACCEPT; }
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 449 "mon_parse.y"
    { mon_drive_execute_disk_cmd((yyvsp[(2) - (3)].str)); }
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 451 "mon_parse.y"
    { mon_out("\t%d\n",(yyvsp[(2) - (3)].i)); }
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 453 "mon_parse.y"
    { mon_command_print_help(NULL); }
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 455 "mon_parse.y"
    { mon_command_print_help((yyvsp[(2) - (3)].str)); }
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 457 "mon_parse.y"
    { printf("SYSTEM COMMAND: %s\n",(yyvsp[(2) - (3)].str)); }
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 459 "mon_parse.y"
    { mon_print_convert((yyvsp[(2) - (3)].i)); }
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 461 "mon_parse.y"
    { mon_change_dir((yyvsp[(2) - (3)].str)); }
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 463 "mon_parse.y"
    { mon_keyboard_feed((yyvsp[(2) - (3)].str)); }
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 465 "mon_parse.y"
    { mon_backtrace(); }
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 467 "mon_parse.y"
    { mon_show_dir((yyvsp[(2) - (3)].str)); }
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 469 "mon_parse.y"
    { mon_show_pwd(); }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 471 "mon_parse.y"
    { mon_screenshot_save((yyvsp[(2) - (3)].str),-1); }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 473 "mon_parse.y"
    { mon_screenshot_save((yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].i)); }
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 475 "mon_parse.y"
    { mon_resource_get((yyvsp[(2) - (3)].str)); }
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 477 "mon_parse.y"
    { mon_resource_set((yyvsp[(2) - (4)].str),(yyvsp[(3) - (4)].str)); }
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 479 "mon_parse.y"
    { mon_reset_machine(-1); }
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 481 "mon_parse.y"
    { mon_reset_machine((yyvsp[(3) - (4)].i)); }
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 483 "mon_parse.y"
    { mon_tape_ctrl((yyvsp[(3) - (4)].i)); }
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 485 "mon_parse.y"
    { mon_cart_freeze(); }
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 487 "mon_parse.y"
    { }
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 491 "mon_parse.y"
    { mon_file_load((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].a), FALSE); }
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 493 "mon_parse.y"
    { mon_file_load((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].a), TRUE); }
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 495 "mon_parse.y"
    { mon_file_save((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], FALSE); }
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 497 "mon_parse.y"
    { return ERR_EXPECT_DEVICE_NUM; }
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 499 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 501 "mon_parse.y"
    { mon_file_save((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], TRUE); }
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 503 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 505 "mon_parse.y"
    { mon_file_verify((yyvsp[(2) - (5)].str),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 507 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 509 "mon_parse.y"
    { mon_drive_block_cmd(0,(yyvsp[(2) - (5)].i),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 511 "mon_parse.y"
    { mon_drive_block_cmd(1,(yyvsp[(2) - (5)].i),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 513 "mon_parse.y"
    { mon_drive_list(-1); }
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 515 "mon_parse.y"
    { mon_drive_list((yyvsp[(2) - (3)].i)); }
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 517 "mon_parse.y"
    { mon_attach((yyvsp[(2) - (4)].str),(yyvsp[(3) - (4)].i)); }
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 519 "mon_parse.y"
    { mon_detach((yyvsp[(2) - (3)].i)); }
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 523 "mon_parse.y"
    { mon_record_commands((yyvsp[(2) - (3)].str)); }
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 525 "mon_parse.y"
    { mon_end_recording(); }
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 527 "mon_parse.y"
    { mon_playback_init((yyvsp[(2) - (3)].str)); }
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 531 "mon_parse.y"
    { mon_memory_fill((yyvsp[(2) - (4)].a), BAD_ADDR, (unsigned char *)(yyvsp[(3) - (4)].str)); }
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 533 "mon_parse.y"
    { printf("Not yet.\n"); }
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 537 "mon_parse.y"
    { yydebug = 1; }
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 540 "mon_parse.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 543 "mon_parse.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 544 "mon_parse.y"
    { (yyval.str) = NULL; }
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 548 "mon_parse.y"
    { return ERR_EXPECT_FILENAME; }
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 552 "mon_parse.y"
    { return ERR_EXPECT_DEVICE_NUM; }
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 555 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 556 "mon_parse.y"
    { (yyval.i) = e_load | e_store | e_exec; }
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 559 "mon_parse.y"
    { (yyval.i) = new_reg(default_memspace, (yyvsp[(1) - (1)].reg)); }
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 560 "mon_parse.y"
    { (yyval.i) = new_reg((yyvsp[(1) - (2)].i), (yyvsp[(2) - (2)].reg)); }
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 568 "mon_parse.y"
    { (monitor_cpu_for_memspace[reg_memspace((yyvsp[(1) - (3)].i))]->mon_register_set_val)(reg_memspace((yyvsp[(1) - (3)].i)), reg_regid((yyvsp[(1) - (3)].i)), (WORD) (yyvsp[(3) - (3)].i)); }
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 571 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 572 "mon_parse.y"
    { return ERR_EXPECT_CHECKNUM; }
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 576 "mon_parse.y"
    { (yyval.range)[0] = (yyvsp[(1) - (1)].a); (yyval.range)[1] = BAD_ADDR; }
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 579 "mon_parse.y"
    { (yyval.range)[0] = (yyvsp[(1) - (3)].a); (yyval.range)[1] = (yyvsp[(3) - (3)].a); }
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 581 "mon_parse.y"
    { if (resolve_range(e_default_space, (yyval.range), (yyvsp[(1) - (1)].str))) return ERR_ADDR_TOO_BIG; }
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 583 "mon_parse.y"
    { if (resolve_range((yyvsp[(1) - (3)].i), (yyval.range), (yyvsp[(3) - (3)].str))) return ERR_ADDR_TOO_BIG; }
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 586 "mon_parse.y"
    { (yyval.a) = (yyvsp[(2) - (2)].a); }
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 587 "mon_parse.y"
    { (yyval.a) = BAD_ADDR; }
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 591 "mon_parse.y"
    {
             (yyval.a) = new_addr(e_default_space,(yyvsp[(1) - (1)].i));
             if (opt_asm) new_cmd = asm_mode = 1; 
         }
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 596 "mon_parse.y"
    {
             (yyval.a) = new_addr((yyvsp[(1) - (3)].i), (yyvsp[(3) - (3)].i));
             if (opt_asm) new_cmd = asm_mode = 1; 
         }
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 601 "mon_parse.y"
    {
             temp = mon_symbol_table_lookup_addr(e_default_space, (yyvsp[(1) - (1)].str));
             if (temp >= 0)
                 (yyval.a) = new_addr(e_default_space, temp);
             else
                 return ERR_UNDEFINED_LABEL;
         }
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 614 "mon_parse.y"
    { (yyval.i) = e_comp_space; }
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 615 "mon_parse.y"
    { (yyval.i) = e_disk8_space; }
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 616 "mon_parse.y"
    { (yyval.i) = e_disk9_space; }
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 617 "mon_parse.y"
    { (yyval.i) = e_disk10_space; }
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 618 "mon_parse.y"
    { (yyval.i) = e_disk11_space; }
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 621 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); if (!CHECK_ADDR((yyvsp[(1) - (1)].i))) return ERR_ADDR_TOO_BIG; }
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 623 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 625 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) + (yyvsp[(3) - (3)].i); }
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 626 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) - (yyvsp[(3) - (3)].i); }
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 627 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) * (yyvsp[(3) - (3)].i); }
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 628 "mon_parse.y"
    { (yyval.i) = ((yyvsp[(3) - (3)].i)) ? ((yyvsp[(1) - (3)].i) / (yyvsp[(3) - (3)].i)) : 1; }
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 629 "mon_parse.y"
    { (yyval.i) = (yyvsp[(2) - (3)].i); }
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 630 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 631 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 635 "mon_parse.y"
    {
               (yyval.cond_node) = new_cond; (yyval.cond_node)->is_parenthized = FALSE;
               (yyval.cond_node)->child1 = (yyvsp[(1) - (3)].cond_node); (yyval.cond_node)->child2 = (yyvsp[(3) - (3)].cond_node); (yyval.cond_node)->operation = (yyvsp[(2) - (3)].cond_op);
           }
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 640 "mon_parse.y"
    { return ERR_INCOMPLETE_COMPARE_OP; }
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 642 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(2) - (3)].cond_node); (yyval.cond_node)->is_parenthized = TRUE; }
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 644 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 646 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(1) - (1)].cond_node); }
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 649 "mon_parse.y"
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->reg_num = (yyvsp[(1) - (1)].i); (yyval.cond_node)->is_reg = TRUE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 655 "mon_parse.y"
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->value = (yyvsp[(1) - (1)].i); (yyval.cond_node)->is_reg = FALSE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 667 "mon_parse.y"
    { mon_add_number_to_buffer((yyvsp[(1) - (1)].i)); }
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 668 "mon_parse.y"
    { mon_add_string_to_buffer((yyvsp[(1) - (1)].str)); }
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 675 "mon_parse.y"
    { mon_add_number_to_buffer((yyvsp[(1) - (1)].i)); }
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 676 "mon_parse.y"
    { mon_add_number_masked_to_buffer((yyvsp[(1) - (1)].i), 0x00); }
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 677 "mon_parse.y"
    { mon_add_string_to_buffer((yyvsp[(1) - (1)].str)); }
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 680 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 681 "mon_parse.y"
    { (yyval.i) = (monitor_cpu_for_memspace[reg_memspace((yyvsp[(1) - (1)].i))]->mon_register_get_val)(reg_memspace((yyvsp[(1) - (1)].i)), reg_regid((yyvsp[(1) - (1)].i))); }
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 684 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 685 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 686 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 687 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 690 "mon_parse.y"
    { (yyval.i) = resolve_datatype(B_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 691 "mon_parse.y"
    { (yyval.i) = resolve_datatype(O_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 692 "mon_parse.y"
    { (yyval.i) = resolve_datatype(D_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 695 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 696 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 697 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 698 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 699 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 707 "mon_parse.y"
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

  case 230:

/* Line 1455 of yacc.c  */
#line 718 "mon_parse.y"
    { asm_mode = 0; }
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 721 "mon_parse.y"
    { if ((yyvsp[(2) - (2)].i) > 0xff)
                          (yyval.i) = join_ints(ASM_ADDR_MODE_IMMEDIATE_16,(yyvsp[(2) - (2)].i));
                        else
                          (yyval.i) = join_ints(ASM_ADDR_MODE_IMMEDIATE,(yyvsp[(2) - (2)].i)); }
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 725 "mon_parse.y"
    { if ((yyvsp[(1) - (1)].i) < 0x100)
               (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE,(yyvsp[(1) - (1)].i));
             else
               (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE,(yyvsp[(1) - (1)].i));
           }
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 730 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100)
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE_X,(yyvsp[(1) - (3)].i));
                          else
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_X,(yyvsp[(1) - (3)].i));
                        }
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 735 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100)
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE_Y,(yyvsp[(1) - (3)].i));
                          else
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_Y,(yyvsp[(1) - (3)].i));
                        }
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 741 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABS_INDIRECT,(yyvsp[(2) - (3)].i)); }
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 743 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_INDIRECT_X,(yyvsp[(2) - (5)].i)); }
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 745 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_INDIRECT_Y,(yyvsp[(2) - (5)].i)); }
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 746 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_BC,0); }
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 747 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_DE,0); }
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 748 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_HL,0); }
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 749 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_IX,0); }
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 750 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_IY,0); }
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 751 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_SP,0); }
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 753 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_A,(yyvsp[(2) - (5)].i)); }
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 755 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_HL,(yyvsp[(2) - (5)].i)); }
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 757 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_IX,(yyvsp[(2) - (5)].i)); }
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 759 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_IY,(yyvsp[(2) - (5)].i)); }
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 760 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_IMPLIED,0); }
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 761 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ACCUMULATOR,0); }
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 762 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_B,0); }
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 763 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_C,0); }
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 764 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_D,0); }
    break;

  case 253:

/* Line 1455 of yacc.c  */
#line 765 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_E,0); }
    break;

  case 254:

/* Line 1455 of yacc.c  */
#line 766 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_H,0); }
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 767 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IXH,0); }
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 768 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IYH,0); }
    break;

  case 257:

/* Line 1455 of yacc.c  */
#line 769 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_L,0); }
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 770 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IXL,0); }
    break;

  case 259:

/* Line 1455 of yacc.c  */
#line 771 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IYL,0); }
    break;

  case 260:

/* Line 1455 of yacc.c  */
#line 772 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_AF,0); }
    break;

  case 261:

/* Line 1455 of yacc.c  */
#line 773 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_BC,0); }
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 774 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_DE,0); }
    break;

  case 263:

/* Line 1455 of yacc.c  */
#line 775 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_HL,0); }
    break;

  case 264:

/* Line 1455 of yacc.c  */
#line 776 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IX,0); }
    break;

  case 265:

/* Line 1455 of yacc.c  */
#line 777 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IY,0); }
    break;

  case 266:

/* Line 1455 of yacc.c  */
#line 778 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_SP,0); }
    break;



/* Line 1455 of yacc.c  */
#line 4205 "mon_parse.c"
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
#line 782 "mon_parse.y"


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
         case ERR_EXPECT_CHECKNUM:
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


