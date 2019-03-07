#include "ElementTree.h"
#include <iostream>

ElementTree::ElementTree(const Symbol &pSymbol, std::vector<std::unique_ptr<ElementTree>>&& pChildreen) :
	symbol(pSymbol), childreen(std::move(pChildreen))
{
}

ElementTree::~ElementTree()
{
}

void ElementTree::print(unsigned int level) const
{
	std::string intend(level * 2, ' ');
	std::cout << intend << symbol.getName() << '\n';
	for (const auto &child : childreen)
	{
		child->print(level + 1);
	}
}
