#pragma once

#include "Material.h"
#include "Vector.h"

struct Intersection
{
	enum class IntersectionType
	{
		NONE,
		IN,
		OUT
	} type;

	double m_t;
	Vector m_normal;
	const Material* m_material;

	explicit Intersection(double t = 0, IntersectionType type = IntersectionType::NONE, const Vector& normal = Vector(), const Material* material = nullptr);

	Intersection(const Intersection& other) = default;

	Intersection(Intersection&& other) = default;

	Intersection& operator=(const Intersection& other) = default;

	Intersection& operator=(Intersection&& other) = default;

	~Intersection() = default;
};

bool operator<(const Intersection& lhs, const Intersection& rhs);

bool operator>(const Intersection& lhs, const Intersection& rhs);
