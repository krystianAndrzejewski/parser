#pragma once

#include "Symbol.h"
#include "Terminal.h"
#include "NonTerminal.h"
#include "Precedence.h"

#include <vector>
#include <string>

class Production
{
public:
    Production(const NonTerminal &pProduct, const std::vector<const Symbol *> &pIgridients, const Precedence *pPrecedence);
    ~Production();

    bool operator==(const Production &rhs) const { return hashableName.compare(rhs.getHashableName()) == 0; }

    const std::string &getHashableName() const { return hashableName; };
    const NonTerminal &getProduct() const { return product; };
    const Precedence *getPrecedence() const { return precedence; }
    const std::vector<const Symbol *> &getIgredients() const { return igredients; };

private:
    void createHashableName();

    const NonTerminal &product;
    std::vector<const Symbol *> igredients;
    std::string hashableName;
    const Precedence *precedence;
};

template <>
struct std::hash<Production>
{
    std::size_t operator()(const Production& k) const
    {
        return std::hash<std::string>()(k.getHashableName());
    }
};

template <>
struct std::equal_to<Production>
{
    bool operator()(const Production& lhs, const Production& rhs) const
    {
        return lhs.getHashableName().compare(rhs.getHashableName()) == 0;
    }
};

template <>
struct std::less<Production>
{
    bool operator()(const Production lhs, const Production rhs) const
    {
        return lhs.getHashableName().compare(rhs.getHashableName()) == -1;
    }
};

template <>
struct std::equal_to<const Production*>
{
    bool operator()(const Production * const lhs, const Production * const rhs) const
    {
        return lhs->getHashableName().compare(rhs->getHashableName()) == 0;
    }
};

template <>
struct std::hash<const Production *>
{
    std::size_t operator()(const Production * const k) const
    {
        return std::hash<std::string>()(k->getHashableName());
    }
};

template <>
struct std::less<const Production *>
{
    bool operator()(const Production * const lhs, const Production * const rhs) const
    {
        return lhs->getHashableName().compare(rhs->getHashableName()) == -1;
    }
};