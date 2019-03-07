#include "FileReader.h"
#include "Grammar.h"
#include "GrammarBuilder.h"
#include "LRTable.h"

int main(char *argv, int argc)
{
    FileReader reader("input.txt");
    reader.printDebugInfo();
    auto grammar = GrammarBuilder::createGrammar(reader);
    grammar->printDebugInfo();
    LRTable table(grammar);
    table.printDebugInfo();
	std::vector<std::string> tokens = { "'1'", "'+'", "'1'", "'*'", "'0'" };
	//ElementTree *result = nullptr;
	//bool success = table.parse(tokens, result);
	//if (success && result)
	//{
	//	result->print();
	//}
    return 0;
}