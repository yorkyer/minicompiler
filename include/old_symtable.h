#ifndef __OLD_SYMTABLE
#define __OLD_SYMTABLE


#include "old_attribute.h"


class SymTable 
{
public:
  std::vector<std::pair<std::string, Attribute>> modifyRecord;
  std::map<std::string, Attribute> allSymbols;
  std::map<std::string, Attribute> currentScopeSymbols;
  

  SymTable() {}

  void addSymbol(std::string name, Attribute &attribute);
  void enterScope();
  void exitScope(); 
  bool hasSymbol(std::string name);
  Attribute& getSymbol(std::string name);

  void printAllSymbols();
  void printCurSymbols();
  void printModifyRecord();
};


#endif