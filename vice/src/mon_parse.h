typedef union {
	MON_ADDR a;
        int i;
        REG_ID reg;
        CONDITIONAL cond_op;
        CONDITIONAL_NODE *cond_node;
        RADIXTYPE rt;
        ACTION action;
        char *str;
} YYSTYPE;
#define	H_NUMBER	258
#define	D_NUMBER	259
#define	O_NUMBER	260
#define	B_NUMBER	261
#define	CONVERT_OP	262
#define	B_DATA	263
#define	D_NUMBER_GUESS	264
#define	O_NUMBER_GUESS	265
#define	B_NUMBER_GUESS	266
#define	TRAIL	267
#define	BAD_CMD	268
#define	MEM_OP	269
#define	IF	270
#define	MEM_COMP	271
#define	MEM_DISK8	272
#define	MEM_DISK9	273
#define	CMD_SEP	274
#define	REG_ASGN_SEP	275
#define	EQUALS	276
#define	CMD_SIDEFX	277
#define	CMD_RETURN	278
#define	CMD_BLOCK_READ	279
#define	CMD_BLOCK_WRITE	280
#define	CMD_UP	281
#define	CMD_DOWN	282
#define	CMD_LOAD	283
#define	CMD_SAVE	284
#define	CMD_VERIFY	285
#define	CMD_IGNORE	286
#define	CMD_HUNT	287
#define	CMD_FILL	288
#define	CMD_MOVE	289
#define	CMD_GOTO	290
#define	CMD_REGISTERS	291
#define	CMD_READSPACE	292
#define	CMD_WRITESPACE	293
#define	CMD_RADIX	294
#define	CMD_MEM_DISPLAY	295
#define	CMD_BREAK	296
#define	CMD_TRACE	297
#define	CMD_IO	298
#define	CMD_BRMON	299
#define	CMD_COMPARE	300
#define	CMD_DUMP	301
#define	CMD_UNDUMP	302
#define	CMD_EXIT	303
#define	CMD_DELETE	304
#define	CMD_CONDITION	305
#define	CMD_COMMAND	306
#define	CMD_ASSEMBLE	307
#define	CMD_DISASSEMBLE	308
#define	CMD_NEXT	309
#define	CMD_STEP	310
#define	CMD_PRINT	311
#define	CMD_DEVICE	312
#define	CMD_HELP	313
#define	CMD_WATCH	314
#define	CMD_DISK	315
#define	CMD_SYSTEM	316
#define	CMD_QUIT	317
#define	CMD_CHDIR	318
#define	CMD_BANK	319
#define	CMD_LOAD_LABELS	320
#define	CMD_SAVE_LABELS	321
#define	CMD_ADD_LABEL	322
#define	CMD_DEL_LABEL	323
#define	CMD_SHOW_LABELS	324
#define	CMD_RECORD	325
#define	CMD_STOP	326
#define	CMD_PLAYBACK	327
#define	CMD_CHAR_DISPLAY	328
#define	CMD_SPRITE_DISPLAY	329
#define	CMD_TEXT_DISPLAY	330
#define	CMD_ENTER_DATA	331
#define	CMD_ENTER_BIN_DATA	332
#define	CMD_BLOAD	333
#define	CMD_BSAVE	334
#define	L_PAREN	335
#define	R_PAREN	336
#define	ARG_IMMEDIATE	337
#define	REG_A	338
#define	REG_X	339
#define	REG_Y	340
#define	COMMA	341
#define	INST_SEP	342
#define	STRING	343
#define	FILENAME	344
#define	R_O_L	345
#define	OPCODE	346
#define	LABEL	347
#define	BANKNAME	348
#define	REGISTER	349
#define	COMPARE_OP	350
#define	RADIX_TYPE	351
#define	INPUT_SPEC	352
#define	CMD_CHECKPT_ONOFF	353
#define	TOGGLE	354


extern YYSTYPE yylval;
