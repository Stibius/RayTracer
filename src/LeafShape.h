#pragma once

#include "ComponentShape.h"

class LeafShape : public ComponentShape
{
public:

    LeafShape() = default;

    LeafShape(ComponentShape* parent, const Material* material, const std::string& name = "");

	LeafShape(const LeafShape& other) = default;

	LeafShape(LeafShape&& other) = default;

	LeafShape& operator=(const LeafShape& other) = default;

	LeafShape& operator=(LeafShape&& other) = default;

	~LeafShape() override = default;
};
