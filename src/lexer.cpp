#include "utils.h"
#include "lexer.h"


vector<Token> tokens;


void lexer(string file);


void lexer(string file)
{
	ifstream input(file);
	char c;
	while (input.get(c)) {
		if (c == '\n') tokens.push_back(Token(TokenTag::LINEEND, ""));
		else if (isspace(c)) ;
		else if (isdigit(c)) {
			string num;
			while (1) {
				num += c;
				if (isdigit(input.peek())) {
					input.get(c);
				} else {
					break;
				}
			}
			tokens.push_back(Token(TokenTag::NUM, num));
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

			if 			(id == "const")		tokens.push_back(Token(TokenTag::CONST, id));
			else if (id == "int")			tokens.push_back(Token(TokenTag::INT, id));
			else if (id == "char")		tokens.push_back(Token(TokenTag::CHAR, id));
			else if (id == "void")		tokens.push_back(Token(TokenTag::VOID, id));
			else if (id == "if")			tokens.push_back(Token(TokenTag::IF, id));
			else if (id == "else")		tokens.push_back(Token(TokenTag::ELSE, id));
			else if (id == "do")			tokens.push_back(Token(TokenTag::DO, id));
			else if (id == "while")		tokens.push_back(Token(TokenTag::WHILE, id));
			else if (id == "for")			tokens.push_back(Token(TokenTag::FOR, id));
			else if (id == "scanf")		tokens.push_back(Token(TokenTag::SCANF, id));
			else if (id == "printf")	tokens.push_back(Token(TokenTag::PRINTF, id));
			else if (id == "return")	tokens.push_back(Token(TokenTag::RETURN, id));
			else											tokens.push_back(Token(TokenTag::ID, id));
		}
		else if (c == '"') {
			string str = "";
			while (1) {
				if (input.get(c) && c != '"') {
					str += c;
				} else {
					break;
				}
			}
			tokens.push_back(Token(TokenTag::STR, str));
		}
		else if (c == '\'') {
			string chr = "";
			input.get(c);
			chr += c;
			input.get(c);
			tokens.push_back(Token(TokenTag::CHR, chr));
		}
		else if (c == '>') {
			if (input.peek() == '=') {
				input.get(c);
				tokens.push_back(Token(TokenTag::GE, ">="));
			} else {
				tokens.push_back(Token(TokenTag::GT, ">"));
			}
		}
		else if (c == '<') {
			if (input.peek() == '=') {
				input.get(c);
				tokens.push_back(Token(TokenTag::LE, "<="));
			} else {
				tokens.push_back(Token(TokenTag::LT, "<"));
			}
		}
		else if (c == '=') {
			if (input.peek() == '=') {
				input.get(c);
				tokens.push_back(Token(TokenTag::EQ, "=="));
			} else {
				tokens.push_back(Token(TokenTag::ASSIGN, "="));
			}
		}
		else if (c == '!') {
			input.get(c);
			tokens.push_back(Token(TokenTag::NE, "!="));
		}
		else if (c == ';') tokens.push_back(Token(TokenTag::SEMICN, ";"));
		else if (c == ',') tokens.push_back(Token(TokenTag::COMMA, ","));
		else if (c == '+') tokens.push_back(Token(TokenTag::ADD, "+"));
		else if (c == '-') tokens.push_back(Token(TokenTag::SUB, "-"));
		else if (c == '*') tokens.push_back(Token(TokenTag::MUL, "*"));		
		else if (c == '/') tokens.push_back(Token(TokenTag::DIV, "/"));
		else if (c == '(') tokens.push_back(Token(TokenTag::LPARENT, "("));
		else if (c == ')') tokens.push_back(Token(TokenTag::RPARENT, ")"));
		else if (c == '[') tokens.push_back(Token(TokenTag::LBRACK, "["));
		else if (c == ']') tokens.push_back(Token(TokenTag::RBRACK, "]"));
		else if (c == '{') tokens.push_back(Token(TokenTag::LBRACE, "{"));
		else if (c == '}') tokens.push_back(Token(TokenTag::RBRACE, "}"));
	}
	input.close();

#ifdef DEBUG
	ofstream output;
	output.open("token.txt");
	for (auto t : tokens)
			output << t.value << "\n";
	output.close();
#endif
}