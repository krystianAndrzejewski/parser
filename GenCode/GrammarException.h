#pragma once
#include <stdexcept>
#include <string>

class GrammarException :
    public std::runtime_error
{
public:
    enum ErrorCause : unsigned char
    {
        production_syntax_error = 0,
        precedence_syntax_error = 1,
        repeated_precedence = 2,
        repeated_start_production = 3,
        not_defined_production = 4,
        unneded_characters_in_file = 5,
        undefined_symbol = 6,
        undefined_symbol_kind = 7,
        repeated_symbol_declaration = 8,
        undefined_precedence = 9,
        empty_production = 10,
        repeated_production_declaration = 11,
        undefined_nonterminal_start = 12,
        infinitive_production = 13,
        incorrect_empty_production = 14
    };
    GrammarException(std::string const& message, ErrorCause value);
private:
    ErrorCause cause;
};

