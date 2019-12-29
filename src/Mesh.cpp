
#include <algorithm>
#include <limits>
#include <sstream>

#include "Mesh.h"
#include "Utility.h"

std::string Mesh::DESCRIPTION_LABEL = "Mesh";

void Mesh::setBoundingBox()
{
	double minValue = std::numeric_limits<double>::max();
	double maxValue = std::numeric_limits<double>::min();

	Vector min = Vector(minValue, minValue, minValue);
	Vector max = Vector(maxValue, maxValue, maxValue);

    for (const Triangle& triangle : m_triangles)
    {
		std::vector<Vector> vertices = { triangle.getV1(), triangle.getV2(), triangle.getV3() };

        for (const Vector& vertex : vertices)
        {
			min.m_x = std::min(min.m_x, vertex.m_x);
			max.m_x = std::max(max.m_x, vertex.m_x);
			min.m_y = std::min(min.m_y, vertex.m_y);
			max.m_y = std::max(max.m_y, vertex.m_y);
			min.m_z = std::min(min.m_z, vertex.m_z);
			max.m_z = std::max(max.m_z, vertex.m_z);
        }
    }

    m_boundingBox = 
	{
		//front
		Quad(Plane(Vector(0, 0, -1), Vector(min.m_x, min.m_y, min.m_z), nullptr, nullptr), Vector(max.m_x - min.m_x, max.m_y - min.m_y, 0), nullptr, nullptr),
		//back
	    Quad(Plane(Vector(0, 0, 1), Vector(min.m_x, min.m_y, max.m_z), nullptr, nullptr), Vector(max.m_x - min.m_x, max.m_y - min.m_y, 0), nullptr, nullptr),
	    //left
	    Quad(Plane(Vector(-1, 0, 0), Vector(min.m_x, min.m_y, min.m_z), nullptr, nullptr), Vector(0, max.m_y - min.m_y, max.m_z - min.m_z), nullptr, nullptr),
	    //right
	    Quad(Plane(Vector(1, 0, 0), Vector(max.m_x, min.m_y, min.m_z), nullptr, nullptr), Vector(0, max.m_y - min.m_y,  max.m_z - min.m_z), nullptr, nullptr),
	    //top
	    Quad(Plane(Vector(0, 1, 0), Vector(min.m_x, max.m_y, min.m_z), nullptr, nullptr), Vector(max.m_x - min.m_x, 0,  max.m_z - min.m_z), nullptr, nullptr),
	    //bottom
	    Quad(Plane(Vector(0, -1, 0), Vector(min.m_x, min.m_y, min.m_z), nullptr, nullptr), Vector(max.m_x - min.m_x, 0,  max.m_z - min.m_z), nullptr, nullptr)
	};
}

Mesh::Mesh(const std::vector<Triangle>& triangles, ComponentShape* parent, const Material* material, bool smooth, const std::string& name)
    : LeafShape(parent, material, name)
	, m_triangles(triangles)
	, m_smooth(smooth)
{
    setBoundingBox();
}

Mesh::Mesh(const std::string& description, const std::vector<Material*>& materials)
{
	fromDescription(description, materials);
}

Intersection Mesh::getIntersection(const Ray& ray) const
{
    Intersection result;
    Intersection bboxInt;

    for (const Quad& quad : m_boundingBox)
    {
        bboxInt = quad.getIntersection(ray);

        if (bboxInt.type != Intersection::IntersectionType::NONE)
        {
            break;
        }
    }

    if (bboxInt.type == Intersection::IntersectionType::NONE)
    {
        return result;
    }

    for (const Triangle& triangle : m_triangles)
    {
        Intersection intersection;

        if (m_smooth)
        {
            intersection = triangle.getIntersectionSmooth(ray);
        }
        else
        {
            intersection = triangle.getIntersection(ray);
        }

        if (result.type == Intersection::IntersectionType::NONE || (intersection.type != Intersection::IntersectionType::NONE && intersection.m_t < result.m_t))
        {
            result = intersection;
        }
    }

    if (result.type != Intersection::IntersectionType::NONE && m_material != nullptr)
    {
        result.m_material = m_material;
    }

    return result;
}

std::vector<Intersection> Mesh::getIntersections(const Ray& ray) const
{
    std::vector<Intersection> intersections;

    for (const Triangle& triangle : m_triangles)
    {
        Intersection intersection;

        if (m_smooth)
        {
            intersection = triangle.getIntersectionSmooth(ray);
        }
        else
        {
            intersection = triangle.getIntersection(ray);
        }

        if (intersection.type != Intersection::IntersectionType::NONE)
        {
			if (m_material != nullptr)
			{
				intersection.m_material = m_material;
			}
            intersections.push_back(intersection);
        }
    }

    std::sort(intersections.begin(), intersections.end());

    return intersections;
}

bool Mesh::clipsPoint(const Vector& point) const
{
	for (const Triangle& triangle : m_triangles)
	{
		if (triangle.clipsPoint(point))
		{
			return true;
		}
	}

	return false;
}

const std::vector<Triangle>& Mesh::getTriangles() const
{
    return m_triangles;
}

bool Mesh::isSmooth() const
{
	return m_smooth;
}

std::string Mesh::toDescription() const
{
	std::string triangleDescriptions;
	for (const Triangle& triangle : m_triangles)
	{
		triangleDescriptions += triangle.toDescription() + ",";
	}

	size_t pos = 0;
	int count = 0;
	while ((pos = triangleDescriptions.find(',', pos)) != std::string::npos)
	{
		pos++;
		count++;
	}

	std::stringstream ss;

	ss << DESCRIPTION_LABEL << ","

		<< 4 + count << ","

		<< m_name << ","
		<< m_smooth << ","

	    << triangleDescriptions
		
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

void Mesh::fromDescription(const std::string& description, const std::vector<Material*>& materials)
{
	std::vector<std::string> list = Utility::split(description, ',');

	int i = 2;

	m_name = list[i++];
	m_smooth = std::stoi(list[i++]);

	m_triangles.clear();
	while (list[i] == "Triangle")
	{
		int count = std::stoi(list[i + 1]);
		std::string triangleDescription;
		for (int j = i; j <= i + count + 1; ++j)
		{
			triangleDescription += list[j];
			if (j != i + count + 1) triangleDescription += ",";
		}

		Triangle triangle;
		triangle.fromDescription(triangleDescription, materials);

		m_triangles.push_back(triangle);

		i += count + 2;
	}

	setBoundingBox();

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

void Mesh::setTriangles(const std::vector<Triangle>& triangles)
{   
    m_triangles = triangles;

    setBoundingBox();  
}

void Mesh::setSmooth(bool smooth)
{
    m_smooth = smooth;
}

void Mesh::setEnabled(bool enabled)
{
	m_enabled = enabled;

	for (Triangle& triangle : m_triangles)
	{
		triangle.setEnabled(enabled);
	}
}

std::unique_ptr<ComponentShape> Mesh::clone() const
{
    return std::make_unique<Mesh>(*this);
}

void Mesh::translate(const Vector& translation)
{
    for (Triangle& triangle : m_triangles)
    {
        triangle.translate(translation);
    }

    setBoundingBox();
}

void Mesh::rotate(double degrees, const Vector& axis)
{
    for (Triangle& triangle : m_triangles)
    {
        triangle.rotate(degrees, axis);
    }

    setBoundingBox();
}

void Mesh::scale(const Vector& factors)
{
    for (Triangle& triangle : m_triangles)
    {
        triangle.scale(factors);
    }

    setBoundingBox();
}
