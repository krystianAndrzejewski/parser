#pragma once
#include <string>
#include "Precedence.h"

class Symbol
{
public:
    Symbol(const std::string &pName, const std::string &pType = "", const Precedence *pPrecedence = nullptr);
    Symbol(const std::string &pType, const Precedence *pPrecedence);
    virtual ~Symbol();

    const std::string &getName() const { return name; }
	const std::string &getType() const { return type; }
    virtual const std::string *getPattern() const { return &pattern; }
    virtual bool isTerminal() const { return false; }
    virtual bool isNonTerminal() const { return false; }
    const Precedence *getPrecedence() const { return precedence; }
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

template <>
struct std::less<Symbol>
{
    bool operator()(const Symbol lhs, const Symbol rhs) const
    {
        return lhs.getName().compare(rhs.getName()) == -1;
    }
};

template <>
struct std::hash<const Symbol *>
{
    std::size_t operator()(const Symbol * const k) const
    {
        return std::hash<std::string>{}(k->getName());
    }
};

template <>
struct std::equal_to<const Symbol *>
{
    bool operator()(const Symbol * const lhs, const Symbol * const rhs) const
    {
        return lhs->getName().compare(rhs->getName()) == 0;
    }
};

template <>
struct std::less<const Symbol *>
{
    bool operator()(const Symbol * const lhs, const Symbol * const rhs) const
    {
        return lhs->getName().compare(rhs->getName()) == -1;
    }
};