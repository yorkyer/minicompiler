#ifndef MIPS_IMPL_H
#define MIPS_IMPL_H


enum class MipsTag
{
	FUNC, MEM, IF, GOTO, CALL, RET, PRINT, READ, INT, CHR,
	ID, NUM, STR, ASSIGN, OP, RELOP, LBRACK, RBRACK, COLON
};


class MipsToken
{
public:
	MipsTag tag;
	string value;

	MipsToken(MipsTag t, string v) : tag(t), value(v) {}
};


class FuncInfo
{
public:
	int spill_s;
	int spill_t;
	int spill_a;

	FuncInfo() : spill_s(0), spill_t(0), spill_a(0) {}
};


void mips();
static void mips_lexer(string file, vector<MipsToken> &tokens);
static void data_parser();
static void code_parser();
static bool isspill(string id);
static void stack_read(string id, string reg, string func, int offset);
static void stack_read(string id, string reg, string func);
static void stack_read(string id, string func);
static void stack_read(string id, string func, int offset);
static void stack_save(string id, string func);
static void stack_save(string id, string func, int offset);
static void stack_save(string id, string reg, string func);
static void stack_save(string id, string reg, string func, int offset);
static void preprocess();
static void postprocess();
static void print_tokens(string file, vector<MipsToken> &tokens);


#endif