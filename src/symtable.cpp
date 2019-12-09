#include "utils.h"
#include "symtable.h"


VarTable vtable;
FunTable ftable;


void print_tables();


string VarAttr::str()
{
  string s;
  s += TypeName[static_cast<int>(type)];
  if (qual == Qual::CONST)
    s += "\tconst\t" + to_string(value) + "\t" + var;
  return s;
}


string FunAttr::str()
{
  string s;
  s += TypeName[static_cast<int>(type)];
  for (Type type : args)
    s += "\t" + TypeName[static_cast<int>(type)];
  return s;
}


void VarTable::addsym(string name, VarAttr &attr)
{
  if (allsyms.count(name) == 0)
    modify.push_back(make_pair(name, VarAttr()));
  else
    modify.push_back(make_pair(name, allsyms[name]));
  allsyms[name] = attr;
  cursyms[name] = attr;
}


bool VarTable::hassym(string name)
{
  return allsyms.count(name) > 0;
}


VarAttr& VarTable::getattr(string name)
{
  return allsyms[name];
}


void VarTable::enter_scope()
{
  cursyms.clear();
  modify.push_back(make_pair("", VarAttr()));
}


void VarTable::exit_scope()
{
  int i;
  for (i = modify.size() - 1; i >= 0; i--) {
    if (modify[i].first == "")
      break;
    if (modify[i].second.type == Type::VOID &&
        modify[i].second.qual == Qual::VOID)
      allsyms.erase(allsyms.find(modify[i].first));
    else
      allsyms[modify[i].first] = modify[i].second;
  }

  if (i < 0) i = 0;
  modify.erase(modify.begin() + i, modify.end());
  cursyms.clear();

  for (i = modify.size() - 1; i >= 0; i--) {
    if (modify[i].first == "")
      break;
    cursyms[modify[i].first] = allsyms[modify[i].first];
  }
}


void VarTable::clear()
{
  allsyms.clear();
  cursyms.clear();
  modify.clear();
}


void FunTable::addsym(string name, FunAttr &attr)
{
  syms[name] = attr;
}


bool FunTable::hassym(string name)
{
  return syms.count(name) > 0;
}


FunAttr& FunTable::getattr(string name)
{
  return syms[name];
}


void print_tables()
{
  ofstream output;
  output.open("symtable.txt");
  output << "vtable:\n";
  output << "\nallsyms\n";
  for (auto pair : vtable.allsyms)
    output << pair.first << ":\t" << pair.second.str() << "\n";

  output << "\ncursyms\n";
  for (auto pair : vtable.cursyms)
    output << pair.first << ":\t" << pair.second.str() << "\n";

  output << "\n\nftable\n"; 
  output << "\nsyms\n";
  for (auto pair : ftable.syms)
    output << pair.first << ":\t" << pair.second.str() << "\n";

  output.close();
}