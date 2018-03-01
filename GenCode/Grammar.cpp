#include "Grammar.h"
#include "GrammarException.h"

#include <stack>
#include <set>
#include <iostream>


const Symbol *Grammar::getSymbol(const std::string &name) const
{
    const Symbol *result = nullptr;
    auto terminalIterator = mapSymbolsString.find(name);
    if (terminalIterator != mapSymbolsString.end())
    {
        result = symbols[terminalIterator->second];
    }
    return result;
}

const Precedence *Grammar::getPrecedence(const std::string &name) const
{
    const Precedence *result = nullptr;
    auto precedenceIterator = mapPrecedencesString.find(name);
    if (precedenceIterator != mapPrecedencesString.end())
    {
        result = precedences[precedenceIterator->second].first;
    }
    return result;
}

const Production *Grammar::getProduction(const std::string &name) const
{
    const Production *result = nullptr;
    auto productionIterator = mapProductionsString.find(name);
    if (productionIterator != mapProductionsString.end())
    {
        result = productions[productionIterator->second];
    }
    return result;
}


const Symbol *Grammar::getSymbol(const std::size_t &number) const
{
    if (number < symbols.size())
    {
        return symbols[number];
    }
    return nullptr;
}


const Production *Grammar::getProduction(const std::size_t &number) const
{
    if (number < productions.size())
    {
        return productions[number];
    }
    return nullptr;
}


const Precedence *Grammar::getPrecedence(const std::size_t &number) const
{
    if (number < precedences.size())
    {
        return precedences[number].first;
    }
    return nullptr;
}


bool Grammar::getSymbol(const std::string &name, std::size_t &result) const
{
    auto symbolIterator = mapSymbolsString.find(name);
    if (symbolIterator != mapSymbolsString.end())
    {
        result = symbolIterator->second;
        return true;
    }
    return false;
}


bool Grammar::getProduction(const std::string &name, std::size_t &result) const
{
    auto productionIterator = mapProductionsString.find(name);
    if (productionIterator != mapProductionsString.end())
    {
        result = productionIterator->second;
        return true;
    }
    return false;
}


bool Grammar::getPrecedence(const std::string &name, std::size_t &result) const
{
    auto precedenceIterator = mapPrecedencesString.find(name);
    if (precedenceIterator != mapPrecedencesString.end())
    {
        result = precedenceIterator->second;
        return true;
    }
    return false;
}


const vector<Production *> &Grammar::getAllProductions() const
{ 
    return productions; 
}

const vector<Symbol *> &Grammar::getAllSymbols() const
{ 
    return symbols; 
}

const vector<const Production *> *Grammar::getProductions(const Symbol* product) const
{
    auto definedProductions = mapProductToProductions.find(product);
    if (definedProductions != mapProductToProductions.end())
    {
        return &(definedProductions->second);
    }
    else
    {
        return nullptr;
    }
}

const NonTerminal* Grammar::getStartNonTerminal() const
{
    return start;
}

void Grammar::printDebugInfo() const
{
    std::string intend = "    ";
    std::cout << "Grammar - final input analisys\n\n"
        << "Symbols:\n";
    for (auto symbol : symbols)
    {
        if (!symbol->isNonTerminal())
        {
            std::cout << intend << "-------Terminal-------\n"
                << intend << "Name: " << symbol->getName() << "\n"
                << intend << "Pattern: " << *(symbol->getPattern()) << "\n";
        }
        else
        {
            std::cout << intend << "-------Nonterminal-------\n"
                << intend << "Name: " << symbol->getName() << "\n";
            auto iterator = mapProductToProductions.find(symbol);
            if (iterator != mapProductToProductions.end())
            {
                std::cout << intend << "Productions:\n";
                for (auto production : iterator->second)
                {
                    std::cout << intend << intend << production->getHashableName() << "\n";
                    if (production->getPrecedence() != nullptr)
                    {
                        auto precedence = production->getPrecedence();
                        std::cout << intend << intend << intend << "Precedence: (Name: " << precedence->getName() << "; Type: " << precedence->getAssociation()
                            << "; Level: " << precedence->getPriority() << ")\n";
                    }
                }
            }
        }
        if (symbol->getPrecedence() != nullptr)
        {
            auto precedence = symbol->getPrecedence();
            std::cout << intend << "Precedence: (Name: " << precedence->getName() << "; Type: " << precedence->getAssociation()
                << "; Level: " << precedence->getPriority() << ")\n";
        }
        auto firstElements = First.find(symbol);
        if (firstElements != First.end())
        {
            std::cout << intend << "First: ";
            for (auto element : firstElements->second)
            {
                std::cout << element->getName() << ", ";
            }
            std::cout << "\n";
        }
        auto followElements = Follow.find(symbol);
        if (followElements != Follow.end())
        {
            std::cout << intend << "Follow: ";
            for (auto element : followElements->second)
            {
                std::cout << element->getName() << "; ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
    if (unusedPrecedences.size() > 0)
    {
        std::cout << "Unused precedences:\n" << intend;
        for (auto precedence : precedences)
        {
            std::cout << precedence.second << "; ";
        }
        std::cout << "\n\n";
    }
    else
    {
        std::cout << "There are not any unused precedences.\n\n";
    }
    if (unusedSymbols.size() > 0)
    {
        std::cout << "Unused symbols:\n" << intend;
        for (auto symbol : unusedSymbols)
        {
            std::cout << symbol->getName() << "; ";
        }
        std::cout << "\n\n";
    }
    else
    {
        std::cout << "There are not any unused symbols.\n\n";
    }
    if (unusedProductions.size() > 0)
    {
        std::cout << "Unreachable productions:\n";
        for (auto production : unusedProductions)
        {
            std::cout << intend << production->getHashableName() << "\n";
        }
        std::cout << "\n";
    }
    else
    {
        std::cout << "There are not any unreachable productions.\n\n";
    }
    if (infinitivePoductions.size() > 0)
    {
        std::cout << "Infinitive productions:\n";
        for (auto production : infinitivePoductions)
        {
            std::cout << intend << production->getHashableName() << "\n";
        }
        std::cout << "\n";
    }
    else
    {
        std::cout << "There are not any infinitive productions.\n\n";
    }
}

Grammar::Grammar()
{
    Grammar::insertTerminal(
        "$",
        "\\0",
        "end");
    Grammar::insertTerminal(
        "%empty",
        "",
        "empty");
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
            mapSymbols[newSymbol] = symbols.size() - 1;
            mapSymbolsString[newSymbol->getName()] = mapSymbols[newSymbol];
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
            mapSymbols[newSymbol] = mapSymbols[newSymbol];
            mapSymbolsString[newSymbol->getName()] = mapSymbols[newSymbol];
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
            mapSymbols[newSymbol] = symbols.size() - 1;
            mapSymbolsString[newSymbol->getName()] = mapSymbols[newSymbol];
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
        Symbol *newSymbol = new Terminal(name, pattern, type);
        symbols.push_back(newSymbol);
        mapSymbols[newSymbol] = symbols.size() - 1;
        mapSymbolsString[newSymbol->getName()] = mapSymbols[newSymbol];
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
        mapPrecedences[newPrecedence] = precedences.size() - 1;
        mapPrecedencesString[newPrecedence->getName()] = mapPrecedences[newPrecedence];
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
    bool emptyProduction = false;
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
            else if (symbol->getName() == "%empty")
            {
                symbol = nullptr;
                emptyProduction = true;
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
        if (symbol)
        {
            productionIgredients.push_back(symbol);
        }
    }
    if ((productionIgredients.empty() && !emptyProduction) | (emptyProduction && !productionIgredients.empty()))
    {
        throw GrammarException("an incorrect empty production.", GrammarException::ErrorCause::incorrect_empty_production_declaration);
    }
    Production *production = new Production(*nonTerminal, productionIgredients, precedence);
    if (mapProductionsString.find(production->getHashableName()) == mapProductionsString.end())
    {
        productions.push_back(production);
        mapProductions[production] = productions.size() - 1;
        mapProductionsString[production->getHashableName()] = mapProductions[production];
        mapProductToProductions[nonTerminal].push_back(production);
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

const std::vector<const Symbol*> *Grammar::getFirst(const Symbol *symbol) const
{
    auto iterator = First.find(symbol);
    if (iterator != First.end())
    {
        return &(iterator->second);
    }
    else
    {
        return nullptr;
    }
}

const std::vector<const Symbol*> *Grammar::getFollow(const Symbol *symbol) const
{
    auto iterator = Follow.find(symbol);
    if (iterator != Follow.end())
    {
        return &(iterator->second);
    }
    else
    {
        return nullptr;
    }
}

void Grammar::checkUnusedPrecedences()
{
    for (auto precedence : precedences)
    {
        if (precedence.second)
        {
            std::size_t precedenceNumber;
            getPrecedence(precedence.first->getName(), precedenceNumber);
            unusedPrecedences.insert(precedence.first);
        }
    }
}

void Grammar::checkUnusedSymbolsAndProductions()
{
    std::set<std::size_t> reachableProduction;
    std::set<std::size_t> reachableSymbols;
    auto iterator = mapSymbols.find(start);
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
            auto productionIterator = mapProductToProductions.find(symbol);
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
                        iterator = mapSymbols.find(symbol);
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
            unusedSymbols.insert(symbol);
        }
    }
    for (auto production : productions)
    {
        std::size_t productionNumber;
        getProduction(production->getHashableName(), productionNumber);
        if (reachableProduction.find(productionNumber) == reachableProduction.end())
        {
            unusedProductions.insert(production);
        }
    }
}

void Grammar::checkInfinitiveProductions()
{
    bool changed;
    std::unordered_set<const Production *> terminatesProductions;
    std::unordered_set<const Symbol *> terminatesSymbols;
    for (auto symbol : symbols)
    {
        if (symbol->isNonTerminal() == false)
        {
            terminatesSymbols.insert(symbol);
        }
    }
    do
    {
        changed = false;
        for (auto production : productions)
        {
            bool terminate = true;
            if (terminatesProductions.find(production) == terminatesProductions.end()
                && unusedProductions.find(production) == unusedProductions.end())
            {
                for (auto symbol : production->getIgredients())
                {
                    if (symbol->isNonTerminal() && terminatesSymbols.find(symbol) == terminatesSymbols.end())
                    {
                        terminate = false;
                        break;
                    }
                }
                if (terminate && terminatesSymbols.find(&(production->getProduct())) == terminatesSymbols.end())
                {
                    changed = true;
                    terminatesSymbols.insert(&(production->getProduct()));
                }
                if (terminate && terminatesProductions.find(production) == terminatesProductions.end())
                {
                    changed = true;
                    terminatesProductions.insert(production);
                }
            }
        }
    } while (changed);
    for (auto production : productions)
    {
        if (unusedProductions.find(production) == unusedProductions.end() && terminatesProductions.find(production) == terminatesProductions.end())
        {
            infinitivePoductions.insert(production);
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
        First[symbol] = std::vector<const Symbol*>();
        if (symbol->isNonTerminal() == false)
        {
            First[symbol].push_back(symbol);
        }
    }

    std::unordered_map<const Symbol*, std::unordered_set<const Symbol*>> prototypedFirst;
    bool changed;
    bool wasEmptyIgredients;
    const Symbol *emptySymbol = getSymbol("%empty");

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
                    for (auto firstElement : First[igredient])
                    {
                        if (firstElement == emptySymbol)
                        {
                            isEmptySign = true;
                        }
                        else
                        {
                            if (prototypedFirst[&(production->getProduct())].find(firstElement) ==
                                prototypedFirst[&(production->getProduct())].end())
                            {
                                prototypedFirst[&(production->getProduct())].insert(firstElement);
                                First[&(production->getProduct())].push_back(firstElement);
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
                    if (prototypedFirst[&(production->getProduct())].find(igredient) ==
                        prototypedFirst[&(production->getProduct())].end())
                    {
                        prototypedFirst[&(production->getProduct())].insert(igredient);
                        First[&(production->getProduct())].push_back(igredient);
                        changed = true;
                    }
                    wasEmptyIgredients = false;
                    break;
                }
            }
            if (wasEmptyIgredients)
            {
                if (prototypedFirst[&(production->getProduct())].find(emptySymbol) ==
                    prototypedFirst[&(production->getProduct())].end())
                {
                    prototypedFirst[&(production->getProduct())].insert(emptySymbol);
                    First[&(production->getProduct())].push_back(emptySymbol);
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

    std::unordered_map<const Symbol *, unordered_set<const Symbol*>> prototypedFollow;
    bool changed = false;
    bool wasEmptyFirst = false;
    const Symbol *emptySymbol = getSymbol("%empty");
	Follow[&(productions[0]->getProduct())].push_back(getSymbol("$"));

    do
    {
        changed = false;
        for (auto production : productions)
        {
            auto igredients = production->getIgredients();
            for (std::size_t i = 0; i < production->getIgredients().size(); i++)
            {
				if (igredients[i]->isNonTerminal())
				{
					wasEmptyFirst = true;
					for (std::size_t j = i + 1; j < production->getIgredients().size(); j++)
					{
						if (igredients[j]->isNonTerminal())
						{
							wasEmptyFirst = false;
							for (auto firstElement : First[igredients[j]])
							{
								if (firstElement == emptySymbol)
								{
									wasEmptyFirst = true;
								}
								else
								{
									if (prototypedFollow[igredients[i]].find(firstElement) ==
										prototypedFollow[igredients[i]].end())
									{
										prototypedFollow[igredients[i]].insert(firstElement);
										Follow[igredients[i]].push_back(firstElement);
										changed = true;
									}
								}
							}
							if (wasEmptyFirst == false)
							{
								break;
							}
						}
						else
						{
							if (prototypedFollow[igredients[i]].find(igredients[j]) ==
								prototypedFollow[igredients[i]].end())
							{
								prototypedFollow[igredients[i]].insert(igredients[j]);
								Follow[igredients[i]].push_back(igredients[j]);
								changed = true;
							}
							wasEmptyFirst = false;
							break;
						}
					}
					if (wasEmptyFirst)
					{
						for (auto followElement : Follow[&(production->getProduct())])
						{
							if (prototypedFollow[igredients[i]].find(followElement) ==
								prototypedFollow[igredients[i]].end())
							{
								prototypedFollow[igredients[i]].insert(followElement);
								Follow[igredients[i]].push_back(followElement);
								changed = true;
							}
						}
					}
				}
            }
        }
    } while (changed == true);
}