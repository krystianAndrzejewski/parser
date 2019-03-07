#pragma once
#include <vector>
#include <memory>
#include "Symbol.h"

class ElementTree
{
public:
	ElementTree(const Symbol &pSymbol, std::vector<std::unique_ptr<ElementTree>> &&pChildreen);
	~ElementTree();

	void print(unsigned int level = 0) const;
private:
	std::vector<std::unique_ptr<ElementTree>> childreen;
	const Symbol &symbol;
};

