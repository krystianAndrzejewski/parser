#include "State.h"
#include <queue>
#include <unordered_set>
#include <iterator>


ParserState::ParserState(const std::vector<const LRItem*> &pCore)
{
    core.reserve(pCore.size());
    std::copy(pCore.begin(), pCore.end(), std::back_inserter(core));
	std::sort(core.begin(), core.end());
    generateExtension();
}


ParserState::~ParserState()
{
}


void ParserState::generateExtension()
{
    std::queue<const LRItem *> elements;
    for (auto coreElement : core)
    {
        elements.push(coreElement);
        auto symbol = coreElement->getSymbol();
        if (symbol && nextStates.find(symbol) == nextStates.end())
        {
            nextStates[symbol] = std::vector<const LRItem*>();
            ateSymbols.push_back(symbol);
        }
        else if (symbol == nullptr)
        {
            reducedItems.push_back(coreElement);
        }
        if (coreElement->getNext())
        {
            nextStates[symbol].push_back(coreElement->getNext());
        }
    }
    const LRItem * actualItem;
    do
    {
        actualItem = elements.back();
        elements.pop();
        auto childreen = actualItem->getClosure();
        if (!childreen.empty())
        {
            for (auto child : childreen)
            {
                elements.push(child);
                extension.push_back(child);
                auto symbol = child->getSymbol();
                if (symbol && nextStates.find(symbol) == nextStates.end())
                {
                    nextStates[symbol] = std::vector<const LRItem*>();
                    ateSymbols.push_back(symbol);
                }
                else if(symbol == nullptr)
                {
                    reducedItems.push_back(child);
                }
                if (child->getNext())
                {
                    nextStates[symbol].push_back(child->getNext());
                }
            }
        }
    } while (!elements.empty());

	for (auto &nextState : nextStates)
	{
		std::sort(nextState.second.begin(), nextState.second.end());
	}
}

void ParserState::setPrevious(const ParserState *pPrevious)
{
    previousStates.push_back(pPrevious);
}