#include "FileReader.h"
#include "Grammar.h"

int main(char *argv, int argc)
{
    FileReader reader("input.txt");
    GrammarBuilder::createGrammar(reader);
    return 0;
}