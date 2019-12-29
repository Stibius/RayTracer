#include "Quad.h"
#include "Quaternion.h"
#include "Utility.h"

#include <sstream>

std::string Quad::DESCRIPTION_LABEL = "Quad";

Quad::Quad(const Plane& plane, const Vector& dimensions, ComponentShape* parent, const Material* material, const std::string& name)
    : LeafShape(parent, material, name)
	, m_plane(plane)
	, m_dimensions(dimensions)
{

}

Quad::Quad(const std::string& description, const std::vector<Material*>& materials)
{
	fromDescription(description, materials);
}

Intersection Quad::getIntersection(const Ray& ray) const
{
    Intersection intersection = m_plane.getIntersection(ray);

    if (intersection.type != Intersection::IntersectionType::NONE)
    {
        Vector point = ray.getPoint(intersection.m_t);
        const Vector& origin = m_plane.getOrigin();
        Vector difference = point - origin;

        if (m_dimensions.m_x < 0)
        {
            if (difference.m_x > 0 || difference.m_x < m_dimensions.m_x)
            {
                return Intersection();
            }
        }
        else if (m_dimensions.m_x > 0)
        {
            if (difference.m_x < 0 || difference.m_x > m_dimensions.m_x)
            {
                return Intersection();
            }
        }

        if (m_dimensions.m_y < 0)
        {
            if (difference.m_y > 0 || difference.m_y < m_dimensions.m_y)
            {
                return Intersection();
            }
        }
        else if (m_dimensions.m_y > 0)
        {
            if (difference.m_y < 0 || difference.m_y > m_dimensions.m_y)
            {
                return Intersection();
            }
        }

        if (m_dimensions.m_z < 0)
        {
            if (difference.m_z > 0 || difference.m_z < m_dimensions.m_z)
            {
                return Intersection();
            }
        }
        else if (m_dimensions.m_z > 0)
        {
            if (difference.m_z < 0 || difference.m_z > m_dimensions.m_z)
            {
                return Intersection();
            }
        }

        intersection.m_material = m_material;
        return intersection;
    }

    return intersection;
}

std::vector<Intersection> Quad::getIntersections(const Ray& ray) const
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

bool Quad::clipsPoint(const Vector& point) const
{
	Vector planeDir = m_plane.getOrigin() - point;
	if (dot(normalize(planeDir), m_plane.getNormal()) >= 0)
	{
		Vector projectedPoint = m_plane.projectPoint(point);
		const Vector& origin = m_plane.getOrigin();
		Vector difference = projectedPoint - origin;

		if (m_dimensions.m_x < 0)
		{
			if (difference.m_x > 0 || difference.m_x < m_dimensions.m_x)
			{
				return false;
			}
		}
		else if (m_dimensions.m_x > 0)
		{
			if (difference.m_x < 0 || difference.m_x > m_dimensions.m_x)
			{
				return false;
			}
		}

		if (m_dimensions.m_y < 0)
		{
			if (difference.m_y > 0 || difference.m_y < m_dimensions.m_y)
			{
				return false;
			}
		}
		else if (m_dimensions.m_y > 0)
		{
			if (difference.m_y < 0 || difference.m_y > m_dimensions.m_y)
			{
				return false;
			}
		}

		if (m_dimensions.m_z < 0)
		{
			if (difference.m_z > 0 || difference.m_z < m_dimensions.m_z)
			{
				return false;
			}
		}
		else if (m_dimensions.m_z > 0)
		{
			if (difference.m_z < 0 || difference.m_z > m_dimensions.m_z)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

const Plane& Quad::getPlane() const
{
    return m_plane;
}

const Vector& Quad::getDimensions() const
{
    return m_dimensions;
}

std::unique_ptr<ComponentShape> Quad::clone() const
{
    return std::make_unique<Quad>(*this);
}

std::string Quad::toDescription() const
{
	std::stringstream ss;

	ss << DESCRIPTION_LABEL << ","

		<< 12 << ","

		<< m_name << ","

		<< m_dimensions.m_x << ","
		<< m_dimensions.m_y << ","
		<< m_dimensions.m_z << ","

		<< m_plane.getOrigin().m_x << ","
		<< m_plane.getOrigin().m_y << ","
		<< m_plane.getOrigin().m_z << ","
		<< m_plane.getNormal().m_x << ","
		<< m_plane.getNormal().m_y << ","
		<< m_plane.getNormal().m_z << ","

	    << m_enabled << ",";

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

void Quad::fromDescription(const std::string& description, const std::vector<Material*>& materials)
{
	std::vector<std::string> list = Utility::split(description, ',');

	int i = 2;

	m_name = list[i++];

	m_dimensions.m_x = std::stod(list[i++]);
	m_dimensions.m_y = std::stod(list[i++]);
	m_dimensions.m_z = std::stod(list[i++]);

	m_plane.setOrigin(Vector(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2])));
	i += 3;
	m_plane.setNormal(Vector(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2])));
	i += 3;
	
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

void Quad::setPlane(const Plane& plane)
{
	m_plane = plane;
}

void Quad::setDimensions(const Vector& dimensions)
{
	m_dimensions = dimensions;
}

void Quad::translate(const Vector& translation)
{
    m_plane.translate(translation);
}

void Quad::rotate(double degrees, const Vector& axis)
{
    m_plane.rotate(degrees, axis);
    Matrix rotation = Quaternion::getMatrix(degrees, axis);
    m_dimensions = rotation * m_dimensions;
}

void Quad::scale(const Vector& factors)
{
    const Vector& origin = m_plane.getOrigin();
    m_dimensions = Vector(
		(origin.m_x + m_dimensions.m_x) * factors.m_x,
        (origin.m_y + m_dimensions.m_y) * factors.m_y,
        (origin.m_z + m_dimensions.m_z) * factors.m_z);

    m_plane.scale(factors);
}
