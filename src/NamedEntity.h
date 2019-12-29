#pragma once

#include <string>

class NamedEntity
{
protected:

    std::string m_name;

public:

    explicit NamedEntity(const std::string& name = "");

	NamedEntity(const NamedEntity& other) = default;

	NamedEntity(NamedEntity&& other) = default;

	NamedEntity& operator=(const NamedEntity& other) = default;

	NamedEntity& operator=(NamedEntity&& other) = default;

	virtual ~NamedEntity() = default;

    void setName(const std::string& name);

    const std::string& getName() const;
};
