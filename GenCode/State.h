#pragma once
#include "LRItem.h"
#include <unordered_map>

class State
{
public:
    State(const std::vector<const LRItem*> &pCore);
    ~State();

    const std::vector<const LRItem*> &getCore() const { return core; }
    const std::vector<const LRItem*> &getExtension() const { return extension; }
    const std::vector<const Symbol*> &getAteSymbols() const { return ateSymbols; }
    const std::vector<const LRItem*> &getReducedLRItems() const { return reducedItems; }
    const std::vector<const LRItem*> *getNextStateCore(const Symbol &symbol) const
    {
        auto iterator = nextStates.find(&symbol);
        if (iterator != nextStates.end())
        {
            return &(iterator->second);
        }
        else
        {
            return nullptr;
        }
    }

    bool operator< (const State &rhs) const
    {
        return core < rhs.getCore();
    }

    bool operator< (const std::vector<const LRItem*> &rhs)
    {
        return core < rhs;
    }

    bool operator== (const State &rhs) const
    {
        return core == rhs.getCore();
    }

    bool operator== (const std::vector<const LRItem*> &rhs)
    {
        return core == rhs;
    }

    void setPrevious(const State *pPrevious);

private:

    void generateExtension();

    std::vector<const LRItem*> core;
    std::vector<const LRItem*> extension;

    std::vector<const Symbol*> ateSymbols;

    std::vector<const State*> previousStates;
    std::unordered_map<const Symbol *, std::vector<const LRItem*>> nextStates;
    std::vector<const LRItem*> reducedItems;

};

template<>
struct std::less<std::pair<const Symbol*, const State*>>
{
    bool operator()(const std::pair<const Symbol*, const State*> &lhs, const std::pair<const Symbol*, const State*> &rhs)
    {
        int compareValue = lhs.first->getName().compare(rhs.first->getName());
        if (compareValue == -1)
        {
            return true;
        }
        else if (compareValue == 0)
        {
            if (*(lhs.second) < *(rhs.second))
            {
                return true;
            }
        }
        return false;
    }
};


template<>
struct std::less<const std::pair<const Symbol*, const State*> *>
{
    bool operator()(const std::pair<const Symbol*, const State*>  * const lhs, const std::pair<const Symbol*, const State*>  * const rhs)
    {
        int compareValue = lhs->first->getName().compare(rhs->first->getName());
        if (compareValue == -1)
        {
            return true;
        }
        else if (compareValue == 0)
        {
            if (*(lhs->second) < *(rhs->second))
            {
                return true;
            }
        }
        return false;
    }
};