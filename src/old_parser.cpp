#include "old_utils.h"
#include "old_lexer.h"
#include "old_symtable.h"

using namespace std;

extern vector<Token> old_tokens;
extern int errorLines;
int current = 0;
int numLines = 1;

SymTable symTable;

ofstream errorLog;


static void error(Tag tag)
{
  switch (tag) {
    case IDENFR: cout << "IDENFR lackness error"; break;
    case INTCON: cout << "INTCON lackness error"; break;
    case CHARCON: cout << "CHARCON lackness error"; break;
    case STRCON: cout << "STRCON lackness error"; break;
    case CONSTTK: cout << "CONSTTK lackness error"; break;
    case INTTK: cout << "INTTK lackness error"; break;
    case CHARTK: cout << "CHARTK lackness error"; break;
    case VOIDTK: cout << "VOIDTK lackness error"; break;
    case MAINTK: cout << "MAINTK lackness error"; break;
    case IFTK: cout << "IFTK lackness error"; break;
    case ELSETK: cout << "ELSETK lackness error"; break;
    case DOTK: cout << "DOTK lackness error"; break;
    case WHILETK: cout << "WHILETK lackness error"; break;
    case FORTK: cout << "FORTK lackness error"; break;
    case SCANFTK: cout << "SCANFTK lackness error"; break;
    case PRINTFTK: cout << "PRINTFTK lackness error"; break;
    case RETURNTK: cout << "RETURNTK lackness error"; break;
    case PLUS: cout << "PLUS lackness error"; break;
    case MINU: cout << "MINU lackness error"; break;
    case MULT: cout << "MULT lackness error"; break;
    case DIV: cout << "DIV lackness error"; break;
    case LSS: cout << "LSS lackness error"; break;
    case LEQ: cout << "LEQ lackness error"; break;
    case GRE: cout << "GRE lackness error"; break;
    case GEQ: cout << "GEQ lackness error"; break;
    case EQL: cout << "EQL lackness error"; break;
    case NEQ: cout << "NEQ lackness error"; break;
    case ASSIGN: cout << "ASSIGN lackness error"; break;
    case SEMICN: cout << "SEMICN lackness error"; break;
    case COMMA: cout << "COMMA lackness error"; break;
    case LPARENT: cout << "LPARENT lackness error"; break;
    case RPARENT: cout << "RPARENT lackness error"; break;
    case LBRACK: cout << "LBRACK lackness error"; break;
    case RBRACK: cout << "RBRACK lackness error"; break;
    case LBRACE: cout << "LBRACE lackness error"; break;
    case RBRACE: cout << "RBRACE lackness error"; break;
    default: 
      cout << "error!\n";
  }
  cout << " at line " << numLines << endl;

  if (tag == SEMICN) {
    int i = 0;
    for (int cur = current - 1; cur >= 0; cur--) {
      if (old_tokens[cur].tag != NEWLINE) {
        break;
      }
      i++;
    }
    errorLog << numLines - i << " k\n";
  }
  else if (tag == RPARENT)
    errorLog << numLines << " l\n";
  else if (tag == RBRACK)
    errorLog << numLines << " m\n";
  else if (tag == WHILETK)
    errorLog << numLines << " n\n"; 
}


bool match(Tag tag)
{
    if (current < old_tokens.size() && old_tokens[current].tag == NEWLINE) {
      numLines++;
      cout << "new line: " << numLines << endl; 
      current++; 
      return match(tag);
    }
    else if (current < old_tokens.size())
      return old_tokens[current].tag == tag;
    else
      return false;
}


bool next(Tag tag)
{
    return old_tokens[current+1].tag == tag;
}


string eat(Tag tag)
{
  if (current < old_tokens.size() && old_tokens[current].tag == NEWLINE) {
    numLines++; 
    cout << "new line: " << numLines << endl; 
    current++; 
    return eat(tag);
  } 
  if (tag == IDENFR && match(INTCON)) {
    cout << "error identifier at line " << numLines << endl;
    errorLog << numLines << " a\n";
    current++;
    return eat(tag);
  }
  if (current >= old_tokens.size() || old_tokens[current].tag != tag) {
    error(tag);
    return "";
  }
  string value = old_tokens[current].value;
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
    case ERROR: cout << "ERROR " << endl; break; 
    default: 
      cout << "error!\n";
  }
  current++;
  return value;
}


void program();
void constDecl();
void constDef();
void unsign();
void num();
void add();
void varDecl();
void varDef();
Type type();
void func();
void proc();
void declHead();
vector<Type> args();
void funcBlock(Type type);
void procBlock();
void block();
void stmts(Type type, bool &hasReturn);
void stmts();
void stmt(Type type, bool &hasReturn);
void stmt();
void _bool();
Type expr();
Type term();
Type factor();
void mul();
void relation();
void assign();
void read();
void write();
void str();
void _return(Type type, bool &hasReturn);
void _return();
vector<Type> values();
void _main();
void checkFuncCallArgs(vector<Type> args, vector<Type> values);
void funcCall(vector<Type> args);
void procCall(vector<Type> args);
void _if(Type type, bool &hasReturn);
void _if();
void loop();
void loop(Type type, bool &hasReturn);
void step();
void checkCharacter(char c);


void program()
{
    if (match(CONSTTK))
        constDecl();
    if ((match(INTTK) || match(CHARTK)) && old_tokens[current+2].tag != LPARENT)
        varDecl();

    while (match(INTTK) || match(CHARTK) ||
          (match(VOIDTK) && next(IDENFR))) {
        if (match(INTTK) || match(CHARTK)) {
            func();
        } else {
            proc();
        }
    }

    _main();
    cout << "<程序>" << endl;     
}


void constDecl()
{
    do {
        eat(CONSTTK);
        constDef();
        eat(SEMICN);
    } while (match(CONSTTK));
    cout << "<常量说明>" << endl; 
}


void checkCharacter(char c)
{
  if (!(c == '+' || c == '-' || c == '*' || c == '/' || isalnum(c))) {
    cout << "illegal character at line " << numLines << "\n";
    errorLog << numLines << " a\n";
  }
}

void constDef()
{
    if (match(INTTK)) {
        eat(INTTK); 
        string name = eat(IDENFR); 
        errorLines = numLines;
        eat(ASSIGN); 
        if (!(match(PLUS) || match(MINU) || match(INTCON))) {
          cout << "const int error at line " << numLines << "\n";
          errorLog << numLines << " o\n";

          cout << current << endl;
          while (!(match(COMMA) || match(SEMICN))) {
            current++;
            cout << current << endl;
          }
        } else {
          num();
        }

        Attribute attr;
        attr.type = Type::INT;
        attr.qualifier = Qualifier::CONST;
        symTable.addSymbol(name, attr);

        while (match(COMMA)) {
            eat(COMMA); 
            string name = eat(IDENFR); 
            errorLines = numLines;
            eat(ASSIGN); 
            if (!(match(PLUS) || match(MINU) || match(INTCON))) {
              cout << "const int error at line " << numLines << "\n";
              errorLog << numLines << " o\n";

              while (!(match(COMMA) || match(SEMICN))) {
                current++;
              }
            } else
              num();

            Attribute attr;
            attr.type = Type::INT;
            attr.qualifier = Qualifier::CONST;
            symTable.addSymbol(name, attr);
        }
    }
    else if (match(CHARTK)) {
        eat(CHARTK); 
        string name = eat(IDENFR); 
        errorLines = numLines;
        eat(ASSIGN);  

        if (match(ERROR)) {
          eat(ERROR);
          cout << "token error at line " << numLines << "\n";
          errorLog << numLines << " a\n";
        }
        else if (!match(CHARCON)) {
          cout << "const char error at line " << numLines << "\n";
          errorLog << numLines << " o\n";
          while (!(match(COMMA) || match(SEMICN))) {
            current++;
          }
        }
        else {
          string s = eat(CHARCON);  
          checkCharacter(s[0]);
        }

        Attribute attr;
        attr.type = Type::CHAR;
        attr.qualifier = Qualifier::CONST;
        symTable.addSymbol(name, attr); 

        while (match(COMMA)) {
            eat(COMMA); 
            string name = eat(IDENFR);
            errorLines = numLines;
            eat(ASSIGN); 
            if (match(ERROR)) {
              eat(ERROR);
              cout << "token error at line " << numLines << "\n";
              errorLog << numLines << " a\n";
            }
            else if (!match(CHARCON)) {
              cout << "const char error at line " << numLines << "\n";
              errorLog << numLines << " o\n";
              while (!(match(COMMA) || match(SEMICN))) {
                current++;
              }
            } 
            else {
              string s = eat(CHARCON);  
              checkCharacter(s[0]);
            }

            Attribute attr;
            attr.type = Type::CHAR;
            attr.qualifier = Qualifier::CONST;
            symTable.addSymbol(name, attr); 
        }    
    }
    cout << "<常量定义>" << endl; 
}


void unsign()
{
    string value = eat(INTCON);
    if (value.size() != 1 && value[0] == '0') {
      cout << "illegal int at line " << numLines << "\n";
      errorLog << numLines << " a\n";
    }
    cout << "<无符号整数>" << endl;
}


void num()
{
    if (match(PLUS) || match(MINU)) {
        add();
    }
    unsign();
    cout << "<整数>" << endl; 
}


void add()
{
    if (match(PLUS)) eat(PLUS);
    else             eat(MINU);
}


void varDecl()
{
    do {
        varDef();
        eat(SEMICN); 
    } while ((match(INTTK) || match(CHARTK)) && old_tokens[current+2].tag != LPARENT); 
    cout << "<变量说明>" << endl;   
}


void varDef()
{
  Type t = type();
  string name = eat(IDENFR);
  errorLines = numLines;
  if (match(LBRACK)) {
      eat(LBRACK); 
      if (expr() != Type::INT) {
        cout << "error index is not int at line " << numLines << "\n";
        errorLog << numLines << " i\n";
      }
      eat(RBRACK); 
  }

  Attribute attr;
  attr.type = t;
  attr.qualifier = Qualifier::VOID;
  symTable.addSymbol(name, attr);

  while (match(COMMA)) {
      eat(COMMA); 
      string name = eat(IDENFR);
      errorLines = numLines;
      if (match(LBRACK)) {
          eat(LBRACK);
          if (expr() != Type::INT) {
            cout << "error index is not int at line " << numLines << "\n";
            errorLog << numLines << " i\n";
          }
          eat(RBRACK); 
      }

      Attribute attr;
      attr.type = t;
      attr.qualifier = Qualifier::VOID;
      symTable.addSymbol(name, attr);
  }

  cout << "<变量定义>" << endl;
}


Type type()
{
    if (match(INTTK)) {
      eat(INTTK);
      return Type::INT;
    }
    else {
      eat(CHARTK);
      return Type::CHAR;
    }
}


void func()
{
    
    Type t = type();

    string name = eat(IDENFR);
    Attribute attr = Attribute(t, Qualifier::FUNC);
    symTable.addSymbol(name, attr);

    errorLines = numLines;
    eat(LPARENT);

    symTable.enterScope();

    vector<Type> as = args();

    eat(RPARENT);

    symTable.getSymbol(name).args = as;

    eat(LBRACE);
    funcBlock(t);
    eat(RBRACE);
    cout << "<有返回值函数定义>" << endl;

    symTable.exitScope();

    
}


void proc()
{
    eat(VOIDTK);

    string name = eat(IDENFR);
    Attribute attr = Attribute(Type::VOID, Qualifier::FUNC);
    symTable.addSymbol(name, attr);

    errorLines = numLines;
    eat(LPARENT);

    symTable.enterScope();

    vector<Type> as = args();
    eat(RPARENT);

    symTable.getSymbol(name).args = as;

    eat(LBRACE);
    procBlock();
    eat(RBRACE);
    cout << "<无返回值函数定义>" << endl;

    symTable.exitScope();
}


// void declHead()
// {
//     type();
//     eat(IDENFR);
//     cout << "<声明头部>" << endl; 
// }


vector<Type> args()
{
    vector<Type> as;
    if (!match(RPARENT)) {
        Type t = type();
        string name = eat(IDENFR);

        Attribute attr;
        attr.type = t;
        attr.qualifier = Qualifier::VOID;
        symTable.addSymbol(name, attr);

        as.push_back(t);

        while (match(COMMA)) {
            eat(COMMA);
            Type t = type();
            string name = eat(IDENFR);

            Attribute attr;
            attr.type = t;
            attr.qualifier = Qualifier::VOID;
            symTable.addSymbol(name, attr);

            as.push_back(t);
        }
    }
    cout << "<参数表>" << endl; 
    return as;
}


void funcBlock(Type t)
{
    if (match(CONSTTK))
        constDecl();
    if (match(INTTK) || match(CHARTK))
        varDecl();
    bool hasReturn = false;
    stmts(t, hasReturn);
    if (!hasReturn) {
      cout << "function has no return at line " << numLines << "\n";
      errorLog << numLines << " h\n";
    }
    cout << "<复合语句>" << endl; 
}


void procBlock()
{
    if (match(CONSTTK))
        constDecl();
    if (match(INTTK) || match(CHARTK))
        varDecl();
    bool hasReturn = false;
    stmts(Type::VOID, hasReturn);
    cout << "<复合语句>" << endl; 
}


void block()
{
    symTable.enterScope();
    if (match(CONSTTK))
        constDecl();
    if (match(INTTK) || match(CHARTK))
        varDecl();
    stmts();
    cout << "<复合语句>" << endl; 
    symTable.exitScope();
}


void stmts()
{
    while (match(IFTK) || match(WHILETK) ||
           match(DOTK) || match(FORTK) ||
           match(LBRACE) || match(IDENFR) ||
           match(SCANFTK) || match(PRINTFTK) ||
           match(SEMICN) || match(RETURNTK)) {
        stmt();
    }
    cout << "<语句列>" << endl;
}

void stmts(Type type, bool &hasReturn)
{
    while (match(IFTK) || match(WHILETK) ||
           match(DOTK) || match(FORTK) ||
           match(LBRACE) || match(IDENFR) ||
           match(SCANFTK) || match(PRINTFTK) ||
           match(SEMICN) || match(RETURNTK)) {
        stmt(type, hasReturn);
    }
    cout << "<语句列>" << endl;
}


void _if()
{
    eat(IFTK);
    eat(LPARENT);
    _bool();
    eat(RPARENT);
    stmt();
    if (match(ELSETK)) {
        eat(ELSETK);
        stmt();
    }
    cout << "<条件语句>" << endl; 
}


void _if(Type type, bool &hasReturn)
{
    eat(IFTK);
    eat(LPARENT);
    _bool();
    eat(RPARENT);
    stmt(type, hasReturn);
    if (match(ELSETK)) {
        eat(ELSETK);
        stmt(type, hasReturn);
    }
    cout << "<条件语句>" << endl; 
}



void step()
{
    unsign();
    cout << "<步长>" << endl;
}


void loop()
{
    if (match(DOTK)) {
        eat(DOTK);
        stmt();
        eat(WHILETK);
        eat(LPARENT);
        _bool();
        eat(RPARENT);
    } 
    else if (match(WHILETK)) {
        eat(WHILETK);
        eat(LPARENT);
        _bool();
        eat(RPARENT);
        stmt();
    }
    else if (match(FORTK)) {
        eat(FORTK);
        eat(LPARENT);
        string name = eat(IDENFR);
        if (!symTable.hasSymbol(name)) {
          cout << name << " not defined error at line " << numLines << "\n";
          errorLog << numLines << " c\n";
        }
        eat(ASSIGN);
        expr();
        eat(SEMICN);
        _bool();
        eat(SEMICN);
        name = eat(IDENFR);
        if (!symTable.hasSymbol(name)) {
          cout << name << " not defined error at line " << numLines << "\n";
          errorLog << numLines << " c\n";
        }
        eat(ASSIGN);
        eat(IDENFR);
        add();
        step();
        eat(RPARENT);
        stmt();
    }
    cout << "<循环语句>" << endl; 
}


void loop(Type type, bool &hasReturn)
{
    if (match(DOTK)) {
        eat(DOTK);
        stmt(type, hasReturn);
        eat(WHILETK);
        eat(LPARENT);
        _bool();
        eat(RPARENT);
    } 
    else if (match(WHILETK)) {
        eat(WHILETK);
        eat(LPARENT);
        _bool();
        eat(RPARENT);
        stmt(type, hasReturn);
    }
    else if (match(FORTK)) {
        eat(FORTK);
        eat(LPARENT);
        string name = eat(IDENFR);
        if (!symTable.hasSymbol(name)) {
          cout << name << " not defined error at line " << numLines << "\n";
          errorLog << numLines << " c\n";
        }
        eat(ASSIGN);
        expr();
        eat(SEMICN);
        _bool();
        eat(SEMICN);
        name = eat(IDENFR);
        if (!symTable.hasSymbol(name)) {
          cout << name << " not defined error at line " << numLines << "\n";
          errorLog << numLines << " c\n";
        }
        eat(ASSIGN);
        eat(IDENFR);
        add();
        step();
        eat(RPARENT);
        stmt(type, hasReturn);
    }
    cout << "<循环语句>" << endl; 
}


void stmt()
{
    if (match(IFTK)) {
        _if();
    } 
    else if (match(DOTK) || match(WHILETK) || match(FORTK)) {
        loop();
    }
    else if (match(LBRACE)) {
        eat(LBRACE);
        stmts();
        eat(RBRACE);
    }
    else if ((match(IDENFR) && next(ASSIGN)) ||
             (match(IDENFR) && next(LBRACK))) {
        assign();
        eat(SEMICN);
    }
    else if (match(SCANFTK)) {
        read();
        eat(SEMICN);
    }
    else if (match(PRINTFTK)) {
        write();
        eat(SEMICN);
    } 
    else if (match(RETURNTK)) {
        _return();
        eat(SEMICN);
    }
    else if (match(SEMICN)) {
        eat(SEMICN);
    }
    else {
        string name = eat(IDENFR);
        vector<Type> args;
        if (!symTable.hasSymbol(name)) {
          cout << name << " not defined error at line " << numLines << "\n";
          errorLog << numLines << " c\n";
          funcCall(args);
        } else {
          Attribute attr = symTable.getSymbol(name);
          args = attr.args;
          if (attr.type == Type::VOID)
            procCall(args);
          else
            funcCall(args);
        }
        eat(SEMICN);
    }
    cout << "<语句>" << endl; 
}


void stmt(Type type, bool &hasReturn)
{
    if (match(IFTK)) {
        _if(type, hasReturn);
    } 
    else if (match(DOTK) || match(WHILETK) || match(FORTK)) {
        loop(type, hasReturn);
    }
    else if (match(LBRACE)) {
        eat(LBRACE);
        stmts(type, hasReturn);
        eat(RBRACE);
    }
    else if ((match(IDENFR) && next(ASSIGN)) ||
             (match(IDENFR) && next(LBRACK))) {
        assign();
        eat(SEMICN);
    }
    else if (match(SCANFTK)) {
        read();
        eat(SEMICN);
    }
    else if (match(PRINTFTK)) {
        write();
        eat(SEMICN);
    } 
    else if (match(RETURNTK)) {
        _return(type, hasReturn);
        eat(SEMICN);
    }
    else if (match(SEMICN)) {
        eat(SEMICN);
    }
    else {
        string name = eat(IDENFR);
        vector<Type> args;
        if (!symTable.hasSymbol(name) ||
             symTable.getSymbol(name).qualifier != Qualifier::FUNC) {
          cout << name << " not defined error at line " << numLines << "\n";
          errorLog << numLines << " c\n";
          funcCall(args);
        } else {
          Attribute attr = symTable.getSymbol(name);
          args = attr.args;
          if (attr.type == Type::VOID)
            procCall(args);
          else
            funcCall(args);
        }
        eat(SEMICN);
    }
    cout << "<语句>" << endl; 
}


void _bool()
{
    if (expr() != Type::INT) {
      cout << "error: the bool expression is not int at line "
                     << numLines << "\n";
      errorLog << numLines << " f\n";
    }
    if (match(LSS) || match(LEQ) ||
        match(GRE) || match(GEQ) ||
        match(EQL) || match(NEQ)) {
        relation();
        if (expr() != Type::INT) {
            cout << "error: the bool expression is not int at line "
                 << numLines << "\n";
            errorLog << numLines << " f\n";
        }
    }
    cout << "<条件>" << endl;
}


Type expr()
{
    if (match(PLUS) || match(MINU)) {
        add();
    }
    Type type = term();
    while (match(PLUS) || match(MINU)) {
        add();
        term();
        type = Type::INT;
    }
    cout << "<表达式>" << endl; 
    return type;
}


Type term()
{
    Type type = factor();
    while (match(MULT) || match(DIV)) {
        mul();
        factor();
        type = Type::INT;
    }
    cout << "<项>" << endl;
    return type; 
}


Type factor()
{
    Type type;
    if (match(IDENFR) && next(LPARENT)) {
        string name = eat(IDENFR);
        vector<Type> as;
        if (!symTable.hasSymbol(name) || 
             symTable.getSymbol(name).qualifier != Qualifier::FUNC) {
          cout << name << " not defined error at line " << numLines << "\n";
          errorLog << numLines << " c\n";
        } else {
          Attribute attr = symTable.getSymbol(name);
          as = attr.args;
          type = attr.type;
        }
        funcCall(as);
    }
    else if (match(CHARCON)) {
        string s = eat(CHARCON);  
        checkCharacter(s[0]);
        type = Type::CHAR;
    }
    else if (match(ERROR)) {
      eat(ERROR);
      cout << "token error at line " << numLines << "\n";
      errorLog << numLines << " a\n";
      type = Type::CHAR;
    }
    else if (match(PLUS) || match(MINU) || match(INTCON)) {
        num();
        type = Type::INT;
    }
    else if (match(LPARENT)) {
        eat(LPARENT);
        type = expr();
        type = Type::INT;
        eat(RPARENT);
    }
    else if (match(IDENFR)) {
        string name = eat(IDENFR);
        if (!symTable.hasSymbol(name)) {
          cout << name << " not defined error at line " << numLines << "\n";
          errorLog << numLines << " c\n";
        } else {
          type = symTable.getSymbol(name).type;
        }
        if (match(LBRACK)) {
            eat(LBRACK);
            if (expr() != Type::INT) {
              cout << "error index is not int at line " << numLines << "\n";
              errorLog << numLines << " i\n";
            }
            eat(RBRACK);
        }
    }
    cout << "<因子>" << endl; 
    return type;
}


void checkFuncCallArgs(vector<Type> args, vector<Type> values)
{
  if (args.size() != values.size()) {
    cout << "the numbers of args error at line " << numLines << "\n";
    errorLog << numLines << " d\n";
  } else {
    for (int i = 0; i < args.size(); i++) {
      if (args[i] != values[i]) {
        cout << "the type of args error at line " << numLines << "\n";
        errorLog << numLines << " e\n";
        break;
      }
    }
  }
}


void funcCall(vector<Type> args)
{
    eat(LPARENT);
    vector<Type> vs = values();
    eat(RPARENT);

    checkFuncCallArgs(args, vs);

    cout << "<有返回值函数调用语句>" << endl; 
}


void procCall(vector<Type> args)
{
    eat(LPARENT);
    vector<Type> vs = values();
    eat(RPARENT);

    checkFuncCallArgs(args, vs);

    cout << "<无返回值函数调用语句>" << endl; 
}


void mul()
{
    if (match(MULT)) eat(MULT);
    else             eat(DIV);
}


void relation()
{
    switch (old_tokens[current].tag) {
        case LSS: eat(LSS); break;
        case LEQ: eat(LEQ); break;
        case GRE: eat(GRE); break;
        case GEQ: eat(GEQ); break;
        case EQL: eat(EQL); break;
        default: eat(NEQ);
    }
}


void assign()
{
    string name = eat(IDENFR);
    if (!symTable.hasSymbol(name)) {
        cout << name << " not defined error at line " << numLines << "\n";
        errorLog << numLines << " c\n";
    } else {
        Attribute attr = symTable.getSymbol(name);
        if (attr.qualifier == Qualifier::CONST) {
          cout << name << " is const, connot be assigned at line " 
               << numLines << "\n";
          errorLog << numLines << " j\n";
        }
    }
    if (match(ASSIGN)) {
        eat(ASSIGN);
        expr();
    }
    else if (match(LBRACK)) {
        eat(LBRACK);
        if (expr() != Type::INT) {
            cout << "error index is not int at line " << numLines << "\n";
            errorLog << numLines << " i\n";
        }
        eat(RBRACK);
        eat(ASSIGN);
        expr(); 
    }
    cout << "<赋值语句>" << endl;
}


void read()
{
    eat(SCANFTK);
    eat(LPARENT);
    eat(IDENFR);
    while (match(COMMA)) {
        eat(COMMA);
        eat(IDENFR);
    }
    eat(RPARENT);
    cout << "<读语句>" << endl; 
}


void checkString(string s)
{
  for (int i = 0; i < s.size(); i++) {
    if (!(s[i] == 32 || s[i] == 33 || (35 <= s[i] && s[i] <= 126))) {
      cout << "string error at line " << numLines << "\n";
      errorLog << numLines << " a\n";
      break;
    }
  }
}


void write()
{
    eat(PRINTFTK);
    eat(LPARENT);
    if (match(STRCON) || match(ERROR)) {
        str();
        if (match(COMMA)) {
            eat(COMMA);
            expr();
        }
    } 
    else {
        expr();
    }
    eat(RPARENT);
    cout << "<写语句>" << endl; 

}


void str()
{
    if (!match(ERROR))
      checkString(eat(STRCON));
    else {
      eat(ERROR);
      cout << "token error at " << numLines << endl;
      errorLog << numLines << " a\n"; 
    }
    cout << "<字符串>" << endl;
}


void _return()
{
    eat(RETURNTK);
    if (match(LPARENT)) {
        eat(LPARENT);
        expr();
        eat(RPARENT);
    }
    cout << "<返回语句>" << endl; 
}


void _return(Type type, bool &hasReturn)
{
    hasReturn = true;
    eat(RETURNTK);
    if (match(LPARENT)) {
        eat(LPARENT);
        if (expr() != type) {
          cout << "error return type at line " << numLines << "\n";
          if (type == Type::VOID)
            errorLog << numLines << " g\n";
          else
            errorLog << numLines << " h\n";
        }
        eat(RPARENT);
    }
    else if (type != Type::VOID) {
      cout << "function no return value at line " << numLines << "\n";
      errorLog << numLines << " h\n";
    }
    cout << "<返回语句>" << endl; 
}


vector<Type> values()
{
    vector<Type> vs;
    if (!match(RPARENT)) {
        vs.push_back(expr());
        while (match(COMMA)) {
            eat(COMMA);
            vs.push_back(expr());
        }
    }
    cout << "<值参数表>" << endl;
    return vs;
}


void _main()
{
    symTable.enterScope();
    eat(VOIDTK);
    eat(MAINTK);
    eat(LPARENT);
    eat(RPARENT);
    eat(LBRACE);
    procBlock();
    eat(RBRACE);
    cout << "<主函数>" << endl; 
    symTable.exitScope();
}


void old_parser()
{
    program();
}


bool error_handle()
{

    ifstream inFile;
    inFile.open("testfile.txt");
    string str((std::istreambuf_iterator<char>(inFile)),
                 std::istreambuf_iterator<char>());
    inFile.close();

    errorLog.open ("error.txt");

    old_lexer(str);

    old_parser();

    cout << endl;
    printTokens();

    errorLog.close();


    ifstream errorfile("error.txt");

    if (errorfile.peek() == ifstream::traits_type::eof()) {
      errorfile.close();
      return true;
    }
    else {
      errorfile.close();
      return false;
    }
}