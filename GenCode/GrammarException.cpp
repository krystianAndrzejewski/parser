#include "GrammarException.h"


GrammarException::GrammarException(std::string const& message, ErrorCause value)
    : std::runtime_error(message), cause(value)
{
}
