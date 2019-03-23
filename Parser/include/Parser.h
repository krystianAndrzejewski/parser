#pragma once
#include "Lexer.h"
#include "LRTable.h"
#include "ElementTree.h"
#include <memory>

class Parser
{
public:
	Parser();
	~Parser();

	bool generateParser(std::istream &stream);
	void printDebugInfo();
	bool parse(std::istream &stream, ElementTree *&root);

private:
	bool isGeneratedParser;

	Grammar* grammar;
	std::unique_ptr<StreamReader> reader;
	std::unique_ptr<LRTable> lrTable;
	std::unique_ptr<Lexer> lexer;
	std::vector<int> tokenIndices;
	int endIndex;
};