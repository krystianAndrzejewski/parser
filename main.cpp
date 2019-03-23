#include "StreamReader.h"
#include "Grammar.h"
#include "GrammarBuilder.h"
#include "LRTable.h"
#include "Parser.h"
#include <fstream>

int main(char *argv, int argc)
{
	std::fstream streamGrammar("..\\example\\grammar.txt");
	if (streamGrammar.is_open() == false)
	{
		return 1;
	}
	Parser parser;
	parser.generateParser(streamGrammar);
	streamGrammar.close();
	std::fstream streamInput("..\\example\\input.txt");
	if (streamInput.is_open() == false)
	{
		return 1;
	}
	ElementTree *result = nullptr;
	bool success = parser.parse(streamInput, result);
	if (success && result)
	{
		result->print();
	}
    return 0;
}