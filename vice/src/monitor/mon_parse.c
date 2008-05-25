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
     CMD_STOP = 329,
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
     CMD_RESET = 352,
     CMD_TAPECTRL = 353,
     CMD_CARTFREEZE = 354,
     CMD_LABEL_ASGN = 355,
     L_PAREN = 356,
     R_PAREN = 357,
     ARG_IMMEDIATE = 358,
     REG_A = 359,
     REG_X = 360,
     REG_Y = 361,
     COMMA = 362,
     INST_SEP = 363,
     REG_B = 364,
     REG_C = 365,
     REG_D = 366,
     REG_E = 367,
     REG_H = 368,
     REG_L = 369,
     REG_AF = 370,
     REG_BC = 371,
     REG_DE = 372,
     REG_HL = 373,
     REG_IX = 374,
     REG_IY = 375,
     REG_SP = 376,
     REG_IXH = 377,
     REG_IXL = 378,
     REG_IYH = 379,
     REG_IYL = 380,
     STRING = 381,
     FILENAME = 382,
     R_O_L = 383,
     OPCODE = 384,
     LABEL = 385,
     BANKNAME = 386,
     CPUTYPE = 387,
     REGISTER = 388,
     COMPARE_OP = 389,
     RADIX_TYPE = 390,
     INPUT_SPEC = 391,
     CMD_CHECKPT_ON = 392,
     CMD_CHECKPT_OFF = 393,
     TOGGLE = 394,
     MASK = 395
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
#define CMD_STOP 329
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
#define CMD_RESET 352
#define CMD_TAPECTRL 353
#define CMD_CARTFREEZE 354
#define CMD_LABEL_ASGN 355
#define L_PAREN 356
#define R_PAREN 357
#define ARG_IMMEDIATE 358
#define REG_A 359
#define REG_X 360
#define REG_Y 361
#define COMMA 362
#define INST_SEP 363
#define REG_B 364
#define REG_C 365
#define REG_D 366
#define REG_E 367
#define REG_H 368
#define REG_L 369
#define REG_AF 370
#define REG_BC 371
#define REG_DE 372
#define REG_HL 373
#define REG_IX 374
#define REG_IY 375
#define REG_SP 376
#define REG_IXH 377
#define REG_IXL 378
#define REG_IYH 379
#define REG_IYL 380
#define STRING 381
#define FILENAME 382
#define R_O_L 383
#define OPCODE 384
#define LABEL 385
#define BANKNAME 386
#define CPUTYPE 387
#define REGISTER 388
#define COMPARE_OP 389
#define RADIX_TYPE 390
#define INPUT_SPEC 391
#define CMD_CHECKPT_ON 392
#define CMD_CHECKPT_OFF 393
#define TOGGLE 394
#define MASK 395




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
#define CHECK_ADDR(x) ((x) == LO16(x))

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
#line 118 "mon_parse.y"
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
#line 505 "mon_parse.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 518 "mon_parse.c"

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
#define YYFINAL  270
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1552

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  147
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  51
/* YYNRULES -- Number of rules.  */
#define YYNRULES  253
/* YYNRULES -- Number of states.  */
#define YYNSTATES  494

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   395

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     145,   146,   143,   141,     2,   142,     2,   144,     2,     2,
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
     135,   136,   137,   138,   139,   140
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    15,    17,    19,
      21,    23,    25,    27,    29,    31,    33,    35,    37,    39,
      41,    43,    45,    47,    50,    54,    58,    64,    68,    71,
      75,    78,    82,    86,    89,    94,    97,   102,   105,   110,
     113,   118,   121,   123,   126,   130,   134,   140,   144,   150,
     154,   160,   164,   170,   174,   177,   182,   188,   189,   195,
     199,   203,   206,   212,   218,   224,   230,   236,   240,   243,
     247,   250,   254,   257,   261,   264,   268,   271,   275,   279,
     285,   291,   295,   298,   301,   304,   307,   311,   315,   319,
     325,   329,   332,   338,   344,   349,   353,   356,   360,   363,
     367,   370,   373,   377,   381,   384,   388,   392,   396,   400,
     404,   407,   411,   414,   418,   424,   428,   433,   436,   441,
     446,   449,   455,   461,   467,   471,   476,   482,   487,   493,
     498,   504,   510,   515,   519,   523,   526,   530,   535,   538,
     541,   543,   545,   546,   548,   550,   552,   554,   556,   557,
     559,   562,   566,   568,   572,   574,   576,   578,   580,   584,
     586,   590,   593,   594,   596,   600,   602,   604,   605,   607,
     609,   611,   613,   615,   617,   619,   623,   627,   631,   635,
     639,   643,   645,   649,   653,   657,   661,   663,   665,   667,
     671,   673,   675,   677,   680,   682,   684,   686,   688,   690,
     692,   694,   696,   698,   700,   702,   704,   706,   708,   710,
     712,   714,   716,   720,   724,   727,   730,   732,   734,   737,
     739,   743,   747,   751,   757,   763,   767,   771,   775,   779,
     783,   787,   793,   799,   805,   811,   812,   814,   816,   818,
     820,   822,   824,   826,   828,   830,   832,   834,   836,   838,
     840,   842,   844,   846
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     148,     0,    -1,   149,    -1,   195,    22,    -1,    22,    -1,
     151,    -1,   149,   151,    -1,    23,    -1,    22,    -1,     1,
      -1,   152,    -1,   154,    -1,   157,    -1,   155,    -1,   158,
      -1,   159,    -1,   160,    -1,   161,    -1,   162,    -1,   163,
      -1,   164,    -1,   165,    -1,    13,    -1,    67,   150,    -1,
      67,   180,   150,    -1,    67,   131,   150,    -1,    67,   180,
     179,   131,   150,    -1,    38,   178,   150,    -1,    46,   150,
      -1,    87,   132,   150,    -1,    26,   150,    -1,    49,   168,
     150,    -1,    50,   168,   150,    -1,    58,   150,    -1,    58,
     179,   183,   150,    -1,    57,   150,    -1,    57,   179,   183,
     150,    -1,    29,   150,    -1,    29,   179,   183,   150,    -1,
      30,   150,    -1,    30,   179,   183,   150,    -1,    85,   150,
      -1,   153,    -1,    39,   150,    -1,    39,   180,   150,    -1,
      39,   172,   150,    -1,    68,   180,   179,   168,   150,    -1,
      68,   168,   150,    -1,    69,   180,   179,   168,   150,    -1,
      69,   168,   150,    -1,    70,   178,   179,   130,   150,    -1,
      71,   130,   150,    -1,    71,   180,   179,   130,   150,    -1,
      72,   180,   150,    -1,    72,   150,    -1,   100,    21,   178,
     150,    -1,   100,    21,   178,    24,   150,    -1,    -1,    55,
     178,   156,   196,   150,    -1,    55,   178,   150,    -1,    56,
     175,   150,    -1,    56,   150,    -1,    37,   176,   179,   178,
     150,    -1,    48,   176,   179,   178,   150,    -1,    36,   176,
     179,   186,   150,    -1,    35,   176,   179,   188,   150,    -1,
      43,   135,   179,   175,   150,    -1,    43,   175,   150,    -1,
      43,   150,    -1,    76,   175,   150,    -1,    76,   150,    -1,
      77,   175,   150,    -1,    77,   150,    -1,    78,   175,   150,
      -1,    78,   150,    -1,    79,   175,   150,    -1,    79,   150,
      -1,    44,   175,   150,    -1,    86,   175,   150,    -1,    44,
     175,    15,   184,   150,    -1,    62,   170,   179,   175,   150,
      -1,    45,   175,   150,    -1,    44,   150,    -1,    86,   150,
      -1,    45,   150,    -1,    62,   150,    -1,   137,   174,   150,
      -1,   138,   174,   150,    -1,    34,   174,   150,    -1,    34,
     174,   179,   183,   150,    -1,    52,   174,   150,    -1,    52,
     150,    -1,    53,   174,    15,   184,   150,    -1,    54,   174,
     179,   126,   150,    -1,    54,   174,     1,   150,    -1,    25,
     139,   150,    -1,    25,   150,    -1,    42,   135,   150,    -1,
      42,   150,    -1,    60,   180,   150,    -1,    65,   150,    -1,
      51,   150,    -1,    63,   166,   150,    -1,    59,   183,   150,
      -1,    61,   150,    -1,    61,   166,   150,    -1,    64,   166,
     150,    -1,     7,   183,   150,    -1,    66,   166,   150,    -1,
      82,   166,   150,    -1,    89,   150,    -1,    92,   167,   150,
      -1,    91,   150,    -1,    90,   168,   150,    -1,    90,   168,
     179,   183,   150,    -1,    93,   126,   150,    -1,    94,   126,
     126,   150,    -1,    97,   150,    -1,    97,   179,   183,   150,
      -1,    98,   179,   183,   150,    -1,    99,   150,    -1,    31,
     168,   169,   177,   150,    -1,    83,   168,   169,   177,   150,
      -1,    32,   168,   169,   176,   150,    -1,    32,   168,     1,
      -1,    32,   168,   169,     1,    -1,    84,   168,   169,   176,
     150,    -1,    84,   168,   169,     1,    -1,    33,   168,   169,
     178,   150,    -1,    33,   168,   169,     1,    -1,    27,   183,
     183,   177,   150,    -1,    28,   183,   183,   178,   150,    -1,
      95,   168,   183,   150,    -1,    96,   183,   150,    -1,    73,
     168,   150,    -1,    74,   150,    -1,    75,   168,   150,    -1,
      80,   178,   186,   150,    -1,    81,   150,    -1,    88,   150,
      -1,   128,    -1,   128,    -1,    -1,   127,    -1,     1,    -1,
     183,    -1,     1,    -1,    14,    -1,    -1,   133,    -1,   180,
     133,    -1,   172,   107,   173,    -1,   173,    -1,   171,    21,
     193,    -1,   191,    -1,     1,    -1,   176,    -1,   178,    -1,
     178,   179,   178,    -1,     9,    -1,   180,   179,     9,    -1,
     179,   178,    -1,    -1,   181,    -1,   180,   179,   181,    -1,
     130,    -1,   107,    -1,    -1,    16,    -1,    17,    -1,    18,
      -1,    19,    -1,    20,    -1,   182,    -1,   193,    -1,   183,
     141,   183,    -1,   183,   142,   183,    -1,   183,   143,   183,
      -1,   183,   144,   183,    -1,   145,   183,   146,    -1,   145,
     183,     1,    -1,   190,    -1,   184,   134,   184,    -1,   184,
     134,     1,    -1,   101,   184,   102,    -1,   101,   184,     1,
      -1,   185,    -1,   171,    -1,   193,    -1,   186,   179,   187,
      -1,   187,    -1,   193,    -1,   126,    -1,   188,   189,    -1,
     189,    -1,   193,    -1,   140,    -1,   126,    -1,   193,    -1,
     171,    -1,     4,    -1,    12,    -1,    11,    -1,    10,    -1,
      12,    -1,    11,    -1,    10,    -1,     3,    -1,     4,    -1,
       5,    -1,     6,    -1,   192,    -1,   194,   108,   195,    -1,
     195,   108,   195,    -1,   195,   108,    -1,   129,   197,    -1,
     195,    -1,   194,    -1,   103,   193,    -1,   193,    -1,   193,
     107,   105,    -1,   193,   107,   106,    -1,   101,   193,   102,
      -1,   101,   193,   107,   105,   102,    -1,   101,   193,   102,
     107,   106,    -1,   101,   116,   102,    -1,   101,   117,   102,
      -1,   101,   118,   102,    -1,   101,   119,   102,    -1,   101,
     120,   102,    -1,   101,   121,   102,    -1,   101,   193,   102,
     107,   104,    -1,   101,   193,   102,   107,   118,    -1,   101,
     193,   102,   107,   119,    -1,   101,   193,   102,   107,   120,
      -1,    -1,   104,    -1,   109,    -1,   110,    -1,   111,    -1,
     112,    -1,   113,    -1,   122,    -1,   124,    -1,   114,    -1,
     123,    -1,   125,    -1,   115,    -1,   116,    -1,   117,    -1,
     118,    -1,   119,    -1,   120,    -1,   121,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   174,   174,   175,   176,   179,   180,   183,   184,   185,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   203,   205,   207,   209,   211,   213,   215,
     217,   219,   221,   223,   225,   227,   229,   231,   233,   235,
     237,   239,   241,   244,   246,   248,   251,   257,   263,   265,
     267,   269,   271,   273,   275,   277,   281,   288,   287,   290,
     292,   294,   298,   300,   302,   304,   306,   308,   310,   312,
     314,   316,   318,   320,   322,   324,   326,   330,   335,   340,
     346,   352,   357,   359,   361,   363,   368,   370,   372,   374,
     376,   378,   380,   382,   384,   388,   390,   395,   397,   415,
     420,   422,   426,   428,   430,   432,   434,   436,   438,   440,
     442,   444,   446,   448,   450,   452,   454,   456,   458,   460,
     462,   467,   469,   471,   473,   475,   477,   479,   481,   483,
     485,   487,   489,   491,   495,   497,   499,   503,   505,   509,
     513,   516,   517,   520,   521,   524,   525,   528,   529,   532,
     533,   536,   537,   540,   544,   545,   548,   549,   552,   553,
     555,   559,   560,   563,   568,   573,   583,   584,   587,   588,
     589,   590,   591,   594,   596,   598,   599,   600,   601,   602,
     603,   604,   607,   612,   614,   616,   618,   622,   628,   636,
     637,   640,   641,   644,   645,   648,   649,   650,   653,   654,
     657,   658,   659,   660,   663,   664,   665,   668,   669,   670,
     671,   672,   675,   676,   677,   680,   690,   691,   694,   698,
     703,   708,   713,   715,   717,   719,   720,   721,   722,   723,
     724,   725,   727,   729,   731,   733,   734,   735,   736,   737,
     738,   739,   740,   741,   742,   743,   744,   745,   746,   747,
     748,   749,   750,   751
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
  "CMD_RECORD", "CMD_STOP", "CMD_PLAYBACK", "CMD_CHAR_DISPLAY",
  "CMD_SPRITE_DISPLAY", "CMD_TEXT_DISPLAY", "CMD_SCREENCODE_DISPLAY",
  "CMD_ENTER_DATA", "CMD_ENTER_BIN_DATA", "CMD_KEYBUF", "CMD_BLOAD",
  "CMD_BSAVE", "CMD_SCREEN", "CMD_UNTIL", "CMD_CPU", "CMD_YYDEBUG",
  "CMD_BACKTRACE", "CMD_SCREENSHOT", "CMD_PWD", "CMD_DIR",
  "CMD_RESOURCE_GET", "CMD_RESOURCE_SET", "CMD_ATTACH", "CMD_DETACH",
  "CMD_RESET", "CMD_TAPECTRL", "CMD_CARTFREEZE", "CMD_LABEL_ASGN",
  "L_PAREN", "R_PAREN", "ARG_IMMEDIATE", "REG_A", "REG_X", "REG_Y",
  "COMMA", "INST_SEP", "REG_B", "REG_C", "REG_D", "REG_E", "REG_H",
  "REG_L", "REG_AF", "REG_BC", "REG_DE", "REG_HL", "REG_IX", "REG_IY",
  "REG_SP", "REG_IXH", "REG_IXL", "REG_IYH", "REG_IYL", "STRING",
  "FILENAME", "R_O_L", "OPCODE", "LABEL", "BANKNAME", "CPUTYPE",
  "REGISTER", "COMPARE_OP", "RADIX_TYPE", "INPUT_SPEC", "CMD_CHECKPT_ON",
  "CMD_CHECKPT_OFF", "TOGGLE", "MASK", "'+'", "'-'", "'*'", "'/'", "'('",
  "')'", "$accept", "top_level", "command_list", "end_cmd", "command",
  "machine_state_rules", "register_mod", "symbol_table_rules", "asm_rules",
  "@1", "memory_rules", "checkpoint_rules", "checkpoint_control_rules",
  "monitor_state_rules", "monitor_misc_rules", "disk_rules",
  "cmd_file_rules", "data_entry_rules", "monitor_debug_rules",
  "rest_of_line", "opt_rest_of_line", "filename", "device_num",
  "opt_mem_op", "register", "reg_list", "reg_asgn", "breakpt_num",
  "address_opt_range", "address_range", "opt_address", "address",
  "opt_sep", "memspace", "memloc", "memaddr", "expression", "cond_expr",
  "compare_operand", "data_list", "data_element", "hunt_list",
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
     395,    43,    45,    42,    47,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   147,   148,   148,   148,   149,   149,   150,   150,   150,
     151,   151,   151,   151,   151,   151,   151,   151,   151,   151,
     151,   151,   151,   152,   152,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   152,   152,   153,   153,   153,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   156,   155,   155,
     155,   155,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   157,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   160,   160,   160,   160,   160,
     160,   160,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   163,   163,   163,   164,   164,   165,
     166,   167,   167,   168,   168,   169,   169,   170,   170,   171,
     171,   172,   172,   173,   174,   174,   175,   175,   176,   176,
     176,   177,   177,   178,   178,   178,   179,   179,   180,   180,
     180,   180,   180,   181,   182,   183,   183,   183,   183,   183,
     183,   183,   184,   184,   184,   184,   184,   185,   185,   186,
     186,   187,   187,   188,   188,   189,   189,   189,   190,   190,
     191,   191,   191,   191,   192,   192,   192,   193,   193,   193,
     193,   193,   194,   194,   194,   195,   196,   196,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     3,     5,     3,     2,     3,
       2,     3,     3,     2,     4,     2,     4,     2,     4,     2,
       4,     2,     1,     2,     3,     3,     5,     3,     5,     3,
       5,     3,     5,     3,     2,     4,     5,     0,     5,     3,
       3,     2,     5,     5,     5,     5,     5,     3,     2,     3,
       2,     3,     2,     3,     2,     3,     2,     3,     3,     5,
       5,     3,     2,     2,     2,     2,     3,     3,     3,     5,
       3,     2,     5,     5,     4,     3,     2,     3,     2,     3,
       2,     2,     3,     3,     2,     3,     3,     3,     3,     3,
       2,     3,     2,     3,     5,     3,     4,     2,     4,     4,
       2,     5,     5,     5,     3,     4,     5,     4,     5,     4,
       5,     5,     4,     3,     3,     2,     3,     4,     2,     2,
       1,     1,     0,     1,     1,     1,     1,     1,     0,     1,
       2,     3,     1,     3,     1,     1,     1,     1,     3,     1,
       3,     2,     0,     1,     3,     1,     1,     0,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     1,     3,     3,     3,     3,     1,     1,     1,     3,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     2,     2,     1,     1,     2,     1,
       3,     3,     3,     5,     5,     3,     3,     3,     3,     3,
       3,     5,     5,     5,     5,     0,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,    22,     4,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   142,     0,
       0,     0,     0,     0,   167,     0,     0,   235,     0,     0,
       0,     2,     5,    10,    42,    11,    13,    12,    14,    15,
      16,    17,    18,    19,    20,    21,     0,   207,   208,   209,
     210,   206,   205,   204,   168,   169,   170,   171,   172,   149,
       0,   199,     0,     0,   181,   211,   198,     9,     8,     7,
       0,    96,    30,     0,     0,   166,    37,     0,    39,     0,
     144,   143,     0,     0,     0,   155,   200,   203,   202,   201,
       0,   154,   159,   165,   167,   167,   167,   163,   173,   174,
     167,   167,     0,   167,    43,     0,     0,   152,     0,     0,
      98,   167,    68,     0,   156,   167,    82,     0,    84,     0,
      28,   167,     0,     0,   101,     9,    91,     0,     0,     0,
       0,    61,     0,    35,     0,    33,     0,     0,     0,   140,
     104,     0,   147,    85,   167,     0,     0,   100,     0,     0,
      23,     0,     0,   167,     0,   167,   167,     0,   167,    54,
       0,     0,   135,     0,    70,     0,    72,     0,    74,     0,
      76,     0,     0,   138,     0,     0,     0,    41,    83,     0,
       0,   139,   110,     0,   112,   141,     0,     0,     0,     0,
       0,   117,     0,     0,   120,     0,     0,     0,   236,   237,
     238,   239,   240,   241,   244,   247,   248,   249,   250,   251,
     252,   253,   242,   245,   243,   246,   219,   215,     0,     0,
       1,     6,     3,     0,   150,     0,     0,     0,     0,   107,
      95,   167,     0,     0,     0,   146,   167,   145,   124,     0,
       0,    88,     0,     0,     0,     0,     0,     0,    27,     0,
       0,     0,    45,    44,    97,     0,    67,     0,    77,    81,
       0,    31,    32,    90,     0,     0,     0,    59,     0,    60,
       0,     0,   103,    99,   105,     0,   102,   106,   108,    25,
      24,     0,    47,     0,    49,     0,     0,    51,     0,    53,
     134,   136,    69,    71,    73,    75,   192,     0,   190,   191,
     109,   167,     0,    78,    29,   113,     0,   111,   115,     0,
       0,   133,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   218,     0,    86,    87,   180,   179,   175,   176,
     177,   178,     0,     0,     0,    38,    40,     0,   125,     0,
     129,     0,     0,   197,   196,     0,   194,   195,   158,   160,
     164,     0,     0,   153,   151,     0,     0,   187,     0,   186,
     188,     0,     0,    94,     0,   217,   216,     0,    36,    34,
       0,     0,     0,     0,     0,     0,   137,     0,     0,   127,
       0,     0,   116,   132,   118,   119,     0,    55,   225,   226,
     227,   228,   229,   230,   222,     0,   220,   221,   130,   161,
     131,   121,   123,   128,    89,    65,   193,    64,    62,    66,
       0,     0,    79,    63,    92,    93,     0,   214,    58,    80,
      26,    46,    48,    50,    52,   189,   122,   126,   114,    56,
       0,     0,   185,   184,   183,   182,   212,   213,   231,   224,
     232,   233,   234,   223
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    80,    81,   121,    82,    83,    84,    85,    86,   318,
      87,    88,    89,    90,    91,    92,    93,    94,    95,   191,
     236,   132,   286,   194,   111,   156,   157,   140,   163,   164,
     382,   165,   383,   112,   147,   148,   287,   408,   409,   347,
     348,   395,   396,   114,   141,   115,   149,   415,    96,   417,
     267
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -315
static const yytype_int16 yypact[] =
{
    1030,   826,  -315,  -315,    15,    72,   826,   826,   745,   745,
      10,    10,    10,   392,  1360,  1360,  1360,  1379,   214,    52,
     918,  1013,  1013,    72,  1360,    10,    10,    72,   627,   392,
     392,  1379,  1013,   745,   745,   826,   367,    68,  1274,   -92,
     -92,    72,   -92,   411,   253,   253,  1379,   401,   571,    10,
      72,    10,  1013,  1013,  1013,  1013,  1379,    72,   -92,    10,
      10,    72,  1013,   -91,    72,    72,    10,    72,   -88,   -83,
     -78,    10,   826,   745,   -53,    72,    37,  1427,   392,   392,
      55,  1144,  -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,
    -315,  -315,  -315,  -315,  -315,  -315,    60,  -315,  -315,  -315,
    -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,
     826,  -315,   -44,    38,  -315,  -315,  -315,  -315,  -315,  -315,
      72,  -315,  -315,   773,   773,  -315,  -315,   826,  -315,   826,
    -315,  -315,    95,   246,    95,  -315,  -315,  -315,  -315,  -315,
     745,  -315,  -315,  -315,   -53,   -53,   -53,  -315,  -315,  -315,
     -53,   -53,    72,   -53,  -315,    87,   128,  -315,    41,    72,
    -315,   -53,  -315,    72,  -315,   102,  -315,   120,  -315,    72,
    -315,   -53,    72,    72,  -315,   185,  -315,    72,    78,    23,
     171,  -315,    72,  -315,   826,  -315,   826,    38,    72,  -315,
    -315,    72,  -315,  -315,   -53,    72,    72,  -315,    72,    72,
    -315,    61,    72,   -53,    72,   -53,   -53,    72,   -53,  -315,
      72,    72,  -315,    72,  -315,    72,  -315,    72,  -315,    72,
    -315,    72,   596,  -315,    72,    95,    95,  -315,  -315,    72,
      72,  -315,  -315,   745,  -315,  -315,    72,    72,   -16,   826,
      38,  -315,   826,   826,  -315,  1379,   859,   795,  -315,  -315,
    -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,
    -315,  -315,  -315,  -315,  -315,  -315,    29,  -315,    72,    72,
    -315,  -315,  -315,    20,  -315,   826,   826,   826,   826,  -315,
    -315,    34,   869,    38,    38,  -315,   266,    75,  1136,  1297,
    1341,  -315,   826,   275,  1379,   659,   596,  1379,  -315,   795,
     795,   291,  -315,  -315,  -315,  1360,  -315,  1242,  -315,  -315,
    1379,  -315,  -315,  -315,  1242,    72,     1,  -315,    17,  -315,
      38,    38,  -315,  -315,  -315,  1360,  -315,  -315,  -315,  -315,
    -315,    27,  -315,    10,  -315,    10,    30,  -315,    54,  -315,
    -315,  -315,  -315,  -315,  -315,  -315,  -315,  1406,  -315,  -315,
    -315,   266,  1317,  -315,  -315,  -315,   826,  -315,  -315,    72,
      38,  -315,    38,    38,    96,    65,    69,    88,    89,   111,
     112,   -82,  -315,   -93,  -315,  -315,  -315,  -315,  -113,  -113,
    -315,  -315,    72,  1379,    72,  -315,  -315,    72,  -315,    72,
    -315,    72,    38,  -315,  -315,   200,  -315,  -315,  -315,  -315,
    -315,  1406,    72,  -315,  -315,    72,  1242,  -315,    22,  -315,
    -315,    72,    22,  -315,    72,    81,    90,    72,  -315,  -315,
      72,    72,    72,    72,    72,    72,  -315,   596,    72,  -315,
      72,    38,  -315,  -315,  -315,  -315,    72,  -315,  -315,  -315,
    -315,  -315,  -315,  -315,   113,   121,  -315,  -315,  -315,  -315,
    -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,
      14,   944,  -315,  -315,  -315,  -315,    17,    17,  -315,  -315,
    -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,
     258,   127,  -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,
    -315,  -315,  -315,  -315
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -315,  -315,  -315,   296,   158,  -315,  -315,  -315,  -315,  -315,
    -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,  -315,    46,
    -315,    21,   -56,  -315,   -17,  -315,   -58,   410,    -3,    -7,
    -272,   351,    -6,   530,  -273,  -315,   618,  -304,  -315,   -52,
    -179,  -315,  -142,  -315,  -315,  -315,    -1,  -315,  -314,  -315,
    -315
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -168
static const yytype_int16 yytable[] =
{
     116,   155,   127,   129,   416,   116,   116,   144,   150,   151,
     412,   130,   446,   447,   387,   482,   117,   171,   167,   169,
     444,   376,   400,   117,   315,   445,   400,   184,   186,   182,
     277,   278,   133,   134,   116,  -162,   189,   118,   119,   117,
     235,   230,   117,   237,   118,   119,   172,   173,   238,   215,
     217,   219,   221,   117,   125,   270,  -162,  -162,   245,   229,
     118,   119,   117,   118,   119,   202,   204,   242,   243,   117,
     211,   116,   213,   117,   118,   119,   266,   289,   290,   428,
     225,   226,   272,   118,   119,   195,   196,   233,   198,   274,
     118,   119,   239,   314,   118,   119,   285,   117,    97,    98,
      99,   100,   460,  -157,   224,   101,   102,   103,   300,   116,
     359,   104,   105,   106,   107,   108,   483,  -157,   118,   119,
     436,   117,   116,   116,  -157,  -157,   116,   414,   116,   117,
     125,   116,   116,   116,   292,   307,   373,   131,   293,   294,
     295,   125,   118,   119,   296,   297,    77,   299,   461,  -167,
     118,   119,   486,   487,   120,   305,   461,   485,   421,   294,
     424,   275,   276,   277,   278,   310,   377,   438,   125,   351,
     352,   439,   117,   316,   274,   275,   276,   277,   278,   275,
     276,   277,   278,   116,   425,   116,  -155,   159,   325,   466,
     440,   441,  -167,   118,   119,   331,   189,   333,   467,   335,
     336,   117,   338,    97,    98,    99,   100,  -155,  -155,   125,
     101,   102,   103,   442,   443,   117,   275,   276,   277,   278,
     480,   349,   118,   119,   116,   116,   481,   356,   109,   493,
     104,   105,   106,   107,   108,   301,   118,   119,   116,   271,
     110,   116,   116,   404,   401,   371,   372,   288,   475,    97,
      98,    99,   100,   456,   130,     0,   101,   102,   103,     0,
       0,     0,   104,   105,   106,   107,   108,  -162,     0,   104,
     105,   106,   107,   108,   116,   116,   116,   116,    97,    98,
      99,   100,   389,     0,   155,   101,   102,   103,  -162,  -162,
     407,   116,   397,     0,     0,   349,     0,   407,     0,   403,
     -57,   122,   405,     0,   126,   128,   410,   104,   105,   106,
     107,   108,     0,   410,   154,   160,   162,   166,   168,   170,
       0,     0,   420,   174,   176,     0,   393,     0,   181,   183,
     185,     0,     0,   190,   193,     0,     0,   197,     0,   200,
     394,   427,     0,     0,   209,   430,   212,   109,   214,   216,
     218,   220,     0,   223,   422,   116,   423,   227,   228,     0,
     231,   232,   488,   234,   489,   145,   145,   145,   152,   241,
       0,   244,     0,   125,     0,   145,   490,   491,   492,   109,
     131,     0,   180,   104,   105,   106,   107,   108,     0,   407,
       0,   110,     0,   135,   397,   427,   136,   206,     0,     0,
       0,   393,   137,   138,   139,   410,     0,   222,     0,   279,
       0,     0,   117,     0,     0,   394,   280,   104,   105,   106,
     107,   108,     0,     0,   109,     0,   349,   104,   105,   106,
     107,   108,     0,   118,   119,     0,   291,     0,   177,   178,
     179,     0,     0,     0,   407,     0,     0,     0,   298,     0,
       0,     0,   302,     0,   303,   304,     0,     0,     0,   306,
     410,     0,     0,   308,     0,   309,     0,     0,   311,   312,
       0,     0,     0,   313,     0,     0,   317,     0,   319,     0,
       0,     0,     0,   322,   323,     0,     0,   324,   268,   269,
       0,   326,   327,     0,   328,   329,     0,   330,   332,     0,
     334,     0,     0,   337,     0,     0,   339,   340,     0,   341,
       0,   342,     0,   343,     0,   344,     0,   345,     0,     0,
     350,     0,     0,     0,     0,   353,   354,     0,     0,   355,
       0,   207,   357,   358,     0,     0,   361,     0,     0,     0,
       0,     0,   199,     0,   146,   146,   146,   153,   158,     0,
     146,   146,   146,     0,   146,     0,     0,     0,     0,     0,
       0,   153,   146,     0,   374,   375,   188,     0,     0,     0,
       0,     0,   117,   201,   203,   205,   153,   208,   210,   385,
     386,     0,   146,   146,   146,   146,   153,   104,   105,   106,
     107,   108,   146,   118,   119,     0,   364,     0,     0,    97,
      98,    99,   100,     0,     0,     0,   101,   102,   103,     0,
       0,   413,     0,     0,     0,     0,   418,   419,     0,   113,
       0,     0,     0,     0,   123,   124,     0,     0,   175,     0,
       0,   136,     0,   384,     0,     0,     0,   137,   138,   139,
     145,   391,     0,   426,     0,   398,     0,     0,   402,   118,
     119,     0,     0,   187,     0,   432,   433,     0,   434,   435,
     437,   411,    97,    98,    99,   100,     0,     0,   399,   101,
     102,   103,     0,     0,     0,     0,     0,     0,   448,     0,
     450,     0,     0,   451,     0,   452,     0,   453,   454,     0,
     240,   455,     0,     0,     0,     0,     0,   457,   458,     0,
       0,   459,     0,   145,   462,     0,     0,   463,   464,     0,
     465,     0,     0,   468,     0,     0,   469,   470,   471,   472,
     473,   474,   346,     0,   476,     0,   477,   478,   273,     0,
       0,     0,   479,     0,   449,     0,     0,     0,     0,     0,
       0,   281,   282,     0,     0,   283,   117,   284,  -167,  -167,
    -167,  -167,     0,     0,     0,  -167,  -167,  -167,     0,     0,
       0,  -167,  -167,  -167,  -167,  -167,     0,   118,   119,     0,
       0,     0,     0,     0,     0,   153,    97,    98,    99,   100,
       0,     0,     0,   101,   102,   103,     0,     0,     0,   104,
     105,   106,   107,   108,     0,     0,     0,     0,    97,    98,
      99,   100,   320,     0,   321,   101,   102,   103,     0,     0,
       0,     0,   153,     0,     0,     0,     0,     0,     0,   146,
     153,     0,     0,     0,   153,     0,     0,   153,     0,    97,
      98,    99,   100,     0,     0,   146,   101,   102,   103,     0,
     153,     0,   104,   105,   106,   107,   108,     0,     0,     0,
       0,     0,   125,     0,     0,   146,     0,   360,     0,     0,
     362,   363,    97,    98,    99,   100,     0,     0,     0,   101,
     102,   103,    97,    98,    99,   100,     0,     0,  -167,   101,
     102,   103,   146,     0,     0,   104,   105,   106,   107,   108,
    -167,     0,     0,   378,   379,   380,   381,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   109,     0,     0,     0,
     392,     0,     0,   153,   275,   276,   277,   278,   110,   117,
       0,    97,    98,    99,   100,     0,     0,   142,   101,   102,
     103,     0,     0,     0,   104,   105,   106,   107,   108,     0,
     118,   119,     0,     0,     0,   484,     0,    97,    98,    99,
     100,     0,     0,     0,   101,   102,   103,     0,     0,   109,
     104,   105,   106,   107,   108,     0,     0,     0,     0,     0,
       0,   110,     0,     0,   431,   365,   366,   367,   368,   369,
     370,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   143,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     275,   276,   277,   278,   117,     0,    97,    98,    99,   100,
       0,     0,   142,   101,   102,   103,     0,     0,     0,   104,
     105,   106,   107,   108,     0,   118,   119,     1,     0,     0,
       0,     0,     0,     2,     0,   406,     0,     0,   143,     0,
       0,     0,     3,   161,     0,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
       0,     0,    19,    20,    21,    22,    23,   109,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,     0,     0,     0,     0,     0,     0,  -146,     0,  -146,
    -146,  -146,  -146,   143,     0,  -146,  -146,  -146,  -146,     0,
       0,     1,  -146,  -146,  -146,  -146,  -146,     2,     0,    77,
       0,     0,     0,     0,     0,     0,     0,    78,    79,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,     0,     0,    19,    20,    21,    22,
      23,     0,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    97,    98,    99,   100,     0,
       0,     0,   101,   102,   103,     0,     0,     0,   104,   105,
     106,   107,   108,     0,     0,     0,  -146,     0,     0,     0,
       0,     0,     0,     0,     0,   117,     0,  -148,  -148,  -148,
    -148,    78,    79,  -148,  -148,  -148,  -148,     0,   192,     0,
    -148,  -148,  -148,  -148,  -148,     0,   118,   119,   388,     0,
      97,    98,    99,   100,     0,     0,   142,   101,   102,   103,
       0,     0,     0,   104,   105,   106,   107,   108,   429,     0,
      97,    98,    99,   100,     0,     0,   142,   101,   102,   103,
       0,     0,     0,   104,   105,   106,   107,   108,     0,     0,
       0,     0,   390,   406,    97,    98,    99,   100,     0,     0,
       0,   101,   102,   103,     0,     0,     0,   104,   105,   106,
     107,   108,     0,    97,    98,    99,   100,     0,     0,   142,
     101,   102,   103,     0,     0,   109,   104,   105,   106,   107,
     108,  -148,    97,    98,    99,   100,     0,     0,     0,   101,
     102,   103,     0,     0,     0,   104,   105,   106,   107,   108,
       0,     0,     0,     0,  -148,     0,     0,   117,     0,  -167,
    -167,  -167,  -167,     0,     0,     0,  -167,  -167,  -167,     0,
       0,     0,     0,     0,     0,     0,     0,   143,   118,   119,
      97,    98,    99,   100,     0,     0,     0,   101,   102,   103,
       0,     0,     0,     0,     0,     0,     0,   143,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   143,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     143,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   143,
       0,     0,     0,   125,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   246,     0,
     247,   248,  -167,     0,     0,     0,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265
};

static const yytype_int16 yycheck[] =
{
       1,    18,     8,     9,   318,     6,     7,    14,    15,    16,
     314,     1,   105,   106,   286,     1,     1,    24,    21,    22,
     102,     1,   295,     1,     1,   107,   299,    33,    34,    32,
     143,   144,    11,    12,    35,     1,   128,    22,    23,     1,
     128,   132,     1,   126,    22,    23,    25,    26,   126,    52,
      53,    54,    55,     1,   107,     0,    22,    23,    21,    62,
      22,    23,     1,    22,    23,    44,    45,    73,    74,     1,
      49,    72,    51,     1,    22,    23,    77,   133,   134,   351,
      59,    60,    22,    22,    23,    39,    40,    66,    42,   133,
      22,    23,    71,    15,    22,    23,     1,     1,     3,     4,
       5,     6,   406,     1,    58,    10,    11,    12,    21,   110,
     126,    16,    17,    18,    19,    20,   102,    15,    22,    23,
      24,     1,   123,   124,    22,    23,   127,   126,   129,     1,
     107,   132,   133,   134,   140,    15,   107,   127,   144,   145,
     146,   107,    22,    23,   150,   151,   129,   153,   134,   126,
      22,    23,   466,   467,   139,   161,   134,   461,   131,   165,
     130,   141,   142,   143,   144,   171,   146,   102,   107,   225,
     226,   102,     1,   179,   133,   141,   142,   143,   144,   141,
     142,   143,   144,   184,   130,   186,     1,   135,   194,   108,
     102,   102,   131,    22,    23,   201,   128,   203,   108,   205,
     206,     1,   208,     3,     4,     5,     6,    22,    23,   107,
      10,    11,    12,   102,   102,     1,   141,   142,   143,   144,
     107,   222,    22,    23,   225,   226,   105,   233,   133,   102,
      16,    17,    18,    19,    20,   107,    22,    23,   239,    81,
     145,   242,   243,   301,   296,   246,   247,     1,   427,     3,
       4,     5,     6,   395,     1,    -1,    10,    11,    12,    -1,
      -1,    -1,    16,    17,    18,    19,    20,     1,    -1,    16,
      17,    18,    19,    20,   275,   276,   277,   278,     3,     4,
       5,     6,   289,    -1,   301,    10,    11,    12,    22,    23,
     307,   292,   293,    -1,    -1,   296,    -1,   314,    -1,   300,
     129,     5,   305,    -1,     8,     9,   307,    16,    17,    18,
      19,    20,    -1,   314,    18,    19,    20,    21,    22,    23,
      -1,    -1,   325,    27,    28,    -1,   126,    -1,    32,    33,
      34,    -1,    -1,    37,    38,    -1,    -1,    41,    -1,    43,
     140,   347,    -1,    -1,    48,   352,    50,   133,    52,    53,
      54,    55,    -1,    57,   333,   356,   335,    61,    62,    -1,
      64,    65,   104,    67,   106,    14,    15,    16,    17,    73,
      -1,    75,    -1,   107,    -1,    24,   118,   119,   120,   133,
     127,    -1,    31,    16,    17,    18,    19,    20,    -1,   406,
      -1,   145,    -1,     1,   395,   401,     4,    46,    -1,    -1,
      -1,   126,    10,    11,    12,   406,    -1,    56,    -1,   113,
      -1,    -1,     1,    -1,    -1,   140,   120,    16,    17,    18,
      19,    20,    -1,    -1,   133,    -1,   427,    16,    17,    18,
      19,    20,    -1,    22,    23,    -1,   140,    -1,    28,    29,
      30,    -1,    -1,    -1,   461,    -1,    -1,    -1,   152,    -1,
      -1,    -1,   156,    -1,   158,   159,    -1,    -1,    -1,   163,
     461,    -1,    -1,   167,    -1,   169,    -1,    -1,   172,   173,
      -1,    -1,    -1,   177,    -1,    -1,   180,    -1,   182,    -1,
      -1,    -1,    -1,   187,   188,    -1,    -1,   191,    78,    79,
      -1,   195,   196,    -1,   198,   199,    -1,   201,   202,    -1,
     204,    -1,    -1,   207,    -1,    -1,   210,   211,    -1,   213,
      -1,   215,    -1,   217,    -1,   219,    -1,   221,    -1,    -1,
     224,    -1,    -1,    -1,    -1,   229,   230,    -1,    -1,   233,
      -1,   130,   236,   237,    -1,    -1,   240,    -1,    -1,    -1,
      -1,    -1,   131,    -1,    14,    15,    16,    17,    18,    -1,
      20,    21,    22,    -1,    24,    -1,    -1,    -1,    -1,    -1,
      -1,    31,    32,    -1,   268,   269,    36,    -1,    -1,    -1,
      -1,    -1,     1,    43,    44,    45,    46,    47,    48,   283,
     284,    -1,    52,    53,    54,    55,    56,    16,    17,    18,
      19,    20,    62,    22,    23,    -1,   245,    -1,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,
      -1,   315,    -1,    -1,    -1,    -1,   320,   321,    -1,     1,
      -1,    -1,    -1,    -1,     6,     7,    -1,    -1,     1,    -1,
      -1,     4,    -1,   282,    -1,    -1,    -1,    10,    11,    12,
     289,   290,    -1,   347,    -1,   294,    -1,    -1,   297,    22,
      23,    -1,    -1,    35,    -1,   359,   360,    -1,   362,   363,
     364,   310,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    -1,    -1,    -1,    -1,    -1,    -1,   382,    -1,
     384,    -1,    -1,   387,    -1,   389,    -1,   391,   392,    -1,
      72,   395,    -1,    -1,    -1,    -1,    -1,   401,   402,    -1,
      -1,   405,    -1,   352,   408,    -1,    -1,   411,   412,    -1,
     414,    -1,    -1,   417,    -1,    -1,   420,   421,   422,   423,
     424,   425,   126,    -1,   428,    -1,   430,   431,   110,    -1,
      -1,    -1,   436,    -1,   383,    -1,    -1,    -1,    -1,    -1,
      -1,   123,   124,    -1,    -1,   127,     1,   129,     3,     4,
       5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,    -1,
      -1,    16,    17,    18,    19,    20,    -1,    22,    23,    -1,
      -1,    -1,    -1,    -1,    -1,   245,     3,     4,     5,     6,
      -1,    -1,    -1,    10,    11,    12,    -1,    -1,    -1,    16,
      17,    18,    19,    20,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,   184,    -1,   186,    10,    11,    12,    -1,    -1,
      -1,    -1,   282,    -1,    -1,    -1,    -1,    -1,    -1,   289,
     290,    -1,    -1,    -1,   294,    -1,    -1,   297,    -1,     3,
       4,     5,     6,    -1,    -1,   305,    10,    11,    12,    -1,
     310,    -1,    16,    17,    18,    19,    20,    -1,    -1,    -1,
      -1,    -1,   107,    -1,    -1,   325,    -1,   239,    -1,    -1,
     242,   243,     3,     4,     5,     6,    -1,    -1,    -1,    10,
      11,    12,     3,     4,     5,     6,    -1,    -1,   133,    10,
      11,    12,   352,    -1,    -1,    16,    17,    18,    19,    20,
     145,    -1,    -1,   275,   276,   277,   278,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   133,    -1,    -1,    -1,
     292,    -1,    -1,   383,   141,   142,   143,   144,   145,     1,
      -1,     3,     4,     5,     6,    -1,    -1,     9,    10,    11,
      12,    -1,    -1,    -1,    16,    17,    18,    19,    20,    -1,
      22,    23,    -1,    -1,    -1,     1,    -1,     3,     4,     5,
       6,    -1,    -1,    -1,    10,    11,    12,    -1,    -1,   133,
      16,    17,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,   145,    -1,    -1,   356,   116,   117,   118,   119,   120,
     121,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   130,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     141,   142,   143,   144,     1,    -1,     3,     4,     5,     6,
      -1,    -1,     9,    10,    11,    12,    -1,    -1,    -1,    16,
      17,    18,    19,    20,    -1,    22,    23,     7,    -1,    -1,
      -1,    -1,    -1,    13,    -1,   101,    -1,    -1,   130,    -1,
      -1,    -1,    22,   135,    -1,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      -1,    -1,    42,    43,    44,    45,    46,   133,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,     3,
       4,     5,     6,   130,    -1,     9,    10,    11,    12,    -1,
      -1,     7,    16,    17,    18,    19,    20,    13,    -1,   129,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   137,   138,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    -1,    -1,    42,    43,    44,    45,
      46,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,     3,     4,     5,     6,    -1,
      -1,    -1,    10,    11,    12,    -1,    -1,    -1,    16,    17,
      18,    19,    20,    -1,    -1,    -1,   130,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     1,    -1,     3,     4,     5,
       6,   137,   138,     9,    10,    11,    12,    -1,    14,    -1,
      16,    17,    18,    19,    20,    -1,    22,    23,     1,    -1,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      -1,    -1,    -1,    16,    17,    18,    19,    20,     1,    -1,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      -1,    -1,    -1,    16,    17,    18,    19,    20,    -1,    -1,
      -1,    -1,     1,   101,     3,     4,     5,     6,    -1,    -1,
      -1,    10,    11,    12,    -1,    -1,    -1,    16,    17,    18,
      19,    20,    -1,     3,     4,     5,     6,    -1,    -1,     9,
      10,    11,    12,    -1,    -1,   133,    16,    17,    18,    19,
      20,   107,     3,     4,     5,     6,    -1,    -1,    -1,    10,
      11,    12,    -1,    -1,    -1,    16,    17,    18,    19,    20,
      -1,    -1,    -1,    -1,   130,    -1,    -1,     1,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   130,    22,    23,
       3,     4,     5,     6,    -1,    -1,    -1,    10,    11,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   130,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   130,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     130,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   130,
      -1,    -1,    -1,   107,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,    -1,
     103,   104,   126,    -1,    -1,    -1,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125
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
      94,    95,    96,    97,    98,    99,   100,   129,   137,   138,
     148,   149,   151,   152,   153,   154,   155,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   195,     3,     4,     5,
       6,    10,    11,    12,    16,    17,    18,    19,    20,   133,
     145,   171,   180,   183,   190,   192,   193,     1,    22,    23,
     139,   150,   150,   183,   183,   107,   150,   179,   150,   179,
       1,   127,   168,   168,   168,     1,     4,    10,    11,    12,
     174,   191,     9,   130,   176,   178,   180,   181,   182,   193,
     176,   176,   178,   180,   150,   171,   172,   173,   180,   135,
     150,   135,   150,   175,   176,   178,   150,   175,   150,   175,
     150,   176,   168,   168,   150,     1,   150,   174,   174,   174,
     178,   150,   175,   150,   179,   150,   179,   183,   180,   128,
     150,   166,    14,   150,   170,   166,   166,   150,   166,   131,
     150,   180,   168,   180,   168,   180,   178,   130,   180,   150,
     180,   168,   150,   168,   150,   175,   150,   175,   150,   175,
     150,   175,   178,   150,   166,   168,   168,   150,   150,   175,
     132,   150,   150,   168,   150,   128,   167,   126,   126,   168,
     183,   150,   179,   179,   150,    21,   101,   103,   104,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   193,   197,   174,   174,
       0,   151,    22,   183,   133,   141,   142,   143,   144,   150,
     150,   183,   183,   183,   183,     1,   169,   183,     1,   169,
     169,   150,   179,   179,   179,   179,   179,   179,   150,   179,
      21,   107,   150,   150,   150,   179,   150,    15,   150,   150,
     179,   150,   150,   150,    15,     1,   179,   150,   156,   150,
     183,   183,   150,   150,   150,   179,   150,   150,   150,   150,
     150,   179,   150,   179,   150,   179,   179,   150,   179,   150,
     150,   150,   150,   150,   150,   150,   126,   186,   187,   193,
     150,   169,   169,   150,   150,   150,   179,   150,   150,   126,
     183,   150,   183,   183,   178,   116,   117,   118,   119,   120,
     121,   193,   193,   107,   150,   150,     1,   146,   183,   183,
     183,   183,   177,   179,   178,   150,   150,   177,     1,   176,
       1,   178,   183,   126,   140,   188,   189,   193,   178,     9,
     181,   186,   178,   193,   173,   175,   101,   171,   184,   185,
     193,   178,   184,   150,   126,   194,   195,   196,   150,   150,
     175,   131,   168,   168,   130,   130,   150,   179,   177,     1,
     176,   183,   150,   150,   150,   150,    24,   150,   102,   102,
     102,   102,   102,   102,   102,   107,   105,   106,   150,   178,
     150,   150,   150,   150,   150,   150,   189,   150,   150,   150,
     184,   134,   150,   150,   150,   150,   108,   108,   150,   150,
     150,   150,   150,   150,   150,   187,   150,   150,   150,   150,
     107,   105,     1,   102,     1,   184,   195,   195,   104,   106,
     118,   119,   120,   102
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
#line 174 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 3:
#line 175 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 4:
#line 176 "mon_parse.y"
    { new_cmd = 1; asm_mode = 0;  (yyval.i) = 0; }
    break;

  case 9:
#line 185 "mon_parse.y"
    { return ERR_EXPECT_END_CMD; }
    break;

  case 22:
#line 200 "mon_parse.y"
    { return ERR_BAD_CMD; }
    break;

  case 23:
#line 204 "mon_parse.y"
    { mon_bank(e_default_space, NULL); }
    break;

  case 24:
#line 206 "mon_parse.y"
    { mon_bank((yyvsp[(2) - (3)].i), NULL); }
    break;

  case 25:
#line 208 "mon_parse.y"
    { mon_bank(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 26:
#line 210 "mon_parse.y"
    { mon_bank((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 27:
#line 212 "mon_parse.y"
    { mon_jump((yyvsp[(2) - (3)].a)); }
    break;

  case 28:
#line 214 "mon_parse.y"
    { mon_display_io_regs(); }
    break;

  case 29:
#line 216 "mon_parse.y"
    { monitor_cpu_type_set((yyvsp[(2) - (3)].str)); }
    break;

  case 30:
#line 218 "mon_parse.y"
    { mon_instruction_return(); }
    break;

  case 31:
#line 220 "mon_parse.y"
    { machine_write_snapshot((yyvsp[(2) - (3)].str),0,0,0); /* FIXME */ }
    break;

  case 32:
#line 222 "mon_parse.y"
    { machine_read_snapshot((yyvsp[(2) - (3)].str), 0); }
    break;

  case 33:
#line 224 "mon_parse.y"
    { mon_instructions_step(-1); }
    break;

  case 34:
#line 226 "mon_parse.y"
    { mon_instructions_step((yyvsp[(3) - (4)].i)); }
    break;

  case 35:
#line 228 "mon_parse.y"
    { mon_instructions_next(-1); }
    break;

  case 36:
#line 230 "mon_parse.y"
    { mon_instructions_next((yyvsp[(3) - (4)].i)); }
    break;

  case 37:
#line 232 "mon_parse.y"
    { mon_stack_up(-1); }
    break;

  case 38:
#line 234 "mon_parse.y"
    { mon_stack_up((yyvsp[(3) - (4)].i)); }
    break;

  case 39:
#line 236 "mon_parse.y"
    { mon_stack_down(-1); }
    break;

  case 40:
#line 238 "mon_parse.y"
    { mon_stack_down((yyvsp[(3) - (4)].i)); }
    break;

  case 41:
#line 240 "mon_parse.y"
    { mon_display_screen(); }
    break;

  case 43:
#line 245 "mon_parse.y"
    { (monitor_cpu_type.mon_register_print)(default_memspace); }
    break;

  case 44:
#line 247 "mon_parse.y"
    { (monitor_cpu_type.mon_register_print)((yyvsp[(2) - (3)].i)); }
    break;

  case 46:
#line 252 "mon_parse.y"
    {
                        /* What about the memspace? */
                        playback = TRUE; playback_name = (yyvsp[(4) - (5)].str);
                        /*mon_load_symbols($2, $3);*/
                    }
    break;

  case 47:
#line 258 "mon_parse.y"
    {
                        /* What about the memspace? */
                        playback = TRUE; playback_name = (yyvsp[(2) - (3)].str);
                        /*mon_load_symbols($2, $3);*/
                    }
    break;

  case 48:
#line 264 "mon_parse.y"
    { mon_save_symbols((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 49:
#line 266 "mon_parse.y"
    { mon_save_symbols(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 50:
#line 268 "mon_parse.y"
    { mon_add_name_to_symbol_table((yyvsp[(2) - (5)].a), (yyvsp[(4) - (5)].str)); }
    break;

  case 51:
#line 270 "mon_parse.y"
    { mon_remove_name_from_symbol_table(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 52:
#line 272 "mon_parse.y"
    { mon_remove_name_from_symbol_table((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 53:
#line 274 "mon_parse.y"
    { mon_print_symbol_table((yyvsp[(2) - (3)].i)); }
    break;

  case 54:
#line 276 "mon_parse.y"
    { mon_print_symbol_table(e_default_space); }
    break;

  case 55:
#line 278 "mon_parse.y"
    {
                        mon_add_name_to_symbol_table((yyvsp[(3) - (4)].a), mon_prepend_dot_to_name((yyvsp[(1) - (4)].str)));
                    }
    break;

  case 56:
#line 282 "mon_parse.y"
    {
                        mon_add_name_to_symbol_table((yyvsp[(3) - (5)].a), mon_prepend_dot_to_name((yyvsp[(1) - (5)].str)));
                    }
    break;

  case 57:
#line 288 "mon_parse.y"
    { mon_start_assemble_mode((yyvsp[(2) - (2)].a), NULL); }
    break;

  case 58:
#line 289 "mon_parse.y"
    { }
    break;

  case 59:
#line 291 "mon_parse.y"
    { mon_start_assemble_mode((yyvsp[(2) - (3)].a), NULL); }
    break;

  case 60:
#line 293 "mon_parse.y"
    { mon_disassemble_lines((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1]); }
    break;

  case 61:
#line 295 "mon_parse.y"
    { mon_disassemble_lines(BAD_ADDR, BAD_ADDR); }
    break;

  case 62:
#line 299 "mon_parse.y"
    { mon_memory_move((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], (yyvsp[(4) - (5)].a)); }
    break;

  case 63:
#line 301 "mon_parse.y"
    { mon_memory_compare((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[0], (yyvsp[(4) - (5)].a)); }
    break;

  case 64:
#line 303 "mon_parse.y"
    { mon_memory_fill((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1],(unsigned char *)(yyvsp[(4) - (5)].str)); }
    break;

  case 65:
#line 305 "mon_parse.y"
    { mon_memory_hunt((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1],(unsigned char *)(yyvsp[(4) - (5)].str)); }
    break;

  case 66:
#line 307 "mon_parse.y"
    { mon_memory_display((yyvsp[(2) - (5)].rt), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], DF_PETSCII); }
    break;

  case 67:
#line 309 "mon_parse.y"
    { mon_memory_display(default_radix, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_PETSCII); }
    break;

  case 68:
#line 311 "mon_parse.y"
    { mon_memory_display(default_radix, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
    break;

  case 69:
#line 313 "mon_parse.y"
    { mon_memory_display_data((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], 8, 8); }
    break;

  case 70:
#line 315 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 8, 8); }
    break;

  case 71:
#line 317 "mon_parse.y"
    { mon_memory_display_data((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], 24, 21); }
    break;

  case 72:
#line 319 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 24, 21); }
    break;

  case 73:
#line 321 "mon_parse.y"
    { mon_memory_display(0, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_PETSCII); }
    break;

  case 74:
#line 323 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
    break;

  case 75:
#line 325 "mon_parse.y"
    { mon_memory_display(0, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_SCREEN_CODE); }
    break;

  case 76:
#line 327 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_SCREEN_CODE); }
    break;

  case 77:
#line 331 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], FALSE, FALSE,
                                                    FALSE, FALSE);
                  }
    break;

  case 78:
#line 336 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], FALSE, FALSE,
                                                    FALSE, TRUE);
                  }
    break;

  case 79:
#line 341 "mon_parse.y"
    {
                      temp = mon_breakpoint_add_checkpoint((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], FALSE,
                                                           FALSE, FALSE, FALSE);
                      mon_breakpoint_set_checkpoint_condition(temp, (yyvsp[(4) - (5)].cond_node));
                  }
    break;

  case 80:
#line 347 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], FALSE,
                      ((yyvsp[(2) - (5)].i) == e_load || (yyvsp[(2) - (5)].i) == e_load_store),
                      ((yyvsp[(2) - (5)].i) == e_store || (yyvsp[(2) - (5)].i) == e_load_store), FALSE);
                  }
    break;

  case 81:
#line 353 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], TRUE, FALSE, FALSE,
                                                    FALSE);
                  }
    break;

  case 82:
#line 358 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 83:
#line 360 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 84:
#line 362 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 85:
#line 364 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 86:
#line 369 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_ON, (yyvsp[(2) - (3)].i)); }
    break;

  case 87:
#line 371 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_OFF, (yyvsp[(2) - (3)].i)); }
    break;

  case 88:
#line 373 "mon_parse.y"
    { mon_breakpoint_set_ignore_count((yyvsp[(2) - (3)].i), -1); }
    break;

  case 89:
#line 375 "mon_parse.y"
    { mon_breakpoint_set_ignore_count((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].i)); }
    break;

  case 90:
#line 377 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint((yyvsp[(2) - (3)].i)); }
    break;

  case 91:
#line 379 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint(-1); }
    break;

  case 92:
#line 381 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_condition((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].cond_node)); }
    break;

  case 93:
#line 383 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_command((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 94:
#line 385 "mon_parse.y"
    { return ERR_EXPECT_STRING; }
    break;

  case 95:
#line 389 "mon_parse.y"
    { sidefx = (((yyvsp[(2) - (3)].action) == e_TOGGLE) ? (sidefx ^ 1) : (yyvsp[(2) - (3)].action)); }
    break;

  case 96:
#line 391 "mon_parse.y"
    {
                         mon_out("I/O side effects are %s\n",
                                   sidefx ? "enabled" : "disabled");
                     }
    break;

  case 97:
#line 396 "mon_parse.y"
    { default_radix = (yyvsp[(2) - (3)].rt); }
    break;

  case 98:
#line 398 "mon_parse.y"
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

  case 99:
#line 416 "mon_parse.y"
    {
                         mon_out("Setting default device to `%s'\n",
                         _mon_space_strings[(int) (yyvsp[(2) - (3)].i)]); default_memspace = (yyvsp[(2) - (3)].i);
                     }
    break;

  case 100:
#line 421 "mon_parse.y"
    { mon_quit(); YYACCEPT; }
    break;

  case 101:
#line 423 "mon_parse.y"
    { exit_mon = 1; YYACCEPT; }
    break;

  case 102:
#line 427 "mon_parse.y"
    { mon_drive_execute_disk_cmd((yyvsp[(2) - (3)].str)); }
    break;

  case 103:
#line 429 "mon_parse.y"
    { mon_out("\t%d\n",(yyvsp[(2) - (3)].i)); }
    break;

  case 104:
#line 431 "mon_parse.y"
    { mon_command_print_help(NULL); }
    break;

  case 105:
#line 433 "mon_parse.y"
    { mon_command_print_help((yyvsp[(2) - (3)].str)); }
    break;

  case 106:
#line 435 "mon_parse.y"
    { printf("SYSTEM COMMAND: %s\n",(yyvsp[(2) - (3)].str)); }
    break;

  case 107:
#line 437 "mon_parse.y"
    { mon_print_convert((yyvsp[(2) - (3)].i)); }
    break;

  case 108:
#line 439 "mon_parse.y"
    { mon_change_dir((yyvsp[(2) - (3)].str)); }
    break;

  case 109:
#line 441 "mon_parse.y"
    { mon_keyboard_feed((yyvsp[(2) - (3)].str)); }
    break;

  case 110:
#line 443 "mon_parse.y"
    { mon_backtrace(); }
    break;

  case 111:
#line 445 "mon_parse.y"
    { mon_show_dir((yyvsp[(2) - (3)].str)); }
    break;

  case 112:
#line 447 "mon_parse.y"
    { mon_show_pwd(); }
    break;

  case 113:
#line 449 "mon_parse.y"
    { mon_screenshot_save((yyvsp[(2) - (3)].str),-1); }
    break;

  case 114:
#line 451 "mon_parse.y"
    { mon_screenshot_save((yyvsp[(2) - (5)].str),(yyvsp[(4) - (5)].i)); }
    break;

  case 115:
#line 453 "mon_parse.y"
    { mon_resource_get((yyvsp[(2) - (3)].str)); }
    break;

  case 116:
#line 455 "mon_parse.y"
    { mon_resource_set((yyvsp[(2) - (4)].str),(yyvsp[(3) - (4)].str)); }
    break;

  case 117:
#line 457 "mon_parse.y"
    { mon_reset_machine(-1); }
    break;

  case 118:
#line 459 "mon_parse.y"
    { mon_reset_machine((yyvsp[(3) - (4)].i)); }
    break;

  case 119:
#line 461 "mon_parse.y"
    { mon_tape_ctrl((yyvsp[(3) - (4)].i)); }
    break;

  case 120:
#line 463 "mon_parse.y"
    { mon_cart_freeze(); }
    break;

  case 121:
#line 468 "mon_parse.y"
    { mon_file_load((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].a), FALSE); }
    break;

  case 122:
#line 470 "mon_parse.y"
    { mon_file_load((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].a), TRUE); }
    break;

  case 123:
#line 472 "mon_parse.y"
    { mon_file_save((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], FALSE); }
    break;

  case 124:
#line 474 "mon_parse.y"
    { return ERR_EXPECT_DEVICE_NUM; }
    break;

  case 125:
#line 476 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 126:
#line 478 "mon_parse.y"
    { mon_file_save((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], TRUE); }
    break;

  case 127:
#line 480 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 128:
#line 482 "mon_parse.y"
    { mon_file_verify((yyvsp[(2) - (5)].str),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 129:
#line 484 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 130:
#line 486 "mon_parse.y"
    { mon_drive_block_cmd(0,(yyvsp[(2) - (5)].i),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 131:
#line 488 "mon_parse.y"
    { mon_drive_block_cmd(1,(yyvsp[(2) - (5)].i),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 132:
#line 490 "mon_parse.y"
    { mon_attach((yyvsp[(2) - (4)].str),(yyvsp[(3) - (4)].i)); }
    break;

  case 133:
#line 492 "mon_parse.y"
    { mon_detach((yyvsp[(2) - (3)].i)); }
    break;

  case 134:
#line 496 "mon_parse.y"
    { mon_record_commands((yyvsp[(2) - (3)].str)); }
    break;

  case 135:
#line 498 "mon_parse.y"
    { mon_end_recording(); }
    break;

  case 136:
#line 500 "mon_parse.y"
    { playback=TRUE; playback_name = (yyvsp[(2) - (3)].str); }
    break;

  case 137:
#line 504 "mon_parse.y"
    { mon_memory_fill((yyvsp[(2) - (4)].a), BAD_ADDR, (unsigned char *)(yyvsp[(3) - (4)].str)); }
    break;

  case 138:
#line 506 "mon_parse.y"
    { printf("Not yet.\n"); }
    break;

  case 139:
#line 510 "mon_parse.y"
    { yydebug = 1; }
    break;

  case 140:
#line 513 "mon_parse.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 141:
#line 516 "mon_parse.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 142:
#line 517 "mon_parse.y"
    { (yyval.str) = NULL; }
    break;

  case 144:
#line 521 "mon_parse.y"
    { return ERR_EXPECT_FILENAME; }
    break;

  case 146:
#line 525 "mon_parse.y"
    { return ERR_EXPECT_DEVICE_NUM; }
    break;

  case 147:
#line 528 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 148:
#line 529 "mon_parse.y"
    { (yyval.i) = e_load_store; }
    break;

  case 149:
#line 532 "mon_parse.y"
    { (yyval.i) = new_reg(default_memspace, (yyvsp[(1) - (1)].reg)); }
    break;

  case 150:
#line 533 "mon_parse.y"
    { (yyval.i) = new_reg((yyvsp[(1) - (2)].i), (yyvsp[(2) - (2)].reg)); }
    break;

  case 153:
#line 541 "mon_parse.y"
    { (monitor_cpu_type.mon_register_set_val)(reg_memspace((yyvsp[(1) - (3)].i)), reg_regid((yyvsp[(1) - (3)].i)), (WORD) (yyvsp[(3) - (3)].i)); }
    break;

  case 154:
#line 544 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 155:
#line 545 "mon_parse.y"
    { return ERR_EXPECT_BRKNUM; }
    break;

  case 157:
#line 549 "mon_parse.y"
    { (yyval.range)[0] = (yyvsp[(1) - (1)].a); (yyval.range)[1] = BAD_ADDR; }
    break;

  case 158:
#line 552 "mon_parse.y"
    { (yyval.range)[0] = (yyvsp[(1) - (3)].a); (yyval.range)[1] = (yyvsp[(3) - (3)].a); }
    break;

  case 159:
#line 554 "mon_parse.y"
    { if (resolve_range(e_default_space, (yyval.range), (yyvsp[(1) - (1)].str))) return ERR_ADDR_TOO_BIG; }
    break;

  case 160:
#line 556 "mon_parse.y"
    { if (resolve_range((yyvsp[(1) - (3)].i), (yyval.range), (yyvsp[(3) - (3)].str))) return ERR_ADDR_TOO_BIG; }
    break;

  case 161:
#line 559 "mon_parse.y"
    { (yyval.a) = (yyvsp[(2) - (2)].a); }
    break;

  case 162:
#line 560 "mon_parse.y"
    { (yyval.a) = BAD_ADDR; }
    break;

  case 163:
#line 564 "mon_parse.y"
    {
             (yyval.a) = new_addr(e_default_space,(yyvsp[(1) - (1)].i));
             if (opt_asm) new_cmd = asm_mode = 1; 
         }
    break;

  case 164:
#line 569 "mon_parse.y"
    {
             (yyval.a) = new_addr((yyvsp[(1) - (3)].i), (yyvsp[(3) - (3)].i));
             if (opt_asm) new_cmd = asm_mode = 1; 
         }
    break;

  case 165:
#line 574 "mon_parse.y"
    {
             temp = mon_symbol_table_lookup_addr(e_default_space, (yyvsp[(1) - (1)].str));
             if (temp >= 0)
                 (yyval.a) = new_addr(e_default_space, temp);
             else
                 return ERR_UNDEFINED_LABEL;
         }
    break;

  case 168:
#line 587 "mon_parse.y"
    { (yyval.i) = e_comp_space; }
    break;

  case 169:
#line 588 "mon_parse.y"
    { (yyval.i) = e_disk8_space; }
    break;

  case 170:
#line 589 "mon_parse.y"
    { (yyval.i) = e_disk9_space; }
    break;

  case 171:
#line 590 "mon_parse.y"
    { (yyval.i) = e_disk10_space; }
    break;

  case 172:
#line 591 "mon_parse.y"
    { (yyval.i) = e_disk11_space; }
    break;

  case 173:
#line 594 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); if (!CHECK_ADDR((yyvsp[(1) - (1)].i))) return ERR_ADDR_TOO_BIG; }
    break;

  case 174:
#line 596 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 175:
#line 598 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) + (yyvsp[(3) - (3)].i); }
    break;

  case 176:
#line 599 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) - (yyvsp[(3) - (3)].i); }
    break;

  case 177:
#line 600 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) * (yyvsp[(3) - (3)].i); }
    break;

  case 178:
#line 601 "mon_parse.y"
    { (yyval.i) = ((yyvsp[(3) - (3)].i)) ? ((yyvsp[(1) - (3)].i) / (yyvsp[(3) - (3)].i)) : 1; }
    break;

  case 179:
#line 602 "mon_parse.y"
    { (yyval.i) = (yyvsp[(2) - (3)].i); }
    break;

  case 180:
#line 603 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 181:
#line 604 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 182:
#line 608 "mon_parse.y"
    {
               (yyval.cond_node) = new_cond; (yyval.cond_node)->is_parenthized = FALSE;
               (yyval.cond_node)->child1 = (yyvsp[(1) - (3)].cond_node); (yyval.cond_node)->child2 = (yyvsp[(3) - (3)].cond_node); (yyval.cond_node)->operation = (yyvsp[(2) - (3)].cond_op);
           }
    break;

  case 183:
#line 613 "mon_parse.y"
    { return ERR_INCOMPLETE_COMPARE_OP; }
    break;

  case 184:
#line 615 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(2) - (3)].cond_node); (yyval.cond_node)->is_parenthized = TRUE; }
    break;

  case 185:
#line 617 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 186:
#line 619 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(1) - (1)].cond_node); }
    break;

  case 187:
#line 622 "mon_parse.y"
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->reg_num = (yyvsp[(1) - (1)].i); (yyval.cond_node)->is_reg = TRUE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
    break;

  case 188:
#line 628 "mon_parse.y"
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->value = (yyvsp[(1) - (1)].i); (yyval.cond_node)->is_reg = FALSE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
    break;

  case 191:
#line 640 "mon_parse.y"
    { mon_add_number_to_buffer((yyvsp[(1) - (1)].i)); }
    break;

  case 192:
#line 641 "mon_parse.y"
    { mon_add_string_to_buffer((yyvsp[(1) - (1)].str)); }
    break;

  case 195:
#line 648 "mon_parse.y"
    { mon_add_number_to_buffer((yyvsp[(1) - (1)].i)); }
    break;

  case 196:
#line 649 "mon_parse.y"
    { mon_add_number_masked_to_buffer((yyvsp[(1) - (1)].i), 0x00); }
    break;

  case 197:
#line 650 "mon_parse.y"
    { mon_add_string_to_buffer((yyvsp[(1) - (1)].str)); }
    break;

  case 198:
#line 653 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 199:
#line 654 "mon_parse.y"
    { (yyval.i) = (monitor_cpu_type.mon_register_get_val)(reg_memspace((yyvsp[(1) - (1)].i)), reg_regid((yyvsp[(1) - (1)].i))); }
    break;

  case 200:
#line 657 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 201:
#line 658 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 202:
#line 659 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 203:
#line 660 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 204:
#line 663 "mon_parse.y"
    { (yyval.i) = resolve_datatype(B_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 205:
#line 664 "mon_parse.y"
    { (yyval.i) = resolve_datatype(O_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 206:
#line 665 "mon_parse.y"
    { (yyval.i) = resolve_datatype(D_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 207:
#line 668 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 208:
#line 669 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 209:
#line 670 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 210:
#line 671 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 211:
#line 672 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 215:
#line 680 "mon_parse.y"
    { (yyval.i) = 0;
                                                if ((yyvsp[(1) - (2)].str)) {
                                                    (monitor_cpu_type.mon_assemble_instr)((yyvsp[(1) - (2)].str), (yyvsp[(2) - (2)].i));
                                                } else {
                                                    new_cmd = 1;
                                                    asm_mode = 0;
                                                }
                                                opt_asm = 0;
                                              }
    break;

  case 217:
#line 691 "mon_parse.y"
    { asm_mode = 0; }
    break;

  case 218:
#line 694 "mon_parse.y"
    { if ((yyvsp[(2) - (2)].i) > 0xff)
                          (yyval.i) = join_ints(ASM_ADDR_MODE_IMMEDIATE_16,(yyvsp[(2) - (2)].i));
                        else
                          (yyval.i) = join_ints(ASM_ADDR_MODE_IMMEDIATE,(yyvsp[(2) - (2)].i)); }
    break;

  case 219:
#line 698 "mon_parse.y"
    { if ((yyvsp[(1) - (1)].i) < 0x100)
               (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE,(yyvsp[(1) - (1)].i));
             else
               (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE,(yyvsp[(1) - (1)].i));
           }
    break;

  case 220:
#line 703 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100)
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE_X,(yyvsp[(1) - (3)].i));
                          else
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_X,(yyvsp[(1) - (3)].i));
                        }
    break;

  case 221:
#line 708 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100)
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE_Y,(yyvsp[(1) - (3)].i));
                          else
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_Y,(yyvsp[(1) - (3)].i));
                        }
    break;

  case 222:
#line 714 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABS_INDIRECT,(yyvsp[(2) - (3)].i)); }
    break;

  case 223:
#line 716 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_INDIRECT_X,(yyvsp[(2) - (5)].i)); }
    break;

  case 224:
#line 718 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_INDIRECT_Y,(yyvsp[(2) - (5)].i)); }
    break;

  case 225:
#line 719 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_BC,0); }
    break;

  case 226:
#line 720 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_DE,0); }
    break;

  case 227:
#line 721 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_HL,0); }
    break;

  case 228:
#line 722 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_IX,0); }
    break;

  case 229:
#line 723 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_IY,0); }
    break;

  case 230:
#line 724 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_SP,0); }
    break;

  case 231:
#line 726 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_A,(yyvsp[(2) - (5)].i)); }
    break;

  case 232:
#line 728 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_HL,(yyvsp[(2) - (5)].i)); }
    break;

  case 233:
#line 730 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_IX,(yyvsp[(2) - (5)].i)); }
    break;

  case 234:
#line 732 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_IY,(yyvsp[(2) - (5)].i)); }
    break;

  case 235:
#line 733 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_IMPLIED,0); }
    break;

  case 236:
#line 734 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ACCUMULATOR,0); }
    break;

  case 237:
#line 735 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_B,0); }
    break;

  case 238:
#line 736 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_C,0); }
    break;

  case 239:
#line 737 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_D,0); }
    break;

  case 240:
#line 738 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_E,0); }
    break;

  case 241:
#line 739 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_H,0); }
    break;

  case 242:
#line 740 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IXH,0); }
    break;

  case 243:
#line 741 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IYH,0); }
    break;

  case 244:
#line 742 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_L,0); }
    break;

  case 245:
#line 743 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IXL,0); }
    break;

  case 246:
#line 744 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IYL,0); }
    break;

  case 247:
#line 745 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_AF,0); }
    break;

  case 248:
#line 746 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_BC,0); }
    break;

  case 249:
#line 747 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_DE,0); }
    break;

  case 250:
#line 748 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_HL,0); }
    break;

  case 251:
#line 749 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IX,0); }
    break;

  case 252:
#line 750 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IY,0); }
    break;

  case 253:
#line 751 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_SP,0); }
    break;


/* Line 1267 of yacc.c.  */
#line 3609 "mon_parse.c"
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


#line 755 "mon_parse.y"


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


