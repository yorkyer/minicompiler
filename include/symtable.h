#ifndef SYMTABLE_H
#define SYMTABLE_H


enum class Type 
{
  INT, CHAR, VOID
};
const vector<string> TypeName = { "int", "char", "void" };


enum class Qual
{
  CONST, VOID
};


class VarAttr 
{
public:
  Type type;
  Qual qual;

  int value;

  string var;

  VarAttr() : type(Type::VOID), qual(Qual::VOID) {}
  string str();
};


class FunAttr
{
public:
	Type type;
	vector<Type> args;

	FunAttr() {}
  string str();
};


class VarTable
{
public:
  vector<pair<string, VarAttr>> modify;
  map<string, VarAttr> allsyms;
  map<string, VarAttr> cursyms;
  
  VarTable() {}
  void addsym(string name, VarAttr &attr);
  bool hassym(string name);
  VarAttr& getattr(string name);
  void enter_scope();
  void exit_scope(); 
  void clear();
};


class FunTable
{
public:
	map<string, FunAttr> syms;

	FunTable() {}
	void addsym(string name, FunAttr &attr);
	bool hassym(string name);
	FunAttr& getattr(string name);
};


void print_tables();


#endif