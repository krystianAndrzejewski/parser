#include "Parser.h"
#include "GrammarBuilder.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <iterator>

Parser::Parser() : grammar(nullptr), isGeneratedParser(false), reader(new StreamReader()), endIndex(-1)
{
}


Parser::~Parser()
{
	GrammarBuilder::deleteGrammar(grammar);
}

static std::string convertRawStringToEscaped(const std::string &rawString)
{
	std::string output = rawString;
	const std::map<std::string, std::string> replaceRules = {
		{ R"((^|[^\\])((\\\\)*)\\t)", "$1$2\n" },
		{ R"((^|[^\\])((\\\\)*)\\n)", "$1$2\t" },
		{ R"((^|[^\\])((\\\\)*)\\r)", "$1$2\r" }
	};
	for (auto rule : replaceRules)
	{
		std::regex regRule(rule.first);
		output = std::regex_replace(output, regRule, rule.second);
	}
	output = std::regex_replace(output, std::regex("\\\\"), "\\");
	return output;
}

bool Parser::generateParser(std::istream & stream)
{
	bool isCorrect = reader->processStream(stream);
	if (!isCorrect)
	{
		return false;
	}

	GrammarBuilder::deleteGrammar(grammar);
	grammar = nullptr;
	grammar = GrammarBuilder::createGrammar(*reader);

	auto symbols = grammar->getAllSymbols();
	std::vector<std::string> patterns;
	int index = 0;
	tokenIndices.clear();
	for (auto symbol : symbols)
	{
		if (dynamic_cast<Terminal*>(symbol))
		{
			const std::string &rawPattern = *symbol->getPattern();
			const std::string pattern = convertRawStringToEscaped(rawPattern);
			if (!(pattern.empty() || symbol->getType() == "end"))
			{
				tokenIndices.push_back(index);
				patterns.push_back(pattern);
			}
			else if(symbol->getType() == "end")
			{
				if (endIndex != -1)
				{
					throw runtime_error("An end symbol is almost twice (the symbol must be present only once)!");
				}
				endIndex = index;
			}
		}
		else if (dynamic_cast<NonTerminal*>(symbol) == false)
		{
			const std::string &rawPattern = *symbol->getPattern();
			const std::string pattern = convertRawStringToEscaped(rawPattern);
			if (!pattern.empty())
			{
				tokenIndices.push_back(index);
				std::stringstream ss;
				for (auto character : pattern)
				{
					ss << '[' << character << ']';
				}
				patterns.push_back(ss.str());
			}
		}
		index++;
	}

	if (endIndex < 0)
	{
		throw runtime_error("An end symbol is not provided (the symbol must be present only once)!");
	}

	for (auto ignore : reader->getIgnores())
	{
		const std::string pattern = convertRawStringToEscaped(ignore);
		patterns.push_back(pattern);
		tokenIndices.push_back(-1);
	}

	lexer.reset(new Lexer(patterns));

	lrTable.reset(new LRTable(*grammar));

	isGeneratedParser = true;

	return isGeneratedParser;
}

void Parser::printDebugInfo()
{
	if (isGeneratedParser)
	{
		std::cout << "\n################STREAM READER################\n";
		reader->printDebugInfo();
		std::cout << "\n################GRAMMAR################\n";
		grammar->printDebugInfo();
		std::cout << "\n################LEXER################\n";
		std::string outputInfo;
		lexer->print(outputInfo);
		std::cout << outputInfo;
		std::cout << "\n################PARSER TABLE################\n";
		lrTable->printDebugInfo();
	}
}

bool Parser::parse(std::istream & stream, ElementTree *& root)
{
	std::vector<unsigned int> stateStack;
	std::vector<std::unique_ptr<ElementTree>> elementTreeStack;
	stateStack.push_back(0);
	bool isEnd = false;
	LexerMatch match = lexer->match(stream);
	while (isEnd == false)
	{
		std::size_t actualState = stateStack.back();
		std::size_t symbolId = (std::size_t)endIndex;
		if (match.isEmpty() == false)
		{
			int tokenId = match.getIndex();
			if (tokenId >= tokenIndices.size())
			{
				throw runtime_error("Symbol index from tokenizer is out of range.");
			}
			if (tokenIndices[tokenId] < 0)
			{
				match = lexer->match(stream);
				continue;
			}
			symbolId = (std::size_t)tokenIndices[tokenId];
		}
		else if((char)stream.get() > '\0')
		{
			return false;
		}
		std::pair<std::size_t, std::size_t> actionDef = std::make_pair(symbolId, actualState);
		std::pair<LRTable::Action, std::size_t> moveProps;
		bool nextStateExists = lrTable->getAction(actionDef, moveProps);
		if (nextStateExists)
		{
			if (moveProps.first == LRTable::reduce)
			{
				std::vector<std::unique_ptr<ElementTree>> childreen;
				std::size_t reducedProductionIndex = moveProps.second;
				auto production = grammar->getProduction(reducedProductionIndex);
				unsigned int productionIngredientsSize = (unsigned int)production->getIgredients().size();
				for (unsigned int j = 0; j < productionIngredientsSize; j++)
				{
					stateStack.pop_back();
					childreen.emplace_back(elementTreeStack.back().release());
					elementTreeStack.pop_back();
				}
				auto &product = production->getProduct();
				elementTreeStack.emplace_back(new ElementTree(product, std::move(childreen)));
				std::size_t productionId = 0;
				bool isValidPtr = grammar->getProduction(production, productionId);
				if (isValidPtr == false)
				{
					throw std::runtime_error("Product index is not found.");
				}
				std::pair<std::size_t, std::size_t> gotoActionDef = std::make_pair(productionId, stateStack.back());
				std::size_t nextState = 0;
				bool isGotoState = lrTable->getGoto(gotoActionDef, nextState);

				if (isGotoState)
				{
					stateStack.push_back((unsigned int)nextState);
				}
				else
				{
					return false;
				}
			}
			else if (moveProps.first == LRTable::shift)
			{
				std::vector<std::unique_ptr<ElementTree>> childreen;
				stateStack.push_back((unsigned int)moveProps.second);
				elementTreeStack.emplace_back(new ElementTree(*grammar->getSymbol(actionDef.first), std::move(childreen)));
				match = lexer->match(stream);
			}
			else
			{
				isEnd = true;
			}
		}
		else
		{
			return false;
		}
	}
	root = elementTreeStack.back().release();
	return true;
}
