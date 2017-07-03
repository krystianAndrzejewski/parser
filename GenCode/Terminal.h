#pragma once
#include "Symbol.h"
class Terminal :
    public Symbol
{
public:
    Terminal(
        const std::string &pName, 
        const std::string &pPattern, 
        const std::string &pType = "", 
        const Precedence *pPrecedence = nullptr);
    virtual ~Terminal();

    virtual bool isTerminal() const { return true; }
    virtual bool isNonTerminal() const { return false; }
};


