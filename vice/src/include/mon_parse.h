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
#define	CMD_SIDEFX	269
#define	CMD_RETURN	270
#define	CMD_BLOCK_READ	271
#define	CMD_BLOCK_WRITE	272
#define	CMD_UP	273
#define	CMD_DOWN	274
#define	CMD_LOAD	275
#define	CMD_SAVE	276
#define	CMD_VERIFY	277
#define	CMD_IGNORE	278
#define	CMD_HUNT	279
#define	CMD_FILL	280
#define	CMD_MOVE	281
#define	CMD_GOTO	282
#define	CMD_REGISTERS	283
#define	CMD_READSPACE	284
#define	CMD_WRITESPACE	285
#define	CMD_DISPLAYTYPE	286
#define	CMD_MEM_DISPLAY	287
#define	CMD_BREAK	288
#define	CMD_TRACE	289
#define	CMD_IO	290
#define	CMD_BRMON	291
#define	CMD_COMPARE	292
#define	CMD_DUMP	293
#define	CMD_UNDUMP	294
#define	CMD_EXIT	295
#define	CMD_DELETE	296
#define	CMD_CONDITION	297
#define	CMD_COMMAND	298
#define	CMD_ASSEMBLE	299
#define	CMD_DISASSEMBLE	300
#define	CMD_NEXT	301
#define	CMD_STEP	302
#define	CMD_PRINT	303
#define	CMD_DEVICE	304
#define	CMD_HELP	305
#define	CMD_WATCH	306
#define	CMD_DISK	307
#define	CMD_SYSTEM	308
#define	CMD_QUIT	309
#define	CMD_CHDIR	310
#define	STRING	311
#define	FILENAME	312
#define	R_O_L	313
#define	ASM_LINE	314
#define	REGISTER	315
#define	COMPARE_OP	316
#define	DATA_TYPE	317
#define	INPUT_SPEC	318
#define	CMD_BREAKPT_ONOFF	319
#define	TOGGLE	320


extern YYSTYPE yylval;
