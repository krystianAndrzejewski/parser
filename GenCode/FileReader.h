#pragma once
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <fstream>

#include "Precedence.h"

using namespace std;

typedef std::vector<std::tuple<std::string, std::vector<std::pair<std::string, bool>>, std::string>> receiptList;

class FileReader
{
public:
    FileReader(const std::string &pPath);
    virtual ~FileReader();

    const std::vector<std::tuple<std::string, unsigned char, unsigned int>> &getPrecedences() const
    {
        return precedences;
    }
    const std::vector<std::tuple<std::string, std::string, std::string>> &getTokens() const
    {
        return tokens;
    }
    const std::vector<std::tuple<std::string, std::string>> &getNonTerminals() const
    {
        return nonTerminals;
    }
    const receiptList &getProductions() const
    {
        return productions;
    }

    const std::string& getStart() const
    {
        return startProduction;
    }

private:
    enum keyword : unsigned char
    {
        precedence_left = Precedence::Association::LEFT,
        precedence_right = Precedence::Association::RIGHT,
        precedence_nonassoc = Precedence::Association::NONASSOC,
        precedence_definition = 
            Precedence::Association::LEFT | 
            Precedence::Association::RIGHT | 
            Precedence::Association::NONASSOC
    };

    void processFile();
    bool readFile();

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

    std::string path;
    std::string content;

    std::string startProduction;
    receiptList productions;
    std::vector<std::tuple<std::string, unsigned char, unsigned int>> precedences;
    std::vector<std::tuple<std::string, std::string, std::string>> tokens;
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

};

