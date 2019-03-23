
#pragma warning(disable : 4503)
#include "LRTable.h"
#include "TrajanAlgorithm.h"
#include "GrammarException.h"
#include "GrammarBuilder.h"

#include <iostream>

void readRelation(
    const std::pair<const Symbol*, const ParserState*> *transition,
    const std::map<const std::vector<const LRItem*> *, const ParserState*> *statesMap,
    std::unordered_set<const Symbol *> *emptyProducts,
    std::vector<const std::pair<const Symbol*, const ParserState*> *> &symbolSet);
void directReadRelation(
    const std::pair<const Symbol*, const ParserState*> *transition,
    const ParserState *firstState,
    const Grammar *grammar,
    std::unordered_set<const Symbol*> &symbolSet);
void readSet(
    const std::pair<const Symbol*, const ParserState*> *transition,
    const std::map<const std::pair< const Symbol *, const ParserState *> *, unordered_set<const Symbol*>>& readSets,
    std::unordered_set<const Symbol*> &symbolSet);
void includeTransitions(
    const std::pair<const Symbol*, const ParserState*> *transition,
    std::map<std::pair< const Symbol *, const ParserState * >, std::vector<std::pair< const Symbol *, const ParserState * >>>* includesDictionary,
    std::vector<const std::pair< const Symbol *, const ParserState * > *> &transitionSet);

LRTable::LRTable(const Grammar &pGrammar)
    : grammar(pGrammar)
{
    generateItems();
    generateStatesLRO();
    generateLALRlookahead();
    generateTable();
}


LRTable::~LRTable()
{
}

void LRTable::printDebugInfo()
{
    std::string intend = "    ";
    std::cout << "LRTable - generating parsing states and table\n\n"
        << "States:\n";
    for (auto state : states)
    {
        std::cout << intend << "ParserState " << statesNumberMap[state] << ":\n";
        for (auto lrItem : state->getCore())
        {
            std::cout << intend << intend << lrItem->getHashableName() << " {";
            auto tmpLrItem = lrItem;
            while (tmpLrItem->getLookaheads().empty() && (tmpLrItem = tmpLrItem->getNext()));
            if (tmpLrItem)
            {
                for (auto symbol : tmpLrItem->getLookaheads())
                {
                    std::cout << " " << symbol->getName() << ";";
                }
            }
            std::cout << " }\n";
        }
        for (auto lrItem : state->getExtension())
        {
            std::cout << intend << intend << lrItem->getHashableName() << " {";
            auto tmpLrItem = lrItem;
            while (tmpLrItem->getLookaheads().empty() && (tmpLrItem = tmpLrItem->getNext()));
            for (auto symbol : tmpLrItem->getLookaheads())
            {
                std::cout << " " << symbol->getName() << ";";
            }
            std::cout << " }\n";
        }
        std::cout << intend << intend << "Rules from state:\n";
        for (auto symbol : state->getAteSymbols())
        {
            std::cout << intend << intend << intend << "Next state " << statesNumberMap[statesMap[state->getNextStateCore(*symbol)]]
                << " for " << symbol->getName() << ".\n";
        }
        std::cout << "\n";
    }
}

bool LRTable::getAction(const std::pair<std::size_t, std::size_t> &actionDef, std::pair< LRTable::Action, std::size_t> &moveProps)
{
	if (actionDef.second >= states.size())
	{
		return false;
	}
	std::pair<const Symbol *, const ParserState *> keyDef = std::make_pair(grammar.get().getSymbol(actionDef.first), states[actionDef.second]);
	auto it = actionTable.find(keyDef);
	if (it != actionTable.end())
	{
		moveProps = it->second;
		return true;
	}
	return false;
}

bool LRTable::getGoto(const std::pair<std::size_t, std::size_t> &gotoActionDef, std::size_t &stateIndex)
{
	auto production = grammar.get().getProduction(gotoActionDef.first);
	if (gotoActionDef.second >= states.size() || production == nullptr)
	{
		return false;
	}
	std::pair<const Symbol *, const ParserState *> keyDef = std::make_pair(&(production->getProduct()), states[gotoActionDef.second]);
	auto it = gotoTable.find(keyDef);
	if (it != gotoTable.end())
	{
		stateIndex = it->second;
		return true;
	}
	return false;
}

//bool LRTable::parse(std::vector<std::string> &tokens, ElementTree *&result)
//{
//	result = nullptr;
//	std::vector<unsigned int> stateStack;
//	std::vector<std::unique_ptr<ElementTree>> elementTreeStack;
//	stateStack.push_back(0);
//	bool isEnd = false;
//	unsigned int i = 0;
//	while (isEnd == false)
//	{
//		std::string token = "$";
//		if (i < tokens.size())
//		{
//			token = tokens[i];
//		}
//		unsigned int actualState = stateStack.back();
//		unsigned int symbolId = 0;
//		bool isValidToken = grammar.get().getSymbol(token, symbolId);
//		if (isValidToken == false)
//		{
//			throw std::runtime_error("Provided token is not valid");
//		}
//		std::pair<const Symbol *, const ParserState *> actionDef = std::make_pair(grammar.get().getSymbol(symbolId), states[actualState]);
//		auto it = actionTable.find(actionDef);
//		if (it != actionTable.end())
//		{
//			auto moveProps = it->second;
//			if (moveProps.first == Action::reduce)
//			{
//				std::vector<std::unique_ptr<ElementTree>> childreen;
//				unsigned int reducedProductionIndex = it->second.second;
//				auto production = grammar.get().getProduction(reducedProductionIndex);
//				unsigned int productionIngredientsSize = production->getIgredients().size();
//				for (unsigned int j = 0; j < productionIngredientsSize; j++)
//				{
//					stateStack.pop_back();
//					childreen.emplace_back(elementTreeStack.back().release());
//					elementTreeStack.pop_back();
//				}
//				elementTreeStack.emplace_back(new ElementTree(grammar.get().getProduction(it->second.second)->getProduct(), std::move(childreen)));
//				std::pair<const Symbol *, const ParserState *> gotoActionDef = std::make_pair(&((const Symbol&)grammar.get().getProduction(it->second.second)->getProduct()), states[stateStack.back()]);
//				auto gotoIt = gotoTable.find(gotoActionDef);
//				if (gotoIt != gotoTable.end())
//				{
//					stateStack.push_back(gotoIt->second);
//				}
//				else
//				{
//					throw std::runtime_error("Provided nonterminal is not consistent with the grammar rules.");
//				}
//			}
//			else if (moveProps.first == Action::shift)
//			{
//				std::vector<std::unique_ptr<ElementTree>> childreen;
//				stateStack.push_back(it->second.second);
//				elementTreeStack.emplace_back(new ElementTree(*actionDef.first, std::move(childreen)));
//				i++;
//			}
//			else
//			{
//				isEnd = true;
//			}
//		}
//		else
//		{
//			throw std::runtime_error("Provided tokens are not consistent with the grammar rules.");
//		}
//	}
//	result = elementTreeStack.back().release();
//	return true;
//}

void LRTable::generateItems()
{
    const Symbol *start = grammar.get().getStartNonTerminal();
    const std::vector<const Production *> *productions = grammar.get().getProductions(start);
    std::stack<LRItem *> inputStack;
	std::size_t numLrItems = 0;
    for (auto production : *productions)
    {
        items.emplace_back(new LRItem(production, 0, numLrItems++));
        inputStack.push(items.back().get());
        if (itemsMap.find(items.back()->getProduction()) == itemsMap.end())
        {
            itemsMap[items.back()->getProduction()] = std::vector<LRItem*>();
        }
        itemsMap[items.back()->getProduction()].push_back(items.back().get());
    }
    do
    {
        auto input = inputStack.top();
        inputStack.pop();
        auto production = input->getProduction();
        auto igredients = production->getIgredients();
        if (input->getPosition() < igredients.size())
        {
            items.emplace_back(new LRItem(input->getProduction(), input->getPosition() + 1, numLrItems++));
            inputStack.push(items.back().get());
            if (itemsMap.find(items.back()->getProduction()) == itemsMap.end())
            {
                itemsMap[input->getProduction()] = std::vector<LRItem*>();
            }
            itemsMap[input->getProduction()].push_back(items.back().get());
            items.back()->setPrevious(input);
            input->setNext(items.back().get());
            auto childreen = grammar.get().getProductions(igredients[input->getPosition()]);
            if (childreen != nullptr)
            {
                for (auto child : *childreen)
                {
                    auto iterator = itemsMap.find(child);
                    if (iterator == itemsMap.end())
                    {
                        items.emplace_back(new LRItem(child, 0, numLrItems++));
                        inputStack.push(items.back().get());
                        if (itemsMap.find(items.back()->getProduction()) == itemsMap.end())
                        {
                            itemsMap[items.back()->getProduction()] = std::vector<LRItem*>();
                        }
                        itemsMap[items.back()->getProduction()].push_back(items.back().get());
                        input->addChild(items.back().get());
                    }
                    else
                    {
                        auto existingLrItems = itemsMap.find(child);
                        input->addChild(existingLrItems->second[0]);
                    }
                }
            }
            
        }
    } while (!inputStack.empty());

}


void LRTable::generateStatesLRO()
{
    const Symbol *start = grammar.get().getStartNonTerminal();
    const std::vector<const Production *> *productions = grammar.get().getProductions(start);
    std::vector<const LRItem *> firstItems;
    std::queue<ParserState*> statesQueue;
    for (auto production : *productions)
    {
        auto iterator = itemsMap.find(production);
        if (iterator != itemsMap.end())
        {
            firstItems.push_back(iterator->second[0]);
        }
    }
    statesQueue.push(new ParserState(firstItems));
    states.push_back(statesQueue.back());
    statesMap[&(states.back()->getCore())] = statesQueue.back();
    do
    {
        auto argument = statesQueue.front();
        statesQueue.pop();
        for (auto symbol : argument->getAteSymbols())
        {
            auto core = argument->getNextStateCore(*symbol);
            if (statesMap.find(core) == statesMap.end())
            {
                statesQueue.push(new ParserState(*(argument->getNextStateCore(*symbol))));
                statesNumberMap[statesQueue.back()] = states.size();
                statesQueue.back()->setPrevious(argument);
                states.push_back(statesQueue.back());
                statesMap[core] = statesQueue.back();
            }
        }
    } while (!statesQueue.empty());
}

void LRTable::generateLALRlookahead()
{
    takeEmptyProducts();
    takeNonTerminalTransitions();
    generateReadSets();
    generateLookbacksAndIncludes();
    generateFollowSets();
    addLookaheads();
}

void LRTable::generateReadSets()
{
    std::function<void(const std::pair< const Symbol *, const ParserState *> *, std::vector<const std::pair< const Symbol *, const ParserState *> *>&)> func2 
        = std::bind(readRelation, std::placeholders::_1, &statesMap, &emptyProducts, std::placeholders::_2);
    std::function<void(const std::pair< const Symbol *, const ParserState *> *, std::unordered_set<const Symbol *>&)> func1
        = std::bind(directReadRelation, std::placeholders::_1, states[0], &grammar.get(), std::placeholders::_2);;
    TrajanAlgorithm(nonTerminalTransitions, func1, func2, readSets);
}

void LRTable::generateFollowSets()
{
    std::function<void(const std::pair< const Symbol *, const ParserState *> *, std::vector<const std::pair< const Symbol *, const ParserState *> *>&)> func2 
        = std::bind(includeTransitions, std::placeholders::_1, &includesDictionary, std::placeholders::_2);
    std::function<void(const std::pair< const Symbol *, const ParserState *> *, std::unordered_set<const Symbol *>&)> func1
        = std::bind(readSet, std::placeholders::_1, readSets, std::placeholders::_2);
    TrajanAlgorithm(nonTerminalTransitions, func1, func2, followSets);
}

void LRTable::takeEmptyProducts()
{
    auto productions = grammar.get().getAllProductions();
    for (auto production : productions)
    {
        if (production->getIgredients().size() == 0)
        {
            emptyProducts.insert(&(production->getProduct()));
        }
    }
}

void LRTable::takeNonTerminalTransitions()
{
    for (auto state : states)
    {
        for (auto symbol : state->getAteSymbols())
        {
            if (symbol->isNonTerminal() && 
                nonTerminalTransitions.find(&std::pair< const Symbol *, const ParserState *>(symbol, state)) == nonTerminalTransitions.end())
            {
                nonTerminalTransitions.insert(new std::pair< const Symbol *, const ParserState *>(symbol, state));
            }
        }
    }
}

void LRTable::generateLookbacksAndIncludes()
{
    for (auto transition : nonTerminalTransitions)
    {
        auto state = transition->second;
        for (auto lrItem : transition->second->getExtension())
        {
            if (!(*(transition->first) == lrItem->getProduction()->getProduct()))
            {
                continue;
            }
            auto tmpState = state;
            while (lrItem->getSymbol() != nullptr)
            {
                auto tmpTransition = std::pair< const Symbol *, const ParserState * >(lrItem->getSymbol(), tmpState);
                if (nonTerminalTransitions.find(&tmpTransition) != nonTerminalTransitions.end())
                {
                    auto tmpLrItem = lrItem->getNext();
                    while (tmpLrItem->getSymbol())
                    {
                        
                        if (! tmpLrItem->getSymbol()->isNonTerminal())
                        {
                            break;
                        }
                        if (emptyProducts.find(tmpLrItem->getSymbol()) == emptyProducts.end())
                        {
                            break;
                        }
                        tmpLrItem = tmpLrItem->getNext();
                    }
                    if (tmpLrItem->getSymbol() == nullptr)
                    {
                        if (includesDictionary.find(tmpTransition) == includesDictionary.end())
                        {
                            includesDictionary[tmpTransition] = std::vector < std::pair< const Symbol *, const ParserState * >>();
                        }
                        includesDictionary[tmpTransition].push_back(*transition);
                    }
                }
                tmpState = statesMap[tmpState->getNextStateCore(*(lrItem->getSymbol()))];
                lrItem = lrItem->getNext();
            }
            for (auto tmpLrItem : tmpState->getCore())
            {
                if (*(tmpLrItem->getProduction()) == *(lrItem->getProduction()))
                {
                    auto lookback = std::pair< const LRItem *, const ParserState * >(tmpLrItem, tmpState);
                    lookBacksDictionary[*transition].push_back(lookback);
                }
            }
            for (auto tmpLrItem : tmpState->getExtension())
            {
                if (*(tmpLrItem->getProduction()) == *(lrItem->getProduction()))
                {
                    auto lookback = std::pair< const LRItem *, const ParserState * >(tmpLrItem, tmpState);
                    lookBacksDictionary[*transition].push_back(lookback);
                }
            }
            
        }
    }
}

void LRTable::addLookaheads()
{
    for (auto lookback : lookBacksDictionary)
    {
        for (auto transition : lookback.second)
        {
            for (auto follow : followSets[&(lookback.first)])
            {
                const_cast<LRItem*>(transition.first)->addLookahead(transition.second, follow);
            }
        }
    }
    for (auto lrItem : states[0]->getCore())
    {
        const ParserState *state = states[0];
        while (lrItem->getNext())
        {
            state = statesMap[state->getNextStateCore(*(lrItem->getSymbol()))];
            lrItem = lrItem->getNext();
        }
        const_cast<LRItem*>(lrItem)->addLookahead(state, grammar.get().getSymbol("$"));
        actionTable[make_pair(grammar.get().getSymbol("$"), state)] = make_pair(Action::accept, 0);
    }
}

void LRTable::generateTable()
{
    std::unordered_map<const Production*, unsigned int> productionReduceNumber;
    for (auto production : grammar.get().getAllProductions())
    {
        productionReduceNumber[production] = 0;
    }
    for (auto state : states)
    {
        for (auto symbol : state->getAteSymbols())
        {
            auto nextState = statesMap[state->getNextStateCore(*symbol)];
            if (symbol->isNonTerminal())
            {
                gotoTable[std::make_pair(symbol, state)] = statesNumberMap[nextState];
            }
            else
            {
                actionTable[std::make_pair(symbol, state)] = std::make_pair(Action::shift, statesNumberMap[nextState]);
            }
        }
        for (auto reducedItem : state->getReducedLRItems())
        {
            std::size_t productionNumber;
            grammar.get().getProduction(reducedItem->getProduction()->getHashableName(), productionNumber);
			// the loop is repeated for LALR - lookaheads, SLR - follows, LR(0) - all terminals
            for (auto lookahead : reducedItem->getLookaheads())
            {
                auto iterator = actionTable.find(std::make_pair(lookahead, state));
                if (iterator == actionTable.end())
                {
                    productionReduceNumber[reducedItem->getProduction()] += 1;
					actionTable[std::make_pair(lookahead, state)] = std::make_pair(Action::reduce, productionNumber);
                }
                else if (iterator->second.first == Action::shift)
                {
                    auto productionPrec = reducedItem->getProduction()->getPrecedence();
                    auto symbolPrec = lookahead->getPrecedence();
                    auto productionPrority = productionPrec ? productionPrec->getPriority() : 0;
                    auto symbolPrority = symbolPrec ? symbolPrec->getPriority() : 0;
                    auto productionAssociation = productionPrec ? productionPrec->getAssociation() : Precedence::RIGHT;
                    if (symbolPrority == productionPrority && productionAssociation == Precedence::NONASSOC)
                    {
                        std::string errorMessage = "Nonassoc priority for production cannot be resolved for state " + statesNumberMap[state];
                        throw GrammarException(errorMessage, GrammarException::ErrorCause::unknown_conflict);
                    }
                    else if (symbolPrority < productionPrority ||
                        (symbolPrority == productionPrority && productionAssociation == Precedence::LEFT))
                    {
                        actionTable[std::make_pair(lookahead, state)] = std::make_pair(Action::reduce, productionNumber);
                        srConflicts.push_back(std::make_tuple(state, productionNumber, iterator->second.second, Action::reduce));
                    }
                    else
                    {
                        srConflicts.push_back(std::make_tuple(state, iterator->second.second, productionNumber, Action::shift));
                    }
                }
                else if (iterator->second.first == Action::reduce)
                {
                    if (productionNumber < iterator->second.second)
                    {
                        productionReduceNumber[reducedItem->getProduction()] += 1;
                        productionReduceNumber[grammar.get().getProduction(iterator->second.second)] -= 1;
                        actionTable[std::make_pair(lookahead, state)] = std::make_pair(Action::reduce, productionNumber);
                        rrConflicts.push_back(std::make_tuple(state, productionNumber, iterator->second.second));
                    }
                    else
                    {
                        rrConflicts.push_back(std::make_tuple(state, iterator->second.second, productionNumber));
                    }
                }
                else
                {
                    if ((*grammar.get().getStartNonTerminal()) == reducedItem->getProduction()->getProduct())
                    {
                        continue;
                    }
                    else
                    {
                        std::string errorMessage = "Unknown conflict for state " + statesNumberMap[state];
                        throw GrammarException(errorMessage, GrammarException::ErrorCause::unknown_conflict);
                    }
                }
            }
        }
    }
    for (auto it = productionReduceNumber.begin(); it != productionReduceNumber.end(); it++)
    {
        if (it->second == 0)
        {
            unreducedProductions.push_back(it->first);
        }
    }
}

void directReadRelation(
    const std::pair<const Symbol*, const ParserState*> *transition, 
    const ParserState *firstState,
    const Grammar *grammar,
    std::unordered_set<const Symbol*> &symbolSet)
{
    auto transitionProducts = transition->second->getNextStateCore(*(transition->first));
    if (transitionProducts)
    {
        for (auto lrItem : *transitionProducts)
        {
            auto symbol = lrItem->getSymbol();
            if (symbol && !symbol->isNonTerminal())
            {
                symbolSet.insert(symbol);
            }
        }
    }
    if (*firstState == *(transition->second))
    {
        for (auto production : *(grammar->getProductions(grammar->getStartNonTerminal())))
        {
            if (production->getIgredients()[0] == transition->first)
            {
                symbolSet.insert(grammar->getSymbol("$"));
                break;
            }
        }
    }
}


void readRelation(
    const std::pair<const Symbol*, const ParserState*> *transition, 
    const std::map<const std::vector<const LRItem*> *, const ParserState*> *statesMap,
    std::unordered_set<const Symbol *> *emptyProducts,
    std::vector<const std::pair<const Symbol*, const ParserState*> *> &symbolSet)
{
    auto transitionProducts = transition->second->getNextStateCore(*(transition->first));
    if (transitionProducts)
    {
        auto nextState = statesMap->find(transitionProducts);
        for (auto lrItem : *transitionProducts)
        {
            auto symbol = lrItem->getSymbol();
            if (symbol && emptyProducts->find(symbol) != emptyProducts->end())
            {
                symbolSet.push_back(transition);
            }
        }
    }
}

void readSet(
    const std::pair<const Symbol*, const ParserState*> *transition,
    const std::map<const std::pair< const Symbol *, const ParserState *> *, unordered_set<const Symbol*>>& readSets,
    std::unordered_set<const Symbol*> &symbolSet)
{
    if (transition)
    {
        auto symbols = readSets.find(transition);
        if (symbols != readSets.end())
        {
            for (auto symbol : symbols->second)
            {
                symbolSet.insert(symbol);
            }
        }
    }
}

void includeTransitions(
    const std::pair<const Symbol*, const ParserState*> *transition,
    std::map<std::pair< const Symbol *, const ParserState * >, std::vector<std::pair< const Symbol *, const ParserState * >>>* includesDictionary,
    std::vector<const std::pair< const Symbol *, const ParserState * > *> &transitionSet)
{
    if (transition)
    {
        auto relatedTransitions = includesDictionary->find(*transition);
        if (relatedTransitions != includesDictionary->end())
        {
            for (size_t i = 0; i < relatedTransitions->second.size(); i++)
            {
                transitionSet.push_back(&(relatedTransitions->second[i]));
            }
        }
    }
}
