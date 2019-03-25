#include "StreamReader.h"
#include "Grammar.h"
#include "GrammarBuilder.h"
#include "LRTable.h"
#include "Parser.h"
#include <fstream>

int main(int argc, const char *argv[])
{
	if (argc != 3)
	{
		return 1;
	}
	std::fstream streamGrammar(argv[1]);
	if (streamGrammar.is_open() == false)
	{
		return 1;
	}
	Parser parser;
	parser.generateParser(streamGrammar);
	streamGrammar.close();
	std::fstream streamInput(argv[2]);
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