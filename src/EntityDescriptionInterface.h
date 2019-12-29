#pragma once

#include <string>
#include <vector>

class Material;

class EntityDescriptionInterface
{

public:

	EntityDescriptionInterface() = default;

	EntityDescriptionInterface(const EntityDescriptionInterface& other) = default;

	EntityDescriptionInterface(EntityDescriptionInterface&& other) = default;

	EntityDescriptionInterface& operator=(const EntityDescriptionInterface& other) = default;

	EntityDescriptionInterface& operator=(EntityDescriptionInterface&& other) = default;

	virtual ~EntityDescriptionInterface() = default;

	virtual std::string toDescription() const = 0;
};

class EntityDescriptionInterface1 : public EntityDescriptionInterface
{

public:

	EntityDescriptionInterface1() = default;

	EntityDescriptionInterface1(const EntityDescriptionInterface1& other) = default;

	EntityDescriptionInterface1(EntityDescriptionInterface1&& other) = default;

	EntityDescriptionInterface1& operator=(const EntityDescriptionInterface1& other) = default;

	EntityDescriptionInterface1& operator=(EntityDescriptionInterface1&& other) = default;

	virtual ~EntityDescriptionInterface1() = default;

	virtual void fromDescription(const std::string& description) = 0;
};

class EntityDescriptionInterface2 : public EntityDescriptionInterface
{

public:

	EntityDescriptionInterface2() = default;

	EntityDescriptionInterface2(const EntityDescriptionInterface2& other) = default;

	EntityDescriptionInterface2(EntityDescriptionInterface2&& other) = default;

	EntityDescriptionInterface2& operator=(const EntityDescriptionInterface2& other) = default;

	EntityDescriptionInterface2& operator=(EntityDescriptionInterface2&& other) = default;

	virtual ~EntityDescriptionInterface2() = default;

	virtual void fromDescription(const std::string& description, const std::vector<Material*>& materials) = 0;
};
