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
     CMD_SEP = 274,
     REG_ASGN_SEP = 275,
     EQUALS = 276,
     CMD_SIDEFX = 277,
     CMD_RETURN = 278,
     CMD_BLOCK_READ = 279,
     CMD_BLOCK_WRITE = 280,
     CMD_UP = 281,
     CMD_DOWN = 282,
     CMD_LOAD = 283,
     CMD_SAVE = 284,
     CMD_VERIFY = 285,
     CMD_IGNORE = 286,
     CMD_HUNT = 287,
     CMD_FILL = 288,
     CMD_MOVE = 289,
     CMD_GOTO = 290,
     CMD_REGISTERS = 291,
     CMD_READSPACE = 292,
     CMD_WRITESPACE = 293,
     CMD_RADIX = 294,
     CMD_MEM_DISPLAY = 295,
     CMD_BREAK = 296,
     CMD_TRACE = 297,
     CMD_IO = 298,
     CMD_BRMON = 299,
     CMD_COMPARE = 300,
     CMD_DUMP = 301,
     CMD_UNDUMP = 302,
     CMD_EXIT = 303,
     CMD_DELETE = 304,
     CMD_CONDITION = 305,
     CMD_COMMAND = 306,
     CMD_ASSEMBLE = 307,
     CMD_DISASSEMBLE = 308,
     CMD_NEXT = 309,
     CMD_STEP = 310,
     CMD_PRINT = 311,
     CMD_DEVICE = 312,
     CMD_HELP = 313,
     CMD_WATCH = 314,
     CMD_DISK = 315,
     CMD_SYSTEM = 316,
     CMD_QUIT = 317,
     CMD_CHDIR = 318,
     CMD_BANK = 319,
     CMD_LOAD_LABELS = 320,
     CMD_SAVE_LABELS = 321,
     CMD_ADD_LABEL = 322,
     CMD_DEL_LABEL = 323,
     CMD_SHOW_LABELS = 324,
     CMD_RECORD = 325,
     CMD_STOP = 326,
     CMD_PLAYBACK = 327,
     CMD_CHAR_DISPLAY = 328,
     CMD_SPRITE_DISPLAY = 329,
     CMD_TEXT_DISPLAY = 330,
     CMD_ENTER_DATA = 331,
     CMD_ENTER_BIN_DATA = 332,
     CMD_KEYBUF = 333,
     CMD_BLOAD = 334,
     CMD_BSAVE = 335,
     CMD_SCREEN = 336,
     CMD_UNTIL = 337,
     CMD_CPU = 338,
     L_PAREN = 339,
     R_PAREN = 340,
     ARG_IMMEDIATE = 341,
     REG_A = 342,
     REG_X = 343,
     REG_Y = 344,
     COMMA = 345,
     INST_SEP = 346,
     REG_B = 347,
     REG_C = 348,
     REG_D = 349,
     REG_E = 350,
     REG_H = 351,
     REG_L = 352,
     REG_AF = 353,
     REG_BC = 354,
     REG_DE = 355,
     REG_HL = 356,
     REG_IX = 357,
     REG_IY = 358,
     REG_SP = 359,
     REG_IXH = 360,
     REG_IXL = 361,
     REG_IYH = 362,
     REG_IYL = 363,
     CPUTYPE_6502 = 364,
     CPUTYPE_Z80 = 365,
     STRING = 366,
     FILENAME = 367,
     R_O_L = 368,
     OPCODE = 369,
     LABEL = 370,
     BANKNAME = 371,
     CPUTYPE = 372,
     REGISTER = 373,
     COMPARE_OP = 374,
     RADIX_TYPE = 375,
     INPUT_SPEC = 376,
     CMD_CHECKPT_ON = 377,
     CMD_CHECKPT_OFF = 378,
     TOGGLE = 379
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
#define CMD_SEP 274
#define REG_ASGN_SEP 275
#define EQUALS 276
#define CMD_SIDEFX 277
#define CMD_RETURN 278
#define CMD_BLOCK_READ 279
#define CMD_BLOCK_WRITE 280
#define CMD_UP 281
#define CMD_DOWN 282
#define CMD_LOAD 283
#define CMD_SAVE 284
#define CMD_VERIFY 285
#define CMD_IGNORE 286
#define CMD_HUNT 287
#define CMD_FILL 288
#define CMD_MOVE 289
#define CMD_GOTO 290
#define CMD_REGISTERS 291
#define CMD_READSPACE 292
#define CMD_WRITESPACE 293
#define CMD_RADIX 294
#define CMD_MEM_DISPLAY 295
#define CMD_BREAK 296
#define CMD_TRACE 297
#define CMD_IO 298
#define CMD_BRMON 299
#define CMD_COMPARE 300
#define CMD_DUMP 301
#define CMD_UNDUMP 302
#define CMD_EXIT 303
#define CMD_DELETE 304
#define CMD_CONDITION 305
#define CMD_COMMAND 306
#define CMD_ASSEMBLE 307
#define CMD_DISASSEMBLE 308
#define CMD_NEXT 309
#define CMD_STEP 310
#define CMD_PRINT 311
#define CMD_DEVICE 312
#define CMD_HELP 313
#define CMD_WATCH 314
#define CMD_DISK 315
#define CMD_SYSTEM 316
#define CMD_QUIT 317
#define CMD_CHDIR 318
#define CMD_BANK 319
#define CMD_LOAD_LABELS 320
#define CMD_SAVE_LABELS 321
#define CMD_ADD_LABEL 322
#define CMD_DEL_LABEL 323
#define CMD_SHOW_LABELS 324
#define CMD_RECORD 325
#define CMD_STOP 326
#define CMD_PLAYBACK 327
#define CMD_CHAR_DISPLAY 328
#define CMD_SPRITE_DISPLAY 329
#define CMD_TEXT_DISPLAY 330
#define CMD_ENTER_DATA 331
#define CMD_ENTER_BIN_DATA 332
#define CMD_KEYBUF 333
#define CMD_BLOAD 334
#define CMD_BSAVE 335
#define CMD_SCREEN 336
#define CMD_UNTIL 337
#define CMD_CPU 338
#define L_PAREN 339
#define R_PAREN 340
#define ARG_IMMEDIATE 341
#define REG_A 342
#define REG_X 343
#define REG_Y 344
#define COMMA 345
#define INST_SEP 346
#define REG_B 347
#define REG_C 348
#define REG_D 349
#define REG_E 350
#define REG_H 351
#define REG_L 352
#define REG_AF 353
#define REG_BC 354
#define REG_DE 355
#define REG_HL 356
#define REG_IX 357
#define REG_IY 358
#define REG_SP 359
#define REG_IXH 360
#define REG_IXL 361
#define REG_IYH 362
#define REG_IYL 363
#define CPUTYPE_6502 364
#define CPUTYPE_Z80 365
#define STRING 366
#define FILENAME 367
#define R_O_L 368
#define OPCODE 369
#define LABEL 370
#define BANKNAME 371
#define CPUTYPE 372
#define REGISTER 373
#define COMPARE_OP 374
#define RADIX_TYPE 375
#define INPUT_SPEC 376
#define CMD_CHECKPT_ON 377
#define CMD_CHECKPT_OFF 378
#define TOGGLE 379




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
#line 299 "y.tab.h"
# define YYSTYPE yystype
#endif

extern YYSTYPE yylval;


#endif /* not BISON_MON_PARSE_H */

