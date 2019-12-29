#include "Intersection.h"

Intersection::Intersection(double t, Intersection::IntersectionType type, const Vector& normal, const Material* material)
	: type(type)
	, m_t(t)
	, m_normal(normal)
	, m_material(material)
{

}

bool operator<(const Intersection& lhs, const Intersection& rhs)
{
	if (lhs.m_t < rhs.m_t)
	{
		return true;
	}
	else if (lhs.m_t == rhs.m_t)
	{
		return lhs.type == Intersection::IntersectionType::IN;
	}
	else
	{
		return false;
	}
}

bool operator>(const Intersection& lhs, const Intersection& rhs)
{
	if (lhs.m_t > rhs.m_t)
	{
		return true;
	}
	else if (lhs.m_t == rhs.m_t)
	{
		return lhs.type == Intersection::IntersectionType::OUT;
	}
	else
	{
		return false;
	}
}