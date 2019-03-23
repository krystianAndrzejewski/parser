#pragma once
#include "Grammar.h"
#include <memory>
#include <functional>

class GrammarBuilder
{
public:
    static Grammar *createGrammar(const StreamReader &reader);
    static void deleteGrammar(const Grammar *grammar);

private:
    static void createNonterminals(
		std::unique_ptr<Grammar, std::function <void(const Grammar*)>>& grammar,
        const std::vector<std::tuple<std::string, std::vector<std::pair<std::string, bool>>, std::string>> &productions,
        const std::vector<std::tuple<std::string, std::string>> &nonTerminals);

    static void createPrecedences(
		std::unique_ptr<Grammar, std::function <void(const Grammar*)>>& grammar,
        const std::vector<std::tuple<std::string, unsigned char, unsigned int>> &precedences);

    static void createTerminals(
		std::unique_ptr<Grammar, std::function <void(const Grammar*)>>& grammar,
        const std::vector<std::tuple<std::string, std::string, std::string>> &terminals);

    static void createProductions(
		std::unique_ptr<Grammar, std::function <void(const Grammar*)>>& grammar,
        const std::vector<std::tuple<std::string, std::vector<std::pair<std::string, bool>>, std::string>> &productions);
};

