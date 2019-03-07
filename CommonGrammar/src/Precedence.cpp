#include "Precedence.h"


Precedence::Precedence(const std::string &pName, unsigned int pPrority, Association pAssociation)
    : name(pName), prority(pPrority), association(pAssociation)
{

}


Precedence::~Precedence()
{
}