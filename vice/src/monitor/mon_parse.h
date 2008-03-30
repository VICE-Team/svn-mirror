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

#ifndef BISON_MON_PARSE_H
# define BISON_MON_PARSE_H

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
     MEM_DISK10 = 274,
     MEM_DISK11 = 275,
     CMD_SEP = 276,
     REG_ASGN_SEP = 277,
     EQUALS = 278,
     CMD_SIDEFX = 279,
     CMD_RETURN = 280,
     CMD_BLOCK_READ = 281,
     CMD_BLOCK_WRITE = 282,
     CMD_UP = 283,
     CMD_DOWN = 284,
     CMD_LOAD = 285,
     CMD_SAVE = 286,
     CMD_VERIFY = 287,
     CMD_IGNORE = 288,
     CMD_HUNT = 289,
     CMD_FILL = 290,
     CMD_MOVE = 291,
     CMD_GOTO = 292,
     CMD_REGISTERS = 293,
     CMD_READSPACE = 294,
     CMD_WRITESPACE = 295,
     CMD_RADIX = 296,
     CMD_MEM_DISPLAY = 297,
     CMD_BREAK = 298,
     CMD_TRACE = 299,
     CMD_IO = 300,
     CMD_BRMON = 301,
     CMD_COMPARE = 302,
     CMD_DUMP = 303,
     CMD_UNDUMP = 304,
     CMD_EXIT = 305,
     CMD_DELETE = 306,
     CMD_CONDITION = 307,
     CMD_COMMAND = 308,
     CMD_ASSEMBLE = 309,
     CMD_DISASSEMBLE = 310,
     CMD_NEXT = 311,
     CMD_STEP = 312,
     CMD_PRINT = 313,
     CMD_DEVICE = 314,
     CMD_HELP = 315,
     CMD_WATCH = 316,
     CMD_DISK = 317,
     CMD_SYSTEM = 318,
     CMD_QUIT = 319,
     CMD_CHDIR = 320,
     CMD_BANK = 321,
     CMD_LOAD_LABELS = 322,
     CMD_SAVE_LABELS = 323,
     CMD_ADD_LABEL = 324,
     CMD_DEL_LABEL = 325,
     CMD_SHOW_LABELS = 326,
     CMD_RECORD = 327,
     CMD_STOP = 328,
     CMD_PLAYBACK = 329,
     CMD_CHAR_DISPLAY = 330,
     CMD_SPRITE_DISPLAY = 331,
     CMD_TEXT_DISPLAY = 332,
     CMD_ENTER_DATA = 333,
     CMD_ENTER_BIN_DATA = 334,
     CMD_KEYBUF = 335,
     CMD_BLOAD = 336,
     CMD_BSAVE = 337,
     CMD_SCREEN = 338,
     CMD_UNTIL = 339,
     CMD_CPU = 340,
     L_PAREN = 341,
     R_PAREN = 342,
     ARG_IMMEDIATE = 343,
     REG_A = 344,
     REG_X = 345,
     REG_Y = 346,
     COMMA = 347,
     INST_SEP = 348,
     REG_B = 349,
     REG_C = 350,
     REG_D = 351,
     REG_E = 352,
     REG_H = 353,
     REG_L = 354,
     REG_AF = 355,
     REG_BC = 356,
     REG_DE = 357,
     REG_HL = 358,
     REG_IX = 359,
     REG_IY = 360,
     REG_SP = 361,
     REG_IXH = 362,
     REG_IXL = 363,
     REG_IYH = 364,
     REG_IYL = 365,
     CPUTYPE_6502 = 366,
     CPUTYPE_Z80 = 367,
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
#define MEM_DISK10 274
#define MEM_DISK11 275
#define CMD_SEP 276
#define REG_ASGN_SEP 277
#define EQUALS 278
#define CMD_SIDEFX 279
#define CMD_RETURN 280
#define CMD_BLOCK_READ 281
#define CMD_BLOCK_WRITE 282
#define CMD_UP 283
#define CMD_DOWN 284
#define CMD_LOAD 285
#define CMD_SAVE 286
#define CMD_VERIFY 287
#define CMD_IGNORE 288
#define CMD_HUNT 289
#define CMD_FILL 290
#define CMD_MOVE 291
#define CMD_GOTO 292
#define CMD_REGISTERS 293
#define CMD_READSPACE 294
#define CMD_WRITESPACE 295
#define CMD_RADIX 296
#define CMD_MEM_DISPLAY 297
#define CMD_BREAK 298
#define CMD_TRACE 299
#define CMD_IO 300
#define CMD_BRMON 301
#define CMD_COMPARE 302
#define CMD_DUMP 303
#define CMD_UNDUMP 304
#define CMD_EXIT 305
#define CMD_DELETE 306
#define CMD_CONDITION 307
#define CMD_COMMAND 308
#define CMD_ASSEMBLE 309
#define CMD_DISASSEMBLE 310
#define CMD_NEXT 311
#define CMD_STEP 312
#define CMD_PRINT 313
#define CMD_DEVICE 314
#define CMD_HELP 315
#define CMD_WATCH 316
#define CMD_DISK 317
#define CMD_SYSTEM 318
#define CMD_QUIT 319
#define CMD_CHDIR 320
#define CMD_BANK 321
#define CMD_LOAD_LABELS 322
#define CMD_SAVE_LABELS 323
#define CMD_ADD_LABEL 324
#define CMD_DEL_LABEL 325
#define CMD_SHOW_LABELS 326
#define CMD_RECORD 327
#define CMD_STOP 328
#define CMD_PLAYBACK 329
#define CMD_CHAR_DISPLAY 330
#define CMD_SPRITE_DISPLAY 331
#define CMD_TEXT_DISPLAY 332
#define CMD_ENTER_DATA 333
#define CMD_ENTER_BIN_DATA 334
#define CMD_KEYBUF 335
#define CMD_BLOAD 336
#define CMD_BSAVE 337
#define CMD_SCREEN 338
#define CMD_UNTIL 339
#define CMD_CPU 340
#define L_PAREN 341
#define R_PAREN 342
#define ARG_IMMEDIATE 343
#define REG_A 344
#define REG_X 345
#define REG_Y 346
#define COMMA 347
#define INST_SEP 348
#define REG_B 349
#define REG_C 350
#define REG_D 351
#define REG_E 352
#define REG_H 353
#define REG_L 354
#define REG_AF 355
#define REG_BC 356
#define REG_DE 357
#define REG_HL 358
#define REG_IX 359
#define REG_IY 360
#define REG_SP 361
#define REG_IXH 362
#define REG_IXL 363
#define REG_IYH 364
#define REG_IYL 365
#define CPUTYPE_6502 366
#define CPUTYPE_Z80 367
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
/* Line 1281 of /usr/share/bison/yacc.c.  */
#line 303 "y.tab.h"
# define YYSTYPE yystype
#endif

extern YYSTYPE yylval;


#endif /* not BISON_MON_PARSE_H */

