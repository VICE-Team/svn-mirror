typedef union {
	M_ADDR a;
	M_ADDR_RANGE arange;
        int i;
        REG_ID reg;
        CONDITIONAL cond_op;
        CONDITIONAL_NODE *cond_node;
        DATATYPE dt;
        ACTION action;
        char *str;
} YYSTYPE;
#define	H_NUMBER	258
#define	D_NUMBER	259
#define	O_NUMBER	260
#define	B_NUMBER	261
#define	COMMAND_NAME	262
#define	CONVERT_OP	263
#define	B_DATA	264
#define	TRAIL	265
#define	BAD_CMD	266
#define	MEM_OP	267
#define	IF	268
#define	MEM_COMP	269
#define	MEM_DISK	270
#define	CMD_SIDEFX	271
#define	CMD_RETURN	272
#define	CMD_BLOCK_READ	273
#define	CMD_BLOCK_WRITE	274
#define	CMD_UP	275
#define	CMD_DOWN	276
#define	CMD_LOAD	277
#define	CMD_SAVE	278
#define	CMD_VERIFY	279
#define	CMD_IGNORE	280
#define	CMD_HUNT	281
#define	CMD_FILL	282
#define	CMD_MOVE	283
#define	CMD_GOTO	284
#define	CMD_REGISTERS	285
#define	CMD_READSPACE	286
#define	CMD_WRITESPACE	287
#define	CMD_DISPLAYTYPE	288
#define	CMD_MEM_DISPLAY	289
#define	CMD_BREAK	290
#define	CMD_TRACE	291
#define	CMD_IO	292
#define	CMD_BRMON	293
#define	CMD_COMPARE	294
#define	CMD_DUMP	295
#define	CMD_UNDUMP	296
#define	CMD_EXIT	297
#define	CMD_DELETE	298
#define	CMD_CONDITION	299
#define	CMD_COMMAND	300
#define	CMD_ASSEMBLE	301
#define	CMD_DISASSEMBLE	302
#define	CMD_NEXT	303
#define	CMD_STEP	304
#define	CMD_PRINT	305
#define	CMD_DEVICE	306
#define	CMD_HELP	307
#define	CMD_WATCH	308
#define	CMD_DISK	309
#define	CMD_SYSTEM	310
#define	CMD_QUIT	311
#define	CMD_CHDIR	312
#define	CMD_LOAD_LABELS	313
#define	CMD_SAVE_LABELS	314
#define	CMD_ADD_LABEL	315
#define	CMD_DEL_LABEL	316
#define	CMD_LABEL	317
#define	L_PAREN	318
#define	R_PAREN	319
#define	ARG_IMMEDIATE	320
#define	REG_A	321
#define	REG_X	322
#define	REG_Y	323
#define	COMMA	324
#define	INST_SEP	325
#define	STRING	326
#define	FILENAME	327
#define	R_O_L	328
#define	OPCODE	329
#define	LABEL	330
#define	REGISTER	331
#define	COMPARE_OP	332
#define	DATA_TYPE	333
#define	INPUT_SPEC	334
#define	CMD_BREAKPT_ONOFF	335
#define	TOGGLE	336


extern YYSTYPE yylval;
