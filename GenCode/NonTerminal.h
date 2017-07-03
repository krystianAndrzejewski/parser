#pragma once
#include "Symbol.h"
class NonTerminal :
    public Symbol
{
public:
    NonTerminal(const std::string &pName, const std::string &pType = "", const Precedence *pPrecedence = nullptr);
    virtual ~NonTerminal();

    virtual const std::string *getPattern() const { return nullptr; }
    virtual bool isTerminal() const { return false; }
    virtual bool isNonTerminal() const { return true; }

};

