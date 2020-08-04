#include "old_utils.h"
#include "old_lexer.h"

using namespace std;

vector<Token> old_tokens;
set<string> keys;
extern ofstream errorLog;
static int lines = 1;


static void error()
{
	// to do
}


void printTokens()
{
  for (int i = 0; i < old_tokens.size(); i++) {
    Tag tag = old_tokens[i].tag;
    string value = old_tokens[i].value;
    switch (tag) {
      case IDENFR: cout << "IDENFR " << value << endl; break;
      case INTCON: cout << "INTCON " << value << endl; break;
      case CHARCON: cout << "CHARCON " << value << endl; break;
      case STRCON: cout << "STRCON " << value << endl; break;
      case CONSTTK: cout << "CONSTTK " << value << endl; break;
      case INTTK: cout << "INTTK " << value << endl; break;
      case CHARTK: cout << "CHARTK " << value << endl; break;
      case VOIDTK: cout << "VOIDTK " << value << endl; break;
      case MAINTK: cout << "MAINTK " << value << endl; break;
      case IFTK: cout << "IFTK " << value << endl; break;
      case ELSETK: cout << "ELSETK " << value << endl; break;
      case DOTK: cout << "DOTK " << value << endl; break;
      case WHILETK: cout << "WHILETK " << value << endl; break;
      case FORTK: cout << "FORTK " << value << endl; break;
      case SCANFTK: cout << "SCANFTK " << value << endl; break;
      case PRINTFTK: cout << "PRINTFTK " << value << endl; break;
      case RETURNTK: cout << "RETURNTK " << value << endl; break;
      case PLUS: cout << "PLUS " << value << endl; break;
      case MINU: cout << "MINU " << value << endl; break;
      case MULT: cout << "MULT " << value << endl; break;
      case DIV: cout << "DIV " << value << endl; break;
      case LSS: cout << "LSS " << value << endl; break;
      case LEQ: cout << "LEQ " << value << endl; break;
      case GRE: cout << "GRE " << value << endl; break;
      case GEQ: cout << "GEQ " << value << endl; break;
      case EQL: cout << "EQL " << value << endl; break;
      case NEQ: cout << "NEQ " << value << endl; break;
      case ASSIGN: cout << "ASSIGN " << value << endl; break;
      case SEMICN: cout << "SEMICN " << value << endl; break;
      case COMMA: cout << "COMMA " << value << endl; break;
      case LPARENT: cout << "LPARENT " << value << endl; break;
      case RPARENT: cout << "RPARENT " << value << endl; break;
      case LBRACK: cout << "LBRACK " << value << endl; break;
      case RBRACK: cout << "RBRACK " << value << endl; break;
      case LBRACE: cout << "LBRACE " << value << endl; break;
      case RBRACE: cout << "RBRACE " << value << endl; break;
      case ERROR: cout << "ERROR " << value << endl; break;
      case NEWLINE: cout << "NEWLINE " << value << endl;
    }
  }
  cout << "\n";
}


void old_lexer(string str)
{
	for (string::iterator it = str.begin(); it != str.end(); it++) {
    if (*it == '\n') {
      old_tokens.push_back(*new Token(NEWLINE, ""));
      lines++;
    }
		else if (isspace(*it)) ;
		else if (isdigit(*it)) {
			string digits = "";
			while (1) {
				digits.push_back(*it);
				if (isdigit(*(it+1))) {
					it++;
				} else {
					break;
				}
			}

			old_tokens.push_back(*new Token(INTCON, digits));
		}
		else if (isalpha(*it) || *it == '_') {
			string name = "";
			while (1) {
				name.push_back(*it);
				if (isalnum(*(it+1)) || *(it+1) == '_') {
					it++;
				} else {
					break;
				}
			}

			if 	    (name == "const")   old_tokens.push_back(*new Token(CONSTTK, name));
			else if (name == "int")     old_tokens.push_back(*new Token(INTTK, name));
			else if (name == "char")    old_tokens.push_back(*new Token(CHARTK, name));
			else if (name == "void")    old_tokens.push_back(*new Token(VOIDTK, name));
			else if (name == "main")    old_tokens.push_back(*new Token(MAINTK, name));
			else if (name == "if") 		old_tokens.push_back(*new Token(IFTK, name));
			else if (name == "else")   	old_tokens.push_back(*new Token(ELSETK, name));
			else if (name == "do")      old_tokens.push_back(*new Token(DOTK, name));
			else if (name == "while") 	old_tokens.push_back(*new Token(WHILETK, name));
			else if (name == "for")     old_tokens.push_back(*new Token(FORTK, name));
			else if (name == "scanf")   old_tokens.push_back(*new Token(SCANFTK, name));
			else if (name == "printf")  old_tokens.push_back(*new Token(PRINTFTK, name));
			else if (name == "return")  old_tokens.push_back(*new Token(RETURNTK, name));
			else	 					old_tokens.push_back(*new Token(IDENFR, name));
		}
		else if (*it == '"') {
			string s = "";
			while (1) {
				it++;
        if (*it == '\n') {
          errorLog << lines << " a\n";
          it -= 1;
          break;
        }
				else if (*it != '"') {
					s.push_back(*it);
				} else {
          old_tokens.push_back(*new Token(STRCON, s));
					break;
				}
			}

		}
		else if (*it == '\'') {
			string s = "";
			s.push_back(*(++it));
			if (*(++it) != '\'') {
				errorLog << lines << " a\n";
        it--;
			}
      else 
			  old_tokens.push_back(*new Token(CHARCON, s));
		}
		else if (*it == '>') {
			if (*(it+1) == '=') {
				it++;
				old_tokens.push_back(*new Token(GEQ, ">="));
			} else {
				old_tokens.push_back(*new Token(GRE, ">"));
			}
		}
		else if (*it == '<') {
			if (*(it+1) == '=') {
				it++;
				old_tokens.push_back(*new Token(LEQ, "<="));
			} else {
				old_tokens.push_back(*new Token(LSS, "<"));
			}
		}
		else if (*it == '=') {
			if (*(it+1) == '=') {
				it++;
				old_tokens.push_back(*new Token(EQL, "=="));
			} else {
				old_tokens.push_back(*new Token(ASSIGN, "="));
			}
		}
		else if (*it == '!') {
			if (*(it+1) == '=') {
				it++;
				old_tokens.push_back(*new Token(NEQ, "!="));
			} else {
				error();
			}
		}
		else if (*it == '+') old_tokens.push_back(*new Token(PLUS, "+"));
		else if (*it == '-') old_tokens.push_back(*new Token(MINU, "-"));
		else if (*it == '*') old_tokens.push_back(*new Token(MULT, "*"));		
		else if (*it == '/') old_tokens.push_back(*new Token(DIV, "/"));
		else if (*it == '(') old_tokens.push_back(*new Token(LPARENT, "("));
		else if (*it == ')') old_tokens.push_back(*new Token(RPARENT, ")"));
		else if (*it == '[') old_tokens.push_back(*new Token(LBRACK, "["));
		else if (*it == ']') old_tokens.push_back(*new Token(RBRACK, "]"));
		else if (*it == '{') old_tokens.push_back(*new Token(LBRACE, "{"));
		else if (*it == '}') old_tokens.push_back(*new Token(RBRACE, "}"));
		else if (*it == ';') old_tokens.push_back(*new Token(SEMICN, ";"));
		else if (*it == ',') old_tokens.push_back(*new Token(COMMA, ","));
    else
      errorLog << lines << " a\n";
	}
}