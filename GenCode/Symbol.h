#pragma once
#include <string>
#include "Precedence.h"

class Symbol
{
public:
    Symbol(const std::string &pName, const std::string &pType = "", const Precedence *pPrecedence = nullptr);
    Symbol::Symbol(const std::string &pType, const Precedence *pPrecedence);
    virtual ~Symbol();

    const std::string &getName() const { return name; }
    virtual const std::string *getPattern() const { return &pattern; }
    virtual bool isTerminal() const { return false; }
    virtual bool isNonTerminal() const { return false; }
    const Precedence *getPrecdence() const { return precedence; }
    void setPrecdence(const Precedence *pPrecedence){ precedence = pPrecedence; }

    bool operator==(const Symbol &rhs) const { return name.compare(rhs.getName()) == 0; }

protected:
    std::string name;
    const Precedence *precedence;
    std::string type;
    std::string pattern;
};

template <>
struct std::hash<Symbol>
{
    std::size_t operator()(const Symbol& k) const
    {
        return std::hash<std::string>{}(k.getName());
    }
};

template <>
struct std::equal_to<Symbol>
{
    bool operator()(const Symbol& lhs, const Symbol& rhs) const
    {
        return lhs.getName().compare(rhs.getName()) == 0;
    }
};
