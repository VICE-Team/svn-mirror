

#define S_NAMED 0x1
#define S_OFFSET 0x2
#define S_LINE 0x4

#define MAX_BREAKPOINTS 16

#define MAX_FUNCTION_CALLS 512

#define BP_FREE 0x0
#define BP_USED 0x1
#define BP_TEMP 0x2

#define PROMPT_REGS 0x1
#define PROMPT_CYCLES 0x2
#define PROMPT_INSN 0x4


struct cpu_regs {
	unsigned X, Y, S, U, PC;
	unsigned A, B, DP;
	unsigned H, N, Z, OV, C;
	unsigned EFI;
#ifdef H6309
	unsigned E, F, V, MD;
#endif
};


struct x_symbol {
	int flags;
	union {
		struct named_symbol {
			char *id;
			char *file;
			target_addr_t addr;
		} named;

		struct line_symbol {
			struct named_symbol *name;
			int lineno;
		} line;

		int offset;
	} u;
};


struct symbol_table {
	struct symbol *addr_to_symbol[0x10000];
	char *name_area;
	int name_area_free;
	char *name_area_next;
};


struct breakpoint {
	target_addr_t addr;
	int flags;
	int count;
};


#define FC_TAIL_CALL 0x1

struct function_call {
	target_addr_t entry_point;
	struct cpu_regs entry_regs;
	int flags;
};

void add_named_symbol (const char *id, target_addr_t value, const char *filename);
struct x_symbol * find_symbol (target_addr_t value);
void monitor_branch (void);
void monitor_call (unsigned int flags);
void monitor_return (void);
const char * monitor_addr_name (target_addr_t addr);
const char * absolute_addr_name (unsigned long addr);



