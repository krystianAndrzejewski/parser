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
    GrammarBuilder::deleteGrammar(grammar);
    return 0;
}