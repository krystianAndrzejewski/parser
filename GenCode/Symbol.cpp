#include "Symbol.h"


Symbol::Symbol(const std::string &pName, const std::string &pType, const Precedence *pPrecedence)
    : type(pType),
    precedence(pPrecedence)
{
    name = "'" + pName + "'";
    pattern = name;
}

Symbol::Symbol(const std::string &pType, const Precedence *pPrecedence)
    : type(pType),
    precedence(pPrecedence)
{

}


Symbol::~Symbol()
{
}