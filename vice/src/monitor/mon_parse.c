/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "mon_parse.y" /* yacc.c:339  */

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

#if !defined(__minix_vmd) && !defined(MACOS_COMPILE)
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
#include "mon_memory.h"
#include "mon_register.h"
#include "mon_util.h"
#include "montypes.h"
#include "resources.h"
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


#line 196 "mon_parse.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_MON_PARSE_H_INCLUDED
# define YY_YY_MON_PARSE_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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
    CMD_RECORD = 327,
    CMD_MON_STOP = 328,
    CMD_PLAYBACK = 329,
    CMD_CHAR_DISPLAY = 330,
    CMD_SPRITE_DISPLAY = 331,
    CMD_TEXT_DISPLAY = 332,
    CMD_SCREENCODE_DISPLAY = 333,
    CMD_ENTER_DATA = 334,
    CMD_ENTER_BIN_DATA = 335,
    CMD_KEYBUF = 336,
    CMD_BLOAD = 337,
    CMD_BSAVE = 338,
    CMD_SCREEN = 339,
    CMD_UNTIL = 340,
    CMD_CPU = 341,
    CMD_YYDEBUG = 342,
    CMD_BACKTRACE = 343,
    CMD_SCREENSHOT = 344,
    CMD_PWD = 345,
    CMD_DIR = 346,
    CMD_RESOURCE_GET = 347,
    CMD_RESOURCE_SET = 348,
    CMD_LOAD_RESOURCES = 349,
    CMD_SAVE_RESOURCES = 350,
    CMD_ATTACH = 351,
    CMD_DETACH = 352,
    CMD_MON_RESET = 353,
    CMD_TAPECTRL = 354,
    CMD_CARTFREEZE = 355,
    CMD_CPUHISTORY = 356,
    CMD_MEMMAPZAP = 357,
    CMD_MEMMAPSHOW = 358,
    CMD_MEMMAPSAVE = 359,
    CMD_COMMENT = 360,
    CMD_LIST = 361,
    CMD_STOPWATCH = 362,
    RESET = 363,
    CMD_EXPORT = 364,
    CMD_AUTOSTART = 365,
    CMD_AUTOLOAD = 366,
    CMD_LABEL_ASGN = 367,
    L_PAREN = 368,
    R_PAREN = 369,
    ARG_IMMEDIATE = 370,
    REG_A = 371,
    REG_X = 372,
    REG_Y = 373,
    COMMA = 374,
    INST_SEP = 375,
    L_BRACKET = 376,
    R_BRACKET = 377,
    LESS_THAN = 378,
    REG_U = 379,
    REG_S = 380,
    REG_PC = 381,
    REG_PCR = 382,
    REG_B = 383,
    REG_C = 384,
    REG_D = 385,
    REG_E = 386,
    REG_H = 387,
    REG_L = 388,
    REG_AF = 389,
    REG_BC = 390,
    REG_DE = 391,
    REG_HL = 392,
    REG_IX = 393,
    REG_IY = 394,
    REG_SP = 395,
    REG_IXH = 396,
    REG_IXL = 397,
    REG_IYH = 398,
    REG_IYL = 399,
    PLUS = 400,
    MINUS = 401,
    STRING = 402,
    FILENAME = 403,
    R_O_L = 404,
    OPCODE = 405,
    LABEL = 406,
    BANKNAME = 407,
    CPUTYPE = 408,
    MON_REGISTER = 409,
    COMPARE_OP = 410,
    RADIX_TYPE = 411,
    INPUT_SPEC = 412,
    CMD_CHECKPT_ON = 413,
    CMD_CHECKPT_OFF = 414,
    TOGGLE = 415,
    MASK = 416,
    LABEL_DATA_TYPE = 417,
    COMMENT = 418
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
#define CMD_RECORD 327
#define CMD_MON_STOP 328
#define CMD_PLAYBACK 329
#define CMD_CHAR_DISPLAY 330
#define CMD_SPRITE_DISPLAY 331
#define CMD_TEXT_DISPLAY 332
#define CMD_SCREENCODE_DISPLAY 333
#define CMD_ENTER_DATA 334
#define CMD_ENTER_BIN_DATA 335
#define CMD_KEYBUF 336
#define CMD_BLOAD 337
#define CMD_BSAVE 338
#define CMD_SCREEN 339
#define CMD_UNTIL 340
#define CMD_CPU 341
#define CMD_YYDEBUG 342
#define CMD_BACKTRACE 343
#define CMD_SCREENSHOT 344
#define CMD_PWD 345
#define CMD_DIR 346
#define CMD_RESOURCE_GET 347
#define CMD_RESOURCE_SET 348
#define CMD_LOAD_RESOURCES 349
#define CMD_SAVE_RESOURCES 350
#define CMD_ATTACH 351
#define CMD_DETACH 352
#define CMD_MON_RESET 353
#define CMD_TAPECTRL 354
#define CMD_CARTFREEZE 355
#define CMD_CPUHISTORY 356
#define CMD_MEMMAPZAP 357
#define CMD_MEMMAPSHOW 358
#define CMD_MEMMAPSAVE 359
#define CMD_COMMENT 360
#define CMD_LIST 361
#define CMD_STOPWATCH 362
#define RESET 363
#define CMD_EXPORT 364
#define CMD_AUTOSTART 365
#define CMD_AUTOLOAD 366
#define CMD_LABEL_ASGN 367
#define L_PAREN 368
#define R_PAREN 369
#define ARG_IMMEDIATE 370
#define REG_A 371
#define REG_X 372
#define REG_Y 373
#define COMMA 374
#define INST_SEP 375
#define L_BRACKET 376
#define R_BRACKET 377
#define LESS_THAN 378
#define REG_U 379
#define REG_S 380
#define REG_PC 381
#define REG_PCR 382
#define REG_B 383
#define REG_C 384
#define REG_D 385
#define REG_E 386
#define REG_H 387
#define REG_L 388
#define REG_AF 389
#define REG_BC 390
#define REG_DE 391
#define REG_HL 392
#define REG_IX 393
#define REG_IY 394
#define REG_SP 395
#define REG_IXH 396
#define REG_IXL 397
#define REG_IYH 398
#define REG_IYL 399
#define PLUS 400
#define MINUS 401
#define STRING 402
#define FILENAME 403
#define R_O_L 404
#define OPCODE 405
#define LABEL 406
#define BANKNAME 407
#define CPUTYPE 408
#define MON_REGISTER 409
#define COMPARE_OP 410
#define RADIX_TYPE 411
#define INPUT_SPEC 412
#define CMD_CHECKPT_ON 413
#define CMD_CHECKPT_OFF 414
#define TOGGLE 415
#define MASK 416
#define LABEL_DATA_TYPE 417
#define COMMENT 418

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 131 "mon_parse.y" /* yacc.c:355  */

    MON_ADDR a;
    MON_ADDR range[2];
    int i;
    REG_ID reg;
    CONDITIONAL cond_op;
    cond_node_t *cond_node;
    RADIXTYPE rt;
    LABELDATATYPE ldt;
    ACTION action;
    char *str;
    asm_mode_addr_info_t mode;

#line 576 "mon_parse.c" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_MON_PARSE_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 591 "mon_parse.c" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
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
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  307
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1729

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  170
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  58
/* YYNRULES -- Number of rules.  */
#define YYNRULES  315
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  624

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   418

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     168,   169,   166,   164,     2,   165,     2,   167,     2,     2,
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
     155,   156,   157,   158,   159,   160,   161,   162,   163
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   201,   201,   202,   203,   206,   207,   210,   211,   212,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   230,   232,   234,   236,   238,   240,   242,
     244,   246,   248,   250,   252,   254,   256,   258,   260,   262,
     264,   266,   268,   270,   272,   274,   276,   278,   281,   283,
     285,   288,   293,   298,   300,   302,   304,   306,   308,   310,
     312,   319,   318,   321,   323,   325,   329,   331,   333,   335,
     337,   339,   341,   343,   345,   347,   349,   351,   353,   355,
     357,   359,   361,   363,   365,   367,   371,   380,   383,   387,
     390,   399,   402,   411,   416,   418,   420,   422,   424,   426,
     428,   430,   432,   434,   436,   440,   442,   447,   449,   467,
     469,   471,   473,   477,   479,   481,   483,   485,   487,   489,
     491,   493,   495,   497,   499,   501,   503,   505,   507,   509,
     511,   513,   515,   517,   519,   521,   525,   527,   529,   531,
     533,   535,   537,   539,   541,   543,   545,   547,   549,   551,
     553,   555,   557,   559,   561,   565,   567,   569,   573,   575,
     579,   583,   586,   587,   590,   591,   594,   595,   598,   599,
     602,   603,   606,   612,   620,   621,   624,   628,   629,   632,
     633,   636,   637,   639,   643,   644,   647,   652,   657,   667,
     668,   671,   672,   673,   674,   675,   678,   680,   682,   683,
     684,   685,   686,   687,   688,   691,   692,   694,   699,   701,
     703,   705,   709,   715,   723,   724,   727,   728,   731,   732,
     735,   736,   737,   740,   741,   744,   745,   746,   747,   750,
     751,   752,   755,   756,   757,   758,   759,   762,   763,   764,
     767,   777,   778,   781,   788,   799,   810,   818,   837,   843,
     851,   859,   861,   863,   864,   865,   866,   867,   868,   869,
     871,   873,   875,   877,   878,   879,   880,   881,   882,   883,
     884,   885,   886,   887,   888,   889,   890,   891,   892,   893,
     894,   895,   897,   898,   913,   917,   921,   925,   929,   933,
     937,   941,   945,   957,   972,   976,   980,   984,   988,   992,
     996,  1000,  1004,  1016,  1021,  1029,  1030,  1031,  1032,  1036,
    1040,  1041,  1045,  1046,  1050,  1051
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
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
  "CMD_ADD_LABEL", "CMD_DEL_LABEL", "CMD_SHOW_LABELS", "CMD_RECORD",
  "CMD_MON_STOP", "CMD_PLAYBACK", "CMD_CHAR_DISPLAY", "CMD_SPRITE_DISPLAY",
  "CMD_TEXT_DISPLAY", "CMD_SCREENCODE_DISPLAY", "CMD_ENTER_DATA",
  "CMD_ENTER_BIN_DATA", "CMD_KEYBUF", "CMD_BLOAD", "CMD_BSAVE",
  "CMD_SCREEN", "CMD_UNTIL", "CMD_CPU", "CMD_YYDEBUG", "CMD_BACKTRACE",
  "CMD_SCREENSHOT", "CMD_PWD", "CMD_DIR", "CMD_RESOURCE_GET",
  "CMD_RESOURCE_SET", "CMD_LOAD_RESOURCES", "CMD_SAVE_RESOURCES",
  "CMD_ATTACH", "CMD_DETACH", "CMD_MON_RESET", "CMD_TAPECTRL",
  "CMD_CARTFREEZE", "CMD_CPUHISTORY", "CMD_MEMMAPZAP", "CMD_MEMMAPSHOW",
  "CMD_MEMMAPSAVE", "CMD_COMMENT", "CMD_LIST", "CMD_STOPWATCH", "RESET",
  "CMD_EXPORT", "CMD_AUTOSTART", "CMD_AUTOLOAD", "CMD_LABEL_ASGN",
  "L_PAREN", "R_PAREN", "ARG_IMMEDIATE", "REG_A", "REG_X", "REG_Y",
  "COMMA", "INST_SEP", "L_BRACKET", "R_BRACKET", "LESS_THAN", "REG_U",
  "REG_S", "REG_PC", "REG_PCR", "REG_B", "REG_C", "REG_D", "REG_E",
  "REG_H", "REG_L", "REG_AF", "REG_BC", "REG_DE", "REG_HL", "REG_IX",
  "REG_IY", "REG_SP", "REG_IXH", "REG_IXL", "REG_IYH", "REG_IYL", "PLUS",
  "MINUS", "STRING", "FILENAME", "R_O_L", "OPCODE", "LABEL", "BANKNAME",
  "CPUTYPE", "MON_REGISTER", "COMPARE_OP", "RADIX_TYPE", "INPUT_SPEC",
  "CMD_CHECKPT_ON", "CMD_CHECKPT_OFF", "TOGGLE", "MASK", "LABEL_DATA_TYPE",
  "COMMENT", "'+'", "'-'", "'*'", "'/'", "'('", "')'", "$accept",
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
  "post_assemble", "asm_operand_mode", "index_reg", "index_ureg",
  "opt_number", "opt_data_type", "opt_comment", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
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
     415,   416,   417,   418,    43,    45,    42,    47,    40,    41
};
# endif

#define YYPACT_NINF -469

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-469)))

#define YYTABLE_NINF -191

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    1140,   845,  -469,  -469,     8,   194,   845,   845,   718,   718,
       9,     9,     9,   624,  1514,  1514,  1514,  1406,   596,    18,
     921,  1103,  1103,  1406,  1514,     9,     9,   194,  1078,   624,
     624,  1532,  1255,   718,   718,   845,   936,   180,  1103,  -144,
     194,  -144,   271,   497,   497,  1532,   385,   992,     9,   194,
       9,  1255,  1255,  1255,  1255,  1532,   194,  -144,     9,     9,
     194,  1255,   166,   194,   194,     9,   194,  -104,   -84,   -77,
       9,     9,     9,   845,   718,   -38,   194,   718,   194,   718,
       9,  -104,    90,   246,   194,     9,     9,    66,  1556,  1078,
    1078,    92,  1275,  -469,  -469,  -469,  -469,  -469,  -469,  -469,
    -469,  -469,  -469,  -469,  -469,  -469,  -469,    68,  -469,  -469,
    -469,  -469,  -469,  -469,  -469,  -469,  -469,  -469,  -469,  -469,
    -469,   845,  -469,   -55,   131,  -469,  -469,  -469,  -469,  -469,
    -469,   194,  -469,  -469,   793,   793,  -469,  -469,   845,  -469,
     845,  -469,  -469,   132,   563,   132,  -469,  -469,  -469,  -469,
    -469,   718,  -469,  -469,  -469,   -38,   -38,   -38,  -469,  -469,
    -469,   -38,   -38,  -469,   194,   -38,  -469,    84,   238,  -469,
      81,   194,  -469,   -38,  -469,   194,  -469,   291,  -469,  -469,
     111,  1514,  -469,  1514,  -469,   194,   -38,   194,   194,  -469,
     255,  -469,   194,    96,    52,   171,  -469,   194,  -469,   845,
    -469,   845,   131,   194,  -469,  -469,   194,  -469,  1514,   194,
    -469,   194,   194,  -469,   317,   194,   -38,   194,   -38,   -38,
     194,   -38,  -469,   194,   194,  -469,   194,  -469,   194,  -469,
     194,  -469,   194,  -469,   194,   687,  -469,   194,   132,   132,
    -469,  -469,   194,   194,  -469,  -469,  -469,   718,  -469,  -469,
     194,   194,   -20,   194,   194,   845,   131,  -469,   845,   845,
    -469,  -469,   845,  -469,  -469,   845,   -38,   194,   308,  -469,
     194,   509,   194,  -469,  -469,  1040,  1040,  1532,   891,  1281,
      15,   310,  1585,  1281,    22,  -469,    28,  -469,  -469,  -469,
    -469,  -469,  -469,  -469,  -469,  -469,  -469,  -469,  -469,  -469,
    -469,    39,  -469,  -469,   194,  -469,   194,  -469,  -469,  -469,
      88,  -469,   845,   845,   845,   845,  -469,  -469,   117,   873,
     131,   131,  -469,   301,  1434,  1454,  1496,  -469,   845,   363,
    1532,  1566,   687,  1532,  -469,  1281,  1281,   525,  -469,  -469,
    -469,  1514,  -469,  -469,   145,   145,  -469,  1532,  -469,  -469,
    -469,  1385,   194,    17,  -469,    19,  -469,   131,   131,  -469,
    -469,  -469,   145,  -469,  -469,  -469,  -469,    10,  -469,     9,
    -469,     9,    33,  -469,    35,  -469,  -469,  -469,  -469,  -469,
    -469,  -469,  -469,  1129,  -469,  -469,  -469,   301,  1474,  -469,
    -469,  -469,   845,  -469,  -469,   194,  -469,  -469,   131,  -469,
     131,   131,   131,   813,   845,  -469,  -469,  -469,  -469,  1281,
    -469,  1281,    16,    59,    77,    82,    86,    91,    94,   -93,
    -469,   116,  -469,  -469,  -469,  -469,   453,    79,  -469,   101,
     515,   103,   107,   -39,  -469,   116,   116,  1603,  -469,  -469,
    -469,  -469,    97,    97,  -469,  -469,   194,  1532,   194,  -469,
    -469,   194,  -469,   194,  -469,   194,   131,  -469,  -469,   906,
    -469,  -469,  -469,  -469,  -469,  1129,   194,  -469,  -469,   194,
    1385,   194,   194,   194,  1385,  -469,    75,  -469,  -469,  -469,
     194,   108,   123,   194,  -469,  -469,   194,   194,   194,   194,
     -38,   194,  -469,   687,   194,  -469,   194,   131,  -469,  -469,
    -469,  -469,  -469,  -469,   194,   131,   194,   194,  -469,   194,
    -469,  -469,  -469,  -469,  -469,  -469,   112,   -65,  -469,   116,
    -469,   105,   116,   532,  -102,   116,   116,     4,   140,  -469,
    -469,  -469,  -469,  -469,  -469,  -469,  -469,  -469,  -469,  -469,
    -469,  -469,  -469,  -469,  -469,  -469,  -469,  -469,  -469,    56,
    -469,  -469,  -469,     6,   984,  -469,  -469,    19,    19,  -469,
    -469,  -469,  -469,  -469,   113,  -469,  -469,  -469,  -469,  -469,
    -469,  -469,  -469,  -469,  -469,   244,   152,   159,  -469,  -469,
     154,   116,   163,  -469,   -83,   181,   182,   188,   195,   204,
    -469,  -469,  -469,  -469,  -469,  -469,  -469,   -38,  -469,  -469,
    -469,  -469,  -469,  -469,   208,  -469,   206,  -469,  -469,   211,
    -469,  -469,  -469,  -469,  -469,  1281,   217,  -469,  -469,  -469,
      16,  -469,   194,  -469
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,    22,     4,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   163,     0,     0,
       0,     0,     0,     0,     0,   190,     0,     0,     0,     0,
       0,   163,     0,     0,     0,     0,     0,     0,   263,     0,
       0,     0,     2,     5,    10,    47,    11,    13,    12,    14,
      15,    16,    17,    18,    19,    20,    21,     0,   232,   233,
     234,   235,   231,   230,   229,   191,   192,   193,   194,   195,
     172,     0,   224,     0,     0,   204,   236,   223,     9,     8,
       7,     0,   106,    35,     0,     0,   189,    42,     0,    44,
       0,   165,   164,     0,     0,     0,   178,   225,   228,   227,
     226,     0,   177,   182,   188,   190,   190,   190,   186,   196,
     197,   190,   190,    28,     0,   190,    48,     0,     0,   175,
       0,     0,   108,   190,    72,     0,   179,   190,   169,    87,
     170,     0,    93,     0,    29,     0,   190,     0,     0,   112,
       9,   101,     0,     0,     0,     0,    65,     0,    40,     0,
      38,     0,     0,     0,   161,   115,     0,    91,     0,     0,
     111,     0,     0,    23,     0,     0,   190,     0,   190,   190,
       0,   190,    59,     0,     0,   156,     0,    74,     0,    76,
       0,    78,     0,    80,     0,     0,   159,     0,     0,     0,
      46,    89,     0,     0,    31,   160,   120,     0,   122,   162,
       0,     0,     0,     0,     0,     0,     0,   129,     0,     0,
     132,    33,     0,    81,    82,     0,   190,     0,     9,   147,
       0,   166,     0,   135,   110,     0,     0,     0,     0,     0,
     264,     0,     0,     0,   265,   266,   267,   268,   269,   272,
     275,   276,   277,   278,   279,   280,   281,   270,   273,   271,
     274,   244,   240,    95,     0,    97,     0,     1,     6,     3,
       0,   173,     0,     0,     0,     0,   117,   105,   190,     0,
       0,     0,   167,   190,   139,     0,     0,    98,     0,     0,
       0,     0,     0,     0,    27,     0,     0,     0,    50,    49,
     107,     0,    71,   168,   206,   206,    30,     0,    36,    37,
     100,     0,     0,     0,    63,     0,    64,     0,     0,   114,
     109,   116,   206,   113,   118,    25,    24,     0,    52,     0,
      54,     0,     0,    56,     0,    58,   155,   157,    73,    75,
      77,    79,   217,     0,   215,   216,   119,   190,     0,    88,
      32,   123,     0,   121,   125,     0,   127,   128,     0,   150,
       0,     0,     0,     0,     0,   133,   148,   134,   151,     0,
     153,     0,   315,     0,     0,     0,     0,     0,     0,     0,
     243,     0,   305,   306,   309,   308,     0,   288,   307,     0,
       0,     0,     0,     0,   282,     0,     0,     0,    94,    96,
     203,   202,   198,   199,   200,   201,     0,     0,     0,    43,
      45,     0,   140,     0,   144,     0,     0,   222,   221,     0,
     219,   220,   181,   183,   187,     0,     0,   176,   174,     0,
       0,     0,     0,     0,     0,   212,     0,   211,   213,   104,
       0,   242,   241,     0,    41,    39,     0,     0,     0,     0,
     190,     0,   158,     0,     0,   142,     0,     0,   126,   149,
     130,   131,    34,    83,     0,     0,     0,     0,   314,     0,
     253,   254,   255,   256,   257,   258,   249,     0,   290,     0,
     286,   284,     0,     0,     0,     0,     0,     0,   303,   289,
     291,   245,   246,   247,   292,   248,   283,   145,   184,   146,
     136,   138,   143,    99,    69,   218,    68,    66,    70,   205,
      86,    92,    67,     0,     0,   102,   103,     0,   239,    62,
      90,    26,    51,    53,   313,    57,   214,   137,   141,   124,
      84,    85,   152,   154,    60,     0,     0,     0,   287,   285,
       0,     0,     0,   298,     0,     0,     0,     0,     0,     0,
     210,   209,   208,   207,   237,   238,   312,   190,   259,   252,
     260,   261,   262,   250,     0,   300,     0,   296,   294,     0,
     299,   301,   302,   293,   304,   311,     0,   297,   295,   310,
     315,   251,     0,    55
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -469,  -469,  -469,    -5,   249,  -469,  -469,  -469,  -469,  -469,
    -469,  -469,  -469,  -469,  -469,  -469,  -469,  -469,  -469,    -3,
     261,   300,   193,  -469,    12,   -17,  -469,    14,   564,   124,
      51,  -299,   100,   486,   740,  -267,  -469,   707,  -337,  -468,
    -469,    13,  -137,  -469,  -110,  -469,  -469,  -469,   273,  -469,
    -344,  -469,  -469,   104,   -74,  -469,  -469,  -256
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    91,    92,   132,    93,    94,    95,    96,    97,   355,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   206,
     250,   143,   270,   180,   181,   122,   168,   169,   151,   175,
     176,   446,   177,   447,   123,   158,   159,   271,   471,   476,
     477,   383,   384,   459,   460,   125,   152,   126,   160,   481,
     107,   483,   302,   427,   428,   620,   597,   509
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     133,   167,   549,   137,   139,   204,   553,   590,   472,   128,
     141,   482,   163,   166,   172,   174,   179,   182,   184,   128,
     583,   516,   189,   191,   451,   486,   517,   196,   198,   200,
     129,   130,   205,   207,   183,   210,   209,   213,   211,   608,
     129,   130,   222,   584,   225,   249,   227,   229,   231,   233,
     208,   236,   576,   352,   237,   240,   241,   244,   245,   246,
     577,   248,   609,   251,   464,   155,   161,   162,   464,   257,
     252,   260,   261,   263,   264,   186,   128,   269,   273,   274,
     527,   136,   128,   528,   303,   305,   593,   277,   494,   440,
     309,   268,   307,   108,   109,   110,   111,   129,   130,   311,
     112,   113,   114,   129,   130,   336,   115,   116,   117,   118,
     119,   351,   129,   130,   156,   156,   156,   164,  -185,   316,
     591,   422,   423,   185,   156,   343,   317,   395,   424,   425,
     587,   195,   128,   322,   421,   108,   109,   110,   111,  -185,
    -185,   435,   112,   113,   114,   219,   327,   436,   115,   116,
     117,   118,   119,   129,   130,   235,   197,   142,   437,   334,
     470,   554,   487,   338,   480,   339,   340,   128,   131,    88,
     342,   136,   128,   510,   171,   228,   230,   232,   234,   508,
     346,   128,   348,   349,   490,   242,   491,   350,   129,   130,
     354,   511,   356,   129,   130,   128,   512,   359,   360,  -190,
     513,   361,   129,   130,   363,   514,   364,   365,   515,   366,
     368,   554,   370,   594,   595,   373,   129,   130,   375,   376,
     522,   377,   525,   378,   521,   379,   526,   380,   557,   381,
     554,   575,   386,   422,   423,   311,   136,   389,   390,   128,
     424,   425,   391,   558,   120,   393,   394,   128,   396,   397,
     579,   399,   312,   313,   314,   315,  -178,   441,   121,   589,
     129,   130,   405,   314,   315,   406,   603,   407,   129,   130,
     408,   410,   128,   604,   127,   596,   605,  -178,  -178,   127,
     127,   312,   313,   314,   315,   607,   120,   115,   116,   117,
     118,   119,  -180,   129,   130,   312,   313,   314,   315,   438,
     121,   439,  -185,   610,   611,   344,  -180,   345,   127,  -167,
     612,   144,   145,  -180,  -180,   449,   450,   613,   128,   243,
     167,   -61,   614,  -185,  -185,   187,   188,   616,   617,   204,
    -167,  -167,   362,   618,   475,   621,   323,   325,   326,   129,
     130,   308,   267,   215,   217,   465,   127,   479,   224,   545,
     226,   468,   484,   485,   272,   127,   566,   337,   238,   239,
     598,   301,   599,   536,   622,   247,   108,   109,   110,   111,
     253,   254,   255,   112,   113,   114,   453,   412,   492,     0,
     266,   600,   601,   602,     0,   275,   276,     0,     0,     0,
     498,     0,     0,   499,   127,   500,   501,   502,   503,     0,
       0,   115,   116,   117,   118,   119,     0,   127,   127,     0,
     136,   127,     0,   127,     0,     0,   127,   127,   127,   448,
     136,     0,     0,   212,     0,   156,   455,   422,   423,     0,
     462,   387,   388,   466,   424,   425,   136,     0,     0,   496,
       0,   537,     0,   539,     0,     0,   540,   473,   541,     0,
     542,   543,     0,   475,   544,     0,   426,   475,     0,     0,
     546,   547,     0,     0,   548,   469,   550,   551,   552,  -190,
       0,   555,   127,     0,   127,   556,     0,     0,   559,     0,
       0,   560,   561,   562,   563,     0,   565,     0,   156,   567,
       0,   568,   569,     0,   138,   140,     0,     0,   141,   570,
     571,   572,   573,     0,   574,     0,     0,     0,   385,     0,
     457,   127,   127,   115,   116,   117,   118,   119,     0,   199,
     201,     0,     0,     0,   458,   518,     0,   504,   127,     0,
     520,   127,   127,     0,   524,   127,   220,   475,   127,   529,
     530,   115,   116,   117,   118,   119,     0,   538,     0,     0,
       0,   419,   420,     0,     0,   433,   434,     0,     0,     0,
     258,   259,     0,   262,   324,   265,   108,   109,   110,   111,
     422,   423,     0,   112,   113,   114,     0,   424,   425,   115,
     116,   117,   118,   119,     0,   127,   127,   127,   127,     0,
       0,     0,   192,   193,   194,     0,     0,   128,     0,   519,
       0,   127,   461,     0,     0,   385,     0,     0,     0,   467,
       0,     0,   115,   116,   117,   118,   119,   623,   129,   130,
       0,     0,     0,   578,   478,   146,   580,   582,   147,   585,
     586,   588,   422,   423,   148,   149,   150,   328,     0,   424,
     425,   329,   330,   331,     0,   142,     0,   332,   333,   422,
     423,   335,     0,   304,   306,     0,   424,   425,     0,   341,
       0,   523,     0,   330,     0,   127,     0,     0,     0,   488,
       0,   489,   347,   312,   313,   314,   315,   127,   581,   120,
     353,     0,   506,     0,   507,   606,     0,     0,     0,     0,
     108,   109,   110,   111,     0,     0,     0,   112,   113,   114,
     367,     0,   369,     0,   371,   372,     0,   374,   124,     0,
     535,     0,     0,   134,   135,     0,     0,   120,     0,   128,
       0,  -190,  -190,  -190,  -190,     0,     0,     0,  -190,  -190,
    -190,   121,   461,   392,  -190,  -190,  -190,  -190,  -190,     0,
     129,   130,   202,   478,     0,     0,     0,   478,     0,     0,
     120,     0,   404,     0,   157,   157,   157,   165,   170,     0,
     157,   409,   411,   165,   157,     0,   385,     0,     0,     0,
       0,   165,   157,     0,     0,     0,   203,     0,     0,     0,
     256,     0,   214,   216,   218,   165,   221,   223,     0,     0,
       0,   157,   157,   157,   157,   165,   108,   109,   110,   111,
       0,   157,     0,   112,   113,   114,     0,     0,     0,   115,
     116,   117,   118,   119,   128,     0,   108,   109,   110,   111,
       0,     0,   153,   112,   113,   114,     0,   478,   310,   115,
     116,   117,   118,   119,   382,   129,   130,   136,     0,     0,
       0,   318,   319,     0,     0,   320,     0,   321,   108,   109,
     110,   111,     0,     0,     0,   112,   113,   114,     0,     0,
       0,   115,   116,   117,   118,   119,     0,     0,     0,   493,
       0,     0,  -190,     0,     0,     0,   108,   109,   110,   111,
       0,     0,     0,   112,   113,   114,  -190,     0,   619,   115,
     116,   117,   118,   119,   108,   109,   110,   111,     0,     0,
       0,   112,   113,   114,     0,     0,   357,   128,   358,   108,
     109,   110,   111,     0,     0,     0,   112,   113,   114,     0,
       0,   157,   128,   157,   108,   109,   110,   111,   129,   130,
     153,   112,   113,   114,     0,     0,     0,   115,   116,   117,
     118,   119,     0,   129,   130,     0,     0,   120,   157,     0,
       0,   493,   115,   116,   117,   118,   119,   312,   313,   314,
     315,   121,   398,     0,   154,   400,   401,     0,     0,   402,
       0,     0,   403,     0,     0,     0,   564,   312,   313,   314,
     315,     0,     0,     0,     0,   592,     0,   108,   109,   110,
     111,     0,     0,   128,   112,   113,   114,     0,     0,   120,
     115,   116,   117,   118,   119,     0,     0,     0,   115,   116,
     117,   118,   119,   121,   129,   130,     0,   165,     0,   442,
     443,   444,   445,     0,   154,     0,   413,   414,   415,   416,
     417,   418,     0,     0,     0,   456,     0,   312,   313,   314,
     315,   128,     0,  -190,  -190,  -190,  -190,     0,     0,     0,
    -190,  -190,  -190,   457,     0,     0,     0,     0,     0,   165,
       0,     0,   129,   130,     0,   157,   165,   458,     0,     0,
     165,     0,   154,   165,     0,     0,     0,   173,     0,   190,
       0,   157,   147,   615,     0,     0,     0,   165,   148,   149,
     150,     0,     0,     0,     0,     0,     0,   474,     0,   497,
     129,   130,     0,     0,   128,     0,  -171,  -171,  -171,  -171,
       0,   505,  -171,  -171,  -171,  -171,     0,   178,     0,  -171,
    -171,  -171,  -171,  -171,     0,   129,   130,     0,   157,     0,
     128,     0,  -190,  -190,  -190,  -190,     0,     0,   120,  -190,
    -190,  -190,     0,   157,     0,     0,     0,     1,     0,     0,
       0,   129,   130,     2,     0,     0,     0,     0,     0,   136,
       0,     0,     3,     0,     0,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
       0,     0,    19,    20,    21,    22,    23,   165,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,   136,    84,
      85,    86,    87,     0,  -171,     0,   128,     0,   108,   109,
     110,   111,     0,     0,   153,   112,   113,   114,     0,     0,
       0,   115,   116,   117,   118,   119,  -190,   129,   130,     0,
       0,     0,     1,     0,   108,   109,   110,   111,     2,     0,
      88,   112,   113,   114,     0,     0,     0,     0,    89,    90,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,     0,     0,    19,    20,    21,
      22,    23,     0,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,     0,    84,    85,    86,    87,   108,   109,
     110,   111,     0,     0,     0,   112,   113,   114,     0,     0,
       0,   115,   116,   117,   118,   119,   154,   128,     0,   108,
     109,   110,   111,     0,     0,     0,   112,   113,   114,     0,
       0,     0,   115,   116,   117,   118,   119,     0,   129,   130,
       0,     0,     0,    89,    90,  -167,     0,  -167,  -167,  -167,
    -167,     0,     0,  -167,  -167,  -167,  -167,     0,     0,     0,
    -167,  -167,  -167,  -167,  -167,   452,     0,   108,   109,   110,
     111,     0,     0,   153,   112,   113,   114,     0,     0,     0,
     115,   116,   117,   118,   119,   495,     0,   108,   109,   110,
     111,     0,     0,   153,   112,   113,   114,     0,     0,     0,
     115,   116,   117,   118,   119,     0,     0,   454,   474,   108,
     109,   110,   111,     0,     0,     0,   112,   113,   114,     0,
       0,     0,   115,   116,   117,   118,   119,   108,   109,   110,
     111,     0,     0,   153,   112,   113,   114,     0,     0,     0,
     115,   116,   117,   118,   119,   108,   109,   110,   111,   120,
       0,     0,   112,   113,   114,     0,     0,     0,   115,   116,
     117,   118,   119,     0,     0,     0,     0,   154,     0,   108,
     109,   110,   111,     0,     0,     0,   112,   113,   114,   108,
     109,   110,   111,     0,     0,   463,   112,   113,   114,     0,
       0,     0,     0,     0,     0,  -167,     0,     0,   108,   109,
     110,   111,     0,     0,     0,   112,   113,   114,     0,     0,
       0,     0,     0,     0,     0,   154,   108,   109,   110,   111,
       0,     0,     0,   112,   113,   114,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   154,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   154,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   154,     0,     0,     0,   278,
       0,   279,   280,     0,     0,   281,     0,   282,     0,   283,
       0,     0,     0,   154,   284,   285,   286,   287,   288,   289,
     290,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   429,     0,     0,   430,     0,     0,     0,     0,     0,
       0,     0,     0,   431,     0,   432,     0,     0,     0,     0,
     531,   532,     0,     0,     0,     0,     0,   424,   533,   534
};

static const yytype_int16 yycheck[] =
{
       5,    18,   470,     8,     9,   149,   474,     1,   345,     1,
       1,   355,    17,    18,    19,    20,    21,    22,    23,     1,
     122,   114,    27,    28,   323,   362,   119,    32,    33,    34,
      22,    23,    37,    38,    22,    40,    39,    42,    41,   122,
      22,    23,    47,   145,    49,   149,    51,    52,    53,    54,
      38,    56,   117,     1,    57,    60,    61,    62,    63,    64,
     125,    66,   145,   147,   331,    14,    15,    16,   335,    74,
     147,    76,    77,    78,    79,    24,     1,    82,    83,    84,
     119,   119,     1,   122,    89,    90,   554,    21,   387,     1,
      22,     1,     0,     3,     4,     5,     6,    22,    23,   154,
      10,    11,    12,    22,    23,    21,    16,    17,    18,    19,
      20,    15,    22,    23,    14,    15,    16,    17,     1,   124,
     114,   117,   118,    23,    24,    14,   131,   147,   124,   125,
     126,    31,     1,     1,   119,     3,     4,     5,     6,    22,
      23,   119,    10,    11,    12,    45,   151,   119,    16,    17,
      18,    19,    20,    22,    23,    55,    32,   148,   119,   164,
      15,   155,   152,   168,   147,   170,   171,     1,   160,   150,
     175,   119,     1,   114,   156,    51,    52,    53,    54,   163,
     185,     1,   187,   188,   151,    61,   151,   192,    22,    23,
     195,   114,   197,    22,    23,     1,   114,   202,   203,   147,
     114,   206,    22,    23,   209,   114,   211,   212,   114,   214,
     215,   155,   217,   557,   558,   220,    22,    23,   223,   224,
     119,   226,   119,   228,   145,   230,   119,   232,   120,   234,
     155,   119,   237,   117,   118,   154,   119,   242,   243,     1,
     124,   125,   247,   120,   154,   250,   251,     1,   253,   254,
     145,   256,   164,   165,   166,   167,     1,   169,   168,   119,
      22,    23,   267,   166,   167,   270,   114,   272,    22,    23,
     275,   276,     1,   114,     1,   162,   122,    22,    23,     6,
       7,   164,   165,   166,   167,   122,   154,    16,    17,    18,
      19,    20,     1,    22,    23,   164,   165,   166,   167,   304,
     168,   306,     1,   122,   122,   181,    15,   183,    35,     1,
     122,    11,    12,    22,    23,   320,   321,   122,     1,   153,
     337,   150,   118,    22,    23,    25,    26,   119,   122,   149,
      22,    23,   208,   122,   351,   118,   143,   144,   145,    22,
      23,    92,    81,    43,    44,   332,    73,   352,    48,   459,
      50,   337,   357,   358,   108,    82,   493,   119,    58,    59,
     116,    88,   118,   437,   620,    65,     3,     4,     5,     6,
      70,    71,    72,    10,    11,    12,   325,   277,   383,    -1,
      80,   137,   138,   139,    -1,    85,    86,    -1,    -1,    -1,
     395,    -1,    -1,   398,   121,   400,   401,   402,   403,    -1,
      -1,    16,    17,    18,    19,    20,    -1,   134,   135,    -1,
     119,   138,    -1,   140,    -1,    -1,   143,   144,   145,   319,
     119,    -1,    -1,   152,    -1,   325,   326,   117,   118,    -1,
     330,   238,   239,   333,   124,   125,   119,    -1,    -1,   388,
      -1,   446,    -1,   448,    -1,    -1,   451,   347,   453,    -1,
     455,   456,    -1,   470,   459,    -1,   146,   474,    -1,    -1,
     465,   466,    -1,    -1,   469,   341,   471,   472,   473,   152,
      -1,   476,   199,    -1,   201,   480,    -1,    -1,   483,    -1,
      -1,   486,   487,   488,   489,    -1,   491,    -1,   388,   494,
      -1,   496,   497,    -1,     8,     9,    -1,    -1,     1,   504,
     505,   506,   507,    -1,   509,    -1,    -1,    -1,   235,    -1,
     147,   238,   239,    16,    17,    18,    19,    20,    -1,    33,
      34,    -1,    -1,    -1,   161,   421,    -1,   403,   255,    -1,
     426,   258,   259,    -1,   430,   262,   151,   554,   265,   435,
     436,    16,    17,    18,    19,    20,    -1,   447,    -1,    -1,
      -1,   278,   279,    -1,    -1,   282,   283,    -1,    -1,    -1,
      74,    75,    -1,    77,     1,    79,     3,     4,     5,     6,
     117,   118,    -1,    10,    11,    12,    -1,   124,   125,    16,
      17,    18,    19,    20,    -1,   312,   313,   314,   315,    -1,
      -1,    -1,    28,    29,    30,    -1,    -1,     1,    -1,   146,
      -1,   328,   329,    -1,    -1,   332,    -1,    -1,    -1,   336,
      -1,    -1,    16,    17,    18,    19,    20,   622,    22,    23,
      -1,    -1,    -1,   519,   351,     1,   522,   523,     4,   525,
     526,   527,   117,   118,    10,    11,    12,   151,    -1,   124,
     125,   155,   156,   157,    -1,   148,    -1,   161,   162,   117,
     118,   165,    -1,    89,    90,    -1,   124,   125,    -1,   173,
      -1,   146,    -1,   177,    -1,   392,    -1,    -1,    -1,   369,
      -1,   371,   186,   164,   165,   166,   167,   404,   146,   154,
     194,    -1,   409,    -1,   411,   581,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,    -1,    -1,    -1,    10,    11,    12,
     214,    -1,   216,    -1,   218,   219,    -1,   221,     1,    -1,
     437,    -1,    -1,     6,     7,    -1,    -1,   154,    -1,     1,
      -1,     3,     4,     5,     6,    -1,    -1,    -1,    10,    11,
      12,   168,   459,   247,    16,    17,    18,    19,    20,    -1,
      22,    23,    35,   470,    -1,    -1,    -1,   474,    -1,    -1,
     154,    -1,   266,    -1,    14,    15,    16,    17,    18,    -1,
      20,   275,   276,    23,    24,    -1,   493,    -1,    -1,    -1,
      -1,    31,    32,    -1,    -1,    -1,    36,    -1,    -1,    -1,
      73,    -1,    42,    43,    44,    45,    46,    47,    -1,    -1,
      -1,    51,    52,    53,    54,    55,     3,     4,     5,     6,
      -1,    61,    -1,    10,    11,    12,    -1,    -1,    -1,    16,
      17,    18,    19,    20,     1,    -1,     3,     4,     5,     6,
      -1,    -1,     9,    10,    11,    12,    -1,   554,   121,    16,
      17,    18,    19,    20,   147,    22,    23,   119,    -1,    -1,
      -1,   134,   135,    -1,    -1,   138,    -1,   140,     3,     4,
       5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,    -1,
      -1,    16,    17,    18,    19,    20,    -1,    -1,    -1,   383,
      -1,    -1,   154,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    -1,    -1,    10,    11,    12,   168,    -1,   615,    16,
      17,    18,    19,    20,     3,     4,     5,     6,    -1,    -1,
      -1,    10,    11,    12,    -1,    -1,   199,     1,   201,     3,
       4,     5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,
      -1,   181,     1,   183,     3,     4,     5,     6,    22,    23,
       9,    10,    11,    12,    -1,    -1,    -1,    16,    17,    18,
      19,    20,    -1,    22,    23,    -1,    -1,   154,   208,    -1,
      -1,   465,    16,    17,    18,    19,    20,   164,   165,   166,
     167,   168,   255,    -1,   151,   258,   259,    -1,    -1,   262,
      -1,    -1,   265,    -1,    -1,    -1,   490,   164,   165,   166,
     167,    -1,    -1,    -1,    -1,     1,    -1,     3,     4,     5,
       6,    -1,    -1,     1,    10,    11,    12,    -1,    -1,   154,
      16,    17,    18,    19,    20,    -1,    -1,    -1,    16,    17,
      18,    19,    20,   168,    22,    23,    -1,   277,    -1,   312,
     313,   314,   315,    -1,   151,    -1,   135,   136,   137,   138,
     139,   140,    -1,    -1,    -1,   328,    -1,   164,   165,   166,
     167,     1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,
      10,    11,    12,   147,    -1,    -1,    -1,    -1,    -1,   319,
      -1,    -1,    22,    23,    -1,   325,   326,   161,    -1,    -1,
     330,    -1,   151,   333,    -1,    -1,    -1,   156,    -1,     1,
      -1,   341,     4,   597,    -1,    -1,    -1,   347,    10,    11,
      12,    -1,    -1,    -1,    -1,    -1,    -1,   113,    -1,   392,
      22,    23,    -1,    -1,     1,    -1,     3,     4,     5,     6,
      -1,   404,     9,    10,    11,    12,    -1,    14,    -1,    16,
      17,    18,    19,    20,    -1,    22,    23,    -1,   388,    -1,
       1,    -1,     3,     4,     5,     6,    -1,    -1,   154,    10,
      11,    12,    -1,   403,    -1,    -1,    -1,     7,    -1,    -1,
      -1,    22,    23,    13,    -1,    -1,    -1,    -1,    -1,   119,
      -1,    -1,    22,    -1,    -1,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      -1,    -1,    42,    43,    44,    45,    46,   447,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   119,   109,
     110,   111,   112,    -1,   151,    -1,     1,    -1,     3,     4,
       5,     6,    -1,    -1,     9,    10,    11,    12,    -1,    -1,
      -1,    16,    17,    18,    19,    20,   147,    22,    23,    -1,
      -1,    -1,     7,    -1,     3,     4,     5,     6,    13,    -1,
     150,    10,    11,    12,    -1,    -1,    -1,    -1,   158,   159,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    -1,    -1,    42,    43,    44,
      45,    46,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,    -1,   109,   110,   111,   112,     3,     4,
       5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,    -1,
      -1,    16,    17,    18,    19,    20,   151,     1,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,
      -1,    -1,    16,    17,    18,    19,    20,    -1,    22,    23,
      -1,    -1,    -1,   158,   159,     1,    -1,     3,     4,     5,
       6,    -1,    -1,     9,    10,    11,    12,    -1,    -1,    -1,
      16,    17,    18,    19,    20,     1,    -1,     3,     4,     5,
       6,    -1,    -1,     9,    10,    11,    12,    -1,    -1,    -1,
      16,    17,    18,    19,    20,     1,    -1,     3,     4,     5,
       6,    -1,    -1,     9,    10,    11,    12,    -1,    -1,    -1,
      16,    17,    18,    19,    20,    -1,    -1,     1,   113,     3,
       4,     5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,
      -1,    -1,    16,    17,    18,    19,    20,     3,     4,     5,
       6,    -1,    -1,     9,    10,    11,    12,    -1,    -1,    -1,
      16,    17,    18,    19,    20,     3,     4,     5,     6,   154,
      -1,    -1,    10,    11,    12,    -1,    -1,    -1,    16,    17,
      18,    19,    20,    -1,    -1,    -1,    -1,   151,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    10,    11,    12,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    12,    -1,
      -1,    -1,    -1,    -1,    -1,   151,    -1,    -1,     3,     4,
       5,     6,    -1,    -1,    -1,    10,    11,    12,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   151,     3,     4,     5,     6,
      -1,    -1,    -1,    10,    11,    12,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   151,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   151,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   151,    -1,    -1,    -1,   113,
      -1,   115,   116,    -1,    -1,   119,    -1,   121,    -1,   123,
      -1,    -1,    -1,   151,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   116,    -1,    -1,   119,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   128,    -1,   130,    -1,    -1,    -1,    -1,
     117,   118,    -1,    -1,    -1,    -1,    -1,   124,   125,   126
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
     104,   105,   106,   107,   109,   110,   111,   112,   150,   158,
     159,   171,   172,   174,   175,   176,   177,   178,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   220,     3,     4,
       5,     6,    10,    11,    12,    16,    17,    18,    19,    20,
     154,   168,   195,   204,   207,   215,   217,   218,     1,    22,
      23,   160,   173,   173,   207,   207,   119,   173,   203,   173,
     203,     1,   148,   191,   191,   191,     1,     4,    10,    11,
      12,   198,   216,     9,   151,   200,   202,   204,   205,   206,
     218,   200,   200,   173,   202,   204,   173,   195,   196,   197,
     204,   156,   173,   156,   173,   199,   200,   202,    14,   173,
     193,   194,   173,   194,   173,   202,   200,   191,   191,   173,
       1,   173,   198,   198,   198,   202,   173,   199,   173,   203,
     173,   203,   207,   204,   149,   173,   189,   173,   194,   189,
     173,   189,   152,   173,   204,   191,   204,   191,   204,   202,
     151,   204,   173,   204,   191,   173,   191,   173,   199,   173,
     199,   173,   199,   173,   199,   202,   173,   189,   191,   191,
     173,   173,   199,   153,   173,   173,   173,   191,   173,   149,
     190,   147,   147,   191,   191,   191,   207,   173,   203,   203,
     173,   173,   203,   173,   173,   203,   191,   190,     1,   173,
     192,   207,   108,   173,   173,   191,   191,    21,   113,   115,
     116,   119,   121,   123,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   218,   222,   173,   198,   173,   198,     0,   174,    22,
     207,   154,   164,   165,   166,   167,   173,   173,   207,   207,
     207,   207,     1,   192,     1,   192,   192,   173,   203,   203,
     203,   203,   203,   203,   173,   203,    21,   119,   173,   173,
     173,   203,   173,    14,   199,   199,   173,   203,   173,   173,
     173,    15,     1,   203,   173,   179,   173,   207,   207,   173,
     173,   173,   199,   173,   173,   173,   173,   203,   173,   203,
     173,   203,   203,   173,   203,   173,   173,   173,   173,   173,
     173,   173,   147,   211,   212,   218,   173,   192,   192,   173,
     173,   173,   203,   173,   173,   147,   173,   173,   207,   173,
     207,   207,   207,   207,   203,   173,   173,   173,   173,   203,
     173,   203,   202,   135,   136,   137,   138,   139,   140,   218,
     218,   119,   117,   118,   124,   125,   146,   223,   224,   116,
     119,   128,   130,   218,   218,   119,   119,   119,   173,   173,
       1,   169,   207,   207,   207,   207,   201,   203,   202,   173,
     173,   201,     1,   200,     1,   202,   207,   147,   161,   213,
     214,   218,   202,     9,   205,   211,   202,   218,   197,   199,
      15,   208,   208,   202,   113,   195,   209,   210,   218,   173,
     147,   219,   220,   221,   173,   173,   208,   152,   191,   191,
     151,   151,   173,   203,   201,     1,   200,   207,   173,   173,
     173,   173,   173,   173,   199,   207,   218,   218,   163,   227,
     114,   114,   114,   114,   114,   114,   114,   119,   223,   146,
     223,   145,   119,   146,   223,   119,   119,   119,   122,   223,
     223,   117,   118,   125,   126,   218,   224,   173,   202,   173,
     173,   173,   173,   173,   173,   214,   173,   173,   173,   209,
     173,   173,   173,   209,   155,   173,   173,   120,   120,   173,
     173,   173,   173,   173,   203,   173,   212,   173,   173,   173,
     173,   173,   173,   173,   173,   119,   117,   125,   223,   145,
     223,   146,   223,   122,   145,   223,   223,   126,   223,   119,
       1,   114,     1,   209,   220,   220,   162,   226,   116,   118,
     137,   138,   139,   114,   114,   122,   223,   122,   122,   145,
     122,   122,   122,   122,   118,   203,   119,   122,   122,   218,
     225,   118,   227,   173
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   170,   171,   171,   171,   172,   172,   173,   173,   173,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   176,   176,
     176,   177,   177,   177,   177,   177,   177,   177,   177,   177,
     177,   179,   178,   178,   178,   178,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   181,   181,   181,   181,
     181,   181,   181,   181,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   183,   183,   183,   183,   183,
     183,   183,   183,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   186,   186,   186,   187,   187,
     188,   189,   190,   190,   191,   191,   192,   192,   193,   193,
     194,   194,   195,   195,   196,   196,   197,   198,   198,   199,
     199,   200,   200,   200,   201,   201,   202,   202,   202,   203,
     203,   204,   204,   204,   204,   204,   205,   206,   207,   207,
     207,   207,   207,   207,   207,   208,   208,   209,   209,   209,
     209,   209,   210,   210,   211,   211,   212,   212,   213,   213,
     214,   214,   214,   215,   215,   216,   216,   216,   216,   217,
     217,   217,   218,   218,   218,   218,   218,   219,   219,   219,
     220,   221,   221,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   222,   223,   223,   223,   223,   224,
     225,   225,   226,   226,   227,   227
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     3,     5,     3,     2,     2,
       3,     2,     3,     2,     4,     2,     3,     3,     2,     4,
       2,     4,     2,     4,     2,     4,     2,     1,     2,     3,
       3,     5,     3,     5,     3,    10,     3,     5,     3,     2,
       5,     0,     5,     3,     3,     2,     5,     5,     5,     5,
       5,     3,     2,     3,     2,     3,     2,     3,     2,     3,
       2,     2,     2,     4,     5,     5,     5,     2,     3,     2,
       5,     2,     5,     2,     3,     2,     3,     2,     3,     5,
       3,     2,     5,     5,     4,     3,     2,     3,     2,     3,
       2,     2,     2,     3,     3,     2,     3,     3,     3,     3,
       2,     3,     2,     3,     5,     3,     4,     3,     3,     2,
       4,     4,     2,     3,     3,     2,     5,     5,     5,     3,
       4,     5,     4,     5,     4,     5,     5,     2,     3,     4,
       3,     3,     5,     3,     5,     3,     2,     3,     4,     2,
       2,     1,     1,     0,     1,     1,     1,     1,     2,     1,
       1,     0,     1,     2,     3,     1,     3,     1,     1,     1,
       1,     3,     1,     3,     2,     0,     1,     3,     1,     1,
       0,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     3,     1,     2,     0,     3,     3,     3,
       3,     1,     1,     1,     3,     1,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     2,
       2,     1,     1,     2,     1,     3,     3,     3,     3,     3,
       5,     7,     5,     3,     3,     3,     3,     3,     3,     5,
       5,     5,     5,     0,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     3,     3,     4,     3,     4,     2,     3,
       3,     3,     3,     5,     5,     6,     5,     6,     4,     5,
       5,     5,     5,     3,     5,     1,     1,     1,     1,     1,
       1,     0,     1,     0,     1,     0
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
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
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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
      if (yytable_value_is_error (yyn))
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

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
#line 201 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = 0; }
#line 2366 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 3:
#line 202 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = 0; }
#line 2372 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 4:
#line 203 "mon_parse.y" /* yacc.c:1646  */
    { new_cmd = 1; asm_mode = 0;  (yyval.i) = 0; }
#line 2378 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 9:
#line 212 "mon_parse.y" /* yacc.c:1646  */
    { return ERR_EXPECT_END_CMD; }
#line 2384 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 22:
#line 227 "mon_parse.y" /* yacc.c:1646  */
    { return ERR_BAD_CMD; }
#line 2390 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 23:
#line 231 "mon_parse.y" /* yacc.c:1646  */
    { mon_bank(e_default_space, NULL); }
#line 2396 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 24:
#line 233 "mon_parse.y" /* yacc.c:1646  */
    { mon_bank((yyvsp[-1].i), NULL); }
#line 2402 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 25:
#line 235 "mon_parse.y" /* yacc.c:1646  */
    { mon_bank(e_default_space, (yyvsp[-1].str)); }
#line 2408 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 26:
#line 237 "mon_parse.y" /* yacc.c:1646  */
    { mon_bank((yyvsp[-3].i), (yyvsp[-1].str)); }
#line 2414 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 27:
#line 239 "mon_parse.y" /* yacc.c:1646  */
    { mon_jump((yyvsp[-1].a)); }
#line 2420 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 28:
#line 241 "mon_parse.y" /* yacc.c:1646  */
    { mon_go(); }
#line 2426 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 29:
#line 243 "mon_parse.y" /* yacc.c:1646  */
    { mon_display_io_regs(0); }
#line 2432 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 30:
#line 245 "mon_parse.y" /* yacc.c:1646  */
    { mon_display_io_regs((yyvsp[-1].a)); }
#line 2438 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 31:
#line 247 "mon_parse.y" /* yacc.c:1646  */
    { monitor_cpu_type_set(""); }
#line 2444 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 32:
#line 249 "mon_parse.y" /* yacc.c:1646  */
    { monitor_cpu_type_set((yyvsp[-1].str)); }
#line 2450 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 33:
#line 251 "mon_parse.y" /* yacc.c:1646  */
    { mon_cpuhistory(-1); }
#line 2456 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 34:
#line 253 "mon_parse.y" /* yacc.c:1646  */
    { mon_cpuhistory((yyvsp[-1].i)); }
#line 2462 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 35:
#line 255 "mon_parse.y" /* yacc.c:1646  */
    { mon_instruction_return(); }
#line 2468 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 36:
#line 257 "mon_parse.y" /* yacc.c:1646  */
    { machine_write_snapshot((yyvsp[-1].str),0,0,0); /* FIXME */ }
#line 2474 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 37:
#line 259 "mon_parse.y" /* yacc.c:1646  */
    { machine_read_snapshot((yyvsp[-1].str), 0); }
#line 2480 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 38:
#line 261 "mon_parse.y" /* yacc.c:1646  */
    { mon_instructions_step(-1); }
#line 2486 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 39:
#line 263 "mon_parse.y" /* yacc.c:1646  */
    { mon_instructions_step((yyvsp[-1].i)); }
#line 2492 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 40:
#line 265 "mon_parse.y" /* yacc.c:1646  */
    { mon_instructions_next(-1); }
#line 2498 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 41:
#line 267 "mon_parse.y" /* yacc.c:1646  */
    { mon_instructions_next((yyvsp[-1].i)); }
#line 2504 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 42:
#line 269 "mon_parse.y" /* yacc.c:1646  */
    { mon_stack_up(-1); }
#line 2510 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 43:
#line 271 "mon_parse.y" /* yacc.c:1646  */
    { mon_stack_up((yyvsp[-1].i)); }
#line 2516 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 44:
#line 273 "mon_parse.y" /* yacc.c:1646  */
    { mon_stack_down(-1); }
#line 2522 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 45:
#line 275 "mon_parse.y" /* yacc.c:1646  */
    { mon_stack_down((yyvsp[-1].i)); }
#line 2528 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 46:
#line 277 "mon_parse.y" /* yacc.c:1646  */
    { mon_display_screen(); }
#line 2534 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 48:
#line 282 "mon_parse.y" /* yacc.c:1646  */
    { (monitor_cpu_for_memspace[default_memspace]->mon_register_print)(default_memspace); }
#line 2540 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 49:
#line 284 "mon_parse.y" /* yacc.c:1646  */
    { (monitor_cpu_for_memspace[(yyvsp[-1].i)]->mon_register_print)((yyvsp[-1].i)); }
#line 2546 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 51:
#line 289 "mon_parse.y" /* yacc.c:1646  */
    {
                        /* What about the memspace? */
                        mon_playback_init((yyvsp[-1].str));
                    }
#line 2555 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 52:
#line 294 "mon_parse.y" /* yacc.c:1646  */
    {
                        /* What about the memspace? */
                        mon_playback_init((yyvsp[-1].str));
                    }
#line 2564 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 53:
#line 299 "mon_parse.y" /* yacc.c:1646  */
    { mon_save_symbols((yyvsp[-3].i), (yyvsp[-1].str)); }
#line 2570 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 54:
#line 301 "mon_parse.y" /* yacc.c:1646  */
    { mon_save_symbols(e_default_space, (yyvsp[-1].str)); }
#line 2576 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 55:
#line 303 "mon_parse.y" /* yacc.c:1646  */
    { mon_add_name_to_symbol_table((yyvsp[-8].a), (yyvsp[-6].str), (yyvsp[-4].ldt), (yyvsp[-2].i), (yyvsp[-1].str)); }
#line 2582 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 56:
#line 305 "mon_parse.y" /* yacc.c:1646  */
    { mon_remove_name_from_symbol_table(e_default_space, (yyvsp[-1].str)); }
#line 2588 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 57:
#line 307 "mon_parse.y" /* yacc.c:1646  */
    { mon_remove_name_from_symbol_table((yyvsp[-3].i), (yyvsp[-1].str)); }
#line 2594 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 58:
#line 309 "mon_parse.y" /* yacc.c:1646  */
    { mon_print_symbol_table((yyvsp[-1].i)); }
#line 2600 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 59:
#line 311 "mon_parse.y" /* yacc.c:1646  */
    { mon_print_symbol_table(e_default_space); }
#line 2606 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 60:
#line 313 "mon_parse.y" /* yacc.c:1646  */
    {
                        mon_add_name_to_symbol_table((yyvsp[-2].a), mon_prepend_dot_to_name((yyvsp[-4].str)), e_label_code, 0, (yyvsp[-1].str));
                    }
#line 2614 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 61:
#line 319 "mon_parse.y" /* yacc.c:1646  */
    { mon_start_assemble_mode((yyvsp[0].a), NULL); }
#line 2620 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 62:
#line 320 "mon_parse.y" /* yacc.c:1646  */
    { }
#line 2626 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 63:
#line 322 "mon_parse.y" /* yacc.c:1646  */
    { mon_start_assemble_mode((yyvsp[-1].a), NULL); }
#line 2632 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 64:
#line 324 "mon_parse.y" /* yacc.c:1646  */
    { mon_disassemble_lines((yyvsp[-1].range)[0], (yyvsp[-1].range)[1]); }
#line 2638 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 65:
#line 326 "mon_parse.y" /* yacc.c:1646  */
    { mon_disassemble_lines(BAD_ADDR, BAD_ADDR); }
#line 2644 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 66:
#line 330 "mon_parse.y" /* yacc.c:1646  */
    { mon_memory_move((yyvsp[-3].range)[0], (yyvsp[-3].range)[1], (yyvsp[-1].a)); }
#line 2650 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 67:
#line 332 "mon_parse.y" /* yacc.c:1646  */
    { mon_memory_compare((yyvsp[-3].range)[0], (yyvsp[-3].range)[1], (yyvsp[-1].a)); }
#line 2656 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 68:
#line 334 "mon_parse.y" /* yacc.c:1646  */
    { mon_memory_fill((yyvsp[-3].range)[0], (yyvsp[-3].range)[1],(unsigned char *)(yyvsp[-1].str)); }
#line 2662 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 69:
#line 336 "mon_parse.y" /* yacc.c:1646  */
    { mon_memory_hunt((yyvsp[-3].range)[0], (yyvsp[-3].range)[1],(unsigned char *)(yyvsp[-1].str)); }
#line 2668 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 70:
#line 338 "mon_parse.y" /* yacc.c:1646  */
    { mon_memory_display((yyvsp[-3].rt), (yyvsp[-1].range)[0], (yyvsp[-1].range)[1], DF_PETSCII); }
#line 2674 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 71:
#line 340 "mon_parse.y" /* yacc.c:1646  */
    { mon_memory_display(default_radix, (yyvsp[-1].range)[0], (yyvsp[-1].range)[1], DF_PETSCII); }
#line 2680 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 72:
#line 342 "mon_parse.y" /* yacc.c:1646  */
    { mon_memory_display(default_radix, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
#line 2686 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 73:
#line 344 "mon_parse.y" /* yacc.c:1646  */
    { mon_memory_display_data((yyvsp[-1].range)[0], (yyvsp[-1].range)[1], 8, 8); }
#line 2692 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 74:
#line 346 "mon_parse.y" /* yacc.c:1646  */
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 8, 8); }
#line 2698 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 75:
#line 348 "mon_parse.y" /* yacc.c:1646  */
    { mon_memory_display_data((yyvsp[-1].range)[0], (yyvsp[-1].range)[1], 24, 21); }
#line 2704 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 76:
#line 350 "mon_parse.y" /* yacc.c:1646  */
    { mon_memory_display_data(BAD_ADDR, BAD_ADDR, 24, 21); }
#line 2710 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 77:
#line 352 "mon_parse.y" /* yacc.c:1646  */
    { mon_memory_display(0, (yyvsp[-1].range)[0], (yyvsp[-1].range)[1], DF_PETSCII); }
#line 2716 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 78:
#line 354 "mon_parse.y" /* yacc.c:1646  */
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_PETSCII); }
#line 2722 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 79:
#line 356 "mon_parse.y" /* yacc.c:1646  */
    { mon_memory_display(0, (yyvsp[-1].range)[0], (yyvsp[-1].range)[1], DF_SCREEN_CODE); }
#line 2728 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 80:
#line 358 "mon_parse.y" /* yacc.c:1646  */
    { mon_memory_display(0, BAD_ADDR, BAD_ADDR, DF_SCREEN_CODE); }
#line 2734 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 81:
#line 360 "mon_parse.y" /* yacc.c:1646  */
    { mon_memmap_zap(); }
#line 2740 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 82:
#line 362 "mon_parse.y" /* yacc.c:1646  */
    { mon_memmap_show(-1,BAD_ADDR,BAD_ADDR); }
#line 2746 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 83:
#line 364 "mon_parse.y" /* yacc.c:1646  */
    { mon_memmap_show((yyvsp[-1].i),BAD_ADDR,BAD_ADDR); }
#line 2752 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 84:
#line 366 "mon_parse.y" /* yacc.c:1646  */
    { mon_memmap_show((yyvsp[-2].i),(yyvsp[-1].range)[0],(yyvsp[-1].range)[1]); }
#line 2758 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 85:
#line 368 "mon_parse.y" /* yacc.c:1646  */
    { mon_memmap_save((yyvsp[-3].str),(yyvsp[-1].i)); }
#line 2764 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 86:
#line 372 "mon_parse.y" /* yacc.c:1646  */
    {
                      if ((yyvsp[-3].i)) {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[-2].range)[0], (yyvsp[-2].range)[1], TRUE, (yyvsp[-3].i), FALSE);
                      } else {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[-2].range)[0], (yyvsp[-2].range)[1], TRUE, e_exec, FALSE);
                      }
                      mon_breakpoint_set_checkpoint_condition(temp, (yyvsp[-1].cond_node));
                  }
#line 2777 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 87:
#line 381 "mon_parse.y" /* yacc.c:1646  */
    { mon_breakpoint_print_checkpoints(); }
#line 2783 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 88:
#line 384 "mon_parse.y" /* yacc.c:1646  */
    {
                      mon_breakpoint_add_checkpoint((yyvsp[-1].range)[0], (yyvsp[-1].range)[1], TRUE, e_exec, TRUE);
                  }
#line 2791 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 89:
#line 388 "mon_parse.y" /* yacc.c:1646  */
    { mon_breakpoint_print_checkpoints(); }
#line 2797 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 90:
#line 391 "mon_parse.y" /* yacc.c:1646  */
    {
                      if ((yyvsp[-3].i)) {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[-2].range)[0], (yyvsp[-2].range)[1], TRUE, (yyvsp[-3].i), FALSE);
                      } else {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[-2].range)[0], (yyvsp[-2].range)[1], TRUE, e_load | e_store, FALSE);
                      }
                      mon_breakpoint_set_checkpoint_condition(temp, (yyvsp[-1].cond_node));
                  }
#line 2810 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 91:
#line 400 "mon_parse.y" /* yacc.c:1646  */
    { mon_breakpoint_print_checkpoints(); }
#line 2816 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 92:
#line 403 "mon_parse.y" /* yacc.c:1646  */
    {
                      if ((yyvsp[-3].i)) {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[-2].range)[0], (yyvsp[-2].range)[1], FALSE, (yyvsp[-3].i), FALSE);
                      } else {
                          temp = mon_breakpoint_add_checkpoint((yyvsp[-2].range)[0], (yyvsp[-2].range)[1], FALSE, e_load | e_store, FALSE);
                      }
                      mon_breakpoint_set_checkpoint_condition(temp, (yyvsp[-1].cond_node));
                  }
#line 2829 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 93:
#line 412 "mon_parse.y" /* yacc.c:1646  */
    { mon_breakpoint_print_checkpoints(); }
#line 2835 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 94:
#line 417 "mon_parse.y" /* yacc.c:1646  */
    { mon_breakpoint_switch_checkpoint(e_ON, (yyvsp[-1].i)); }
#line 2841 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 95:
#line 419 "mon_parse.y" /* yacc.c:1646  */
    { mon_breakpoint_switch_checkpoint(e_ON, -1); }
#line 2847 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 96:
#line 421 "mon_parse.y" /* yacc.c:1646  */
    { mon_breakpoint_switch_checkpoint(e_OFF, (yyvsp[-1].i)); }
#line 2853 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 97:
#line 423 "mon_parse.y" /* yacc.c:1646  */
    { mon_breakpoint_switch_checkpoint(e_OFF, -1); }
#line 2859 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 98:
#line 425 "mon_parse.y" /* yacc.c:1646  */
    { mon_breakpoint_set_ignore_count((yyvsp[-1].i), -1); }
#line 2865 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 99:
#line 427 "mon_parse.y" /* yacc.c:1646  */
    { mon_breakpoint_set_ignore_count((yyvsp[-3].i), (yyvsp[-1].i)); }
#line 2871 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 100:
#line 429 "mon_parse.y" /* yacc.c:1646  */
    { mon_breakpoint_delete_checkpoint((yyvsp[-1].i)); }
#line 2877 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 101:
#line 431 "mon_parse.y" /* yacc.c:1646  */
    { mon_breakpoint_delete_checkpoint(-1); }
#line 2883 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 102:
#line 433 "mon_parse.y" /* yacc.c:1646  */
    { mon_breakpoint_set_checkpoint_condition((yyvsp[-3].i), (yyvsp[-1].cond_node)); }
#line 2889 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 103:
#line 435 "mon_parse.y" /* yacc.c:1646  */
    { mon_breakpoint_set_checkpoint_command((yyvsp[-3].i), (yyvsp[-1].str)); }
#line 2895 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 104:
#line 437 "mon_parse.y" /* yacc.c:1646  */
    { return ERR_EXPECT_STRING; }
#line 2901 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 105:
#line 441 "mon_parse.y" /* yacc.c:1646  */
    { sidefx = (((yyvsp[-1].action) == e_TOGGLE) ? (sidefx ^ 1) : (yyvsp[-1].action)); }
#line 2907 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 106:
#line 443 "mon_parse.y" /* yacc.c:1646  */
    {
                         mon_out("I/O side effects are %s\n",
                                   sidefx ? "enabled" : "disabled");
                     }
#line 2916 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 107:
#line 448 "mon_parse.y" /* yacc.c:1646  */
    { default_radix = (yyvsp[-1].rt); }
#line 2922 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 108:
#line 450 "mon_parse.y" /* yacc.c:1646  */
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
#line 2943 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 109:
#line 468 "mon_parse.y" /* yacc.c:1646  */
    { monitor_change_device((yyvsp[-1].i)); }
#line 2949 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 110:
#line 470 "mon_parse.y" /* yacc.c:1646  */
    { mon_export(); }
#line 2955 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 111:
#line 472 "mon_parse.y" /* yacc.c:1646  */
    { mon_quit(); YYACCEPT; }
#line 2961 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 112:
#line 474 "mon_parse.y" /* yacc.c:1646  */
    { mon_exit(); YYACCEPT; }
#line 2967 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 113:
#line 478 "mon_parse.y" /* yacc.c:1646  */
    { mon_drive_execute_disk_cmd((yyvsp[-1].str)); }
#line 2973 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 114:
#line 480 "mon_parse.y" /* yacc.c:1646  */
    { mon_out("\t%d\n",(yyvsp[-1].i)); }
#line 2979 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 115:
#line 482 "mon_parse.y" /* yacc.c:1646  */
    { mon_command_print_help(NULL); }
#line 2985 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 116:
#line 484 "mon_parse.y" /* yacc.c:1646  */
    { mon_command_print_help((yyvsp[-1].str)); }
#line 2991 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 117:
#line 486 "mon_parse.y" /* yacc.c:1646  */
    { mon_print_convert((yyvsp[-1].i)); }
#line 2997 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 118:
#line 488 "mon_parse.y" /* yacc.c:1646  */
    { mon_change_dir((yyvsp[-1].str)); }
#line 3003 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 119:
#line 490 "mon_parse.y" /* yacc.c:1646  */
    { mon_keyboard_feed((yyvsp[-1].str)); }
#line 3009 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 120:
#line 492 "mon_parse.y" /* yacc.c:1646  */
    { mon_backtrace(); }
#line 3015 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 121:
#line 494 "mon_parse.y" /* yacc.c:1646  */
    { mon_show_dir((yyvsp[-1].str)); }
#line 3021 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 122:
#line 496 "mon_parse.y" /* yacc.c:1646  */
    { mon_show_pwd(); }
#line 3027 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 123:
#line 498 "mon_parse.y" /* yacc.c:1646  */
    { mon_screenshot_save((yyvsp[-1].str),-1); }
#line 3033 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 124:
#line 500 "mon_parse.y" /* yacc.c:1646  */
    { mon_screenshot_save((yyvsp[-3].str),(yyvsp[-1].i)); }
#line 3039 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 125:
#line 502 "mon_parse.y" /* yacc.c:1646  */
    { mon_resource_get((yyvsp[-1].str)); }
#line 3045 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 126:
#line 504 "mon_parse.y" /* yacc.c:1646  */
    { mon_resource_set((yyvsp[-2].str),(yyvsp[-1].str)); }
#line 3051 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 127:
#line 506 "mon_parse.y" /* yacc.c:1646  */
    { resources_load((yyvsp[-1].str)); }
#line 3057 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 128:
#line 508 "mon_parse.y" /* yacc.c:1646  */
    { resources_save((yyvsp[-1].str)); }
#line 3063 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 129:
#line 510 "mon_parse.y" /* yacc.c:1646  */
    { mon_reset_machine(-1); }
#line 3069 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 130:
#line 512 "mon_parse.y" /* yacc.c:1646  */
    { mon_reset_machine((yyvsp[-1].i)); }
#line 3075 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 131:
#line 514 "mon_parse.y" /* yacc.c:1646  */
    { mon_tape_ctrl((yyvsp[-1].i)); }
#line 3081 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 132:
#line 516 "mon_parse.y" /* yacc.c:1646  */
    { mon_cart_freeze(); }
#line 3087 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 133:
#line 518 "mon_parse.y" /* yacc.c:1646  */
    { }
#line 3093 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 134:
#line 520 "mon_parse.y" /* yacc.c:1646  */
    { mon_stopwatch_reset(); }
#line 3099 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 135:
#line 522 "mon_parse.y" /* yacc.c:1646  */
    { mon_stopwatch_show("Stopwatch: ", "\n"); }
#line 3105 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 136:
#line 526 "mon_parse.y" /* yacc.c:1646  */
    { mon_file_load((yyvsp[-3].str), (yyvsp[-2].i), (yyvsp[-1].a), FALSE); }
#line 3111 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 137:
#line 528 "mon_parse.y" /* yacc.c:1646  */
    { mon_file_load((yyvsp[-3].str), (yyvsp[-2].i), (yyvsp[-1].a), TRUE); }
#line 3117 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 138:
#line 530 "mon_parse.y" /* yacc.c:1646  */
    { mon_file_save((yyvsp[-3].str), (yyvsp[-2].i), (yyvsp[-1].range)[0], (yyvsp[-1].range)[1], FALSE); }
#line 3123 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 139:
#line 532 "mon_parse.y" /* yacc.c:1646  */
    { return ERR_EXPECT_DEVICE_NUM; }
#line 3129 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 140:
#line 534 "mon_parse.y" /* yacc.c:1646  */
    { return ERR_EXPECT_ADDRESS; }
#line 3135 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 141:
#line 536 "mon_parse.y" /* yacc.c:1646  */
    { mon_file_save((yyvsp[-3].str), (yyvsp[-2].i), (yyvsp[-1].range)[0], (yyvsp[-1].range)[1], TRUE); }
#line 3141 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 142:
#line 538 "mon_parse.y" /* yacc.c:1646  */
    { return ERR_EXPECT_ADDRESS; }
#line 3147 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 143:
#line 540 "mon_parse.y" /* yacc.c:1646  */
    { mon_file_verify((yyvsp[-3].str),(yyvsp[-2].i),(yyvsp[-1].a)); }
#line 3153 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 144:
#line 542 "mon_parse.y" /* yacc.c:1646  */
    { return ERR_EXPECT_ADDRESS; }
#line 3159 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 145:
#line 544 "mon_parse.y" /* yacc.c:1646  */
    { mon_drive_block_cmd(0,(yyvsp[-3].i),(yyvsp[-2].i),(yyvsp[-1].a)); }
#line 3165 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 146:
#line 546 "mon_parse.y" /* yacc.c:1646  */
    { mon_drive_block_cmd(1,(yyvsp[-3].i),(yyvsp[-2].i),(yyvsp[-1].a)); }
#line 3171 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 147:
#line 548 "mon_parse.y" /* yacc.c:1646  */
    { mon_drive_list(-1); }
#line 3177 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 148:
#line 550 "mon_parse.y" /* yacc.c:1646  */
    { mon_drive_list((yyvsp[-1].i)); }
#line 3183 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 149:
#line 552 "mon_parse.y" /* yacc.c:1646  */
    { mon_attach((yyvsp[-2].str),(yyvsp[-1].i)); }
#line 3189 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 150:
#line 554 "mon_parse.y" /* yacc.c:1646  */
    { mon_detach((yyvsp[-1].i)); }
#line 3195 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 151:
#line 556 "mon_parse.y" /* yacc.c:1646  */
    { mon_autostart((yyvsp[-1].str),0,1); }
#line 3201 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 152:
#line 558 "mon_parse.y" /* yacc.c:1646  */
    { mon_autostart((yyvsp[-3].str),(yyvsp[-1].i),1); }
#line 3207 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 153:
#line 560 "mon_parse.y" /* yacc.c:1646  */
    { mon_autostart((yyvsp[-1].str),0,0); }
#line 3213 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 154:
#line 562 "mon_parse.y" /* yacc.c:1646  */
    { mon_autostart((yyvsp[-3].str),(yyvsp[-1].i),0); }
#line 3219 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 155:
#line 566 "mon_parse.y" /* yacc.c:1646  */
    { mon_record_commands((yyvsp[-1].str)); }
#line 3225 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 156:
#line 568 "mon_parse.y" /* yacc.c:1646  */
    { mon_end_recording(); }
#line 3231 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 157:
#line 570 "mon_parse.y" /* yacc.c:1646  */
    { mon_playback_init((yyvsp[-1].str)); }
#line 3237 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 158:
#line 574 "mon_parse.y" /* yacc.c:1646  */
    { mon_memory_fill((yyvsp[-2].a), BAD_ADDR, (unsigned char *)(yyvsp[-1].str)); }
#line 3243 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 159:
#line 576 "mon_parse.y" /* yacc.c:1646  */
    { printf("Not yet.\n"); }
#line 3249 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 160:
#line 580 "mon_parse.y" /* yacc.c:1646  */
    { yydebug = 1; }
#line 3255 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 161:
#line 583 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[0].str); }
#line 3261 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 162:
#line 586 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[0].str); }
#line 3267 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 163:
#line 587 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.str) = NULL; }
#line 3273 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 165:
#line 591 "mon_parse.y" /* yacc.c:1646  */
    { return ERR_EXPECT_FILENAME; }
#line 3279 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 167:
#line 595 "mon_parse.y" /* yacc.c:1646  */
    { return ERR_EXPECT_DEVICE_NUM; }
#line 3285 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 168:
#line 598 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[-1].i) | (yyvsp[0].i); }
#line 3291 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 169:
#line 599 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3297 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 170:
#line 602 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3303 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 171:
#line 603 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = 0; }
#line 3309 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 172:
#line 606 "mon_parse.y" /* yacc.c:1646  */
    {
                                    if (!mon_register_valid(default_memspace, (yyvsp[0].reg))) {
                                        return ERR_INVALID_REGISTER;
                                    }
                                    (yyval.i) = new_reg(default_memspace, (yyvsp[0].reg));
                                }
#line 3320 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 173:
#line 612 "mon_parse.y" /* yacc.c:1646  */
    {
                                    if (!mon_register_valid((yyvsp[-1].i), (yyvsp[0].reg))) {
                                        return ERR_INVALID_REGISTER;
                                    }
                                    (yyval.i) = new_reg((yyvsp[-1].i), (yyvsp[0].reg));
                                }
#line 3331 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 176:
#line 625 "mon_parse.y" /* yacc.c:1646  */
    { (monitor_cpu_for_memspace[reg_memspace((yyvsp[-2].i))]->mon_register_set_val)(reg_memspace((yyvsp[-2].i)), reg_regid((yyvsp[-2].i)), (WORD) (yyvsp[0].i)); }
#line 3337 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 177:
#line 628 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3343 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 178:
#line 629 "mon_parse.y" /* yacc.c:1646  */
    { return ERR_EXPECT_CHECKNUM; }
#line 3349 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 180:
#line 633 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.range)[0] = (yyvsp[0].a); (yyval.range)[1] = BAD_ADDR; }
#line 3355 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 181:
#line 636 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.range)[0] = (yyvsp[-2].a); (yyval.range)[1] = (yyvsp[0].a); }
#line 3361 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 182:
#line 638 "mon_parse.y" /* yacc.c:1646  */
    { if (resolve_range(e_default_space, (yyval.range), (yyvsp[0].str))) return ERR_ADDR_TOO_BIG; }
#line 3367 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 183:
#line 640 "mon_parse.y" /* yacc.c:1646  */
    { if (resolve_range((yyvsp[-2].i), (yyval.range), (yyvsp[0].str))) return ERR_ADDR_TOO_BIG; }
#line 3373 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 184:
#line 643 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.a) = (yyvsp[0].a); }
#line 3379 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 185:
#line 644 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.a) = BAD_ADDR; }
#line 3385 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 186:
#line 648 "mon_parse.y" /* yacc.c:1646  */
    {
             (yyval.a) = new_addr(e_default_space,(yyvsp[0].i));
             if (opt_asm) new_cmd = asm_mode = 1;
         }
#line 3394 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 187:
#line 653 "mon_parse.y" /* yacc.c:1646  */
    {
             (yyval.a) = new_addr((yyvsp[-2].i), (yyvsp[0].i));
             if (opt_asm) new_cmd = asm_mode = 1;
         }
#line 3403 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 188:
#line 658 "mon_parse.y" /* yacc.c:1646  */
    {
             temp = mon_symbol_table_lookup_addr(e_default_space, (yyvsp[0].str));
             if (temp >= 0)
                 (yyval.a) = new_addr(e_default_space, temp);
             else
                 return ERR_UNDEFINED_LABEL;
         }
#line 3415 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 191:
#line 671 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = e_comp_space; }
#line 3421 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 192:
#line 672 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = e_disk8_space; }
#line 3427 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 193:
#line 673 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = e_disk9_space; }
#line 3433 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 194:
#line 674 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = e_disk10_space; }
#line 3439 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 195:
#line 675 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = e_disk11_space; }
#line 3445 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 196:
#line 678 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); if (!CHECK_ADDR((yyvsp[0].i))) return ERR_ADDR_TOO_BIG; }
#line 3451 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 197:
#line 680 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3457 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 198:
#line 682 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[-2].i) + (yyvsp[0].i); }
#line 3463 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 199:
#line 683 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[-2].i) - (yyvsp[0].i); }
#line 3469 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 200:
#line 684 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[-2].i) * (yyvsp[0].i); }
#line 3475 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 201:
#line 685 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = ((yyvsp[0].i)) ? ((yyvsp[-2].i) / (yyvsp[0].i)) : 1; }
#line 3481 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 202:
#line 686 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[-1].i); }
#line 3487 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 203:
#line 687 "mon_parse.y" /* yacc.c:1646  */
    { return ERR_MISSING_CLOSE_PAREN; }
#line 3493 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 204:
#line 688 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3499 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 205:
#line 691 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.cond_node) = (yyvsp[0].cond_node); }
#line 3505 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 206:
#line 692 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.cond_node) = 0; }
#line 3511 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 207:
#line 695 "mon_parse.y" /* yacc.c:1646  */
    {
               (yyval.cond_node) = new_cond; (yyval.cond_node)->is_parenthized = FALSE;
               (yyval.cond_node)->child1 = (yyvsp[-2].cond_node); (yyval.cond_node)->child2 = (yyvsp[0].cond_node); (yyval.cond_node)->operation = (yyvsp[-1].cond_op);
           }
#line 3520 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 208:
#line 700 "mon_parse.y" /* yacc.c:1646  */
    { return ERR_INCOMPLETE_COMPARE_OP; }
#line 3526 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 209:
#line 702 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.cond_node) = (yyvsp[-1].cond_node); (yyval.cond_node)->is_parenthized = TRUE; }
#line 3532 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 210:
#line 704 "mon_parse.y" /* yacc.c:1646  */
    { return ERR_MISSING_CLOSE_PAREN; }
#line 3538 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 211:
#line 706 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.cond_node) = (yyvsp[0].cond_node); }
#line 3544 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 212:
#line 709 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->reg_num = (yyvsp[0].i); (yyval.cond_node)->is_reg = TRUE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
#line 3555 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 213:
#line 715 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.cond_node) = new_cond;
                            (yyval.cond_node)->operation = e_INV;
                            (yyval.cond_node)->is_parenthized = FALSE;
                            (yyval.cond_node)->value = (yyvsp[0].i); (yyval.cond_node)->is_reg = FALSE;
                            (yyval.cond_node)->child1 = NULL; (yyval.cond_node)->child2 = NULL;
                          }
#line 3566 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 216:
#line 727 "mon_parse.y" /* yacc.c:1646  */
    { mon_add_number_to_buffer((yyvsp[0].i)); }
#line 3572 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 217:
#line 728 "mon_parse.y" /* yacc.c:1646  */
    { mon_add_string_to_buffer((yyvsp[0].str)); }
#line 3578 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 220:
#line 735 "mon_parse.y" /* yacc.c:1646  */
    { mon_add_number_to_buffer((yyvsp[0].i)); }
#line 3584 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 221:
#line 736 "mon_parse.y" /* yacc.c:1646  */
    { mon_add_number_masked_to_buffer((yyvsp[0].i), 0x00); }
#line 3590 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 222:
#line 737 "mon_parse.y" /* yacc.c:1646  */
    { mon_add_string_to_buffer((yyvsp[0].str)); }
#line 3596 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 223:
#line 740 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3602 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 224:
#line 741 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (monitor_cpu_for_memspace[reg_memspace((yyvsp[0].i))]->mon_register_get_val)(reg_memspace((yyvsp[0].i)), reg_regid((yyvsp[0].i))); }
#line 3608 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 225:
#line 744 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3614 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 226:
#line 745 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = strtol((yyvsp[0].str), NULL, 10); }
#line 3620 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 227:
#line 746 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = strtol((yyvsp[0].str), NULL, 10); }
#line 3626 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 228:
#line 747 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = strtol((yyvsp[0].str), NULL, 10); }
#line 3632 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 229:
#line 750 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = resolve_datatype(B_NUMBER,(yyvsp[0].str)); }
#line 3638 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 230:
#line 751 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = resolve_datatype(O_NUMBER,(yyvsp[0].str)); }
#line 3644 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 231:
#line 752 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = resolve_datatype(D_NUMBER,(yyvsp[0].str)); }
#line 3650 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 232:
#line 755 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3656 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 233:
#line 756 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3662 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 234:
#line 757 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3668 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 235:
#line 758 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3674 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 236:
#line 759 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3680 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 240:
#line 767 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = 0;
                                                if ((yyvsp[-1].str)) {
                                                    (monitor_cpu_for_memspace[default_memspace]->mon_assemble_instr)((yyvsp[-1].str), (yyvsp[0].mode));
                                                } else {
                                                    new_cmd = 1;
                                                    asm_mode = 0;
                                                }
                                                opt_asm = 0;
                                              }
#line 3694 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 242:
#line 778 "mon_parse.y" /* yacc.c:1646  */
    { asm_mode = 0; }
#line 3700 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 243:
#line 781 "mon_parse.y" /* yacc.c:1646  */
    { if ((yyvsp[0].i) > 0xff) {
                          (yyval.mode).addr_mode = ASM_ADDR_MODE_IMMEDIATE_16;
                          (yyval.mode).param = (yyvsp[0].i);
                        } else {
                          (yyval.mode).addr_mode = ASM_ADDR_MODE_IMMEDIATE;
                          (yyval.mode).param = (yyvsp[0].i);
                        } }
#line 3712 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 244:
#line 788 "mon_parse.y" /* yacc.c:1646  */
    { if ((yyvsp[0].i) >= 0x10000) {
               (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_LONG;
               (yyval.mode).param = (yyvsp[0].i);
             } else if ((yyvsp[0].i) < 0x100) {
               (yyval.mode).addr_mode = ASM_ADDR_MODE_ZERO_PAGE;
               (yyval.mode).param = (yyvsp[0].i);
             } else {
               (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE;
               (yyval.mode).param = (yyvsp[0].i);
             }
           }
#line 3728 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 245:
#line 799 "mon_parse.y" /* yacc.c:1646  */
    { if ((yyvsp[-2].i) >= 0x10000) {
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_LONG_X;
                            (yyval.mode).param = (yyvsp[-2].i);
                          } else if ((yyvsp[-2].i) < 0x100) { 
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_ZERO_PAGE_X;
                            (yyval.mode).param = (yyvsp[-2].i);
                          } else {
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_X;
                            (yyval.mode).param = (yyvsp[-2].i);
                          }
                        }
#line 3744 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 246:
#line 810 "mon_parse.y" /* yacc.c:1646  */
    { if ((yyvsp[-2].i) < 0x100) {
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_ZERO_PAGE_Y;
                            (yyval.mode).param = (yyvsp[-2].i);
                          } else {
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_Y;
                            (yyval.mode).param = (yyvsp[-2].i);
                          }
                        }
#line 3757 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 247:
#line 818 "mon_parse.y" /* yacc.c:1646  */
    { if ((yyvsp[-2].i) < 0x100) {
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_STACK_RELATIVE;
                            (yyval.mode).param = (yyvsp[-2].i);
                          } else { /* 6809 */
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
                            if ((yyvsp[-2].i) >= -16 && (yyvsp[-2].i) < 16) {
                                (yyval.mode).addr_submode = (yyvsp[0].i) | ((yyvsp[-2].i) & 0x1F);
                            } else if ((yyvsp[-2].i) >= -128 && (yyvsp[-2].i) < 128) {
                                (yyval.mode).addr_submode = 0x80 | (yyvsp[0].i) | ASM_ADDR_MODE_INDEXED_OFF8;
                                (yyval.mode).param = (yyvsp[-2].i);
                            } else if ((yyvsp[-2].i) >= -32768 && (yyvsp[-2].i) < 32768) {
                                (yyval.mode).addr_submode = 0x80 | (yyvsp[0].i) | ASM_ADDR_MODE_INDEXED_OFF16;
                                (yyval.mode).param = (yyvsp[-2].i);
                            } else {
                                (yyval.mode).addr_mode = ASM_ADDR_MODE_ILLEGAL;
                                mon_out("offset too large even for 16 bits (signed)\n");
                            }
                          }
                        }
#line 3781 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 248:
#line 837 "mon_parse.y" /* yacc.c:1646  */
    { if ((yyvsp[-2].i) < 0x100) {
                            (yyval.mode).addr_mode = ASM_ADDR_MODE_DOUBLE;
                            (yyval.mode).param = (yyvsp[0].i);
                            (yyval.mode).addr_submode = (yyvsp[-2].i);
                          }
                        }
#line 3792 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 249:
#line 843 "mon_parse.y" /* yacc.c:1646  */
    { if ((yyvsp[-1].i) < 0x100) {
                               (yyval.mode).addr_mode = ASM_ADDR_MODE_INDIRECT;
                               (yyval.mode).param = (yyvsp[-1].i);
                             } else {
                               (yyval.mode).addr_mode = ASM_ADDR_MODE_ABS_INDIRECT;
                               (yyval.mode).param = (yyvsp[-1].i);
                             }
                           }
#line 3805 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 250:
#line 851 "mon_parse.y" /* yacc.c:1646  */
    { if ((yyvsp[-3].i) < 0x100) {
                                           (yyval.mode).addr_mode = ASM_ADDR_MODE_INDIRECT_X;
                                           (yyval.mode).param = (yyvsp[-3].i);
                                         } else {
                                           (yyval.mode).addr_mode = ASM_ADDR_MODE_ABS_INDIRECT_X;
                                           (yyval.mode).param = (yyvsp[-3].i);
                                         }
                                       }
#line 3818 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 251:
#line 860 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_STACK_RELATIVE_Y; (yyval.mode).param = (yyvsp[-5].i); }
#line 3824 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 252:
#line 862 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_INDIRECT_Y; (yyval.mode).param = (yyvsp[-3].i); }
#line 3830 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 253:
#line 863 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_BC; }
#line 3836 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 254:
#line 864 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_DE; }
#line 3842 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 255:
#line 865 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_HL; }
#line 3848 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 256:
#line 866 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_IX; }
#line 3854 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 257:
#line 867 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_IY; }
#line 3860 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 258:
#line 868 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IND_SP; }
#line 3866 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 259:
#line 870 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_A; (yyval.mode).param = (yyvsp[-3].i); }
#line 3872 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 260:
#line 872 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_HL; (yyval.mode).param = (yyvsp[-3].i); }
#line 3878 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 261:
#line 874 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_IX; (yyval.mode).param = (yyvsp[-3].i); }
#line 3884 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 262:
#line 876 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_ABSOLUTE_IY; (yyval.mode).param = (yyvsp[-3].i); }
#line 3890 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 263:
#line 877 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_IMPLIED; }
#line 3896 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 264:
#line 878 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_ACCUMULATOR; }
#line 3902 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 265:
#line 879 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_B; }
#line 3908 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 266:
#line 880 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_C; }
#line 3914 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 267:
#line 881 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_D; }
#line 3920 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 268:
#line 882 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_E; }
#line 3926 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 269:
#line 883 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_H; }
#line 3932 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 270:
#line 884 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IXH; }
#line 3938 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 271:
#line 885 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IYH; }
#line 3944 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 272:
#line 886 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_L; }
#line 3950 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 273:
#line 887 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IXL; }
#line 3956 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 274:
#line 888 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IYL; }
#line 3962 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 275:
#line 889 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_AF; }
#line 3968 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 276:
#line 890 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_BC; }
#line 3974 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 277:
#line 891 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_DE; }
#line 3980 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 278:
#line 892 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_HL; }
#line 3986 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 279:
#line 893 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IX; }
#line 3992 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 280:
#line 894 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_IY; }
#line 3998 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 281:
#line 895 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_REG_SP; }
#line 4004 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 282:
#line 897 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.mode).addr_mode = ASM_ADDR_MODE_DIRECT; (yyval.mode).param = (yyvsp[0].i); }
#line 4010 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 283:
#line 898 "mon_parse.y" /* yacc.c:1646  */
    {    /* Clash with addr,x addr,y addr,s modes! */
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        if ((yyvsp[-2].i) >= -16 && (yyvsp[-2].i) < 16) {
            (yyval.mode).addr_submode = (3 << 5) | ((yyvsp[-2].i) & 0x1F);
        } else if ((yyvsp[-2].i) >= -128 && (yyvsp[-2].i) < 128) {
            (yyval.mode).addr_submode = 0x80 | (3 << 5) | ASM_ADDR_MODE_INDEXED_OFF8;
            (yyval.mode).param = (yyvsp[-2].i);
        } else if ((yyvsp[-2].i) >= -32768 && (yyvsp[-2].i) < 32768) {
            (yyval.mode).addr_submode = 0x80 | (3 << 5) | ASM_ADDR_MODE_INDEXED_OFF16;
            (yyval.mode).param = (yyvsp[-2].i);
        } else {
            (yyval.mode).addr_mode = ASM_ADDR_MODE_ILLEGAL;
            mon_out("offset too large even for 16 bits (signed)\n");
        }
    }
#line 4030 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 284:
#line 913 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[-1].i) | ASM_ADDR_MODE_INDEXED_INC1;
        }
#line 4039 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 285:
#line 917 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[-2].i) | ASM_ADDR_MODE_INDEXED_INC2;
        }
#line 4048 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 286:
#line 921 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[0].i) | ASM_ADDR_MODE_INDEXED_DEC1;
        }
#line 4057 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 287:
#line 925 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[0].i) | ASM_ADDR_MODE_INDEXED_DEC2;
        }
#line 4066 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 288:
#line 929 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[0].i) | ASM_ADDR_MODE_INDEXED_OFF0;
        }
#line 4075 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 289:
#line 933 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[-1].i) | ASM_ADDR_MODE_INDEXED_OFFB;
        }
#line 4084 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 290:
#line 937 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[-1].i) | ASM_ADDR_MODE_INDEXED_OFFA;
        }
#line 4093 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 291:
#line 941 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[-1].i) | ASM_ADDR_MODE_INDEXED_OFFD;
        }
#line 4102 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 292:
#line 945 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).param = (yyvsp[-2].i);
        if ((yyvsp[-2].i) >= -128 && (yyvsp[-2].i) < 128) {
            (yyval.mode).addr_submode = ASM_ADDR_MODE_INDEXED_OFFPC8;
        } else if ((yyvsp[-2].i) >= -32768 && (yyvsp[-2].i) < 32768) {
            (yyval.mode).addr_submode = ASM_ADDR_MODE_INDEXED_OFFPC16;
        } else {
            (yyval.mode).addr_mode = ASM_ADDR_MODE_ILLEGAL;
            mon_out("offset too large even for 16 bits (signed)\n");
        }
    }
#line 4119 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 293:
#line 957 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        if ((yyvsp[-3].i) >= -16 && (yyvsp[-3].i) < 16) {
            (yyval.mode).addr_submode = (yyvsp[-3].i) & 0x1F;
        } else if ((yyvsp[-4].i) >= -128 && (yyvsp[-4].i) < 128) {
            (yyval.mode).addr_submode = ASM_ADDR_MODE_INDEXED_OFF8;
            (yyval.mode).param = (yyvsp[-3].i);
        } else if ((yyvsp[-3].i) >= -32768 && (yyvsp[-3].i) < 32768) {
            (yyval.mode).addr_submode = ASM_ADDR_MODE_INDEXED_OFF16;
            (yyval.mode).param = (yyvsp[-3].i);
        } else {
            (yyval.mode).addr_mode = ASM_ADDR_MODE_ILLEGAL;
            mon_out("offset too large even for 16 bits (signed)\n");
        }
    }
#line 4139 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 294:
#line 972 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[-2].i) | ASM_ADDR_MODE_INDEXED_INC1;
        }
#line 4148 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 295:
#line 976 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[-3].i) | ASM_ADDR_MODE_INDEXED_INC2;
        }
#line 4157 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 296:
#line 980 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[-1].i) | ASM_ADDR_MODE_INDEXED_DEC1;
        }
#line 4166 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 297:
#line 984 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[-1].i) | ASM_ADDR_MODE_INDEXED_DEC2;
        }
#line 4175 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 298:
#line 988 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[-1].i) | ASM_ADDR_MODE_INDEXED_OFF0;
        }
#line 4184 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 299:
#line 992 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[-2].i) | ASM_ADDR_MODE_INDEXED_OFFB;
        }
#line 4193 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 300:
#line 996 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[-2].i) | ASM_ADDR_MODE_INDEXED_OFFA;
        }
#line 4202 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 301:
#line 1000 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | (yyvsp[-2].i) | ASM_ADDR_MODE_INDEXED_OFFD;
        }
#line 4211 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 302:
#line 1004 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).param = (yyvsp[-3].i);
        if ((yyvsp[-3].i) >= -128 && (yyvsp[-3].i) < 128) {
            (yyval.mode).addr_submode = ASM_ADDR_MODE_INDEXED_OFFPC8_IND;
        } else if ((yyvsp[-3].i) >= -32768 && (yyvsp[-3].i) < 32768) {
            (yyval.mode).addr_submode = ASM_ADDR_MODE_INDEXED_OFFPC16_IND;
        } else {
            (yyval.mode).addr_mode = ASM_ADDR_MODE_ILLEGAL;
            mon_out("offset too large even for 16 bits (signed)\n");
        }
    }
#line 4228 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 303:
#line 1016 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDEXED;
        (yyval.mode).addr_submode = 0x80 | ASM_ADDR_MODE_EXTENDED_INDIRECT;
        (yyval.mode).param = (yyvsp[-1].i);
        }
#line 4238 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 304:
#line 1021 "mon_parse.y" /* yacc.c:1646  */
    {
        (yyval.mode).addr_mode = ASM_ADDR_MODE_INDIRECT_LONG_Y;
        (yyval.mode).param = (yyvsp[-3].i);
        }
#line 4247 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 305:
#line 1029 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (0 << 5); printf("reg_x\n"); }
#line 4253 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 306:
#line 1030 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (1 << 5); printf("reg_y\n"); }
#line 4259 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 307:
#line 1031 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 4265 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 308:
#line 1032 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (3 << 5); printf("reg_s\n"); }
#line 4271 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 309:
#line 1036 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (2 << 5); printf("reg_u\n"); }
#line 4277 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 310:
#line 1040 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 4283 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 311:
#line 1041 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.i) = 0; }
#line 4289 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 312:
#line 1045 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.ldt) = (yyvsp[0].ldt); }
#line 4295 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 313:
#line 1046 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.ldt) = e_label_code; }
#line 4301 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 314:
#line 1050 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.str) = (yyvsp[0].str); }
#line 4307 "mon_parse.c" /* yacc.c:1646  */
    break;

  case 315:
#line 1051 "mon_parse.y" /* yacc.c:1646  */
    { (yyval.str) = NULL; }
#line 4313 "mon_parse.c" /* yacc.c:1646  */
    break;


#line 4317 "mon_parse.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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

#if !defined yyoverflow || YYERROR_VERBOSE
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
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 1055 "mon_parse.y" /* yacc.c:1906  */


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


