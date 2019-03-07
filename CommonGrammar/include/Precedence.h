#pragma once
#include <string>

class Precedence
{
public:
    enum Association : unsigned char
    {
        LEFT = 0,
        RIGHT = 1,
        NONASSOC = 2
    };

    Precedence(const std::string &pName, unsigned int pPrority, Association pAssociation);
    ~Precedence();

    const std::string &getName() const { return name; }
    unsigned int getPriority() const { return prority; }
    Association getAssociation() const { return association; }

    bool operator==(const Precedence &rhs) const { return name.compare(rhs.getName()) == 0; }

private:
    Association association;
    unsigned int prority;
    std::string name;
};

template <>
struct std::hash<Precedence>
{
    std::size_t operator()(const Precedence& k) const
    {
        return std::hash<std::string>()(k.getName());
    }
};

template <>
struct std::equal_to<Precedence>
{
    bool operator()(const Precedence& lhs, const Precedence& rhs) const
    {
        return lhs.getName().compare(rhs.getName()) == 0;
    }
};

template <>
struct std::hash<const Precedence *>
{
    std::size_t operator()(const Precedence * const k) const
    {
        return std::hash<std::string>()(k->getName());
    }
};

template <>
struct std::equal_to<const Precedence * >
{
    bool operator()(const Precedence * const lhs, const Precedence * const rhs) const
    {
        return lhs->getName().compare(rhs->getName()) == 0;
    }
};
