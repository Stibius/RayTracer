#include "Plane.h"
#include "Quaternion.h"
#include "Matrix.h"
#include "Utility.h"

#include <sstream>

std::string Plane::DESCRIPTION_LABEL = "Plane";

Plane::Plane(const Vector& normal, const Vector& origin, ComponentShape* parent, const Material* material, const std::string& name)
    : LeafShape(parent, material, name)
	, m_origin(origin)
{
    m_normal = normalize(normal);
    m_d = -dot(m_normal, origin);
}

Plane::Plane(const std::string& description, const std::vector<Material*>& materials)
{
	fromDescription(description, materials);
}

Intersection Plane::getIntersection(const Ray& ray) const
{
	double np = dot(m_normal, ray.getDirection());

    if (np == 0)
    {
        return Intersection();
    }

	double t = -(m_d + dot(m_normal, ray.getOrigin())) / np;

	if (t > 0)
	{
		if (dot(normalize(ray.getDirection()), m_normal) > 0)
		{
			return Intersection(t, Intersection::IntersectionType::OUT, -m_normal, m_material);
		}
		else
		{
			return Intersection(t, Intersection::IntersectionType::IN, m_normal, m_material);
		}
	}
	else
	{
		return Intersection();
	}
}

std::vector<Intersection> Plane::getIntersections(const Ray& ray) const
{
    std::vector<Intersection> intersections;

    Intersection intersection = getIntersection(ray);

    if (intersection.type == Intersection::IntersectionType::NONE)
    {
        return intersections;
    }
    else if (intersection.type == Intersection::IntersectionType::IN)
    {
		intersections = 
		{
			intersection,
		    Intersection(intersection.m_t, Intersection::IntersectionType::OUT, intersection.m_normal, m_material)
		};
    }
    else if (intersection.type == Intersection::IntersectionType::OUT)
    {
		intersections =
		{
			Intersection(intersection.m_t, Intersection::IntersectionType::IN, intersection.m_normal, m_material),
			intersection
		};
    }

    return intersections;
}

bool Plane::clipsPoint(const Vector& point) const
{
	Vector planeDir = m_origin - point;
	return dot(normalize(planeDir), m_normal) >= 0;
}

const Vector& Plane::getNormal() const
{
    return m_normal;
}

const Vector& Plane::getOrigin() const
{
    return m_origin;
}

std::string Plane::toDescription() const
{
	std::stringstream ss;

	ss << DESCRIPTION_LABEL << ","

		<< 9 << ","

		<< m_name << ","

		<< m_origin.m_x << ","
		<< m_origin.m_y << ","
		<< m_origin.m_z << ","
		<< m_normal.m_x << ","
		<< m_normal.m_y << ","
		<< m_normal.m_z << ",";

	ss << m_enabled << ",";

	if (m_material == nullptr)
	{
		ss << NO_MATERIAL_LABEL;
	}
	else
	{
		ss << m_material->getName();
	}

	return ss.str();
}

void Plane::fromDescription(const std::string& description, const std::vector<Material*>& materials)
{
	std::vector<std::string> list = Utility::split(description, ',');

	int i = 2;

	m_name = list[i++];

	m_origin.m_x = std::stod(list[i++]);
	m_origin.m_y = std::stod(list[i++]);
	m_origin.m_z = std::stod(list[i++]);
	m_normal.m_x = std::stod(list[i++]);
	m_normal.m_y = std::stod(list[i++]);
	m_normal.m_z = std::stod(list[i++]);
	m_normal = normalize(m_normal);

	m_d = -dot(m_normal, m_origin);

	m_enabled = std::stoi(list[i++]);

	m_material = nullptr;

	if (list[i] != NO_MATERIAL_LABEL)
	{
		for (const Material* material : materials)
		{
			if (material != nullptr && material->getName() == list[i])
			{
				m_material = material;
			}
		}
	}
}

void Plane::setOrigin(const Vector& origin)
{    
    m_origin = origin;
    m_d = -dot(m_normal, origin);
}

void Plane::setNormal(const Vector& normal)
{    
    m_normal = normalize(normal);
    m_d = -dot(m_normal, m_origin);
}

Vector Plane::projectPoint(const Vector& point) const
{
    Vector direction = normalize(point - m_origin);
    double distance = dot(direction, m_normal);
    return point - distance * m_normal;
}

std::unique_ptr<ComponentShape> Plane::clone() const
{
    return std::make_unique<Plane>(*this);
}

void Plane::translate(const Vector& translation)
{
    m_origin += translation;
    m_d = -dot(m_normal, m_origin);
}

void Plane::rotate(double degrees, const Vector& axis)
{
    Matrix rotation = Quaternion::getMatrix(degrees, axis);
    m_origin = rotation * m_origin;
    m_normal = normalize(rotation * m_normal);
    m_d = -dot(m_normal, m_origin);
}

void Plane::scale(const Vector& factors)
{
    m_origin = Vector(m_origin.m_x * factors.m_x, m_origin.m_y * factors.m_y, m_origin.m_z * factors.m_z);
    m_d = -dot(m_normal, m_origin);
}
