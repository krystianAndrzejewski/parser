#include "LRItem.h"
#include "State.h"
#include <iterator>

std::size_t LRItem::amoutOfLRItems = 0;

LRItem::LRItem(const Production *pProduction, const std::size_t pPosition, const std::size_t pNumber)
    : production(pProduction), 
    position(pPosition),
    previous(nullptr),
	number(pNumber),
    next(nullptr)
{
    hashableName = production->getProduct().getName();
    hashableName += " ->";
    for (size_t i = 0; i < production->getIgredients().size() + 1; i++)
    {
        if (i == position)
        {
            hashableName += " .";
        }
        if (i < production->getIgredients().size())
        {
            hashableName += " " + production->getIgredients()[i]->getName();
        }
    }
}


LRItem::~LRItem()
{
}

const Symbol *LRItem::getSymbol() const
{
    if (position < production->getIgredients().size())
    {
        return production->getIgredients()[position];
    }
    else
    {
        return nullptr;
    }
}

void LRItem::addLookahead(const ParserState *pState, const Symbol *plookahead)
{
    lookaheads.insert(plookahead);
    lookaheadsSet[pState].insert(plookahead);
}

void LRItem::addChild(const LRItem *child) 
{ 
    childreen.push_back(child); 
}

void LRItem::addChild(const std::vector<const LRItem*> &pchildreen)
{
    std::copy(pchildreen.begin(), pchildreen.end(), std::back_inserter(childreen));
}
