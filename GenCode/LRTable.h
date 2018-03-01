#pragma once
#include "LRItem.h"
#include "Grammar.h"
#include "State.h"
#include <set>

template<typename T>
struct std::less<T*>
{
    bool operator()(const T * const lhs, const T * const rhs) const
    {
        return *lhs < *rhs;
    }
};

template<typename T>
struct std::equal_to<const T*>
{
    bool operator()(const T* const lhs, const T* const rhs) const
    {
        return *lhs == *rhs;
    }
};

class LRTable
{
public:

    LRTable(const Grammar *pGrammar);
    ~LRTable();

    void printDebugInfo();

    enum Action : unsigned char
    {
        reduce = 0,
        shift = 1,
        accept = 2
    };

private:

    void generateItems();
    void generateStatesLRO();
    void takeEmptyProducts();
    void takeNonTerminalTransitions();
    void generateLookbacksAndIncludes();
    void generateFollowSets();
    void addLookaheads();

    void generateLALRlookahead();
    void generateReadSets();

    void generateTable();

    const Grammar *grammar;

    std::vector<LRItem*> items;
    std::unordered_map<const Production *, std::vector<LRItem*>> itemsMap;

    std::vector<State*> states;
    std::map<const State*, std::size_t> statesNumberMap;
    std::map<const std::vector<const LRItem*> *, const State*> statesMap;

    std::unordered_set<const Symbol *> emptyProducts;
    std::set<const std::pair< const Symbol *, const State * >*> nonTerminalTransitions;

    std::map<std::pair< const Symbol *, const State * >, std::vector<std::pair< const LRItem *, const State * >>> lookBacksDictionary;
    std::map<std::pair< const Symbol *, const State * >, std::vector<std::pair< const Symbol *, const State * >>> includesDictionary;
    std::map<const std::pair< const Symbol *, const State *> *, unordered_set<const Symbol*>> readSets;
    std::map<const std::pair< const Symbol *, const State *> *, unordered_set<const Symbol*>> followSets;

    std::map<const std::pair< const Symbol *, const State *>, std::pair< Action, std::size_t>> actionTable;
    std::map<const std::pair< const Symbol *, const State *>, std::size_t> gotoTable;

    std::vector<std::tuple<const State*, std::size_t, std::size_t, Action>> srConflicts;
    std::vector<std::tuple<const State*, std::size_t, std::size_t>> rrConflicts;

    std::vector<const Production *> unreducedProductions;
};