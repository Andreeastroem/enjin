#pragma once
#include <string>
#include <array>
#include <span>

using std::string;

enum class AttributeNames : std::size_t
{
    STRENGTH,
    DEXTERITY,
    INTELLIGENCE,
    VITALITY,
    MANA,
    COUNT
};

struct Attribute
{
    string name;
    uint32_t level;
    u_int64_t exp;
};

class Attributes
{
public:
    Attributes(std::span<const Attribute> attributes);
    ~Attributes();

    Attribute getAttribute(string name);

    Attribute operator[](const uint16_t index);

    void addExp(string name, u_int64_t amount);

private:
    std::array<Attribute, static_cast<std::size_t>(AttributeNames::COUNT)> m_attributes;
};