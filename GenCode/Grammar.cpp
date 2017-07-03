#include "Grammar.h"
#include "GrammarException.h"

#include <stack>
#include <set>


const Symbol *Grammar::getSymbol(const std::string &name)
{
    const Symbol *result = nullptr;
    auto terminalIterator = mapSymbolsString.find(name);
    if (terminalIterator != mapSymbolsString.end())
    {
        result = symbols[terminalIterator->second];
    }
    return result;
}

const Precedence *Grammar::getPrecedence(const std::string &name)
{
    const Precedence *result = nullptr;
    auto precedenceIterator = mapPrecedencesString.find(name);
    if (precedenceIterator != mapPrecedencesString.end())
    {
        result = precedences[precedenceIterator->second].first;
    }
    return result;
}

const Production *Grammar::getProduction(const std::string &name)
{
    const Production *result = nullptr;
    auto productionIterator = mapProductionsString.find(name);
    if (productionIterator != mapProductionsString.end())
    {
        result = productions[productionIterator->second];
    }
    return result;
}


const Symbol *Grammar::getSymbol(const std::size_t &number)
{
    if (number < symbols.size())
    {
        return symbols[number];
    }
    return nullptr;
}


const Production *Grammar::getProduction(const std::size_t &number)
{
    if (number < productions.size())
    {
        return productions[number];
    }
    return nullptr;
}


const Precedence *Grammar::getPrecedence(const std::size_t &number)
{
    if (number < precedences.size())
    {
        return precedences[number].first;
    }
    return nullptr;
}


bool Grammar::getSymbol(const std::string &name, std::size_t &result)
{
    auto symbolIterator = mapSymbolsString.find(name);
    if (symbolIterator != mapSymbolsString.end())
    {
        result = symbolIterator->second;
        return true;
    }
    return false;
}


bool Grammar::getProduction(const std::string &name, std::size_t &result)
{
    auto productionIterator = mapProductionsString.find(name);
    if (productionIterator != mapProductionsString.end())
    {
        result = productionIterator->second;
        return true;
    }
    return false;
}


bool Grammar::getPrecedence(const std::string &name, std::size_t &result)
{
    auto precedenceIterator = mapPrecedencesString.find(name);
    if (precedenceIterator != mapPrecedencesString.end())
    {
        result = precedenceIterator->second;
        return true;
    }
    return false;
}

Grammar::Grammar()
{
    Grammar::insertTerminal(
        "$",
        "\0",
        "end");
}


Grammar::~Grammar()
{
    mapSymbols.clear();
    for (std::size_t i = 0; i < symbols.size(); i++)
    {
        delete symbols[i];
    }
    symbols.clear();
    mapPrecedences.clear();
    for (std::size_t i = 0; i < precedences.size(); i++)
    {
        delete precedences[i].first;
    }
    precedences.clear();
    mapProductions.clear();
    for (std::size_t i = 0; i < productions.size(); i++)
    {
        delete productions[i];
    }
    productions.clear();
}


bool Grammar::insertSymbol(
    symbolsKind kind,
    const std::string &name,
    const std::string &type)
{
    bool success = true;
    Symbol *newSymbol;
    switch (kind)
    {
    case symbolsKind::kNonTerminal:
        if (mapSymbolsString.find(name) == mapSymbolsString.end())
        {
            newSymbol = new NonTerminal(name, type);
            symbols.push_back(newSymbol);
            mapSymbols[*newSymbol] = symbols.size() - 1;
            mapSymbolsString[newSymbol->getName()] = mapSymbols[*newSymbol];
        }
        else
        {
            success = false;
        }
        break;
    case symbolsKind::kTerminal:
        if (mapSymbolsString.find(name) == mapSymbolsString.end())
        {
            newSymbol = new Terminal(name, "", type);
            symbols.push_back(newSymbol);
            mapSymbols[*newSymbol] = mapSymbols[*newSymbol];
            mapSymbolsString[newSymbol->getName()] = mapSymbols[*newSymbol];
        }
        else
        {
            success = false;
        }
        break;
    case symbolsKind::kSimpleSymbol:
        if (mapSymbolsString.find(name) == mapSymbolsString.end())
        {
            newSymbol = new Symbol(name, type);
            symbols.push_back(newSymbol);
            mapSymbols[*newSymbol] = symbols.size() - 1;
            mapSymbolsString[newSymbol->getName()] = mapSymbols[*newSymbol];
        }
        else
        {
            success = false;
        }
        break;
    default:
        throw GrammarException("Undefined symbol kind.", GrammarException::ErrorCause::undefined_symbol_kind);
        break;
    }
    return success;
}

bool Grammar::insertTerminal(
    const std::string &name,
    const std::string &pattern,
    const std::string &type)
{
    bool success = true;
    if (mapSymbolsString.find(name) == mapSymbolsString.end())
    {
        Symbol *newSymbol = new Terminal(name, type);
        symbols.push_back(newSymbol);
        mapSymbols[*newSymbol] = symbols.size() - 1;
        mapSymbolsString[newSymbol->getName()] = mapSymbols[*newSymbol];
    }
    else
    {
        success = false;
    }
    return success;
}

void Grammar::setTerminalsPrecedence()
{
    for (auto token : symbols)
    {
        if (token->isTerminal())
        {
            auto precedenceIterator = mapPrecedencesString.find(token->getName());
            if (precedenceIterator != mapPrecedencesString.end())
            {
                auto precedence = precedences[precedenceIterator->second];
                token->setPrecdence(precedence.first);
                precedence.second = true;
            }
        }
    }
    
}

bool Grammar::insertPrecedence(
    const std::string &name,
    unsigned int prority,
    Precedence::Association association)
{
    bool success = true;
    if (mapPrecedencesString.find(name) == mapPrecedencesString.end())
    {
        Precedence *newPrecedence = new Precedence(name, prority, association);
        precedences.push_back(make_pair(newPrecedence, false));
        mapPrecedences[*newPrecedence] = precedences.size() - 1;
        mapPrecedencesString[newPrecedence->getName()] = mapPrecedences[*newPrecedence];
    }
    else
    {
        success = false;
    }
    return success;
}

bool Grammar::insertProduction(
    const std::string &product,
    const std::vector<std::pair<std::string, bool>> &igredients,
    const std::string &namePrecedence)
{
    bool success = true;
    auto &allSymbols = mapSymbols;

    const Precedence *precedence = getUnusedPrecedence(namePrecedence);
    
    const NonTerminal *nonTerminal = dynamic_cast<const NonTerminal *>(getSymbol(product));
    if (!nonTerminal)
    {
        throw GrammarException("Undefined symbol.", GrammarException::ErrorCause::undefined_symbol);
    }
    vector<const Symbol *> productionIgredients;
    for (auto igredient : igredients)
    {
        std::string name;
        const Symbol *symbol = nullptr;
        if (igredient.second)
        {
            name = igredient.first;
            symbol = getSymbol(name);
            if (!symbol)
            {
                insertSymbol(symbolsKind::kTerminal, name, "");
                symbol = getSymbol(name);
            }
        }
        else
        {
            name = "'" + igredient.first + "'";
            symbol = getSymbol(name);
            if (!symbol)
            {
                insertSymbol(symbolsKind::kSimpleSymbol, igredient.first, "");
                symbol = getSymbol(name);
            }
        }
        productionIgredients.push_back(symbol);
    }
    if (productionIgredients.empty())
    {
        throw GrammarException("Empty production.", GrammarException::ErrorCause::empty_production);
    }
    Production *production = new Production(*nonTerminal, productionIgredients, precedence);
    if (mapProductionsString.find(production->getHashableName()) == mapProductionsString.end())
    {
        productions.push_back(production);
        mapProductions[*production] = productions.size() - 1;
        mapProductionsString[production->getHashableName()] = mapProductions[*production];
        if (mapProductToProductions.find(*nonTerminal) == mapProductToProductions.end())
        {
            mapProductToProductions[*nonTerminal] = std::vector<const Production*>();
        }
        mapProductToProductions[*nonTerminal].push_back(production);
    }
    else
    {
        success = false;
        delete production;
    }
    return success;
}


void Grammar::setStart(const std::string &name)
{
    start = dynamic_cast<const NonTerminal *>(getSymbol(name));
    if (start == nullptr)
    {
        throw GrammarException("An initial noterminal is not set.", GrammarException::ErrorCause::undefined_nonterminal_start);
    }
}


const Precedence *Grammar::getUnusedPrecedence(const std::string &namePrecedence)
{
    const Precedence *precedence = nullptr;
    auto precedenceIterator = mapPrecedencesString.find(namePrecedence);
    if (precedenceIterator == mapPrecedencesString.end() && !namePrecedence.empty())
    {
        throw GrammarException("Undefined precedence.", GrammarException::ErrorCause::undefined_precedence);
    }
    else if (precedenceIterator != mapPrecedencesString.end())
    {
        precedence = precedences[precedenceIterator->second].first;
        if (precedences[precedenceIterator->second].second)
        {
            throw GrammarException("Repeated precedence.", GrammarException::ErrorCause::repeated_precedence);
        }
    }
    return precedence;
}

void Grammar::checkUnusedPrecedences()
{
    for (auto precedence : precedences)
    {
        if (precedence.second)
        {
            std::size_t precedenceNumber;
            getPrecedence(precedence.first->getName(), precedenceNumber);
            unusedPrecedences.insert(*precedence.first);
        }
    }
}

void Grammar::checkUnusedSymbolsAndProductions()
{
    std::set<std::size_t> reachableProduction;
    std::set<std::size_t> reachableSymbols;
    auto iterator = mapSymbols.find(*start);
    std::stack<std::size_t> symbolsStack;
    symbolsStack.push(iterator->second);
    reachableSymbols.insert(iterator->second);
    do
    {
        std::size_t symbolNumber = symbolsStack.top();
        symbolsStack.pop();
        const Symbol *symbol = getSymbol(symbolNumber);
        if (symbol != nullptr)
        {
            auto productionIterator = mapProductToProductions.find(*symbol);
            if (productionIterator != mapProductToProductions.end())
            {
                for (auto production : productionIterator->second)
                {
                    std::size_t productionNumber;
                    if (getProduction(production->getHashableName(), productionNumber))
                    {
                        reachableProduction.insert(productionNumber);
                    }
                    for (auto symbol : production->getIgredients())
                    {
                        iterator = mapSymbols.find(*symbol);
                        if (iterator != mapSymbols.end() && reachableSymbols.find(iterator->second) == reachableSymbols.end())
                        {
                            reachableSymbols.insert(iterator->second);
                            symbolsStack.push(iterator->second);
                        }
                    }
                }
            }
        }
    } while (symbolsStack.empty() == false);
    for (auto symbol : symbols)
    {
        std::size_t symbolNumber;
        getSymbol(symbol->getName(), symbolNumber);
        if (reachableSymbols.find(symbolNumber) == reachableSymbols.end())
        {
            unusedSymbols.insert(*symbol);
        }
    }
    for (auto production : productions)
    {
        std::size_t productionNumber;
        getProduction(production->getHashableName(), productionNumber);
        if (reachableProduction.find(productionNumber) == reachableProduction.end())
        {
            unusedProductions.insert(*production);
        }
    }
}

void Grammar::checkInfinitiveProductions()
{
    bool changed;
    std::unordered_set<Production> terminatesProductions;
    std::unordered_set<Symbol> terminatesSymbols;
    for (auto symbol : symbols)
    {
        if (symbol->isNonTerminal() == false)
        {
            terminatesSymbols.insert(*symbol);
        }
    }
    do
    {
        changed = false;
        for (auto production : productions)
        {
            bool terminate = true;
            if (terminatesProductions.find(*production) == terminatesProductions.end()
                && unusedProductions.find(*production) == unusedProductions.end())
            {
                for (auto symbol : production->getIgredients())
                {
                    if (symbol->isNonTerminal() && terminatesSymbols.find(*symbol) == terminatesSymbols.end())
                    {
                        terminate = false;
                        break;
                    }
                }
                if (terminate && terminatesSymbols.find(production->getProduct()) == terminatesSymbols.end())
                {
                    changed = true;
                    terminatesSymbols.insert(production->getProduct());
                }
                if (terminate && terminatesProductions.find(*production) == terminatesProductions.end())
                {
                    changed = true;
                    terminatesProductions.insert(*production);
                }
            }
        }
    } while (changed);
    for (auto production : productions)
    {
        if (unusedProductions.find(*production) == unusedProductions.end() && terminatesProductions.find(*production) == terminatesProductions.end())
        {
            infinitivePoductions.insert(*production);
        }
    }
    if (infinitivePoductions.size() > 0)
    {
        throw GrammarException("Some infinitive productions", GrammarException::ErrorCause::infinitive_production);
    }
}


void Grammar::createFirstTable()
{
    for (auto symbol : symbols)
    {
        First[*symbol] = std::vector<const Symbol*>();
        if (symbol->isNonTerminal() == false)
        {
            First[*symbol].push_back(symbol);
        }
    }

    std::unordered_map<Symbol, unordered_set<const Symbol*>> prototypedFirst;
    bool changed;
    bool wasEmptyIgredients;
    const Symbol *emptySymbol = getSymbol("$");

    do
    {
        changed = false;
        for (auto production : productions)
        {
            wasEmptyIgredients = true;
            for (auto igredient : production->getIgredients())
            {
                if (igredient->isNonTerminal())
                {
                    bool isEmptySign = false;
                    for (auto firstElement : First[*igredient])
                    {
                        if (*firstElement == *emptySymbol)
                        {
                            isEmptySign = true;
                        }
                        else
                        {
                            if (prototypedFirst[production->getProduct()].find(firstElement) == 
                                prototypedFirst[production->getProduct()].end())
                            {
                                prototypedFirst[production->getProduct()].insert(firstElement);
                                First[production->getProduct()].push_back(firstElement);
                                changed = true;
                            }
                        }
                    }
                    
                    if (!isEmptySign)
                    {
                        wasEmptyIgredients = false;
                        break;
                    }
                }
                else
                {
                    if (igredient == emptySymbol)
                    {
                        if (production->getIgredients().size() > 1)
                        {
                            throw GrammarException("Empty production has more than one igredient", 
                                GrammarException::ErrorCause::incorrect_empty_production);
                        }
                        break;
                    }
                    if (prototypedFirst[production->getProduct()].find(igredient) == 
                        prototypedFirst[production->getProduct()].end())
                    {
                        prototypedFirst[production->getProduct()].insert(igredient);
                        First[production->getProduct()].push_back(igredient);
                        wasEmptyIgredients = false;
                        changed = true;
                    }
                    break;
                }
            }
            if (wasEmptyIgredients)
            {
                if (prototypedFirst[production->getProduct()].find(emptySymbol) ==
                    prototypedFirst[production->getProduct()].end())
                {
                    prototypedFirst[production->getProduct()].insert(emptySymbol);
                    First[production->getProduct()].push_back(emptySymbol);
                    changed = true;
                }
            }
        }
    } while (changed == true);
}


void Grammar::createFollowTable()
{
    if (First.empty())
    {
        createFirstTable();
    }

    std::unordered_map<Symbol, unordered_set<const Symbol*>> prototypedFollow;
    bool changed;
    bool wasEmptyFirst;
    const Symbol *emptySymbol = getSymbol("$");

    do
    {
        changed = false;
        for (auto production : productions)
        {
            auto igredients = production->getIgredients();
            for (int i = 0; i < production->getIgredients().size(); i++)
            {
                wasEmptyFirst = true;
                for (int j = i + 1; j < production->getIgredients().size(); j++)
                {
                    if (igredients[j]->isNonTerminal())
                    {
                        for (auto firstElement : First[*(igredients[j])])
                        {
                            if (*firstElement == *emptySymbol)
                            {
                                wasEmptyFirst = true;
                            }
                            else
                            {
                                if (prototypedFollow[*(igredients[i])].find(firstElement) ==
                                    prototypedFollow[*(igredients[i])].end())
                                {
                                    prototypedFollow[production->getProduct()].insert(firstElement);
                                    Follow[production->getProduct()].push_back(firstElement);
                                    changed = true;
                                }
                            }
                        }
                    }
                    else
                    {

                    }
                }
                if (wasEmptyFirst)
                {
                    if (prototypedFollow[production->getProduct()].find(emptySymbol) ==
                        prototypedFollow[production->getProduct()].end())
                    {
                        prototypedFollow[production->getProduct()].insert(emptySymbol);
                        Follow[production->getProduct()].push_back(emptySymbol);
                        changed = true;
                    }
                }
            }
        }
    } while (changed == true);
}


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

    delete grammar;
    return nullptr;
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