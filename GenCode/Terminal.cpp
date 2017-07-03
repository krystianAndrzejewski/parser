#include "Terminal.h"


Terminal::Terminal(
    const std::string &pName, 
    const std::string &pPattern, 
    const std::string &pType,
    const Precedence *pPrecedence)
    : Symbol(pType, pPrecedence)
{
    name = pName;
    pattern = pPattern;
}


Terminal::~Terminal()
{
}

