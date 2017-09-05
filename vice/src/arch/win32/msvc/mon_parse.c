/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
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
#define YYBISON_VERSION "2.4.2"

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

#if !defined(__minix_vmd) && !defined(MACOS_COMPILE) && !(defined(__OS2__) && defined(IDE_COMPILE))
#ifdef __GNUC__
#undef alloca
#ifndef ANDROID_COMPILE
#define        alloca(n)       __builtin_alloca (n)
#endif
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
#include "mon_memmap.h"
#include "mon_memory.h"
#include "mon_register.h"
#include "mon_util.h"
#include "montypes.h"
#include "resources.h"
#include "types.h"
#include "uimon.h"

#ifdef AMIGA_MORPHOS
#undef REG_PC
#endif

#define join_ints(x,y) (LO16_TO_HI16(x)|y)
#define separate_int1(x) (HI16_TO_LO16(x))
#define separate_int2(x) (LO16(x))

static int yyerror(char *s);
static int temp;
static int resolve_datatype(unsigned guess_type, const char *num);
static int resolve_range(enum t_memspace memspace, MON_ADDR range[2],
                         const char *num);

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
#define ERR_INVALID_REGISTER 16

#define BAD_ADDR (new_addr(e_invalid_space, 0))
#define CHECK_ADDR(x) ((x) == addr_mask(x))

#define YYDEBUG 1



/* Line 189 of yacc.c  */
#line 207 "mon_parse.c"

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
     CMD_QUIT = 319,
     CMD_CHDIR = 320,
     CMD_BANK = 321,
     CMD_LOAD_LABELS = 322,
     CMD_SAVE_LABELS = 323,
     CMD_ADD_LABEL = 324,
     CMD_DEL_LABEL = 325,
     CMD_SHOW_LABELS = 326,
     CMD_CLEAR_LABELS = 327,
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
     CMD_LOAD_RESOURCES = 350,
     CMD_SAVE_RESOURCES = 351,
     CMD_ATTACH = 352,
     CMD_DETACH = 353,
     CMD_MON_RESET = 354,
     CMD_TAPECTRL = 355,
     CMD_CARTFREEZE = 356,
     CMD_CPUHISTORY = 357,
     CMD_MEMMAPZAP = 358,
     CMD_MEMMAPSHOW = 359,
     CMD_MEMMAPSAVE = 360,
     CMD_COMMENT = 361,
     CMD_LIST = 362,
     CMD_STOPWATCH = 363,
     RESET = 364,
     CMD_EXPORT = 365,
     CMD_AUTOSTART = 366,
     CMD_AUTOLOAD = 367,
     CMD_LABEL_ASGN = 368,
     L_PAREN = 369,
     R_PAREN = 370,
     ARG_IMMEDIATE = 371,
     REG_A = 372,
     REG_X = 373,
     REG_Y = 374,
     COMMA = 375,
     INST_SEP = 376,
     L_BRACKET = 377,
     R_BRACKET = 378,
     LESS_THAN = 379,
     REG_U = 380,
     REG_S = 381,
     REG_PC = 382,
     REG_PCR = 383,
     REG_B = 384,
     REG_C = 385,
     REG_D = 386,
     REG_E = 387,
     REG_H = 388,
     REG_L = 389,
     REG_AF = 390,
     REG_BC = 391,
     REG_DE = 392,
     REG_HL = 393,
     REG_IX = 394,
     REG_IY = 395,
     REG_SP = 396,
     REG_IXH = 397,
     REG_IXL = 398,
     REG_IYH = 399,
     REG_IYL = 400,
     PLUS = 401,
     MINUS = 402,
     STRING = 403,
     FILENAME = 404,
     R_O_L = 405,
     OPCODE = 406,
     LABEL = 407,
     BANKNAME = 408,
     CPUTYPE = 409,
     MON_REGISTER = 410,
     COMPARE_OP = 411,
     RADIX_TYPE = 412,
     INPUT_SPEC = 413,
     CMD_CHECKPT_ON = 414,
     CMD_CHECKPT_OFF = 415,
     TOGGLE = 416,
     MASK = 417
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
#define CMD_QUIT 319
#define CMD_CHDIR 320
#define CMD_BANK 321
#define CMD_LOAD_LABELS 322
#define CMD_SAVE_LABELS 323
#define CMD_ADD_LABEL 324
#define CMD_DEL_LABEL 325
#define CMD_SHOW_LABELS 326
#define CMD_CLEAR_LABELS 327
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
#define CMD_LOAD_RESOURCES 350
#define CMD_SAVE_RESOURCES 351
#define CMD_ATTACH 352
#define CMD_DETACH 353
#define CMD_MON_RESET 354
#define CMD_TAPECTRL 355
#define CMD_CARTFREEZE 356
#define CMD_CPUHISTORY 357
#define CMD_MEMMAPZAP 358
#define CMD_MEMMAPSHOW 359
#define CMD_MEMMAPSAVE 360
#define CMD_COMMENT 361
#define CMD_LIST 362
#define CMD_STOPWATCH 363
#define RESET 364
#define CMD_EXPORT 365
#define CMD_AUTOSTART 366
#define CMD_AUTOLOAD 367
#define CMD_LABEL_ASGN 368
#define L_PAREN 369
#define R_PAREN 370
#define ARG_IMMEDIATE 371
#define REG_A 372
#define REG_X 373
#define REG_Y 374
#define COMMA 375
#define INST_SEP 376
#define L_BRACKET 377
#define R_BRACKET 378
#define LESS_THAN 379
#define REG_U 380
#define REG_S 381
#define REG_PC 382
#define REG_PCR 383
#define REG_B 384
#define REG_C 385
#define REG_D 386
#define REG_E 387
#define REG_H 388
#define REG_L 389
#define REG_AF 390
#define REG_BC 391
#define REG_DE 392
#define REG_HL 393
#define REG_IX 394
#define REG_IY 395
#define REG_SP 396
#define REG_IXH 397
#define REG_IXL 398
#define REG_IYH 399
#define REG_IYL 400
#define PLUS 401
#define MINUS 402
#define STRING 403
#define FILENAME 404
#define R_O_L 405
#define OPCODE 406
#define LABEL 407
#define BANKNAME 408
#define CPUTYPE 409
#define MON_REGISTER 410
#define COMPARE_OP 411
#define RADIX_TYPE 412
#define INPUT_SPEC 413
#define CMD_CHECKPT_ON 414
#define CMD_CHECKPT_OFF 415
#define TOGGLE 416
#define MASK 417




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 135 "mon_parse.y"

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
#line 582 "mon_parse.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 594 "mon_parse.c"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
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
#define YYFINAL  310
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1750

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  171
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  55
/* YYNRULES -- Number of rules.  */
#define YYNRULES  313
/* YYNRULES -- Number of states.  */
#define YYNSTATES  625

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   417

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     167,   168,   165,   163,     2,   164,     2,   166,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   170,     2,
       2,     2,     2,     2,   169,     2,     2,     2,     2,     2,
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
     155,   156,   157,   158,   159,   160,   161,   162
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    15,    17,    19,
      21,    23,    25,    27,    29,    31,    33,    35,    37,    39,
      41,    43,    45,    47,    50,    54,    58,    64,    68,    71,
      74,    78,    81,    85,    88,    93,    96,   100,   104,   107,
     112,   115,   120,   123,   128,   131,   136,   139,   141,   144,
     148,   152,   158,   162,   168,   172,   178,   182,   188,   192,
     195,   199,   202,   207,   213,   214,   220,   224,   228,   231,
     237,   243,   249,   255,   261,   265,   268,   272,   275,   279,
     282,   286,   289,   293,   296,   299,   302,   307,   313,   319,
     325,   328,   332,   335,   341,   344,   350,   353,   357,   360,
     364,   367,   371,   377,   381,   384,   390,   396,   401,   405,
     408,   412,   415,   419,   422,   425,   428,   432,   436,   439,
     443,   447,   451,   455,   458,   462,   465,   469,   475,   479,
     484,   488,   492,   495,   500,   505,   508,   512,   516,   519,
     525,   531,   537,   541,   546,   552,   557,   563,   568,   574,
     580,   583,   587,   592,   596,   600,   606,   610,   616,   620,
     623,   627,   632,   635,   638,   640,   642,   643,   645,   647,
     649,   651,   654,   656,   658,   659,   661,   664,   668,   670,
     674,   676,   678,   680,   682,   686,   688,   692,   695,   696,
     698,   702,   704,   706,   707,   709,   711,   713,   715,   717,
     719,   721,   725,   729,   733,   737,   741,   745,   747,   750,
     751,   755,   759,   763,   767,   769,   771,   773,   778,   782,
     784,   786,   788,   791,   793,   795,   797,   799,   801,   803,
     805,   807,   809,   811,   813,   815,   817,   819,   821,   823,
     825,   827,   831,   835,   838,   841,   843,   845,   848,   850,
     854,   858,   862,   866,   870,   876,   884,   890,   894,   898,
     902,   906,   910,   914,   920,   926,   932,   938,   939,   941,
     943,   945,   947,   949,   951,   953,   955,   957,   959,   961,
     963,   965,   967,   969,   971,   973,   975,   978,   982,   986,
     991,   995,  1000,  1003,  1007,  1011,  1015,  1019,  1025,  1031,
    1038,  1044,  1051,  1056,  1062,  1068,  1074,  1080,  1084,  1090,
    1092,  1094,  1096,  1098
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     172,     0,    -1,   173,    -1,   221,    22,    -1,    22,    -1,
     175,    -1,   173,   175,    -1,    23,    -1,    22,    -1,     1,
      -1,   176,    -1,   178,    -1,   181,    -1,   179,    -1,   182,
      -1,   183,    -1,   184,    -1,   185,    -1,   186,    -1,   187,
      -1,   188,    -1,   189,    -1,    13,    -1,    66,   174,    -1,
      66,   205,   174,    -1,    66,   153,   174,    -1,    66,   205,
     204,   153,   174,    -1,    38,   203,   174,    -1,    38,   174,
      -1,    46,   174,    -1,    46,   203,   174,    -1,    87,   174,
      -1,    87,   154,   174,    -1,   102,   174,    -1,   102,   204,
     208,   174,    -1,    26,   174,    -1,    49,   192,   174,    -1,
      50,   192,   174,    -1,    58,   174,    -1,    58,   204,   208,
     174,    -1,    57,   174,    -1,    57,   204,   208,   174,    -1,
      29,   174,    -1,    29,   204,   208,   174,    -1,    30,   174,
      -1,    30,   204,   208,   174,    -1,    85,   174,    -1,   177,
      -1,    39,   174,    -1,    39,   205,   174,    -1,    39,   197,
     174,    -1,    67,   205,   204,   192,   174,    -1,    67,   192,
     174,    -1,    68,   205,   204,   192,   174,    -1,    68,   192,
     174,    -1,    69,   203,   204,   152,   174,    -1,    70,   152,
     174,    -1,    70,   205,   204,   152,   174,    -1,    71,   205,
     174,    -1,    71,   174,    -1,    72,   205,   174,    -1,    72,
     174,    -1,   113,    21,   203,   174,    -1,   113,    21,   203,
      24,   174,    -1,    -1,    55,   203,   180,   222,   174,    -1,
      55,   203,   174,    -1,    56,   200,   174,    -1,    56,   174,
      -1,    37,   201,   204,   203,   174,    -1,    48,   201,   204,
     203,   174,    -1,    36,   201,   204,   212,   174,    -1,    35,
     201,   204,   214,   174,    -1,    43,   157,   204,   200,   174,
      -1,    43,   200,   174,    -1,    43,   174,    -1,    76,   200,
     174,    -1,    76,   174,    -1,    77,   200,   174,    -1,    77,
     174,    -1,    78,   200,   174,    -1,    78,   174,    -1,    79,
     200,   174,    -1,    79,   174,    -1,   103,   174,    -1,   104,
     174,    -1,   104,   204,   208,   174,    -1,   104,   204,   208,
     200,   174,    -1,   105,   192,   204,   208,   174,    -1,    44,
     195,   200,   209,   174,    -1,    44,   174,    -1,    86,   200,
     174,    -1,    86,   174,    -1,    62,   195,   200,   209,   174,
      -1,    62,   174,    -1,    45,   195,   200,   209,   174,    -1,
      45,   174,    -1,   159,   199,   174,    -1,   159,   174,    -1,
     160,   199,   174,    -1,   160,   174,    -1,    34,   199,   174,
      -1,    34,   199,   204,   208,   174,    -1,    52,   199,   174,
      -1,    52,   174,    -1,    53,   199,    15,   210,   174,    -1,
      54,   199,   204,   148,   174,    -1,    54,   199,     1,   174,
      -1,    25,   161,   174,    -1,    25,   174,    -1,    42,   157,
     174,    -1,    42,   174,    -1,    60,   205,   174,    -1,   110,
     174,    -1,    64,   174,    -1,    51,   174,    -1,    63,   190,
     174,    -1,    59,   208,   174,    -1,    61,   174,    -1,    61,
     190,   174,    -1,     7,   208,   174,    -1,    65,   190,   174,
      -1,    82,   190,   174,    -1,    89,   174,    -1,    92,   191,
     174,    -1,    91,   174,    -1,    90,   192,   174,    -1,    90,
     192,   204,   208,   174,    -1,    93,   148,   174,    -1,    94,
     148,   148,   174,    -1,    95,   192,   174,    -1,    96,   192,
     174,    -1,    99,   174,    -1,    99,   204,   208,   174,    -1,
     100,   204,   208,   174,    -1,   101,   174,    -1,   106,   191,
     174,    -1,   108,   109,   174,    -1,   108,   174,    -1,    31,
     192,   193,   202,   174,    -1,    83,   192,   193,   202,   174,
      -1,    32,   192,   193,   201,   174,    -1,    32,   192,     1,
      -1,    32,   192,   193,     1,    -1,    84,   192,   193,   201,
     174,    -1,    84,   192,   193,     1,    -1,    33,   192,   193,
     203,   174,    -1,    33,   192,   193,     1,    -1,    27,   208,
     208,   202,   174,    -1,    28,   208,   208,   203,   174,    -1,
     107,   174,    -1,   107,   193,   174,    -1,    97,   192,   208,
     174,    -1,    98,   208,   174,    -1,   111,   192,   174,    -1,
     111,   192,   204,   219,   174,    -1,   112,   192,   174,    -1,
     112,   192,   204,   219,   174,    -1,    73,   192,   174,    -1,
      74,   174,    -1,    75,   192,   174,    -1,    80,   203,   212,
     174,    -1,    81,   174,    -1,    88,   174,    -1,   150,    -1,
     150,    -1,    -1,   149,    -1,     1,    -1,   208,    -1,     1,
      -1,   194,    14,    -1,    14,    -1,   194,    -1,    -1,   155,
      -1,   205,   155,    -1,   197,   120,   198,    -1,   198,    -1,
     196,    21,   219,    -1,   217,    -1,     1,    -1,   201,    -1,
     203,    -1,   203,   204,   203,    -1,     9,    -1,   205,   204,
       9,    -1,   204,   203,    -1,    -1,   206,    -1,   205,   204,
     206,    -1,   152,    -1,   120,    -1,    -1,    16,    -1,    17,
      -1,    18,    -1,    19,    -1,    20,    -1,   207,    -1,   219,
      -1,   208,   163,   208,    -1,   208,   164,   208,    -1,   208,
     165,   208,    -1,   208,   166,   208,    -1,   167,   208,   168,
      -1,   167,   208,     1,    -1,   216,    -1,    15,   210,    -1,
      -1,   210,   156,   210,    -1,   210,   156,     1,    -1,   114,
     210,   115,    -1,   114,   210,     1,    -1,   211,    -1,   196,
      -1,   219,    -1,   169,   153,   170,   203,    -1,   212,   204,
     213,    -1,   213,    -1,   219,    -1,   148,    -1,   214,   215,
      -1,   215,    -1,   219,    -1,   162,    -1,   148,    -1,   219,
      -1,   196,    -1,     4,    -1,    12,    -1,    11,    -1,    10,
      -1,    12,    -1,    11,    -1,    10,    -1,     3,    -1,     4,
      -1,     5,    -1,     6,    -1,   218,    -1,   220,   121,   221,
      -1,   221,   121,   221,    -1,   221,   121,    -1,   151,   223,
      -1,   221,    -1,   220,    -1,   116,   219,    -1,   219,    -1,
     219,   120,   118,    -1,   219,   120,   119,    -1,   219,   120,
     126,    -1,   219,   120,   219,    -1,   114,   219,   115,    -1,
     114,   219,   120,   118,   115,    -1,   114,   219,   120,   126,
     115,   120,   119,    -1,   114,   219,   115,   120,   119,    -1,
     114,   136,   115,    -1,   114,   137,   115,    -1,   114,   138,
     115,    -1,   114,   139,   115,    -1,   114,   140,   115,    -1,
     114,   141,   115,    -1,   114,   219,   115,   120,   117,    -1,
     114,   219,   115,   120,   138,    -1,   114,   219,   115,   120,
     139,    -1,   114,   219,   115,   120,   140,    -1,    -1,   117,
      -1,   129,    -1,   130,    -1,   131,    -1,   132,    -1,   133,
      -1,   142,    -1,   144,    -1,   134,    -1,   143,    -1,   145,
      -1,   135,    -1,   136,    -1,   137,    -1,   138,    -1,   139,
      -1,   140,    -1,   141,    -1,   124,   219,    -1,   219,   120,
     225,    -1,   120,   224,   146,    -1,   120,   224,   146,   146,
      -1,   120,   147,   224,    -1,   120,   147,   147,   224,    -1,
     120,   224,    -1,   129,   120,   224,    -1,   117,   120,   224,
      -1,   131,   120,   224,    -1,   219,   120,   127,    -1,   122,
     219,   120,   224,   123,    -1,   122,   120,   224,   146,   123,
      -1,   122,   120,   224,   146,   146,   123,    -1,   122,   120,
     147,   224,   123,    -1,   122,   120,   147,   147,   224,   123,
      -1,   122,   120,   224,   123,    -1,   122,   129,   120,   224,
     123,    -1,   122,   117,   120,   224,   123,    -1,   122,   131,
     120,   224,   123,    -1,   122,   219,   120,   127,   123,    -1,
     122,   219,   123,    -1,   122,   219,   123,   120,   119,    -1,
     118,    -1,   119,    -1,   225,    -1,   126,    -1,   125,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   199,   199,   200,   201,   204,   205,   208,   209,   210,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   228,   230,   232,   234,   236,   238,   240,
     242,   244,   246,   248,   250,   252,   254,   256,   258,   260,
     262,   264,   266,   268,   270,   272,   274,   276,   279,   281,
     283,   286,   291,   296,   298,   300,   302,   304,   306,   308,
     310,   312,   314,   318,   325,   324,   327,   329,   331,   335,
     337,   339,   341,   343,   345,   347,   349,   351,   353,   355,
     357,   359,   361,   363,   365,   367,   369,   371,   373,   377,
     386,   389,   393,   396,   405,   408,   417,   422,   424,   426,
     428,   430,   432,   434,   436,   438,   440,   442,   446,   448,
     453,   455,   473,   475,   477,   479,   483,   485,   487,   489,
     491,   493,   495,   497,   499,   501,   503,   505,   507,   509,
     511,   513,   515,   517,   519,   521,   523,   525,   527,   531,
     533,   535,   537,   539,   541,   543,   545,   547,   549,   551,
     553,   555,   557,   559,   561,   563,   565,   567,   571,   573,
     575,   579,   581,   585,   589,   592,   593,   596,   597,   600,
     601,   604,   605,   608,   609,   612,   618,   626,   627,   630,
     634,   635,   638,   639,   642,   643,   645,   649,   650,   653,
     658,   663,   673,   674,   677,   678,   679,   680,   681,   684,
     686,   688,   689,   690,   691,   692,   693,   694,   697,   698,
     700,   705,   707,   709,   711,   715,   721,   727,   735,   736,
     739,   740,   743,   744,   747,   748,   749,   752,   753,   756,
     757,   758,   759,   762,   763,   764,   767,   768,   769,   770,
     771,   774,   775,   776,   779,   789,   790,   793,   800,   811,
     822,   830,   849,   855,   863,   871,   873,   875,   876,   877,
     878,   879,   880,   881,   883,   885,   887,   889,   890,   891,
     892,   893,   894,   895,   896,   897,   898,   899,   900,   901,
     902,   903,   904,   905,   906,   907,   909,   910,   925,   929,
     933,   937,   941,   945,   949,   953,   957,   969,   984,   988,
     992,   996,  1000,  1004,  1008,  1012,  1016,  1028,  1033,  1041,
    1042,  1043,  1044,  1048
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
  "CMD_DEVICE", "CMD_HELP", "CMD_WATCH", "CMD_DISK", "CMD_QUIT",
  "CMD_CHDIR", "CMD_BANK", "CMD_LOAD_LABELS", "CMD_SAVE_LABELS",
  "CMD_ADD_LABEL", "CMD_DEL_LABEL", "CMD_SHOW_LABELS", "CMD_CLEAR_LABELS",
  "CMD_RECORD", "CMD_MON_STOP", "CMD_PLAYBACK", "CMD_CHAR_DISPLAY",
  "CMD_SPRITE_DISPLAY", "CMD_TEXT_DISPLAY", "CMD_SCREENCODE_DISPLAY",
  "CMD_ENTER_DATA", "CMD_ENTER_BIN_DATA", "CMD_KEYBUF", "CMD_BLOAD",
  "CMD_BSAVE", "CMD_SCREEN", "CMD_UNTIL", "CMD_CPU", "CMD_YYDEBUG",
  "CMD_BACKTRACE", "CMD_SCREENSHOT", "CMD_PWD", "CMD_DIR",
  "CMD_RESOURCE_GET", "CMD_RESOURCE_SET", "CMD_LOAD_RESOURCES",
  "CMD_SAVE_RESOURCES", "CMD_ATTACH", "CMD_DETACH", "CMD_MON_RESET",
  "CMD_TAPECTRL", "CMD_CARTFREEZE", "CMD_CPUHISTORY", "CMD_MEMMAPZAP",
  "CMD_MEMMAPSHOW", "CMD_MEMMAPSAVE", "CMD_COMMENT", "CMD_LIST",
  "CMD_STOPWATCH", "RESET", "CMD_EXPORT", "CMD_AUTOSTART", "CMD_AUTOLOAD",
  "CMD_LABEL_ASGN", "L_PAREN", "R_PAREN", "ARG_IMMEDIATE", "REG_A",
  "REG_X", "REG_Y", "COMMA", "INST_SEP", "L_BRACKET", "R_BRACKET",
  "LESS_THAN", "REG_U", "REG_S", "REG_PC", "REG_PCR", "REG_B", "REG_C",
  "REG_D", "REG_E", "REG_H", "REG_L", "REG_AF", "REG_BC", "REG_DE",
  "REG_HL", "REG_IX", "REG_IY", "REG_SP", "REG_IXH", "REG_IXL", "REG_IYH",
  "REG_IYL", "PLUS", "MINUS", "STRING", "FILENAME", "R_O_L", "OPCODE",
  "LABEL", "BANKNAME", "CPUTYPE", "MON_REGISTER", "COMPARE_OP",
  "RADIX_TYPE", "INPUT_SPEC", "CMD_CHECKPT_ON", "CMD_CHECKPT_OFF",
  "TOGGLE", "MASK", "'+'", "'-'", "'*'", "'/'", "'('", "')'", "'@'", "':'",
  "$accept", "top_level", "command_list", "end_cmd", "command",
  "machine_state_rules", "register_mod", "symbol_table_rules", "asm_rules",
  "$@1", "memory_rules", "checkpoint_rules", "checkpoint_control_rules",
  "monitor_state_rules", "monitor_misc_rules", "disk_rules",
  "cmd_file_rules", "data_entry_rules", "monitor_debug_rules",
  "rest_of_line", "opt_rest_of_line", "filename", "device_num", "mem_op",
  "opt_mem_op", "register", "reg_list", "reg_asgn", "checkpt_num",
  "address_opt_range", "address_range", "opt_address", "address",
  "opt_sep", "memspace", "memloc", "memaddr", "expression",
  "opt_if_cond_expr", "cond_expr", "compare_operand", "data_list",
  "data_element", "hunt_list", "hunt_element", "value", "d_number",
  "guess_default", "number", "assembly_instr_list", "assembly_instruction",
  "post_assemble", "asm_operand_mode", "index_reg", "index_ureg", 0
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
     415,   416,   417,    43,    45,    42,    47,    40,    41,    64,
      58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   171,   172,   172,   172,   173,   173,   174,   174,   174,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   177,   177,
     177,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   180,   179,   179,   179,   179,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   182,
     182,   182,   182,   182,   182,   182,   182,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   184,   184,
     184,   184,   184,   184,   184,   184,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   187,   187,
     187,   188,   188,   189,   190,   191,   191,   192,   192,   193,
     193,   194,   194,   195,   195,   196,   196,   197,   197,   198,
     199,   199,   200,   200,   201,   201,   201,   202,   202,   203,
     203,   203,   204,   204,   205,   205,   205,   205,   205,   206,
     207,   208,   208,   208,   208,   208,   208,   208,   209,   209,
     210,   210,   210,   210,   210,   211,   211,   211,   212,   212,
     213,   213,   214,   214,   215,   215,   215,   216,   216,   217,
     217,   217,   217,   218,   218,   218,   219,   219,   219,   219,
     219,   220,   220,   220,   221,   222,   222,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   224,
     224,   224,   224,   225
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     3,     5,     3,     2,     2,
       3,     2,     3,     2,     4,     2,     3,     3,     2,     4,
       2,     4,     2,     4,     2,     4,     2,     1,     2,     3,
       3,     5,     3,     5,     3,     5,     3,     5,     3,     2,
       3,     2,     4,     5,     0,     5,     3,     3,     2,     5,
       5,     5,     5,     5,     3,     2,     3,     2,     3,     2,
       3,     2,     3,     2,     2,     2,     4,     5,     5,     5,
       2,     3,     2,     5,     2,     5,     2,     3,     2,     3,
       2,     3,     5,     3,     2,     5,     5,     4,     3,     2,
       3,     2,     3,     2,     2,     2,     3,     3,     2,     3,
       3,     3,     3,     2,     3,     2,     3,     5,     3,     4,
       3,     3,     2,     4,     4,     2,     3,     3,     2,     5,
       5,     5,     3,     4,     5,     4,     5,     4,     5,     5,
       2,     3,     4,     3,     3,     5,     3,     5,     3,     2,
       3,     4,     2,     2,     1,     1,     0,     1,     1,     1,
       1,     2,     1,     1,     0,     1,     2,     3,     1,     3,
       1,     1,     1,     1,     3,     1,     3,     2,     0,     1,
       3,     1,     1,     0,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     1,     2,     0,
       3,     3,     3,     3,     1,     1,     1,     4,     3,     1,
       1,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     2,     2,     1,     1,     2,     1,     3,
       3,     3,     3,     3,     5,     7,     5,     3,     3,     3,
       3,     3,     3,     5,     5,     5,     5,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     3,     3,     4,
       3,     4,     2,     3,     3,     3,     3,     5,     5,     6,
       5,     6,     4,     5,     5,     5,     5,     3,     5,     1,
       1,     1,     1,     1
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
       0,     0,     0,     0,     0,     0,     0,     0,   166,     0,
       0,     0,     0,     0,     0,     0,   193,     0,     0,     0,
       0,     0,   166,     0,     0,     0,     0,     0,     0,   267,
       0,     0,     0,     2,     5,    10,    47,    11,    13,    12,
      14,    15,    16,    17,    18,    19,    20,    21,     0,   236,
     237,   238,   239,   235,   234,   233,   194,   195,   196,   197,
     198,   175,     0,   228,     0,     0,   207,   240,   227,     9,
       8,     7,     0,   109,    35,     0,     0,   192,    42,     0,
      44,     0,   168,   167,     0,     0,     0,   181,   229,   232,
     231,   230,     0,   180,   185,   191,   193,   193,   193,   189,
     199,   200,   193,   193,    28,     0,   193,    48,     0,     0,
     178,     0,     0,   111,   193,    75,     0,   182,   193,   172,
      90,   173,     0,    96,     0,    29,     0,   193,     0,     0,
     115,     9,   104,     0,     0,     0,     0,    68,     0,    40,
       0,    38,     0,     0,     0,   164,   118,     0,    94,     0,
       0,   114,     0,     0,    23,     0,     0,   193,     0,   193,
     193,     0,   193,    59,     0,    61,     0,     0,   159,     0,
      77,     0,    79,     0,    81,     0,    83,     0,     0,   162,
       0,     0,     0,    46,    92,     0,     0,    31,   163,   123,
       0,   125,   165,     0,     0,     0,     0,     0,     0,     0,
     132,     0,     0,   135,    33,     0,    84,    85,     0,   193,
       0,     9,   150,     0,   169,     0,   138,   113,     0,     0,
       0,     0,     0,   268,     0,     0,     0,   269,   270,   271,
     272,   273,   276,   279,   280,   281,   282,   283,   284,   285,
     274,   277,   275,   278,   248,   244,    98,     0,   100,     0,
       1,     6,     3,     0,   176,     0,     0,     0,     0,   120,
     108,   193,     0,     0,     0,   170,   193,   142,     0,     0,
     101,     0,     0,     0,     0,     0,     0,    27,     0,     0,
       0,    50,    49,   110,     0,    74,   171,   209,   209,    30,
       0,    36,    37,   103,     0,     0,     0,    66,     0,    67,
       0,     0,   117,   112,   119,   209,   116,   121,    25,    24,
       0,    52,     0,    54,     0,     0,    56,     0,    58,    60,
     158,   160,    76,    78,    80,    82,   221,     0,   219,   220,
     122,   193,     0,    91,    32,   126,     0,   124,   128,     0,
     130,   131,     0,   153,     0,     0,     0,     0,     0,   136,
     151,   137,   154,     0,   156,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   247,     0,   309,   310,   313,   312,
       0,   292,   311,     0,     0,     0,     0,     0,   286,     0,
       0,     0,    97,    99,   206,   205,   201,   202,   203,   204,
       0,     0,     0,    43,    45,     0,   143,     0,   147,     0,
       0,   226,   225,     0,   223,   224,   184,   186,   190,     0,
       0,   179,   177,     0,     0,     0,     0,     0,     0,     0,
     215,     0,   214,   216,   107,     0,   246,   245,     0,    41,
      39,     0,     0,     0,     0,     0,     0,   161,     0,     0,
     145,     0,     0,   129,   152,   133,   134,    34,    86,     0,
       0,     0,     0,     0,    62,   257,   258,   259,   260,   261,
     262,   253,     0,   294,     0,   290,   288,     0,     0,     0,
       0,     0,     0,   307,   293,   295,   249,   250,   251,   296,
     252,   287,   148,   187,   149,   139,   141,   146,   102,    72,
     222,    71,    69,    73,   208,    89,    95,    70,     0,     0,
       0,   105,   106,     0,   243,    65,    93,    26,    51,    53,
      55,    57,   218,   140,   144,   127,    87,    88,   155,   157,
      63,     0,     0,     0,   291,   289,     0,     0,     0,   302,
       0,     0,     0,     0,     0,     0,   213,   212,     0,   211,
     210,   241,   242,   263,   256,   264,   265,   266,   254,     0,
     304,     0,   300,   298,     0,   303,   305,   306,   297,   308,
     217,     0,   301,   299,   255
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    92,    93,   133,    94,    95,    96,    97,    98,   358,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   207,
     253,   144,   273,   181,   182,   123,   169,   170,   152,   176,
     177,   450,   178,   451,   124,   159,   160,   274,   475,   481,
     482,   387,   388,   463,   464,   126,   153,   127,   161,   486,
     108,   488,   305,   431,   432
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -452
static const yytype_int16 yypact[] =
{
    1215,   871,  -452,  -452,    14,    90,   871,   871,   491,   491,
      15,    15,    15,   665,  1529,  1529,  1529,  1329,   257,    88,
    1081,  1172,  1172,  1329,  1529,    15,    15,    90,  1128,   665,
     665,  1547,  1198,   491,   491,   871,   822,   102,  1172,  -143,
      90,  -143,   365,   450,   450,  1547,   643,   702,   702,    15,
      90,    15,  1198,  1198,  1198,  1198,  1547,    90,  -143,    15,
      15,    90,  1198,    92,    90,    90,    15,    90,   -86,   -64,
     -43,    15,    15,    15,   871,   491,   -69,    90,   491,    90,
     491,    15,   -86,   188,   323,    90,    15,    15,    86,  1571,
    1128,  1128,   118,  1351,  -452,  -452,  -452,  -452,  -452,  -452,
    -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,    97,  -452,
    -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,
    -452,  -452,   871,  -452,   -18,    98,  -452,  -452,  -452,  -452,
    -452,  -452,    90,  -452,  -452,   947,   947,  -452,  -452,   871,
    -452,   871,  -452,  -452,   237,   677,   237,  -452,  -452,  -452,
    -452,  -452,   491,  -452,  -452,  -452,   -69,   -69,   -69,  -452,
    -452,  -452,   -69,   -69,  -452,    90,   -69,  -452,   124,   266,
    -452,   195,    90,  -452,   -69,  -452,    90,  -452,   433,  -452,
    -452,   138,  1529,  -452,  1529,  -452,    90,   -69,    90,    90,
    -452,   264,  -452,    90,   142,    19,    65,  -452,    90,  -452,
     871,  -452,   871,    98,    90,  -452,  -452,    90,  -452,  1529,
      90,  -452,    90,    90,  -452,   280,    90,   -69,    90,   -69,
     -69,    90,   -69,  -452,    90,  -452,    90,    90,  -452,    90,
    -452,    90,  -452,    90,  -452,    90,  -452,    90,   848,  -452,
      90,   237,   237,  -452,  -452,    90,    90,  -452,  -452,  -452,
     491,  -452,  -452,    90,    90,    23,    90,    90,   871,    98,
    -452,   871,   871,  -452,  -452,   871,  -452,  -452,   871,   -69,
      90,   312,  -452,    90,   535,    90,  -452,  -452,  1114,  1114,
    1547,  1581,  1142,    56,   192,  1609,  1142,    59,  -452,    68,
    -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,
    -452,  -452,  -452,  -452,    70,  -452,  -452,    90,  -452,    90,
    -452,  -452,  -452,    18,  -452,   871,   871,   871,   871,  -452,
    -452,    58,  1061,    98,    98,  -452,   282,   518,  1466,  1511,
    -452,   871,    67,  1547,   965,   848,  1547,  -452,  1142,  1142,
     455,  -452,  -452,  -452,  1529,  -452,  -452,   162,   162,  -452,
    1547,  -452,  -452,  -452,   817,    90,    54,  -452,    52,  -452,
      98,    98,  -452,  -452,  -452,   162,  -452,  -452,  -452,  -452,
      61,  -452,    15,  -452,    15,    73,  -452,    76,  -452,  -452,
    -452,  -452,  -452,  -452,  -452,  -452,  -452,   631,  -452,  -452,
    -452,   282,  1486,  -452,  -452,  -452,   871,  -452,  -452,    90,
    -452,  -452,    98,  -452,    98,    98,    98,   893,   871,  -452,
    -452,  -452,  -452,  1142,  -452,  1142,   347,   116,   119,   135,
     136,   144,   145,    12,  -452,   334,  -452,  -452,  -452,  -452,
     249,    87,  -452,   148,   503,   149,   152,    46,  -452,   334,
     334,  1623,  -452,  -452,  -452,  -452,   -71,   -71,  -452,  -452,
      90,  1547,    90,  -452,  -452,    90,  -452,    90,  -452,    90,
      98,  -452,  -452,   150,  -452,  -452,  -452,  -452,  -452,   631,
      90,  -452,  -452,    90,   817,    90,    90,    90,   817,   125,
    -452,    53,  -452,  -452,  -452,    90,   161,   163,    90,  -452,
    -452,    90,    90,    90,    90,    90,    90,  -452,   848,    90,
    -452,    90,    98,  -452,  -452,  -452,  -452,  -452,  -452,    90,
      98,    90,    90,    90,  -452,  -452,  -452,  -452,  -452,  -452,
    -452,   170,   -22,  -452,   334,  -452,    93,   334,   588,   -99,
     334,   334,   753,   174,  -452,  -452,  -452,  -452,  -452,  -452,
    -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,
    -452,  -452,  -452,  -452,   160,  -452,  -452,  -452,     7,   151,
     130,  -452,  -452,    52,    52,  -452,  -452,  -452,  -452,  -452,
    -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,
    -452,   324,   185,   205,  -452,  -452,   200,   334,   203,  -452,
     -97,   208,   221,   231,   241,   209,  -452,  -452,  1547,  -452,
    -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,   240,
    -452,   267,  -452,  -452,   278,  -452,  -452,  -452,  -452,  -452,
    -452,   284,  -452,  -452,  -452
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -452,  -452,  -452,   540,   318,  -452,  -452,  -452,  -452,  -452,
    -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,  -452,   322,
     331,    57,   -83,  -452,    64,   -13,  -452,    75,   460,     3,
      -3,  -301,    29,   262,   -14,  -137,  -452,   667,  -298,  -451,
    -452,    81,   -93,  -452,   -46,  -452,  -452,  -452,    91,  -452,
    -344,  -452,  -452,  -133,    -6
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -194
static const yytype_int16 yytable[] =
{
     158,   158,   158,   166,   171,   168,   158,   205,   596,   166,
     158,   156,   162,   163,   487,   129,   142,   166,   158,   444,
     355,   187,   204,   554,   589,   455,   613,   558,   215,   217,
     219,   166,   222,   224,   226,   198,   130,   131,   158,   158,
     158,   158,   166,   157,   157,   157,   165,   590,   158,   614,
     476,   137,   186,   157,   129,   231,   233,   235,   237,  -188,
     196,   326,   328,   329,   252,   245,   129,   491,   145,   146,
     109,   110,   111,   112,   220,   130,   131,   113,   114,   115,
    -188,  -188,   188,   189,   254,   238,   184,   130,   131,   129,
     499,   129,   128,   129,   317,   318,   582,   128,   128,   129,
     216,   218,   209,   129,   583,   255,   227,   280,   229,   600,
     130,   131,   130,   131,   130,   131,   241,   242,   310,   312,
     130,   131,   597,   250,   130,   131,   128,   521,   256,   257,
     258,   599,   522,   109,   110,   111,   112,   314,   269,   137,
     113,   114,   115,   278,   279,   339,   116,   117,   118,   119,
     120,   129,   346,   109,   110,   111,   112,   354,   391,   392,
     113,   114,   115,   560,   143,   128,   532,  -193,   158,   533,
     158,   399,   130,   131,   128,   132,   425,   474,   137,   439,
     304,   315,   316,   317,   318,   347,   445,   348,   440,   271,
     441,   109,   110,   111,   112,   158,   129,   468,   113,   114,
     115,   468,   485,    89,   116,   117,   118,   119,   120,   560,
     130,   131,   365,   128,   492,   461,   -64,   130,   131,   601,
     602,   315,   316,   317,   318,   495,   128,   128,   496,   462,
     128,   515,   128,   526,   516,   128,   128,   128,   325,   585,
     109,   110,   111,   112,   478,   172,   246,   113,   114,   115,
     517,   518,   205,   116,   117,   118,   119,   120,   129,   519,
     520,   315,   316,   317,   318,  -181,   166,   129,   527,   530,
     139,   141,   531,   116,   117,   118,   119,   120,   559,   130,
     131,   129,   563,  -188,   564,   121,  -181,  -181,   130,   131,
     581,   128,   523,   128,   595,   200,   202,   525,   461,   479,
     608,   529,   130,   131,  -188,  -188,   534,   535,   166,   416,
     426,   427,   462,  -170,   158,   166,   560,   428,   429,   166,
     609,   598,   166,   610,   129,   457,   612,   168,   619,   389,
     158,   615,   128,   128,  -170,  -170,   166,   261,   262,   430,
     265,   480,   268,   121,   616,   130,   131,   473,   129,   128,
     314,   452,   128,   128,   617,   122,   128,   157,   459,   128,
     621,   210,   466,   212,   618,   470,   129,   426,   427,   130,
     131,   513,   423,   424,   428,   429,   437,   438,   158,   477,
     240,   116,   117,   118,   119,   120,   340,   130,   131,   501,
     622,   584,   121,   158,   586,   588,   524,   591,   592,   594,
     137,   623,   137,   624,   122,   572,   128,   128,   128,   128,
     509,   311,   121,   270,   331,   472,   469,   550,   332,   333,
     334,   157,   128,   465,   335,   336,   389,     0,   338,   493,
     471,   494,   275,  -193,  -183,   541,   344,   166,     0,     0,
     333,   603,     0,   604,     0,   483,     0,     0,  -183,   350,
       0,   142,   426,   427,   611,  -183,  -183,   356,     0,   428,
     429,   480,   605,   606,   607,   480,   116,   117,   118,   119,
     120,   116,   117,   118,   119,   120,     0,   370,     0,   372,
     543,   374,   375,     0,   377,     0,     0,   128,   193,   194,
     195,     0,   129,     0,  -193,  -193,  -193,  -193,     0,   128,
       0,  -193,  -193,  -193,   511,     0,   512,  -193,  -193,  -193,
    -193,  -193,   396,   130,   131,     0,     0,     0,   213,  -170,
       0,  -170,  -170,  -170,  -170,     0,     0,  -170,  -170,  -170,
    -170,   408,   540,     0,  -170,  -170,  -170,  -170,  -170,     0,
     413,   415,     0,     0,     0,   134,     0,   480,   138,   140,
     307,   309,     0,   137,   465,     0,     0,   164,   167,   173,
     175,   180,   183,   185,     0,   483,     0,   190,   192,   483,
       0,     0,   197,   199,   201,     0,     0,   206,   208,     0,
     211,     0,   214,     0,   166,     0,     0,   223,   225,   389,
     228,     0,   230,   232,   234,   236,     0,   239,     0,   143,
       0,   243,   244,   247,   248,   249,     0,   251,     0,     0,
     121,   137,     0,     0,     0,   260,     0,   263,   264,   266,
     267,   426,   427,   272,   276,   277,     0,   620,   428,   429,
     306,   308,   129,     0,  -193,  -193,  -193,  -193,     0,     0,
       0,  -193,  -193,  -193,     0,     0,  -193,     0,     0,   498,
     528,   483,     0,   130,   131,     0,     0,     0,  -193,   116,
     117,   118,   119,   120,     0,   319,   147,     0,   125,   148,
    -170,     0,   320,   135,   136,   149,   150,   151,   327,     0,
     109,   110,   111,   112,     0,     0,     0,   113,   114,   115,
       0,     0,   330,   116,   117,   118,   119,   120,   315,   316,
     317,   318,   203,   129,     0,   337,   426,   427,     0,   341,
       0,   342,   343,   428,   429,     0,   345,     0,   116,   117,
     118,   119,   120,     0,   130,   131,   349,     0,   351,   352,
       0,   498,     0,   353,     0,   587,   357,     0,   359,     0,
       0,   259,     0,   362,   363,     0,     0,   364,     0,     0,
     366,   137,   367,   368,     0,   369,   371,     0,   373,     0,
       0,   376,     0,     0,   378,     0,   379,   380,     0,   381,
       0,   382,     0,   383,     0,   384,     0,   385,     0,  -193,
     390,     0,     0,     0,     0,   393,   394,     0,     0,   313,
     395,     0,     0,   397,   398,   221,   400,   401,     0,   403,
       0,     0,   321,   322,     0,     0,   323,     0,   324,     0,
     409,     0,     0,   410,     0,   411,     0,     0,   412,   414,
     109,   110,   111,   112,     0,     0,     0,   113,   114,   115,
       0,     0,   121,   116,   117,   118,   119,   120,   116,   117,
     118,   119,   120,     0,   122,     0,     0,   442,     0,   443,
       0,   109,   110,   111,   112,     0,     0,     0,   113,   114,
     115,     0,     0,   453,   454,     0,     0,   360,     0,   361,
       0,   426,   427,     0,   109,   110,   111,   112,   428,   429,
     593,   113,   114,   115,     0,     0,     0,   116,   117,   118,
     119,   120,     0,     0,   129,   484,   109,   110,   111,   112,
     489,   490,   154,   113,   114,   115,     0,     0,     0,   116,
     117,   118,   119,   120,     0,   130,   131,     0,     0,     0,
       0,     0,     0,     0,     0,   402,     0,   497,   404,   405,
       0,   478,   406,     0,     0,   407,     0,     0,     0,   503,
       0,     0,   504,     0,   505,   506,   507,   508,     0,     0,
     109,   110,   111,   112,     0,     0,   514,   113,   114,   115,
       0,     0,     0,   116,   117,   118,   119,   120,   109,   110,
     111,   112,   121,     0,   467,   113,   114,   115,     0,     0,
       0,     0,   446,   447,   448,   449,   479,     0,     0,     0,
     542,     0,   544,     0,     0,   545,   386,   546,   460,   547,
     548,     0,     0,   549,     0,     0,     0,     0,     0,   551,
     552,     0,     0,   553,     0,   555,   556,   557,     0,     0,
       0,   561,     0,     0,     0,   562,   121,     0,   565,     0,
       0,   566,   567,   568,   569,   570,   571,     0,   122,   573,
       0,   574,   575,     0,     0,   155,     0,     0,     0,   576,
     577,   578,   579,   580,     0,     0,   315,   316,   317,   318,
       0,     0,     0,   502,   109,   110,   111,   112,     0,     0,
       0,   113,   114,   115,     0,   510,     0,   116,   117,   118,
     119,   120,   129,     0,   109,   110,   111,   112,     0,     0,
     154,   113,   114,   115,     0,     0,     0,   116,   117,   118,
     119,   120,   121,   130,   131,     0,     0,     0,     0,     0,
     315,   316,   317,   318,   122,   129,     0,  -193,  -193,  -193,
    -193,     0,     0,     0,  -193,  -193,  -193,     0,     0,   191,
       0,     0,   148,     0,     0,     0,   130,   131,   149,   150,
     151,     0,     0,     0,     0,   109,   110,   111,   112,     0,
     130,   131,   113,   114,   115,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   129,     0,  -174,  -174,  -174,  -174,     0,
       0,  -174,  -174,  -174,  -174,     0,   179,     0,  -174,  -174,
    -174,  -174,  -174,     0,   130,   131,     0,     0,     0,   129,
       0,   109,   110,   111,   112,     0,     0,   154,   113,   114,
     115,     0,     0,   155,   116,   117,   118,   119,   120,     0,
     130,   131,     1,     0,   315,   316,   317,   318,     2,     0,
       0,     0,     0,   155,   137,     0,     0,     3,   174,     0,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,     0,     0,    19,    20,    21,
      22,    23,     0,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,  -174,    85,    86,    87,    88,     0,
     129,     0,   109,   110,   111,   112,     0,     0,     0,   113,
     114,   115,     0,     0,     0,   116,   117,   118,   119,   120,
     155,   130,   131,     0,     0,     0,     0,     0,     1,     0,
       0,     0,     0,     0,     2,     0,    89,     0,     0,     0,
       0,     0,     0,     0,    90,    91,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,     0,     0,    19,    20,    21,    22,    23,     0,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
       0,    85,    86,    87,    88,     0,     0,   456,     0,   109,
     110,   111,   112,     0,     0,   154,   113,   114,   115,     0,
       0,   155,   116,   117,   118,   119,   120,   500,     0,   109,
     110,   111,   112,     0,     0,   154,   113,   114,   115,     0,
       0,     0,   116,   117,   118,   119,   120,     0,     0,     0,
      90,    91,   458,     0,   109,   110,   111,   112,     0,     0,
       0,   113,   114,   115,     0,     0,     0,   116,   117,   118,
     119,   120,   109,   110,   111,   112,     0,     0,   154,   113,
     114,   115,     0,     0,     0,   116,   117,   118,   119,   120,
     109,   110,   111,   112,     0,     0,     0,   113,   114,   115,
       0,     0,     0,   116,   117,   118,   119,   120,     0,     0,
       0,     0,     0,     0,   109,   110,   111,   112,     0,     0,
       0,   113,   114,   115,   109,   110,   111,   112,     0,     0,
       0,   113,   114,   115,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   109,   110,   111,   112,     0,     0,   155,   113,
     114,   115,     0,     0,     0,     0,   109,   110,   111,   112,
       0,     0,     0,   113,   114,   115,     0,     0,   155,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   155,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   155,     0,     0,     0,   281,     0,   282,   283,     0,
       0,   284,     0,   285,     0,   286,     0,     0,     0,   155,
     287,   288,   289,   290,   291,   292,   293,   294,   295,   296,
     297,   298,   299,   300,   301,   302,   303,   417,   418,   419,
     420,   421,   422,     0,     0,     0,   433,     0,     0,   434,
       0,     0,     0,     0,     0,     0,     0,     0,   435,     0,
     436,   536,   537,     0,     0,     0,     0,     0,   428,   538,
     539
};

static const yytype_int16 yycheck[] =
{
      14,    15,    16,    17,    18,    18,    20,   150,     1,    23,
      24,    14,    15,    16,   358,     1,     1,    31,    32,     1,
       1,    24,    36,   474,   123,   326,   123,   478,    42,    43,
      44,    45,    46,    47,    48,    32,    22,    23,    52,    53,
      54,    55,    56,    14,    15,    16,    17,   146,    62,   146,
     348,   120,    23,    24,     1,    52,    53,    54,    55,     1,
      31,   144,   145,   146,   150,    62,     1,   365,    11,    12,
       3,     4,     5,     6,    45,    22,    23,    10,    11,    12,
      22,    23,    25,    26,   148,    56,    22,    22,    23,     1,
     391,     1,     1,     1,   165,   166,   118,     6,     7,     1,
      43,    44,    38,     1,   126,   148,    49,    21,    51,   560,
      22,    23,    22,    23,    22,    23,    59,    60,     0,    22,
      22,    23,   115,    66,    22,    23,    35,   115,    71,    72,
      73,     1,   120,     3,     4,     5,     6,   155,    81,   120,
      10,    11,    12,    86,    87,    21,    16,    17,    18,    19,
      20,     1,    14,     3,     4,     5,     6,    15,   241,   242,
      10,    11,    12,   156,   149,    74,   120,   148,   182,   123,
     184,   148,    22,    23,    83,   161,   120,    15,   120,   120,
      89,   163,   164,   165,   166,   182,   168,   184,   120,     1,
     120,     3,     4,     5,     6,   209,     1,   334,    10,    11,
      12,   338,   148,   151,    16,    17,    18,    19,    20,   156,
      22,    23,   209,   122,   153,   148,   151,    22,    23,   563,
     564,   163,   164,   165,   166,   152,   135,   136,   152,   162,
     139,   115,   141,   146,   115,   144,   145,   146,     1,   146,
       3,     4,     5,     6,   114,   157,   154,    10,    11,    12,
     115,   115,   150,    16,    17,    18,    19,    20,     1,   115,
     115,   163,   164,   165,   166,     1,   280,     1,   120,   120,
       8,     9,   120,    16,    17,    18,    19,    20,   153,    22,
      23,     1,   121,     1,   121,   155,    22,    23,    22,    23,
     120,   200,   425,   202,   120,    33,    34,   430,   148,   169,
     115,   434,    22,    23,    22,    23,   439,   440,   322,   280,
     118,   119,   162,     1,   328,   329,   156,   125,   126,   333,
     115,   170,   336,   123,     1,   328,   123,   340,   119,   238,
     344,   123,   241,   242,    22,    23,   350,    75,    76,   147,
      78,   354,    80,   155,   123,    22,    23,   344,     1,   258,
     155,   322,   261,   262,   123,   167,   265,   328,   329,   268,
     120,    39,   333,    41,   123,   336,     1,   118,   119,    22,
      23,    24,   281,   282,   125,   126,   285,   286,   392,   350,
      58,    16,    17,    18,    19,    20,   120,    22,    23,   392,
     123,   524,   155,   407,   527,   528,   147,   530,   531,   532,
     120,   123,   120,   119,   167,   498,   315,   316,   317,   318,
     407,    93,   155,    82,   152,   340,   335,   463,   156,   157,
     158,   392,   331,   332,   162,   163,   335,    -1,   166,   372,
     339,   374,   109,   153,     1,   441,   174,   451,    -1,    -1,
     178,   117,    -1,   119,    -1,   354,    -1,    -1,    15,   187,
      -1,     1,   118,   119,   587,    22,    23,   195,    -1,   125,
     126,   474,   138,   139,   140,   478,    16,    17,    18,    19,
      20,    16,    17,    18,    19,    20,    -1,   215,    -1,   217,
     451,   219,   220,    -1,   222,    -1,    -1,   396,    28,    29,
      30,    -1,     1,    -1,     3,     4,     5,     6,    -1,   408,
      -1,    10,    11,    12,   413,    -1,   415,    16,    17,    18,
      19,    20,   250,    22,    23,    -1,    -1,    -1,   153,     1,
      -1,     3,     4,     5,     6,    -1,    -1,     9,    10,    11,
      12,   269,   441,    -1,    16,    17,    18,    19,    20,    -1,
     278,   279,    -1,    -1,    -1,     5,    -1,   560,     8,     9,
      90,    91,    -1,   120,   463,    -1,    -1,    17,    18,    19,
      20,    21,    22,    23,    -1,   474,    -1,    27,    28,   478,
      -1,    -1,    32,    33,    34,    -1,    -1,    37,    38,    -1,
      40,    -1,    42,    -1,   598,    -1,    -1,    47,    48,   498,
      50,    -1,    52,    53,    54,    55,    -1,    57,    -1,   149,
      -1,    61,    62,    63,    64,    65,    -1,    67,    -1,    -1,
     155,   120,    -1,    -1,    -1,    75,    -1,    77,    78,    79,
      80,   118,   119,    83,    84,    85,    -1,   598,   125,   126,
      90,    91,     1,    -1,     3,     4,     5,     6,    -1,    -1,
      -1,    10,    11,    12,    -1,    -1,   155,    -1,    -1,   387,
     147,   560,    -1,    22,    23,    -1,    -1,    -1,   167,    16,
      17,    18,    19,    20,    -1,   125,     1,    -1,     1,     4,
     152,    -1,   132,     6,     7,    10,    11,    12,     1,    -1,
       3,     4,     5,     6,    -1,    -1,    -1,    10,    11,    12,
      -1,    -1,   152,    16,    17,    18,    19,    20,   163,   164,
     165,   166,    35,     1,    -1,   165,   118,   119,    -1,   169,
      -1,   171,   172,   125,   126,    -1,   176,    -1,    16,    17,
      18,    19,    20,    -1,    22,    23,   186,    -1,   188,   189,
      -1,   469,    -1,   193,    -1,   147,   196,    -1,   198,    -1,
      -1,    74,    -1,   203,   204,    -1,    -1,   207,    -1,    -1,
     210,   120,   212,   213,    -1,   215,   216,    -1,   218,    -1,
      -1,   221,    -1,    -1,   224,    -1,   226,   227,    -1,   229,
      -1,   231,    -1,   233,    -1,   235,    -1,   237,    -1,   148,
     240,    -1,    -1,    -1,    -1,   245,   246,    -1,    -1,   122,
     250,    -1,    -1,   253,   254,   152,   256,   257,    -1,   259,
      -1,    -1,   135,   136,    -1,    -1,   139,    -1,   141,    -1,
     270,    -1,    -1,   273,    -1,   275,    -1,    -1,   278,   279,
       3,     4,     5,     6,    -1,    -1,    -1,    10,    11,    12,
      -1,    -1,   155,    16,    17,    18,    19,    20,    16,    17,
      18,    19,    20,    -1,   167,    -1,    -1,   307,    -1,   309,
      -1,     3,     4,     5,     6,    -1,    -1,    -1,    10,    11,
      12,    -1,    -1,   323,   324,    -1,    -1,   200,    -1,   202,
      -1,   118,   119,    -1,     3,     4,     5,     6,   125,   126,
     127,    10,    11,    12,    -1,    -1,    -1,    16,    17,    18,
      19,    20,    -1,    -1,     1,   355,     3,     4,     5,     6,
     360,   361,     9,    10,    11,    12,    -1,    -1,    -1,    16,
      17,    18,    19,    20,    -1,    22,    23,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   258,    -1,   387,   261,   262,
      -1,   114,   265,    -1,    -1,   268,    -1,    -1,    -1,   399,
      -1,    -1,   402,    -1,   404,   405,   406,   407,    -1,    -1,
       3,     4,     5,     6,    -1,    -1,   416,    10,    11,    12,
      -1,    -1,    -1,    16,    17,    18,    19,    20,     3,     4,
       5,     6,   155,    -1,     9,    10,    11,    12,    -1,    -1,
      -1,    -1,   315,   316,   317,   318,   169,    -1,    -1,    -1,
     450,    -1,   452,    -1,    -1,   455,   148,   457,   331,   459,
     460,    -1,    -1,   463,    -1,    -1,    -1,    -1,    -1,   469,
     470,    -1,    -1,   473,    -1,   475,   476,   477,    -1,    -1,
      -1,   481,    -1,    -1,    -1,   485,   155,    -1,   488,    -1,
      -1,   491,   492,   493,   494,   495,   496,    -1,   167,   499,
      -1,   501,   502,    -1,    -1,   152,    -1,    -1,    -1,   509,
     510,   511,   512,   513,    -1,    -1,   163,   164,   165,   166,
      -1,    -1,    -1,   396,     3,     4,     5,     6,    -1,    -1,
      -1,    10,    11,    12,    -1,   408,    -1,    16,    17,    18,
      19,    20,     1,    -1,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    12,    -1,    -1,    -1,    16,    17,    18,
      19,    20,   155,    22,    23,    -1,    -1,    -1,    -1,    -1,
     163,   164,   165,   166,   167,     1,    -1,     3,     4,     5,
       6,    -1,    -1,    -1,    10,    11,    12,    -1,    -1,     1,
      -1,    -1,     4,    -1,    -1,    -1,    22,    23,    10,    11,
      12,    -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,
      22,    23,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,    -1,
      -1,     9,    10,    11,    12,    -1,    14,    -1,    16,    17,
      18,    19,    20,    -1,    22,    23,    -1,    -1,    -1,     1,
      -1,     3,     4,     5,     6,    -1,    -1,     9,    10,    11,
      12,    -1,    -1,   152,    16,    17,    18,    19,    20,    -1,
      22,    23,     7,    -1,   163,   164,   165,   166,    13,    -1,
      -1,    -1,    -1,   152,   120,    -1,    -1,    22,   157,    -1,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    -1,    -1,    42,    43,    44,
      45,    46,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   152,   110,   111,   112,   113,    -1,
       1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,    10,
      11,    12,    -1,    -1,    -1,    16,    17,    18,    19,    20,
     152,    22,    23,    -1,    -1,    -1,    -1,    -1,     7,    -1,
      -1,    -1,    -1,    -1,    13,    -1,   151,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   159,   160,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    -1,    -1,    42,    43,    44,    45,    46,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
      -1,   110,   111,   112,   113,    -1,    -1,     1,    -1,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    12,    -1,
      -1,   152,    16,    17,    18,    19,    20,     1,    -1,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    12,    -1,
      -1,    -1,    16,    17,    18,    19,    20,    -1,    -1,    -1,
     159,   160,     1,    -1,     3,     4,     5,     6,    -1,    -1,
      -1,    10,    11,    12,    -1,    -1,    -1,    16,    17,    18,
      19,    20,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    -1,    -1,    -1,    16,    17,    18,    19,    20,
       3,     4,     5,     6,    -1,    -1,    -1,    10,    11,    12,
      -1,    -1,    -1,    16,    17,    18,    19,    20,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,    -1,
      -1,    10,    11,    12,     3,     4,     5,     6,    -1,    -1,
      -1,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,    -1,    -1,   152,    10,
      11,    12,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    -1,    -1,    10,    11,    12,    -1,    -1,   152,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   152,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   152,    -1,    -1,    -1,   114,    -1,   116,   117,    -1,
      -1,   120,    -1,   122,    -1,   124,    -1,    -1,    -1,   152,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   136,   137,   138,
     139,   140,   141,    -1,    -1,    -1,   117,    -1,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   129,    -1,
     131,   118,   119,    -1,    -1,    -1,    -1,    -1,   125,   126,
     127
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
     104,   105,   106,   107,   108,   110,   111,   112,   113,   151,
     159,   160,   172,   173,   175,   176,   177,   178,   179,   181,
     182,   183,   184,   185,   186,   187,   188,   189,   221,     3,
       4,     5,     6,    10,    11,    12,    16,    17,    18,    19,
      20,   155,   167,   196,   205,   208,   216,   218,   219,     1,
      22,    23,   161,   174,   174,   208,   208,   120,   174,   204,
     174,   204,     1,   149,   192,   192,   192,     1,     4,    10,
      11,    12,   199,   217,     9,   152,   201,   203,   205,   206,
     207,   219,   201,   201,   174,   203,   205,   174,   196,   197,
     198,   205,   157,   174,   157,   174,   200,   201,   203,    14,
     174,   194,   195,   174,   195,   174,   203,   201,   192,   192,
     174,     1,   174,   199,   199,   199,   203,   174,   200,   174,
     204,   174,   204,   208,   205,   150,   174,   190,   174,   195,
     190,   174,   190,   153,   174,   205,   192,   205,   192,   205,
     203,   152,   205,   174,   205,   174,   205,   192,   174,   192,
     174,   200,   174,   200,   174,   200,   174,   200,   203,   174,
     190,   192,   192,   174,   174,   200,   154,   174,   174,   174,
     192,   174,   150,   191,   148,   148,   192,   192,   192,   208,
     174,   204,   204,   174,   174,   204,   174,   174,   204,   192,
     191,     1,   174,   193,   208,   109,   174,   174,   192,   192,
      21,   114,   116,   117,   120,   122,   124,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   219,   223,   174,   199,   174,   199,
       0,   175,    22,   208,   155,   163,   164,   165,   166,   174,
     174,   208,   208,   208,   208,     1,   193,     1,   193,   193,
     174,   204,   204,   204,   204,   204,   204,   174,   204,    21,
     120,   174,   174,   174,   204,   174,    14,   200,   200,   174,
     204,   174,   174,   174,    15,     1,   204,   174,   180,   174,
     208,   208,   174,   174,   174,   200,   174,   174,   174,   174,
     204,   174,   204,   174,   204,   204,   174,   204,   174,   174,
     174,   174,   174,   174,   174,   174,   148,   212,   213,   219,
     174,   193,   193,   174,   174,   174,   204,   174,   174,   148,
     174,   174,   208,   174,   208,   208,   208,   208,   204,   174,
     174,   174,   174,   204,   174,   204,   203,   136,   137,   138,
     139,   140,   141,   219,   219,   120,   118,   119,   125,   126,
     147,   224,   225,   117,   120,   129,   131,   219,   219,   120,
     120,   120,   174,   174,     1,   168,   208,   208,   208,   208,
     202,   204,   203,   174,   174,   202,     1,   201,     1,   203,
     208,   148,   162,   214,   215,   219,   203,     9,   206,   212,
     203,   219,   198,   200,    15,   209,   209,   203,   114,   169,
     196,   210,   211,   219,   174,   148,   220,   221,   222,   174,
     174,   209,   153,   192,   192,   152,   152,   174,   204,   202,
       1,   201,   208,   174,   174,   174,   174,   174,   174,   200,
     208,   219,   219,    24,   174,   115,   115,   115,   115,   115,
     115,   115,   120,   224,   147,   224,   146,   120,   147,   224,
     120,   120,   120,   123,   224,   224,   118,   119,   126,   127,
     219,   225,   174,   203,   174,   174,   174,   174,   174,   174,
     215,   174,   174,   174,   210,   174,   174,   174,   210,   153,
     156,   174,   174,   121,   121,   174,   174,   174,   174,   174,
     174,   174,   213,   174,   174,   174,   174,   174,   174,   174,
     174,   120,   118,   126,   224,   146,   224,   147,   224,   123,
     146,   224,   224,   127,   224,   120,     1,   115,   170,     1,
     210,   221,   221,   117,   119,   138,   139,   140,   115,   115,
     123,   224,   123,   123,   146,   123,   123,   123,   123,   119,
     203,   120,   123,   123,   119
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
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

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
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
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

/* Line 1464 of yacc.c  */
#line 199 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 3:

/* Line 1464 of yacc.c  */
#line 200 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 4:

/* Line 1464 of yacc.c  */
#line 201 "mon_parse.y"
    { new_cmd = 1; asm_mode = 0;  (yyval.i) = 0; }
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 210 "mon_parse.y"
    { return ERR_EXPECT_END_CMD; }
    break;

  case 22:

/* Line 1464 of yacc.c  */
#line 225 "mon_parse.y"
    { return ERR_BAD_CMD; }
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 229 "mon_parse.y"
    { mon_bank(e_default_space, NULL); }
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 231 "mon_parse.y"
    { mon_bank((yyvsp[(2) - (3)].i), NULL); }
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 233 "mon_parse.y"
    { mon_bank(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 235 "mon_parse.y"
    { mon_bank((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 237 "mon_parse.y"
    { mon_jump((yyvsp[(2) - (3)].a)); }
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 239 "mon_parse.y"
    { mon_go(); }
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 241 "mon_parse.y"
    { mon_display_io_regs(0); }
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 243 "mon_parse.y"
    { mon_display_io_regs((yyvsp[(2) - (3)].a)); }
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 245 "mon_parse.y"
    { monitor_cpu_type_set(""); }
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 247 "mon_parse.y"
    { monitor_cpu_type_set((yyvsp[(2) - (3)].str)); }
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 249 "mon_parse.y"
    { mon_cpuhistory(-1); }
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 251 "mon_parse.y"
    { mon_cpuhistory((yyvsp[(3) - (4)].i)); }
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 253 "mon_parse.y"
    { mon_instruction_return(); }
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 255 "mon_parse.y"
    { machine_write_snapshot((yyvsp[(2) - (3)].str),0,0,0); /* FIXME */ }
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 257 "mon_parse.y"
    { machine_read_snapshot((yyvsp[(2) - (3)].str), 0); }
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 259 "mon_parse.y"
    { mon_instructions_step(-1); }
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 261 "mon_parse.y"
    { mon_instructions_step((yyvsp[(3) - (4)].i)); }
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 263 "mon_parse.y"
    { mon_instructions_next(-1); }
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 265 "mon_parse.y"
    { mon_instructions_next((yyvsp[(3) - (4)].i)); }
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 267 "mon_parse.y"
    { mon_stack_up(-1); }
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 269 "mon_parse.y"
    { mon_stack_up((yyvsp[(3) - (4)].i)); }
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 271 "mon_parse.y"
    { mon_stack_down(-1); }
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 273 "mon_parse.y"
    { mon_stack_down((yyvsp[(3) - (4)].i)); }
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 275 "mon_parse.y"
    { mon_display_screen(); }
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 280 "mon_parse.y"
    { (monitor_cpu_for_memspace[default_memspace]->mon_register_print)(default_memspace); }
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 282 "mon_parse.y"
    { (monitor_cpu_for_memspace[(yyvsp[(2) - (3)].i)]->mon_register_print)((yyvsp[(2) - (3)].i)); }
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 287 "mon_parse.y"
    {
                        /* What about the memspace? */
                        mon_playback_init((yyvsp[(4) - (5)].str));
                    }
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 292 "mon_parse.y"
    {
                        /* What about the memspace? */
                        mon_playback_init((yyvsp[(2) - (3)].str));
                    }
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 297 "mon_parse.y"
    { mon_save_symbols((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 299 "mon_parse.y"
    { mon_save_symbols(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 301 "mon_parse.y"
    { mon_add_name_to_symbol_table((yyvsp[(2) - (5)].a), (yyvsp[(4) - (5)].str)); }
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 303 "mon_parse.y"
    { mon_remove_name_from_symbol_table(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 305 "mon_parse.y"
    { mon_remove_name_from_symbol_table((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 307 "mon_parse.y"
    { mon_print_symbol_table((yyvsp[(2) - (3)].i)); }
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 309 "mon_parse.y"
    { mon_print_symbol_table(e_default_space); }
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 311 "mon_parse.y"
    { mon_clear_symbol_table((yyvsp[(2) - (3)].i)); }
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 313 "mon_parse.y"
    { mon_clear_symbol_table(e_default_space); }
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 315 "mon_parse.y"
    {
                        mon_add_name_to_symbol_table((yyvsp[(3) - (4)].a), mon_prepend_dot_to_name((yyvsp[(1) - (4)].str)));
                    }
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 319 "mon_parse.y"
    {
                        mon_add_name_to_symbol_table((yyvsp[(3) - (5)].a), mon_prepend_dot_to_name((yyvsp[(1) - (5)].str)));
                    }
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 325 "mon_parse.y"
    { mon_start_assemble_mode((yyvsp[(2) - (2)].a), NULL); }
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 326 "mon_parse.y"
    { }
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 328 "mon_parse.y"
    { mon_start_assemble_mode((yyvsp[(2) - (3)].a), NULL); }
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 330 "mon_parse.y"
    { mon_disassemble_lines((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1]); }
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 332 "mon_parse.y"
    { mon_disassemble_lines(BAD_ADDR, BAD_ADDR); }
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 336 "mon_parse.y"
    { mon_memory_move((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], (yyvsp[(4) - (5)].a)); }
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 338 "mon_parse.y"
    { mon_memory_compare((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], (yyvsp[(4) - (5)].a)); }
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 340 "mon_parse.y"
    { mon_memory_fill((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1],(unsigned char *)(yyvsp[(4) - (5)].str)); }
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 342 "mon_parse.y"
    { mon_memory_hunt((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1],(unsigned char *)(yyvsp[(4) - (5)].str)); }
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 344 "mon_parse.y"
    { mon_memory_display((yyvsp[(2) - (5)].rt), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], DF_PETSCII); }
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 346 "mon_parse.y"
    { mon_memory_display(default_radix, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_PETSCII); }
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 348 "mon_parse.y"
    { mon_memory_display(default_radix, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 350 "mon_parse.y"
    { mon_memory_display_data((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], 8, 8); }
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 352 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 8, 8); }
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 354 "mon_parse.y"
    { mon_memory_display_data((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], 24, 21); }
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 356 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 24, 21); }
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 358 "mon_parse.y"
    { mon_memory_display(0, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_PETSCII); }
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 360 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 362 "mon_parse.y"
    { mon_memory_display(0, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_SCREEN_CODE); }
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 364 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_SCREEN_CODE); }
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 366 "mon_parse.y"
    { mon_memmap_zap(); }
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 368 "mon_parse.y"
    { mon_memmap_show(-1,BAD_ADDR,BAD_ADDR); }
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 370 "mon_parse.y"
    { mon_memmap_show((yyvsp[(3) - (4)].i),BAD_ADDR,BAD_ADDR); }
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 372 "mon_parse.y"
    { mon_memmap_show((yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].range)[0],(yyvsp[(4) - (5)].range)[1]); }
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 374 "mon_parse.y"
    { mon_memmap_save((yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].i)); }
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 378 "mon_parse.y"
    {
                      if ((yyvsp[(2) - (5)].i)) {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[(3) - (5)].range)[0], (yyvsp[(3) - (5)].range)[1], TRUE, (yyvsp[(2) - (5)].i), FALSE);
                      } else {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[(3) - (5)].range)[0], (yyvsp[(3) - (5)].range)[1], TRUE, e_exec, FALSE);
                      }
                      mon_breakpoint_set_checkpoint_condition(temp, (yyvsp[(4) - (5)].cond_node));
                  }
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 387 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 390 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], TRUE, e_exec, TRUE);
                  }
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 394 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 397 "mon_parse.y"
    {
                      if ((yyvsp[(2) - (5)].i)) {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[(3) - (5)].range)[0], (yyvsp[(3) - (5)].range)[1], TRUE, (yyvsp[(2) - (5)].i), FALSE);
                      } else {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[(3) - (5)].range)[0], (yyvsp[(3) - (5)].range)[1], TRUE, e_load | e_store, FALSE);
                      }
                      mon_breakpoint_set_checkpoint_condition(temp, (yyvsp[(4) - (5)].cond_node));
                  }
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 406 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 409 "mon_parse.y"
    {
                      if ((yyvsp[(2) - (5)].i)) {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[(3) - (5)].range)[0], (yyvsp[(3) - (5)].range)[1], FALSE, (yyvsp[(2) - (5)].i), FALSE);
                      } else {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[(3) - (5)].range)[0], (yyvsp[(3) - (5)].range)[1], FALSE, e_load | e_store, FALSE);
                      }
                      mon_breakpoint_set_checkpoint_condition(temp, (yyvsp[(4) - (5)].cond_node));
                  }
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 418 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 423 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_ON, (yyvsp[(2) - (3)].i)); }
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 425 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_ON, -1); }
    break;

  case 99:

/* Line 1464 of yacc.c  */
#line 427 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_OFF, (yyvsp[(2) - (3)].i)); }
    break;

  case 100:

/* Line 1464 of yacc.c  */
#line 429 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_OFF, -1); }
    break;

  case 101:

/* Line 1464 of yacc.c  */
#line 431 "mon_parse.y"
    { mon_breakpoint_set_ignore_count((yyvsp[(2) - (3)].i), -1); }
    break;

  case 102:

/* Line 1464 of yacc.c  */
#line 433 "mon_parse.y"
    { mon_breakpoint_set_ignore_count((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].i)); }
    break;

  case 103:

/* Line 1464 of yacc.c  */
#line 435 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint((yyvsp[(2) - (3)].i)); }
    break;

  case 104:

/* Line 1464 of yacc.c  */
#line 437 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint(-1); }
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 439 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_condition((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].cond_node)); }
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 441 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_command((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 443 "mon_parse.y"
    { return ERR_EXPECT_STRING; }
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 447 "mon_parse.y"
    { sidefx = (((yyvsp[(2) - (3)].action) == e_TOGGLE) ? (sidefx ^ 1) : (yyvsp[(2) - (3)].action)); }
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 449 "mon_parse.y"
    {
                         mon_out("I/O side effects are %s\n",
                                   sidefx ? "enabled" : "disabled");
                     }
    break;

  case 110:

/* Line 1464 of yacc.c  */
#line 454 "mon_parse.y"
    { default_radix = (yyvsp[(2) - (3)].rt); }
    break;

  case 111:

/* Line 1464 of yacc.c  */
#line 456 "mon_parse.y"
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

  case 112:

/* Line 1464 of yacc.c  */
#line 474 "mon_parse.y"
    { monitor_change_device((yyvsp[(2) - (3)].i)); }
    break;

  case 113:

/* Line 1464 of yacc.c  */
#line 476 "mon_parse.y"
    { mon_export(); }
    break;

  case 114:

/* Line 1464 of yacc.c  */
#line 478 "mon_parse.y"
    { mon_quit(); YYACCEPT; }
    break;

  case 115:

/* Line 1464 of yacc.c  */
#line 480 "mon_parse.y"
    { mon_exit(); YYACCEPT; }
    break;

  case 116:

/* Line 1464 of yacc.c  */
#line 484 "mon_parse.y"
    { mon_drive_execute_disk_cmd((yyvsp[(2) - (3)].str)); }
    break;

  case 117:

/* Line 1464 of yacc.c  */
#line 486 "mon_parse.y"
    { mon_out("\t%d\n",(yyvsp[(2) - (3)].i)); }
    break;

  case 118:

/* Line 1464 of yacc.c  */
#line 488 "mon_parse.y"
    { mon_command_print_help(NULL); }
    break;

  case 119:

/* Line 1464 of yacc.c  */
#line 490 "mon_parse.y"
    { mon_command_print_help((yyvsp[(2) - (3)].str)); }
    break;

  case 120:

/* Line 1464 of yacc.c  */
#line 492 "mon_parse.y"
    { mon_print_convert((yyvsp[(2) - (3)].i)); }
    break;

  case 121:

/* Line 1464 of yacc.c  */
#line 494 "mon_parse.y"
    { mon_change_dir((yyvsp[(2) - (3)].str)); }
    break;

  case 122:

/* Line 1464 of yacc.c  */
#line 496 "mon_parse.y"
    { mon_keyboard_feed((yyvsp[(2) - (3)].str)); }
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 498 "mon_parse.y"
    { mon_backtrace(); }
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 500 "mon_parse.y"
    { mon_show_dir((yyvsp[(2) - (3)].str)); }
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 502 "mon_parse.y"
    { mon_show_pwd(); }
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 504 "mon_parse.y"
    { mon_screenshot_save((yyvsp[(2) - (3)].str),-1); }
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 506 "mon_parse.y"
    { mon_screenshot_save((yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].i)); }
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 508 "mon_parse.y"
    { mon_resource_get((yyvsp[(2) - (3)].str)); }
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 510 "mon_parse.y"
    { mon_resource_set((yyvsp[(2) - (4)].str),(yyvsp[(3) - (4)].str)); }
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 512 "mon_parse.y"
    { resources_load((yyvsp[(2) - (3)].str)); }
    break;

  case 131:

/* Line 1464 of yacc.c  */
#line 514 "mon_parse.y"
    { resources_save((yyvsp[(2) - (3)].str)); }
    break;

  case 132:

/* Line 1464 of yacc.c  */
#line 516 "mon_parse.y"
    { mon_reset_machine(-1); }
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 518 "mon_parse.y"
    { mon_reset_machine((yyvsp[(3) - (4)].i)); }
    break;

  case 134:

/* Line 1464 of yacc.c  */
#line 520 "mon_parse.y"
    { mon_tape_ctrl((yyvsp[(3) - (4)].i)); }
    break;

  case 135:

/* Line 1464 of yacc.c  */
#line 522 "mon_parse.y"
    { mon_cart_freeze(); }
    break;

  case 136:

/* Line 1464 of yacc.c  */
#line 524 "mon_parse.y"
    { }
    break;

  case 137:

/* Line 1464 of yacc.c  */
#line 526 "mon_parse.y"
    { mon_stopwatch_reset(); }
    break;

  case 138:

/* Line 1464 of yacc.c  */
#line 528 "mon_parse.y"
    { mon_stopwatch_show("Stopwatch: ", "\n"); }
    break;

  case 139:

/* Line 1464 of yacc.c  */
#line 532 "mon_parse.y"
    { mon_file_load((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].a), FALSE); }
    break;

  case 140:

/* Line 1464 of yacc.c  */
#line 534 "mon_parse.y"
    { mon_file_load((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].a), TRUE); }
    break;

  case 141:

/* Line 1464 of yacc.c  */
#line 536 "mon_parse.y"
    { mon_file_save((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], FALSE); }
    break;

  case 142:

/* Line 1464 of yacc.c  */
#line 538 "mon_parse.y"
    { return ERR_EXPECT_DEVICE_NUM; }
    break;

  case 143:

/* Line 1464 of yacc.c  */
#line 540 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 144:

/* Line 1464 of yacc.c  */
#line 542 "mon_parse.y"
    { mon_file_save((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], TRUE); }
    break;

  case 145:

/* Line 1464 of yacc.c  */
#line 544 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 146:

/* Line 1464 of yacc.c  */
#line 546 "mon_parse.y"
    { mon_file_verify((yyvsp[(2) - (5)].str),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 147:

/* Line 1464 of yacc.c  */
#line 548 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 550 "mon_parse.y"
    { mon_drive_block_cmd(0,(yyvsp[(2) - (5)].i),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 552 "mon_parse.y"
    { mon_drive_block_cmd(1,(yyvsp[(2) - (5)].i),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 554 "mon_parse.y"
    { mon_drive_list(-1); }
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 556 "mon_parse.y"
    { mon_drive_list((yyvsp[(2) - (3)].i)); }
    break;

  case 152:

/* Line 1464 of yacc.c  */
#line 558 "mon_parse.y"
    { mon_attach((yyvsp[(2) - (4)].str),(yyvsp[(3) - (4)].i)); }
    break;

  case 153:

/* Line 1464 of yacc.c  */
#line 560 "mon_parse.y"
    { mon_detach((yyvsp[(2) - (3)].i)); }
    break;

  case 154:

/* Line 1464 of yacc.c  */
#line 562 "mon_parse.y"
    { mon_autostart((yyvsp[(2) - (3)].str),0,1); }
    break;

  case 155:

/* Line 1464 of yacc.c  */
#line 564 "mon_parse.y"
    { mon_autostart((yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].i),1); }
    break;

  case 156:

/* Line 1464 of yacc.c  */
#line 566 "mon_parse.y"
    { mon_autostart((yyvsp[(2) - (3)].str),0,0); }
    break;

  case 157:

/* Line 1464 of yacc.c  */
#line 568 "mon_parse.y"
    { mon_autostart((yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].i),0); }
    break;

  case 158:

/* Line 1464 of yacc.c  */
#line 572 "mon_parse.y"
    { mon_record_commands((yyvsp[(2) - (3)].str)); }
    break;

  case 159:

/* Line 1464 of yacc.c  */
#line 574 "mon_parse.y"
    { mon_end_recording(); }
    break;

  case 160:

/* Line 1464 of yacc.c  */
#line 576 "mon_parse.y"
    { mon_playback_init((yyvsp[(2) - (3)].str)); }
    break;

  case 161:

/* Line 1464 of yacc.c  */
#line 580 "mon_parse.y"
    { mon_memory_fill((yyvsp[(2) - (4)].a), BAD_ADDR, (unsigned char *)(yyvsp[(3) - (4)].str)); }
    break;

  case 162:

/* Line 1464 of yacc.c  */
#line 582 "mon_parse.y"
    { printf("Not yet.\n"); }
    break;

  case 163:

/* Line 1464 of yacc.c  */
#line 586 "mon_parse.y"
    { yydebug = 1; }
    break;

  case 164:

/* Line 1464 of yacc.c  */
#line 589 "mon_parse.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 165:

/* Line 1464 of yacc.c  */
#line 592 "mon_parse.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 593 "mon_parse.y"
    { (yyval.str) = NULL; }
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 597 "mon_parse.y"
    { return ERR_EXPECT_FILENAME; }
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 601 "mon_parse.y"
    { return ERR_EXPECT_DEVICE_NUM; }
    break;

  case 171:

/* Line 1464 of yacc.c  */
#line 604 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (2)].i) | (yyvsp[(2) - (2)].i); }
    break;

  case 172:

/* Line 1464 of yacc.c  */
#line 605 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 173:

/* Line 1464 of yacc.c  */
#line 608 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 174:

/* Line 1464 of yacc.c  */
#line 609 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 175:

/* Line 1464 of yacc.c  */
#line 612 "mon_parse.y"
    {
                                    if (!mon_register_valid(default_memspace, (yyvsp[(1) - (1)].reg))) {
                                        return ERR_INVALID_REGISTER;
                                    }
                                    (yyval.i) = new_reg(default_memspace, (yyvsp[(1) - (1)].reg));
                                }
    break;

  case 176:

/* Line 1464 of yacc.c  */
#line 618 "mon_parse.y"
    {
                                    if (!mon_register_valid((yyvsp[(1) - (2)].i), (yyvsp[(2) - (2)].reg))) {
                                        return ERR_INVALID_REGISTER;
                                    }
                                    (yyval.i) = new_reg((yyvsp[(1) - (2)].i), (yyvsp[(2) - (2)].reg));
                                }
    break;

  case 179:

/* Line 1464 of yacc.c  */
#line 631 "mon_parse.y"
    { (monitor_cpu_for_memspace[reg_memspace((yyvsp[(1) - (3)].i))]->mon_register_set_val)(reg_memspace((yyvsp[(1) - (3)].i)), reg_regid((yyvsp[(1) - (3)].i)), (uint16_t) (yyvsp[(3) - (3)].i)); }
    break;

  case 180:

/* Line 1464 of yacc.c  */
#line 634 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 181:

/* Line 1464 of yacc.c  */
#line 635 "mon_parse.y"
    { return ERR_EXPECT_CHECKNUM; }
    break;

  case 183:

/* Line 1464 of yacc.c  */
#line 639 "mon_parse.y"
    { (yyval.range)[0] = (yyvsp[(1) - (1)].a); (yyval.range)[1] = BAD_ADDR; }
    break;

  case 184:

/* Line 1464 of yacc.c  */
#line 642 "mon_parse.y"
    { (yyval.range)[0] = (yyvsp[(1) - (3)].a); (yyval.range)[1] = (yyvsp[(3) - (3)].a); }
    break;

  case 185:

/* Line 1464 of yacc.c  */
#line 644 "mon_parse.y"
    { if (resolve_range(e_default_space, (yyval.range), (yyvsp[(1) - (1)].str))) return ERR_ADDR_TOO_BIG; }
    break;

  case 186:

/* Line 1464 of yacc.c  */
#line 646 "mon_parse.y"
    { if (resolve_range((yyvsp[(1) - (3)].i), (yyval.range), (yyvsp[(3) - (3)].str))) return ERR_ADDR_TOO_BIG; }
    break;

  case 187:

/* Line 1464 of yacc.c  */
#line 649 "mon_parse.y"
    { (yyval.a) = (yyvsp[(2) - (2)].a); }
    break;

  case 188:

/* Line 1464 of yacc.c  */
#line 650 "mon_parse.y"
    { (yyval.a) = BAD_ADDR; }
    break;

  case 189:

/* Line 1464 of yacc.c  */
#line 654 "mon_parse.y"
    {
             (yyval.a) = new_addr(e_default_space,(yyvsp[(1) - (1)].i));
             if (opt_asm) new_cmd = asm_mode = 1;
         }
    break;

  case 190:

/* Line 1464 of yacc.c  */
#line 659 "mon_parse.y"
    {
             (yyval.a) = new_addr((yyvsp[(1) - (3)].i), (yyvsp[(3) - (3)].i));
             if (opt_asm) new_cmd = asm_mode = 1;
         }
    break;

  case 191:

/* Line 1464 of yacc.c  */
#line 664 "mon_parse.y"
    {
             temp = mon_symbol_table_lookup_addr(e_default_space, (yyvsp[(1) - (1)].str));
             if (temp >= 0)
                 (yyval.a) = new_addr(e_default_space, temp);
             else
                 return ERR_UNDEFINED_LABEL;
         }
    break;

  case 194:

/* Line 1464 of yacc.c  */
#line 677 "mon_parse.y"
    { (yyval.i) = e_comp_space; }
    break;

  case 195:

/* Line 1464 of yacc.c  */
#line 678 "mon_parse.y"
    { (yyval.i) = e_disk8_space; }
    break;

  case 196:

/* Line 1464 of yacc.c  */
#line 679 "mon_parse.y"
    { (yyval.i) = e_disk9_space; }
    break;

  case 197:

/* Line 1464 of yacc.c  */
#line 680 "mon_parse.y"
    { (yyval.i) = e_disk10_space; }
    break;

  case 198:

/* Line 1464 of yacc.c  */
#line 681 "mon_parse.y"
    { (yyval.i) = e_disk11_space; }
    break;

  case 199:

/* Line 1464 of yacc.c  */
#line 684 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); if (!CHECK_ADDR((yyvsp[(1) - (1)].i))) return ERR_ADDR_TOO_BIG; }
    break;

  case 200:

/* Line 1464 of yacc.c  */
#line 686 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 201:

/* Line 1464 of yacc.c  */
#line 688 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) + (yyvsp[(3) - (3)].i); }
    break;

  case 202:

/* Line 1464 of yacc.c  */
#line 689 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) - (yyvsp[(3) - (3)].i); }
    break;

  case 203:

/* Line 1464 of yacc.c  */
#line 690 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) * (yyvsp[(3) - (3)].i); }
    break;

  case 204:

/* Line 1464 of yacc.c  */
#line 691 "mon_parse.y"
    { (yyval.i) = ((yyvsp[(3) - (3)].i)) ? ((yyvsp[(1) - (3)].i) / (yyvsp[(3) - (3)].i)) : 1; }
    break;

  case 205:

/* Line 1464 of yacc.c  */
#line 692 "mon_parse.y"
    { (yyval.i) = (yyvsp[(2) - (3)].i); }
    break;

  case 206:

/* Line 1464 of yacc.c  */
#line 693 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 207:

/* Line 1464 of yacc.c  */
#line 694 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 208:

/* Line 1464 of yacc.c  */
#line 697 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(2) - (2)].cond_node); }
    break;

  case 209:

/* Line 1464 of yacc.c  */
#line 698 "mon_parse.y"
    { (yyval.cond_node) = 0; }
    break;

  case 210:

/* Line 1464 of yacc.c  */
#line 701 "mon_parse.y"
    {
               (yyval.cond_node) = new_cond; (yyval.cond_node)->is_parenthized = FALSE;
               (yyval.cond_node)->child1 = (yyvsp[(1) - (3)].cond_node); (yyval.cond_node)->child2 = (yyvsp[(3) - (3)].cond_node); (yyval.cond_node)->operation = (yyvsp[(2) - (3)].cond_op);
           }
    break;

  case 211:

/* Line 1464 of yacc.c  */
#line 706 "mon_parse.y"
    { return ERR_INCOMPLETE_COMPARE_OP; }
    break;

  case 212:

/* Line 1464 of yacc.c  */
#line 708 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(2) - (3)].cond_node); (yyval.cond_node)->is_parenthized = TRUE; }
    break;

  case 213:

/* Line 1464 of yacc.c  */
#line 710 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 214:

/* Line 1464 of yacc.c  */
#line 712 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(1) - (1)].cond_node); }
    break;

  case 215:

/* Line 1464 of yacc.c  */
#line 715 "mon_parse.y"
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->reg_num = (yyvsp[(1) - (1)].i); (yyval.cond_node)->is_reg = TRUE; (yyval.cond_node)->banknum=-1;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
    break;

  case 216:

/* Line 1464 of yacc.c  */
#line 721 "mon_parse.y"
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->value = (yyvsp[(1) - (1)].i); (yyval.cond_node)->is_reg = FALSE; (yyval.cond_node)->banknum=-1;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
    break;

  case 217:

/* Line 1464 of yacc.c  */
#line 727 "mon_parse.y"
    {(yyval.cond_node)=new_cond;
                            (yyval.cond_node)->operation=e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->banknum=mon_banknum_from_bank(e_default_space,(yyvsp[(2) - (4)].str)); (yyval.cond_node)->value = (yyvsp[(4) - (4)].a); (yyval.cond_node)->is_reg = FALSE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;  
                        }
    break;

  case 220:

/* Line 1464 of yacc.c  */
#line 739 "mon_parse.y"
    { mon_add_number_to_buffer((yyvsp[(1) - (1)].i)); }
    break;

  case 221:

/* Line 1464 of yacc.c  */
#line 740 "mon_parse.y"
    { mon_add_string_to_buffer((yyvsp[(1) - (1)].str)); }
    break;

  case 224:

/* Line 1464 of yacc.c  */
#line 747 "mon_parse.y"
    { mon_add_number_to_buffer((yyvsp[(1) - (1)].i)); }
    break;

  case 225:

/* Line 1464 of yacc.c  */
#line 748 "mon_parse.y"
    { mon_add_number_masked_to_buffer((yyvsp[(1) - (1)].i), 0x00); }
    break;

  case 226:

/* Line 1464 of yacc.c  */
#line 749 "mon_parse.y"
    { mon_add_string_to_buffer((yyvsp[(1) - (1)].str)); }
    break;

  case 227:

/* Line 1464 of yacc.c  */
#line 752 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 228:

/* Line 1464 of yacc.c  */
#line 753 "mon_parse.y"
    { (yyval.i) = (monitor_cpu_for_memspace[reg_memspace((yyvsp[(1) - (1)].i))]->mon_register_get_val)(reg_memspace((yyvsp[(1) - (1)].i)), reg_regid((yyvsp[(1) - (1)].i))); }
    break;

  case 229:

/* Line 1464 of yacc.c  */
#line 756 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 230:

/* Line 1464 of yacc.c  */
#line 757 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 231:

/* Line 1464 of yacc.c  */
#line 758 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 232:

/* Line 1464 of yacc.c  */
#line 759 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 233:

/* Line 1464 of yacc.c  */
#line 762 "mon_parse.y"
    { (yyval.i) = resolve_datatype(B_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 234:

/* Line 1464 of yacc.c  */
#line 763 "mon_parse.y"
    { (yyval.i) = resolve_datatype(O_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 235:

/* Line 1464 of yacc.c  */
#line 764 "mon_parse.y"
    { (yyval.i) = resolve_datatype(D_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 236:

/* Line 1464 of yacc.c  */
#line 767 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 237:

/* Line 1464 of yacc.c  */
#line 768 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 238:

/* Line 1464 of yacc.c  */
#line 769 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 239:

/* Line 1464 of yacc.c  */
#line 770 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 240:

/* Line 1464 of yacc.c  */
#line 771 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 244:

/* Line 1464 of yacc.c  */
#line 779 "mon_parse.y"
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

  case 246:

/* Line 1464 of yacc.c  */
#line 790 "mon_parse.y"
    { asm_mode = 0; }
    break;

  case 247:

/* Line 1464 of yacc.c  */
#line 793 "mon_parse.y"
    { if ((yyvsp[(2) - (2)].i) > 0xff) {
                          (yyval.mode).addr_mode = ASM_ADDR_MODE_IMMEDIATE_16;
                          (yyval.mode).param = (yyvsp[(2) - (2)].i);
                        } else {
                          (yyval.mode).addr_mode = ASM_ADDR_MODE_IMMEDIATE;
                          (yyval.mode).param = (yyvsp[(2) - (2)].i);
                        } }
    break;

  case 248:

/* Line 1464 of yacc.c  */
#line 800 "mon_parse.y"
    { if ((yyvsp[(1) - (1)].i) >= 0x10000) {
               (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_LONG;
               (yyval.mode).param = (yyvsp[(1) - (1)].i);
             } else if ((yyvsp[(1) - (1)].i) < 0x100) {
               (yyval.mode).addr_mode = ASM_ADDR_MODE_ZERO_PAGE;
               (yyval.mode).param = (yyvsp[(1) - (1)].i);
             } else {
               (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE;
               (yyval.mode).param = (yyvsp[(1) - (1)].i);
             }
           }
    break;

  case 249:

/* Line 1464 of yacc.c  */
#line 811 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) >= 0x10000) {
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_LONG_X;
                            (yyval.mode).param = (yyvsp[(1) - (3)].i);
                          } else if ((yyvsp[(1) - (3)].i) < 0x100) { 
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_ZERO_PAGE_X;
                            (yyval.mode).param = (yyvsp[(1) - (3)].i);
                          } else {
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_X;
                            (yyval.mode).param = (yyvsp[(1) - (3)].i);
                          }
                        }
    break;

  case 250:

/* Line 1464 of yacc.c  */
#line 822 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100) {
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_ZERO_PAGE_Y;
                            (yyval.mode).param = (yyvsp[(1) - (3)].i);
                          } else {
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_Y;
                            (yyval.mode).param = (yyvsp[(1) - (3)].i);
                          }
                        }
    break;

  case 251:

/* Line 1464 of yacc.c  */
#line 830 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100) {
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_STACK_RELATIVE;
                            (yyval.mode).param = (yyvsp[(1) - (3)].i);
                          } else { /* 6809 */
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
                        }
    break;

  case 252:

/* Line 1464 of yacc.c  */
#line 849 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100) {
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_DOUBLE;
                            (yyval.mode).param = (yyvsp[(3) - (3)].i);
                            (yyval.mode).addr_submode = (yyvsp[(1) - (3)].i);
                          }
                        }
    break;

  case 253:

/* Line 1464 of yacc.c  */
#line 855 "mon_parse.y"
    { if ((yyvsp[(2) - (3)].i) < 0x100) {
                               (yyval.mode).addr_mode = ASM_ADDR_MODE_INDIRECT;
                               (yyval.mode).param = (yyvsp[(2) - (3)].i);
                             } else {
                               (yyval.mode).addr_mode = ASM_ADDR_MODE_ABS_INDIRECT;
                               (yyval.mode).param = (yyvsp[(2) - (3)].i);
                             }
                           }
    break;

  case 254:

/* Line 1464 of yacc.c  */
#line 863 "mon_parse.y"
    { if ((yyvsp[(2) - (5)].i) < 0x100) {
                                           (yyval.mode).addr_mode = ASM_ADDR_MODE_INDIRECT_X;
                                           (yyval.mode).param = (yyvsp[(2) - (5)].i);
                                         } else {
                                           (yyval.mode).addr_mode = ASM_ADDR_MODE_ABS_INDIRECT_X;
                                           (yyval.mode).param = (yyvsp[(2) - (5)].i);
                                         }
                                       }
    break;

  case 255:

/* Line 1464 of yacc.c  */
#line 872 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_STACK_RELATIVE_Y; (yyval.mode).param = (yyvsp[(2) - (7)].i); }
    break;

  case 256:

/* Line 1464 of yacc.c  */
#line 874 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_INDIRECT_Y; (yyval.mode).param = (yyvsp[(2) - (5)].i); }
    break;

  case 257:

/* Line 1464 of yacc.c  */
#line 875 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_BC; }
    break;

  case 258:

/* Line 1464 of yacc.c  */
#line 876 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_DE; }
    break;

  case 259:

/* Line 1464 of yacc.c  */
#line 877 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_HL; }
    break;

  case 260:

/* Line 1464 of yacc.c  */
#line 878 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_IX; }
    break;

  case 261:

/* Line 1464 of yacc.c  */
#line 879 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_IY; }
    break;

  case 262:

/* Line 1464 of yacc.c  */
#line 880 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_SP; }
    break;

  case 263:

/* Line 1464 of yacc.c  */
#line 882 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_A; (yyval.mode).param = (yyvsp[(2) - (5)].i); }
    break;

  case 264:

/* Line 1464 of yacc.c  */
#line 884 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_HL; (yyval.mode).param = (yyvsp[(2) - (5)].i); }
    break;

  case 265:

/* Line 1464 of yacc.c  */
#line 886 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_IX; (yyval.mode).param = (yyvsp[(2) - (5)].i); }
    break;

  case 266:

/* Line 1464 of yacc.c  */
#line 888 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_IY; (yyval.mode).param = (yyvsp[(2) - (5)].i); }
    break;

  case 267:

/* Line 1464 of yacc.c  */
#line 889 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_IMPLIED; }
    break;

  case 268:

/* Line 1464 of yacc.c  */
#line 890 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_ACCUMULATOR; }
    break;

  case 269:

/* Line 1464 of yacc.c  */
#line 891 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_B; }
    break;

  case 270:

/* Line 1464 of yacc.c  */
#line 892 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_C; }
    break;

  case 271:

/* Line 1464 of yacc.c  */
#line 893 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_D; }
    break;

  case 272:

/* Line 1464 of yacc.c  */
#line 894 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_E; }
    break;

  case 273:

/* Line 1464 of yacc.c  */
#line 895 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_H; }
    break;

  case 274:

/* Line 1464 of yacc.c  */
#line 896 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IXH; }
    break;

  case 275:

/* Line 1464 of yacc.c  */
#line 897 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IYH; }
    break;

  case 276:

/* Line 1464 of yacc.c  */
#line 898 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_L; }
    break;

  case 277:

/* Line 1464 of yacc.c  */
#line 899 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IXL; }
    break;

  case 278:

/* Line 1464 of yacc.c  */
#line 900 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IYL; }
    break;

  case 279:

/* Line 1464 of yacc.c  */
#line 901 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_AF; }
    break;

  case 280:

/* Line 1464 of yacc.c  */
#line 902 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_BC; }
    break;

  case 281:

/* Line 1464 of yacc.c  */
#line 903 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_DE; }
    break;

  case 282:

/* Line 1464 of yacc.c  */
#line 904 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_HL; }
    break;

  case 283:

/* Line 1464 of yacc.c  */
#line 905 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IX; }
    break;

  case 284:

/* Line 1464 of yacc.c  */
#line 906 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IY; }
    break;

  case 285:

/* Line 1464 of yacc.c  */
#line 907 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_SP; }
    break;

  case 286:

/* Line 1464 of yacc.c  */
#line 909 "mon_parse.y"
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_DIRECT; (yyval.mode).param = (yyvsp[(2) - (2)].i); }
    break;

  case 287:

/* Line 1464 of yacc.c  */
#line 910 "mon_parse.y"
    {    /* Clash with addr,x addr,y addr,s modes! */
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        if ((yyvsp[(1) - (3)].i) >= -16 && (yyvsp[(1) - (3)].i) < 16) {
            (yyval.mode).addr_submode = (3 << 5) | ((yyvsp[(1) - (3)].i) & 0x1F);
        } else if ((yyvsp[(1) - (3)].i) >= -128 && (yyvsp[(1) - (3)].i) < 128) {
            (yyval.mode).addr_submode = 0x80 | (3 << 5) | ASM_ADDR_MODE_INDEXED_OFF8;
            (yyval.mode).param = (yyvsp[(1) - (3)].i);
        } else if ((yyvsp[(1) - (3)].i) >= -32768 && (yyvsp[(1) - (3)].i) < 32768) {
            (yyval.mode).addr_submode = 0x80 | (3 << 5) | ASM_ADDR_MODE_INDEXED_OFF16;
            (yyval.mode).param = (yyvsp[(1) - (3)].i);
        } else {
            (yyval.mode).addr_mode = ASM_ADDR_MODE_ILLEGAL;
            mon_out("offset too large even for 16 bits (signed)\n");
        }
    }
    break;

  case 288:

/* Line 1464 of yacc.c  */
#line 925 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(2) - (3)].i) | ASM_ADDR_MODE_INDEXED_INC1;
        }
    break;

  case 289:

/* Line 1464 of yacc.c  */
#line 929 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(2) - (4)].i) | ASM_ADDR_MODE_INDEXED_INC2;
        }
    break;

  case 290:

/* Line 1464 of yacc.c  */
#line 933 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(3) - (3)].i) | ASM_ADDR_MODE_INDEXED_DEC1;
        }
    break;

  case 291:

/* Line 1464 of yacc.c  */
#line 937 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(4) - (4)].i) | ASM_ADDR_MODE_INDEXED_DEC2;
        }
    break;

  case 292:

/* Line 1464 of yacc.c  */
#line 941 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(2) - (2)].i) | ASM_ADDR_MODE_INDEXED_OFF0;
        }
    break;

  case 293:

/* Line 1464 of yacc.c  */
#line 945 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(2) - (3)].i) | ASM_ADDR_MODE_INDEXED_OFFB;
        }
    break;

  case 294:

/* Line 1464 of yacc.c  */
#line 949 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(2) - (3)].i) | ASM_ADDR_MODE_INDEXED_OFFA;
        }
    break;

  case 295:

/* Line 1464 of yacc.c  */
#line 953 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(2) - (3)].i) | ASM_ADDR_MODE_INDEXED_OFFD;
        }
    break;

  case 296:

/* Line 1464 of yacc.c  */
#line 957 "mon_parse.y"
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

  case 297:

/* Line 1464 of yacc.c  */
#line 969 "mon_parse.y"
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

  case 298:

/* Line 1464 of yacc.c  */
#line 984 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(3) - (5)].i) | ASM_ADDR_MODE_INDEXED_INC1;
        }
    break;

  case 299:

/* Line 1464 of yacc.c  */
#line 988 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(3) - (6)].i) | ASM_ADDR_MODE_INDEXED_INC2;
        }
    break;

  case 300:

/* Line 1464 of yacc.c  */
#line 992 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(4) - (5)].i) | ASM_ADDR_MODE_INDEXED_DEC1;
        }
    break;

  case 301:

/* Line 1464 of yacc.c  */
#line 996 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(5) - (6)].i) | ASM_ADDR_MODE_INDEXED_DEC2;
        }
    break;

  case 302:

/* Line 1464 of yacc.c  */
#line 1000 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(3) - (4)].i) | ASM_ADDR_MODE_INDEXED_OFF0;
        }
    break;

  case 303:

/* Line 1464 of yacc.c  */
#line 1004 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(3) - (5)].i) | ASM_ADDR_MODE_INDEXED_OFFB;
        }
    break;

  case 304:

/* Line 1464 of yacc.c  */
#line 1008 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(3) - (5)].i) | ASM_ADDR_MODE_INDEXED_OFFA;
        }
    break;

  case 305:

/* Line 1464 of yacc.c  */
#line 1012 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[(3) - (5)].i) | ASM_ADDR_MODE_INDEXED_OFFD;
        }
    break;

  case 306:

/* Line 1464 of yacc.c  */
#line 1016 "mon_parse.y"
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

  case 307:

/* Line 1464 of yacc.c  */
#line 1028 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | ASM_ADDR_MODE_EXTENDED_INDIRECT;
        (yyval.mode).param = (yyvsp[(2) - (3)].i);
        }
    break;

  case 308:

/* Line 1464 of yacc.c  */
#line 1033 "mon_parse.y"
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDIRECT_LONG_Y;
        (yyval.mode).param = (yyvsp[(2) - (5)].i);
        }
    break;

  case 309:

/* Line 1464 of yacc.c  */
#line 1041 "mon_parse.y"
    { (yyval.i) = (0 << 5); printf("reg_x\n"); }
    break;

  case 310:

/* Line 1464 of yacc.c  */
#line 1042 "mon_parse.y"
    { (yyval.i) = (1 << 5); printf("reg_y\n"); }
    break;

  case 311:

/* Line 1464 of yacc.c  */
#line 1043 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 312:

/* Line 1464 of yacc.c  */
#line 1044 "mon_parse.y"
    { (yyval.i) = (3 << 5); printf("reg_s\n"); }
    break;

  case 313:

/* Line 1464 of yacc.c  */
#line 1048 "mon_parse.y"
    { (yyval.i) = (2 << 5); printf("reg_u\n"); }
    break;



/* Line 1464 of yacc.c  */
#line 4864 "mon_parse.c"
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



/* Line 1684 of yacc.c  */
#line 1052 "mon_parse.y"


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
   mon_clear_buffer();
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
         case ERR_INVALID_REGISTER:
           mon_out("Invalid register.\n");
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



