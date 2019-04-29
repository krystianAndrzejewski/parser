#include "StreamReader.h"
#include "GrammarException.h"

#include <sstream>
#include <iostream>

const std::map<StreamReader::keyword, std::string> StreamReader::keywordText = { 
    { precedence_left, "%left" },
    { precedence_right, "%right" },
    { precedence_nonassoc, "%nonassoc" },
    { precedence_definition, "%prec" },
    { token_definition, "%token" },
    { nonterminal_definition, "%type" },
    { start_definition, "%start" },
    { ignore_definition, "%ignore" },
	{ empty_definition, "%empty" }
};


StreamReader::StreamReader() :
    namePattern("([a-zA-Z_][a-zA-Z0-9_]*)"),
    nameOrSpecialPattern("([a-zA-Z_%][a-zA-Z0-9_]*)"),
    whiteSigns("\\s*"),
    interspace("[ \\t]*"),
    textInQuationMark("'(.*?[^\\\\](\\\\)*?)'"),
    textInDQuationMark("\"(.*?[^\\\\](\\\\)*?)\""),
    alternativeProductionBegin("\\|[ \\t]*"),
    commentPattern("^\\s*//.*?$"),
	isGenerated(false)
{
}


StreamReader::~StreamReader()
{

}


bool StreamReader::processStream(std::istream &reader)
{
	clearInternalProps();
    if (readStream(reader))
    {
        removeMatches(commentPattern);
        takeStartProduction();
        takeTerminals();
		takeIgnores();
        takeNonTerminals();
        takePrecedences();
        takeProductions();
        isContentEmpty();
		isGenerated = true;
    }
	return isGenerated;
}

void StreamReader::printDebugInfo()
{
	if (isGenerated == false)
	{
		std::cout << "StreamReader - an initial text analisys was not done.\n\n";
		return;
	}
    std::cout << "StreamReader - an initial text analisys was processed corectly.\n\n"
        << "Tokens:\n";
    for (auto token : tokens)
    {
        std::cout << "    (Name: " << get<0>(token) << "; Pattern: " << get<1>(token);
        if (!get<2>(token).empty())
        {
            std::cout << "; Type: " << get<2>(token) << ")\n";
        }
        else
        {
            std::cout << "; Type: default)\n";
        }
    }
	std::cout << "Ignores:\n";
	for (auto ignore : ignores)
	{
		std::cout << "    (Pattern: " << ignore << ")\n";
	}
    std::cout << "Nonterminals:\n";
    for (auto nonTerminal : nonTerminals)
    {
        std::cout << "    (Name: " << get<0>(nonTerminal);
        if (!get<1>(nonTerminal).empty())
        {
            std::cout << "; Type: " << get<1>(nonTerminal) << ")\n";
        }
        else
        {
            std::cout << "; Type: default)\n";
        }
    }
    std::cout << "\n" << "Precedences:\n";
    for (auto precedence : precedences)
    {
        std::cout << "    (Name: " << get<0>(precedence) << "; Type: " << keywordText.at(static_cast<keyword>(get<1>(precedence)))
            << "; Level: " << get<2>(precedence) << ")\n";
    }
    std::cout << "\n" << "Productions:\n";
    for (auto production : productions)
    {
        std::cout << "    (Production: " << get<0>(production) << " :";
        for (auto igredient : get<1>(production))
        {
            if (igredient.second)
            {
                std::cout << " " << igredient.first;
            }
            else
            {
                std::cout << " '" << igredient.first << "'";
            }
        }
        if (!get<2>(production).empty())
        {
            std::cout << "; Precedence association: " << get<2>(production);
        }
        std::cout << ")\n";
    }
    std::cout << "\n";
}


bool StreamReader::readStream(std::istream &reader)
{
    try
    {
        std::stringstream buffer;
        buffer << reader.rdbuf();
        content = buffer.str(); 
        return true;
    }
	catch (...)
	{
		// Does not handle the exceptions
	}
    return false;
}

void StreamReader::takeProductions()
{
    std::stringstream stream;
    stream << "^" << whiteSigns << namePattern << interspace << ":" << interspace 
        << "(" 
        << "("
        << "("
        << "("
        << nameOrSpecialPattern << "|" << textInQuationMark
        << ")" << interspace
        << ")+"
        << ")"  
        << "(" << "\\n" << interspace 
        << "("
        << alternativeProductionBegin 
        << "("
        << "("
        << nameOrSpecialPattern << "|" << textInQuationMark
        << ")" << interspace
        << ")+"
        << ")?"
        << ")*"
        << ")";
    std::string productionPatern = stream.str();
    stream.str("");
    stream << whiteSigns << alternativeProductionBegin << "|"
        << "("
        << nameOrSpecialPattern << "|" << textInQuationMark
        << ")";
    std::string igredientPatern = stream.str();
    std::regex productionRegex(productionPatern);
    std::regex igredientRegex(igredientPatern);
    std::sregex_iterator productionIterator(content.begin(), content.end(), productionRegex);
    std::sregex_iterator end;
    for (; productionIterator != end; ++productionIterator)
    {
        std::string product = productionIterator->str(1);
        std::string productionBody = productionIterator->str(2);
        processProductionIgredient(
            product,
            productionBody,
            igredientRegex);
        if (std::get<1>(productions.back()).size() == 0)
        {
            throw GrammarException(
                "An empty production definition.",
                GrammarException::ErrorCause::production_syntax_error);
        }
    }
    removeMatches(productionRegex);
}


void StreamReader::processProductionIgredient(
    const std::string &product, 
    const std::string &productionBody,
    const std::regex &igredientRegex)
{
    std::sregex_iterator igredientIterator(
        productionBody.begin(),
        productionBody.end(),
        igredientRegex);
    std::sregex_iterator end;
    productions.push_back(std::tuple<std::string, std::vector<std::pair<std::string, bool>>, std::string>
        (product, std::vector<std::pair<std::string, bool>>(), ""));
    for (; igredientIterator != end; ++igredientIterator)
    {
        if (igredientIterator->str(1).empty())
        {
            productions.push_back(std::tuple<std::string, std::vector<std::pair<std::string, bool>>, std::string>
                (product, std::vector<std::pair<std::string, bool>>(), ""));
        }
        else if (!igredientIterator->str(2).empty() && igredientIterator->str(2) == keywordText.at(empty_definition))
        {
            if (std::get<1>(productions
                .back()).empty())
            {
                std::get<1>(productions
                    .back())
                    .push_back(std::make_pair(igredientIterator->str(2), true));
            }
            else
            {
                throw GrammarException(
                    "A wrong defined empty production.",
                    GrammarException::ErrorCause::production_syntax_error);
            }
        }
        else if (!igredientIterator->str(2).empty() && igredientIterator->str(2) != keywordText.at(precedence_definition))
        {
            std::get<1>(productions
                .back())
                .push_back(std::make_pair(igredientIterator->str(2), true));
        }
        else if (!igredientIterator->str(3).empty() && igredientIterator->str(3) != keywordText.at(precedence_definition))
        {
            std::get<1>(productions
                .back())
                .push_back(std::make_pair(igredientIterator->str(3), false));
        }
        else
        {
            if (processPrecedenceForProduction(igredientIterator))
            {
                break;
            }
        }
    }
}


bool StreamReader::processPrecedenceForProduction(std::sregex_iterator &iterator)
{
    std::sregex_iterator end;
    iterator++;
    if (iterator == end || iterator->str(2).empty())
    {
        throw GrammarException(
            "Wrong defined precedence for a production.",
            GrammarException::ErrorCause::production_syntax_error);
    }

    std::get<2>(productions.back()) = iterator->str(2);

    iterator++;
    if (iterator == end)
    {
        return true;
    }
    if (iterator != end && iterator->str(1).empty() == false)
    {
        throw GrammarException(
            "Precedence definition for a production is not last in a sentance.",
            GrammarException::ErrorCause::production_syntax_error);
    }
    return false;
}


void StreamReader::takePrecedences()
{
    std::stringstream stream;
    stream << "^" << whiteSigns
        << "("
        << "("
        << keywordText.at(precedence_left)
        << ")" << "|"
        << "("
        << keywordText.at(precedence_right)
        << ")" << "|"
        << "("
        << keywordText.at(precedence_nonassoc)
        << ")"
        << ")"
        << "("
        << "("
        << interspace << namePattern
        << ")+"
        << ")" << whiteSigns << "$";
    std::string precedencePattern = stream.str();
    std::regex precedenceRegex(precedencePattern);
    std::regex nameRegex(namePattern);
    std::sregex_iterator precedenceIterator(content.begin(), content.end(), precedenceRegex);
    std::sregex_iterator end;
    for (unsigned int i = 0; precedenceIterator != end; ++precedenceIterator, ++i)
    {
        processPrecedenceIgredient(
            precedenceIterator,
            nameRegex,
            i);
    }
    removeMatches(precedenceRegex);
}

void StreamReader::processPrecedenceIgredient(
    const std::sregex_iterator &iterator,
    const std::regex &nameRegex,
    unsigned int level)
{
    unsigned char precedenceKind;
    std::string names = iterator->str(5);
    std::sregex_iterator end;

    if (!iterator->str(2).empty())
    {
        precedenceKind = Precedence::Association::LEFT;
    }
    else if (!iterator->str(3).empty())
    {
        precedenceKind = Precedence::Association::RIGHT;
    }
    else
    {
        precedenceKind = Precedence::Association::NONASSOC;
    }

    std::sregex_iterator nameIterator(
        names.begin(),
        names.end(),
        nameRegex);
    for (; nameIterator != end; ++nameIterator)
    {
        precedences.push_back(
            std::tuple<std::string, unsigned char, unsigned int>(
            nameIterator->str(),
            precedenceKind,
            level)
            );
    }
}


void StreamReader::takeTerminals()
{
    std::stringstream stream;
    stream << "^" << whiteSigns << keywordText.at(token_definition)
        << "("
        << "<" << namePattern << ">"
        << ")?"
        << "("
        << "("
        << interspace << namePattern << interspace << textInDQuationMark
        << ")+"
        << ")" << whiteSigns << "$";
    std::string terminalPattern = stream.str();
    stream.str("");
    stream << namePattern << interspace << textInDQuationMark;
    std::string tokenPattern = stream.str();
    std::regex terminalRegex(terminalPattern);
    std::regex nameRegex(tokenPattern);
    std::sregex_iterator terminalIterator(content.begin(), content.end(), terminalRegex);
    std::sregex_iterator end;
    for (; terminalIterator != end; ++terminalIterator)
    {
        processTokens(
            terminalIterator,
            nameRegex);
    }
    removeMatches(terminalRegex);
}

void StreamReader::takeIgnores()
{
	std::stringstream stream;
	stream << "^" << whiteSigns << keywordText.at(ignore_definition)
		<< "("
		<< interspace << textInDQuationMark
		<< ")+" << whiteSigns << "$";
	std::string ignorePattern = stream.str();
	std::regex ignoreRegex(ignorePattern);
	std::sregex_iterator ignoreIterator(content.begin(), content.end(), ignoreRegex);
	std::sregex_iterator end;
	for (; ignoreIterator != end; ++ignoreIterator)
	{
		ignores.push_back(ignoreIterator->str(2));
	}
	removeMatches(ignorePattern);
}

void StreamReader::processTokens(
    const std::sregex_iterator &iterator,
    const std::regex &tokenRegex)
{
    std::sregex_iterator end;
    std::string tokensDefinition = iterator->str();

    std::sregex_iterator tokenIterator(
        tokensDefinition.begin(),
        tokensDefinition.end(),
        tokenRegex);
    for (; tokenIterator != end; ++tokenIterator)
    {
        tokens.push_back(
            std::tuple<std::string, std::string, std::string>(
                tokenIterator->str(1),
                tokenIterator->str(2),
                iterator->str(2)
            ));
    }
}


void StreamReader::takeNonTerminals()
{
    std::stringstream stream;
    stream << "^" << whiteSigns << keywordText.at(nonterminal_definition)
        << "("
        << "<" << namePattern << ">"
        << ")?"
        << "("
        << "("
        << interspace << namePattern
        << ")+"
        << ")" << whiteSigns << "$";
    std::string nonTerminalPattern = stream.str();
    std::regex nonTerminalRegex(nonTerminalPattern);
    std::regex nameRegex(namePattern);
    std::sregex_iterator nonTerminalIterator(content.begin(), content.end(), nonTerminalRegex);
    std::sregex_iterator end;
    for (; nonTerminalIterator != end; ++nonTerminalIterator)
    {
        processNonterminals(
            nonTerminalIterator,
            nameRegex);
    }
    removeMatches(nonTerminalRegex);
}


void StreamReader::processNonterminals(
    const std::sregex_iterator &iterator,
    const std::regex &nonTerminalRegex)
{
    std::sregex_iterator end;
    std::string tokensDefinition = iterator->str(3);

    std::sregex_iterator nameIterator(
        tokensDefinition.begin(),
        tokensDefinition.end(),
        nonTerminalRegex);
    for (; nameIterator != end; ++nameIterator)
    {
        nonTerminals.push_back(
            std::tuple<std::string, std::string>(
            nameIterator->str(1),
            iterator->str(2)
            ));
    }
}


void StreamReader::takeStartProduction()
{
    std::stringstream stream;
    stream << "^" << whiteSigns << keywordText.at(start_definition) << interspace << namePattern << whiteSigns << "$";
    std::string startPattern = stream.str();
    std::regex startRegex(startPattern);
    std::sregex_iterator startIterator(content.begin(), content.end(), startRegex);
    std::sregex_iterator end;
    if (startIterator == end)
    {
        throw GrammarException("A initial production is not defined.", GrammarException::ErrorCause::not_defined_production);
    }
    startProduction = startIterator->str(1);
    ++startIterator;
    if (startIterator != end)
    {
        throw GrammarException("A initial production is duplicated.", GrammarException::ErrorCause::repeated_start_production);
    }
    removeMatches(startRegex);
}

void StreamReader::isContentEmpty()
{
    if (!content.empty())
    {
        throw GrammarException("Some characters was not processed by the parser.", GrammarException::ErrorCause::unneded_characters_in_file);
    }
}

void StreamReader::removeMatches(const std::regex &regExp)
{
    std::string tempContent = std::move(content);
    std::regex_replace(std::back_inserter(content), tempContent.begin(), tempContent.end(), regExp, "");
}

void StreamReader::removeMatches(const std::string &regExpPattern)
{
    std::regex regExp(regExpPattern);
    std::string tempContent = std::move(content);
    std::regex_replace(std::back_inserter(content), tempContent.begin(), tempContent.end(), regExp, "");
}

void StreamReader::clearInternalProps()
{
	isGenerated = false;
	content.clear();
	startProduction.clear();
	productions.clear();
	precedences.clear();
	tokens.clear();
	nonTerminals.clear();
}
