#pragma once
#include "Production.h"

#include <unordered_set>
#include <map>

class ParserState;

class LRItem
{
public:
    LRItem(const Production *pProduction, const std::size_t pPosition, const std::size_t pNumber);
    ~LRItem();

    void addLookahead(const ParserState *pState, const Symbol *pLookahead);

    const std::string &getHashableName() const { return hashableName; }
    const LRItem *getNext() const { return next; }
    const LRItem *getPrevious() const { return previous; }
    std::size_t getPosition() const { return position; }
    const Symbol *getSymbol() const;
    const std::vector<const LRItem *>getClosure() const { return childreen; };
    const std::unordered_set<const Symbol *> &getLookaheads() const { return lookaheads; };

    void setNext(const LRItem *pNext) { next = pNext; }
    void setPrevious(const LRItem *pPrevious) { previous = pPrevious; }
    void addChild(const LRItem *child);
    void addChild(const std::vector<const LRItem*> &childreen);

    bool operator==(const LRItem &rhs) const { return hashableName.compare(rhs.getHashableName()) == 0; }

    std::size_t getNumber() const { return number; }

    const Production * getProduction() const { return production; }

private:
    const Production *production;
    const std::size_t position;
    std::unordered_set<const Symbol*> lookaheads;
    std::map<const ParserState *, std::unordered_set<const Symbol*>> lookaheadsSet;
    std::string hashableName;
    const LRItem *next;
    const LRItem *previous;
    std::vector<const LRItem *> childreen;
    static std::size_t amoutOfLRItems;
    const std::size_t number;
};

template <>
struct std::hash<LRItem>
{
    std::size_t operator()(const LRItem& k) const
    {
        return std::hash<std::string>()(k.getHashableName());
    }
};

template <>
struct std::equal_to<LRItem>
{
    bool operator()(const LRItem& lhs, const LRItem& rhs) const
    {
        return lhs.getHashableName().compare(rhs.getHashableName()) == 0;
    }
};

template <>
struct std::less<LRItem>
{
	bool operator()(const LRItem &lhs, const LRItem &rhs) const
	{
		return lhs.getNumber() < rhs.getNumber();
	}
};

template <>
struct std::hash<LRItem *>
{
    std::size_t operator()(const LRItem * const k) const
    {
        return std::hash<std::string>()(k->getHashableName());
    }
};

template <>
struct std::equal_to<LRItem *>
{
    bool operator()(const LRItem * const lhs, const LRItem * const rhs) const
    {
        return lhs->getHashableName().compare(rhs->getHashableName()) == 0;
    }
};


template<>
struct std::less<LRItem*>
{
	bool operator() (const LRItem * const lhs, const LRItem * const rhs) const
	{
		return lhs->getNumber() < rhs->getNumber();
	}
};

template<>
struct std::less<const std::vector<const LRItem*> *>
{
    bool operator() (const std::vector<const LRItem*> * const lhs, const std::vector<const LRItem*> * const rhs) const
    {
        if (lhs->size() < rhs->size())
        {
            return true;
        }
        else if (lhs->size() == rhs->size())
        {
            for (std::size_t i = 0; i < lhs->size(); i++)
            {
                if ((*lhs)[i]->getNumber() < (*rhs)[i]->getNumber())
                {
                    return true;
                }

				if ((*lhs)[i]->getNumber() > (*rhs)[i]->getNumber())
				{
					return false;
				}
            }
            return false;
        }
        else
        {
            return false;
        }
    }
};


