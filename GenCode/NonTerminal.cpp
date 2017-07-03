#include "NonTerminal.h"


NonTerminal::NonTerminal(const std::string &pName, const std::string &pType, const Precedence *pPrecedence)
    : Symbol(pType, pPrecedence)
{
    name = pName;
}


NonTerminal::~NonTerminal()
{
}
