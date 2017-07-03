#pragma once

#include <vector>
#include <string>

#include "Symbol.h"
#include "Terminal.h"
#include "NonTerminal.h"
#include "Precedence.h"

class Production
{
public:
    Production(const NonTerminal &pProduct, const std::vector<const Symbol *> &pIgridients, const Precedence *pPrecedence);
    ~Production();

    bool operator==(const Production &rhs) const { return hashableName.compare(rhs.getHashableName()) == 0; }

    const std::string &getHashableName() const { return hashableName; };
    const NonTerminal &getProduct() const { return product; };
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
