#include "utils.h"
#include "mips_impl.h"
#include "optimize.h"


static vector<MipsToken> data_tokens;
static vector<MipsToken> code_tokens;
static set<string> global_arrays;
static map<string, FuncInfo> table;
static string indent;
static ofstream output;


void mips(bool isoptimize)
{
	mips_lexer("data.txt", data_tokens);
	if (isoptimize) {
		optimize();
		mips_lexer("optimize.txt", code_tokens);
	}
	else {
		mips_lexer("code.txt", code_tokens);
	}	
	data_parser();
	code_parser();
}
	

static void mips_lexer(string file, vector<MipsToken> &tokens)
{
	ifstream input;
	input.open(file);
	char c;
	while (input.get(c)) {
		if (isspace(c)) ;
		else if (isdigit(c)) {
			string num;
			while (1) {
				num += c;
				if (isdigit(input.peek()) || input.peek() == 'x') {
					input.get(c);
				} else {
					break;
				}
			}
			tokens.push_back(MipsToken(MipsTag::NUM, num));
		}
		else if (isalpha(c) || c == '_') {
			string id;
			while (1) {
				id += c;
				if (isalnum(input.peek()) || input.peek() == '_') {
					input.get(c);
				} else {
					break;
				}
			}

			if 			(id == "FUNCTION")tokens.push_back(MipsToken(MipsTag::FUNC, id));
			else if (id == "M")			  tokens.push_back(MipsToken(MipsTag::MEM, id));
			else if (id == "IF")			tokens.push_back(MipsToken(MipsTag::IF, id));
			else if (id == "GOTO")		tokens.push_back(MipsToken(MipsTag::GOTO, id));
			else if (id == "CALL")		tokens.push_back(MipsToken(MipsTag::CALL, id));
			else if (id == "RET")			tokens.push_back(MipsToken(MipsTag::RET, id));
			else if (id == "PRINT")		tokens.push_back(MipsToken(MipsTag::PRINT, id));
			else if (id == "READ")		tokens.push_back(MipsToken(MipsTag::READ, id));
			else if (id == "INT")			tokens.push_back(MipsToken(MipsTag::INT, id));
			else if (id == "CHR")			tokens.push_back(MipsToken(MipsTag::CHR, id));
			else if (id == "STR")			tokens.push_back(MipsToken(MipsTag::STR, id));
			else											tokens.push_back(MipsToken(MipsTag::ID, id));
		}
		else if (c == '"') {
			string str = "";
			str += '"';
			while (1) {
				if (input.get(c) && c != '"') {
					if (c == '\\')
						str += '\\';
					str += c;
				} else {
					str += '"';
					break;
				}
			}
			tokens.push_back(MipsToken(MipsTag::STR, str));
		}
		else if (c == '>') {
			if (input.peek() == '=') {
				input.get(c);
				tokens.push_back(MipsToken(MipsTag::OP, ">="));
			} else {
				tokens.push_back(MipsToken(MipsTag::OP, ">"));
			}
		}
		else if (c == '<') {
			if (input.peek() == '=') {
				input.get(c);
				tokens.push_back(MipsToken(MipsTag::OP, "<="));
			} else {
				tokens.push_back(MipsToken(MipsTag::OP, "<"));
			}
		}
		else if (c == '=') {
			if (input.peek() == '=') {
				input.get(c);
				tokens.push_back(MipsToken(MipsTag::OP, "=="));
			} else {
				tokens.push_back(MipsToken(MipsTag::ASSIGN, "="));
			}
		}
		else if (c == '!') {
			input.get(c);
			tokens.push_back(MipsToken(MipsTag::OP, "!="));
		}
		else if (c == '+' || c == '-') {
			if (tokens[tokens.size()-1].tag == MipsTag::ID &&
					tokens[tokens.size()-2].tag == MipsTag::ASSIGN) {
				string op(1, c);
				tokens.push_back(MipsToken(MipsTag::OP, op));
			}
			else {
				string num;
				while (1) {
					num += c;
					if (isdigit(input.peek()) || input.peek() == 'x') {
						input.get(c);
					} else {
						break;
					}
				}
				tokens.push_back(MipsToken(MipsTag::NUM, num));
			}
		}
		else if (c == '-') tokens.push_back(MipsToken(MipsTag::OP, "-"));
		else if (c == '*') tokens.push_back(MipsToken(MipsTag::OP, "*"));		
		else if (c == '/') tokens.push_back(MipsToken(MipsTag::OP, "/"));
		else if (c == '[') tokens.push_back(MipsToken(MipsTag::LBRACK, "["));
		else if (c == ']') tokens.push_back(MipsToken(MipsTag::RBRACK, "]"));
		else if (c == ':') tokens.push_back(MipsToken(MipsTag::COLON, ":"));
	}
	input.close();

#ifdef DEBUG
	print_tokens(file, tokens);
#endif
}


static void print_tokens(string file, vector<MipsToken> &tokens)
{
	ofstream output;
	if (file == "data.txt")
		output.open("data_token.txt");
	else
		output.open("code_token.txt");
	for (auto t : tokens)
			output << t.value << "\n";
	output.close();
}


static void data_parser()
{
	output.open("data_area.txt");

	for (int i = 0; i < data_tokens.size(); i++) {
		if (data_tokens[i+1].tag == MipsTag::NUM) { // <id> <num>
			output << data_tokens[i].value << ":\t\t" 
						 << ".space " << stoi(data_tokens[i+1].value)*4 << "\n";
			global_arrays.insert(data_tokens[i].value);
			i += 1;
		}
		else if (data_tokens[i+2].tag == MipsTag::NUM) { // <id> = <num>
			output << data_tokens[i].value << ":\t\t" 
						 << ".word " << data_tokens[i+2].value << "\n";
			i += 2;
		}
		else if (data_tokens[i+2].tag == MipsTag::STR) { // <id> = <str>
			output << data_tokens[i].value << ":\t\t" 
						 << ".asciiz " << data_tokens[i+2].value << "\n";
			i += 2;
		}
		else { // <id>
			output << data_tokens[i].value << ":\t\t" << ".space " << "4\n";
		}
	}

	output.close();
}


static void code_parser()
{
	preprocess();

	output.open("code_area.txt");
	string func;
	for (int i = 0; i < code_tokens.size(); i++) {
		if (code_tokens[i].tag == MipsTag::FUNC) {
			func = code_tokens[i+1].value;
			
			output << "\n__" << func << ":\n\n";
			int offset = 4 * (8 + table[func].spill_s + table[func].spill_t);
			output << "sub $sp, $sp, " << offset << "\n";
			output << "sw $s0, 0($sp)\n";
			output << "sw $s1, 4($sp)\n";
			output << "sw $s2, 8($sp)\n";
			output << "sw $s3, 12($sp)\n";
			output << "sw $s4, 16($sp)\n";
			output << "sw $s5, 20($sp)\n";
			output << "sw $s6, 24($sp)\n";
			output << "sw $s7, 28($sp)\n";
			output << "\n";
			
			i += 2;
		}
		else if (code_tokens[i].tag == MipsTag::ID && 
						 code_tokens[i+1].tag == MipsTag::COLON) {
			string label = code_tokens[i].value;
			output << label << ":\n";

			i += 1;
		}
		else if (code_tokens[i].tag == MipsTag::ID && 
						 code_tokens[i+2].tag == MipsTag::NUM && 
						 code_tokens[i+3].tag != MipsTag::OP) {
			string id = code_tokens[i].value;
			string num = code_tokens[i+2].value;
			if (isspill(id)) {
				output << "li $t8, " << num << "\n";
				stack_save(id, func);
			}
			else
				output << "li $" << id << ", " << num << "\n";
			
			i += 2;
		}
		else if (code_tokens[i].tag == MipsTag::ID && 
						 code_tokens[i+2].tag == MipsTag::ID && 
						 code_tokens[i+3].tag != MipsTag::OP) {
			string id = code_tokens[i].value;
			string id1 = code_tokens[i+2].value;
			if (isspill(id1)) {
				stack_read(id1, func);
				id1 = "t8";
			}
			if (isspill(id)) {
				stack_save(id, id1, func);
			}
			else
				output << "move $" << id << ", $" << id1 << "\n";

			i += 2;
		}
		else if (code_tokens[i].tag == MipsTag::ID && 
						 code_tokens[i+2].tag == MipsTag::ID && 
						 code_tokens[i+3].tag == MipsTag::OP) {
			string id = code_tokens[i].value;
			string id1 = code_tokens[i+2].value;
			string op = code_tokens[i+3].value;
			string atom = code_tokens[i+4].value;
			if (isspill(id1)) {
				stack_read(id1, func);
				id1 = "t8";
			}
			if (isspill(atom)) {
				stack_read(atom, "t9", func);
				atom = "t9";
			}
			if (!isdigit(atom[0]) && atom[0] != '-') atom = "$" + atom;
			if (isspill(id)) {
				id1 = "$" + id1;
				if (op == "+") output << "add $t8, " << id1 << ", " << atom << "\n";
				else if (op == "-") output << "sub $t8, " << id1 << ", " << atom << "\n";
				else if (op == "*") output << "mul $t8, " << id1 << ", " << atom << "\n";
				else output << "div $t8, " << id1 << ", " << atom << "\n";
				stack_save(id, func);
			}
			else {
				id = "$" + id;
				id1 = "$" + id1;
				if (op == "+") output << "add " << id << ", " << id1 << ", " << atom << "\n";
				else if (op == "-") output << "sub " << id << ", " << id1 << ", " << atom << "\n";
				else if (op == "*") output << "mul " << id << ", " << id1 << ", " << atom << "\n";
				else output << "div " << id << ", " << id1 << ", " << atom << "\n";
			}

			i += 4;
		}
		else if (code_tokens[i].tag == MipsTag::IF) {
			string id1 = code_tokens[i+1].value;
			string op = code_tokens[i+2].value;
			string atom = code_tokens[i+3].value;
			string label = code_tokens[i+5].value;
			output << "\n";
			if (isspill(id1)) {
				stack_read(id1, func);
				id1 = "t8";
			}
			if (isspill(atom)) {
				stack_read(atom, "t9", func);
				atom = "t9";
			}
			if (!isdigit(atom[0]) && atom[0] != '-') atom = "$" + atom;
			id1 = "$" + id1;
			if (op == ">") output << "bgt " << id1 << ", " << atom << ", " << label << "\n";
			else if (op == ">=") output << "bge " << id1 << ", " << atom << ", " << label << "\n";
			else if (op == "<") output << "blt " << id1 << ", " << atom << ", " << label << "\n";
			else if (op == "<=") output << "ble " << id1 << ", " << atom << ", " << label << "\n";
			else if (op == "!=") output << "bne " << id1 << ", " << atom << ", " << label << "\n";
			else output << "beq " << id1 << ", " << atom << ", " << label << "\n";
			output << "\n";

			i += 5;
		}
		else if (code_tokens[i].tag == MipsTag::GOTO) {
			string label = code_tokens[i+1].value;
			output << "j " << label << "\n";

			i += 1;
		}
		else if (code_tokens[i].tag == MipsTag::MEM) {
			string id1 = code_tokens[i+2].value;
			string id2 = code_tokens[i+5].value;
			output << "\n";
			if (isspill(id1)) {
				stack_read(id1, func);
				id1 = "t8";
			}
			if (isspill(id2)) {
				stack_read(id2, "t9", func);
				id2 = "t9";
			}
			id1 = "$" + id1;
			if (code_tokens[i+5].tag == MipsTag::NUM) {
				output << "li $t9, " << id2 << "\n";
				output << "sw $t9" << ", " << "0(" << id1 << ")\n\n";
			}
			else
				output << "sw $" << id2 << ", " << "0(" << id1 << ")\n\n";

			i += 5;
		}
		else if (code_tokens[i].tag == MipsTag::ID && 
						 code_tokens[i+2].tag == MipsTag::MEM) {
			string id = code_tokens[i].value;
			string id1 = code_tokens[i+4].value;
			output << "\n";
			if (isspill(id1)) {
				stack_read(id1, func);
				id1 = "t8";
			}
			if (isspill(id)) {
				id1 = "$" + id1;
				output << "lw $t8, 0(" << id1 << ")\n";
				stack_save(id, func); 
			}
			else {
				id1 = "$" + id1;
				id = "$" + id;
				output << "lw " << id << ", 0(" << id1 << ")\n";
			}
			output << "\n";

			i += 5;
		}
		else if (code_tokens[i].tag == MipsTag::CALL) {
			int offset = 4 * (1 + 4 + 8);
			output << "\nsub $sp, $sp, " << offset << "\n";
			output << "sw $ra, 0($sp)\n";
			output << "sw $a0, 4($sp)\n";
			output << "sw $a1, 8($sp)\n";
			output << "sw $a2, 12($sp)\n";
			output << "sw $a3, 16($sp)\n";
			output << "sw $t0, 20($sp)\n";
			output << "sw $t1, 24($sp)\n";
			output << "sw $t2, 28($sp)\n";
			output << "sw $t3, 32($sp)\n";
			output << "sw $t4, 36($sp)\n";
			output << "sw $t5, 40($sp)\n";
			output << "sw $t6, 44($sp)\n";
			output << "sw $t7, 48($sp)\n\n";

			i += 1;

			string call_f = code_tokens[i].value;
			if (table[call_f].spill_a > 0) {
				offset = 4 * table[call_f].spill_a;
				output << "sub $sp, $sp, " << offset << "\n";
			}
			int count = 0;
			while ((code_tokens[i+1].tag == MipsTag::ID || code_tokens[i+1].tag == MipsTag::NUM) &&
						 code_tokens[i+2].tag != MipsTag::ASSIGN && 
						 code_tokens[i+2].tag != MipsTag::COLON) {
				i += 1;
				string arg = code_tokens[i].value; 
				if (isspill(arg)) {
					offset = 4 * (1 + 4 + 8) + 4 * table[call_f].spill_a;
					stack_read(arg, func, offset);
					arg = "t8";
				}
				arg = "$" + arg;
				if (count < 4)
					if (code_tokens[i].tag == MipsTag::NUM)
						output << "li $a" << count << ", " << code_tokens[i].value << "\n";
					else
						output << "move $a" << count << ", " << arg << "\n";
				else
					if (code_tokens[i].tag == MipsTag::NUM) {
						output << "li $t8" << ", " << code_tokens[i].value << "\n";
						output << "sw $t8" << ", " << (count - 4) * 4 << "($sp)\n";
					}
					else
						output << "sw " << arg << ", " << (count - 4) * 4 << "($sp)\n";
				count += 1;
			}

			output << "\njal __" << call_f << "\n\n";

			if (table[call_f].spill_a > 0) {
				offset = 4 * table[call_f].spill_a;
				output << "add $sp, $sp, " << offset << "\n";
			}

			output << "\nlw $ra, 0($sp)\n";
			output << "lw $a0, 4($sp)\n";
			output << "lw $a1, 8($sp)\n";
			output << "lw $a2, 12($sp)\n";
			output << "lw $a3, 16($sp)\n";
			output << "lw $t0, 20($sp)\n";
			output << "lw $t1, 24($sp)\n";
			output << "lw $t2, 28($sp)\n";
			output << "lw $t3, 32($sp)\n";
			output << "lw $t4, 36($sp)\n";
			output << "lw $t5, 40($sp)\n";
			output << "lw $t6, 44($sp)\n";
			output << "lw $t7, 48($sp)\n";
			offset = 4 * (1 + 4 + 8);
			output << "add $sp, $sp, " << offset << "\n\n";
		}
		else if (code_tokens[i].tag == MipsTag::PRINT && 
			       code_tokens[i+1].tag != MipsTag::STR &&
			       code_tokens[i+2].tag == MipsTag::ID) {
			string id1 = code_tokens[i+2].value;
			output << "\nsub $sp, $sp, 8\n";
			output << "sw $a0, 0($sp)\n";
			output << "sw $v0, 4($sp)\n";
			if (isspill(id1)) {
				stack_read(id1, func, 8);
				id1 = "t8";
			}
			id1 = "$" + id1;
			output << "move $a0, " << id1 << "\n";
			if (code_tokens[i+1].tag == MipsTag::INT)
				output << "li $v0, 1\n";
			else // MIpsTag::CHR
				output << "li $v0, 11\n";
			output << "syscall\n\n";
			output << "lw $a0, 0($sp)\n";
			output << "lw $v0, 4($sp)\n";
			output << "add $sp, $sp, 8\n\n";

			i += 2;
		}
		else if (code_tokens[i].tag == MipsTag::PRINT && 
			       (code_tokens[i+1].tag == MipsTag::CHR || code_tokens[i+1].tag == MipsTag::INT) &&
			       code_tokens[i+2].tag == MipsTag::NUM) {
			string num = code_tokens[i+2].value;
			output << "\nsub $sp, $sp, 8\n";
			output << "sw $a0, 0($sp)\n";
			output << "sw $v0, 4($sp)\n";
			output << "li $a0, " << num << "\n";
			if (code_tokens[i+1].tag == MipsTag::INT)
				output << "li $v0, 1\n";
			else
				output << "li $v0, 11\n";
			output << "syscall\n\n";
			output << "lw $a0, 0($sp)\n";
			output << "lw $v0, 4($sp)\n";
			output << "add $sp, $sp, 8\n\n";

			i += 2;
		}
		else if (code_tokens[i].tag == MipsTag::PRINT) {
			string id1 = code_tokens[i+2].value;
			output << "\nsub $sp, $sp, 8\n";
			output << "sw $a0, 0($sp)\n";
			output << "sw $v0, 4($sp)\n";
			output << "la $a0, " << id1 << "\n";
			output << "li $v0, 4\n";
			output << "syscall\n";
			output << "lw $a0, 0($sp)\n";
			output << "lw $v0, 4($sp)\n";
			output << "add $sp, $sp, 8\n\n";

			i += 2;
		}
		else if (code_tokens[i].tag == MipsTag::READ) {
			string id = code_tokens[i+2].value;
			output << "\nsub $sp, $sp, 4\n";
			output << "sw $v0, 0($sp)\n";
			if (code_tokens[i+1].tag == MipsTag::INT)
				output << "li $v0, 5\n";
			else
				output << "li $v0, 12\n";
			output << "syscall\n\n";
			if (isspill(id)) {
				output << "move $t8, $v0\n";
				stack_save(id, func, 4);
			}
			else {
				id = "$" + id;
				output << "move " << id << ", $v0\n";  
			}
			output << "lw $v0, 0($sp)\n";
			output << "add $s0, $sp, 4\n\n";

			i += 2;
		}
		else if (code_tokens[i].tag == MipsTag::RET) {
			output << "\nlw $s0, 0($sp)\n";
			output << "lw $s1, 4($sp)\n";
			output << "lw $s2, 8($sp)\n";
			output << "lw $s3, 12($sp)\n";
			output << "lw $s4, 16($sp)\n";
			output << "lw $s5, 20($sp)\n";
			output << "lw $s6, 24($sp)\n";
			output << "lw $s7, 28($sp)\n";
			int offset = 4 * (8 + table[func].spill_s + table[func].spill_t);
			output << "add $sp, $sp, " << offset << "\n";
			output << "jr $ra\n\n";
		}
	}
	output.close();

	postprocess();
}


static bool isspill(string id)
{
	if (id[0] == 'g') return true;
	if (id[0] == 's' && stoi(id.substr(1)) >= 8) return true;
	if (id[0] == 't' && stoi(id.substr(1)) >= 8) return true;
	if (id[0] == 'a' && stoi(id.substr(1)) >= 4) return true;
	return false;
}


static void stack_read(string id, string reg, string func, int offset)
{
	if (id[0] == 'g') {
		if (global_arrays.count(id) > 0) {
			reg = "$" + reg;
			output << "la " << reg << ", " << id << "\n";
		}
		else {
			reg = "$" + reg;
			output << "lw " << reg << ", " << id << "\n";
		}
		return;
	}

	int index = stoi(id.substr(1));
	if (id[0] == 'a') 
		offset += 4 * (8 + table[func].spill_s + table[func].spill_t + index - 4);
	else if (id[0] == 's')
		offset += 4 * (8 + index - 8);
	else // 't'
		offset += 4 * (8 + table[func].spill_s + index - 8);
	reg = "$" + reg;
	output << "lw " << reg << ", " << offset << "($sp)\n";
}


static void stack_read(string id, string reg, string func)
{
	stack_read(id, reg, func, 0);
}


static void stack_read(string id, string func)
{
	stack_read(id, "t8", func, 0);
}


static void stack_read(string id, string func, int offset)
{
	stack_read(id, "t8", func, offset);
}


static void stack_save(string id, string func, int offset)
{
	stack_save(id, "t8", func, offset);
}


static void stack_save(string id, string func)
{
	stack_save(id, "t8", func, 0);
}


static void stack_save(string id, string reg, string func)
{
	stack_save(id, reg, func, 0);
}


static void stack_save(string id, string reg, string func, int offset)
{
	if (id[0] == 'g') {
		reg = "$" + reg;
		output << "sw " << reg << ", " << id << "\n";
		return;
	}

	int index = stoi(id.substr(1));
	if (id[0] == 'a') 
		offset += 4 * (8 + table[func].spill_s + table[func].spill_t + index - 4);
	else if (id[0] == 's')
		offset += 4 * (8 + index - 8);
	else // 't'
		offset += 4 * (8 + table[func].spill_s + index - 8);
	reg = "$" + reg;
	output << "sw " << reg << ", " << offset << "($sp)\n";
}


static void postprocess()
{
	ifstream data_area, text_area;
	data_area.open("data_area.txt");
	text_area.open("code_area.txt");

	output.open("mips.txt");
	string line;
	output << ".data\n";
	while (getline(data_area, line))
		output << line << "\n";
	output << "\n.text\n";
	output << "jal __main\n";
	output << "\nli $v0, 10\n";
	output << "syscall\n\n";
	while (getline(text_area, line))
		output << line << "\n";
	output.close();

	data_area.close();
	text_area.close();
	
	#ifndef DEBUG
		remove("syntaxtree.txt");
		remove("code.txt");
		remove("data.txt");
		remove("data_area.txt");
		remove("code_area.txt");
	#endif
}


static void preprocess()
{
	for (int i = 0; i < code_tokens.size(); i++) {
		if (code_tokens[i].tag == MipsTag::FUNC) {
			i += 1;
			int index_s = 0; // the max index of s_i
			int index_t = 0; // the max index of t_i
			string func = code_tokens[i].value;
			i += 1;
			while (1) {
				if (!(i+1 < code_tokens.size() && code_tokens[i+1].tag != MipsTag::FUNC))
					break;
				if (code_tokens[i].tag == MipsTag::ID) {
					string id = code_tokens[i].value;
					if (id[0] == 's' && index_s < stoi(id.substr(1)))
						index_s = stoi(id.substr(1));
					if (id[0] == 't' && index_t < stoi(id.substr(1)))
						index_t = stoi(id.substr(1));
				}
				i += 1;

				if (code_tokens[i].tag == MipsTag::CALL) {
					int count_a = 0; // the numbers of a_i
					i += 1; // CALL
					string call_f = code_tokens[i].value;
					i += 1; // FUNC_NAME
					while ((code_tokens[i].tag == MipsTag::ID || code_tokens[i].tag == MipsTag::NUM) &&
								 code_tokens[i+1].tag != MipsTag::ASSIGN && 
								 code_tokens[i+1].tag != MipsTag::COLON) {
						i += 1;
						count_a++;
						if (code_tokens[i].tag == MipsTag::ID) {
							string id = code_tokens[i].value;
							if (id[0] == 's' && index_s < stoi(id.substr(1)))
								index_s = stoi(id.substr(1));
							if (id[0] == 't' && index_t < stoi(id.substr(1)))
								index_t = stoi(id.substr(1));
						}
					}
					if (count_a > 4)
						table[call_f].spill_a = count_a - 4;
					else
						table[call_f].spill_a = 0;
				}
			}
			if (index_s > 7)
				table[func].spill_s = index_s - 7;
			else
				table[func].spill_s = 0;
			if (index_t > 7)
				table[func].spill_t = index_t - 7;
			else
				table[func].spill_t = 0;
		}
	}


	#ifdef DEBUG
		for (auto f : table) 
			cout << f.first << "\t\t" << " spill_s: " << f.second.spill_s
																<< " spill_t: " << f.second.spill_t
																<< " spill_a: " << f.second.spill_a
																<< endl;
	#endif
}


// int main(int argc, char const *argv[])
// {
// 	mips();
// 	return 0;
// }