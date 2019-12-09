#ifndef LEXER_H
#define LEXER_H


enum class TokenTag
{
	CONST, INT, CHAR, VOID, IF, ELSE, DO, WHILE, FOR, SCANF, PRINTF, RETURN,
	ID, NUM, CHR, STR,
	ADD, SUB, MUL, DIV, LT, LE, GT, GE, EQ, NE,
	ASSIGN, SEMICN, COMMA, LPARENT, RPARENT, LBRACK, RBRACK, LBRACE, RBRACE,
	LINEEND
};
const vector<string> TokenTagName = { "const", "int", "char", "void",
		"if", "else", "do", "while", "for", "scanf", "printf", "return", 
		"id", "num", "chr", "str", 
		"+", "-", "*", "/", "<", "<=", ">", ">=", "==", "!=",
		"=", ";", ",", "(", ")", "[", "]", "{", "}", "\n"};


class Token
{
public:
	TokenTag tag;
	string value;

	Token(TokenTag t, string v) : tag(t), value(v) {}
};


void lexer(int argc, char *argv[]);


#endif