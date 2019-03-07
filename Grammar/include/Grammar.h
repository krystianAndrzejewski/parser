#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <unordered_set>

#include "Production.h"
#include "Symbol.h"
#include "NonTerminal.h"
#include "Terminal.h"
#include "FileReader.h"

class GrammarBuilder;

class Grammar
{
public:
    const Symbol *getSymbol(const std::string &name) const;
    const Production *getProduction(const std::string &name) const;
    const Precedence *getPrecedence(const std::string &name) const;

    const Symbol *getSymbol(const std::size_t &number) const;
    const Production *getProduction(const std::size_t &number) const;
    const Precedence *getPrecedence(const std::size_t &number) const;

    bool getSymbol(const std::string &name, std::size_t &result) const;
    bool getProduction(const std::string &name, std::size_t &result) const;
    bool getPrecedence(const std::string &name, std::size_t &result) const;

    const std::vector<const Symbol*> *getFirst(const Symbol *symbol) const;
    const std::vector<const Symbol*> *getFollow(const Symbol *symbol) const;

    const vector<Production *> &getAllProductions() const;
    const vector<Symbol *> &getAllSymbols() const;

    const vector<const Production *> *getProductions(const Symbol* product) const;

    const NonTerminal* getStartNonTerminal() const;

    void printDebugInfo() const;

private:
    enum symbolsKind : unsigned char
    {
        kSimpleSymbol = 0,
        kTerminal = 1,
        kNonTerminal = 2
    };

    Grammar();
    ~Grammar();

    bool insertSymbol(
        symbolsKind kind, 
        const std::string &name,
        const std::string &type);
    bool insertTerminal(
        const std::string &name, 
        const std::string &pattern,
        const std::string &type);
    void setTerminalsPrecedence();

    bool insertPrecedence(
        const std::string &name, 
        unsigned int prority, 
        Precedence::Association association);

    bool insertProduction(
        const std::string &product, 
        const std::vector<std::pair<std::string, bool>> &igredients,
        const std::string &namePrecedence);

    const Precedence *getUnusedPrecedence(const std::string &name);

    void setStart(const std::string &name);

    void checkUnusedPrecedences();
    void checkUnusedSymbolsAndProductions();
    void checkInfinitiveProductions();
    void createFirstTable();
    void createFollowTable();

    const NonTerminal *start;

    std::vector<Symbol*> symbols;
    std::unordered_map<const Symbol *, std::size_t> mapSymbols;
    std::unordered_map<std::string, std::size_t> mapSymbolsString;

    std::vector<std::pair<Precedence*, bool>> precedences;
    std::unordered_map<const Precedence*, std::size_t> mapPrecedences;
    std::unordered_map<std::string, std::size_t> mapPrecedencesString;

    std::vector<Production*> productions;
    std::unordered_map<const Production *, std::size_t> mapProductions;
    std::unordered_map<std::string, std::size_t> mapProductionsString;
    std::unordered_map<const Symbol*, std::vector<const Production*>> mapProductToProductions;

    std::unordered_map<const Symbol*, std::vector<const Symbol*>> First;
    std::unordered_map<const Symbol*, std::vector<const Symbol*>> Follow;

    std::unordered_set<const Precedence *> unusedPrecedences;
    std::unordered_set<const Symbol *> unusedSymbols;
    std::unordered_set<const Production *> unusedProductions;
    std::unordered_set<const Production *> infinitivePoductions;

    friend class GrammarBuilder;
};

