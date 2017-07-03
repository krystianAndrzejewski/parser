#include "Production.h"


Production::Production(const NonTerminal &pProduct, const std::vector<const Symbol*> &pIgridients, const Precedence *pPrecedence) :
    product(pProduct),
    precedence(pPrecedence)
{
    igredients.reserve(pIgridients.size());
    std::copy(pIgridients.begin(), pIgridients.end(), std::back_inserter(igredients));
    createHashableName();
}


Production::~Production()
{

}

void Production::createHashableName()
{
    hashableName = product.getName();
    hashableName += " ->";
    for (auto element : igredients)
    {
        hashableName += " " + element->getName();
    }
}
