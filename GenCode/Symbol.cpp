#include "Symbol.h"


Symbol::Symbol(const std::string &pName, const std::string &pType, const Precedence *pPrecedence)
    : type(pType),
    precedence(pPrecedence)
{
    name = "'" + pName + "'";
    pattern = pName;
}

Symbol::Symbol(const std::string &pType, const Precedence *pPrecedence)
    : type(pType),
    precedence(pPrecedence)
{

}


Symbol::~Symbol()
{
}