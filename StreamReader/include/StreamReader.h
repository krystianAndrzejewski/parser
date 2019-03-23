#pragma once
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <istream>

#include "Precedence.h"

using namespace std;

typedef std::vector<std::tuple<std::string, std::vector<std::pair<std::string, bool>>, std::string>> ReceiptList;

class StreamReader
{
public:
	StreamReader();
    virtual ~StreamReader();
	bool processStream(std::istream &reader);

    const std::vector<std::tuple<std::string, unsigned char, unsigned int>> &getPrecedences() const
    {
        return precedences;
    }
    const std::vector<std::tuple<std::string, std::string, std::string>> &getTokens() const
    {
        return tokens;
    }
	const std::vector<std::string> &getIgnores() const
	{
		return ignores;
	}
    const std::vector<std::tuple<std::string, std::string>> &getNonTerminals() const
    {
        return nonTerminals;
    }
    const ReceiptList &getProductions() const
    {
        return productions;
    }

    const std::string& getStart() const
    {
        return startProduction;
    }

    void printDebugInfo();

private:
    enum keyword : unsigned char
    {
        precedence_left = Precedence::Association::LEFT,
        precedence_right = Precedence::Association::RIGHT,
        precedence_nonassoc = Precedence::Association::NONASSOC,
        precedence_definition,
        token_definition,
        nonterminal_definition,
        start_definition,
		ignore_definition,
        empty_definition
    };

    bool readStream(std::istream &reader);

    void takeProductions();
    void processProductionIgredient(
        const std::string &product, 
        const std::string &productionBody,
        const std::regex &igredientRegex);
    bool processPrecedenceForProduction(
        std::sregex_iterator &iterator);

    void takePrecedences();
    void processPrecedenceIgredient(
        const std::sregex_iterator &iterator,
        const std::regex &nameRegex,
        unsigned int level);

    void takeTerminals();
	void takeIgnores();
    void processTokens(
        const std::sregex_iterator &iterator,
        const std::regex &tokenRegex);

    void takeNonTerminals();
    void processNonterminals(
        const std::sregex_iterator &iterator,
        const std::regex &nonTerminalRegex);

    void takeStartProduction();

    void isContentEmpty();

    void removeMatches(const std::regex &regExp);
    void removeMatches(const std::string &regExpPattern);
	void clearInternalProps();

    std::string content;

    std::string startProduction;
    ReceiptList productions;
    std::vector<std::tuple<std::string, unsigned char, unsigned int>> precedences;
    std::vector<std::tuple<std::string, std::string, std::string>> tokens;
	std::vector<std::string> ignores;
    std::vector<std::tuple<std::string, std::string>> nonTerminals;
    static const std::map<keyword, std::string> keywordText;

    const std::string namePattern;
    const std::string commentPattern;
    const std::string nameOrSpecialPattern;
    const std::string whiteSigns;
    const std::string interspace;
    const std::string textInQuationMark;
    const std::string textInDQuationMark;
    const std::string alternativeProductionBegin;
	bool isGenerated;
};

