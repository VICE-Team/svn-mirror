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
#define	TRAIL	264
#define	BAD_CMD	265
#define	MEM_OP	266
#define	IF	267
#define	MEM_COMP	268
#define	MEM_DISK8	269
#define	MEM_DISK9	270
#define	CMD_SEP	271
#define	REG_ASGN_SEP	272
#define	EQUALS	273
#define	CMD_SIDEFX	274
#define	CMD_RETURN	275
#define	CMD_BLOCK_READ	276
#define	CMD_BLOCK_WRITE	277
#define	CMD_UP	278
#define	CMD_DOWN	279
#define	CMD_LOAD	280
#define	CMD_SAVE	281
#define	CMD_VERIFY	282
#define	CMD_IGNORE	283
#define	CMD_HUNT	284
#define	CMD_FILL	285
#define	CMD_MOVE	286
#define	CMD_GOTO	287
#define	CMD_REGISTERS	288
#define	CMD_READSPACE	289
#define	CMD_WRITESPACE	290
#define	CMD_RADIX	291
#define	CMD_MEM_DISPLAY	292
#define	CMD_BREAK	293
#define	CMD_TRACE	294
#define	CMD_IO	295
#define	CMD_BRMON	296
#define	CMD_COMPARE	297
#define	CMD_DUMP	298
#define	CMD_UNDUMP	299
#define	CMD_EXIT	300
#define	CMD_DELETE	301
#define	CMD_CONDITION	302
#define	CMD_COMMAND	303
#define	CMD_ASSEMBLE	304
#define	CMD_DISASSEMBLE	305
#define	CMD_NEXT	306
#define	CMD_STEP	307
#define	CMD_PRINT	308
#define	CMD_DEVICE	309
#define	CMD_HELP	310
#define	CMD_WATCH	311
#define	CMD_DISK	312
#define	CMD_SYSTEM	313
#define	CMD_QUIT	314
#define	CMD_CHDIR	315
#define	CMD_BANK	316
#define	CMD_LOAD_LABELS	317
#define	CMD_SAVE_LABELS	318
#define	CMD_ADD_LABEL	319
#define	CMD_DEL_LABEL	320
#define	CMD_SHOW_LABELS	321
#define	CMD_RECORD	322
#define	CMD_STOP	323
#define	CMD_PLAYBACK	324
#define	CMD_CHAR_DISPLAY	325
#define	CMD_SPRITE_DISPLAY	326
#define	CMD_TEXT_DISPLAY	327
#define	CMD_ENTER_DATA	328
#define	CMD_ENTER_BIN_DATA	329
#define	CMD_BLOAD	330
#define	CMD_BSAVE	331
#define	L_PAREN	332
#define	R_PAREN	333
#define	ARG_IMMEDIATE	334
#define	REG_A	335
#define	REG_X	336
#define	REG_Y	337
#define	COMMA	338
#define	INST_SEP	339
#define	STRING	340
#define	FILENAME	341
#define	R_O_L	342
#define	OPCODE	343
#define	LABEL	344
#define	BANKNAME	345
#define	REGISTER	346
#define	COMPARE_OP	347
#define	RADIX_TYPE	348
#define	INPUT_SPEC	349
#define	CMD_CHECKPT_ONOFF	350
#define	TOGGLE	351


extern YYSTYPE yylval;
