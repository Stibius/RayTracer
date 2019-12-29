#include "NamedEntity.h"

NamedEntity::NamedEntity(const std::string& name)
    : m_name(name)
{

}

void NamedEntity::setName(const std::string& name)
{
    m_name = name;
}

const std::string& NamedEntity::getName() const
{
    return m_name;
}
