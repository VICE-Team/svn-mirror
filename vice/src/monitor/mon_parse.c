
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
     CMD_STOPWATCH = 361,
     RESET = 362,
     CMD_EXPORT = 363,
     CMD_AUTOSTART = 364,
     CMD_AUTOLOAD = 365,
     CMD_LABEL_ASGN = 366,
     L_PAREN = 367,
     R_PAREN = 368,
     ARG_IMMEDIATE = 369,
     REG_A = 370,
     REG_X = 371,
     REG_Y = 372,
     COMMA = 373,
     INST_SEP = 374,
     L_BRACKET = 375,
     R_BRACKET = 376,
     LESS_THAN = 377,
     REG_U = 378,
     REG_S = 379,
     REG_PC = 380,
     REG_PCR = 381,
     REG_B = 382,
     REG_C = 383,
     REG_D = 384,
     REG_E = 385,
     REG_H = 386,
     REG_L = 387,
     REG_AF = 388,
     REG_BC = 389,
     REG_DE = 390,
     REG_HL = 391,
     REG_IX = 392,
     REG_IY = 393,
     REG_SP = 394,
     REG_IXH = 395,
     REG_IXL = 396,
     REG_IYH = 397,
     REG_IYL = 398,
     PLUS = 399,
     MINUS = 400,
     STRING = 401,
     FILENAME = 402,
     R_O_L = 403,
     OPCODE = 404,
     LABEL = 405,
     BANKNAME = 406,
     CPUTYPE = 407,
     MON_REGISTER = 408,
     COMPARE_OP = 409,
     RADIX_TYPE = 410,
     INPUT_SPEC = 411,
     CMD_CHECKPT_ON = 412,
     CMD_CHECKPT_OFF = 413,
     TOGGLE = 414,
     MASK = 415
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
#define CMD_STOPWATCH 361
#define RESET 362
#define CMD_EXPORT 363
#define CMD_AUTOSTART 364
#define CMD_AUTOLOAD 365
#define CMD_LABEL_ASGN 366
#define L_PAREN 367
#define R_PAREN 368
#define ARG_IMMEDIATE 369
#define REG_A 370
#define REG_X 371
#define REG_Y 372
#define COMMA 373
#define INST_SEP 374
#define L_BRACKET 375
#define R_BRACKET 376
#define LESS_THAN 377
#define REG_U 378
#define REG_S 379
#define REG_PC 380
#define REG_PCR 381
#define REG_B 382
#define REG_C 383
#define REG_D 384
#define REG_E 385
#define REG_H 386
#define REG_L 387
#define REG_AF 388
#define REG_BC 389
#define REG_DE 390
#define REG_HL 391
#define REG_IX 392
#define REG_IY 393
#define REG_SP 394
#define REG_IXH 395
#define REG_IXL 396
#define REG_IYH 397
#define REG_IYL 398
#define PLUS 399
#define MINUS 400
#define STRING 401
#define FILENAME 402
#define R_O_L 403
#define OPCODE 404
#define LABEL 405
#define BANKNAME 406
#define CPUTYPE 407
#define MON_REGISTER 408
#define COMPARE_OP 409
#define RADIX_TYPE 410
#define INPUT_SPEC 411
#define CMD_CHECKPT_ON 412
#define CMD_CHECKPT_OFF 413
#define TOGGLE 414
#define MASK 415




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
    asm_mode_addr_info_t mode;



/* Line 214 of yacc.c  */
#line 571 "mon_parse.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 583 "mon_parse.c"

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
#define YYFINAL  302
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1715

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  167
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  55
/* YYNRULES -- Number of rules.  */
#define YYNRULES  302
/* YYNRULES -- Number of states.  */
#define YYNSTATES  603

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   415

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     165,   166,   163,   161,     2,   162,     2,   164,     2,     2,
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
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160
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
     283,   286,   289,   292,   297,   303,   309,   315,   318,   322,
     325,   331,   334,   340,   343,   347,   351,   355,   361,   365,
     368,   374,   380,   385,   389,   392,   396,   399,   403,   406,
     409,   412,   416,   420,   423,   427,   431,   435,   439,   443,
     446,   450,   453,   457,   463,   467,   472,   475,   480,   485,
     488,   492,   496,   499,   505,   511,   517,   521,   526,   532,
     537,   543,   548,   554,   560,   563,   567,   572,   576,   580,
     586,   590,   596,   600,   603,   607,   612,   615,   618,   620,
     622,   623,   625,   627,   629,   631,   634,   636,   638,   639,
     641,   644,   648,   650,   654,   656,   658,   660,   662,   666,
     668,   672,   675,   676,   678,   682,   684,   686,   687,   689,
     691,   693,   695,   697,   699,   701,   705,   709,   713,   717,
     721,   725,   727,   730,   731,   735,   739,   743,   747,   749,
     751,   753,   757,   759,   761,   763,   766,   768,   770,   772,
     774,   776,   778,   780,   782,   784,   786,   788,   790,   792,
     794,   796,   798,   800,   802,   806,   810,   813,   816,   818,
     820,   823,   825,   829,   833,   837,   843,   849,   853,   857,
     861,   865,   869,   873,   879,   885,   891,   897,   898,   900,
     902,   904,   906,   908,   910,   912,   914,   916,   918,   920,
     922,   924,   926,   928,   930,   932,   934,   937,   941,   945,
     950,   954,   959,   962,   966,   970,   974,   978,   984,   990,
     997,  1003,  1010,  1015,  1021,  1027,  1033,  1039,  1043,  1045,
    1047,  1049,  1051
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     168,     0,    -1,   169,    -1,   217,    22,    -1,    22,    -1,
     171,    -1,   169,   171,    -1,    23,    -1,    22,    -1,     1,
      -1,   172,    -1,   174,    -1,   177,    -1,   175,    -1,   178,
      -1,   179,    -1,   180,    -1,   181,    -1,   182,    -1,   183,
      -1,   184,    -1,   185,    -1,    13,    -1,    67,   170,    -1,
      67,   201,   170,    -1,    67,   151,   170,    -1,    67,   201,
     200,   151,   170,    -1,    38,   199,   170,    -1,    46,   170,
      -1,    46,   199,   170,    -1,    87,   170,    -1,    87,   152,
     170,    -1,   100,   170,    -1,   100,   200,   204,   170,    -1,
      26,   170,    -1,    49,   188,   170,    -1,    50,   188,   170,
      -1,    58,   170,    -1,    58,   200,   204,   170,    -1,    57,
     170,    -1,    57,   200,   204,   170,    -1,    29,   170,    -1,
      29,   200,   204,   170,    -1,    30,   170,    -1,    30,   200,
     204,   170,    -1,    85,   170,    -1,   173,    -1,    39,   170,
      -1,    39,   201,   170,    -1,    39,   193,   170,    -1,    68,
     201,   200,   188,   170,    -1,    68,   188,   170,    -1,    69,
     201,   200,   188,   170,    -1,    69,   188,   170,    -1,    70,
     199,   200,   150,   170,    -1,    71,   150,   170,    -1,    71,
     201,   200,   150,   170,    -1,    72,   201,   170,    -1,    72,
     170,    -1,   111,    21,   199,   170,    -1,   111,    21,   199,
      24,   170,    -1,    -1,    55,   199,   176,   218,   170,    -1,
      55,   199,   170,    -1,    56,   196,   170,    -1,    56,   170,
      -1,    37,   197,   200,   199,   170,    -1,    48,   197,   200,
     199,   170,    -1,    36,   197,   200,   208,   170,    -1,    35,
     197,   200,   210,   170,    -1,    43,   155,   200,   196,   170,
      -1,    43,   196,   170,    -1,    43,   170,    -1,    76,   196,
     170,    -1,    76,   170,    -1,    77,   196,   170,    -1,    77,
     170,    -1,    78,   196,   170,    -1,    78,   170,    -1,    79,
     196,   170,    -1,    79,   170,    -1,   101,   170,    -1,   102,
     170,    -1,   102,   200,   204,   170,    -1,   102,   200,   204,
     196,   170,    -1,   103,   188,   200,   204,   170,    -1,    44,
     191,   196,   205,   170,    -1,    44,   170,    -1,    86,   196,
     170,    -1,    86,   170,    -1,    62,   191,   196,   205,   170,
      -1,    62,   170,    -1,    45,   191,   196,   205,   170,    -1,
      45,   170,    -1,   157,   195,   170,    -1,   158,   195,   170,
      -1,    34,   195,   170,    -1,    34,   195,   200,   204,   170,
      -1,    52,   195,   170,    -1,    52,   170,    -1,    53,   195,
      15,   206,   170,    -1,    54,   195,   200,   146,   170,    -1,
      54,   195,     1,   170,    -1,    25,   159,   170,    -1,    25,
     170,    -1,    42,   155,   170,    -1,    42,   170,    -1,    60,
     201,   170,    -1,   108,   170,    -1,    65,   170,    -1,    51,
     170,    -1,    63,   186,   170,    -1,    59,   204,   170,    -1,
      61,   170,    -1,    61,   186,   170,    -1,    64,   186,   170,
      -1,     7,   204,   170,    -1,    66,   186,   170,    -1,    82,
     186,   170,    -1,    89,   170,    -1,    92,   187,   170,    -1,
      91,   170,    -1,    90,   188,   170,    -1,    90,   188,   200,
     204,   170,    -1,    93,   146,   170,    -1,    94,   146,   146,
     170,    -1,    97,   170,    -1,    97,   200,   204,   170,    -1,
      98,   200,   204,   170,    -1,    99,   170,    -1,   104,   187,
     170,    -1,   106,   107,   170,    -1,   106,   170,    -1,    31,
     188,   189,   198,   170,    -1,    83,   188,   189,   198,   170,
      -1,    32,   188,   189,   197,   170,    -1,    32,   188,     1,
      -1,    32,   188,   189,     1,    -1,    84,   188,   189,   197,
     170,    -1,    84,   188,   189,     1,    -1,    33,   188,   189,
     199,   170,    -1,    33,   188,   189,     1,    -1,    27,   204,
     204,   198,   170,    -1,    28,   204,   204,   199,   170,    -1,
     105,   170,    -1,   105,   189,   170,    -1,    95,   188,   204,
     170,    -1,    96,   204,   170,    -1,   109,   188,   170,    -1,
     109,   188,   200,   215,   170,    -1,   110,   188,   170,    -1,
     110,   188,   200,   215,   170,    -1,    73,   188,   170,    -1,
      74,   170,    -1,    75,   188,   170,    -1,    80,   199,   208,
     170,    -1,    81,   170,    -1,    88,   170,    -1,   148,    -1,
     148,    -1,    -1,   147,    -1,     1,    -1,   204,    -1,     1,
      -1,   190,    14,    -1,    14,    -1,   190,    -1,    -1,   153,
      -1,   201,   153,    -1,   193,   118,   194,    -1,   194,    -1,
     192,    21,   215,    -1,   213,    -1,     1,    -1,   197,    -1,
     199,    -1,   199,   200,   199,    -1,     9,    -1,   201,   200,
       9,    -1,   200,   199,    -1,    -1,   202,    -1,   201,   200,
     202,    -1,   150,    -1,   118,    -1,    -1,    16,    -1,    17,
      -1,    18,    -1,    19,    -1,    20,    -1,   203,    -1,   215,
      -1,   204,   161,   204,    -1,   204,   162,   204,    -1,   204,
     163,   204,    -1,   204,   164,   204,    -1,   165,   204,   166,
      -1,   165,   204,     1,    -1,   212,    -1,    15,   206,    -1,
      -1,   206,   154,   206,    -1,   206,   154,     1,    -1,   112,
     206,   113,    -1,   112,   206,     1,    -1,   207,    -1,   192,
      -1,   215,    -1,   208,   200,   209,    -1,   209,    -1,   215,
      -1,   146,    -1,   210,   211,    -1,   211,    -1,   215,    -1,
     160,    -1,   146,    -1,   215,    -1,   192,    -1,     4,    -1,
      12,    -1,    11,    -1,    10,    -1,    12,    -1,    11,    -1,
      10,    -1,     3,    -1,     4,    -1,     5,    -1,     6,    -1,
     214,    -1,   216,   119,   217,    -1,   217,   119,   217,    -1,
     217,   119,    -1,   149,   219,    -1,   217,    -1,   216,    -1,
     114,   215,    -1,   215,    -1,   215,   118,   116,    -1,   215,
     118,   117,    -1,   112,   215,   113,    -1,   112,   215,   118,
     116,   113,    -1,   112,   215,   113,   118,   117,    -1,   112,
     134,   113,    -1,   112,   135,   113,    -1,   112,   136,   113,
      -1,   112,   137,   113,    -1,   112,   138,   113,    -1,   112,
     139,   113,    -1,   112,   215,   113,   118,   115,    -1,   112,
     215,   113,   118,   136,    -1,   112,   215,   113,   118,   137,
      -1,   112,   215,   113,   118,   138,    -1,    -1,   115,    -1,
     127,    -1,   128,    -1,   129,    -1,   130,    -1,   131,    -1,
     140,    -1,   142,    -1,   132,    -1,   141,    -1,   143,    -1,
     133,    -1,   134,    -1,   135,    -1,   136,    -1,   137,    -1,
     138,    -1,   139,    -1,   122,   215,    -1,   215,   118,   221,
      -1,   118,   220,   144,    -1,   118,   220,   144,   144,    -1,
     118,   145,   220,    -1,   118,   145,   145,   220,    -1,   118,
     220,    -1,   127,   118,   220,    -1,   115,   118,   220,    -1,
     129,   118,   220,    -1,   215,   118,   125,    -1,   120,   215,
     118,   220,   121,    -1,   120,   118,   220,   144,   121,    -1,
     120,   118,   220,   144,   144,   121,    -1,   120,   118,   145,
     220,   121,    -1,   120,   118,   145,   145,   220,   121,    -1,
     120,   118,   220,   121,    -1,   120,   127,   118,   220,   121,
      -1,   120,   115,   118,   220,   121,    -1,   120,   129,   118,
     220,   121,    -1,   120,   215,   118,   125,   121,    -1,   120,
     215,   121,    -1,   116,    -1,   117,    -1,   221,    -1,   123,
      -1,   124,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   191,   191,   192,   193,   196,   197,   200,   201,   202,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   220,   222,   224,   226,   228,   230,   232,
     234,   236,   238,   240,   242,   244,   246,   248,   250,   252,
     254,   256,   258,   260,   262,   264,   266,   269,   271,   273,
     276,   281,   286,   288,   290,   292,   294,   296,   298,   300,
     304,   311,   310,   313,   315,   317,   321,   323,   325,   327,
     329,   331,   333,   335,   337,   339,   341,   343,   345,   347,
     349,   351,   353,   355,   357,   359,   363,   372,   375,   379,
     382,   391,   394,   403,   408,   410,   412,   414,   416,   418,
     420,   422,   424,   428,   430,   435,   437,   455,   457,   459,
     461,   465,   467,   469,   471,   473,   475,   477,   479,   481,
     483,   485,   487,   489,   491,   493,   495,   497,   499,   501,
     503,   505,   507,   511,   513,   515,   517,   519,   521,   523,
     525,   527,   529,   531,   533,   535,   537,   539,   541,   543,
     545,   547,   551,   553,   555,   559,   561,   565,   569,   572,
     573,   576,   577,   580,   581,   584,   585,   588,   589,   592,
     593,   596,   597,   600,   604,   605,   608,   609,   612,   613,
     615,   619,   620,   623,   628,   633,   643,   644,   647,   648,
     649,   650,   651,   654,   656,   658,   659,   660,   661,   662,
     663,   664,   667,   668,   670,   675,   677,   679,   681,   685,
     691,   699,   700,   703,   704,   707,   708,   711,   712,   713,
     716,   717,   720,   721,   722,   723,   726,   727,   728,   731,
     732,   733,   734,   735,   738,   739,   740,   743,   753,   754,
     757,   764,   772,   780,   788,   790,   792,   794,   795,   796,
     797,   798,   799,   800,   802,   804,   806,   808,   809,   810,
     811,   812,   813,   814,   815,   816,   817,   818,   819,   820,
     821,   822,   823,   824,   825,   826,   828,   829,   844,   848,
     852,   856,   860,   864,   868,   872,   876,   888,   903,   907,
     911,   915,   919,   923,   927,   931,   935,   947,   956,   957,
     958,   962,   963
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
  "CMD_LIST", "CMD_STOPWATCH", "RESET", "CMD_EXPORT", "CMD_AUTOSTART",
  "CMD_AUTOLOAD", "CMD_LABEL_ASGN", "L_PAREN", "R_PAREN", "ARG_IMMEDIATE",
  "REG_A", "REG_X", "REG_Y", "COMMA", "INST_SEP", "L_BRACKET", "R_BRACKET",
  "LESS_THAN", "REG_U", "REG_S", "REG_PC", "REG_PCR", "REG_B", "REG_C",
  "REG_D", "REG_E", "REG_H", "REG_L", "REG_AF", "REG_BC", "REG_DE",
  "REG_HL", "REG_IX", "REG_IY", "REG_SP", "REG_IXH", "REG_IXL", "REG_IYH",
  "REG_IYL", "PLUS", "MINUS", "STRING", "FILENAME", "R_O_L", "OPCODE",
  "LABEL", "BANKNAME", "CPUTYPE", "MON_REGISTER", "COMPARE_OP",
  "RADIX_TYPE", "INPUT_SPEC", "CMD_CHECKPT_ON", "CMD_CHECKPT_OFF",
  "TOGGLE", "MASK", "'+'", "'-'", "'*'", "'/'", "'('", "')'", "$accept",
  "top_level", "command_list", "end_cmd", "command", "machine_state_rules",
  "register_mod", "symbol_table_rules", "asm_rules", "$@1", "memory_rules",
  "checkpoint_rules", "checkpoint_control_rules", "monitor_state_rules",
  "monitor_misc_rules", "disk_rules", "cmd_file_rules", "data_entry_rules",
  "monitor_debug_rules", "rest_of_line", "opt_rest_of_line", "filename",
  "device_num", "mem_op", "opt_mem_op", "register", "reg_list", "reg_asgn",
  "checkpt_num", "address_opt_range", "address_range", "opt_address",
  "address", "opt_sep", "memspace", "memloc", "memaddr", "expression",
  "opt_if_cond_expr", "cond_expr", "compare_operand", "data_list",
  "data_element", "hunt_list", "hunt_element", "value", "d_number",
  "guess_default", "number", "assembly_instr_list", "assembly_instruction",
  "post_assemble", "asm_operand_mode", "index_reg", "index_usreg", 0
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
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,    43,    45,    42,    47,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   167,   168,   168,   168,   169,   169,   170,   170,   170,
     171,   171,   171,   171,   171,   171,   171,   171,   171,   171,
     171,   171,   171,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   173,   173,   173,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   176,   175,   175,   175,   175,   177,   177,   177,   177,
     177,   177,   177,   177,   177,   177,   177,   177,   177,   177,
     177,   177,   177,   177,   177,   177,   178,   178,   178,   178,
     178,   178,   178,   178,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   180,   180,   180,   180,   180,   180,   180,
     180,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   183,   183,   183,   184,   184,   185,   186,   187,
     187,   188,   188,   189,   189,   190,   190,   191,   191,   192,
     192,   193,   193,   194,   195,   195,   196,   196,   197,   197,
     197,   198,   198,   199,   199,   199,   200,   200,   201,   201,
     201,   201,   201,   202,   203,   204,   204,   204,   204,   204,
     204,   204,   205,   205,   206,   206,   206,   206,   206,   207,
     207,   208,   208,   209,   209,   210,   210,   211,   211,   211,
     212,   212,   213,   213,   213,   213,   214,   214,   214,   215,
     215,   215,   215,   215,   216,   216,   216,   217,   218,   218,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   220,   220,
     220,   221,   221
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
       2,     2,     2,     4,     5,     5,     5,     2,     3,     2,
       5,     2,     5,     2,     3,     3,     3,     5,     3,     2,
       5,     5,     4,     3,     2,     3,     2,     3,     2,     2,
       2,     3,     3,     2,     3,     3,     3,     3,     3,     2,
       3,     2,     3,     5,     3,     4,     2,     4,     4,     2,
       3,     3,     2,     5,     5,     5,     3,     4,     5,     4,
       5,     4,     5,     5,     2,     3,     4,     3,     3,     5,
       3,     5,     3,     2,     3,     4,     2,     2,     1,     1,
       0,     1,     1,     1,     1,     2,     1,     1,     0,     1,
       2,     3,     1,     3,     1,     1,     1,     1,     3,     1,
       3,     2,     0,     1,     3,     1,     1,     0,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     1,     2,     0,     3,     3,     3,     3,     1,     1,
       1,     3,     1,     1,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     2,     2,     1,     1,
       2,     1,     3,     3,     3,     5,     5,     3,     3,     3,
       3,     3,     3,     5,     5,     5,     5,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     3,     3,     4,
       3,     4,     2,     3,     3,     3,     3,     5,     5,     6,
       5,     6,     4,     5,     5,     5,     5,     3,     1,     1,
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
       0,     0,     0,     0,     0,     0,     0,     0,   160,     0,
       0,     0,     0,     0,   187,     0,     0,     0,     0,     0,
     160,     0,     0,     0,     0,     0,     0,   257,     0,     0,
       0,     2,     5,    10,    46,    11,    13,    12,    14,    15,
      16,    17,    18,    19,    20,    21,     0,   229,   230,   231,
     232,   228,   227,   226,   188,   189,   190,   191,   192,   169,
       0,   221,     0,     0,   201,   233,   220,     9,     8,     7,
       0,   104,    34,     0,     0,   186,    41,     0,    43,     0,
     162,   161,     0,     0,     0,   175,   222,   225,   224,   223,
       0,   174,   179,   185,   187,   187,   187,   183,   193,   194,
     187,   187,     0,   187,    47,     0,     0,   172,     0,     0,
     106,   187,    72,     0,   176,   187,   166,    87,   167,     0,
      93,     0,    28,     0,   187,     0,     0,   110,     9,    99,
       0,     0,     0,     0,    65,     0,    39,     0,    37,     0,
       0,     0,   158,   113,     0,    91,     0,     0,     0,   109,
       0,     0,    23,     0,     0,   187,     0,   187,   187,     0,
     187,    58,     0,     0,   153,     0,    74,     0,    76,     0,
      78,     0,    80,     0,     0,   156,     0,     0,     0,    45,
      89,     0,     0,    30,   157,   119,     0,   121,   159,     0,
       0,     0,     0,     0,   126,     0,     0,   129,    32,     0,
      81,    82,     0,   187,     0,     9,   144,     0,   163,     0,
     132,   108,     0,     0,     0,     0,     0,   258,     0,     0,
       0,   259,   260,   261,   262,   263,   266,   269,   270,   271,
     272,   273,   274,   275,   264,   267,   265,   268,   241,   237,
       0,     0,     1,     6,     3,     0,   170,     0,     0,     0,
       0,   116,   103,   187,     0,     0,     0,   164,   187,   136,
       0,     0,    96,     0,     0,     0,     0,     0,     0,    27,
       0,     0,     0,    49,    48,   105,     0,    71,   165,   203,
     203,    29,     0,    35,    36,    98,     0,     0,     0,    63,
       0,    64,     0,     0,   112,   107,   114,   203,   111,   115,
     117,    25,    24,     0,    51,     0,    53,     0,     0,    55,
       0,    57,   152,   154,    73,    75,    77,    79,   214,     0,
     212,   213,   118,   187,     0,    88,    31,   122,     0,   120,
     124,     0,     0,   147,     0,     0,     0,     0,     0,   130,
     145,   131,   148,     0,   150,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   240,     0,   298,   299,   301,   302,
       0,   282,   300,     0,     0,     0,     0,     0,   276,     0,
       0,     0,    94,    95,   200,   199,   195,   196,   197,   198,
       0,     0,     0,    42,    44,     0,   137,     0,   141,     0,
       0,   219,   218,     0,   216,   217,   178,   180,   184,     0,
       0,   173,   171,     0,     0,     0,     0,     0,     0,   209,
       0,   208,   210,   102,     0,   239,   238,     0,    40,    38,
       0,     0,     0,     0,     0,     0,   155,     0,     0,   139,
       0,     0,   125,   146,   127,   128,    33,    83,     0,     0,
       0,     0,     0,    59,   247,   248,   249,   250,   251,   252,
     244,     0,   284,     0,   280,   278,     0,     0,     0,     0,
       0,     0,   297,   283,   285,   242,   243,   286,   277,   142,
     181,   143,   133,   135,   140,    97,    69,   215,    68,    66,
      70,   202,    86,    92,    67,     0,     0,   100,   101,     0,
     236,    62,    90,    26,    50,    52,    54,    56,   211,   134,
     138,   123,    84,    85,   149,   151,    60,     0,     0,   281,
     279,     0,     0,     0,   292,     0,     0,     0,     0,     0,
     207,   206,   205,   204,   234,   235,   253,   246,   254,   255,
     256,   245,   294,     0,   290,   288,     0,   293,   295,   296,
     287,   291,   289
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    90,    91,   131,    92,    93,    94,    95,    96,   350,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   204,
     249,   142,   267,   178,   179,   121,   166,   167,   150,   173,
     174,   440,   175,   441,   122,   157,   158,   268,   465,   470,
     471,   379,   380,   453,   454,   124,   151,   125,   159,   475,
     106,   477,   299,   421,   422
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -396
static const yytype_int16 yypact[] =
{
    1185,   917,  -396,  -396,    25,   193,   917,   917,    88,    88,
       6,     6,     6,   608,  1171,  1171,  1171,  1515,   616,    13,
    1046,  1149,  1149,  1432,  1171,     6,     6,   193,  1102,   608,
     608,  1515,  1297,    88,    88,   917,   759,    65,  1149,  -126,
    -126,   193,  -126,   207,   425,   425,  1515,   253,   711,     6,
     193,     6,  1297,  1297,  1297,  1297,  1515,   193,  -126,     6,
       6,   193,  1297,    41,   193,   193,     6,   193,  -124,   -90,
     -70,     6,   917,    88,   -67,   193,    88,   193,    88,     6,
    -124,   472,    80,   193,     6,     6,    76,  1556,   608,   608,
     109,  1319,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,
    -396,  -396,  -396,  -396,  -396,  -396,    91,  -396,  -396,  -396,
    -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,
     917,  -396,   -27,    18,  -396,  -396,  -396,  -396,  -396,  -396,
     193,  -396,  -396,   754,   754,  -396,  -396,   917,  -396,   917,
    -396,  -396,   111,   498,   111,  -396,  -396,  -396,  -396,  -396,
      88,  -396,  -396,  -396,   -67,   -67,   -67,  -396,  -396,  -396,
     -67,   -67,   193,   -67,  -396,   114,    73,  -396,    32,   193,
    -396,   -67,  -396,   193,  -396,   144,  -396,  -396,   123,  1171,
    -396,  1171,  -396,   193,   -67,   193,   193,  -396,   221,  -396,
     193,   137,    16,    61,  -396,   193,  -396,   917,  -396,   917,
      18,   193,  -396,  -396,   193,  -396,  1171,   193,   193,  -396,
     193,   193,  -396,   117,   193,   -67,   193,   -67,   -67,   193,
     -67,  -396,   193,   193,  -396,   193,  -396,   193,  -396,   193,
    -396,   193,  -396,   193,   328,  -396,   193,   111,   111,  -396,
    -396,   193,   193,  -396,  -396,  -396,    88,  -396,  -396,   193,
     193,    30,   917,    18,  -396,   917,   917,  -396,  -396,   917,
    -396,  -396,   917,   -67,   193,   414,  -396,   193,   392,   193,
    -396,  -396,  1597,  1597,  1515,   656,   882,    46,   389,  1585,
     882,    59,  -396,    70,  -396,  -396,  -396,  -396,  -396,  -396,
    -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,    71,  -396,
     193,   193,  -396,  -396,  -396,     9,  -396,   917,   917,   917,
     917,  -396,  -396,    56,   955,    18,    18,  -396,   124,  1455,
    1477,   279,  -396,   917,   454,  1515,   734,   328,  1515,  -396,
     882,   882,   229,  -396,  -396,  -396,  1171,  -396,  -396,   177,
     177,  -396,  1515,  -396,  -396,  -396,   998,   193,    49,  -396,
      48,  -396,    18,    18,  -396,  -396,  -396,   177,  -396,  -396,
    -396,  -396,  -396,    51,  -396,     6,  -396,     6,    55,  -396,
      78,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  1535,
    -396,  -396,  -396,   124,  1497,  -396,  -396,  -396,   917,  -396,
    -396,   193,    18,  -396,    18,    18,    18,   861,   917,  -396,
    -396,  -396,  -396,   882,  -396,   882,   426,   108,   125,   126,
     139,   143,   150,   -95,  -396,   188,  -396,  -396,  -396,  -396,
     404,   121,  -396,   159,   415,   163,   168,    42,  -396,   188,
     188,   553,  -396,  -396,  -396,  -396,  -114,  -114,  -396,  -396,
     193,  1515,   193,  -396,  -396,   193,  -396,   193,  -396,   193,
      18,  -396,  -396,   977,  -396,  -396,  -396,  -396,  -396,  1535,
     193,  -396,  -396,   193,   998,   193,   193,   193,   998,  -396,
      36,  -396,  -396,  -396,   193,   173,   174,   193,  -396,  -396,
     193,   193,   193,   193,   193,   193,  -396,   328,   193,  -396,
     193,    18,  -396,  -396,  -396,  -396,  -396,  -396,   193,    18,
     193,   193,   193,  -396,  -396,  -396,  -396,  -396,  -396,  -396,
     176,   184,  -396,   188,  -396,   166,   188,   478,  -106,   188,
     188,   575,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,
    -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,
    -396,   160,  -396,  -396,  -396,     7,  1073,  -396,  -396,    48,
      48,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,
    -396,  -396,  -396,  -396,  -396,  -396,  -396,   231,   203,  -396,
    -396,   197,   188,   198,  -396,   -20,   216,   220,   223,   230,
    -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,
    -396,  -396,  -396,   232,  -396,  -396,   235,  -396,  -396,  -396,
    -396,  -396,  -396
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -396,  -396,  -396,   357,   233,  -396,  -396,  -396,  -396,  -396,
    -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,   192,
     248,   276,   354,  -396,    39,   -17,  -396,    20,   171,   -23,
      -3,  -293,    29,    -6,   670,  -310,  -396,   647,  -275,  -395,
    -396,    37,  -128,  -396,   -83,  -396,  -396,  -396,    -1,  -396,
    -346,  -396,  -396,   523,   -68
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -188
static const yytype_int16 yytable[] =
{
     126,   165,   137,   139,   476,   126,   126,   140,   580,   195,
     434,   154,   160,   161,   127,   574,   458,   347,   510,   127,
     458,   184,   202,   511,   248,   445,   127,   197,   199,   227,
     229,   231,   233,   127,   126,   128,   129,   127,   575,   241,
     128,   129,   127,   155,   155,   155,   162,   128,   129,   309,
     310,   135,   183,   155,   128,   129,   250,  -182,   128,   129,
     193,   181,   127,   128,   129,   466,   127,   255,   256,   541,
     259,   126,   262,   545,   127,   218,   251,   206,  -182,  -182,
     126,   127,   480,   128,   129,   234,   298,   128,   129,   127,
     488,  -187,  -187,  -187,  -187,   128,   129,   274,  -187,  -187,
    -187,   595,   128,   129,  -187,  -187,  -187,  -187,  -187,   302,
     128,   129,   317,   304,   107,   108,   109,   110,   127,   126,
     581,   111,   112,   113,   596,  -182,   306,   114,   115,   116,
     117,   118,   126,   126,   135,   331,   126,   338,   126,   128,
     129,   126,   126,   126,   323,  -177,  -182,  -182,   324,   325,
     326,   583,   346,   141,   327,   328,   339,   330,   340,  -177,
     521,   546,  -187,   522,   415,   336,  -177,  -177,   169,   325,
     307,   308,   309,   310,   135,   435,   391,   429,   342,   307,
     308,   309,   310,   357,   130,   306,   348,   269,   430,   431,
     546,   332,   464,   242,   127,   474,   126,    87,   126,   190,
     191,   192,   481,   584,   585,   484,   135,   363,   127,   365,
     -61,   367,   368,   202,   370,   128,   129,   307,   308,   309,
     310,   504,  -175,   114,   115,   116,   117,   118,   485,   128,
     129,   207,   208,   381,   210,   135,   126,   126,   505,   506,
     388,  -187,   135,  -175,  -175,   114,   115,   116,   117,   118,
     236,   126,   507,  -187,   126,   126,   508,   398,   126,   300,
     301,   126,   135,   509,   119,   515,   403,   405,  -187,   114,
     115,   116,   117,   118,   413,   414,   120,   516,   427,   428,
     448,   519,   107,   108,   109,   110,   520,   143,   144,   111,
     112,   113,   549,   550,   567,   114,   115,   116,   117,   118,
     568,   185,   186,   406,   416,   417,   126,   126,   126,   126,
     570,   418,   419,   463,   546,   165,   591,   447,   592,   594,
     214,   216,   126,   455,   303,   223,   381,   225,   264,   469,
     461,   107,   108,   109,   110,   237,   238,   597,   111,   112,
     113,   598,   246,   442,   599,   472,   586,   252,   587,   155,
     449,   600,   462,   601,   456,   263,   602,   460,   211,   558,
     272,   273,   132,   528,   459,   136,   138,   588,   589,   590,
     537,   467,     0,   487,   498,   164,   170,   172,   177,   180,
     182,   490,   119,     0,   187,   189,     0,   126,     0,   194,
     196,   198,     0,     0,   203,   205,     0,   126,   209,     0,
     212,     0,   500,   219,   501,   221,     0,   224,     0,   226,
     228,   230,   232,   155,   235,  -164,     0,     0,   239,   240,
     243,   244,   245,     0,   247,     0,   140,   127,     0,   153,
     254,     0,   257,   258,   260,   261,  -164,  -164,   266,   270,
     271,   114,   115,   116,   117,   118,     0,   469,   128,   129,
     502,   469,   455,   487,     0,     0,     0,   107,   108,   109,
     110,     0,     0,   472,   111,   112,   113,   472,     0,     0,
     530,     0,     0,   265,   378,   107,   108,   109,   110,     0,
     311,     0,   111,   112,   113,     0,   381,   312,   114,   115,
     116,   117,   118,     0,   128,   129,   318,   320,   321,   319,
       0,   107,   108,   109,   110,   416,   417,   322,   111,   112,
     113,     0,   418,   419,   114,   115,   116,   117,   118,   329,
     416,   417,     0,   333,     0,   334,   335,   418,   419,   469,
     337,   416,   417,     0,   420,     0,     0,     0,   418,   419,
     341,     0,   343,   344,     0,   472,     0,   345,     0,   513,
     349,     0,   351,   307,   308,   309,   310,   354,   355,     0,
     517,   356,     0,     0,   358,   359,     0,   360,   361,     0,
     362,   364,   141,   366,     0,     0,   369,     0,     0,   371,
     372,     0,   373,     0,   374,     0,   375,     0,   376,     0,
     377,   383,   384,   382,   416,   417,     0,     0,   385,   386,
     451,   418,   419,   387,     0,     0,   389,   390,     0,   145,
     393,     0,   146,     0,   452,     0,     0,   127,   147,   148,
     149,   399,     0,   572,   400,   119,   401,     0,     0,   402,
     404,     0,   114,   115,   116,   117,   118,   120,   128,   129,
       0,   482,     0,   483,     0,     0,     0,     0,   123,     0,
       0,   119,     0,   133,   134,     0,     0,   432,   433,   107,
     108,   109,   110,   120,     0,     0,   111,   112,   113,   525,
     526,     0,   443,   444,     0,     0,   418,   419,   527,     0,
       0,     0,   200,     0,   156,   156,   156,   163,   168,     0,
     156,   416,   417,   163,   156,     0,     0,     0,   418,   419,
     578,   163,   156,     0,   473,     0,   201,     0,     0,   478,
     479,     0,   127,   213,   215,   217,   163,   220,   222,   253,
       0,     0,   156,   156,   156,   156,   163,   114,   115,   116,
     117,   118,   156,   128,   129,     0,   486,   107,   108,   109,
     110,     0,     0,   457,   111,   112,   113,     0,   492,   493,
       0,   494,   495,   496,   497,     0,     0,   107,   108,   109,
     110,     0,     0,   503,   111,   112,   113,   305,     0,   119,
     114,   115,   116,   117,   118,   114,   115,   116,   117,   118,
     313,   314,     0,     0,   315,     0,   316,     0,     0,     0,
     407,   408,   409,   410,   411,   412,     0,   529,     0,   531,
       0,     0,   532,     0,   533,     0,   534,   535,     0,     0,
     536,     0,     0,     0,     0,     0,   538,   539,     0,     0,
     540,     0,   542,   543,   544,     0,     0,   547,     0,     0,
       0,   548,     0,     0,   551,     0,     0,   552,   553,   554,
     555,   556,   557,     0,   352,   559,   353,   560,   561,   156,
       0,   156,     0,     0,     0,   562,   563,   564,   565,   566,
       0,     0,   127,     0,   107,   108,   109,   110,     0,     0,
     152,   111,   112,   113,     0,     0,   156,   114,   115,   116,
     117,   118,     0,   128,   129,   107,   108,   109,   110,     0,
       0,     0,   111,   112,   113,     0,     0,     0,     0,   392,
       0,     0,   394,   395,     0,     0,   396,   119,     0,   397,
       0,     0,     0,     0,     0,   307,   308,   309,   310,   120,
     107,   108,   109,   110,     0,     0,     0,   111,   112,   113,
       0,     0,     0,   114,   115,   116,   117,   118,   512,     0,
       0,     0,     0,   514,   163,     0,     0,   518,     0,     0,
       0,     0,   523,   524,   436,   437,   438,   439,   107,   108,
     109,   110,     0,     0,     0,   111,   112,   113,     0,     0,
     450,   114,   115,   116,   117,   118,     0,     0,   127,     0,
     107,   108,   109,   110,   163,     0,     0,   111,   112,   113,
     156,   163,     0,     0,     0,   163,     0,     0,   163,   128,
     129,   107,   108,   109,   110,     0,   156,     0,   111,   112,
     113,   153,   163,     0,   114,   115,   116,   117,   118,     0,
       0,     0,   307,   308,   309,   310,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   491,   569,     0,     0,   571,
     573,     0,   576,   577,   579,   499,     0,   127,     0,   107,
     108,   109,   110,     0,   156,   152,   111,   112,   113,     0,
       0,     0,   114,   115,   116,   117,   118,   156,   128,   129,
     119,     0,     0,     0,   582,     0,   107,   108,   109,   110,
       0,     0,   120,   111,   112,   113,     0,     0,     0,   114,
     115,   116,   117,   118,     0,   593,     0,     0,     0,     0,
       0,     0,     0,   188,     0,   153,   146,     0,     0,     0,
     468,   163,   147,   148,   149,     0,   307,   308,   309,   310,
       0,     0,     0,   451,   128,   129,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   452,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     127,   119,  -168,  -168,  -168,  -168,     0,     0,  -168,  -168,
    -168,  -168,     0,   176,     0,  -168,  -168,  -168,  -168,  -168,
       0,   128,   129,     0,   107,   108,   109,   110,     0,     0,
     152,   111,   112,   113,     0,   468,     0,   114,   115,   116,
     117,   118,     1,     0,     0,     0,   153,     0,     2,     0,
       0,   171,     0,     0,     0,     0,     0,     3,     0,     0,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,     0,   119,    19,    20,    21,
      22,    23,     0,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,     0,    83,    84,    85,    86,     0,   127,  -168,
     107,   108,   109,   110,     0,     0,   152,   111,   112,   113,
       0,     0,     0,   114,   115,   116,   117,   118,     0,   128,
     129,   153,     0,     0,     0,     0,     1,     0,     0,     0,
       0,     0,     2,     0,    87,     0,     0,     0,     0,     0,
       0,     0,    88,    89,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,     0,
       0,    19,    20,    21,    22,    23,     0,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,     0,    83,    84,    85,
      86,     0,     0,   127,     0,   107,   108,   109,   110,     0,
       0,     0,   111,   112,   113,     0,     0,   153,   114,   115,
     116,   117,   118,     0,   128,   129,  -164,     0,  -164,  -164,
    -164,  -164,     0,     0,  -164,  -164,  -164,  -164,     0,     0,
       0,  -164,  -164,  -164,  -164,  -164,    88,    89,   446,     0,
     107,   108,   109,   110,     0,     0,   152,   111,   112,   113,
       0,     0,     0,   114,   115,   116,   117,   118,   489,     0,
     107,   108,   109,   110,     0,     0,   152,   111,   112,   113,
       0,     0,     0,   114,   115,   116,   117,   118,   107,   108,
     109,   110,     0,     0,     0,   111,   112,   113,     0,     0,
       0,   114,   115,   116,   117,   118,   127,     0,  -187,  -187,
    -187,  -187,     0,     0,     0,  -187,  -187,  -187,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   128,   129,   107,
     108,   109,   110,     0,     0,     0,   111,   112,   113,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   153,     0,     0,     0,     0,     0,   107,   108,
     109,   110,     0,     0,     0,   111,   112,   113,   127,     0,
    -187,  -187,  -187,  -187,     0,  -164,     0,  -187,  -187,  -187,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   128,
     129,     0,     0,     0,     0,     0,     0,   153,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   153,     0,     0,
       0,     0,     0,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   153,     0,     0,   275,     0,
     276,   277,     0,     0,   278,     0,   279,     0,   280,     0,
       0,  -187,     0,   281,   282,   283,   284,   285,   286,   287,
     288,   289,   290,   291,   292,   293,   294,   295,   296,   297,
     423,     0,     0,   424,     0,     0,     0,     0,     0,     0,
       0,     0,   425,     0,   426,   135
};

static const yytype_int16 yycheck[] =
{
       1,    18,     8,     9,   350,     6,     7,     1,     1,    32,
       1,    14,    15,    16,     1,   121,   326,     1,   113,     1,
     330,    24,   148,   118,   148,   318,     1,    33,    34,    52,
      53,    54,    55,     1,    35,    22,    23,     1,   144,    62,
      22,    23,     1,    14,    15,    16,    17,    22,    23,   163,
     164,   118,    23,    24,    22,    23,   146,     1,    22,    23,
      31,    22,     1,    22,    23,   340,     1,    73,    74,   464,
      76,    72,    78,   468,     1,    46,   146,    38,    22,    23,
      81,     1,   357,    22,    23,    56,    87,    22,    23,     1,
     383,     3,     4,     5,     6,    22,    23,    21,    10,    11,
      12,   121,    22,    23,    16,    17,    18,    19,    20,     0,
      22,    23,     1,    22,     3,     4,     5,     6,     1,   120,
     113,    10,    11,    12,   144,     1,   153,    16,    17,    18,
      19,    20,   133,   134,   118,    21,   137,    14,   139,    22,
      23,   142,   143,   144,   150,     1,    22,    23,   154,   155,
     156,   546,    15,   147,   160,   161,   179,   163,   181,    15,
     118,   154,   146,   121,   118,   171,    22,    23,   155,   175,
     161,   162,   163,   164,   118,   166,   146,   118,   184,   161,
     162,   163,   164,   206,   159,   153,   192,   107,   118,   118,
     154,   118,    15,   152,     1,   146,   197,   149,   199,    28,
      29,    30,   151,   549,   550,   150,   118,   213,     1,   215,
     149,   217,   218,   148,   220,    22,    23,   161,   162,   163,
     164,   113,     1,    16,    17,    18,    19,    20,   150,    22,
      23,    39,    40,   234,    42,   118,   237,   238,   113,   113,
     246,   153,   118,    22,    23,    16,    17,    18,    19,    20,
      58,   252,   113,   165,   255,   256,   113,   263,   259,    88,
      89,   262,   118,   113,   153,   144,   272,   273,   151,    16,
      17,    18,    19,    20,   275,   276,   165,   118,   279,   280,
       1,   118,     3,     4,     5,     6,   118,    11,    12,    10,
      11,    12,   119,   119,   118,    16,    17,    18,    19,    20,
     116,    25,    26,   274,   116,   117,   307,   308,   309,   310,
     144,   123,   124,   336,   154,   332,   113,   320,   121,   121,
      44,    45,   323,   324,    91,    49,   327,    51,    80,   346,
     331,     3,     4,     5,     6,    59,    60,   121,    10,    11,
      12,   121,    66,   314,   121,   346,   115,    71,   117,   320,
     321,   121,   332,   121,   325,    79,   121,   328,   151,   487,
      84,    85,     5,   431,   327,     8,     9,   136,   137,   138,
     453,   342,    -1,   379,   397,    18,    19,    20,    21,    22,
      23,   384,   153,    -1,    27,    28,    -1,   388,    -1,    32,
      33,    34,    -1,    -1,    37,    38,    -1,   398,    41,    -1,
      43,    -1,   403,   150,   405,    48,    -1,    50,    -1,    52,
      53,    54,    55,   384,    57,     1,    -1,    -1,    61,    62,
      63,    64,    65,    -1,    67,    -1,     1,     1,    -1,   150,
      73,    -1,    75,    76,    77,    78,    22,    23,    81,    82,
      83,    16,    17,    18,    19,    20,    -1,   464,    22,    23,
      24,   468,   453,   459,    -1,    -1,    -1,     3,     4,     5,
       6,    -1,    -1,   464,    10,    11,    12,   468,    -1,    -1,
     441,    -1,    -1,     1,   146,     3,     4,     5,     6,    -1,
     123,    -1,    10,    11,    12,    -1,   487,   130,    16,    17,
      18,    19,    20,    -1,    22,    23,   142,   143,   144,     1,
      -1,     3,     4,     5,     6,   116,   117,   150,    10,    11,
      12,    -1,   123,   124,    16,    17,    18,    19,    20,   162,
     116,   117,    -1,   166,    -1,   168,   169,   123,   124,   546,
     173,   116,   117,    -1,   145,    -1,    -1,    -1,   123,   124,
     183,    -1,   185,   186,    -1,   546,    -1,   190,    -1,   145,
     193,    -1,   195,   161,   162,   163,   164,   200,   201,    -1,
     145,   204,    -1,    -1,   207,   208,    -1,   210,   211,    -1,
     213,   214,   147,   216,    -1,    -1,   219,    -1,    -1,   222,
     223,    -1,   225,    -1,   227,    -1,   229,    -1,   231,    -1,
     233,   237,   238,   236,   116,   117,    -1,    -1,   241,   242,
     146,   123,   124,   246,    -1,    -1,   249,   250,    -1,     1,
     253,    -1,     4,    -1,   160,    -1,    -1,     1,    10,    11,
      12,   264,    -1,   145,   267,   153,   269,    -1,    -1,   272,
     273,    -1,    16,    17,    18,    19,    20,   165,    22,    23,
      -1,   365,    -1,   367,    -1,    -1,    -1,    -1,     1,    -1,
      -1,   153,    -1,     6,     7,    -1,    -1,   300,   301,     3,
       4,     5,     6,   165,    -1,    -1,    10,    11,    12,   116,
     117,    -1,   315,   316,    -1,    -1,   123,   124,   125,    -1,
      -1,    -1,    35,    -1,    14,    15,    16,    17,    18,    -1,
      20,   116,   117,    23,    24,    -1,    -1,    -1,   123,   124,
     125,    31,    32,    -1,   347,    -1,    36,    -1,    -1,   352,
     353,    -1,     1,    43,    44,    45,    46,    47,    48,    72,
      -1,    -1,    52,    53,    54,    55,    56,    16,    17,    18,
      19,    20,    62,    22,    23,    -1,   379,     3,     4,     5,
       6,    -1,    -1,     9,    10,    11,    12,    -1,   391,   392,
      -1,   394,   395,   396,   397,    -1,    -1,     3,     4,     5,
       6,    -1,    -1,   406,    10,    11,    12,   120,    -1,   153,
      16,    17,    18,    19,    20,    16,    17,    18,    19,    20,
     133,   134,    -1,    -1,   137,    -1,   139,    -1,    -1,    -1,
     134,   135,   136,   137,   138,   139,    -1,   440,    -1,   442,
      -1,    -1,   445,    -1,   447,    -1,   449,   450,    -1,    -1,
     453,    -1,    -1,    -1,    -1,    -1,   459,   460,    -1,    -1,
     463,    -1,   465,   466,   467,    -1,    -1,   470,    -1,    -1,
      -1,   474,    -1,    -1,   477,    -1,    -1,   480,   481,   482,
     483,   484,   485,    -1,   197,   488,   199,   490,   491,   179,
      -1,   181,    -1,    -1,    -1,   498,   499,   500,   501,   502,
      -1,    -1,     1,    -1,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    12,    -1,    -1,   206,    16,    17,    18,
      19,    20,    -1,    22,    23,     3,     4,     5,     6,    -1,
      -1,    -1,    10,    11,    12,    -1,    -1,    -1,    -1,   252,
      -1,    -1,   255,   256,    -1,    -1,   259,   153,    -1,   262,
      -1,    -1,    -1,    -1,    -1,   161,   162,   163,   164,   165,
       3,     4,     5,     6,    -1,    -1,    -1,    10,    11,    12,
      -1,    -1,    -1,    16,    17,    18,    19,    20,   415,    -1,
      -1,    -1,    -1,   420,   274,    -1,    -1,   424,    -1,    -1,
      -1,    -1,   429,   430,   307,   308,   309,   310,     3,     4,
       5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,    -1,
     323,    16,    17,    18,    19,    20,    -1,    -1,     1,    -1,
       3,     4,     5,     6,   314,    -1,    -1,    10,    11,    12,
     320,   321,    -1,    -1,    -1,   325,    -1,    -1,   328,    22,
      23,     3,     4,     5,     6,    -1,   336,    -1,    10,    11,
      12,   150,   342,    -1,    16,    17,    18,    19,    20,    -1,
      -1,    -1,   161,   162,   163,   164,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   388,   513,    -1,    -1,   516,
     517,    -1,   519,   520,   521,   398,    -1,     1,    -1,     3,
       4,     5,     6,    -1,   384,     9,    10,    11,    12,    -1,
      -1,    -1,    16,    17,    18,    19,    20,   397,    22,    23,
     153,    -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,
      -1,    -1,   165,    10,    11,    12,    -1,    -1,    -1,    16,
      17,    18,    19,    20,    -1,   572,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     1,    -1,   150,     4,    -1,    -1,    -1,
     112,   441,    10,    11,    12,    -1,   161,   162,   163,   164,
      -1,    -1,    -1,   146,    22,    23,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   160,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       1,   153,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    -1,    14,    -1,    16,    17,    18,    19,    20,
      -1,    22,    23,    -1,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    12,    -1,   112,    -1,    16,    17,    18,
      19,    20,     7,    -1,    -1,    -1,   150,    -1,    13,    -1,
      -1,   155,    -1,    -1,    -1,    -1,    -1,    22,    -1,    -1,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    -1,   153,    42,    43,    44,
      45,    46,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,    -1,   108,   109,   110,   111,    -1,     1,   150,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      -1,    -1,    -1,    16,    17,    18,    19,    20,    -1,    22,
      23,   150,    -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,
      -1,    -1,    13,    -1,   149,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   157,   158,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    -1,
      -1,    42,    43,    44,    45,    46,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,    -1,   108,   109,   110,
     111,    -1,    -1,     1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    10,    11,    12,    -1,    -1,   150,    16,    17,
      18,    19,    20,    -1,    22,    23,     1,    -1,     3,     4,
       5,     6,    -1,    -1,     9,    10,    11,    12,    -1,    -1,
      -1,    16,    17,    18,    19,    20,   157,   158,     1,    -1,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      -1,    -1,    -1,    16,    17,    18,    19,    20,     1,    -1,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      -1,    -1,    -1,    16,    17,    18,    19,    20,     3,     4,
       5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,    -1,
      -1,    16,    17,    18,    19,    20,     1,    -1,     3,     4,
       5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    22,    23,     3,
       4,     5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   150,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,    -1,    -1,    -1,    10,    11,    12,     1,    -1,
       3,     4,     5,     6,    -1,   150,    -1,    10,    11,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    22,
      23,    -1,    -1,    -1,    -1,    -1,    -1,   150,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   150,    -1,    -1,
      -1,    -1,    -1,   118,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   150,    -1,    -1,   112,    -1,
     114,   115,    -1,    -1,   118,    -1,   120,    -1,   122,    -1,
      -1,   146,    -1,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     115,    -1,    -1,   118,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   127,    -1,   129,   118
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
     104,   105,   106,   108,   109,   110,   111,   149,   157,   158,
     168,   169,   171,   172,   173,   174,   175,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   217,     3,     4,     5,
       6,    10,    11,    12,    16,    17,    18,    19,    20,   153,
     165,   192,   201,   204,   212,   214,   215,     1,    22,    23,
     159,   170,   170,   204,   204,   118,   170,   200,   170,   200,
       1,   147,   188,   188,   188,     1,     4,    10,    11,    12,
     195,   213,     9,   150,   197,   199,   201,   202,   203,   215,
     197,   197,   199,   201,   170,   192,   193,   194,   201,   155,
     170,   155,   170,   196,   197,   199,    14,   170,   190,   191,
     170,   191,   170,   199,   197,   188,   188,   170,     1,   170,
     195,   195,   195,   199,   170,   196,   170,   200,   170,   200,
     204,   201,   148,   170,   186,   170,   191,   186,   186,   170,
     186,   151,   170,   201,   188,   201,   188,   201,   199,   150,
     201,   170,   201,   188,   170,   188,   170,   196,   170,   196,
     170,   196,   170,   196,   199,   170,   186,   188,   188,   170,
     170,   196,   152,   170,   170,   170,   188,   170,   148,   187,
     146,   146,   188,   204,   170,   200,   200,   170,   170,   200,
     170,   170,   200,   188,   187,     1,   170,   189,   204,   107,
     170,   170,   188,   188,    21,   112,   114,   115,   118,   120,
     122,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   215,   219,
     195,   195,     0,   171,    22,   204,   153,   161,   162,   163,
     164,   170,   170,   204,   204,   204,   204,     1,   189,     1,
     189,   189,   170,   200,   200,   200,   200,   200,   200,   170,
     200,    21,   118,   170,   170,   170,   200,   170,    14,   196,
     196,   170,   200,   170,   170,   170,    15,     1,   200,   170,
     176,   170,   204,   204,   170,   170,   170,   196,   170,   170,
     170,   170,   170,   200,   170,   200,   170,   200,   200,   170,
     200,   170,   170,   170,   170,   170,   170,   170,   146,   208,
     209,   215,   170,   189,   189,   170,   170,   170,   200,   170,
     170,   146,   204,   170,   204,   204,   204,   204,   200,   170,
     170,   170,   170,   200,   170,   200,   199,   134,   135,   136,
     137,   138,   139,   215,   215,   118,   116,   117,   123,   124,
     145,   220,   221,   115,   118,   127,   129,   215,   215,   118,
     118,   118,   170,   170,     1,   166,   204,   204,   204,   204,
     198,   200,   199,   170,   170,   198,     1,   197,     1,   199,
     204,   146,   160,   210,   211,   215,   199,     9,   202,   208,
     199,   215,   194,   196,    15,   205,   205,   199,   112,   192,
     206,   207,   215,   170,   146,   216,   217,   218,   170,   170,
     205,   151,   188,   188,   150,   150,   170,   200,   198,     1,
     197,   204,   170,   170,   170,   170,   170,   170,   196,   204,
     215,   215,    24,   170,   113,   113,   113,   113,   113,   113,
     113,   118,   220,   145,   220,   144,   118,   145,   220,   118,
     118,   118,   121,   220,   220,   116,   117,   125,   221,   170,
     199,   170,   170,   170,   170,   170,   170,   211,   170,   170,
     170,   206,   170,   170,   170,   206,   154,   170,   170,   119,
     119,   170,   170,   170,   170,   170,   170,   170,   209,   170,
     170,   170,   170,   170,   170,   170,   170,   118,   116,   220,
     144,   220,   145,   220,   121,   144,   220,   220,   125,   220,
       1,   113,     1,   206,   217,   217,   115,   117,   136,   137,
     138,   113,   121,   220,   121,   121,   144,   121,   121,   121,
     121,   121,   121
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
#line 191 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 192 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 193 "mon_parse.y"
    { new_cmd = 1; asm_mode = 0;  (yyval.i) = 0; }
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 202 "mon_parse.y"
    { return ERR_EXPECT_END_CMD; }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 217 "mon_parse.y"
    { return ERR_BAD_CMD; }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 221 "mon_parse.y"
    { mon_bank(e_default_space, NULL); }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 223 "mon_parse.y"
    { mon_bank((yyvsp[(2) - (3)].i), NULL); }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 225 "mon_parse.y"
    { mon_bank(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 227 "mon_parse.y"
    { mon_bank((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 229 "mon_parse.y"
    { mon_jump((yyvsp[(2) - (3)].a)); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 231 "mon_parse.y"
    { mon_display_io_regs(0); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 233 "mon_parse.y"
    { mon_display_io_regs((yyvsp[(2) - (3)].a)); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 235 "mon_parse.y"
    { monitor_cpu_type_set(""); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 237 "mon_parse.y"
    { monitor_cpu_type_set((yyvsp[(2) - (3)].str)); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 239 "mon_parse.y"
    { mon_cpuhistory(-1); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 241 "mon_parse.y"
    { mon_cpuhistory((yyvsp[(3) - (4)].i)); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 243 "mon_parse.y"
    { mon_instruction_return(); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 245 "mon_parse.y"
    { machine_write_snapshot((yyvsp[(2) - (3)].str),0,0,0); /* FIXME */ }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 247 "mon_parse.y"
    { machine_read_snapshot((yyvsp[(2) - (3)].str), 0); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 249 "mon_parse.y"
    { mon_instructions_step(-1); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 251 "mon_parse.y"
    { mon_instructions_step((yyvsp[(3) - (4)].i)); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 253 "mon_parse.y"
    { mon_instructions_next(-1); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 255 "mon_parse.y"
    { mon_instructions_next((yyvsp[(3) - (4)].i)); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 257 "mon_parse.y"
    { mon_stack_up(-1); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 259 "mon_parse.y"
    { mon_stack_up((yyvsp[(3) - (4)].i)); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 261 "mon_parse.y"
    { mon_stack_down(-1); }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 263 "mon_parse.y"
    { mon_stack_down((yyvsp[(3) - (4)].i)); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 265 "mon_parse.y"
    { mon_display_screen(); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 270 "mon_parse.y"
    { (monitor_cpu_for_memspace[default_memspace]->mon_register_print)(default_memspace); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 272 "mon_parse.y"
    { (monitor_cpu_for_memspace[(yyvsp[(2) - (3)].i)]->mon_register_print)((yyvsp[(2) - (3)].i)); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 277 "mon_parse.y"
    {
                        /* What about the memspace? */
                        mon_playback_init((yyvsp[(4) - (5)].str));
                    }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 282 "mon_parse.y"
    {
                        /* What about the memspace? */
                        mon_playback_init((yyvsp[(2) - (3)].str));
                    }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 287 "mon_parse.y"
    { mon_save_symbols((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 289 "mon_parse.y"
    { mon_save_symbols(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 291 "mon_parse.y"
    { mon_add_name_to_symbol_table((yyvsp[(2) - (5)].a), (yyvsp[(4) - (5)].str)); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 293 "mon_parse.y"
    { mon_remove_name_from_symbol_table(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 295 "mon_parse.y"
    { mon_remove_name_from_symbol_table((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 297 "mon_parse.y"
    { mon_print_symbol_table((yyvsp[(2) - (3)].i)); }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 299 "mon_parse.y"
    { mon_print_symbol_table(e_default_space); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 301 "mon_parse.y"
    {
                        mon_add_name_to_symbol_table((yyvsp[(3) - (4)].a), mon_prepend_dot_to_name((yyvsp[(1) - (4)].str)));
                    }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 305 "mon_parse.y"
    {
                        mon_add_name_to_symbol_table((yyvsp[(3) - (5)].a), mon_prepend_dot_to_name((yyvsp[(1) - (5)].str)));
                    }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 311 "mon_parse.y"
    { mon_start_assemble_mode((yyvsp[(2) - (2)].a), NULL); }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 312 "mon_parse.y"
    { }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 314 "mon_parse.y"
    { mon_start_assemble_mode((yyvsp[(2) - (3)].a), NULL); }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 316 "mon_parse.y"
    { mon_disassemble_lines((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1]); }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 318 "mon_parse.y"
    { mon_disassemble_lines(BAD_ADDR, BAD_ADDR); }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 322 "mon_parse.y"
    { mon_memory_move((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], (yyvsp[(4) - (5)].a)); }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 324 "mon_parse.y"
    { mon_memory_compare((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], (yyvsp[(4) - (5)].a)); }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 326 "mon_parse.y"
    { mon_memory_fill((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1],(unsigned char *)(yyvsp[(4) - (5)].str)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 328 "mon_parse.y"
    { mon_memory_hunt((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1],(unsigned char *)(yyvsp[(4) - (5)].str)); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 330 "mon_parse.y"
    { mon_memory_display((yyvsp[(2) - (5)].rt), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], DF_PETSCII); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 332 "mon_parse.y"
    { mon_memory_display(default_radix, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_PETSCII); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 334 "mon_parse.y"
    { mon_memory_display(default_radix, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 336 "mon_parse.y"
    { mon_memory_display_data((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], 8, 8); }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 338 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 8, 8); }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 340 "mon_parse.y"
    { mon_memory_display_data((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], 24, 21); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 342 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 24, 21); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 344 "mon_parse.y"
    { mon_memory_display(0, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_PETSCII); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 346 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 348 "mon_parse.y"
    { mon_memory_display(0, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_SCREEN_CODE); }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 350 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_SCREEN_CODE); }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 352 "mon_parse.y"
    { mon_memmap_zap(); }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 354 "mon_parse.y"
    { mon_memmap_show(-1,BAD_ADDR,BAD_ADDR); }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 356 "mon_parse.y"
    { mon_memmap_show((yyvsp[(3) - (4)].i),BAD_ADDR,BAD_ADDR); }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 358 "mon_parse.y"
    { mon_memmap_show((yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].range)[0],(yyvsp[(4) - (5)].range)[1]); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 360 "mon_parse.y"
    { mon_memmap_save((yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].i)); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 364 "mon_parse.y"
    {
                      if ((yyvsp[(2) - (5)].i)) {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[(3) - (5)].range)[0], (yyvsp[(3) - (5)].range)[1], TRUE, (yyvsp[(2) - (5)].i), FALSE);
                      } else {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[(3) - (5)].range)[0], (yyvsp[(3) - (5)].range)[1], TRUE, e_exec, FALSE);
                      }
                      mon_breakpoint_set_checkpoint_condition(temp, (yyvsp[(4) - (5)].cond_node));
                  }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 373 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 376 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], TRUE, e_exec, TRUE);
                  }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 380 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 383 "mon_parse.y"
    {
                      if ((yyvsp[(2) - (5)].i)) {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[(3) - (5)].range)[0], (yyvsp[(3) - (5)].range)[1], TRUE, (yyvsp[(2) - (5)].i), FALSE);
                      } else {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[(3) - (5)].range)[0], (yyvsp[(3) - (5)].range)[1], TRUE, e_load | e_store, FALSE);
                      }
                      mon_breakpoint_set_checkpoint_condition(temp, (yyvsp[(4) - (5)].cond_node));
                  }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 392 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 395 "mon_parse.y"
    {
                      if ((yyvsp[(2) - (5)].i)) {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[(3) - (5)].range)[0], (yyvsp[(3) - (5)].range)[1], FALSE, (yyvsp[(2) - (5)].i), FALSE);
                      } else {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[(3) - (5)].range)[0], (yyvsp[(3) - (5)].range)[1], FALSE, e_load | e_store, FALSE);
                      }
                      mon_breakpoint_set_checkpoint_condition(temp, (yyvsp[(4) - (5)].cond_node));
                  }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 404 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 409 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_ON, (yyvsp[(2) - (3)].i)); }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 411 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_OFF, (yyvsp[(2) - (3)].i)); }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 413 "mon_parse.y"
    { mon_breakpoint_set_ignore_count((yyvsp[(2) - (3)].i), -1); }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 415 "mon_parse.y"
    { mon_breakpoint_set_ignore_count((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].i)); }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 417 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint((yyvsp[(2) - (3)].i)); }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 419 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint(-1); }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 421 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_condition((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].cond_node)); }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 423 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_command((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 425 "mon_parse.y"
    { return ERR_EXPECT_STRING; }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 429 "mon_parse.y"
    { sidefx = (((yyvsp[(2) - (3)].action) == e_TOGGLE) ? (sidefx ^ 1) : (yyvsp[(2) - (3)].action)); }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 431 "mon_parse.y"
    {
                         mon_out("I/O side effects are %s\n",
                                   sidefx ? "enabled" : "disabled");
                     }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 436 "mon_parse.y"
    { default_radix = (yyvsp[(2) - (3)].rt); }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 438 "mon_parse.y"
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

/* Line 1455 of yacc.c  */
#line 456 "mon_parse.y"
    { monitor_change_device((yyvsp[(2) - (3)].i)); }
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 458 "mon_parse.y"
    { mon_export(); }
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 460 "mon_parse.y"
    { mon_quit(); YYACCEPT; }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 462 "mon_parse.y"
    { exit_mon = 1; YYACCEPT; }
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 466 "mon_parse.y"
    { mon_drive_execute_disk_cmd((yyvsp[(2) - (3)].str)); }
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 468 "mon_parse.y"
    { mon_out("\t%d\n",(yyvsp[(2) - (3)].i)); }
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 470 "mon_parse.y"
    { mon_command_print_help(NULL); }
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 472 "mon_parse.y"
    { mon_command_print_help((yyvsp[(2) - (3)].str)); }
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 474 "mon_parse.y"
    { printf("SYSTEM COMMAND: %s\n",(yyvsp[(2) - (3)].str)); }
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 476 "mon_parse.y"
    { mon_print_convert((yyvsp[(2) - (3)].i)); }
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 478 "mon_parse.y"
    { mon_change_dir((yyvsp[(2) - (3)].str)); }
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 480 "mon_parse.y"
    { mon_keyboard_feed((yyvsp[(2) - (3)].str)); }
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 482 "mon_parse.y"
    { mon_backtrace(); }
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 484 "mon_parse.y"
    { mon_show_dir((yyvsp[(2) - (3)].str)); }
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 486 "mon_parse.y"
    { mon_show_pwd(); }
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 488 "mon_parse.y"
    { mon_screenshot_save((yyvsp[(2) - (3)].str),-1); }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 490 "mon_parse.y"
    { mon_screenshot_save((yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].i)); }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 492 "mon_parse.y"
    { mon_resource_get((yyvsp[(2) - (3)].str)); }
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 494 "mon_parse.y"
    { mon_resource_set((yyvsp[(2) - (4)].str),(yyvsp[(3) - (4)].str)); }
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 496 "mon_parse.y"
    { mon_reset_machine(-1); }
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 498 "mon_parse.y"
    { mon_reset_machine((yyvsp[(3) - (4)].i)); }
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 500 "mon_parse.y"
    { mon_tape_ctrl((yyvsp[(3) - (4)].i)); }
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 502 "mon_parse.y"
    { mon_cart_freeze(); }
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 504 "mon_parse.y"
    { }
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 506 "mon_parse.y"
    { mon_stopwatch_reset(); }
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 508 "mon_parse.y"
    { mon_stopwatch_show("Stopwatch: ", "\n"); }
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 512 "mon_parse.y"
    { mon_file_load((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].a), FALSE); }
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 514 "mon_parse.y"
    { mon_file_load((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].a), TRUE); }
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 516 "mon_parse.y"
    { mon_file_save((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], FALSE); }
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 518 "mon_parse.y"
    { return ERR_EXPECT_DEVICE_NUM; }
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 520 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 522 "mon_parse.y"
    { mon_file_save((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], TRUE); }
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 524 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 526 "mon_parse.y"
    { mon_file_verify((yyvsp[(2) - (5)].str),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 528 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 530 "mon_parse.y"
    { mon_drive_block_cmd(0,(yyvsp[(2) - (5)].i),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 532 "mon_parse.y"
    { mon_drive_block_cmd(1,(yyvsp[(2) - (5)].i),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 534 "mon_parse.y"
    { mon_drive_list(-1); }
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 536 "mon_parse.y"
    { mon_drive_list((yyvsp[(2) - (3)].i)); }
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 538 "mon_parse.y"
    { mon_attach((yyvsp[(2) - (4)].str),(yyvsp[(3) - (4)].i)); }
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 540 "mon_parse.y"
    { mon_detach((yyvsp[(2) - (3)].i)); }
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 542 "mon_parse.y"
    { mon_autostart((yyvsp[(2) - (3)].str),0,1); }
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 544 "mon_parse.y"
    { mon_autostart((yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].i),1); }
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 546 "mon_parse.y"
    { mon_autostart((yyvsp[(2) - (3)].str),0,0); }
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 548 "mon_parse.y"
    { mon_autostart((yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].i),0); }
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 552 "mon_parse.y"
    { mon_record_commands((yyvsp[(2) - (3)].str)); }
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 554 "mon_parse.y"
    { mon_end_recording(); }
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 556 "mon_parse.y"
    { mon_playback_init((yyvsp[(2) - (3)].str)); }
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 560 "mon_parse.y"
    { mon_memory_fill((yyvsp[(2) - (4)].a), BAD_ADDR, (unsigned char *)(yyvsp[(3) - (4)].str)); }
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 562 "mon_parse.y"
    { printf("Not yet.\n"); }
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 566 "mon_parse.y"
    { yydebug = 1; }
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 569 "mon_parse.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 572 "mon_parse.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 573 "mon_parse.y"
    { (yyval.str) = NULL; }
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 577 "mon_parse.y"
    { return ERR_EXPECT_FILENAME; }
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 581 "mon_parse.y"
    { return ERR_EXPECT_DEVICE_NUM; }
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 584 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (2)].i) | (yyvsp[(2) - (2)].i); }
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 585 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 588 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 589 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 592 "mon_parse.y"
    { (yyval.i) = new_reg(default_memspace, (yyvsp[(1) - (1)].reg)); }
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 593 "mon_parse.y"
    { (yyval.i) = new_reg((yyvsp[(1) - (2)].i), (yyvsp[(2) - (2)].reg)); }
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 601 "mon_parse.y"
    { (monitor_cpu_for_memspace[reg_memspace((yyvsp[(1) - (3)].i))]->mon_register_set_val)(reg_memspace((yyvsp[(1) - (3)].i)), reg_regid((yyvsp[(1) - (3)].i)), (WORD) (yyvsp[(3) - (3)].i)); }
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 604 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 605 "mon_parse.y"
    { return ERR_EXPECT_CHECKNUM; }
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 609 "mon_parse.y"
    { (yyval.range)[0] = (yyvsp[(1) - (1)].a); (yyval.range)[1] = BAD_ADDR; }
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 612 "mon_parse.y"
    { (yyval.range)[0] = (yyvsp[(1) - (3)].a); (yyval.range)[1] = (yyvsp[(3) - (3)].a); }
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 614 "mon_parse.y"
    { if (resolve_range(e_default_space, (yyval.range), (yyvsp[(1) - (1)].str))) return ERR_ADDR_TOO_BIG; }
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 616 "mon_parse.y"
    { if (resolve_range((yyvsp[(1) - (3)].i), (yyval.range), (yyvsp[(3) - (3)].str))) return ERR_ADDR_TOO_BIG; }
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 619 "mon_parse.y"
    { (yyval.a) = (yyvsp[(2) - (2)].a); }
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 620 "mon_parse.y"
    { (yyval.a) = BAD_ADDR; }
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 624 "mon_parse.y"
    {
             (yyval.a) = new_addr(e_default_space,(yyvsp[(1) - (1)].i));
             if (opt_asm) new_cmd = asm_mode = 1;
         }
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 629 "mon_parse.y"
    {
             (yyval.a) = new_addr((yyvsp[(1) - (3)].i), (yyvsp[(3) - (3)].i));
             if (opt_asm) new_cmd = asm_mode = 1;
         }
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 634 "mon_parse.y"
    {
             temp = mon_symbol_table_lookup_addr(e_default_space, (yyvsp[(1) - (1)].str));
             if (temp >= 0)
                 (yyval.a) = new_addr(e_default_space, temp);
             else
                 return ERR_UNDEFINED_LABEL;
         }
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 647 "mon_parse.y"
    { (yyval.i) = e_comp_space; }
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 648 "mon_parse.y"
    { (yyval.i) = e_disk8_space; }
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 649 "mon_parse.y"
    { (yyval.i) = e_disk9_space; }
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 650 "mon_parse.y"
    { (yyval.i) = e_disk10_space; }
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 651 "mon_parse.y"
    { (yyval.i) = e_disk11_space; }
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 654 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); if (!CHECK_ADDR((yyvsp[(1) - (1)].i))) return ERR_ADDR_TOO_BIG; }
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 656 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 658 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) + (yyvsp[(3) - (3)].i); }
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 659 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) - (yyvsp[(3) - (3)].i); }
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 660 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) * (yyvsp[(3) - (3)].i); }
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 661 "mon_parse.y"
    { (yyval.i) = ((yyvsp[(3) - (3)].i)) ? ((yyvsp[(1) - (3)].i) / (yyvsp[(3) - (3)].i)) : 1; }
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 662 "mon_parse.y"
    { (yyval.i) = (yyvsp[(2) - (3)].i); }
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 663 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 664 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 667 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(2) - (2)].cond_node); }
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 668 "mon_parse.y"
    { (yyval.cond_node) = 0; }
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 671 "mon_parse.y"
    {
               (yyval.cond_node) = new_cond; (yyval.cond_node)->is_parenthized = FALSE;
               (yyval.cond_node)->child1 = (yyvsp[(1) - (3)].cond_node); (yyval.cond_node)->child2 = (yyvsp[(3) - (3)].cond_node); (yyval.cond_node)->operation = (yyvsp[(2) - (3)].cond_op);
           }
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 676 "mon_parse.y"
    { return ERR_INCOMPLETE_COMPARE_OP; }
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 678 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(2) - (3)].cond_node); (yyval.cond_node)->is_parenthized = TRUE; }
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 680 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 682 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(1) - (1)].cond_node); }
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 685 "mon_parse.y"
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->reg_num = (yyvsp[(1) - (1)].i); (yyval.cond_node)->is_reg = TRUE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 691 "mon_parse.y"
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->value = (yyvsp[(1) - (1)].i); (yyval.cond_node)->is_reg = FALSE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 703 "mon_parse.y"
    { mon_add_number_to_buffer((yyvsp[(1) - (1)].i)); }
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 704 "mon_parse.y"
    { mon_add_string_to_buffer((yyvsp[(1) - (1)].str)); }
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 711 "mon_parse.y"
    { mon_add_number_to_buffer((yyvsp[(1) - (1)].i)); }
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 712 "mon_parse.y"
    { mon_add_number_masked_to_buffer((yyvsp[(1) - (1)].i), 0x00); }
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 713 "mon_parse.y"
    { mon_add_string_to_buffer((yyvsp[(1) - (1)].str)); }
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 716 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 717 "mon_parse.y"
    { (yyval.i) = (monitor_cpu_for_memspace[reg_memspace((yyvsp[(1) - (1)].i))]->mon_register_get_val)(reg_memspace((yyvsp[(1) - (1)].i)), reg_regid((yyvsp[(1) - (1)].i))); }
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 720 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 721 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 722 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 723 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 726 "mon_parse.y"
    { (yyval.i) = resolve_datatype(B_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 727 "mon_parse.y"
    { (yyval.i) = resolve_datatype(O_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 728 "mon_parse.y"
    { (yyval.i) = resolve_datatype(D_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 731 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 732 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 733 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 734 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 735 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 743 "mon_parse.y"
    { (yyval.i) = 0;
                                                if ((yyvsp[(1) - (2)].str)) {
                                                    (monitor_cpu_for_memspace[default_memspace]->mon_assemble_instr)((yyvsp[(1) - (2)].str), (yyvsp[(2) - (2)].mode));
                                                } else {
                                                    new_cmd = 1;
                                                    asm_mode = 0;
                                                }
                                                opt_asm = 0;
                                              }
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 754 "mon_parse.y"
    { asm_mode = 0; }
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 757 "mon_parse.y"
    { if ((yyvsp[(2) - (2)].i) > 0xff) {
                          (yyval.mode).addr_mode = ASM_ADDR_MODE_IMMEDIATE_16;
                          (yyval.mode).param = (yyvsp[(2) - (2)].i);
                        } else {
                          (yyval.mode).addr_mode = ASM_ADDR_MODE_IMMEDIATE;
                          (yyval.mode).param = (yyvsp[(2) - (2)].i);
                        } }
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 764 "mon_parse.y"
    { if ((yyvsp[(1) - (1)].i) < 0x100) {
               (yyval.mode).addr_mode = ASM_ADDR_MODE_ZERO_PAGE;
               (yyval.mode).param = (yyvsp[(1) - (1)].i);
             } else {
               (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE;
               (yyval.mode).param = (yyvsp[(1) - (1)].i);
             }
           }
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 772 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100) {
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_ZERO_PAGE_X;
                            (yyval.mode).param = (yyvsp[(1) - (3)].i);
                          } else {
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_X;
                            (yyval.mode).param = (yyvsp[(1) - (3)].i);
                          }
                        }
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 780 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100) {
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_ZERO_PAGE_Y;
                            (yyval.mode).param = (yyvsp[(1) - (3)].i);
                          } else {
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_Y;
                            (yyval.mode).param = (yyvsp[(1) - (3)].i);
                          }
                        }
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 789 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_ABS_INDIRECT; (yyval.mode).param = (yyvsp[(2) - (3)].i); }
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 791 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_INDIRECT_X; (yyval.mode).param = (yyvsp[(2) - (5)].i); }
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 793 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_INDIRECT_Y; (yyval.mode).param = (yyvsp[(2) - (5)].i); }
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 794 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_BC; }
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 795 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_DE; }
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 796 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_HL; }
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 797 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_IX; }
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 798 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_IY; }
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 799 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_SP; }
    break;

  case 253:

/* Line 1455 of yacc.c  */
#line 801 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_A; (yyval.mode).param = (yyvsp[(2) - (5)].i); }
    break;

  case 254:

/* Line 1455 of yacc.c  */
#line 803 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_HL; (yyval.mode).param = (yyvsp[(2) - (5)].i); }
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 805 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_IX; (yyval.mode).param = (yyvsp[(2) - (5)].i); }
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 807 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_IY; (yyval.mode).param = (yyvsp[(2) - (5)].i); }
    break;

  case 257:

/* Line 1455 of yacc.c  */
#line 808 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_IMPLIED; }
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 809 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_ACCUMULATOR; }
    break;

  case 259:

/* Line 1455 of yacc.c  */
#line 810 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_B; }
    break;

  case 260:

/* Line 1455 of yacc.c  */
#line 811 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_C; }
    break;

  case 261:

/* Line 1455 of yacc.c  */
#line 812 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_D; }
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 813 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_E; }
    break;

  case 263:

/* Line 1455 of yacc.c  */
#line 814 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_H; }
    break;

  case 264:

/* Line 1455 of yacc.c  */
#line 815 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IXH; }
    break;

  case 265:

/* Line 1455 of yacc.c  */
#line 816 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IYH; }
    break;

  case 266:

/* Line 1455 of yacc.c  */
#line 817 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_L; }
    break;

  case 267:

/* Line 1455 of yacc.c  */
#line 818 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IXL; }
    break;

  case 268:

/* Line 1455 of yacc.c  */
#line 819 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IYL; }
    break;

  case 269:

/* Line 1455 of yacc.c  */
#line 820 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_AF; }
    break;

  case 270:

/* Line 1455 of yacc.c  */
#line 821 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_BC; }
    break;

  case 271:

/* Line 1455 of yacc.c  */
#line 822 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_DE; }
    break;

  case 272:

/* Line 1455 of yacc.c  */
#line 823 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_HL; }
    break;

  case 273:

/* Line 1455 of yacc.c  */
#line 824 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IX; }
    break;

  case 274:

/* Line 1455 of yacc.c  */
#line 825 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IY; }
    break;

  case 275:

/* Line 1455 of yacc.c  */
#line 826 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_SP; }
    break;

  case 276:

/* Line 1455 of yacc.c  */
#line 828 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_DIRECT; (yyval.mode).param = (yyvsp[(2) - (2)].i); }
    break;

  case 277:

/* Line 1455 of yacc.c  */
#line 829 "mon_parse.y"
    {    /* Clash with addr,x addr,y modes! */
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        if ((yyvsp[(1) - (3)].i) >= -16 && (yyvsp[(1) - (3)].i) < 16) {
            (yyval.mode).addr_submode = (yyvsp[(3) - (3)].i) | ((yyvsp[(1) - (3)].i) & 0x1F);
        } else if ((yyvsp[(1) - (3)].i) >= -128 && (yyvsp[(1) - (3)].i) < 128) {
            (yyval.mode).addr_submode = 0x80 | (yyvsp[(3) - (3)].i) | ASM_ADDR_MODE_INDEXED_OFF8;
            (yyval.mode).param = (yyvsp[(1) - (3)].i);
        } else if ((yyvsp[(1) - (3)].i) >= -32768 && (yyvsp[(1) - (3)].i) < 32768) {
            (yyval.mode).addr_submode = 0x80 | (yyvsp[(3) - (3)].i) | ASM_ADDR_MODE_INDEXED_OFF16;
            (yyval.mode).param = (yyvsp[(1) - (3)].i);
        } else {
            (yyval.mode).addr_mode = ASM_ADDR_MODE_ILLEGAL;
            mon_out("offset too large even for 16 bits (signed)\n");
        }
    }
    break;

  case 278:

/* Line 1455 of yacc.c  */
#line 844 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(2) - (3)].i) | ASM_ADDR_MODE_INDEXED_INC1;
        }
    break;

  case 279:

/* Line 1455 of yacc.c  */
#line 848 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(2) - (4)].i) | ASM_ADDR_MODE_INDEXED_INC2;
        }
    break;

  case 280:

/* Line 1455 of yacc.c  */
#line 852 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(3) - (3)].i) | ASM_ADDR_MODE_INDEXED_DEC1;
        }
    break;

  case 281:

/* Line 1455 of yacc.c  */
#line 856 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(4) - (4)].i) | ASM_ADDR_MODE_INDEXED_DEC2;
        }
    break;

  case 282:

/* Line 1455 of yacc.c  */
#line 860 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(2) - (2)].i) | ASM_ADDR_MODE_INDEXED_OFF0;
        }
    break;

  case 283:

/* Line 1455 of yacc.c  */
#line 864 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(2) - (3)].i) | ASM_ADDR_MODE_INDEXED_OFFB;
        }
    break;

  case 284:

/* Line 1455 of yacc.c  */
#line 868 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(2) - (3)].i) | ASM_ADDR_MODE_INDEXED_OFFA;
        }
    break;

  case 285:

/* Line 1455 of yacc.c  */
#line 872 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(2) - (3)].i) | ASM_ADDR_MODE_INDEXED_OFFD;
        }
    break;

  case 286:

/* Line 1455 of yacc.c  */
#line 876 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).param = (yyvsp[(1) - (3)].i);
        if ((yyvsp[(1) - (3)].i) >= -128 && (yyvsp[(1) - (3)].i) < 128) {
            (yyval.mode).addr_submode = ASM_ADDR_MODE_INDEXED_OFFPC8;
        } else if ((yyvsp[(1) - (3)].i) >= -32768 && (yyvsp[(1) - (3)].i) < 32768) {
            (yyval.mode).addr_submode = ASM_ADDR_MODE_INDEXED_OFFPC16;
        } else {
            (yyval.mode).addr_mode = ASM_ADDR_MODE_ILLEGAL;
            mon_out("offset too large even for 16 bits (signed)\n");
        }
    }
    break;

  case 287:

/* Line 1455 of yacc.c  */
#line 888 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        if ((yyvsp[(2) - (5)].i) >= -16 && (yyvsp[(2) - (5)].i) < 16) {
            (yyval.mode).addr_submode = (yyvsp[(2) - (5)].i) & 0x1F;
        } else if ((yyvsp[(1) - (5)].i) >= -128 && (yyvsp[(1) - (5)].i) < 128) {
            (yyval.mode).addr_submode = ASM_ADDR_MODE_INDEXED_OFF8;
            (yyval.mode).param = (yyvsp[(2) - (5)].i);
        } else if ((yyvsp[(2) - (5)].i) >= -32768 && (yyvsp[(2) - (5)].i) < 32768) {
            (yyval.mode).addr_submode = ASM_ADDR_MODE_INDEXED_OFF16;
            (yyval.mode).param = (yyvsp[(2) - (5)].i);
        } else {
            (yyval.mode).addr_mode = ASM_ADDR_MODE_ILLEGAL;
            mon_out("offset too large even for 16 bits (signed)\n");
        }
    }
    break;

  case 288:

/* Line 1455 of yacc.c  */
#line 903 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(3) - (5)].i) | ASM_ADDR_MODE_INDEXED_INC1;
        }
    break;

  case 289:

/* Line 1455 of yacc.c  */
#line 907 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(3) - (6)].i) | ASM_ADDR_MODE_INDEXED_INC2;
        }
    break;

  case 290:

/* Line 1455 of yacc.c  */
#line 911 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(4) - (5)].i) | ASM_ADDR_MODE_INDEXED_DEC1;
        }
    break;

  case 291:

/* Line 1455 of yacc.c  */
#line 915 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(5) - (6)].i) | ASM_ADDR_MODE_INDEXED_DEC2;
        }
    break;

  case 292:

/* Line 1455 of yacc.c  */
#line 919 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(3) - (4)].i) | ASM_ADDR_MODE_INDEXED_OFF0;
        }
    break;

  case 293:

/* Line 1455 of yacc.c  */
#line 923 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(3) - (5)].i) | ASM_ADDR_MODE_INDEXED_OFFB;
        }
    break;

  case 294:

/* Line 1455 of yacc.c  */
#line 927 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(3) - (5)].i) | ASM_ADDR_MODE_INDEXED_OFFA;
        }
    break;

  case 295:

/* Line 1455 of yacc.c  */
#line 931 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(3) - (5)].i) | ASM_ADDR_MODE_INDEXED_OFFD;
        }
    break;

  case 296:

/* Line 1455 of yacc.c  */
#line 935 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).param = (yyvsp[(2) - (5)].i);
        if ((yyvsp[(2) - (5)].i) >= -128 && (yyvsp[(2) - (5)].i) < 128) {
            (yyval.mode).addr_submode = ASM_ADDR_MODE_INDEXED_OFFPC8_IND;
        } else if ((yyvsp[(2) - (5)].i) >= -32768 && (yyvsp[(2) - (5)].i) < 32768) {
            (yyval.mode).addr_submode = ASM_ADDR_MODE_INDEXED_OFFPC16_IND;
        } else {
            (yyval.mode).addr_mode = ASM_ADDR_MODE_ILLEGAL;
            mon_out("offset too large even for 16 bits (signed)\n");
        }
    }
    break;

  case 297:

/* Line 1455 of yacc.c  */
#line 947 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | ASM_ADDR_MODE_EXTENDED_INDIRECT;
        (yyval.mode).param = (yyvsp[(2) - (3)].i);
        }
    break;

  case 298:

/* Line 1455 of yacc.c  */
#line 956 "mon_parse.y"
    { (yyval.i) = (0 << 5); printf("reg_x\n"); }
    break;

  case 299:

/* Line 1455 of yacc.c  */
#line 957 "mon_parse.y"
    { (yyval.i) = (1 << 5); printf("reg_y\n"); }
    break;

  case 300:

/* Line 1455 of yacc.c  */
#line 958 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 301:

/* Line 1455 of yacc.c  */
#line 962 "mon_parse.y"
    { (yyval.i) = (2 << 5); printf("reg_u\n"); }
    break;

  case 302:

/* Line 1455 of yacc.c  */
#line 963 "mon_parse.y"
    { (yyval.i) = (3 << 5); printf("reg_s\n"); }
    break;



/* Line 1455 of yacc.c  */
#line 4681 "mon_parse.c"
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
#line 967 "mon_parse.y"


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


