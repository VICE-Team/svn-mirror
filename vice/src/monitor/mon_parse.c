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
     CMD_LABEL_ASGN = 344,
     L_PAREN = 345,
     R_PAREN = 346,
     ARG_IMMEDIATE = 347,
     REG_A = 348,
     REG_X = 349,
     REG_Y = 350,
     COMMA = 351,
     INST_SEP = 352,
     REG_B = 353,
     REG_C = 354,
     REG_D = 355,
     REG_E = 356,
     REG_H = 357,
     REG_L = 358,
     REG_AF = 359,
     REG_BC = 360,
     REG_DE = 361,
     REG_HL = 362,
     REG_IX = 363,
     REG_IY = 364,
     REG_SP = 365,
     REG_IXH = 366,
     REG_IXL = 367,
     REG_IYH = 368,
     REG_IYL = 369,
     STRING = 370,
     FILENAME = 371,
     R_O_L = 372,
     OPCODE = 373,
     LABEL = 374,
     BANKNAME = 375,
     CPUTYPE = 376,
     REGISTER = 377,
     COMPARE_OP = 378,
     RADIX_TYPE = 379,
     INPUT_SPEC = 380,
     CMD_CHECKPT_ON = 381,
     CMD_CHECKPT_OFF = 382,
     TOGGLE = 383
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
#define CMD_LABEL_ASGN 344
#define L_PAREN 345
#define R_PAREN 346
#define ARG_IMMEDIATE 347
#define REG_A 348
#define REG_X 349
#define REG_Y 350
#define COMMA 351
#define INST_SEP 352
#define REG_B 353
#define REG_C 354
#define REG_D 355
#define REG_E 356
#define REG_H 357
#define REG_L 358
#define REG_AF 359
#define REG_BC 360
#define REG_DE 361
#define REG_HL 362
#define REG_IX 363
#define REG_IY 364
#define REG_SP 365
#define REG_IXH 366
#define REG_IXL 367
#define REG_IYH 368
#define REG_IYL 369
#define STRING 370
#define FILENAME 371
#define R_O_L 372
#define OPCODE 373
#define LABEL 374
#define BANKNAME 375
#define CPUTYPE 376
#define REGISTER 377
#define COMPARE_OP 378
#define RADIX_TYPE 379
#define INPUT_SPEC 380
#define CMD_CHECKPT_ON 381
#define CMD_CHECKPT_OFF 382
#define TOGGLE 383




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
#line 481 "mon_parse.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 494 "mon_parse.c"

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
#define YYFINAL  246
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1346

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  135
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  48
/* YYNRULES -- Number of rules.  */
#define YYNRULES  233
/* YYNRULES -- Number of states.  */
#define YYNSTATES  450

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   383

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     133,   134,   131,   129,     2,   130,     2,   132,     2,     2,
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
     125,   126,   127,   128
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
     404,   410,   416,   422,   426,   431,   437,   442,   448,   453,
     459,   465,   469,   472,   476,   481,   484,   487,   489,   491,
     493,   495,   497,   499,   500,   502,   505,   509,   511,   515,
     517,   519,   521,   523,   527,   529,   533,   536,   537,   539,
     543,   545,   547,   548,   550,   552,   554,   556,   558,   560,
     562,   566,   570,   574,   578,   582,   586,   588,   592,   596,
     600,   604,   606,   608,   610,   614,   616,   618,   620,   622,
     624,   626,   628,   630,   632,   634,   636,   638,   640,   642,
     644,   646,   648,   652,   656,   659,   662,   664,   666,   669,
     671,   675,   679,   683,   689,   695,   699,   703,   707,   711,
     715,   719,   725,   731,   737,   743,   744,   746,   748,   750,
     752,   754,   756,   758,   760,   762,   764,   766,   768,   770,
     772,   774,   776,   778
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     136,     0,    -1,   137,    -1,   180,    22,    -1,    22,    -1,
     139,    -1,   137,   139,    -1,    23,    -1,    22,    -1,     1,
      -1,   140,    -1,   142,    -1,   145,    -1,   143,    -1,   146,
      -1,   147,    -1,   148,    -1,   149,    -1,   150,    -1,   151,
      -1,   152,    -1,   153,    -1,    13,    -1,    67,   138,    -1,
      67,   167,   138,    -1,    67,   120,   138,    -1,    67,   167,
     166,   120,   138,    -1,    38,   165,   138,    -1,    46,   138,
      -1,    87,   121,   138,    -1,    26,   138,    -1,    49,   155,
     138,    -1,    50,   155,   138,    -1,    58,   138,    -1,    58,
     166,   170,   138,    -1,    57,   138,    -1,    57,   166,   170,
     138,    -1,    29,   138,    -1,    29,   166,   170,   138,    -1,
      30,   138,    -1,    30,   166,   170,   138,    -1,    85,   138,
      -1,   141,    -1,    39,   138,    -1,    39,   167,   138,    -1,
      39,   159,   138,    -1,    68,   167,   166,   155,   138,    -1,
      68,   155,   138,    -1,    69,   167,   166,   155,   138,    -1,
      69,   155,   138,    -1,    70,   165,   166,   119,   138,    -1,
      71,   119,   138,    -1,    71,   167,   166,   119,   138,    -1,
      72,   167,   138,    -1,    72,   138,    -1,    89,    21,   165,
     138,    -1,    89,    21,   165,    24,   138,    -1,    -1,    55,
     165,   144,   181,   138,    -1,    55,   165,   138,    -1,    56,
     162,   138,    -1,    56,   138,    -1,    37,   163,   166,   165,
     138,    -1,    48,   163,   166,   165,   138,    -1,    36,   163,
     166,   173,   138,    -1,    35,   163,   166,   173,   138,    -1,
      43,   124,   166,   162,   138,    -1,    43,   162,   138,    -1,
      43,   138,    -1,    76,   162,   138,    -1,    76,   138,    -1,
      77,   162,   138,    -1,    77,   138,    -1,    78,   162,   138,
      -1,    78,   138,    -1,    79,   162,   138,    -1,    79,   138,
      -1,    44,   162,   138,    -1,    86,   162,   138,    -1,    44,
     162,    15,   171,   138,    -1,    62,   157,   166,   162,   138,
      -1,    45,   162,   138,    -1,    44,   138,    -1,    86,   138,
      -1,    45,   138,    -1,    62,   138,    -1,   126,   161,   138,
      -1,   127,   161,   138,    -1,    34,   161,   138,    -1,    34,
     161,   166,   170,   138,    -1,    52,   161,   138,    -1,    52,
     138,    -1,    53,   161,    15,   171,   138,    -1,    54,   161,
     166,   115,   138,    -1,    54,   161,     1,   138,    -1,    25,
     128,   138,    -1,    25,   138,    -1,    42,   124,   138,    -1,
      42,   138,    -1,    60,   167,   138,    -1,    65,   138,    -1,
      51,   138,    -1,    63,   154,   138,    -1,    59,   170,   138,
      -1,    61,   138,    -1,    61,   154,   138,    -1,    64,   154,
     138,    -1,     7,   170,   138,    -1,    66,   154,   138,    -1,
      82,   154,   138,    -1,    31,   155,   156,   164,   138,    -1,
      83,   155,   156,   164,   138,    -1,    32,   155,   156,   163,
     138,    -1,    32,   155,     1,    -1,    32,   155,   156,     1,
      -1,    84,   155,   156,   163,   138,    -1,    84,   155,   156,
       1,    -1,    33,   155,   156,   165,   138,    -1,    33,   155,
     156,     1,    -1,    27,   170,   170,   164,   138,    -1,    28,
     170,   170,   165,   138,    -1,    73,   155,   138,    -1,    74,
     138,    -1,    75,   155,   138,    -1,    80,   165,   173,   138,
      -1,    81,   138,    -1,    88,   138,    -1,   117,    -1,   116,
      -1,     1,    -1,   170,    -1,     1,    -1,    14,    -1,    -1,
     122,    -1,   167,   122,    -1,   159,    96,   160,    -1,   160,
      -1,   158,    21,   178,    -1,   176,    -1,     1,    -1,   163,
      -1,   165,    -1,   165,   166,   165,    -1,     9,    -1,   167,
     166,     9,    -1,   166,   165,    -1,    -1,   168,    -1,   167,
     166,   168,    -1,   119,    -1,    96,    -1,    -1,    16,    -1,
      17,    -1,    18,    -1,    19,    -1,    20,    -1,   169,    -1,
     178,    -1,   170,   129,   170,    -1,   170,   130,   170,    -1,
     170,   131,   170,    -1,   170,   132,   170,    -1,   133,   170,
     134,    -1,   133,   170,     1,    -1,   175,    -1,   171,   123,
     171,    -1,   171,   123,     1,    -1,    90,   171,    91,    -1,
      90,   171,     1,    -1,   172,    -1,   158,    -1,   178,    -1,
     173,   166,   174,    -1,   174,    -1,   178,    -1,   115,    -1,
     178,    -1,   158,    -1,     4,    -1,    12,    -1,    11,    -1,
      10,    -1,    12,    -1,    11,    -1,    10,    -1,     3,    -1,
       4,    -1,     5,    -1,     6,    -1,   177,    -1,   179,    97,
     180,    -1,   180,    97,   180,    -1,   180,    97,    -1,   118,
     182,    -1,   180,    -1,   179,    -1,    92,   178,    -1,   178,
      -1,   178,    96,    94,    -1,   178,    96,    95,    -1,    90,
     178,    91,    -1,    90,   178,    96,    94,    91,    -1,    90,
     178,    91,    96,    95,    -1,    90,   105,    91,    -1,    90,
     106,    91,    -1,    90,   107,    91,    -1,    90,   108,    91,
      -1,    90,   109,    91,    -1,    90,   110,    91,    -1,    90,
     178,    91,    96,    93,    -1,    90,   178,    91,    96,   107,
      -1,    90,   178,    91,    96,   108,    -1,    90,   178,    91,
      96,   109,    -1,    -1,    93,    -1,    98,    -1,    99,    -1,
     100,    -1,   101,    -1,   102,    -1,   111,    -1,   113,    -1,
     103,    -1,   112,    -1,   114,    -1,   104,    -1,   105,    -1,
     106,    -1,   107,    -1,   108,    -1,   109,    -1,   110,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   169,   169,   170,   171,   174,   175,   178,   179,   180,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   198,   200,   202,   204,   206,   208,   210,
     212,   214,   216,   218,   220,   222,   224,   226,   228,   230,
     232,   234,   236,   239,   241,   243,   246,   252,   258,   260,
     262,   264,   266,   268,   270,   272,   276,   283,   282,   285,
     287,   289,   293,   295,   297,   299,   301,   303,   305,   307,
     309,   311,   313,   315,   317,   319,   321,   325,   330,   335,
     341,   347,   352,   354,   356,   358,   363,   365,   367,   369,
     371,   373,   375,   377,   379,   383,   385,   390,   392,   410,
     415,   417,   421,   423,   425,   427,   429,   431,   433,   435,
     439,   441,   443,   445,   447,   449,   451,   453,   455,   457,
     459,   463,   465,   467,   471,   473,   477,   481,   484,   485,
     488,   489,   492,   493,   496,   497,   500,   501,   504,   508,
     509,   512,   513,   516,   517,   519,   523,   524,   527,   532,
     537,   547,   548,   551,   552,   553,   554,   555,   558,   560,
     562,   563,   564,   565,   566,   567,   568,   571,   576,   578,
     580,   582,   586,   592,   600,   601,   604,   605,   608,   609,
     612,   613,   614,   615,   618,   619,   620,   623,   624,   625,
     626,   627,   630,   631,   632,   635,   645,   646,   649,   653,
     658,   663,   668,   670,   672,   674,   675,   676,   677,   678,
     679,   680,   682,   684,   686,   688,   689,   690,   691,   692,
     693,   694,   695,   696,   697,   698,   699,   700,   701,   702,
     703,   704,   705,   706
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
  "CMD_LABEL_ASGN", "L_PAREN", "R_PAREN", "ARG_IMMEDIATE", "REG_A",
  "REG_X", "REG_Y", "COMMA", "INST_SEP", "REG_B", "REG_C", "REG_D",
  "REG_E", "REG_H", "REG_L", "REG_AF", "REG_BC", "REG_DE", "REG_HL",
  "REG_IX", "REG_IY", "REG_SP", "REG_IXH", "REG_IXL", "REG_IYH", "REG_IYL",
  "STRING", "FILENAME", "R_O_L", "OPCODE", "LABEL", "BANKNAME", "CPUTYPE",
  "REGISTER", "COMPARE_OP", "RADIX_TYPE", "INPUT_SPEC", "CMD_CHECKPT_ON",
  "CMD_CHECKPT_OFF", "TOGGLE", "'+'", "'-'", "'*'", "'/'", "'('", "')'",
  "$accept", "top_level", "command_list", "end_cmd", "command",
  "machine_state_rules", "register_mod", "symbol_table_rules", "asm_rules",
  "@1", "memory_rules", "checkpoint_rules", "checkpoint_control_rules",
  "monitor_state_rules", "monitor_misc_rules", "disk_rules",
  "cmd_file_rules", "data_entry_rules", "monitor_debug_rules",
  "rest_of_line", "filename", "device_num", "opt_mem_op", "register",
  "reg_list", "reg_asgn", "breakpt_num", "address_opt_range",
  "address_range", "opt_address", "address", "opt_sep", "memspace",
  "memloc", "memaddr", "expression", "cond_expr", "compare_operand",
  "data_list", "data_element", "value", "d_number", "guess_default",
  "number", "assembly_instr_list", "assembly_instruction", "post_assemble",
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
     375,   376,   377,   378,   379,   380,   381,   382,   383,    43,
      45,    42,    47,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   135,   136,   136,   136,   137,   137,   138,   138,   138,
     139,   139,   139,   139,   139,   139,   139,   139,   139,   139,
     139,   139,   139,   140,   140,   140,   140,   140,   140,   140,
     140,   140,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   140,   140,   141,   141,   141,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   142,   142,   144,   143,   143,
     143,   143,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   148,   148,   148,   148,   148,
     148,   148,   149,   149,   149,   149,   149,   149,   149,   149,
     150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
     150,   151,   151,   151,   152,   152,   153,   154,   155,   155,
     156,   156,   157,   157,   158,   158,   159,   159,   160,   161,
     161,   162,   162,   163,   163,   163,   164,   164,   165,   165,
     165,   166,   166,   167,   167,   167,   167,   167,   168,   169,
     170,   170,   170,   170,   170,   170,   170,   171,   171,   171,
     171,   171,   172,   172,   173,   173,   174,   174,   175,   175,
     176,   176,   176,   176,   177,   177,   177,   178,   178,   178,
     178,   178,   179,   179,   179,   180,   181,   181,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182
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
       5,     5,     5,     3,     4,     5,     4,     5,     4,     5,
       5,     3,     2,     3,     4,     2,     2,     1,     1,     1,
       1,     1,     1,     0,     1,     2,     3,     1,     3,     1,
       1,     1,     1,     3,     1,     3,     2,     0,     1,     3,
       1,     1,     0,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     3,     3,     3,     1,     3,     3,     3,
       3,     1,     1,     1,     3,     1,     1,     1,     1,     1,
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
       0,     0,     0,     0,     0,     0,   215,     0,     0,     0,
       2,     5,    10,    42,    11,    13,    12,    14,    15,    16,
      17,    18,    19,    20,    21,     0,   187,   188,   189,   190,
     186,   185,   184,   153,   154,   155,   156,   157,   134,     0,
     179,     0,     0,   166,   191,   178,     9,     8,     7,     0,
      96,    30,     0,     0,   151,    37,     0,    39,     0,   129,
     128,     0,     0,     0,   140,   180,   183,   182,   181,     0,
     139,   144,   150,   152,   152,   152,   148,   158,   159,   152,
     152,     0,   152,    43,     0,     0,   137,     0,     0,    98,
     152,    68,     0,   141,   152,    82,     0,    84,     0,    28,
     152,     0,     0,   101,     9,    91,     0,     0,     0,     0,
      61,     0,    35,     0,    33,     0,     0,     0,   127,   104,
       0,   132,    85,   152,     0,     0,   100,     0,     0,    23,
       0,     0,   152,     0,   152,   152,     0,   152,    54,     0,
       0,   122,     0,    70,     0,    72,     0,    74,     0,    76,
       0,     0,   125,     0,     0,     0,    41,    83,     0,     0,
     126,     0,     0,     0,   216,   217,   218,   219,   220,   221,
     224,   227,   228,   229,   230,   231,   232,   233,   222,   225,
     223,   226,   199,   195,     0,     0,     1,     6,     3,     0,
     135,     0,     0,     0,     0,   107,    95,   152,     0,     0,
       0,   131,   152,   130,   113,     0,     0,    88,     0,     0,
       0,     0,     0,     0,    27,     0,     0,     0,    45,    44,
      97,     0,    67,     0,    77,    81,     0,    31,    32,    90,
       0,     0,     0,    59,     0,    60,     0,     0,   103,    99,
     105,     0,   102,   106,   108,    25,    24,     0,    47,     0,
      49,     0,     0,    51,     0,    53,   121,   123,    69,    71,
      73,    75,   177,     0,   175,   176,   109,   152,     0,    78,
      29,     0,     0,     0,     0,     0,     0,     0,     0,   198,
       0,    86,    87,   165,   164,   160,   161,   162,   163,     0,
       0,     0,    38,    40,     0,   114,     0,   118,     0,     0,
       0,   143,   145,   149,     0,     0,   138,   136,     0,     0,
     172,     0,   171,   173,     0,     0,    94,     0,   197,   196,
       0,    36,    34,     0,     0,     0,     0,     0,     0,   124,
       0,     0,   116,     0,     0,    55,   205,   206,   207,   208,
     209,   210,   202,     0,   200,   201,   119,   146,   120,   110,
     112,   117,    89,    65,    64,    62,    66,     0,     0,    79,
      63,    92,    93,     0,   194,    58,    80,    26,    46,    48,
      50,    52,   174,   111,   115,    56,     0,     0,   170,   169,
     168,   167,   192,   193,   211,   204,   212,   213,   214,   203
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    69,    70,   110,    71,    72,    73,    74,    75,   294,
      76,    77,    78,    79,    80,    81,    82,    83,    84,   180,
     121,   262,   183,   100,   145,   146,   129,   152,   153,   349,
     154,   350,   101,   136,   137,   263,   371,   372,   323,   324,
     103,   130,   104,   138,   378,    85,   380,   243
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -283
static const yytype_int16 yypact[] =
{
     861,   697,  -283,  -283,     3,   115,   697,   697,    71,    71,
      14,    14,    14,   446,  1187,  1187,  1187,  1210,   178,    31,
     758,   847,   847,   115,  1187,    14,    14,   115,   814,   446,
     446,  1210,   847,    71,    71,   697,   506,   103,  1083,   -86,
     -86,   115,   -86,   375,   395,   395,  1210,   389,   526,    14,
     115,    14,   847,   847,   847,   847,  1210,   115,   -86,    14,
      14,   115,   847,   -88,   115,    24,  1232,   446,   446,    52,
     964,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,
    -283,  -283,  -283,  -283,  -283,    35,  -283,  -283,  -283,  -283,
    -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,   697,
    -283,   -64,   240,  -283,  -283,  -283,  -283,  -283,  -283,   115,
    -283,  -283,   678,   678,  -283,  -283,   697,  -283,   697,  -283,
    -283,   228,   558,   228,  -283,  -283,  -283,  -283,  -283,    71,
    -283,  -283,  -283,   -37,   -37,   -37,  -283,  -283,  -283,   -37,
     -37,   115,   -37,  -283,    40,   180,  -283,    63,   115,  -283,
     -37,  -283,   115,  -283,   242,  -283,   204,  -283,   115,  -283,
     -37,   115,   115,  -283,   148,  -283,   115,    48,    28,   255,
    -283,   115,  -283,   697,  -283,   697,   240,   115,  -283,  -283,
     115,  -283,  -283,   -37,   115,   115,  -283,   115,   115,  -283,
      79,   115,   -37,   115,   -37,   -37,   115,   -37,  -283,   115,
     115,  -283,   115,  -283,   115,  -283,   115,  -283,   115,  -283,
     115,   607,  -283,   115,   228,   228,  -283,  -283,   115,   115,
    -283,  1210,   970,   486,  -283,  -283,  -283,  -283,  -283,  -283,
    -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,
    -283,  -283,   -30,  -283,   115,   115,  -283,  -283,  -283,    29,
    -283,   697,   697,   697,   697,  -283,  -283,    77,   715,   240,
     240,  -283,   214,   383,  1106,  1126,  1166,  -283,   697,   607,
    1210,   735,   607,  1210,  -283,   486,   486,    90,  -283,  -283,
    -283,  1187,  -283,  1051,  -283,  -283,  1210,  -283,  -283,  -283,
    1051,   115,   -46,  -283,   -45,  -283,   240,   240,  -283,  -283,
    -283,  1187,  -283,  -283,  -283,  -283,  -283,   -36,  -283,    14,
    -283,    14,   -27,  -283,   -24,  -283,  -283,  -283,  -283,  -283,
    -283,  -283,  -283,   578,  -283,  -283,  -283,   214,  1146,  -283,
    -283,   318,     5,    23,    41,    54,    61,    65,   -72,  -283,
     -55,  -283,  -283,  -283,  -283,   -82,   -82,  -283,  -283,   115,
    1210,   115,  -283,  -283,   115,  -283,   115,  -283,   115,   240,
     578,  -283,  -283,  -283,   578,   115,  -283,  -283,   115,  1051,
    -283,    45,  -283,  -283,   115,    45,  -283,   115,     8,    60,
     115,  -283,  -283,   115,   115,   115,   115,   115,   115,  -283,
     607,   115,  -283,   115,   115,  -283,  -283,  -283,  -283,  -283,
    -283,  -283,    68,    72,  -283,  -283,  -283,  -283,  -283,  -283,
    -283,  -283,  -283,  -283,  -283,  -283,  -283,    12,   786,  -283,
    -283,  -283,  -283,   -45,   -45,  -283,  -283,  -283,  -283,  -283,
    -283,  -283,  -283,  -283,  -283,  -283,   331,    74,  -283,  -283,
    -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -283,  -283,  -283,   275,   106,  -283,  -283,  -283,  -283,  -283,
    -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,  -283,   111,
      11,   -75,  -283,   -17,  -283,   -99,   319,   -11,    -7,  -248,
     329,    -6,   485,  -255,  -283,   112,  -272,  -283,  -234,  -210,
    -283,  -283,  -283,    -1,  -283,  -282,  -283,  -283
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -153
static const yytype_int16 yytable[] =
{
     105,   144,   116,   118,   106,   105,   105,   133,   139,   140,
     156,   158,   379,   438,   354,   119,   363,   160,   375,   402,
     363,   171,   122,   123,   403,   107,   108,   173,   175,   291,
     343,   178,   106,   219,   105,   360,   161,   162,   364,   404,
     405,   204,   206,   208,   210,   221,   106,   265,   266,   253,
     254,   218,   246,   107,   108,   191,   193,   248,   250,   114,
     200,   276,   202,   290,   106,   242,   340,   107,   108,   377,
     214,   215,   106,    66,  -152,  -152,  -152,  -152,  -147,   391,
     106,  -152,  -152,  -152,   384,   107,   108,  -152,  -152,  -152,
    -152,  -152,   387,   107,   108,   388,   396,   417,   105,  -147,
    -147,   107,   108,   439,   106,   423,    93,    94,    95,    96,
      97,   105,   105,   102,   397,   105,   106,   105,   112,   113,
     105,   105,   105,   268,   114,   107,   108,   269,   270,   271,
     120,   109,   398,   272,   273,   418,   275,   107,   108,   327,
     328,   442,   443,  -152,   281,   399,   441,   176,   270,  -140,
     184,   185,   400,   187,   286,   148,   401,   424,   251,   252,
     253,   254,   292,   344,   436,   449,   437,   114,   418,   213,
    -140,  -140,   105,   114,   105,   114,   247,   301,   367,   106,
     432,   106,     0,     0,   307,   250,   309,     0,   311,   312,
       0,   314,     0,  -152,    93,    94,    95,    96,    97,  -152,
     107,   108,   107,   108,  -152,   106,   251,   252,   253,   254,
     325,   249,    98,   105,   105,  -147,     0,     0,     0,   283,
     178,   338,   339,     0,   257,   258,   107,   108,   259,   261,
     260,    86,    87,    88,    89,     0,  -147,  -147,    90,    91,
      92,   106,     0,  -142,    93,    94,    95,    96,    97,     0,
     105,   105,   105,   105,     0,     0,   106,  -142,   356,     0,
     144,     0,   107,   108,  -142,  -142,   370,   105,   325,     0,
     368,   325,     0,   370,     0,   366,   277,   107,   108,     0,
     111,     0,   373,   115,   117,   296,     0,   297,     0,   373,
     383,     0,     0,   143,   149,   151,   155,   157,   159,     0,
      98,     0,   163,   165,     0,     0,     0,   170,   172,   174,
     114,     0,   179,   182,     0,     0,   186,   390,   189,   106,
     385,   393,   386,   198,     0,   201,     0,   203,   205,   207,
     209,     0,   212,     0,     0,     0,   216,   217,   114,   220,
     107,   108,   394,   134,   134,   134,   141,   166,   167,   168,
      98,     0,   370,   134,   390,     0,     0,     0,   390,     0,
     169,    99,     0,   345,   346,   347,   348,     0,   373,   251,
     252,   253,   254,   -57,     0,   195,   106,   255,     0,     0,
     359,     0,     0,     0,   256,   211,   244,   245,     0,   325,
       0,    93,    94,    95,    96,    97,   119,   107,   108,     0,
       0,   370,     0,     0,   267,    93,    94,    95,    96,    97,
       0,    93,    94,    95,    96,    97,   274,   373,     0,     0,
     278,     0,   279,   280,   444,     0,   445,   282,     0,     0,
       0,   284,     0,   285,     0,     0,   287,   288,   446,   447,
     448,   289,     0,     0,   293,     0,   295,   124,     0,     0,
     125,   298,   299,     0,     0,   300,   126,   127,   128,   302,
     303,     0,   304,   305,     0,   306,   308,     0,   310,     0,
       0,   313,     0,     0,   315,   316,     0,   317,     0,   318,
       0,   319,     0,   320,     0,   321,     0,     0,   326,    86,
      87,    88,    89,   329,   330,   188,    90,    91,    92,   135,
     135,   135,   142,   147,     0,   135,   135,   135,   196,   135,
       0,   120,   251,   252,   253,   254,   142,   135,     0,   341,
     342,   177,    93,    94,    95,    96,    97,   106,   190,   192,
     194,   142,   197,   199,   352,   353,     0,   135,   135,   135,
     135,   142,    93,    94,    95,    96,    97,   135,   107,   108,
     331,     0,     0,     0,     0,     0,     0,     0,     0,   264,
       0,    86,    87,    88,    89,     0,   376,     0,    90,    91,
      92,   381,   382,     0,    93,    94,    95,    96,    97,   106,
       0,  -152,  -152,  -152,  -152,     0,     0,   351,  -152,  -152,
    -152,     0,     0,     0,   134,   358,     0,     0,   389,   361,
     107,   108,   365,     0,     0,     0,   395,     0,     0,     0,
      86,    87,    88,    89,     0,   374,     0,    90,    91,    92,
       0,     0,     0,     0,   406,     0,   408,     0,     0,   409,
       0,   410,     0,   411,   412,   413,     0,     0,     0,   414,
     415,     0,     0,   416,     0,     0,   419,     0,     0,   420,
     421,     0,   422,     0,     0,   425,     0,   134,   426,   427,
     428,   429,   430,   431,     0,     0,   433,     0,   434,   435,
       0,     0,     0,     0,   114,     0,     0,     0,     0,   407,
      98,    86,    87,    88,    89,     0,     0,     0,    90,    91,
      92,    99,     0,  -152,    93,    94,    95,    96,    97,     0,
      86,    87,    88,    89,     0,     0,   142,    90,    91,    92,
       0,     0,     0,    93,    94,    95,    96,    97,    86,    87,
      88,    89,   322,     0,     0,    90,    91,    92,     0,     0,
       0,    93,    94,    95,    96,    97,     0,     0,    86,    87,
      88,    89,     0,   142,   362,    90,    91,    92,     0,     0,
     135,   142,     0,     0,     0,   142,     0,     0,   142,   106,
       0,    86,    87,    88,    89,     0,   135,   131,    90,    91,
      92,   142,     0,     0,    93,    94,    95,    96,    97,     0,
     107,   108,     0,     0,     0,     0,   135,   440,     0,    86,
      87,    88,    89,     0,     0,     0,    90,    91,    92,     0,
      98,     0,    93,    94,    95,    96,    97,   251,   252,   253,
     254,    99,     0,   135,     0,   164,     0,     0,   125,    98,
       0,     0,     0,     0,   126,   127,   128,     0,     0,     0,
      99,     0,     0,     0,   132,   142,   107,   108,     0,     0,
       0,     0,     0,     0,   251,   252,   253,   254,   106,     0,
      86,    87,    88,    89,     0,     0,   131,    90,    91,    92,
       0,     0,     0,    93,    94,    95,    96,    97,     1,   107,
     108,     0,     0,     0,     2,     0,   369,   132,     0,     0,
       0,     0,   150,     3,     0,     0,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,     0,     0,    19,    20,    21,    22,    23,    98,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   132,     0,     0,     0,
       0,     1,     0,    86,    87,    88,    89,     2,     0,    66,
      90,    91,    92,     0,     0,     0,     0,    67,    68,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,     0,     0,    19,    20,    21,    22,
      23,     0,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    86,    87,    88,    89,     0,     0,
       0,    90,    91,    92,     0,     0,     0,    93,    94,    95,
      96,    97,     0,     0,     0,   332,   333,   334,   335,   336,
     337,     0,     0,     0,   106,     0,  -133,  -133,  -133,  -133,
      67,    68,  -133,  -133,  -133,  -133,     0,   181,     0,  -133,
    -133,  -133,  -133,  -133,     0,   107,   108,  -131,     0,  -131,
    -131,  -131,  -131,     0,     0,  -131,  -131,  -131,  -131,     0,
       0,     0,  -131,  -131,  -131,  -131,  -131,   355,     0,    86,
      87,    88,    89,     0,     0,   131,    90,    91,    92,     0,
       0,   369,    93,    94,    95,    96,    97,   392,     0,    86,
      87,    88,    89,     0,     0,   131,    90,    91,    92,     0,
       0,     0,    93,    94,    95,    96,    97,   357,     0,    86,
      87,    88,    89,    98,     0,     0,    90,    91,    92,  -133,
       0,     0,    93,    94,    95,    96,    97,     0,     0,     0,
      86,    87,    88,    89,     0,     0,   131,    90,    91,    92,
       0,     0,  -133,    93,    94,    95,    96,    97,     0,     0,
       0,     0,     0,    86,    87,    88,    89,     0,     0,     0,
      90,    91,    92,     0,     0,  -131,    93,    94,    95,    96,
      97,     0,     0,     0,     0,    86,    87,    88,    89,     0,
       0,     0,    90,    91,    92,   132,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   132,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   132,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   132,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   222,     0,   223,   224,     0,     0,     0,   132,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241
};

static const yytype_int16 yycheck[] =
{
       1,    18,     8,     9,     1,     6,     7,    14,    15,    16,
      21,    22,   294,     1,   262,     1,   271,    24,   290,    91,
     275,    32,    11,    12,    96,    22,    23,    33,    34,     1,
       1,   117,     1,   121,    35,   269,    25,    26,   272,    94,
      95,    52,    53,    54,    55,    21,     1,   122,   123,   131,
     132,    62,     0,    22,    23,    44,    45,    22,   122,    96,
      49,    21,    51,    15,     1,    66,    96,    22,    23,   115,
      59,    60,     1,   118,     3,     4,     5,     6,     1,   327,
       1,    10,    11,    12,   120,    22,    23,    16,    17,    18,
      19,    20,   119,    22,    23,   119,    91,   369,    99,    22,
      23,    22,    23,    91,     1,    97,    16,    17,    18,    19,
      20,   112,   113,     1,    91,   116,     1,   118,     6,     7,
     121,   122,   123,   129,    96,    22,    23,   133,   134,   135,
     116,   128,    91,   139,   140,   123,   142,    22,    23,   214,
     215,   423,   424,   115,   150,    91,   418,    35,   154,     1,
      39,    40,    91,    42,   160,   124,    91,    97,   129,   130,
     131,   132,   168,   134,    96,    91,    94,    96,   123,    58,
      22,    23,   173,    96,   175,    96,    70,   183,   277,     1,
     390,     1,    -1,    -1,   190,   122,   192,    -1,   194,   195,
      -1,   197,    -1,   122,    16,    17,    18,    19,    20,   120,
      22,    23,    22,    23,   133,     1,   129,   130,   131,   132,
     211,    99,   122,   214,   215,     1,    -1,    -1,    -1,    15,
     117,   222,   223,    -1,   112,   113,    22,    23,   116,     1,
     118,     3,     4,     5,     6,    -1,    22,    23,    10,    11,
      12,     1,    -1,     1,    16,    17,    18,    19,    20,    -1,
     251,   252,   253,   254,    -1,    -1,     1,    15,   265,    -1,
     277,    -1,    22,    23,    22,    23,   283,   268,   269,    -1,
     281,   272,    -1,   290,    -1,   276,    96,    22,    23,    -1,
       5,    -1,   283,     8,     9,   173,    -1,   175,    -1,   290,
     301,    -1,    -1,    18,    19,    20,    21,    22,    23,    -1,
     122,    -1,    27,    28,    -1,    -1,    -1,    32,    33,    34,
      96,    -1,    37,    38,    -1,    -1,    41,   323,    43,     1,
     309,   328,   311,    48,    -1,    50,    -1,    52,    53,    54,
      55,    -1,    57,    -1,    -1,    -1,    61,    62,    96,    64,
      22,    23,    24,    14,    15,    16,    17,    28,    29,    30,
     122,    -1,   369,    24,   360,    -1,    -1,    -1,   364,    -1,
      31,   133,    -1,   251,   252,   253,   254,    -1,   369,   129,
     130,   131,   132,   118,    -1,    46,     1,   102,    -1,    -1,
     268,    -1,    -1,    -1,   109,    56,    67,    68,    -1,   390,
      -1,    16,    17,    18,    19,    20,     1,    22,    23,    -1,
      -1,   418,    -1,    -1,   129,    16,    17,    18,    19,    20,
      -1,    16,    17,    18,    19,    20,   141,   418,    -1,    -1,
     145,    -1,   147,   148,    93,    -1,    95,   152,    -1,    -1,
      -1,   156,    -1,   158,    -1,    -1,   161,   162,   107,   108,
     109,   166,    -1,    -1,   169,    -1,   171,     1,    -1,    -1,
       4,   176,   177,    -1,    -1,   180,    10,    11,    12,   184,
     185,    -1,   187,   188,    -1,   190,   191,    -1,   193,    -1,
      -1,   196,    -1,    -1,   199,   200,    -1,   202,    -1,   204,
      -1,   206,    -1,   208,    -1,   210,    -1,    -1,   213,     3,
       4,     5,     6,   218,   219,   120,    10,    11,    12,    14,
      15,    16,    17,    18,    -1,    20,    21,    22,   119,    24,
      -1,   116,   129,   130,   131,   132,    31,    32,    -1,   244,
     245,    36,    16,    17,    18,    19,    20,     1,    43,    44,
      45,    46,    47,    48,   259,   260,    -1,    52,    53,    54,
      55,    56,    16,    17,    18,    19,    20,    62,    22,    23,
     221,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,
      -1,     3,     4,     5,     6,    -1,   291,    -1,    10,    11,
      12,   296,   297,    -1,    16,    17,    18,    19,    20,     1,
      -1,     3,     4,     5,     6,    -1,    -1,   258,    10,    11,
      12,    -1,    -1,    -1,   265,   266,    -1,    -1,   323,   270,
      22,    23,   273,    -1,    -1,    -1,   331,    -1,    -1,    -1,
       3,     4,     5,     6,    -1,   286,    -1,    10,    11,    12,
      -1,    -1,    -1,    -1,   349,    -1,   351,    -1,    -1,   354,
      -1,   356,    -1,   358,   359,   360,    -1,    -1,    -1,   364,
     365,    -1,    -1,   368,    -1,    -1,   371,    -1,    -1,   374,
     375,    -1,   377,    -1,    -1,   380,    -1,   328,   383,   384,
     385,   386,   387,   388,    -1,    -1,   391,    -1,   393,   394,
      -1,    -1,    -1,    -1,    96,    -1,    -1,    -1,    -1,   350,
     122,     3,     4,     5,     6,    -1,    -1,    -1,    10,    11,
      12,   133,    -1,   115,    16,    17,    18,    19,    20,    -1,
       3,     4,     5,     6,    -1,    -1,   221,    10,    11,    12,
      -1,    -1,    -1,    16,    17,    18,    19,    20,     3,     4,
       5,     6,   115,    -1,    -1,    10,    11,    12,    -1,    -1,
      -1,    16,    17,    18,    19,    20,    -1,    -1,     3,     4,
       5,     6,    -1,   258,     9,    10,    11,    12,    -1,    -1,
     265,   266,    -1,    -1,    -1,   270,    -1,    -1,   273,     1,
      -1,     3,     4,     5,     6,    -1,   281,     9,    10,    11,
      12,   286,    -1,    -1,    16,    17,    18,    19,    20,    -1,
      22,    23,    -1,    -1,    -1,    -1,   301,     1,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,
     122,    -1,    16,    17,    18,    19,    20,   129,   130,   131,
     132,   133,    -1,   328,    -1,     1,    -1,    -1,     4,   122,
      -1,    -1,    -1,    -1,    10,    11,    12,    -1,    -1,    -1,
     133,    -1,    -1,    -1,   119,   350,    22,    23,    -1,    -1,
      -1,    -1,    -1,    -1,   129,   130,   131,   132,     1,    -1,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      -1,    -1,    -1,    16,    17,    18,    19,    20,     7,    22,
      23,    -1,    -1,    -1,    13,    -1,    90,   119,    -1,    -1,
      -1,    -1,   124,    22,    -1,    -1,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    -1,    -1,    42,    43,    44,    45,    46,   122,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   119,    -1,    -1,    -1,
      -1,     7,    -1,     3,     4,     5,     6,    13,    -1,   118,
      10,    11,    12,    -1,    -1,    -1,    -1,   126,   127,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    -1,    -1,    42,    43,    44,    45,
      46,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,     3,     4,     5,     6,    -1,    -1,
      -1,    10,    11,    12,    -1,    -1,    -1,    16,    17,    18,
      19,    20,    -1,    -1,    -1,   105,   106,   107,   108,   109,
     110,    -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,
     126,   127,     9,    10,    11,    12,    -1,    14,    -1,    16,
      17,    18,    19,    20,    -1,    22,    23,     1,    -1,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    12,    -1,
      -1,    -1,    16,    17,    18,    19,    20,     1,    -1,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    12,    -1,
      -1,    90,    16,    17,    18,    19,    20,     1,    -1,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    12,    -1,
      -1,    -1,    16,    17,    18,    19,    20,     1,    -1,     3,
       4,     5,     6,   122,    -1,    -1,    10,    11,    12,    96,
      -1,    -1,    16,    17,    18,    19,    20,    -1,    -1,    -1,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      -1,    -1,   119,    16,    17,    18,    19,    20,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,
      10,    11,    12,    -1,    -1,   119,    16,    17,    18,    19,
      20,    -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    10,    11,    12,   119,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   119,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   119,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   119,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    90,    -1,    92,    93,    -1,    -1,    -1,   119,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114
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
      84,    85,    86,    87,    88,    89,   118,   126,   127,   136,
     137,   139,   140,   141,   142,   143,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   180,     3,     4,     5,     6,
      10,    11,    12,    16,    17,    18,    19,    20,   122,   133,
     158,   167,   170,   175,   177,   178,     1,    22,    23,   128,
     138,   138,   170,   170,    96,   138,   166,   138,   166,     1,
     116,   155,   155,   155,     1,     4,    10,    11,    12,   161,
     176,     9,   119,   163,   165,   167,   168,   169,   178,   163,
     163,   165,   167,   138,   158,   159,   160,   167,   124,   138,
     124,   138,   162,   163,   165,   138,   162,   138,   162,   138,
     163,   155,   155,   138,     1,   138,   161,   161,   161,   165,
     138,   162,   138,   166,   138,   166,   170,   167,   117,   138,
     154,    14,   138,   157,   154,   154,   138,   154,   120,   138,
     167,   155,   167,   155,   167,   165,   119,   167,   138,   167,
     155,   138,   155,   138,   162,   138,   162,   138,   162,   138,
     162,   165,   138,   154,   155,   155,   138,   138,   162,   121,
     138,    21,    90,    92,    93,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   178,   182,   161,   161,     0,   139,    22,   170,
     122,   129,   130,   131,   132,   138,   138,   170,   170,   170,
     170,     1,   156,   170,     1,   156,   156,   138,   166,   166,
     166,   166,   166,   166,   138,   166,    21,    96,   138,   138,
     138,   166,   138,    15,   138,   138,   166,   138,   138,   138,
      15,     1,   166,   138,   144,   138,   170,   170,   138,   138,
     138,   166,   138,   138,   138,   138,   138,   166,   138,   166,
     138,   166,   166,   138,   166,   138,   138,   138,   138,   138,
     138,   138,   115,   173,   174,   178,   138,   156,   156,   138,
     138,   165,   105,   106,   107,   108,   109,   110,   178,   178,
      96,   138,   138,     1,   134,   170,   170,   170,   170,   164,
     166,   165,   138,   138,   164,     1,   163,     1,   165,   170,
     173,   165,     9,   168,   173,   165,   178,   160,   162,    90,
     158,   171,   172,   178,   165,   171,   138,   115,   179,   180,
     181,   138,   138,   162,   120,   155,   155,   119,   119,   138,
     166,   164,     1,   163,    24,   138,    91,    91,    91,    91,
      91,    91,    91,    96,    94,    95,   138,   165,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   171,   123,   138,
     138,   138,   138,    97,    97,   138,   138,   138,   138,   138,
     138,   138,   174,   138,   138,   138,    96,    94,     1,    91,
       1,   171,   180,   180,    93,    95,   107,   108,   109,    91
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
#line 169 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 3:
#line 170 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 4:
#line 171 "mon_parse.y"
    { new_cmd = 1; asm_mode = 0;  (yyval.i) = 0; }
    break;

  case 9:
#line 180 "mon_parse.y"
    { return ERR_EXPECT_END_CMD; }
    break;

  case 22:
#line 195 "mon_parse.y"
    { return ERR_BAD_CMD; }
    break;

  case 23:
#line 199 "mon_parse.y"
    { mon_bank(e_default_space, NULL); }
    break;

  case 24:
#line 201 "mon_parse.y"
    { mon_bank((yyvsp[(2) - (3)].i), NULL); }
    break;

  case 25:
#line 203 "mon_parse.y"
    { mon_bank(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 26:
#line 205 "mon_parse.y"
    { mon_bank((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 27:
#line 207 "mon_parse.y"
    { mon_jump((yyvsp[(2) - (3)].a)); }
    break;

  case 28:
#line 209 "mon_parse.y"
    { mon_display_io_regs(); }
    break;

  case 29:
#line 211 "mon_parse.y"
    { monitor_cpu_type_set((yyvsp[(2) - (3)].str)); }
    break;

  case 30:
#line 213 "mon_parse.y"
    { mon_instruction_return(); }
    break;

  case 31:
#line 215 "mon_parse.y"
    { machine_write_snapshot((yyvsp[(2) - (3)].str),0,0,0); /* FIXME */ }
    break;

  case 32:
#line 217 "mon_parse.y"
    { machine_read_snapshot((yyvsp[(2) - (3)].str), 0); }
    break;

  case 33:
#line 219 "mon_parse.y"
    { mon_instructions_step(-1); }
    break;

  case 34:
#line 221 "mon_parse.y"
    { mon_instructions_step((yyvsp[(3) - (4)].i)); }
    break;

  case 35:
#line 223 "mon_parse.y"
    { mon_instructions_next(-1); }
    break;

  case 36:
#line 225 "mon_parse.y"
    { mon_instructions_next((yyvsp[(3) - (4)].i)); }
    break;

  case 37:
#line 227 "mon_parse.y"
    { mon_stack_up(-1); }
    break;

  case 38:
#line 229 "mon_parse.y"
    { mon_stack_up((yyvsp[(3) - (4)].i)); }
    break;

  case 39:
#line 231 "mon_parse.y"
    { mon_stack_down(-1); }
    break;

  case 40:
#line 233 "mon_parse.y"
    { mon_stack_down((yyvsp[(3) - (4)].i)); }
    break;

  case 41:
#line 235 "mon_parse.y"
    { mon_display_screen(); }
    break;

  case 43:
#line 240 "mon_parse.y"
    { (monitor_cpu_type.mon_register_print)(default_memspace); }
    break;

  case 44:
#line 242 "mon_parse.y"
    { (monitor_cpu_type.mon_register_print)((yyvsp[(2) - (3)].i)); }
    break;

  case 46:
#line 247 "mon_parse.y"
    {
                        /* What about the memspace? */
                        playback = TRUE; playback_name = (yyvsp[(4) - (5)].str);
                        /*mon_load_symbols($2, $3);*/
                    }
    break;

  case 47:
#line 253 "mon_parse.y"
    {
                        /* What about the memspace? */
                        playback = TRUE; playback_name = (yyvsp[(2) - (3)].str);
                        /*mon_load_symbols($2, $3);*/
                    }
    break;

  case 48:
#line 259 "mon_parse.y"
    { mon_save_symbols((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 49:
#line 261 "mon_parse.y"
    { mon_save_symbols(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 50:
#line 263 "mon_parse.y"
    { mon_add_name_to_symbol_table((yyvsp[(2) - (5)].a), (yyvsp[(4) - (5)].str)); }
    break;

  case 51:
#line 265 "mon_parse.y"
    { mon_remove_name_from_symbol_table(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 52:
#line 267 "mon_parse.y"
    { mon_remove_name_from_symbol_table((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 53:
#line 269 "mon_parse.y"
    { mon_print_symbol_table((yyvsp[(2) - (3)].i)); }
    break;

  case 54:
#line 271 "mon_parse.y"
    { mon_print_symbol_table(e_default_space); }
    break;

  case 55:
#line 273 "mon_parse.y"
    {
                        mon_add_name_to_symbol_table((yyvsp[(3) - (4)].a), mon_prepend_dot_to_name((yyvsp[(1) - (4)].str)));
                    }
    break;

  case 56:
#line 277 "mon_parse.y"
    {
                        mon_add_name_to_symbol_table((yyvsp[(3) - (5)].a), mon_prepend_dot_to_name((yyvsp[(1) - (5)].str)));
                    }
    break;

  case 57:
#line 283 "mon_parse.y"
    { mon_start_assemble_mode((yyvsp[(2) - (2)].a), NULL); }
    break;

  case 58:
#line 284 "mon_parse.y"
    { }
    break;

  case 59:
#line 286 "mon_parse.y"
    { mon_start_assemble_mode((yyvsp[(2) - (3)].a), NULL); }
    break;

  case 60:
#line 288 "mon_parse.y"
    { mon_disassemble_lines((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1]); }
    break;

  case 61:
#line 290 "mon_parse.y"
    { mon_disassemble_lines(BAD_ADDR, BAD_ADDR); }
    break;

  case 62:
#line 294 "mon_parse.y"
    { mon_memory_move((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], (yyvsp[(4) - (5)].a)); }
    break;

  case 63:
#line 296 "mon_parse.y"
    { mon_memory_compare((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[0], (yyvsp[(4) - (5)].a)); }
    break;

  case 64:
#line 298 "mon_parse.y"
    { mon_memory_fill((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1],(unsigned char *)(yyvsp[(4) - (5)].str)); }
    break;

  case 65:
#line 300 "mon_parse.y"
    { mon_memory_hunt((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1],(unsigned char *)(yyvsp[(4) - (5)].str)); }
    break;

  case 66:
#line 302 "mon_parse.y"
    { mon_memory_display((yyvsp[(2) - (5)].rt), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], DF_PETSCII); }
    break;

  case 67:
#line 304 "mon_parse.y"
    { mon_memory_display(default_radix, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_PETSCII); }
    break;

  case 68:
#line 306 "mon_parse.y"
    { mon_memory_display(default_radix, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
    break;

  case 69:
#line 308 "mon_parse.y"
    { mon_memory_display_data((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], 8, 8); }
    break;

  case 70:
#line 310 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 8, 8); }
    break;

  case 71:
#line 312 "mon_parse.y"
    { mon_memory_display_data((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], 24, 21); }
    break;

  case 72:
#line 314 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 24, 21); }
    break;

  case 73:
#line 316 "mon_parse.y"
    { mon_memory_display(0, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_PETSCII); }
    break;

  case 74:
#line 318 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
    break;

  case 75:
#line 320 "mon_parse.y"
    { mon_memory_display(0, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_SCREEN_CODE); }
    break;

  case 76:
#line 322 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_SCREEN_CODE); }
    break;

  case 77:
#line 326 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], FALSE, FALSE,
                                                    FALSE, FALSE);
                  }
    break;

  case 78:
#line 331 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], FALSE, FALSE,
                                                    FALSE, TRUE);
                  }
    break;

  case 79:
#line 336 "mon_parse.y"
    {
                      temp = mon_breakpoint_add_checkpoint((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], FALSE,
                                                           FALSE, FALSE, FALSE);
                      mon_breakpoint_set_checkpoint_condition(temp, (yyvsp[(4) - (5)].cond_node));
                  }
    break;

  case 80:
#line 342 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], FALSE,
                      ((yyvsp[(2) - (5)].i) == e_load || (yyvsp[(2) - (5)].i) == e_load_store),
                      ((yyvsp[(2) - (5)].i) == e_store || (yyvsp[(2) - (5)].i) == e_load_store), FALSE);
                  }
    break;

  case 81:
#line 348 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], TRUE, FALSE, FALSE,
                                                    FALSE);
                  }
    break;

  case 82:
#line 353 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 83:
#line 355 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 84:
#line 357 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 85:
#line 359 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 86:
#line 364 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_ON, (yyvsp[(2) - (3)].i)); }
    break;

  case 87:
#line 366 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_OFF, (yyvsp[(2) - (3)].i)); }
    break;

  case 88:
#line 368 "mon_parse.y"
    { mon_breakpoint_set_ignore_count((yyvsp[(2) - (3)].i), -1); }
    break;

  case 89:
#line 370 "mon_parse.y"
    { mon_breakpoint_set_ignore_count((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].i)); }
    break;

  case 90:
#line 372 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint((yyvsp[(2) - (3)].i)); }
    break;

  case 91:
#line 374 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint(-1); }
    break;

  case 92:
#line 376 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_condition((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].cond_node)); }
    break;

  case 93:
#line 378 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_command((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 94:
#line 380 "mon_parse.y"
    { return ERR_EXPECT_STRING; }
    break;

  case 95:
#line 384 "mon_parse.y"
    { sidefx = (((yyvsp[(2) - (3)].action) == e_TOGGLE) ? (sidefx ^ 1) : (yyvsp[(2) - (3)].action)); }
    break;

  case 96:
#line 386 "mon_parse.y"
    {
                         mon_out("I/O side effects are %s\n",
                                   sidefx ? "enabled" : "disabled");
                     }
    break;

  case 97:
#line 391 "mon_parse.y"
    { default_radix = (yyvsp[(2) - (3)].rt); }
    break;

  case 98:
#line 393 "mon_parse.y"
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
#line 411 "mon_parse.y"
    {
                         mon_out("Setting default device to `%s'\n",
                         _mon_space_strings[(int) (yyvsp[(2) - (3)].i)]); default_memspace = (yyvsp[(2) - (3)].i);
                     }
    break;

  case 100:
#line 416 "mon_parse.y"
    { mon_quit(); YYACCEPT; }
    break;

  case 101:
#line 418 "mon_parse.y"
    { exit_mon = 1; YYACCEPT; }
    break;

  case 102:
#line 422 "mon_parse.y"
    { mon_drive_execute_disk_cmd((yyvsp[(2) - (3)].str)); }
    break;

  case 103:
#line 424 "mon_parse.y"
    { mon_out("\t%d\n",(yyvsp[(2) - (3)].i)); }
    break;

  case 104:
#line 426 "mon_parse.y"
    { mon_command_print_help(NULL); }
    break;

  case 105:
#line 428 "mon_parse.y"
    { mon_command_print_help((yyvsp[(2) - (3)].str)); }
    break;

  case 106:
#line 430 "mon_parse.y"
    { printf("SYSTEM COMMAND: %s\n",(yyvsp[(2) - (3)].str)); }
    break;

  case 107:
#line 432 "mon_parse.y"
    { mon_print_convert((yyvsp[(2) - (3)].i)); }
    break;

  case 108:
#line 434 "mon_parse.y"
    { mon_change_dir((yyvsp[(2) - (3)].str)); }
    break;

  case 109:
#line 436 "mon_parse.y"
    { mon_keyboard_feed((yyvsp[(2) - (3)].str)); }
    break;

  case 110:
#line 440 "mon_parse.y"
    { mon_file_load((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].a), FALSE); }
    break;

  case 111:
#line 442 "mon_parse.y"
    { mon_file_load((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].a), TRUE); }
    break;

  case 112:
#line 444 "mon_parse.y"
    { mon_file_save((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], FALSE); }
    break;

  case 113:
#line 446 "mon_parse.y"
    { return ERR_EXPECT_DEVICE_NUM; }
    break;

  case 114:
#line 448 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 115:
#line 450 "mon_parse.y"
    { mon_file_save((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], TRUE); }
    break;

  case 116:
#line 452 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 117:
#line 454 "mon_parse.y"
    { mon_file_verify((yyvsp[(2) - (5)].str),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 118:
#line 456 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 119:
#line 458 "mon_parse.y"
    { mon_drive_block_cmd(0,(yyvsp[(2) - (5)].i),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 120:
#line 460 "mon_parse.y"
    { mon_drive_block_cmd(1,(yyvsp[(2) - (5)].i),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 121:
#line 464 "mon_parse.y"
    { mon_record_commands((yyvsp[(2) - (3)].str)); }
    break;

  case 122:
#line 466 "mon_parse.y"
    { mon_end_recording(); }
    break;

  case 123:
#line 468 "mon_parse.y"
    { playback=TRUE; playback_name = (yyvsp[(2) - (3)].str); }
    break;

  case 124:
#line 472 "mon_parse.y"
    { mon_memory_fill((yyvsp[(2) - (4)].a), BAD_ADDR, (unsigned char *)(yyvsp[(3) - (4)].str)); }
    break;

  case 125:
#line 474 "mon_parse.y"
    { printf("Not yet.\n"); }
    break;

  case 126:
#line 478 "mon_parse.y"
    { yydebug = 1; }
    break;

  case 127:
#line 481 "mon_parse.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 129:
#line 485 "mon_parse.y"
    { return ERR_EXPECT_FILENAME; }
    break;

  case 131:
#line 489 "mon_parse.y"
    { return ERR_EXPECT_DEVICE_NUM; }
    break;

  case 132:
#line 492 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 133:
#line 493 "mon_parse.y"
    { (yyval.i) = e_load_store; }
    break;

  case 134:
#line 496 "mon_parse.y"
    { (yyval.i) = new_reg(default_memspace, (yyvsp[(1) - (1)].reg)); }
    break;

  case 135:
#line 497 "mon_parse.y"
    { (yyval.i) = new_reg((yyvsp[(1) - (2)].i), (yyvsp[(2) - (2)].reg)); }
    break;

  case 138:
#line 505 "mon_parse.y"
    { (monitor_cpu_type.mon_register_set_val)(reg_memspace((yyvsp[(1) - (3)].i)), reg_regid((yyvsp[(1) - (3)].i)), (WORD) (yyvsp[(3) - (3)].i)); }
    break;

  case 139:
#line 508 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 140:
#line 509 "mon_parse.y"
    { return ERR_EXPECT_BRKNUM; }
    break;

  case 142:
#line 513 "mon_parse.y"
    { (yyval.range)[0] = (yyvsp[(1) - (1)].a); (yyval.range)[1] = BAD_ADDR; }
    break;

  case 143:
#line 516 "mon_parse.y"
    { (yyval.range)[0] = (yyvsp[(1) - (3)].a); (yyval.range)[1] = (yyvsp[(3) - (3)].a); }
    break;

  case 144:
#line 518 "mon_parse.y"
    { if (resolve_range(e_default_space, (yyval.range), (yyvsp[(1) - (1)].str))) return ERR_ADDR_TOO_BIG; }
    break;

  case 145:
#line 520 "mon_parse.y"
    { if (resolve_range((yyvsp[(1) - (3)].i), (yyval.range), (yyvsp[(3) - (3)].str))) return ERR_ADDR_TOO_BIG; }
    break;

  case 146:
#line 523 "mon_parse.y"
    { (yyval.a) = (yyvsp[(2) - (2)].a); }
    break;

  case 147:
#line 524 "mon_parse.y"
    { (yyval.a) = BAD_ADDR; }
    break;

  case 148:
#line 528 "mon_parse.y"
    {
             (yyval.a) = new_addr(e_default_space,(yyvsp[(1) - (1)].i));
             if (opt_asm) new_cmd = asm_mode = 1; 
         }
    break;

  case 149:
#line 533 "mon_parse.y"
    {
             (yyval.a) = new_addr((yyvsp[(1) - (3)].i), (yyvsp[(3) - (3)].i));
             if (opt_asm) new_cmd = asm_mode = 1; 
         }
    break;

  case 150:
#line 538 "mon_parse.y"
    {
             temp = mon_symbol_table_lookup_addr(e_default_space, (yyvsp[(1) - (1)].str));
             if (temp >= 0)
                 (yyval.a) = new_addr(e_default_space, temp);
             else
                 return ERR_UNDEFINED_LABEL;
         }
    break;

  case 153:
#line 551 "mon_parse.y"
    { (yyval.i) = e_comp_space; }
    break;

  case 154:
#line 552 "mon_parse.y"
    { (yyval.i) = e_disk8_space; }
    break;

  case 155:
#line 553 "mon_parse.y"
    { (yyval.i) = e_disk9_space; }
    break;

  case 156:
#line 554 "mon_parse.y"
    { (yyval.i) = e_disk10_space; }
    break;

  case 157:
#line 555 "mon_parse.y"
    { (yyval.i) = e_disk11_space; }
    break;

  case 158:
#line 558 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); if (!CHECK_ADDR((yyvsp[(1) - (1)].i))) return ERR_ADDR_TOO_BIG; }
    break;

  case 159:
#line 560 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 160:
#line 562 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) + (yyvsp[(3) - (3)].i); }
    break;

  case 161:
#line 563 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) - (yyvsp[(3) - (3)].i); }
    break;

  case 162:
#line 564 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) * (yyvsp[(3) - (3)].i); }
    break;

  case 163:
#line 565 "mon_parse.y"
    { (yyval.i) = ((yyvsp[(3) - (3)].i)) ? ((yyvsp[(1) - (3)].i) / (yyvsp[(3) - (3)].i)) : 1; }
    break;

  case 164:
#line 566 "mon_parse.y"
    { (yyval.i) = (yyvsp[(2) - (3)].i); }
    break;

  case 165:
#line 567 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 166:
#line 568 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 167:
#line 572 "mon_parse.y"
    {
               (yyval.cond_node) = new_cond; (yyval.cond_node)->is_parenthized = FALSE;
               (yyval.cond_node)->child1 = (yyvsp[(1) - (3)].cond_node); (yyval.cond_node)->child2 = (yyvsp[(3) - (3)].cond_node); (yyval.cond_node)->operation = (yyvsp[(2) - (3)].cond_op);
           }
    break;

  case 168:
#line 577 "mon_parse.y"
    { return ERR_INCOMPLETE_COMPARE_OP; }
    break;

  case 169:
#line 579 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(2) - (3)].cond_node); (yyval.cond_node)->is_parenthized = TRUE; }
    break;

  case 170:
#line 581 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 171:
#line 583 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(1) - (1)].cond_node); }
    break;

  case 172:
#line 586 "mon_parse.y"
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->reg_num = (yyvsp[(1) - (1)].i); (yyval.cond_node)->is_reg = TRUE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
    break;

  case 173:
#line 592 "mon_parse.y"
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->value = (yyvsp[(1) - (1)].i); (yyval.cond_node)->is_reg = FALSE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
    break;

  case 176:
#line 604 "mon_parse.y"
    { mon_add_number_to_buffer((yyvsp[(1) - (1)].i)); }
    break;

  case 177:
#line 605 "mon_parse.y"
    { mon_add_string_to_buffer((yyvsp[(1) - (1)].str)); }
    break;

  case 178:
#line 608 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 179:
#line 609 "mon_parse.y"
    { (yyval.i) = (monitor_cpu_type.mon_register_get_val)(reg_memspace((yyvsp[(1) - (1)].i)), reg_regid((yyvsp[(1) - (1)].i))); }
    break;

  case 180:
#line 612 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 181:
#line 613 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 182:
#line 614 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 183:
#line 615 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 184:
#line 618 "mon_parse.y"
    { (yyval.i) = resolve_datatype(B_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 185:
#line 619 "mon_parse.y"
    { (yyval.i) = resolve_datatype(O_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 186:
#line 620 "mon_parse.y"
    { (yyval.i) = resolve_datatype(D_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 187:
#line 623 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 188:
#line 624 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 189:
#line 625 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 190:
#line 626 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 191:
#line 627 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 195:
#line 635 "mon_parse.y"
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

  case 197:
#line 646 "mon_parse.y"
    { asm_mode = 0; }
    break;

  case 198:
#line 649 "mon_parse.y"
    { if ((yyvsp[(2) - (2)].i) > 0xff)
                          (yyval.i) = join_ints(ASM_ADDR_MODE_IMMEDIATE_16,(yyvsp[(2) - (2)].i));
                        else
                          (yyval.i) = join_ints(ASM_ADDR_MODE_IMMEDIATE,(yyvsp[(2) - (2)].i)); }
    break;

  case 199:
#line 653 "mon_parse.y"
    { if ((yyvsp[(1) - (1)].i) < 0x100)
               (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE,(yyvsp[(1) - (1)].i));
             else
               (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE,(yyvsp[(1) - (1)].i));
           }
    break;

  case 200:
#line 658 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100)
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE_X,(yyvsp[(1) - (3)].i));
                          else
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_X,(yyvsp[(1) - (3)].i));
                        }
    break;

  case 201:
#line 663 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100)
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE_Y,(yyvsp[(1) - (3)].i));
                          else
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_Y,(yyvsp[(1) - (3)].i));
                        }
    break;

  case 202:
#line 669 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABS_INDIRECT,(yyvsp[(2) - (3)].i)); }
    break;

  case 203:
#line 671 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_INDIRECT_X,(yyvsp[(2) - (5)].i)); }
    break;

  case 204:
#line 673 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_INDIRECT_Y,(yyvsp[(2) - (5)].i)); }
    break;

  case 205:
#line 674 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_BC,0); }
    break;

  case 206:
#line 675 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_DE,0); }
    break;

  case 207:
#line 676 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_HL,0); }
    break;

  case 208:
#line 677 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_IX,0); }
    break;

  case 209:
#line 678 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_IY,0); }
    break;

  case 210:
#line 679 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_SP,0); }
    break;

  case 211:
#line 681 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_A,(yyvsp[(2) - (5)].i)); }
    break;

  case 212:
#line 683 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_HL,(yyvsp[(2) - (5)].i)); }
    break;

  case 213:
#line 685 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_IX,(yyvsp[(2) - (5)].i)); }
    break;

  case 214:
#line 687 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_IY,(yyvsp[(2) - (5)].i)); }
    break;

  case 215:
#line 688 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_IMPLIED,0); }
    break;

  case 216:
#line 689 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ACCUMULATOR,0); }
    break;

  case 217:
#line 690 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_B,0); }
    break;

  case 218:
#line 691 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_C,0); }
    break;

  case 219:
#line 692 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_D,0); }
    break;

  case 220:
#line 693 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_E,0); }
    break;

  case 221:
#line 694 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_H,0); }
    break;

  case 222:
#line 695 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IXH,0); }
    break;

  case 223:
#line 696 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IYH,0); }
    break;

  case 224:
#line 697 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_L,0); }
    break;

  case 225:
#line 698 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IXL,0); }
    break;

  case 226:
#line 699 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IYL,0); }
    break;

  case 227:
#line 700 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_AF,0); }
    break;

  case 228:
#line 701 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_BC,0); }
    break;

  case 229:
#line 702 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_DE,0); }
    break;

  case 230:
#line 703 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_HL,0); }
    break;

  case 231:
#line 704 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IX,0); }
    break;

  case 232:
#line 705 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IY,0); }
    break;

  case 233:
#line 706 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_SP,0); }
    break;


/* Line 1267 of yacc.c.  */
#line 3415 "mon_parse.c"
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


#line 710 "mon_parse.y"


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


