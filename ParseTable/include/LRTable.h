#pragma once
#include "LRItem.h"
#include "Grammar.h"
#include "State.h"
#include <set>
#include <memory>

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

    LRTable(const Grammar &pGrammar);
    ~LRTable();

	enum Action : unsigned char
	{
		reduce = 0,
		shift = 1,
		accept = 2
	};

    void printDebugInfo();
	bool getAction(const std::pair<std::size_t, std::size_t>& actionDef, std::pair<LRTable::Action, std::size_t>& moveProps);
	bool getGoto(const std::pair<std::size_t, std::size_t>& gotoActionDef, std::size_t & stateIndex);

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

    std::reference_wrapper<const Grammar> grammar;

    std::vector<std::unique_ptr<LRItem>> items;
    std::unordered_map<const Production *, std::vector<LRItem*>> itemsMap;

    std::vector<ParserState*> states;
    std::map<const ParserState*, std::size_t> statesNumberMap;
    std::map<const std::vector<const LRItem*> *, const ParserState*> statesMap;

    std::unordered_set<const Symbol *> emptyProducts;
    std::set<const std::pair< const Symbol *, const ParserState * >*> nonTerminalTransitions;

    std::map<std::pair< const Symbol *, const ParserState * >, std::vector<std::pair< const LRItem *, const ParserState * >>> lookBacksDictionary;
    std::map<std::pair< const Symbol *, const ParserState * >, std::vector<std::pair< const Symbol *, const ParserState * >>> includesDictionary;
    std::map<const std::pair< const Symbol *, const ParserState *> *, unordered_set<const Symbol*>> readSets;
    std::map<const std::pair< const Symbol *, const ParserState *> *, unordered_set<const Symbol*>> followSets;

    std::map<const std::pair< const Symbol *, const ParserState *>, std::pair< Action, std::size_t>> actionTable;
    std::map<const std::pair< const Symbol *, const ParserState *>, std::size_t> gotoTable;

    std::vector<std::tuple<const ParserState*, std::size_t, std::size_t, Action>> srConflicts;
    std::vector<std::tuple<const ParserState*, std::size_t, std::size_t>> rrConflicts;

    std::vector<const Production *> unreducedProductions;
};