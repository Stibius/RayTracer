#include "Triangle.h"
#include "Plane.h"
#include "Quaternion.h"
#include "Utility.h"

#include <sstream>

std::string Triangle::DESCRIPTION_LABEL = "Triangle";

Triangle::Triangle(const Vector& v1, const Vector& v2, const Vector& v3, ComponentShape* parent, const Material* material, const std::string& name)
    : LeafShape(parent, material, name)
	, m_v1(v1)
	, m_v2(v2)
	, m_v3(v3)
{
}

Triangle::Triangle(const Vector& v1, const Vector& v2, const Vector& v3, const Vector& n1, const Vector& n2, const Vector& n3, ComponentShape* parent, const Material* material, const std::string& name)
    : LeafShape(parent, material, name)
	, m_v1(v1)
	, m_v2(v2)
	, m_v3(v3)
	, m_n1(normalize(n1))
	, m_n2(normalize(n2))
	, m_n3(normalize(n3))
{

}

Triangle::Triangle(const std::string& description, const std::vector<Material*>& materials)
{
	fromDescription(description, materials);
}

Intersection Triangle::getIntersection(const Ray& ray) const
{
    Vector normal = cross(m_v2 - m_v1, m_v3 - m_v1).normalize();

    const double EPSILON = 0.0000001;
    Vector vertex0 = m_v1;
    Vector vertex1 = m_v2;
    Vector vertex2 = m_v3;
    Vector edge1, edge2, h, s, q;
    float d,f,u,v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = cross(ray.getDirection(), edge2);
    d = dot(edge1, h);
    if (d > -EPSILON && d < EPSILON)
        return Intersection();
    f = 1/d;
    s = ray.getOrigin() - vertex0;
    u = f * (dot(s, h));
    if (u < 0.0 || u > 1.0)
        return Intersection();
    q = cross(s, edge1);
    v = f * dot(ray.getDirection(), q);
    if (v < 0.0 || u + v > 1.0)
        return Intersection();
    //At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * dot(edge2, q);
    if (t > EPSILON) //ray intersection
    {
        if (dot(normalize(ray.getDirection()), normal) > 0)
        {
            return Intersection(t, Intersection::IntersectionType::OUT, -normal, m_material);
        }
        else
        {
            return Intersection(t, Intersection::IntersectionType::IN, normal, m_material);
        }
    }
    else //This means that there is a line intersection but not a ray intersection.
        return Intersection();
}

Intersection Triangle::getIntersectionSmooth(const Ray& ray) const
{
    Intersection intersection = getIntersection(ray);

    if (intersection.m_t == 0)
    {
        return Intersection();
    }

    Vector point = ray.getPoint(intersection.m_t);

    Vector vv0 = m_v2 - m_v1;
    Vector vv1 = m_v3 - m_v1;
    Vector vv2 = point - m_v1;
    double d00 = dot(vv0, vv0);
    double d01 = dot(vv0, vv1);
    double d11 = dot(vv1, vv1);
    double d20 = dot(vv2, vv0);
    double d21 = dot(vv2, vv1);
    double denom = d00 * d11 - d01 * d01;
    double bar1 = (d11 * d20 - d01 * d21) / denom;
    double bar2 = (d00 * d21 - d01 * d20) / denom;
    double bar3 = 1.0f - bar1 - bar2;

    Vector normal2 = (bar3 * m_n1) + (bar1 * m_n2) + (bar2 * m_n3);

    if (dot(normalize(ray.getDirection()), normal2) > 0)
    {
        return Intersection(intersection.m_t, Intersection::IntersectionType::OUT, -normal2, m_material);
    }
    else
    {
        return Intersection(intersection.m_t, Intersection::IntersectionType::IN, normal2, m_material);
    }
}

std::vector<Intersection> Triangle::getIntersections(const Ray& ray) const
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

std::vector<Intersection> Triangle::getIntersectionsSmooth(const Ray& ray) const
{
	std::vector<Intersection> intersections;

    Intersection intersection = getIntersectionSmooth(ray);

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

bool Triangle::clipsPoint(const Vector& point) const
{
	Vector vv0 = m_v2 - m_v1;
	Vector vv1 = m_v3 - m_v1;
	Vector  normal = normalize(cross(vv0, vv1));
	if (dot(normal, m_n1) < 0)
	{
		normal *= -1;
	}
	Vector direction = normalize(m_v1 - point);
	if (dot(direction, normal) > 0)
	{
		double distance = dot(-direction, normal);
		Vector projectedPoint = point - distance * normal;

		Vector vv2 = projectedPoint - m_v1;
		double d00 = dot(vv0, vv0);
		double d01 = dot(vv0, vv1);
		double d11 = dot(vv1, vv1);
		double d20 = dot(vv2, vv0);
		double d21 = dot(vv2, vv1);
		double denom = d00 * d11 - d01 * d01;
		double bar1 = (d11 * d20 - d01 * d21) / denom;
		double bar2 = (d00 * d21 - d01 * d20) / denom;
		double bar3 = 1.0f - bar1 - bar2;

		if (bar1 >= 0 && bar1 <= 1 && bar2 >= 0 && bar2 <= 1 && bar3 >= 0 && bar3 <= 1)
		{
			return true;
		}
	}

	return false;
}

const Vector& Triangle::getV1() const
{
    return m_v1;
}

const Vector& Triangle::getV2() const
{
    return m_v2;
}

const Vector& Triangle::getV3() const
{
    return m_v3;
}

const Vector& Triangle::getN1() const
{
    return m_n1;
}

const Vector& Triangle::getN2() const
{
    return m_n2;
}

const Vector& Triangle::getN3() const
{
    return m_n3;
}

std::unique_ptr<ComponentShape> Triangle::clone() const
{
    return std::make_unique<Triangle>(*this);
}

std::string Triangle::toDescription() const
{
	std::stringstream ss;

	ss << DESCRIPTION_LABEL << ","

		<< 21 << ","

		<< m_name << ","

		<< m_v1.m_x << ","
		<< m_v1.m_y << ","
		<< m_v1.m_z << ","
		<< m_v2.m_x << ","
		<< m_v2.m_y << ","
		<< m_v2.m_z << ","
		<< m_v3.m_x << ","
		<< m_v3.m_y << ","
		<< m_v3.m_z << ","

		<< m_n1.m_x << ","
		<< m_n1.m_y << ","
		<< m_n1.m_z << ","
		<< m_n2.m_x << ","
		<< m_n2.m_y << ","
		<< m_n2.m_z << ","
		<< m_n3.m_x << ","
		<< m_n3.m_y << ","
		<< m_n3.m_z << ",";

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

void Triangle::setV1(const Vector& v1)
{
	m_v1 = v1;
}

void Triangle::setV2(const Vector& v2)
{
	m_v2 = v2;
}

void Triangle::setV3(const Vector& v3)
{
	m_v3 = v3;
}

void Triangle::setN1(const Vector& n1)
{
	m_n1 = n1;
}

void Triangle::setN2(const Vector& n2)
{
	m_n2 = n2;
}

void Triangle::setN3(const Vector& n3)
{
	m_n3 = n3;
}

void Triangle::fromDescription(const std::string& description, const std::vector<Material*>& materials)
{
	std::vector<std::string> list = Utility::split(description, ',');

	int i = 2;

	m_name = list[i++];

	m_v1 = Vector(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_v2 = Vector(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_v3 = Vector(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;

	m_n1 = Vector(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_n2 = Vector(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_n3 = Vector(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
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

void Triangle::translate(const Vector& translation)
{
    m_v1 += translation;
    m_v2 += translation;
    m_v3 += translation;
}

void Triangle::rotate(double degrees, const Vector& axis)
{
    Matrix rotation = Quaternion::getMatrix(degrees, axis);
    m_v1 = rotation * m_v1;
    m_v2 = rotation * m_v2;
    m_v3 = rotation * m_v3;
    m_n1 = rotation * m_n1;
    m_n2 = rotation * m_n2;
    m_n3 = rotation * m_n3;
}

void Triangle::scale(const Vector& factors)
{
    m_v1 = Vector(m_v1.m_x * factors.m_x, m_v1.m_y * factors.m_y, m_v1.m_z * factors.m_z);
    m_v2 = Vector(m_v2.m_x * factors.m_x, m_v2.m_y * factors.m_y, m_v2.m_z * factors.m_z);
    m_v3 = Vector(m_v3.m_x * factors.m_x, m_v3.m_y * factors.m_y, m_v3.m_z * factors.m_z);
}
