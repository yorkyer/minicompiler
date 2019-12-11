#include "utils.h"
#include "lexer.h"
#include "symtable.h"


extern vector<Token> tokens;
extern VarTable vtable;
extern FunTable ftable;

static int curtoken = 0;
static string HP;
static ofstream outdata, outcode;
static int t_index = 0;
static int a_index = 0;
static int g_index = 0;
static int s_index = 0;
static int if_index = 0;
static int for_index = 0;
static int while_index = 0;
static int dowhile_index = 0;
static string indent;


void gencode();
static bool match(TokenTag tag);
static bool match(int offset, TokenTag tag);
static string eat(TokenTag tag);
static string newvar(string prefix);
static void resetvar(string prefix);
static string newlabel(string prefix);
static void addindent();
static void subindent();

static void program();
static void function();
static void main_function();
static void block(bool isscoped);

static void const_data(bool isglobal);
static void const_declaration(bool isglobal);
static void variable_data(bool isglobal);
static void variable_declaration(bool isglobal);

static void stmts();
static void stmt();
static void assign();
static void call();
static string call(Type &type);
static void if_condition();
static void for_loop();
static void while_loop();
static void dowhile_loop();
static void read();
static void print();
static void ret();

static Type expr(string var);
static Type term(string var);
static string factor(Type &type);

static void args();
static vector<string> values();

static string condition(bool reverse);
static string for_step();
static Type type();
static string atom(Type &tp);
static string atom();
static string num();
static string positive_num();


void gencode()
{
	vtable.clear();
	outdata.open("data.txt");
	outcode.open("code.txt");
	program();
	outdata.close();
	outcode.close();
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
		return eat(tag);
	}
	return tokens[curtoken++].value;
}


static string newvar(string prefix)
{
	
	if (prefix == "g") return "g" + to_string(g_index++);
	if (prefix == "a") return "a" + to_string(a_index++);
	if (prefix == "s") return "s" + to_string(s_index++);
	return "t" + to_string(t_index++);
}


static void resetvar(string prefix)
{
	
	if (prefix == "g") g_index = 0;
	if (prefix == "a") a_index = 0;
	if (prefix == "t") t_index = 0;
	if (prefix == "s") s_index = 0;
}


static string newlabel(string prefix)
{
	if (prefix == "if") return "if" + to_string(if_index++);
	if (prefix == "for") return "for" + to_string(for_index++);
	if (prefix == "while") return "while" + to_string(while_index++);
	return "dowhile" + to_string(dowhile_index++);
}


static void addindent()
{
	indent += "\t";
}


static void subindent()
{
	if (indent != "")
		indent = indent.substr(0, indent.size()-1);
}


static void program()
{
	HP = newvar("g");
	outdata << HP << " = 0x10040000\n";
	const_data(true);
	variable_data(true);
	while ((match(TokenTag::VOID) || match(TokenTag::INT) ||
				  match(TokenTag::CHAR)) &&
				 match(1, TokenTag::ID) && tokens[curtoken+1].value != "main")
		function();
	main_function();
}


static void function()
{
	resetvar("a");
	resetvar("t");
	resetvar("s");

	if (match(TokenTag::VOID))
		eat(TokenTag::VOID);
	else
		type();
	string id = eat(TokenTag::ID);

	outcode << indent << "FUNCTION " << id << ":" << endl;
	addindent();

	vtable.enter_scope();

	eat(TokenTag::LPARENT);
	args();
	eat(TokenTag::RPARENT);
	block(true);

	outcode << indent << "RET\n\n";
	subindent();
}


static void main_function()
{
	resetvar("a");
	resetvar("t");
	resetvar("s");
	outcode << indent << "FUNCTION " << "main:\n";
	addindent();

	eat(TokenTag::VOID);
	eat(TokenTag::ID);
	eat(TokenTag::LPARENT);
	eat(TokenTag::RPARENT);
	block(false);

	outcode << indent << "RET\n\n";
	subindent();
}


static void block(bool isscoped)
{
	eat(TokenTag::LBRACE);

	if (!isscoped) vtable.enter_scope();

	const_data(false);
	variable_data(false);
	stmts();
	eat(TokenTag::RBRACE);

	vtable.exit_scope();
}


static void const_data(bool isglobal)
{
	while (match(TokenTag::CONST)) {
		const_declaration(isglobal);
		eat(TokenTag::SEMICN);
	}
}


static void const_declaration(bool isglobal)
{
	eat(TokenTag::CONST);
	Type tp = type();
	string id = eat(TokenTag::ID);
	eat(TokenTag::ASSIGN);
	string value = atom();

	VarAttr attr;
	attr.type = tp;
	attr.qual = Qual::CONST;
	attr.value = stoi(value);
	vtable.addsym(id, attr);

	string var;
	if (isglobal) {
		var = newvar("g");
		outdata << var << " = " << value << endl;
	}
	else {
		var = newvar("s");
		outcode << indent << var << " = " << value << endl;
	}
	vtable.getattr(id).var = var;

	while (match(TokenTag::COMMA)) {
		eat(TokenTag::COMMA);
		id = eat(TokenTag::ID);
		eat(TokenTag::ASSIGN);
		value = atom();

		VarAttr attr;
		attr.type = tp;
		attr.qual = Qual::CONST;
		attr.value = stoi(value);
		vtable.addsym(id, attr);

		if (isglobal) {
			var = newvar("g");
			outdata << var << " = " << value << endl;
		}
		else {
			var = newvar("s");
			outcode << indent << var << " = " << value << endl;
		}
		vtable.getattr(id).var = var;
	}
}


static void variable_data(bool isglobal)
{
	while ((match(TokenTag::INT) || match(TokenTag::CHAR)) &&
				 !match(2, TokenTag::LPARENT)) {
		variable_declaration(isglobal);
		eat(TokenTag::SEMICN);
	}
}


static void variable_declaration(bool isglobal)
{
	Type tp = type();
	string id = eat(TokenTag::ID);
	string size;
	string var;

	if (match(TokenTag::LBRACK)) {
		eat(TokenTag::LBRACK);
		size = positive_num();
		eat(TokenTag::RBRACK);

		if (isglobal) {
			var = newvar("g");
			outdata << var << " " << size << endl;
		}
		else {
			var = newvar("s");
			outcode << indent << var << " = " << HP << endl;
			outcode << indent << HP << " = " << var << " + " << stoi(size) * 4 << endl;
		}
	}
	else {
		if (isglobal) {
			var = newvar("g");
			outdata << var << endl;
		}
		else {
			var = newvar("s");
		}
	}

	VarAttr attr;
	attr.type = tp;
	attr.qual = Qual::VOID;
	vtable.addsym(id, attr);

	vtable.getattr(id).var = var;

	while (match(TokenTag::COMMA)) {
		eat(TokenTag::COMMA);
		id = eat(TokenTag::ID);
		if (match(TokenTag::LBRACK)) {
			eat(TokenTag::LBRACK);
			size = positive_num();
			eat(TokenTag::RBRACK);

			if (isglobal) {
				var = newvar("g");
				outdata << var << " " << size << endl;
			}
			else {
				var = newvar("s");
				outcode << indent << var << " = " << HP << endl;
				outcode << indent << HP << " = " << var << " + " << stoi(size) * 4 << endl;
			}
		}
		else {
			if (isglobal) {
				var = newvar("g");
				outdata << var << endl;
			}
			else {
				var = newvar("s");
			}
		}

		VarAttr attr;
		attr.type = tp;
		attr.qual = Qual::VOID;
		vtable.addsym(id, attr);

		vtable.getattr(id).var = var;
	}
}


static void stmts()
{
	while (match(TokenTag::SEMICN) || match(TokenTag::ID) ||
				 match(TokenTag::IF) || match(TokenTag::FOR) ||
				 match(TokenTag::WHILE) || match(TokenTag::DO) ||
				 match(TokenTag::SCANF) || match(TokenTag::PRINTF) ||
				 match(TokenTag::RETURN) || match(TokenTag::LBRACE)) {
		stmt();
		outcode << endl;
	}
}


static void stmt()
{
	resetvar("t");
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
}



static void assign()
{
	string id = eat(TokenTag::ID);
	string left;
	if (match(TokenTag::LBRACK)) {
		string base = vtable.getattr(id).var;
		string offset = newvar("t");

		eat(TokenTag::LBRACK);
		expr(offset);
		eat(TokenTag::RBRACK);

		outcode << indent << offset << " = " << offset << " * " << 4 << endl;
		outcode << indent << offset << " = " << base << " + " << offset << endl;
		left = "M[ " + offset + " ]";
	}
	else {
		left = vtable.getattr(id).var;
	}
	eat(TokenTag::ASSIGN);

	string var = newvar("t");
	expr(var);

	outcode << indent << left << " = " << var << endl;
}


static void call()
{
	Type type;
	call(type);	
}


static string call(Type &type)
{
	string id =	eat(TokenTag::ID);
	type = ftable.getattr(id).type;

	eat(TokenTag::LPARENT);
	vector<string> args = values();
	eat(TokenTag::RPARENT);

	outcode << indent << "CALL " << id;
	for (auto s : args)
		outcode << " " << s;
	outcode << "\n\n";
	return "v0";
}


static void if_condition()
{
	string label = newlabel("if");
	string label_else = "else" + to_string(if_index - 1);
	string label_end = label + "_end";
	outcode << indent << label << ":\n";
	addindent();

	eat(TokenTag::IF);
	eat(TokenTag::LPARENT);
	string cond = condition(true);
	eat(TokenTag::RPARENT);

	outcode << indent << "IF " << cond << " GOTO " << label_else << endl;

	stmt();
	if (match(TokenTag::ELSE)) {
		outcode << indent << "GOTO " << label_end << "\n";
		subindent();
		outcode << indent << label_else << ":\n";
		addindent(); 

		eat(TokenTag::ELSE);
		stmt();

		subindent();
		outcode << indent << label_end << ":\n";
	}
	else {
		subindent();
		outcode << indent << label_else << ":\n";
	}
}


static void for_loop()
{
	string label = newlabel("for");
	string label_end = label + "_end";

	eat(TokenTag::FOR);
	eat(TokenTag::LPARENT);
	assign();
	eat(TokenTag::SEMICN);

	outcode << indent << label << ":\n";
	addindent();

	string cond = condition(true);

	outcode << indent << "IF " << cond << " GOTO " << label_end << "\n\n";

	eat(TokenTag::SEMICN);
	string step = for_step();
	eat(TokenTag::RPARENT);
	stmt();

	outcode << indent << step << "\n";
	outcode << indent << "GOTO " << label << endl;
	subindent();
	outcode << indent << label_end << ":\n";
}


static void while_loop()
{
	string label = newlabel("while");
	string label_end = label + "_end";
	outcode << indent << label << ":\n";
	addindent();

	eat(TokenTag::WHILE);
	eat(TokenTag::LPARENT);
	string cond = condition(true);

	outcode << indent << "IF " << cond << " GOTO " << label_end << "\n\n";

	eat(TokenTag::RPARENT);
	stmt();

	outcode << indent << "GOTO " << label << endl;
	subindent();
	outcode << indent << label_end << ":\n";
}


static void dowhile_loop()
{
	string label = newlabel("dowhile");
	string label_end = label + "_end";
	outcode << indent << label << ":\n";
	addindent();

	eat(TokenTag::DO);
	stmt();
	eat(TokenTag::WHILE);
	eat(TokenTag::LPARENT);
	string cond = condition(false);
	eat(TokenTag::RPARENT);

	outcode << indent << "IF " << cond << " GOTO " << label << "\n";
	subindent();
	outcode << indent << label_end << ":\n";
}


static void read()
{
	eat(TokenTag::SCANF);
	eat(TokenTag::LPARENT);

	string id = eat(TokenTag::ID);
	string var = vtable.getattr(id).var;
	Type type = vtable.getattr(id).type;
	if (type == Type::INT)
		outcode << indent << "READ INT " << var << "\n";
	else
		outcode << indent << "READ CHR " << var << "\n";

	while (match(TokenTag::COMMA)) {
		eat(TokenTag::COMMA);
		id = eat(TokenTag::ID);

		var = vtable.getattr(id).var;
		type = vtable.getattr(id).type;
		if (type == Type::INT)
			outcode << indent << "READ INT " << var << "\n";
		else
			outcode << indent << "READ CHR " << var << "\n";
	}
	eat(TokenTag::RPARENT);
}


static void print()
{
	eat(TokenTag::PRINTF);
	eat(TokenTag::LPARENT);
	if (match(TokenTag::STR)) {
		string str = eat(TokenTag::STR);

		string var = newvar("g");
		outdata << var << " = \"" << str << "\"" << "\n";
		outcode << indent << "PRINT STR " << var << "\n";

		if (match(TokenTag::COMMA)) {
			eat(TokenTag::COMMA);

			var = newvar("t");
			Type type = expr(var);
			if (type == Type::INT)
				outcode << indent << "PRINT INT " << var << "\n";
			else
				outcode << indent << "PRINT CHR " << var << "\n";
		}
	}
	else {
		string var = newvar("t");
		Type type = expr(var);
		if (type == Type::INT)
			outcode << indent << "PRINT INT " << var << "\n";
		else
			outcode << indent << "PRINT CHR " << var << "\n";
	}
	eat(TokenTag::RPARENT);
	outcode << indent << "PRINT CHR 10\n"; // '\n'
}


static void ret()
{
	eat(TokenTag::RETURN);
	if (match(TokenTag::LPARENT)) {
		string var = newvar("t");
		eat(TokenTag::LPARENT);
		expr(var);
		eat(TokenTag::RPARENT);
		outcode << indent << "v0 = " << var << "\n";
	}
	outcode << indent << "RET\n\n";
}


static Type expr(string var)
{
	string op;

	if (match(1, TokenTag::ID) || match(1, TokenTag::ADD) || match(1, TokenTag::SUB)) {
		if (match(TokenTag::ADD)) eat(TokenTag::ADD);
		else if (match(TokenTag::SUB)) {
			eat(TokenTag::SUB);
			op = "-";
		}
	}
	Type type = term(var);

	if (op != "")
		outcode << indent << var << " = " << "zero " << op << " "<< var << "\n";

	if (!(match(TokenTag::ADD) || match(TokenTag::SUB)))
		return type;

	string var1 = newvar("t");

	while (match(TokenTag::ADD) || match(TokenTag::SUB)) {
		if (match(TokenTag::ADD)) {
			eat(TokenTag::ADD);
			op = "+";
		}
		else {
			eat(TokenTag::SUB);
			op = "-";
		}
		term(var1);
		outcode << indent << var << " = " << var << " " << op << " " << var1 << "\n";
	}
	return Type::INT;
}


static Type term(string var)
{
	Type type;
	string var1 = factor(type);

	if (!(match(TokenTag::MUL) || match(TokenTag::DIV))) {
		outcode << indent << var << " = " << var1 << endl;
		return type;
	}

	string op;
	string var2;
	while (match(TokenTag::MUL) || match(TokenTag::DIV)) {
		if (match(TokenTag::MUL)) {
			eat(TokenTag::MUL);
			op = "*";
		}
		else {
			eat(TokenTag::DIV);
			op = "/";
		}
		var2 = factor(type);
		if ((isdigit(var1[0]) || var1[0] == '-') && (isdigit(var2[0]) || var2[0] == '-'))
			if (op == "*") outcode << indent << var << " = " << stoi(var1) * stoi(var2) << "\n"; 
			else 					 outcode << indent << var << " = " << stoi(var1) / stoi(var2) << "\n";
		else if (isdigit(var1[0]) || var1[0] == '-') 
			if (op == "*")
				outcode << indent << var << " = " << var2 << " " << op << " " << var1 << "\n";
			else {
				string tmp = newvar("t");
				outcode << indent << tmp << " = " << var1 << "\n";
				outcode << indent << var << " = " << tmp << " " << op << " " << var2 << "\n";
			}
		else 
			outcode << indent << var << " = " << var1 << " " << op << " " << var2 << "\n";
		var1 = var;
	}
	return Type::INT;
}


static string factor(Type &type)
{
	if (match(TokenTag::ID) && match(1, TokenTag::LPARENT)) return call(type);
	else if (match(TokenTag::ID) && match(1, TokenTag::LBRACK)) {
		string id = eat(TokenTag::ID);

		type = vtable.getattr(id).type;
		string base = vtable.getattr(id).var;
		string var = newvar("t");

		eat(TokenTag::LBRACK);
		expr(var);
		eat(TokenTag::RBRACK);

		outcode << indent << var << " = " << var << " * " << "4\n";
		outcode << indent << var << " = " << base << " + " << var << "\n";
		outcode << indent << var << " = " << "M[ " << var << " ]\n";
		return var;
	}
	else if (match(TokenTag::ID)) {
		string id = eat(TokenTag::ID);
		type = vtable.getattr(id).type;
		return vtable.getattr(id).var;
	}
	else if (match(TokenTag::LPARENT)){
		type = Type::INT;
		string var = newvar("t");

		eat(TokenTag::LPARENT);
		expr(var);
		eat(TokenTag::RPARENT);

		return var;
	}
	else {
		return atom(type);
	}
}


static void args()
{
	string id, var;
	if (match(TokenTag::INT) || match(TokenTag::CHAR)) {
		Type tp = type();
		id = eat(TokenTag::ID);

		VarAttr attr;
		attr.type = tp;
		vtable.addsym(id, attr);

		var = newvar("a");
		vtable.getattr(id).var = var;

		while (match(TokenTag::COMMA)) {
			eat(TokenTag::COMMA);
			type();
			id = eat(TokenTag::ID);
			var = newvar("a");

			VarAttr attr;
			attr.type = tp;
			vtable.addsym(id, attr);

			vtable.getattr(id).var = var;
		}
	}
}


static vector<string> values()
{
	vector<string> args;
	if (!match(TokenTag::RPARENT)) {
		string var = newvar("t");
		expr(var);
		args.push_back(var);
		while (match(TokenTag::COMMA)) {
			eat(TokenTag::COMMA);
			string var = newvar("t");
			expr(var);
			args.push_back(var);
		}
	}
	return args;
}


static string condition(bool reverse)
{
	map<string, string> reverse_op;
	reverse_op["<"]  = ">=";
	reverse_op["<="] = ">";
	reverse_op[">"]  = "<=";
	reverse_op[">="] = "<";
	reverse_op["=="] = "!=";
	reverse_op["!="] = "==";

	string var1 = newvar("t");
	string op = "!=";
	string var2 = "0";
	expr(var1);
	if (match(TokenTag::LT) || match(TokenTag::LE) ||
		  match(TokenTag::GT) || match(TokenTag::GE) ||
		  match(TokenTag::EQ) || match(TokenTag::NE)) {
		if (match(TokenTag::LT)) {
		 eat(TokenTag::LT);
		 op = "<";
		}
		else if (match(TokenTag::LE)) {
			eat(TokenTag::LE);
			op = "<=";
		}
		else if (match(TokenTag::GT)) {
			eat(TokenTag::GT);
			op = ">";
		}
		else if (match(TokenTag::GE)) {
			eat(TokenTag::GE);
			op = ">=";
		}
		else if (match(TokenTag::EQ)) {
			eat(TokenTag::EQ);
			op = "==";
		}
		else {
			eat(TokenTag::NE);
			op = "!=";
		}
		var2 = newvar("t");
		expr(var2);
	}

	if (reverse) op = reverse_op[op];

	return var1 + " " + op + " " + var2;
}


static string for_step()
{
	string id1 =	eat(TokenTag::ID);
	string var1 = vtable.getattr(id1).var;

	eat(TokenTag::ASSIGN);

	string id2 =	eat(TokenTag::ID);
	string var2 = vtable.getattr(id2).var;
	string op = "";

	if (match(TokenTag::ADD)) {
		eat(TokenTag::ADD);
		op = "+";
	}
	else {
		eat(TokenTag::SUB);
		op = "-";
	}
	string num = atom();
	return var1 + " = " + var2 + " " + op + " " + num;
}


static Type type()
{
	if (match(TokenTag::INT)) {
		eat(TokenTag::INT);
		return Type::INT;
	}
	eat(TokenTag::CHAR);
	return Type::CHAR;
}


static string atom(Type &type)
{
	if (!match(TokenTag::CHR)) {
		type = Type::INT;
		return num();
	}
	type = Type::CHAR;
	return to_string((int)eat(TokenTag::CHR)[0]);
}


static string atom()
{
	Type type;
	return atom(type);
}


static string num()
{
	string sign;
	if (match(TokenTag::ADD)) eat(TokenTag::ADD);
	else if (match(TokenTag::SUB)) {
		eat(TokenTag::SUB);
		sign = "-";
	}
	return sign + eat(TokenTag::NUM);
}


static string positive_num()
{
	if (match(TokenTag::ADD)) eat(TokenTag::ADD);
	return eat(TokenTag::NUM);
}