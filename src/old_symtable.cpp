#include "old_utils.h"
#include "old_symtable.h" 


int errorLines;
extern std::ofstream errorLog;


void SymTable::addSymbol(std::string name, Attribute &attribute)
{
  if (mapHas(currentScopeSymbols, name)) {
    std::cout << name << " repeated error! at " << errorLines << "\n";
    errorLog << errorLines << " b\n";
    return;
  }

  if (!mapHas(allSymbols, name)) {
    modifyRecord.push_back(std::make_pair(name, Attribute()));
  }
  else {
    Attribute oldAttr = allSymbols.find(name)->second;
    modifyRecord.push_back(std::make_pair(name, oldAttr));
  }
  mapForceInsert(allSymbols, make_pair(name, attribute));
  mapForceInsert(currentScopeSymbols, make_pair(name, attribute));
}


void SymTable::enterScope()
{
  std::cout << "enter a new scope!\n";
  currentScopeSymbols.clear();
  modifyRecord.push_back(std::make_pair("", Attribute()));
}


void SymTable::exitScope()
{
  std::cout << "exit the current scope!\n";
  int i;
  for (i = modifyRecord.size() - 1; i >= 0; i--) {
    if (modifyRecord[i].first == "") {
      break;
    }
    if (modifyRecord[i].second.type == Type::VOID &&
        modifyRecord[i].second.qualifier == Qualifier::VOID) {
      allSymbols.erase(allSymbols.find(modifyRecord[i].first));
    }
    else {
      mapForceInsert(allSymbols, modifyRecord[i]);
    }
  }

  if (i < 0) i = 0;
  modifyRecord.erase(modifyRecord.begin() + i, modifyRecord.end());
  currentScopeSymbols.clear();

  for (i = modifyRecord.size() - 1; i >= 0; i--) {
    if (modifyRecord[i].first == "") {
      break;
    }
    std::string name = modifyRecord[i].first;
    currentScopeSymbols.insert(std::make_pair(name, getSymbol(name)));
  }
}


bool SymTable::hasSymbol(std::string name)
{
  return mapHas(allSymbols, name);
}


Attribute& SymTable::getSymbol(std::string name)
{
  return allSymbols.find(name)->second;
}


void SymTable::printAllSymbols()
{
  std::cout << "all symbols: \n";
  for (std::map<std::string, Attribute>::iterator it = allSymbols.begin(); it != allSymbols.end(); it++) {
    std::cout << it->first << ": ";
    if (it->second.type == Type::INT)
      std::cout << "INT ";
    else if (it->second.type == Type::CHAR)
      std::cout << "CHAR ";
    else
      std::cout << "VOID ";

    if (it->second.qualifier == Qualifier::CONST)
      std::cout << "CONST ";
    else if (it->second.qualifier == Qualifier::FUNC)
      std::cout << "FUNC " << it->second.args.size();
    else
      std::cout << "VOID ";
    std::cout << "\n";
  }
  std::cout << "\n";
}


void SymTable::printCurSymbols()
{
  std::cout << "current scope symbols: \n";
  for (std::map<std::string, Attribute>::iterator it = currentScopeSymbols.begin(); it != currentScopeSymbols.end(); it++) {
    std::cout << it->first << ": ";
    if (it->second.type == Type::INT)
      std::cout << "INT ";
    else if (it->second.type == Type::CHAR)
      std::cout << "CHAR ";
    else
      std::cout << "VOID ";

    if (it->second.qualifier == Qualifier::CONST)
      std::cout << "CONST ";
    else if (it->second.qualifier == Qualifier::FUNC)
      std::cout << "FUNC " << it->second.args.size();
    else
      std::cout << "VOID ";
    std::cout << "\n";
  }
  std::cout << "\n";
}


void SymTable::printModifyRecord()
{
  std::cout << "modify record: \n";
  for (std::vector<std::pair<std::string, Attribute>>::iterator it = modifyRecord.begin(); it != modifyRecord.end(); it++) {
    std::cout << it->first << ": ";
    if (it->second.type == Type::INT)
      std::cout << "INT ";
    else if (it->second.type == Type::CHAR)
      std::cout << "CHAR ";
    else
      std::cout << "VOID ";

    if (it->second.qualifier == Qualifier::CONST)
      std::cout << "CONST ";
    else if (it->second.qualifier == Qualifier::FUNC)
      std::cout << "FUNC " << it->second.args.size();
    else
      std::cout << "VOID ";
    std::cout << "\n";
  }
  std::cout << "\n";
}