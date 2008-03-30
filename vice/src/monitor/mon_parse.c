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
     REG_ASGN_SEP = 276,
     EQUALS = 277,
     TRAIL = 278,
     CMD_SEP = 279,
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
     L_PAREN = 343,
     R_PAREN = 344,
     ARG_IMMEDIATE = 345,
     REG_A = 346,
     REG_X = 347,
     REG_Y = 348,
     COMMA = 349,
     INST_SEP = 350,
     REG_B = 351,
     REG_C = 352,
     REG_D = 353,
     REG_E = 354,
     REG_H = 355,
     REG_L = 356,
     REG_AF = 357,
     REG_BC = 358,
     REG_DE = 359,
     REG_HL = 360,
     REG_IX = 361,
     REG_IY = 362,
     REG_SP = 363,
     REG_IXH = 364,
     REG_IXL = 365,
     REG_IYH = 366,
     REG_IYL = 367,
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
#define REG_ASGN_SEP 276
#define EQUALS 277
#define TRAIL 278
#define CMD_SEP 279
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
#define L_PAREN 343
#define R_PAREN 344
#define ARG_IMMEDIATE 345
#define REG_A 346
#define REG_X 347
#define REG_Y 348
#define COMMA 349
#define INST_SEP 350
#define REG_B 351
#define REG_C 352
#define REG_D 353
#define REG_E 354
#define REG_H 355
#define REG_L 356
#define REG_AF 357
#define REG_BC 358
#define REG_DE 359
#define REG_HL 360
#define REG_IX 361
#define REG_IY 362
#define REG_SP 363
#define REG_IXH 364
#define REG_IXL 365
#define REG_IYH 366
#define REG_IYL 367
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
#line 116 "mon_parse.y"
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
/* Line 193 of yacc.c.  */
#line 475 "mon_parse.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 488 "mon_parse.c"

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
#define YYFINAL  241
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1359

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  133
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  47
/* YYNRULES -- Number of rules.  */
#define YYNRULES  229
/* YYNRULES -- Number of states.  */
#define YYNSTATES  441

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   381

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
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
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    15,    17,    19,
      21,    23,    25,    27,    29,    31,    33,    35,    37,    39,
      41,    43,    45,    48,    52,    56,    62,    66,    69,    73,
      76,    80,    84,    87,    92,    95,   100,   103,   108,   111,
     116,   119,   121,   124,   128,   132,   138,   142,   148,   152,
     158,   162,   168,   172,   175,   176,   182,   186,   190,   193,
     199,   205,   211,   217,   223,   227,   230,   234,   237,   241,
     244,   248,   251,   255,   258,   262,   266,   272,   278,   282,
     285,   288,   291,   294,   298,   302,   306,   312,   316,   319,
     325,   331,   336,   340,   343,   347,   350,   354,   357,   360,
     364,   368,   371,   375,   379,   383,   387,   391,   397,   403,
     409,   413,   418,   424,   429,   435,   440,   446,   452,   456,
     459,   463,   468,   471,   473,   475,   477,   479,   481,   483,
     484,   486,   489,   493,   495,   499,   501,   503,   505,   507,
     511,   513,   517,   520,   521,   523,   527,   529,   531,   532,
     534,   536,   538,   540,   542,   544,   546,   550,   554,   558,
     562,   566,   570,   572,   576,   580,   584,   588,   590,   592,
     594,   598,   600,   602,   604,   606,   608,   610,   612,   614,
     616,   618,   620,   622,   624,   626,   628,   630,   632,   636,
     640,   643,   646,   648,   650,   653,   655,   659,   663,   667,
     673,   679,   683,   687,   691,   695,   699,   703,   709,   715,
     721,   727,   728,   730,   732,   734,   736,   738,   740,   742,
     744,   746,   748,   750,   752,   754,   756,   758,   760,   762
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     134,     0,    -1,   135,    -1,   177,    23,    -1,    23,    -1,
     137,    -1,   135,   137,    -1,    24,    -1,    23,    -1,     1,
      -1,   138,    -1,   140,    -1,   143,    -1,   141,    -1,   144,
      -1,   145,    -1,   146,    -1,   147,    -1,   148,    -1,   149,
      -1,   150,    -1,    13,    -1,    67,   136,    -1,    67,   164,
     136,    -1,    67,   118,   136,    -1,    67,   164,   163,   118,
     136,    -1,    38,   162,   136,    -1,    46,   136,    -1,    87,
     119,   136,    -1,    26,   136,    -1,    49,   152,   136,    -1,
      50,   152,   136,    -1,    58,   136,    -1,    58,   163,   167,
     136,    -1,    57,   136,    -1,    57,   163,   167,   136,    -1,
      29,   136,    -1,    29,   163,   167,   136,    -1,    30,   136,
      -1,    30,   163,   167,   136,    -1,    85,   136,    -1,   139,
      -1,    39,   136,    -1,    39,   164,   136,    -1,    39,   156,
     136,    -1,    68,   164,   163,   152,   136,    -1,    68,   152,
     136,    -1,    69,   164,   163,   152,   136,    -1,    69,   152,
     136,    -1,    70,   162,   163,   117,   136,    -1,    71,   117,
     136,    -1,    71,   164,   163,   117,   136,    -1,    72,   164,
     136,    -1,    72,   136,    -1,    -1,    55,   162,   142,   178,
     136,    -1,    55,   162,   136,    -1,    56,   159,   136,    -1,
      56,   136,    -1,    37,   160,   163,   162,   136,    -1,    48,
     160,   163,   162,   136,    -1,    36,   160,   163,   170,   136,
      -1,    35,   160,   163,   170,   136,    -1,    43,   122,   163,
     159,   136,    -1,    43,   159,   136,    -1,    43,   136,    -1,
      76,   159,   136,    -1,    76,   136,    -1,    77,   159,   136,
      -1,    77,   136,    -1,    78,   159,   136,    -1,    78,   136,
      -1,    79,   159,   136,    -1,    79,   136,    -1,    44,   159,
     136,    -1,    86,   159,   136,    -1,    44,   159,    15,   168,
     136,    -1,    62,   154,   163,   159,   136,    -1,    45,   159,
     136,    -1,    44,   136,    -1,    86,   136,    -1,    45,   136,
      -1,    62,   136,    -1,   124,   158,   136,    -1,   125,   158,
     136,    -1,    34,   158,   136,    -1,    34,   158,   163,   167,
     136,    -1,    52,   158,   136,    -1,    52,   136,    -1,    53,
     158,    15,   168,   136,    -1,    54,   158,   163,   113,   136,
      -1,    54,   158,     1,   136,    -1,    25,   126,   136,    -1,
      25,   136,    -1,    42,   122,   136,    -1,    42,   136,    -1,
      60,   164,   136,    -1,    65,   136,    -1,    51,   136,    -1,
      63,   151,   136,    -1,    59,   167,   136,    -1,    61,   136,
      -1,    61,   151,   136,    -1,    64,   151,   136,    -1,     7,
     167,   136,    -1,    66,   151,   136,    -1,    82,   151,   136,
      -1,    31,   152,   153,   161,   136,    -1,    83,   152,   153,
     161,   136,    -1,    32,   152,   153,   160,   136,    -1,    32,
     152,     1,    -1,    32,   152,   153,     1,    -1,    84,   152,
     153,   160,   136,    -1,    84,   152,   153,     1,    -1,    33,
     152,   153,   162,   136,    -1,    33,   152,   153,     1,    -1,
      27,   167,   167,   161,   136,    -1,    28,   167,   167,   162,
     136,    -1,    73,   152,   136,    -1,    74,   136,    -1,    75,
     152,   136,    -1,    80,   162,   170,   136,    -1,    81,   136,
      -1,   115,    -1,   114,    -1,     1,    -1,   167,    -1,     1,
      -1,    14,    -1,    -1,   120,    -1,   164,   120,    -1,   156,
      21,   157,    -1,   157,    -1,   155,    22,   175,    -1,   173,
      -1,     1,    -1,   160,    -1,   162,    -1,   162,   163,   162,
      -1,     9,    -1,   164,   163,     9,    -1,   163,   162,    -1,
      -1,   165,    -1,   164,   163,   165,    -1,   117,    -1,    94,
      -1,    -1,    16,    -1,    17,    -1,    18,    -1,    19,    -1,
      20,    -1,   166,    -1,   175,    -1,   167,   127,   167,    -1,
     167,   128,   167,    -1,   167,   129,   167,    -1,   167,   130,
     167,    -1,   131,   167,   132,    -1,   131,   167,     1,    -1,
     172,    -1,   168,   121,   168,    -1,   168,   121,     1,    -1,
      88,   168,    89,    -1,    88,   168,     1,    -1,   169,    -1,
     155,    -1,   175,    -1,   170,   163,   171,    -1,   171,    -1,
     175,    -1,   113,    -1,   175,    -1,   155,    -1,     4,    -1,
      12,    -1,    11,    -1,    10,    -1,    12,    -1,    11,    -1,
      10,    -1,     3,    -1,     4,    -1,     5,    -1,     6,    -1,
     174,    -1,   176,    95,   177,    -1,   177,    95,   177,    -1,
     177,    95,    -1,   116,   179,    -1,   177,    -1,   176,    -1,
      90,   175,    -1,   175,    -1,   175,    94,    92,    -1,   175,
      94,    93,    -1,    88,   175,    89,    -1,    88,   175,    94,
      92,    89,    -1,    88,   175,    89,    94,    93,    -1,    88,
     103,    89,    -1,    88,   104,    89,    -1,    88,   105,    89,
      -1,    88,   106,    89,    -1,    88,   107,    89,    -1,    88,
     108,    89,    -1,    88,   175,    89,    94,    91,    -1,    88,
     175,    89,    94,   105,    -1,    88,   175,    89,    94,   106,
      -1,    88,   175,    89,    94,   107,    -1,    -1,    91,    -1,
      96,    -1,    97,    -1,    98,    -1,    99,    -1,   100,    -1,
     109,    -1,   111,    -1,   101,    -1,   110,    -1,   112,    -1,
     102,    -1,   103,    -1,   104,    -1,   105,    -1,   106,    -1,
     107,    -1,   108,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   166,   166,   167,   168,   171,   172,   175,   176,   177,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   194,   196,   198,   200,   202,   204,   206,   208,
     210,   212,   214,   216,   218,   220,   222,   224,   226,   228,
     230,   232,   235,   237,   239,   242,   248,   254,   256,   258,
     260,   262,   264,   266,   271,   270,   273,   275,   277,   281,
     283,   285,   287,   289,   291,   293,   295,   297,   299,   301,
     303,   305,   307,   309,   313,   318,   323,   329,   335,   340,
     342,   344,   346,   351,   353,   355,   357,   359,   361,   363,
     365,   367,   371,   373,   378,   380,   398,   403,   405,   409,
     411,   413,   415,   417,   419,   421,   423,   427,   429,   431,
     433,   435,   437,   439,   441,   443,   445,   447,   451,   453,
     455,   459,   461,   465,   468,   469,   472,   473,   476,   477,
     480,   481,   484,   485,   488,   492,   493,   496,   497,   500,
     501,   503,   507,   508,   511,   516,   521,   531,   532,   535,
     536,   537,   538,   539,   542,   544,   546,   547,   548,   549,
     550,   551,   552,   555,   560,   562,   564,   566,   570,   576,
     584,   585,   588,   589,   592,   593,   596,   597,   598,   599,
     602,   603,   604,   607,   608,   609,   610,   611,   614,   615,
     616,   619,   629,   630,   633,   637,   642,   647,   652,   654,
     656,   658,   659,   660,   661,   662,   663,   664,   666,   668,
     670,   672,   673,   674,   675,   676,   677,   678,   679,   680,
     681,   682,   683,   684,   685,   686,   687,   688,   689,   690
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
  "REG_ASGN_SEP", "EQUALS", "TRAIL", "CMD_SEP", "CMD_SIDEFX", "CMD_RETURN",
  "CMD_BLOCK_READ", "CMD_BLOCK_WRITE", "CMD_UP", "CMD_DOWN", "CMD_LOAD",
  "CMD_SAVE", "CMD_VERIFY", "CMD_IGNORE", "CMD_HUNT", "CMD_FILL",
  "CMD_MOVE", "CMD_GOTO", "CMD_REGISTERS", "CMD_READSPACE",
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
  "CMD_BSAVE", "CMD_SCREEN", "CMD_UNTIL", "CMD_CPU", "L_PAREN", "R_PAREN",
  "ARG_IMMEDIATE", "REG_A", "REG_X", "REG_Y", "COMMA", "INST_SEP", "REG_B",
  "REG_C", "REG_D", "REG_E", "REG_H", "REG_L", "REG_AF", "REG_BC",
  "REG_DE", "REG_HL", "REG_IX", "REG_IY", "REG_SP", "REG_IXH", "REG_IXL",
  "REG_IYH", "REG_IYL", "STRING", "FILENAME", "R_O_L", "OPCODE", "LABEL",
  "BANKNAME", "CPUTYPE", "REGISTER", "COMPARE_OP", "RADIX_TYPE",
  "INPUT_SPEC", "CMD_CHECKPT_ON", "CMD_CHECKPT_OFF", "TOGGLE", "'+'",
  "'-'", "'*'", "'/'", "'('", "')'", "$accept", "top_level",
  "command_list", "end_cmd", "command", "machine_state_rules",
  "register_mod", "symbol_table_rules", "asm_rules", "@1", "memory_rules",
  "checkpoint_rules", "checkpoint_control_rules", "monitor_state_rules",
  "monitor_misc_rules", "disk_rules", "cmd_file_rules", "data_entry_rules",
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
     375,   376,   377,   378,   379,   380,   381,    43,    45,    42,
      47,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   133,   134,   134,   134,   135,   135,   136,   136,   136,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   139,   139,   139,   140,   140,   140,   140,   140,
     140,   140,   140,   140,   142,   141,   141,   141,   141,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   146,   146,   146,   146,   146,   146,   146,   147,
     147,   147,   147,   147,   147,   147,   147,   148,   148,   148,
     148,   148,   148,   148,   148,   148,   148,   148,   149,   149,
     149,   150,   150,   151,   152,   152,   153,   153,   154,   154,
     155,   155,   156,   156,   157,   158,   158,   159,   159,   160,
     160,   160,   161,   161,   162,   162,   162,   163,   163,   164,
     164,   164,   164,   164,   165,   166,   167,   167,   167,   167,
     167,   167,   167,   168,   168,   168,   168,   168,   169,   169,
     170,   170,   171,   171,   172,   172,   173,   173,   173,   173,
     174,   174,   174,   175,   175,   175,   175,   175,   176,   176,
     176,   177,   178,   178,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     3,     3,     5,     3,     2,     3,     2,
       3,     3,     2,     4,     2,     4,     2,     4,     2,     4,
       2,     1,     2,     3,     3,     5,     3,     5,     3,     5,
       3,     5,     3,     2,     0,     5,     3,     3,     2,     5,
       5,     5,     5,     5,     3,     2,     3,     2,     3,     2,
       3,     2,     3,     2,     3,     3,     5,     5,     3,     2,
       2,     2,     2,     3,     3,     3,     5,     3,     2,     5,
       5,     4,     3,     2,     3,     2,     3,     2,     2,     3,
       3,     2,     3,     3,     3,     3,     3,     5,     5,     5,
       3,     4,     5,     4,     5,     4,     5,     5,     3,     2,
       3,     4,     2,     1,     1,     1,     1,     1,     1,     0,
       1,     2,     3,     1,     3,     1,     1,     1,     1,     3,
       1,     3,     2,     0,     1,     3,     1,     1,     0,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     3,     3,
       3,     3,     1,     3,     3,     3,     3,     1,     1,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       2,     2,     1,     1,     2,     1,     3,     3,     3,     5,
       5,     3,     3,     3,     3,     3,     3,     5,     5,     5,
       5,     0,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,    21,     4,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   211,     0,     0,     0,     2,     5,
      10,    41,    11,    13,    12,    14,    15,    16,    17,    18,
      19,    20,     0,   183,   184,   185,   186,   182,   181,   180,
     149,   150,   151,   152,   153,   130,     0,   175,     0,     0,
     162,   187,   174,     9,     8,     7,     0,    93,    29,     0,
       0,   147,    36,     0,    38,     0,   125,   124,     0,     0,
       0,   136,   176,   179,   178,   177,     0,   135,   140,   146,
     148,   148,   148,   144,   154,   155,   148,   148,     0,   148,
      42,     0,     0,   133,     0,     0,    95,   148,    65,     0,
     137,   148,    79,     0,    81,     0,    27,   148,     0,     0,
      98,     9,    88,     0,     0,     0,     0,    58,     0,    34,
       0,    32,     0,     0,     0,   123,   101,     0,   128,    82,
     148,     0,     0,    97,     0,     0,    22,     0,     0,   148,
       0,   148,   148,     0,   148,    53,     0,     0,   119,     0,
      67,     0,    69,     0,    71,     0,    73,     0,     0,   122,
       0,     0,     0,    40,    80,     0,     0,     0,     0,   212,
     213,   214,   215,   216,   217,   220,   223,   224,   225,   226,
     227,   228,   229,   218,   221,   219,   222,   195,   191,     0,
       0,     1,     6,     3,     0,   131,     0,     0,     0,     0,
     104,    92,   148,     0,     0,     0,   127,   148,   126,   110,
       0,     0,    85,     0,     0,     0,     0,     0,     0,    26,
       0,     0,     0,    44,    43,    94,     0,    64,     0,    74,
      78,     0,    30,    31,    87,     0,     0,     0,    56,     0,
      57,     0,     0,   100,    96,   102,     0,    99,   103,   105,
      24,    23,     0,    46,     0,    48,     0,     0,    50,     0,
      52,   118,   120,    66,    68,    70,    72,   173,     0,   171,
     172,   106,   148,     0,    75,    28,     0,     0,     0,     0,
       0,     0,     0,   194,     0,    83,    84,   161,   160,   156,
     157,   158,   159,     0,     0,     0,    37,    39,     0,   111,
       0,   115,     0,     0,     0,   139,   141,   145,     0,     0,
     134,   132,     0,     0,   168,     0,   167,   169,     0,     0,
      91,     0,   193,   192,     0,    35,    33,     0,     0,     0,
       0,     0,     0,   121,     0,     0,   113,     0,   201,   202,
     203,   204,   205,   206,   198,     0,   196,   197,   116,   142,
     117,   107,   109,   114,    86,    62,    61,    59,    63,     0,
       0,    76,    60,    89,    90,     0,   190,    55,    77,    25,
      45,    47,    49,    51,   170,   108,   112,     0,     0,   166,
     165,   164,   163,   188,   189,   207,   200,   208,   209,   210,
     199
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    67,    68,   107,    69,    70,    71,    72,    73,   289,
      74,    75,    76,    77,    78,    79,    80,    81,   177,   118,
     257,   180,    97,   142,   143,   126,   149,   150,   343,   151,
     344,    98,   133,   134,   258,   365,   366,   318,   319,   100,
     127,   101,   135,   372,    82,   374,   238
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -277
static const yytype_int16 yypact[] =
{
     867,   719,  -277,  -277,     8,   250,   719,   719,    63,    63,
      37,    37,    37,   277,  1177,  1177,  1177,  1195,   205,    21,
     764,   842,   842,   250,  1177,    37,    37,   250,   393,   277,
     277,  1195,   842,    63,    63,   719,   420,   177,  1085,  -107,
    -107,   250,  -107,   218,   106,   106,  1195,   370,   602,    37,
     250,    37,   842,   842,   842,   842,  1195,   250,  -107,    37,
      37,   250,   842,   -96,  1241,   277,   277,    28,   968,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,
    -277,  -277,    17,  -277,  -277,  -277,  -277,  -277,  -277,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,   719,  -277,   -67,    75,
    -277,  -277,  -277,  -277,  -277,  -277,   250,  -277,  -277,   692,
     692,  -277,  -277,   719,  -277,   719,  -277,  -277,   596,   633,
     596,  -277,  -277,  -277,  -277,  -277,    63,  -277,  -277,  -277,
     -40,   -40,   -40,  -277,  -277,  -277,   -40,   -40,   250,   -40,
    -277,    49,   282,  -277,    38,   250,  -277,   -40,  -277,   250,
    -277,   377,  -277,    87,  -277,   250,  -277,   -40,   250,   250,
    -277,   321,  -277,   250,    44,    96,    77,  -277,   250,  -277,
     719,  -277,   719,    75,   250,  -277,  -277,   250,  -277,  -277,
     -40,   250,   250,  -277,   250,   250,  -277,    26,   250,   -40,
     250,   -40,   -40,   250,   -40,  -277,   250,   250,  -277,   250,
    -277,   250,  -277,   250,  -277,   250,  -277,   250,   347,  -277,
     250,   596,   596,  -277,  -277,   250,   250,  1251,   525,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,  -277,   -34,  -277,   250,
     250,  -277,  -277,  -277,     1,  -277,   719,   719,   719,   719,
    -277,  -277,    69,   553,    75,    75,  -277,   114,   322,   960,
    1109,  1158,  -277,   719,   347,  1195,   536,   347,  1195,  -277,
     525,   525,   356,  -277,  -277,  -277,  1177,  -277,  1053,  -277,
    -277,  1195,  -277,  -277,  -277,  1053,   250,   -41,  -277,   -39,
    -277,    75,    75,  -277,  -277,  -277,  1177,  -277,  -277,  -277,
    -277,  -277,   -33,  -277,    37,  -277,    37,   -27,  -277,   -21,
    -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  1217,  -277,
    -277,  -277,   114,  1133,  -277,  -277,    53,    67,    73,    83,
      97,    98,   -68,  -277,   -78,  -277,  -277,  -277,  -277,   -82,
     -82,  -277,  -277,   250,  1195,   250,  -277,  -277,   250,  -277,
     250,  -277,   250,    75,  1217,  -277,  -277,  -277,  1217,   250,
    -277,  -277,   250,  1053,  -277,     6,  -277,  -277,   250,     6,
    -277,   250,    11,    57,   250,  -277,  -277,   250,   250,   250,
     250,   250,   250,  -277,   347,   250,  -277,   250,  -277,  -277,
    -277,  -277,  -277,  -277,    91,    99,  -277,  -277,  -277,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,    32,
     788,  -277,  -277,  -277,  -277,   -39,   -39,  -277,  -277,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,  -277,    -2,   126,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,
    -277
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -277,  -277,  -277,   127,   150,  -277,  -277,  -277,  -277,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,   134,    -8,
      47,  -277,   -17,  -277,   -77,   184,     3,    -4,  -238,   324,
     387,   411,  -224,  -277,   514,  -269,  -277,  -151,  -157,  -277,
    -277,  -277,    -1,  -277,  -276,  -277,  -277
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -149
static const yytype_int16 yytable[] =
{
     102,   141,   337,   119,   120,   102,   102,   103,   175,   103,
     130,   136,   137,   373,   396,   397,   369,   158,   159,   348,
     157,   394,   103,   216,   153,   155,   395,   103,   241,   104,
     105,   104,   105,   429,   102,   168,   188,   190,   116,   103,
     243,   197,   357,   199,   104,   105,   357,   248,   249,   104,
     105,   211,   212,   245,   111,   201,   203,   205,   207,   285,
     334,   104,   105,   237,   103,   215,  -148,  -148,  -148,  -148,
    -143,   271,   371,  -148,  -148,  -148,   103,    64,   103,  -148,
    -148,  -148,  -148,  -148,   385,   378,   104,   105,   103,   435,
     381,   436,  -143,  -143,   409,   102,   382,   286,   104,   105,
     104,   105,   278,   437,   438,   439,   415,   116,   102,   102,
     104,   105,   102,   354,   102,  -143,   358,   102,   102,   102,
     111,   430,    90,    91,    92,    93,    94,   410,   246,   247,
     248,   249,   108,   338,   106,   112,   114,  -143,  -143,   433,
     434,   432,   388,   145,  -148,   140,   146,   148,   152,   154,
     156,   117,   416,   410,   160,   162,   389,   111,   245,   167,
     169,   171,   390,   111,   176,   179,   260,   261,   183,   102,
     186,   102,   391,   181,   182,   195,   184,   198,   103,   200,
     202,   204,   206,  -148,   209,   427,   392,   393,   213,   214,
     111,   428,   210,   -54,  -148,   361,   246,   247,   248,   249,
     104,   105,   246,   247,   248,   249,   103,   320,   111,  -148,
     102,   102,   163,   164,   165,   440,   332,   333,   242,   103,
     117,    90,    91,    92,    93,    94,   250,   424,   104,   105,
       0,     0,     0,   251,    90,    91,    92,    93,    94,     0,
       0,   104,   105,     0,     0,   102,   102,   102,   102,   239,
     240,   103,     0,   262,     0,   141,   350,     0,   322,   323,
       0,   364,   102,   320,     0,   269,   320,     0,   364,   273,
     360,   274,   275,   104,   105,     0,   277,   367,   121,   362,
     279,   122,   280,   103,   367,   282,   283,   123,   124,   125,
     284,     0,   175,   288,     0,   290,   379,     0,   380,   377,
     293,   294,     0,   272,   295,   104,   105,     0,   297,   298,
       0,   299,   300,     0,   301,   303,     0,   305,     0,   387,
     308,     0,  -136,   310,   311,    95,   312,     0,   313,     0,
     314,     0,   315,     0,   316,     0,   185,   321,   131,   131,
     131,   138,   324,   325,  -136,  -136,   364,     0,   131,     0,
      83,    84,    85,    86,     0,   166,     0,    87,    88,    89,
       0,     0,   367,     0,     0,     0,   335,   336,     0,     0,
     192,     0,    90,    91,    92,    93,    94,     0,  -138,     0,
     208,   346,   347,   320,     0,     0,    90,    91,    92,    93,
      94,     0,  -138,   364,   161,   113,   115,   122,     0,     0,
    -138,  -138,     0,   123,   124,   125,     0,     0,     0,   367,
       0,     0,     0,   370,     0,     0,   104,   105,   375,   376,
     170,   172,     0,     0,     0,   132,   132,   132,   139,   144,
       0,   132,   132,   132,     0,   132,    90,    91,    92,    93,
      94,     0,   139,   132,     0,   383,     0,   174,     0,   246,
     247,   248,   249,     0,   187,   189,   191,   139,   194,   196,
     317,     0,     0,   132,   132,   132,   132,   139,     0,     0,
     398,   111,   400,   132,     0,   401,    95,   402,     0,   403,
     404,   405,     0,     0,     0,   406,   407,   193,     0,   408,
       0,     0,   411,     0,     0,   412,   413,     0,   414,     0,
       0,   417,     0,     0,   418,   419,   420,   421,   422,   423,
       0,     0,   425,   263,   426,    99,     0,   264,   265,   266,
     109,   110,     0,   267,   268,     0,   270,     0,    83,    84,
      85,    86,     0,     0,   276,    87,    88,    89,   265,    83,
      84,    85,    86,     0,   281,   356,    87,    88,    89,   173,
       0,     0,   287,     0,     0,     0,    83,    84,    85,    86,
       0,     0,     0,    87,    88,    89,     0,   296,     0,    90,
      91,    92,    93,    94,   302,     0,   304,   345,   306,   307,
       0,   309,     0,     0,   131,   352,     0,     0,     0,   355,
       0,     0,   359,     0,     0,     0,     0,   256,     0,    83,
      84,    85,    86,   103,     0,   368,    87,    88,    89,     0,
     244,     0,    90,    91,    92,    93,    94,     0,    90,    91,
      92,    93,    94,   252,   253,   104,   105,   254,     0,   255,
       0,     0,     0,     0,   259,     0,    83,    84,    85,    86,
       0,     0,     0,    87,    88,    89,     0,   131,     0,    90,
      91,    92,    93,    94,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   139,     0,     0,     0,   399,     0,
     129,   132,   139,     0,     0,     0,   139,     0,     0,   139,
     246,   247,   248,   249,   291,     0,   292,   132,     0,     0,
       0,     0,   139,     0,     0,    83,    84,    85,    86,     0,
       0,     0,    87,    88,    89,   384,     0,   132,    90,    91,
      92,    93,    94,     0,     0,     0,    95,     0,     0,     0,
       0,     0,    83,    84,    85,    86,     0,    96,     0,    87,
      88,    89,     0,     0,   132,    90,    91,    92,    93,    94,
       0,   384,     0,     0,     0,   384,     0,     0,     0,     0,
       0,     0,     0,    95,     0,   139,     0,     0,     0,     0,
     339,   340,   341,   342,    96,   103,     0,    83,    84,    85,
      86,     0,     0,   128,    87,    88,    89,   353,     0,     0,
      90,    91,    92,    93,    94,     0,     0,   104,   105,   431,
       0,    83,    84,    85,    86,     0,     0,     0,    87,    88,
      89,     0,     0,     0,    90,    91,    92,    93,    94,     0,
       0,     0,    95,     0,     0,     0,     0,     0,     0,   246,
     247,   248,   249,    96,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
       0,     0,     0,   103,     0,    83,    84,    85,    86,     0,
      96,   128,    87,    88,    89,     0,     0,     0,    90,    91,
      92,    93,    94,     0,     0,   104,   105,     0,     0,     0,
       0,     0,     0,     0,     1,     0,   363,     0,     0,     0,
       2,   129,     0,     0,     0,     0,   147,     0,     0,     0,
       3,     0,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,     0,    95,    19,
      20,    21,    22,    23,     0,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,     0,     0,     0,     0,   129,
       0,  -127,     0,  -127,  -127,  -127,  -127,     0,     0,  -127,
    -127,  -127,  -127,     0,     0,     1,  -127,  -127,  -127,  -127,
    -127,     2,     0,    64,     0,     0,     0,     0,     0,     0,
       0,    65,    66,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,     0,     0,
      19,    20,    21,    22,    23,     0,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    83,    84,    85,    86,
       0,     0,     0,    87,    88,    89,     0,     0,     0,    90,
      91,    92,    93,    94,     0,     0,     0,  -127,     0,     0,
       0,     0,     0,     0,     0,     0,   103,     0,  -129,  -129,
    -129,  -129,    65,    66,  -129,  -129,  -129,  -129,     0,   178,
       0,  -129,  -129,  -129,  -129,  -129,     0,     0,   104,   105,
     349,     0,    83,    84,    85,    86,     0,     0,   128,    87,
      88,    89,     0,     0,     0,    90,    91,    92,    93,    94,
       0,     0,     0,     0,   386,     0,    83,    84,    85,    86,
       0,   363,   128,    87,    88,    89,     0,     0,     0,    90,
      91,    92,    93,    94,     0,     0,     0,     0,     0,   351,
       0,    83,    84,    85,    86,     0,     0,     0,    87,    88,
      89,     0,     0,    95,    90,    91,    92,    93,    94,  -129,
      83,    84,    85,    86,     0,     0,   128,    87,    88,    89,
       0,     0,     0,    90,    91,    92,    93,    94,    83,    84,
      85,    86,  -129,     0,     0,    87,    88,    89,     0,     0,
       0,    90,    91,    92,    93,    94,     0,     0,   103,     0,
    -148,  -148,  -148,  -148,     0,     0,   129,  -148,  -148,  -148,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     104,   105,     0,     0,    83,    84,    85,    86,     0,     0,
     129,    87,    88,    89,    83,    84,    85,    86,     0,     0,
       0,    87,    88,    89,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   129,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   129,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   111,   129,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   217,
    -148,   218,   219,     0,     0,     0,     0,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   326,   327,   328,   329,   330,   331
};

static const yytype_int16 yycheck[] =
{
       1,    18,     1,    11,    12,     6,     7,     1,   115,     1,
      14,    15,    16,   289,    92,    93,   285,    25,    26,   257,
      24,    89,     1,   119,    21,    22,    94,     1,     0,    23,
      24,    23,    24,     1,    35,    32,    44,    45,     1,     1,
      23,    49,   266,    51,    23,    24,   270,   129,   130,    23,
      24,    59,    60,   120,    94,    52,    53,    54,    55,    15,
      94,    23,    24,    64,     1,    62,     3,     4,     5,     6,
       1,    22,   113,    10,    11,    12,     1,   116,     1,    16,
      17,    18,    19,    20,   322,   118,    23,    24,     1,    91,
     117,    93,    23,    24,   363,    96,   117,     1,    23,    24,
      23,    24,    15,   105,   106,   107,    95,     1,   109,   110,
      23,    24,   113,   264,   115,     1,   267,   118,   119,   120,
      94,    89,    16,    17,    18,    19,    20,   121,   127,   128,
     129,   130,     5,   132,   126,     8,     9,    23,    24,   415,
     416,   410,    89,   122,   118,    18,    19,    20,    21,    22,
      23,   114,    95,   121,    27,    28,    89,    94,   120,    32,
      33,    34,    89,    94,    37,    38,   119,   120,    41,   170,
      43,   172,    89,    39,    40,    48,    42,    50,     1,    52,
      53,    54,    55,   120,    57,    94,    89,    89,    61,    62,
      94,    92,    58,   116,   131,   272,   127,   128,   129,   130,
      23,    24,   127,   128,   129,   130,     1,   208,    94,   113,
     211,   212,    28,    29,    30,    89,   217,   218,    68,     1,
     114,    16,    17,    18,    19,    20,    99,   384,    23,    24,
      -1,    -1,    -1,   106,    16,    17,    18,    19,    20,    -1,
      -1,    23,    24,    -1,    -1,   246,   247,   248,   249,    65,
      66,     1,    -1,   126,    -1,   272,   260,    -1,   211,   212,
      -1,   278,   263,   264,    -1,   138,   267,    -1,   285,   142,
     271,   144,   145,    23,    24,    -1,   149,   278,     1,   276,
     153,     4,   155,     1,   285,   158,   159,    10,    11,    12,
     163,    -1,   115,   166,    -1,   168,   304,    -1,   306,   296,
     173,   174,    -1,    21,   177,    23,    24,    -1,   181,   182,
      -1,   184,   185,    -1,   187,   188,    -1,   190,    -1,   323,
     193,    -1,     1,   196,   197,   120,   199,    -1,   201,    -1,
     203,    -1,   205,    -1,   207,    -1,   118,   210,    14,    15,
      16,    17,   215,   216,    23,    24,   363,    -1,    24,    -1,
       3,     4,     5,     6,    -1,    31,    -1,    10,    11,    12,
      -1,    -1,   363,    -1,    -1,    -1,   239,   240,    -1,    -1,
      46,    -1,    16,    17,    18,    19,    20,    -1,     1,    -1,
      56,   254,   255,   384,    -1,    -1,    16,    17,    18,    19,
      20,    -1,    15,   410,     1,     8,     9,     4,    -1,    -1,
      23,    24,    -1,    10,    11,    12,    -1,    -1,    -1,   410,
      -1,    -1,    -1,   286,    -1,    -1,    23,    24,   291,   292,
      33,    34,    -1,    -1,    -1,    14,    15,    16,    17,    18,
      -1,    20,    21,    22,    -1,    24,    16,    17,    18,    19,
      20,    -1,    31,    32,    -1,   318,    -1,    36,    -1,   127,
     128,   129,   130,    -1,    43,    44,    45,    46,    47,    48,
     113,    -1,    -1,    52,    53,    54,    55,    56,    -1,    -1,
     343,    94,   345,    62,    -1,   348,   120,   350,    -1,   352,
     353,   354,    -1,    -1,    -1,   358,   359,   117,    -1,   362,
      -1,    -1,   365,    -1,    -1,   368,   369,    -1,   371,    -1,
      -1,   374,    -1,    -1,   377,   378,   379,   380,   381,   382,
      -1,    -1,   385,   126,   387,     1,    -1,   130,   131,   132,
       6,     7,    -1,   136,   137,    -1,   139,    -1,     3,     4,
       5,     6,    -1,    -1,   147,    10,    11,    12,   151,     3,
       4,     5,     6,    -1,   157,     9,    10,    11,    12,    35,
      -1,    -1,   165,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    -1,    -1,    10,    11,    12,    -1,   180,    -1,    16,
      17,    18,    19,    20,   187,    -1,   189,   253,   191,   192,
      -1,   194,    -1,    -1,   260,   261,    -1,    -1,    -1,   265,
      -1,    -1,   268,    -1,    -1,    -1,    -1,     1,    -1,     3,
       4,     5,     6,     1,    -1,   281,    10,    11,    12,    -1,
      96,    -1,    16,    17,    18,    19,    20,    -1,    16,    17,
      18,    19,    20,   109,   110,    23,    24,   113,    -1,   115,
      -1,    -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,
      -1,    -1,    -1,    10,    11,    12,    -1,   323,    -1,    16,
      17,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   253,    -1,    -1,    -1,   344,    -1,
     117,   260,   261,    -1,    -1,    -1,   265,    -1,    -1,   268,
     127,   128,   129,   130,   170,    -1,   172,   276,    -1,    -1,
      -1,    -1,   281,    -1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    10,    11,    12,   318,    -1,   296,    16,    17,
      18,    19,    20,    -1,    -1,    -1,   120,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,    -1,   131,    -1,    10,
      11,    12,    -1,    -1,   323,    16,    17,    18,    19,    20,
      -1,   354,    -1,    -1,    -1,   358,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   120,    -1,   344,    -1,    -1,    -1,    -1,
     246,   247,   248,   249,   131,     1,    -1,     3,     4,     5,
       6,    -1,    -1,     9,    10,    11,    12,   263,    -1,    -1,
      16,    17,    18,    19,    20,    -1,    -1,    23,    24,     1,
      -1,     3,     4,     5,     6,    -1,    -1,    -1,    10,    11,
      12,    -1,    -1,    -1,    16,    17,    18,    19,    20,    -1,
      -1,    -1,   120,    -1,    -1,    -1,    -1,    -1,    -1,   127,
     128,   129,   130,   131,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   120,
      -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,    -1,
     131,     9,    10,    11,    12,    -1,    -1,    -1,    16,    17,
      18,    19,    20,    -1,    -1,    23,    24,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,    88,    -1,    -1,    -1,
      13,   117,    -1,    -1,    -1,    -1,   122,    -1,    -1,    -1,
      23,    -1,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    -1,   120,    42,
      43,    44,    45,    46,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    -1,    -1,    -1,    -1,   117,
      -1,     1,    -1,     3,     4,     5,     6,    -1,    -1,     9,
      10,    11,    12,    -1,    -1,     7,    16,    17,    18,    19,
      20,    13,    -1,   116,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   124,   125,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    -1,    -1,
      42,    43,    44,    45,    46,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,     3,     4,     5,     6,
      -1,    -1,    -1,    10,    11,    12,    -1,    -1,    -1,    16,
      17,    18,    19,    20,    -1,    -1,    -1,   117,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,     3,     4,
       5,     6,   124,   125,     9,    10,    11,    12,    -1,    14,
      -1,    16,    17,    18,    19,    20,    -1,    -1,    23,    24,
       1,    -1,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    -1,    -1,    -1,    16,    17,    18,    19,    20,
      -1,    -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,
      -1,    88,     9,    10,    11,    12,    -1,    -1,    -1,    16,
      17,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,     1,
      -1,     3,     4,     5,     6,    -1,    -1,    -1,    10,    11,
      12,    -1,    -1,   120,    16,    17,    18,    19,    20,    94,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      -1,    -1,    -1,    16,    17,    18,    19,    20,     3,     4,
       5,     6,   117,    -1,    -1,    10,    11,    12,    -1,    -1,
      -1,    16,    17,    18,    19,    20,    -1,    -1,     1,    -1,
       3,     4,     5,     6,    -1,    -1,   117,    10,    11,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      23,    24,    -1,    -1,     3,     4,     5,     6,    -1,    -1,
     117,    10,    11,    12,     3,     4,     5,     6,    -1,    -1,
      -1,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   117,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    94,   117,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,
     113,    90,    91,    -1,    -1,    -1,    -1,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   103,   104,   105,   106,   107,   108
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     7,    13,    23,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    42,
      43,    44,    45,    46,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,   116,   124,   125,   134,   135,   137,
     138,   139,   140,   141,   143,   144,   145,   146,   147,   148,
     149,   150,   177,     3,     4,     5,     6,    10,    11,    12,
      16,    17,    18,    19,    20,   120,   131,   155,   164,   167,
     172,   174,   175,     1,    23,    24,   126,   136,   136,   167,
     167,    94,   136,   163,   136,   163,     1,   114,   152,   152,
     152,     1,     4,    10,    11,    12,   158,   173,     9,   117,
     160,   162,   164,   165,   166,   175,   160,   160,   162,   164,
     136,   155,   156,   157,   164,   122,   136,   122,   136,   159,
     160,   162,   136,   159,   136,   159,   136,   160,   152,   152,
     136,     1,   136,   158,   158,   158,   162,   136,   159,   136,
     163,   136,   163,   167,   164,   115,   136,   151,    14,   136,
     154,   151,   151,   136,   151,   118,   136,   164,   152,   164,
     152,   164,   162,   117,   164,   136,   164,   152,   136,   152,
     136,   159,   136,   159,   136,   159,   136,   159,   162,   136,
     151,   152,   152,   136,   136,   159,   119,    88,    90,    91,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   175,   179,   158,
     158,     0,   137,    23,   167,   120,   127,   128,   129,   130,
     136,   136,   167,   167,   167,   167,     1,   153,   167,     1,
     153,   153,   136,   163,   163,   163,   163,   163,   163,   136,
     163,    22,    21,   136,   136,   136,   163,   136,    15,   136,
     136,   163,   136,   136,   136,    15,     1,   163,   136,   142,
     136,   167,   167,   136,   136,   136,   163,   136,   136,   136,
     136,   136,   163,   136,   163,   136,   163,   163,   136,   163,
     136,   136,   136,   136,   136,   136,   136,   113,   170,   171,
     175,   136,   153,   153,   136,   136,   103,   104,   105,   106,
     107,   108,   175,   175,    94,   136,   136,     1,   132,   167,
     167,   167,   167,   161,   163,   162,   136,   136,   161,     1,
     160,     1,   162,   167,   170,   162,     9,   165,   170,   162,
     175,   157,   159,    88,   155,   168,   169,   175,   162,   168,
     136,   113,   176,   177,   178,   136,   136,   159,   118,   152,
     152,   117,   117,   136,   163,   161,     1,   160,    89,    89,
      89,    89,    89,    89,    89,    94,    92,    93,   136,   162,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   168,
     121,   136,   136,   136,   136,    95,    95,   136,   136,   136,
     136,   136,   136,   136,   171,   136,   136,    94,    92,     1,
      89,     1,   168,   177,   177,    91,    93,   105,   106,   107,
      89
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
#line 166 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 3:
#line 167 "mon_parse.y"
    { (yyval.i) = 0; }
    break;

  case 4:
#line 168 "mon_parse.y"
    { new_cmd = 1; asm_mode = 0;  (yyval.i) = 0; }
    break;

  case 9:
#line 177 "mon_parse.y"
    { return ERR_EXPECT_END_CMD; }
    break;

  case 21:
#line 191 "mon_parse.y"
    { return ERR_BAD_CMD; }
    break;

  case 22:
#line 195 "mon_parse.y"
    { mon_bank(e_default_space, NULL); }
    break;

  case 23:
#line 197 "mon_parse.y"
    { mon_bank((yyvsp[(2) - (3)].i), NULL); }
    break;

  case 24:
#line 199 "mon_parse.y"
    { mon_bank(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 25:
#line 201 "mon_parse.y"
    { mon_bank((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 26:
#line 203 "mon_parse.y"
    { mon_jump((yyvsp[(2) - (3)].a)); }
    break;

  case 27:
#line 205 "mon_parse.y"
    { mon_display_io_regs(); }
    break;

  case 28:
#line 207 "mon_parse.y"
    { monitor_cpu_type_set((yyvsp[(2) - (3)].str)); }
    break;

  case 29:
#line 209 "mon_parse.y"
    { mon_instruction_return(); }
    break;

  case 30:
#line 211 "mon_parse.y"
    { machine_write_snapshot((yyvsp[(2) - (3)].str),0,0,0); /* FIXME */ }
    break;

  case 31:
#line 213 "mon_parse.y"
    { machine_read_snapshot((yyvsp[(2) - (3)].str), 0); }
    break;

  case 32:
#line 215 "mon_parse.y"
    { mon_instructions_step(-1); }
    break;

  case 33:
#line 217 "mon_parse.y"
    { mon_instructions_step((yyvsp[(3) - (4)].i)); }
    break;

  case 34:
#line 219 "mon_parse.y"
    { mon_instructions_next(-1); }
    break;

  case 35:
#line 221 "mon_parse.y"
    { mon_instructions_next((yyvsp[(3) - (4)].i)); }
    break;

  case 36:
#line 223 "mon_parse.y"
    { mon_stack_up(-1); }
    break;

  case 37:
#line 225 "mon_parse.y"
    { mon_stack_up((yyvsp[(3) - (4)].i)); }
    break;

  case 38:
#line 227 "mon_parse.y"
    { mon_stack_down(-1); }
    break;

  case 39:
#line 229 "mon_parse.y"
    { mon_stack_down((yyvsp[(3) - (4)].i)); }
    break;

  case 40:
#line 231 "mon_parse.y"
    { mon_display_screen(); }
    break;

  case 42:
#line 236 "mon_parse.y"
    { (monitor_cpu_type.mon_register_print)(default_memspace); }
    break;

  case 43:
#line 238 "mon_parse.y"
    { (monitor_cpu_type.mon_register_print)((yyvsp[(2) - (3)].i)); }
    break;

  case 45:
#line 243 "mon_parse.y"
    {
                        /* What about the memspace? */
                        playback = TRUE; playback_name = (yyvsp[(4) - (5)].str);
                        /*mon_load_symbols($2, $3);*/
                    }
    break;

  case 46:
#line 249 "mon_parse.y"
    {
                        /* What about the memspace? */
                        playback = TRUE; playback_name = (yyvsp[(2) - (3)].str);
                        /*mon_load_symbols($2, $3);*/
                    }
    break;

  case 47:
#line 255 "mon_parse.y"
    { mon_save_symbols((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 48:
#line 257 "mon_parse.y"
    { mon_save_symbols(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 49:
#line 259 "mon_parse.y"
    { mon_add_name_to_symbol_table((yyvsp[(2) - (5)].a), (yyvsp[(4) - (5)].str)); }
    break;

  case 50:
#line 261 "mon_parse.y"
    { mon_remove_name_from_symbol_table(e_default_space, (yyvsp[(2) - (3)].str)); }
    break;

  case 51:
#line 263 "mon_parse.y"
    { mon_remove_name_from_symbol_table((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 52:
#line 265 "mon_parse.y"
    { mon_print_symbol_table((yyvsp[(2) - (3)].i)); }
    break;

  case 53:
#line 267 "mon_parse.y"
    { mon_print_symbol_table(e_default_space); }
    break;

  case 54:
#line 271 "mon_parse.y"
    { mon_start_assemble_mode((yyvsp[(2) - (2)].a), NULL); }
    break;

  case 55:
#line 272 "mon_parse.y"
    { }
    break;

  case 56:
#line 274 "mon_parse.y"
    { mon_start_assemble_mode((yyvsp[(2) - (3)].a), NULL); }
    break;

  case 57:
#line 276 "mon_parse.y"
    { mon_disassemble_lines((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1]); }
    break;

  case 58:
#line 278 "mon_parse.y"
    { mon_disassemble_lines(BAD_ADDR, BAD_ADDR); }
    break;

  case 59:
#line 282 "mon_parse.y"
    { mon_memory_move((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], (yyvsp[(4) - (5)].a)); }
    break;

  case 60:
#line 284 "mon_parse.y"
    { mon_memory_compare((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[0], (yyvsp[(4) - (5)].a)); }
    break;

  case 61:
#line 286 "mon_parse.y"
    { mon_memory_fill((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1],(unsigned char *)(yyvsp[(4) - (5)].str)); }
    break;

  case 62:
#line 288 "mon_parse.y"
    { mon_memory_hunt((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1],(unsigned char *)(yyvsp[(4) - (5)].str)); }
    break;

  case 63:
#line 290 "mon_parse.y"
    { mon_memory_display((yyvsp[(2) - (5)].rt), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], DF_PETSCII); }
    break;

  case 64:
#line 292 "mon_parse.y"
    { mon_memory_display(default_radix, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_PETSCII); }
    break;

  case 65:
#line 294 "mon_parse.y"
    { mon_memory_display(default_radix, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
    break;

  case 66:
#line 296 "mon_parse.y"
    { mon_memory_display_data((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], 8, 8); }
    break;

  case 67:
#line 298 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 8, 8); }
    break;

  case 68:
#line 300 "mon_parse.y"
    { mon_memory_display_data((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], 24, 21); }
    break;

  case 69:
#line 302 "mon_parse.y"
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 24, 21); }
    break;

  case 70:
#line 304 "mon_parse.y"
    { mon_memory_display(0, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_PETSCII); }
    break;

  case 71:
#line 306 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
    break;

  case 72:
#line 308 "mon_parse.y"
    { mon_memory_display(0, (yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], DF_SCREEN_CODE); }
    break;

  case 73:
#line 310 "mon_parse.y"
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_SCREEN_CODE); }
    break;

  case 74:
#line 314 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], FALSE, FALSE,
                                                    FALSE, FALSE);
                  }
    break;

  case 75:
#line 319 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], FALSE, FALSE,
                                                    FALSE, TRUE);
                  }
    break;

  case 76:
#line 324 "mon_parse.y"
    {
                      temp = mon_breakpoint_add_checkpoint((yyvsp[(2) - (5)].range)[0], (yyvsp[(2) - (5)].range)[1], FALSE,
                                                           FALSE, FALSE, FALSE);
                      mon_breakpoint_set_checkpoint_condition(temp, (yyvsp[(4) - (5)].cond_node));
                  }
    break;

  case 77:
#line 330 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], FALSE,
                      ((yyvsp[(2) - (5)].i) == e_load || (yyvsp[(2) - (5)].i) == e_load_store),
                      ((yyvsp[(2) - (5)].i) == e_store || (yyvsp[(2) - (5)].i) == e_load_store), FALSE);
                  }
    break;

  case 78:
#line 336 "mon_parse.y"
    {
                      mon_breakpoint_add_checkpoint((yyvsp[(2) - (3)].range)[0], (yyvsp[(2) - (3)].range)[1], TRUE, FALSE, FALSE,
                                                    FALSE);
                  }
    break;

  case 79:
#line 341 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 80:
#line 343 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 81:
#line 345 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 82:
#line 347 "mon_parse.y"
    { mon_breakpoint_print_checkpoints(); }
    break;

  case 83:
#line 352 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_ON, (yyvsp[(2) - (3)].i)); }
    break;

  case 84:
#line 354 "mon_parse.y"
    { mon_breakpoint_switch_checkpoint(e_OFF, (yyvsp[(2) - (3)].i)); }
    break;

  case 85:
#line 356 "mon_parse.y"
    { mon_breakpoint_set_ignore_count((yyvsp[(2) - (3)].i), -1); }
    break;

  case 86:
#line 358 "mon_parse.y"
    { mon_breakpoint_set_ignore_count((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].i)); }
    break;

  case 87:
#line 360 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint((yyvsp[(2) - (3)].i)); }
    break;

  case 88:
#line 362 "mon_parse.y"
    { mon_breakpoint_delete_checkpoint(-1); }
    break;

  case 89:
#line 364 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_condition((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].cond_node)); }
    break;

  case 90:
#line 366 "mon_parse.y"
    { mon_breakpoint_set_checkpoint_command((yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].str)); }
    break;

  case 91:
#line 368 "mon_parse.y"
    { return ERR_EXPECT_STRING; }
    break;

  case 92:
#line 372 "mon_parse.y"
    { sidefx = (((yyvsp[(2) - (3)].action) == e_TOGGLE) ? (sidefx ^ 1) : (yyvsp[(2) - (3)].action)); }
    break;

  case 93:
#line 374 "mon_parse.y"
    {
                         mon_out("I/O side effects are %s\n",
                                   sidefx ? "enabled" : "disabled");
                     }
    break;

  case 94:
#line 379 "mon_parse.y"
    { default_radix = (yyvsp[(2) - (3)].rt); }
    break;

  case 95:
#line 381 "mon_parse.y"
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

  case 96:
#line 399 "mon_parse.y"
    {
                         mon_out("Setting default device to `%s'\n",
                         _mon_space_strings[(int) (yyvsp[(2) - (3)].i)]); default_memspace = (yyvsp[(2) - (3)].i);
                     }
    break;

  case 97:
#line 404 "mon_parse.y"
    { mon_quit(); YYACCEPT; }
    break;

  case 98:
#line 406 "mon_parse.y"
    { exit_mon = 1; YYACCEPT; }
    break;

  case 99:
#line 410 "mon_parse.y"
    { mon_drive_execute_disk_cmd((yyvsp[(2) - (3)].str)); }
    break;

  case 100:
#line 412 "mon_parse.y"
    { mon_out("\t%d\n",(yyvsp[(2) - (3)].i)); }
    break;

  case 101:
#line 414 "mon_parse.y"
    { mon_command_print_help(NULL); }
    break;

  case 102:
#line 416 "mon_parse.y"
    { mon_command_print_help((yyvsp[(2) - (3)].str)); }
    break;

  case 103:
#line 418 "mon_parse.y"
    { printf("SYSTEM COMMAND: %s\n",(yyvsp[(2) - (3)].str)); }
    break;

  case 104:
#line 420 "mon_parse.y"
    { mon_print_convert((yyvsp[(2) - (3)].i)); }
    break;

  case 105:
#line 422 "mon_parse.y"
    { mon_change_dir((yyvsp[(2) - (3)].str)); }
    break;

  case 106:
#line 424 "mon_parse.y"
    { mon_keyboard_feed((yyvsp[(2) - (3)].str)); }
    break;

  case 107:
#line 428 "mon_parse.y"
    { mon_file_load((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].a), FALSE); }
    break;

  case 108:
#line 430 "mon_parse.y"
    { mon_file_load((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].a), TRUE); }
    break;

  case 109:
#line 432 "mon_parse.y"
    { mon_file_save((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], FALSE); }
    break;

  case 110:
#line 434 "mon_parse.y"
    { return ERR_EXPECT_DEVICE_NUM; }
    break;

  case 111:
#line 436 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 112:
#line 438 "mon_parse.y"
    { mon_file_save((yyvsp[(2) - (5)].str), (yyvsp[(3) - (5)].i), (yyvsp[(4) - (5)].range)[0], (yyvsp[(4) - (5)].range)[1], TRUE); }
    break;

  case 113:
#line 440 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 114:
#line 442 "mon_parse.y"
    { mon_file_verify((yyvsp[(2) - (5)].str),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 115:
#line 444 "mon_parse.y"
    { return ERR_EXPECT_ADDRESS; }
    break;

  case 116:
#line 446 "mon_parse.y"
    { mon_drive_block_cmd(0,(yyvsp[(2) - (5)].i),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 117:
#line 448 "mon_parse.y"
    { mon_drive_block_cmd(1,(yyvsp[(2) - (5)].i),(yyvsp[(3) - (5)].i),(yyvsp[(4) - (5)].a)); }
    break;

  case 118:
#line 452 "mon_parse.y"
    { mon_record_commands((yyvsp[(2) - (3)].str)); }
    break;

  case 119:
#line 454 "mon_parse.y"
    { mon_end_recording(); }
    break;

  case 120:
#line 456 "mon_parse.y"
    { playback=TRUE; playback_name = (yyvsp[(2) - (3)].str); }
    break;

  case 121:
#line 460 "mon_parse.y"
    { mon_memory_fill((yyvsp[(2) - (4)].a), BAD_ADDR, (unsigned char *)(yyvsp[(3) - (4)].str)); }
    break;

  case 122:
#line 462 "mon_parse.y"
    { printf("Not yet.\n"); }
    break;

  case 123:
#line 465 "mon_parse.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 125:
#line 469 "mon_parse.y"
    { return ERR_EXPECT_FILENAME; }
    break;

  case 127:
#line 473 "mon_parse.y"
    { return ERR_EXPECT_DEVICE_NUM; }
    break;

  case 128:
#line 476 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 129:
#line 477 "mon_parse.y"
    { (yyval.i) = e_load_store; }
    break;

  case 130:
#line 480 "mon_parse.y"
    { (yyval.i) = new_reg(default_memspace, (yyvsp[(1) - (1)].reg)); }
    break;

  case 131:
#line 481 "mon_parse.y"
    { (yyval.i) = new_reg((yyvsp[(1) - (2)].i), (yyvsp[(2) - (2)].reg)); }
    break;

  case 134:
#line 489 "mon_parse.y"
    { (monitor_cpu_type.mon_register_set_val)(reg_memspace((yyvsp[(1) - (3)].i)), reg_regid((yyvsp[(1) - (3)].i)), (WORD) (yyvsp[(3) - (3)].i)); }
    break;

  case 135:
#line 492 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 136:
#line 493 "mon_parse.y"
    { return ERR_EXPECT_BRKNUM; }
    break;

  case 138:
#line 497 "mon_parse.y"
    { (yyval.range)[0] = (yyvsp[(1) - (1)].a); (yyval.range)[1] = BAD_ADDR; }
    break;

  case 139:
#line 500 "mon_parse.y"
    { (yyval.range)[0] = (yyvsp[(1) - (3)].a); (yyval.range)[1] = (yyvsp[(3) - (3)].a); }
    break;

  case 140:
#line 502 "mon_parse.y"
    { if (resolve_range(e_default_space, (yyval.range), (yyvsp[(1) - (1)].str))) return ERR_ADDR_TOO_BIG; }
    break;

  case 141:
#line 504 "mon_parse.y"
    { if (resolve_range((yyvsp[(1) - (3)].i), (yyval.range), (yyvsp[(3) - (3)].str))) return ERR_ADDR_TOO_BIG; }
    break;

  case 142:
#line 507 "mon_parse.y"
    { (yyval.a) = (yyvsp[(2) - (2)].a); }
    break;

  case 143:
#line 508 "mon_parse.y"
    { (yyval.a) = BAD_ADDR; }
    break;

  case 144:
#line 512 "mon_parse.y"
    {
             (yyval.a) = new_addr(e_default_space,(yyvsp[(1) - (1)].i));
             if (opt_asm) new_cmd = asm_mode = 1; 
         }
    break;

  case 145:
#line 517 "mon_parse.y"
    {
             (yyval.a) = new_addr((yyvsp[(1) - (3)].i), (yyvsp[(3) - (3)].i));
             if (opt_asm) new_cmd = asm_mode = 1; 
         }
    break;

  case 146:
#line 522 "mon_parse.y"
    {
             temp = mon_symbol_table_lookup_addr(e_default_space, (yyvsp[(1) - (1)].str));
             if (temp >= 0)
                 (yyval.a) = new_addr(e_default_space, temp);
             else
                 return ERR_UNDEFINED_LABEL;
         }
    break;

  case 149:
#line 535 "mon_parse.y"
    { (yyval.i) = e_comp_space; }
    break;

  case 150:
#line 536 "mon_parse.y"
    { (yyval.i) = e_disk8_space; }
    break;

  case 151:
#line 537 "mon_parse.y"
    { (yyval.i) = e_disk9_space; }
    break;

  case 152:
#line 538 "mon_parse.y"
    { (yyval.i) = e_disk10_space; }
    break;

  case 153:
#line 539 "mon_parse.y"
    { (yyval.i) = e_disk11_space; }
    break;

  case 154:
#line 542 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); if (!CHECK_ADDR((yyvsp[(1) - (1)].i))) return ERR_ADDR_TOO_BIG; }
    break;

  case 155:
#line 544 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 156:
#line 546 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) + (yyvsp[(3) - (3)].i); }
    break;

  case 157:
#line 547 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) - (yyvsp[(3) - (3)].i); }
    break;

  case 158:
#line 548 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) * (yyvsp[(3) - (3)].i); }
    break;

  case 159:
#line 549 "mon_parse.y"
    { (yyval.i) = ((yyvsp[(3) - (3)].i)) ? ((yyvsp[(1) - (3)].i) / (yyvsp[(3) - (3)].i)) : 1; }
    break;

  case 160:
#line 550 "mon_parse.y"
    { (yyval.i) = (yyvsp[(2) - (3)].i); }
    break;

  case 161:
#line 551 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 162:
#line 552 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 163:
#line 556 "mon_parse.y"
    {
               (yyval.cond_node) = new_cond; (yyval.cond_node)->is_parenthized = FALSE;
               (yyval.cond_node)->child1 = (yyvsp[(1) - (3)].cond_node); (yyval.cond_node)->child2 = (yyvsp[(3) - (3)].cond_node); (yyval.cond_node)->operation = (yyvsp[(2) - (3)].cond_op);
           }
    break;

  case 164:
#line 561 "mon_parse.y"
    { return ERR_INCOMPLETE_COMPARE_OP; }
    break;

  case 165:
#line 563 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(2) - (3)].cond_node); (yyval.cond_node)->is_parenthized = TRUE; }
    break;

  case 166:
#line 565 "mon_parse.y"
    { return ERR_MISSING_CLOSE_PAREN; }
    break;

  case 167:
#line 567 "mon_parse.y"
    { (yyval.cond_node) = (yyvsp[(1) - (1)].cond_node); }
    break;

  case 168:
#line 570 "mon_parse.y"
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->reg_num = (yyvsp[(1) - (1)].i); (yyval.cond_node)->is_reg = TRUE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
    break;

  case 169:
#line 576 "mon_parse.y"
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->value = (yyvsp[(1) - (1)].i); (yyval.cond_node)->is_reg = FALSE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
    break;

  case 172:
#line 588 "mon_parse.y"
    { mon_add_number_to_buffer((yyvsp[(1) - (1)].i)); }
    break;

  case 173:
#line 589 "mon_parse.y"
    { mon_add_string_to_buffer((yyvsp[(1) - (1)].str)); }
    break;

  case 174:
#line 592 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 175:
#line 593 "mon_parse.y"
    { (yyval.i) = (monitor_cpu_type.mon_register_get_val)(reg_memspace((yyvsp[(1) - (1)].i)), reg_regid((yyvsp[(1) - (1)].i))); }
    break;

  case 176:
#line 596 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 177:
#line 597 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 178:
#line 598 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 179:
#line 599 "mon_parse.y"
    { (yyval.i) = strtol((yyvsp[(1) - (1)].str), NULL, 10); }
    break;

  case 180:
#line 602 "mon_parse.y"
    { (yyval.i) = resolve_datatype(B_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 181:
#line 603 "mon_parse.y"
    { (yyval.i) = resolve_datatype(O_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 182:
#line 604 "mon_parse.y"
    { (yyval.i) = resolve_datatype(D_NUMBER,(yyvsp[(1) - (1)].str)); }
    break;

  case 183:
#line 607 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 184:
#line 608 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 185:
#line 609 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 186:
#line 610 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 187:
#line 611 "mon_parse.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); }
    break;

  case 191:
#line 619 "mon_parse.y"
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

  case 193:
#line 630 "mon_parse.y"
    { asm_mode = 0; }
    break;

  case 194:
#line 633 "mon_parse.y"
    { if ((yyvsp[(2) - (2)].i) > 0xff)
                          (yyval.i) = join_ints(ASM_ADDR_MODE_IMMEDIATE_16,(yyvsp[(2) - (2)].i));
                        else
                          (yyval.i) = join_ints(ASM_ADDR_MODE_IMMEDIATE,(yyvsp[(2) - (2)].i)); }
    break;

  case 195:
#line 637 "mon_parse.y"
    { if ((yyvsp[(1) - (1)].i) < 0x100)
               (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE,(yyvsp[(1) - (1)].i));
             else
               (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE,(yyvsp[(1) - (1)].i));
           }
    break;

  case 196:
#line 642 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100)
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE_X,(yyvsp[(1) - (3)].i));
                          else
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_X,(yyvsp[(1) - (3)].i));
                        }
    break;

  case 197:
#line 647 "mon_parse.y"
    { if ((yyvsp[(1) - (3)].i) < 0x100)
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ZERO_PAGE_Y,(yyvsp[(1) - (3)].i));
                          else
                            (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_Y,(yyvsp[(1) - (3)].i));
                        }
    break;

  case 198:
#line 653 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABS_INDIRECT,(yyvsp[(2) - (3)].i)); }
    break;

  case 199:
#line 655 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_INDIRECT_X,(yyvsp[(2) - (5)].i)); }
    break;

  case 200:
#line 657 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_INDIRECT_Y,(yyvsp[(2) - (5)].i)); }
    break;

  case 201:
#line 658 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_BC,0); }
    break;

  case 202:
#line 659 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_DE,0); }
    break;

  case 203:
#line 660 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_HL,0); }
    break;

  case 204:
#line 661 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_IX,0); }
    break;

  case 205:
#line 662 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_IY,0); }
    break;

  case 206:
#line 663 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IND_SP,0); }
    break;

  case 207:
#line 665 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_A,(yyvsp[(2) - (5)].i)); }
    break;

  case 208:
#line 667 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_HL,(yyvsp[(2) - (5)].i)); }
    break;

  case 209:
#line 669 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_IX,(yyvsp[(2) - (5)].i)); }
    break;

  case 210:
#line 671 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ABSOLUTE_IY,(yyvsp[(2) - (5)].i)); }
    break;

  case 211:
#line 672 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_IMPLIED,0); }
    break;

  case 212:
#line 673 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_ACCUMULATOR,0); }
    break;

  case 213:
#line 674 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_B,0); }
    break;

  case 214:
#line 675 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_C,0); }
    break;

  case 215:
#line 676 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_D,0); }
    break;

  case 216:
#line 677 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_E,0); }
    break;

  case 217:
#line 678 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_H,0); }
    break;

  case 218:
#line 679 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IXH,0); }
    break;

  case 219:
#line 680 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IYH,0); }
    break;

  case 220:
#line 681 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_L,0); }
    break;

  case 221:
#line 682 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IXL,0); }
    break;

  case 222:
#line 683 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IYL,0); }
    break;

  case 223:
#line 684 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_AF,0); }
    break;

  case 224:
#line 685 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_BC,0); }
    break;

  case 225:
#line 686 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_DE,0); }
    break;

  case 226:
#line 687 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_HL,0); }
    break;

  case 227:
#line 688 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IX,0); }
    break;

  case 228:
#line 689 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_IY,0); }
    break;

  case 229:
#line 690 "mon_parse.y"
    { (yyval.i) = join_ints(ASM_ADDR_MODE_REG_SP,0); }
    break;


/* Line 1267 of yacc.c.  */
#line 3386 "mon_parse.c"
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


#line 694 "mon_parse.y"


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


