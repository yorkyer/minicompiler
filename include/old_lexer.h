#ifndef __OLD_LEXER
#define __OLD_LEXER


enum Tag
{
    IDENFR, INTCON, CHARCON, STRCON, CONSTTK, INTTK, CHARTK, VOIDTK, MAINTK,
    IFTK, ELSETK, DOTK, WHILETK, FORTK, SCANFTK, PRINTFTK, RETURNTK, PLUS,
    MINU, MULT, DIV, LSS, LEQ, GRE, GEQ, EQL, NEQ,
    ASSIGN, SEMICN, COMMA, LPARENT, RPARENT, LBRACK, RBRACK, LBRACE, RBRACE,
    NEWLINE, ERROR
};


class Token
{
public:
    Tag tag;
    std::string value;

  
    Token(Tag t, std::string v) : tag(t), value(v) {}
};


void old_lexer(std::string str);
void printTokens();


#endif