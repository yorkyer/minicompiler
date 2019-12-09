#include "utils.h"
#include "lexer.h"
#include "symtable.h"


extern vector<Token> tokens;
extern VarTable vtable;
extern FunTable ftable;

static int curtoken = 0;
static int numlines = 1;
static ofstream output;


void parser();
static bool match(TokenTag tag);
static bool match(int offset, TokenTag tag);
static string eat(TokenTag tag);

static void program();
static void function();
static void main_function();
static void block(bool isscoped);

static void const_data();
static void const_declaration();
static void variable_data();
static void variable_declaration();

static void stmts();
static void stmt();
static void assign();
static Type call();
static void if_condition();
static void for_loop();
static void while_loop();
static void dowhile_loop();
static void read();
static void print();
static void ret();

static Type expr();
static Type term();
static Type factor();

static vector<Type> args();
static vector<Type> values();

static void condition();
static void for_step();
static Type type();
static Type atom();
static Type atom(int &value);
static string num();
static void positive_num();


void parser()
{
	output.open("syntaxtree.txt");
	program();
	output.close();
}


static bool match(TokenTag tag)
{
	return match(0, tag);
}	


static bool match(int offset, TokenTag tag)
{
	if (curtoken + offset < tokens.size() && 
			tokens[curtoken + offset].tag == TokenTag::LINEEND) {
		curtoken++;
		numlines++;
		return match(tag);
	}
	return curtoken + offset < tokens.size() && 
			   tokens[curtoken + offset].tag == tag;
}


static string eat(TokenTag tag)
{
	if (curtoken < tokens.size() && 
		  tokens[curtoken].tag == TokenTag::LINEEND) {
		curtoken++;
		numlines++;
		return eat(tag);
	}
	if (curtoken >= tokens.size()) {
		cout << "parse error at line " << numlines << "\n";
		exit(1);
	}

	output << "\t" << tokens[curtoken].value << "\t"
				 << TokenTagName[static_cast<int>(tag)] << endl;
	if (tag != tokens[curtoken].tag) {
		cout << "parse error at line " << numlines << "\n";
		exit(1);
	}
	return tokens[curtoken++].value;
}


static void program()
{
	output << "enter program\n";
	const_data();
	variable_data();
	while ((match(TokenTag::VOID) || match(TokenTag::INT) ||
				  match(TokenTag::CHAR)) &&
				 match(1, TokenTag::ID) && tokens[curtoken+1].value != "main")
		function();
	main_function();
	output << "exit program\n";
}


static void function()
{
	output << "enter function\n";
	Type tp = Type::VOID;
	if (match(TokenTag::VOID))
		eat(TokenTag::VOID);
	else
		tp = type();
	string id = eat(TokenTag::ID);

	FunAttr attr;
	attr.type = tp;
	ftable.addsym(id, attr);

	vtable.enter_scope();
	output << "enter a new scope\n";

	eat(TokenTag::LPARENT);
	attr.args = args();
	eat(TokenTag::RPARENT);
	block(true);
	output << "exit function\n";
}


static void main_function()
{
	output << "enter main_function\n";
	eat(TokenTag::VOID);
	eat(TokenTag::ID);

	FunAttr attr;
	attr.type = Type::VOID;
	ftable.addsym("main", attr);

	eat(TokenTag::LPARENT);
	eat(TokenTag::RPARENT);
	block(false);
	output << "exit main_function\n";
}


static void block(bool isscoped)
{
	output << "enter block\n";
	eat(TokenTag::LBRACE);
	if (!isscoped) {
		vtable.enter_scope();
		output << "enter a new scope\n";
	}
	const_data();
	variable_data();
	stmts();
	eat(TokenTag::RBRACE);
	vtable.exit_scope();
	output << "exit the current scope\n";
	output << "exit block\n";
}


static void const_data()
{
	output << "enter const_data\n";
	while (match(TokenTag::CONST)) {
		const_declaration();
		eat(TokenTag::SEMICN);
	}
	output << "exit const_data\n";
}


static void const_declaration()
{
	output << "enter const_declaration\n";
	eat(TokenTag::CONST);
	Type tp = type();
	string id = eat(TokenTag::ID);
	eat(TokenTag::ASSIGN);
	int value;
	atom(value);

	VarAttr attr;
	attr.type = tp;
	attr.qual = Qual::CONST;
	attr.value = value;
	vtable.addsym(id, attr);

	while (match(TokenTag::COMMA)) {
		eat(TokenTag::COMMA);
		id = eat(TokenTag::ID);
		eat(TokenTag::ASSIGN);
		atom(value);

		VarAttr attr;
		attr.type = tp;
		attr.qual = Qual::CONST;
		attr.value = value;
		vtable.addsym(id, attr);
	}
	output << "exit const_declaration\n";
}


static void variable_data()
{
	output << "enter variable_data\n";
	while ((match(TokenTag::INT) || match(TokenTag::CHAR)) &&
				 !match(2, TokenTag::LPARENT)) {
		variable_declaration();
		eat(TokenTag::SEMICN);
	}
	output << "exit variable_data\n";
}


static void variable_declaration()
{
	output << "enter variable_declaration\n";
	Type tp = type();
	string id = eat(TokenTag::ID);
	if (match(TokenTag::LBRACK)) {
		eat(TokenTag::LBRACK);
		positive_num();
		eat(TokenTag::RBRACK);
	}

	VarAttr attr;
	attr.type = tp;
	attr.qual = Qual::VOID;
	vtable.addsym(id, attr);

	while (match(TokenTag::COMMA)) {
		eat(TokenTag::COMMA);
		id = eat(TokenTag::ID);
		if (match(TokenTag::LBRACK)) {
			eat(TokenTag::LBRACK);
			positive_num();
			eat(TokenTag::RBRACK);
		}
		VarAttr attr;
		attr.type = tp;
		attr.qual = Qual::VOID;
		vtable.addsym(id, attr);
	}
	output << "exit variable_declaration\n";
}


static void stmts()
{
	output << "enter stmts\n";
	while (match(TokenTag::SEMICN) || match(TokenTag::ID) ||
				 match(TokenTag::IF) || match(TokenTag::FOR) ||
				 match(TokenTag::WHILE) || match(TokenTag::DO) ||
				 match(TokenTag::SCANF) || match(TokenTag::PRINTF) ||
				 match(TokenTag::RETURN) || match(TokenTag::LBRACE)) {
		stmt();
	}
	output << "exit stmts\n";
}


static void stmt()
{
	output << "enter stmt\n";
	if (match(TokenTag::ID) && match(1, TokenTag::LPARENT)) {
		call();
		eat(TokenTag::SEMICN);
	}
	else if (match(TokenTag::ID)) {
		assign();
		eat(TokenTag::SEMICN);
	}
	else if (match(TokenTag::IF)) if_condition();
	else if (match(TokenTag::FOR)) for_loop();
	else if (match(TokenTag::WHILE)) while_loop();
	else if (match(TokenTag::DO)) dowhile_loop();
	else if (match(TokenTag::SCANF)) {
		read();
		eat(TokenTag::SEMICN);
	}
	else if (match(TokenTag::PRINTF)) {
		print();
		eat(TokenTag::SEMICN);
	}
	else if (match(TokenTag::RETURN)) {
		ret();
		eat(TokenTag::SEMICN);
	}
	else if (match(TokenTag::LBRACE)) block(false);
	else eat(TokenTag::SEMICN);
	output << "exit stmt\n";
}


static void assign()
{
	output << "enter assign\n";
	eat(TokenTag::ID);
	if (match(TokenTag::LBRACK)) {
		eat(TokenTag::LBRACK);
		expr();
		eat(TokenTag::RBRACK);
	}
	eat(TokenTag::ASSIGN);
	expr();
	output << "exit assign\n";
}


static Type call()
{
	output << "enter call\n";
	string id = eat(TokenTag::ID);
	eat(TokenTag::LPARENT);
	vector<Type> args = values();
	eat(TokenTag::RPARENT);
	output << "exit call\n";
	return ftable.getattr(id).type;
}


static void if_condition()
{
	output << "enter if_condition\n";
	eat(TokenTag::IF);
	eat(TokenTag::LPARENT);
	condition();
	eat(TokenTag::RPARENT);
	stmt();
	if (match(TokenTag::ELSE)) {
		eat(TokenTag::ELSE);
		stmt();
	}
}


static void for_loop()
{
	output << "enter for_loop\n";
	eat(TokenTag::FOR);
	eat(TokenTag::LPARENT);
	assign();
	eat(TokenTag::SEMICN);
	condition();
	eat(TokenTag::SEMICN);
	for_step();
	eat(TokenTag::RPARENT);
	stmt();
	output << "exit for_loop\n";
}


static void while_loop()
{
	output << "enter while_loop\n";
	eat(TokenTag::WHILE);
	eat(TokenTag::LPARENT);
	condition();
	eat(TokenTag::RPARENT);
	stmt();
	output << "exit while_loop\n";
}


static void dowhile_loop()
{
	output << "enter dowhile_loop\n";
	eat(TokenTag::DO);
	stmt();
	eat(TokenTag::WHILE);
	eat(TokenTag::LPARENT);
	condition();
	eat(TokenTag::RPARENT);
	output << "enter dowhile_loop\n";
}


static void read()
{
	output << "enter read\n";
	eat(TokenTag::SCANF);
	eat(TokenTag::LPARENT);
	eat(TokenTag::ID);
	while (match(TokenTag::COMMA)) {
		eat(TokenTag::COMMA);
		eat(TokenTag::ID);
	}
	eat(TokenTag::RPARENT);
	output << "exit read\n";
}


static void print()
{
	output << "enter print\n";
	eat(TokenTag::PRINTF);
	eat(TokenTag::LPARENT);
	if (match(TokenTag::STR)) {
		eat(TokenTag::STR);
		if (match(TokenTag::COMMA)) {
			eat(TokenTag::COMMA);
			expr();
		}
	}
	else expr();
	eat(TokenTag::RPARENT);
	output << "exit print\n";
}


static void ret()
{
	output << "enter ret\n";
	eat(TokenTag::RETURN);
	if (match(TokenTag::LPARENT)) {
		eat(TokenTag::LPARENT);
		expr();
		eat(TokenTag::RPARENT);
	}
	output << "exit ret\n";
}


static Type expr()
{
	output << "enter expr\n";
	if (match(1, TokenTag::ID) || match(1, TokenTag::ADD) || match(1, TokenTag::SUB)) {
		if (match(TokenTag::ADD)) eat(TokenTag::ADD);
		else if (match(TokenTag::SUB)) eat(TokenTag::SUB);
	}
	Type type = term();
	while (match(TokenTag::ADD) || match(TokenTag::SUB)) {
		if (match(TokenTag::ADD)) eat(TokenTag::ADD);
		else eat(TokenTag::SUB);
		term();
		type = Type::INT;
	}
	output << "exit expr\n";
	return type;
}


static Type term()
{
	output << "enter term\n";
	Type type = factor();
	while (match(TokenTag::MUL) || match(TokenTag::DIV)) {
		if (match(TokenTag::MUL)) eat(TokenTag::MUL);
		else (eat(TokenTag::DIV));
		factor();
		type = Type::INT;
	}
	output << "exit term\n";
	return type;
}


static Type factor()
{
	output << "enter factor\n";
	Type type = Type::INT;
	if (match(TokenTag::ID) && match(1, TokenTag::LPARENT)) type = call();
	else if (match(TokenTag::ID) && match(1, TokenTag::LBRACK)) {
		string id = eat(TokenTag::ID);
		eat(TokenTag::LBRACK);
		expr();
		eat(TokenTag::RBRACK);
		type = vtable.getattr(id).type;
	}
	else if (match(TokenTag::ID)) {
		string id = eat(TokenTag::ID);
		type = vtable.getattr(id).type;
	}
	else if (match(TokenTag::LPARENT)){
		eat(TokenTag::LPARENT);
		expr();
		eat(TokenTag::RPARENT);
	}
	else type = atom();
	output << "exit factor\n";
	return type;
}


static vector<Type> args()
{
	output << "enter args\n";
	vector<Type> args;
	Type tp;
	string id;
	if (match(TokenTag::INT) || match(TokenTag::CHAR)) {
		tp = type();
		args.push_back(tp);
		id = eat(TokenTag::ID);

		VarAttr attr;
		attr.type = tp;
		vtable.addsym(id, attr);

		while (match(TokenTag::COMMA)) {
			eat(TokenTag::COMMA);
			tp = type();
			args.push_back(tp);
			id = eat(TokenTag::ID);
			
			VarAttr attr;
			attr.type = tp;
			vtable.addsym(id, attr);
		}
	}
	output << "exit args\n";
	return args;
}


static vector<Type> values()
{
	output << "enter values\n";
	vector<Type> args;
	if (!match(TokenTag::RPARENT)) {
		args.push_back(expr());
		while (match(TokenTag::COMMA)) {
			eat(TokenTag::COMMA);
			args.push_back(expr());
		}
	}
	output << "exit values\n";
	return args;
}


static void condition()
{
	output << "enter condition\n";
	expr();
	if (match(TokenTag::LT) || match(TokenTag::LE) ||
		  match(TokenTag::GT) || match(TokenTag::GE) ||
		  match(TokenTag::EQ) || match(TokenTag::NE)) {
		if (match(TokenTag::LT)) eat(TokenTag::LT);
		else if (match(TokenTag::LE)) eat(TokenTag::LE);
		else if (match(TokenTag::GT)) eat(TokenTag::GT);
		else if (match(TokenTag::GE)) eat(TokenTag::GE);
		else if (match(TokenTag::EQ)) eat(TokenTag::EQ);
		else eat(TokenTag::NE);
		expr();
	}
	output << "exit condition\n";
}


static void for_step()
{
	output << "enter for_step\n";
	eat(TokenTag::ID);
	eat(TokenTag::ASSIGN);
	eat(TokenTag::ID);
	if (match(TokenTag::ADD)) eat(TokenTag::ADD);
	else eat(TokenTag::SUB);
	atom();
	output << "exit for_step\n";
}


static Type type()
{
	if (match(TokenTag::INT)) {
		eat(TokenTag::INT);
		return Type::INT;
	}
	else {
		eat(TokenTag::CHAR);
		return Type::CHAR;
	}
}


static Type atom()
{
	int value;
	return atom(value);
}


static Type atom(int &value)
{
	if (!match(TokenTag::CHR)) {
		value = stoi(num());
		return Type::INT;
	}
	value = (int)eat(TokenTag::CHR)[0];
	return Type::CHAR;
}


static string num()
{
	string sign = "";
	if (match(TokenTag::ADD)) {
		eat(TokenTag::ADD);
	}
	else if (match(TokenTag::SUB)) {
		eat(TokenTag::SUB);
		sign = "-";
	}
	return sign + eat(TokenTag::NUM);
}


static void positive_num()
{
	if (match(TokenTag::ADD)) eat(TokenTag::ADD);
	eat(TokenTag::NUM);
}