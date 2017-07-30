#include "GrammarBuilder.h"
#include "GrammarException.h"


Grammar *GrammarBuilder::createGrammar(const FileReader &reader)
{
    auto tokens = reader.getTokens();
    auto precedences = reader.getPrecedences();
    auto nonTerminals = reader.getNonTerminals();
    auto productions = reader.getProductions();
    Grammar *grammar = new Grammar();

    createNonterminals(grammar, productions, nonTerminals);
    createPrecedences(grammar, precedences);
    createTerminals(grammar, tokens);
    createProductions(grammar, productions);
    grammar->setTerminalsPrecedence();
    grammar->setStart(reader.getStart());
    grammar->checkUnusedSymbolsAndProductions();
    grammar->checkInfinitiveProductions();
    grammar->createFirstTable();
    grammar->createFollowTable();

    return grammar;
}

void GrammarBuilder::deleteGrammar(const Grammar *grammar)
{
    delete grammar;
}

void GrammarBuilder::createNonterminals(
    Grammar *grammar,
    const std::vector<std::tuple<std::string, std::vector<std::pair<std::string, bool>>, std::string>> &productions,
    const std::vector<std::tuple<std::string, std::string>> &nonTerminals)
{
    for (auto nonTerminal : nonTerminals)
    {
        if (grammar->insertSymbol(
            Grammar::symbolsKind::kNonTerminal,
            std::get<0>(nonTerminal),
            std::get<1>(nonTerminal)) == false)
        {
            throw GrammarException("Repeated nonTerminal declaration.", GrammarException::ErrorCause::repeated_symbol_declaration);
        }
    }

    for (auto production : productions)
    {
        grammar->insertSymbol(Grammar::symbolsKind::kNonTerminal, std::get<0>(production), "");
    }
}


void GrammarBuilder::createPrecedences(
    Grammar *grammar,
    const std::vector<std::tuple<std::string, unsigned char, unsigned int>> &precedences)
{
    for (auto precedence : precedences)
    {
        if (grammar->insertPrecedence(
            std::get<0>(precedence),
            std::get<2>(precedence),
            static_cast<Precedence::Association>(std::get<1>(precedence))) == false)
        {
            throw GrammarException("Repeated nonTerminal declaration.", GrammarException::ErrorCause::repeated_symbol_declaration);
        }
    }
}


void GrammarBuilder::createTerminals(
    Grammar *grammar,
    const std::vector<std::tuple<std::string, std::string, std::string>> &terminals)
{
    for (auto token : terminals)
    {
        if (grammar->insertTerminal(std::get<0>(token), std::get<1>(token), std::get<2>(token)) == false)
        {
            throw GrammarException("Repeated terminal declaration.", GrammarException::ErrorCause::repeated_symbol_declaration);
        }
    }
}


void GrammarBuilder::createProductions(
    Grammar *grammar,
    const std::vector<std::tuple<std::string, std::vector<std::pair<std::string, bool>>, std::string>> &productions)
{
    for (auto production : productions)
    {
        if (grammar->insertProduction(std::get<0>(production), std::get<1>(production), std::get<2>(production)) == false)
        {
            throw GrammarException("Repeated production declaration.", GrammarException::ErrorCause::repeated_production_declaration);
        }
    }
}
