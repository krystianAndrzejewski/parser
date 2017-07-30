#pragma once
#include "Grammar.h"

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

