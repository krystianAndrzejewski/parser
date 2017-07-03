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
    const Symbol *getSymbol(const std::string &name);
    const Production *getProduction(const std::string &name);
    const Precedence *getPrecedence(const std::string &name);
    const Symbol *getSymbol(const std::size_t &number);
    const Production *getProduction(const std::size_t &number);
    const Precedence *getPrecedence(const std::size_t &number);
    bool getSymbol(const std::string &name, std::size_t &result);
    bool getProduction(const std::string &name, std::size_t &result);
    bool getPrecedence(const std::string &name, std::size_t &result);

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
    std::unordered_map<Symbol, std::size_t> mapSymbols;
    std::unordered_map<std::string, std::size_t> mapSymbolsString;

    std::vector<std::pair<Precedence*, bool>> precedences;
    std::unordered_map<Precedence, std::size_t> mapPrecedences;
    std::unordered_map<std::string, std::size_t> mapPrecedencesString;

    std::vector<Production*> productions;
    std::unordered_map<Production, std::size_t> mapProductions;
    std::unordered_map<std::string, std::size_t> mapProductionsString;
    std::unordered_map<Symbol, std::vector<const Production*>> mapProductToProductions;

    std::unordered_map<Symbol, std::vector<const Symbol*>> First;
    std::unordered_map<Symbol, std::vector<const Symbol*>> Follow;

    std::unordered_set<Precedence> unusedPrecedences;
    std::unordered_set<Symbol> unusedSymbols;
    std::unordered_set<Production> unusedProductions;
    std::unordered_set<Production> infinitivePoductions;

    friend class GrammarBuilder;
};


class GrammarBuilder
{
public:
    static Grammar *createGrammar(const FileReader &reader);
    static void deleteGrammar(const Grammar *grammar);
private:
    static void createNonterminals(
        Grammar *grammar,
        const std::vector<std::tuple<std::string, std::vector<std::pair<std::string, bool>>, std::string>> &productions,
        const std::vector<std::tuple<std::string, std::string>> &nonTerminals);
    static void createPrecedences(
        Grammar *grammar,
        const std::vector<std::tuple<std::string, unsigned char, unsigned int>> &precedences);
    static void createTerminals(
        Grammar *grammar,
        const std::vector<std::tuple<std::string, std::string, std::string>> &terminals);
    static void createProductions(
        Grammar *grammar,
        const std::vector<std::tuple<std::string, std::vector<std::pair<std::string, bool>>, std::string>> &productions);
};

