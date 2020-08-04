#ifndef __OLD_ATTRIBUTE
#define __OLD_ATTRIBUTE


enum class Type 
{
  INT, CHAR, VOID
};


enum class Qualifier 
{
  CONST, FUNC, VOID
};


class Attribute 
{
public:
  Type type;
  Qualifier qualifier;

  std::vector<Type> args; // qualifier == FUNC
  int intValue; // Type == INT && Qualifier == CONST
  int charValue; // Type == CHAR && Qualifier == CONST


  Attribute() {}
  Attribute(Type t, Qualifier q) : type(t), qualifier(q) {}
};


#endif