#ifndef YY_parse_h_included
#define YY_parse_h_included
/*#define YY_USE_CLASS 
*/
#line 1 "/usr/share/bison++/bison.h"
/* before anything */
#ifdef c_plusplus
 #ifndef __cplusplus
  #define __cplusplus
 #endif
#endif


 #line 8 "/usr/share/bison++/bison.h"

#line 127 "mon_parse.y"
typedef union {
    MON_ADDR a;
    MON_ADDR range[2];
    int i;
    REG_ID reg;
    CONDITIONAL cond_op;
    cond_node_t *cond_node;
    RADIXTYPE rt;
    ACTION action;
    char *str;
} yy_parse_stype;
#define YY_parse_STYPE yy_parse_stype
#ifndef YY_USE_CLASS
#define YYSTYPE yy_parse_stype
#endif

#line 21 "/usr/share/bison++/bison.h"
 /* %{ and %header{ and %union, during decl */
#ifndef YY_parse_COMPATIBILITY
 #ifndef YY_USE_CLASS
  #define  YY_parse_COMPATIBILITY 1
 #else
  #define  YY_parse_COMPATIBILITY 0
 #endif
#endif

#if YY_parse_COMPATIBILITY != 0
/* backward compatibility */
 #ifdef YYLTYPE
  #ifndef YY_parse_LTYPE
   #define YY_parse_LTYPE YYLTYPE
/* WARNING obsolete !!! user defined YYLTYPE not reported into generated header */
/* use %define LTYPE */
  #endif
 #endif
/*#ifdef YYSTYPE*/
  #ifndef YY_parse_STYPE
   #define YY_parse_STYPE YYSTYPE
  /* WARNING obsolete !!! user defined YYSTYPE not reported into generated header */
   /* use %define STYPE */
  #endif
/*#endif*/
 #ifdef YYDEBUG
  #ifndef YY_parse_DEBUG
   #define  YY_parse_DEBUG YYDEBUG
   /* WARNING obsolete !!! user defined YYDEBUG not reported into generated header */
   /* use %define DEBUG */
  #endif
 #endif 
 /* use goto to be compatible */
 #ifndef YY_parse_USE_GOTO
  #define YY_parse_USE_GOTO 1
 #endif
#endif

/* use no goto to be clean in C++ */
#ifndef YY_parse_USE_GOTO
 #define YY_parse_USE_GOTO 0
#endif

#ifndef YY_parse_PURE

 #line 65 "/usr/share/bison++/bison.h"

#line 65 "/usr/share/bison++/bison.h"
/* YY_parse_PURE */
#endif


 #line 68 "/usr/share/bison++/bison.h"

#line 68 "/usr/share/bison++/bison.h"
/* prefix */

#ifndef YY_parse_DEBUG

 #line 71 "/usr/share/bison++/bison.h"
#define YY_parse_DEBUG 1

#line 71 "/usr/share/bison++/bison.h"
/* YY_parse_DEBUG */
#endif

#ifndef YY_parse_LSP_NEEDED

 #line 75 "/usr/share/bison++/bison.h"

#line 75 "/usr/share/bison++/bison.h"
 /* YY_parse_LSP_NEEDED*/
#endif

/* DEFAULT LTYPE*/
#ifdef YY_parse_LSP_NEEDED
 #ifndef YY_parse_LTYPE
  #ifndef BISON_YYLTYPE_ISDECLARED
   #define BISON_YYLTYPE_ISDECLARED
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;
  #endif

  #define YY_parse_LTYPE yyltype
 #endif
#endif

/* DEFAULT STYPE*/
#ifndef YY_parse_STYPE
 #define YY_parse_STYPE int
#endif

/* DEFAULT MISCELANEOUS */
#ifndef YY_parse_PARSE
 #define YY_parse_PARSE yyparse
#endif

#ifndef YY_parse_LEX
 #define YY_parse_LEX yylex
#endif

#ifndef YY_parse_LVAL
 #define YY_parse_LVAL yylval
#endif

#ifndef YY_parse_LLOC
 #define YY_parse_LLOC yylloc
#endif

#ifndef YY_parse_CHAR
 #define YY_parse_CHAR yychar
#endif

#ifndef YY_parse_NERRS
 #define YY_parse_NERRS yynerrs
#endif

#ifndef YY_parse_DEBUG_FLAG
 #define YY_parse_DEBUG_FLAG yydebug
#endif

#ifndef YY_parse_ERROR
 #define YY_parse_ERROR yyerror
#endif

#ifndef YY_parse_PARSE_PARAM
 #ifndef __STDC__
  #ifndef __cplusplus
   #ifndef YY_USE_CLASS
    #define YY_parse_PARSE_PARAM
    #ifndef YY_parse_PARSE_PARAM_DEF
     #define YY_parse_PARSE_PARAM_DEF
    #endif
   #endif
  #endif
 #endif
 #ifndef YY_parse_PARSE_PARAM
  #define YY_parse_PARSE_PARAM void
 #endif
#endif

/* TOKEN C */
#ifndef YY_USE_CLASS

 #ifndef YY_parse_PURE
  #ifndef yylval
   extern YY_parse_STYPE YY_parse_LVAL;
  #else
   #if yylval != YY_parse_LVAL
    extern YY_parse_STYPE YY_parse_LVAL;
   #else
    #warning "Namespace conflict, disabling some functionality (bison++ only)"
   #endif
  #endif
 #endif


 #line 169 "/usr/share/bison++/bison.h"
#define	H_NUMBER	258
#define	D_NUMBER	259
#define	O_NUMBER	260
#define	B_NUMBER	261
#define	CONVERT_OP	262
#define	B_DATA	263
#define	H_RANGE_GUESS	264
#define	D_NUMBER_GUESS	265
#define	O_NUMBER_GUESS	266
#define	B_NUMBER_GUESS	267
#define	BAD_CMD	268
#define	MEM_OP	269
#define	IF	270
#define	MEM_COMP	271
#define	MEM_DISK8	272
#define	MEM_DISK9	273
#define	MEM_DISK10	274
#define	MEM_DISK11	275
#define	EQUALS	276
#define	TRAIL	277
#define	CMD_SEP	278
#define	LABEL_ASGN_COMMENT	279
#define	CMD_SIDEFX	280
#define	CMD_RETURN	281
#define	CMD_BLOCK_READ	282
#define	CMD_BLOCK_WRITE	283
#define	CMD_UP	284
#define	CMD_DOWN	285
#define	CMD_LOAD	286
#define	CMD_SAVE	287
#define	CMD_VERIFY	288
#define	CMD_IGNORE	289
#define	CMD_HUNT	290
#define	CMD_FILL	291
#define	CMD_MOVE	292
#define	CMD_GOTO	293
#define	CMD_REGISTERS	294
#define	CMD_READSPACE	295
#define	CMD_WRITESPACE	296
#define	CMD_RADIX	297
#define	CMD_MEM_DISPLAY	298
#define	CMD_BREAK	299
#define	CMD_TRACE	300
#define	CMD_IO	301
#define	CMD_BRMON	302
#define	CMD_COMPARE	303
#define	CMD_DUMP	304
#define	CMD_UNDUMP	305
#define	CMD_EXIT	306
#define	CMD_DELETE	307
#define	CMD_CONDITION	308
#define	CMD_COMMAND	309
#define	CMD_ASSEMBLE	310
#define	CMD_DISASSEMBLE	311
#define	CMD_NEXT	312
#define	CMD_STEP	313
#define	CMD_PRINT	314
#define	CMD_DEVICE	315
#define	CMD_HELP	316
#define	CMD_WATCH	317
#define	CMD_DISK	318
#define	CMD_SYSTEM	319
#define	CMD_QUIT	320
#define	CMD_CHDIR	321
#define	CMD_BANK	322
#define	CMD_LOAD_LABELS	323
#define	CMD_SAVE_LABELS	324
#define	CMD_ADD_LABEL	325
#define	CMD_DEL_LABEL	326
#define	CMD_SHOW_LABELS	327
#define	CMD_RECORD	328
#define	CMD_MON_STOP	329
#define	CMD_PLAYBACK	330
#define	CMD_CHAR_DISPLAY	331
#define	CMD_SPRITE_DISPLAY	332
#define	CMD_TEXT_DISPLAY	333
#define	CMD_SCREENCODE_DISPLAY	334
#define	CMD_ENTER_DATA	335
#define	CMD_ENTER_BIN_DATA	336
#define	CMD_KEYBUF	337
#define	CMD_BLOAD	338
#define	CMD_BSAVE	339
#define	CMD_SCREEN	340
#define	CMD_UNTIL	341
#define	CMD_CPU	342
#define	CMD_YYDEBUG	343
#define	CMD_BACKTRACE	344
#define	CMD_SCREENSHOT	345
#define	CMD_PWD	346
#define	CMD_DIR	347
#define	CMD_RESOURCE_GET	348
#define	CMD_RESOURCE_SET	349
#define	CMD_ATTACH	350
#define	CMD_DETACH	351
#define	CMD_MON_RESET	352
#define	CMD_TAPECTRL	353
#define	CMD_CARTFREEZE	354
#define	CMD_CPUHISTORY	355
#define	CMD_MEMMAPZAP	356
#define	CMD_MEMMAPSHOW	357
#define	CMD_MEMMAPSAVE	358
#define	CMD_COMMENT	359
#define	CMD_LIST	360
#define	CMD_EXPORT	361
#define	CMD_AUTOSTART	362
#define	CMD_AUTOLOAD	363
#define	CMD_LABEL_ASGN	364
#define	L_PAREN	365
#define	R_PAREN	366
#define	ARG_IMMEDIATE	367
#define	REG_A	368
#define	REG_X	369
#define	REG_Y	370
#define	COMMA	371
#define	INST_SEP	372
#define	REG_B	373
#define	REG_C	374
#define	REG_D	375
#define	REG_E	376
#define	REG_H	377
#define	REG_L	378
#define	REG_AF	379
#define	REG_BC	380
#define	REG_DE	381
#define	REG_HL	382
#define	REG_IX	383
#define	REG_IY	384
#define	REG_SP	385
#define	REG_IXH	386
#define	REG_IXL	387
#define	REG_IYH	388
#define	REG_IYL	389
#define	STRING	390
#define	FILENAME	391
#define	R_O_L	392
#define	OPCODE	393
#define	LABEL	394
#define	BANKNAME	395
#define	CPUTYPE	396
#define	MON_REGISTER	397
#define	COMPARE_OP	398
#define	RADIX_TYPE	399
#define	INPUT_SPEC	400
#define	CMD_CHECKPT_ON	401
#define	CMD_CHECKPT_OFF	402
#define	TOGGLE	403
#define	MASK	404


#line 169 "/usr/share/bison++/bison.h"
 /* #defines token */
/* after #define tokens, before const tokens S5*/
#else
 #ifndef YY_parse_CLASS
  #define YY_parse_CLASS parse
 #endif

 #ifndef YY_parse_INHERIT
  #define YY_parse_INHERIT
 #endif

 #ifndef YY_parse_MEMBERS
  #define YY_parse_MEMBERS 
 #endif

 #ifndef YY_parse_LEX_BODY
  #define YY_parse_LEX_BODY  
 #endif

 #ifndef YY_parse_ERROR_BODY
  #define YY_parse_ERROR_BODY  
 #endif

 #ifndef YY_parse_CONSTRUCTOR_PARAM
  #define YY_parse_CONSTRUCTOR_PARAM
 #endif
 /* choose between enum and const */
 #ifndef YY_parse_USE_CONST_TOKEN
  #define YY_parse_USE_CONST_TOKEN 0
  /* yes enum is more compatible with flex,  */
  /* so by default we use it */ 
 #endif
 #if YY_parse_USE_CONST_TOKEN != 0
  #ifndef YY_parse_ENUM_TOKEN
   #define YY_parse_ENUM_TOKEN yy_parse_enum_token
  #endif
 #endif

class YY_parse_CLASS YY_parse_INHERIT
{
public: 
 #if YY_parse_USE_CONST_TOKEN != 0
  /* static const int token ... */
  
 #line 212 "/usr/share/bison++/bison.h"
static const int H_NUMBER;
static const int D_NUMBER;
static const int O_NUMBER;
static const int B_NUMBER;
static const int CONVERT_OP;
static const int B_DATA;
static const int H_RANGE_GUESS;
static const int D_NUMBER_GUESS;
static const int O_NUMBER_GUESS;
static const int B_NUMBER_GUESS;
static const int BAD_CMD;
static const int MEM_OP;
static const int IF;
static const int MEM_COMP;
static const int MEM_DISK8;
static const int MEM_DISK9;
static const int MEM_DISK10;
static const int MEM_DISK11;
static const int EQUALS;
static const int TRAIL;
static const int CMD_SEP;
static const int LABEL_ASGN_COMMENT;
static const int CMD_SIDEFX;
static const int CMD_RETURN;
static const int CMD_BLOCK_READ;
static const int CMD_BLOCK_WRITE;
static const int CMD_UP;
static const int CMD_DOWN;
static const int CMD_LOAD;
static const int CMD_SAVE;
static const int CMD_VERIFY;
static const int CMD_IGNORE;
static const int CMD_HUNT;
static const int CMD_FILL;
static const int CMD_MOVE;
static const int CMD_GOTO;
static const int CMD_REGISTERS;
static const int CMD_READSPACE;
static const int CMD_WRITESPACE;
static const int CMD_RADIX;
static const int CMD_MEM_DISPLAY;
static const int CMD_BREAK;
static const int CMD_TRACE;
static const int CMD_IO;
static const int CMD_BRMON;
static const int CMD_COMPARE;
static const int CMD_DUMP;
static const int CMD_UNDUMP;
static const int CMD_EXIT;
static const int CMD_DELETE;
static const int CMD_CONDITION;
static const int CMD_COMMAND;
static const int CMD_ASSEMBLE;
static const int CMD_DISASSEMBLE;
static const int CMD_NEXT;
static const int CMD_STEP;
static const int CMD_PRINT;
static const int CMD_DEVICE;
static const int CMD_HELP;
static const int CMD_WATCH;
static const int CMD_DISK;
static const int CMD_SYSTEM;
static const int CMD_QUIT;
static const int CMD_CHDIR;
static const int CMD_BANK;
static const int CMD_LOAD_LABELS;
static const int CMD_SAVE_LABELS;
static const int CMD_ADD_LABEL;
static const int CMD_DEL_LABEL;
static const int CMD_SHOW_LABELS;
static const int CMD_RECORD;
static const int CMD_MON_STOP;
static const int CMD_PLAYBACK;
static const int CMD_CHAR_DISPLAY;
static const int CMD_SPRITE_DISPLAY;
static const int CMD_TEXT_DISPLAY;
static const int CMD_SCREENCODE_DISPLAY;
static const int CMD_ENTER_DATA;
static const int CMD_ENTER_BIN_DATA;
static const int CMD_KEYBUF;
static const int CMD_BLOAD;
static const int CMD_BSAVE;
static const int CMD_SCREEN;
static const int CMD_UNTIL;
static const int CMD_CPU;
static const int CMD_YYDEBUG;
static const int CMD_BACKTRACE;
static const int CMD_SCREENSHOT;
static const int CMD_PWD;
static const int CMD_DIR;
static const int CMD_RESOURCE_GET;
static const int CMD_RESOURCE_SET;
static const int CMD_ATTACH;
static const int CMD_DETACH;
static const int CMD_MON_RESET;
static const int CMD_TAPECTRL;
static const int CMD_CARTFREEZE;
static const int CMD_CPUHISTORY;
static const int CMD_MEMMAPZAP;
static const int CMD_MEMMAPSHOW;
static const int CMD_MEMMAPSAVE;
static const int CMD_COMMENT;
static const int CMD_LIST;
static const int CMD_EXPORT;
static const int CMD_AUTOSTART;
static const int CMD_AUTOLOAD;
static const int CMD_LABEL_ASGN;
static const int L_PAREN;
static const int R_PAREN;
static const int ARG_IMMEDIATE;
static const int REG_A;
static const int REG_X;
static const int REG_Y;
static const int COMMA;
static const int INST_SEP;
static const int REG_B;
static const int REG_C;
static const int REG_D;
static const int REG_E;
static const int REG_H;
static const int REG_L;
static const int REG_AF;
static const int REG_BC;
static const int REG_DE;
static const int REG_HL;
static const int REG_IX;
static const int REG_IY;
static const int REG_SP;
static const int REG_IXH;
static const int REG_IXL;
static const int REG_IYH;
static const int REG_IYL;
static const int STRING;
static const int FILENAME;
static const int R_O_L;
static const int OPCODE;
static const int LABEL;
static const int BANKNAME;
static const int CPUTYPE;
static const int MON_REGISTER;
static const int COMPARE_OP;
static const int RADIX_TYPE;
static const int INPUT_SPEC;
static const int CMD_CHECKPT_ON;
static const int CMD_CHECKPT_OFF;
static const int TOGGLE;
static const int MASK;


#line 212 "/usr/share/bison++/bison.h"
 /* decl const */
 #else
  enum YY_parse_ENUM_TOKEN { YY_parse_NULL_TOKEN=0
  
 #line 215 "/usr/share/bison++/bison.h"
	,H_NUMBER=258
	,D_NUMBER=259
	,O_NUMBER=260
	,B_NUMBER=261
	,CONVERT_OP=262
	,B_DATA=263
	,H_RANGE_GUESS=264
	,D_NUMBER_GUESS=265
	,O_NUMBER_GUESS=266
	,B_NUMBER_GUESS=267
	,BAD_CMD=268
	,MEM_OP=269
	,IF=270
	,MEM_COMP=271
	,MEM_DISK8=272
	,MEM_DISK9=273
	,MEM_DISK10=274
	,MEM_DISK11=275
	,EQUALS=276
	,TRAIL=277
	,CMD_SEP=278
	,LABEL_ASGN_COMMENT=279
	,CMD_SIDEFX=280
	,CMD_RETURN=281
	,CMD_BLOCK_READ=282
	,CMD_BLOCK_WRITE=283
	,CMD_UP=284
	,CMD_DOWN=285
	,CMD_LOAD=286
	,CMD_SAVE=287
	,CMD_VERIFY=288
	,CMD_IGNORE=289
	,CMD_HUNT=290
	,CMD_FILL=291
	,CMD_MOVE=292
	,CMD_GOTO=293
	,CMD_REGISTERS=294
	,CMD_READSPACE=295
	,CMD_WRITESPACE=296
	,CMD_RADIX=297
	,CMD_MEM_DISPLAY=298
	,CMD_BREAK=299
	,CMD_TRACE=300
	,CMD_IO=301
	,CMD_BRMON=302
	,CMD_COMPARE=303
	,CMD_DUMP=304
	,CMD_UNDUMP=305
	,CMD_EXIT=306
	,CMD_DELETE=307
	,CMD_CONDITION=308
	,CMD_COMMAND=309
	,CMD_ASSEMBLE=310
	,CMD_DISASSEMBLE=311
	,CMD_NEXT=312
	,CMD_STEP=313
	,CMD_PRINT=314
	,CMD_DEVICE=315
	,CMD_HELP=316
	,CMD_WATCH=317
	,CMD_DISK=318
	,CMD_SYSTEM=319
	,CMD_QUIT=320
	,CMD_CHDIR=321
	,CMD_BANK=322
	,CMD_LOAD_LABELS=323
	,CMD_SAVE_LABELS=324
	,CMD_ADD_LABEL=325
	,CMD_DEL_LABEL=326
	,CMD_SHOW_LABELS=327
	,CMD_RECORD=328
	,CMD_MON_STOP=329
	,CMD_PLAYBACK=330
	,CMD_CHAR_DISPLAY=331
	,CMD_SPRITE_DISPLAY=332
	,CMD_TEXT_DISPLAY=333
	,CMD_SCREENCODE_DISPLAY=334
	,CMD_ENTER_DATA=335
	,CMD_ENTER_BIN_DATA=336
	,CMD_KEYBUF=337
	,CMD_BLOAD=338
	,CMD_BSAVE=339
	,CMD_SCREEN=340
	,CMD_UNTIL=341
	,CMD_CPU=342
	,CMD_YYDEBUG=343
	,CMD_BACKTRACE=344
	,CMD_SCREENSHOT=345
	,CMD_PWD=346
	,CMD_DIR=347
	,CMD_RESOURCE_GET=348
	,CMD_RESOURCE_SET=349
	,CMD_ATTACH=350
	,CMD_DETACH=351
	,CMD_MON_RESET=352
	,CMD_TAPECTRL=353
	,CMD_CARTFREEZE=354
	,CMD_CPUHISTORY=355
	,CMD_MEMMAPZAP=356
	,CMD_MEMMAPSHOW=357
	,CMD_MEMMAPSAVE=358
	,CMD_COMMENT=359
	,CMD_LIST=360
	,CMD_EXPORT=361
	,CMD_AUTOSTART=362
	,CMD_AUTOLOAD=363
	,CMD_LABEL_ASGN=364
	,L_PAREN=365
	,R_PAREN=366
	,ARG_IMMEDIATE=367
	,REG_A=368
	,REG_X=369
	,REG_Y=370
	,COMMA=371
	,INST_SEP=372
	,REG_B=373
	,REG_C=374
	,REG_D=375
	,REG_E=376
	,REG_H=377
	,REG_L=378
	,REG_AF=379
	,REG_BC=380
	,REG_DE=381
	,REG_HL=382
	,REG_IX=383
	,REG_IY=384
	,REG_SP=385
	,REG_IXH=386
	,REG_IXL=387
	,REG_IYH=388
	,REG_IYL=389
	,STRING=390
	,FILENAME=391
	,R_O_L=392
	,OPCODE=393
	,LABEL=394
	,BANKNAME=395
	,CPUTYPE=396
	,MON_REGISTER=397
	,COMPARE_OP=398
	,RADIX_TYPE=399
	,INPUT_SPEC=400
	,CMD_CHECKPT_ON=401
	,CMD_CHECKPT_OFF=402
	,TOGGLE=403
	,MASK=404


#line 215 "/usr/share/bison++/bison.h"
 /* enum token */
     }; /* end of enum declaration */
 #endif
public:
 int YY_parse_PARSE(YY_parse_PARSE_PARAM);
 virtual void YY_parse_ERROR(char *msg) YY_parse_ERROR_BODY;
 #ifdef YY_parse_PURE
  #ifdef YY_parse_LSP_NEEDED
   virtual int  YY_parse_LEX(YY_parse_STYPE *YY_parse_LVAL,YY_parse_LTYPE *YY_parse_LLOC) YY_parse_LEX_BODY;
  #else
   virtual int  YY_parse_LEX(YY_parse_STYPE *YY_parse_LVAL) YY_parse_LEX_BODY;
  #endif
 #else
  virtual int YY_parse_LEX() YY_parse_LEX_BODY;
  YY_parse_STYPE YY_parse_LVAL;
  #ifdef YY_parse_LSP_NEEDED
   YY_parse_LTYPE YY_parse_LLOC;
  #endif
  int YY_parse_NERRS;
  int YY_parse_CHAR;
 #endif
 #if YY_parse_DEBUG != 0
  public:
   int YY_parse_DEBUG_FLAG;	/*  nonzero means print parse trace	*/
 #endif
public:
 YY_parse_CLASS(YY_parse_CONSTRUCTOR_PARAM);
public:
 YY_parse_MEMBERS 
};
/* other declare folow */
#endif


#if YY_parse_COMPATIBILITY != 0
 /* backward compatibility */
 /* Removed due to bison problems
 /#ifndef YYSTYPE
 / #define YYSTYPE YY_parse_STYPE
 /#endif*/

 #ifndef YYLTYPE
  #define YYLTYPE YY_parse_LTYPE
 #endif
 #ifndef YYDEBUG
  #ifdef YY_parse_DEBUG 
   #define YYDEBUG YY_parse_DEBUG
  #endif
 #endif

#endif
/* END */

 #line 267 "/usr/share/bison++/bison.h"
#endif
