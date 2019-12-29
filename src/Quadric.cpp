#include <algorithm>
#include <sstream>

#include "Quadric.h"
#include "Constants.h"
#include "Utility.h"

std::string Quadric::DESCRIPTION_LABEL = "Quadric";

Quadric::Quadric(double a, double b, double c, double d, double e,
                 double f, double g, double h, double i, double j,
	             ComponentShape* parent, const Material* material, const std::string& name)
    : LeafShape(parent, material, name)
	, m_a(a)
	, m_b(b)
	, m_c(c)
	, m_d(d)
	, m_e(e)
	, m_f(f)
	, m_g(g)
	, m_h(h)
	, m_i(i)
	, m_j(j)
{

}

Quadric::Quadric(const std::string& description, const std::vector<Material*>& materials)
{
	fromDescription(description, materials);
}

Intersection Quadric::getIntersection(const Ray& ray) const
{
    const Vector& start = ray.getOrigin();
	const Vector& direction = ray.getDirection();

    double aa = 
		(m_a * direction.m_x * direction.m_x) +
        (m_b * direction.m_y * direction.m_y) +
        (m_c * direction.m_z * direction.m_z) +
        (m_d * direction.m_x * direction.m_y) +
        (m_e * direction.m_x * direction.m_z) +
        (m_f * direction.m_y * direction.m_z);

    double bb = 
		(2 * m_a * start.m_x * direction.m_x) +
        (2 * m_b * start.m_y * direction.m_y) +
        (2 * m_c * start.m_z * direction.m_z) +
        (m_d * ((start.m_x * direction.m_y) + (start.m_y * direction.m_x))) +
        (m_e * ((start.m_x * direction.m_z) + (start.m_z * direction.m_x))) +
        (m_f * ((start.m_y * direction.m_z) + (direction.m_y * start.m_z))) +
        (m_g * direction.m_x) +
        (m_h * direction.m_y) +
        (m_i * direction.m_z);

    double cc = 
		(m_a * start.m_x * start.m_x) +
        (m_b * start.m_y * start.m_y) +
        (m_c * start.m_z * start.m_z) +
        (m_d * start.m_x * start.m_y) +
        (m_e * start.m_x * start.m_z) +
        (m_f * start.m_y * start.m_z) +
        (m_g * start.m_x) +
        (m_h * start.m_y) +
        (m_i * start.m_z) +
        m_j;

    if (aa == 0.0)
    {
		double t = -cc / bb;

        if (t > 0)
        {
            Vector point = Ray(start, direction).getPoint(t);
            Vector normal = Vector((2 * m_a * point.m_x) + (m_d * point.m_y) + (m_e * point.m_z) + m_g,
                                   (2 * m_b * point.m_y) + (m_d * point.m_x) + (m_f * point.m_z) + m_h,
                                   (2 * m_c * point.m_z) + (m_e * point.m_x) + (m_f * point.m_y) + m_i);

            normal = normalize(normal);

            if (dot(normalize(direction), normal) > 0)
            {
                return Intersection(t, Intersection::IntersectionType::OUT, -normal, m_material);
            }
            else
            {
                return Intersection(t, Intersection::IntersectionType::IN, normal, m_material);
            }
        }
    }
    else
    {
		double D = bb * bb - 4 * aa * cc;

        if (D > 0)
        {
            double sD = sqrt(D);
			double t1 = (-bb - sD) / (2 * aa);
			double t2 = (-bb + sD) / (2 * aa);

            if (t1 > 0)
            {
                Vector point = Ray(start, direction).getPoint(t1);
                Vector normal = Vector((2 * m_a * point.m_x) + (m_d * point.m_y) + (m_e * point.m_z) + m_g,
                                       (2 * m_b * point.m_y) + (m_d * point.m_x) + (m_f * point.m_z) + m_h,
                                       (2 * m_c * point.m_z) + (m_e * point.m_x) + (m_f * point.m_y) + m_i);

                normal = normalize(normal);

                if (dot(normalize(direction), normal) > 0)
                {
                    return Intersection(t1, Intersection::IntersectionType::OUT, -normal, m_material);
                }
                else
                {
                    return Intersection(t1, Intersection::IntersectionType::IN, normal, m_material);
                }
            }

            if (t2 > 0)
            {
                Vector point = Ray(start, direction).getPoint(t2);
                Vector normal = Vector((2 * m_a * point.m_x) + (m_d * point.m_y) + (m_e * point.m_z) + m_g,
                                       (2 * m_b * point.m_y) + (m_d * point.m_x) + (m_f * point.m_z) + m_h,
                                       (2 * m_c * point.m_z) + (m_e * point.m_x) + (m_f * point.m_y) + m_i);

                normal = normalize(normal);

                if (dot(normalize(direction), normal) > 0)
                {
                    return Intersection(t2, Intersection::IntersectionType::OUT, -normal, m_material);
                }
                else
                {
                    return Intersection(t2, Intersection::IntersectionType::IN, normal, m_material);
                }
            }
        }
    }

	return Intersection();
}

std::vector<Intersection> Quadric::getIntersections(const Ray& ray) const
{
    std::vector<Intersection> intersections;

	const Vector& start = ray.getOrigin();
	const Vector& direction = ray.getDirection();

    double aa = 
		(m_a * direction.m_x * direction.m_x) +
        (m_b * direction.m_y * direction.m_y) +
        (m_c * direction.m_z * direction.m_z) +
        (m_d * direction.m_x * direction.m_y) +
        (m_e * direction.m_x * direction.m_z) +
        (m_f * direction.m_y * direction.m_z);

    double bb = 
		(2 * m_a * start.m_x * direction.m_x) +
        (2 * m_b * start.m_y * direction.m_y) +
        (2 * m_c * start.m_z * direction.m_z) +
        (m_d * ((start.m_x * direction.m_y) + (start.m_y * direction.m_x))) +
        (m_e * ((start.m_x * direction.m_z) + (start.m_z * direction.m_x))) +
        (m_f * ((start.m_y * direction.m_z) + (direction.m_y * start.m_z))) +
        (m_g * direction.m_x) +
        (m_h * direction.m_y) +
        (m_i * direction.m_z);

    double cc =
		(m_a * start.m_x * start.m_x) +
        (m_b * start.m_y * start.m_y) +
        (m_c * start.m_z * start.m_z) +
        (m_d * start.m_x * start.m_y) +
        (m_e * start.m_x * start.m_z) +
        (m_f * start.m_y * start.m_z) +
        (m_g * start.m_x) +
        (m_h * start.m_y) +
        (m_i * start.m_z) +
        m_j;

    if (aa == 0.0)
    {
		double t = -cc / bb;

        if (t > 0)
        {
            Vector point = Ray(start, direction).getPoint(t);
            Vector normal = Vector((2 * m_a * point.m_x) + (m_d * point.m_y) + (m_e * point.m_z) + m_g,
                                   (2 * m_b * point.m_y) + (m_d * point.m_x) + (m_f * point.m_z) + m_h,
                                   (2 * m_c * point.m_z) + (m_e * point.m_x) + (m_f * point.m_y) + m_i);

            normal = normalize(normal);

            if (dot(normalize(direction), normal) > 0)
            {
				intersections = 
				{
					Intersection(t, Intersection::IntersectionType::IN, -normal, m_material),
					Intersection(t, Intersection::IntersectionType::OUT, normal, m_material)
				};
            }
            else
            {
				intersections =
				{
					Intersection(t, Intersection::IntersectionType::IN, normal, m_material),
					Intersection(t, Intersection::IntersectionType::OUT, -normal, m_material)
				};
            }
        }
    }
    else
    {
		double D = bb * bb - 4 * aa * cc;

        if (D > 0)
        {
            double sD = sqrt(D);
			double t1 = (-bb - sD) / (2 * aa);
			double t2 = (-bb + sD) / (2 * aa);

            if (t1 > 0)
            {
                Vector point = Ray(start, direction).getPoint(t1);
                Vector normal = Vector((2 * m_a * point.m_x) + (m_d * point.m_y) + (m_e * point.m_z) + m_g,
                                       (2 * m_b * point.m_y) + (m_d * point.m_x) + (m_f * point.m_z) + m_h,
                                       (2 * m_c * point.m_z) + (m_e * point.m_x) + (m_f * point.m_y) + m_i);

                normal = normalize(normal);

                if (dot(normalize(direction), normal) > 0)
                {
                    intersections.push_back(Intersection(t1, Intersection::IntersectionType::OUT, -normal, m_material));
                }
                else
                {
                    intersections.push_back(Intersection(t1, Intersection::IntersectionType::IN, normal, m_material));
                }
            }

            if (t2 > 0)
            {
                Vector point = Ray(start, direction).getPoint(t2);
                Vector normal = Vector((2 * m_a * point.m_x) + (m_d * point.m_y) + (m_e * point.m_z) + m_g,
                                       (2 * m_b * point.m_y) + (m_d * point.m_x) + (m_f * point.m_z) + m_h,
                                       (2 * m_c * point.m_z) + (m_e * point.m_x) + (m_f * point.m_y) + m_i);

                normal = normalize(normal);

                if (dot(normalize(direction), normal) > 0)
                {
                    intersections.push_back(Intersection(t2, Intersection::IntersectionType::OUT, -normal, m_material));
                }
                else
                {
                    intersections.push_back(Intersection(t2, Intersection::IntersectionType::IN, normal, m_material));
                }
            }
        }
    }

    std::sort(intersections.begin(), intersections.end());

    return intersections;
}

bool Quadric::clipsPoint(const Vector& point) const
{
	return m_a * point.m_x * point.m_x +
		m_b * point.m_y * point.m_y +
		m_c * point.m_z * point.m_z +
		m_d * point.m_x * point.m_y +
		m_e * point.m_x * point.m_z +
		m_f * point.m_y * point.m_z +
		m_g * point.m_x +
		m_h * point.m_y +
		m_i * point.m_z +
		m_j <= 0;

}

double Quadric::getA() const
{
    return m_a;
}

double Quadric::getB() const
{
    return m_b;
}

double Quadric::getC() const
{
    return m_c;
}

double Quadric::getD() const
{
    return m_d;
}

double Quadric::getE() const
{
    return m_e;
}

double Quadric::getF() const
{
    return m_f;
}

double Quadric::getG() const
{
    return m_g;
}

double Quadric::getH() const
{
    return m_h;
}

double Quadric::getI() const
{
    return m_i;
}

double Quadric::getJ() const
{
    return m_j;
}

std::string Quadric::toDescription() const
{
	std::stringstream ss;

	ss << DESCRIPTION_LABEL << ","

		<< 13 << ","

		<< m_name << ","

		<< m_a << ","
		<< m_b << ","
		<< m_c << ","
		<< m_d << ","
		<< m_e << ","
		<< m_f << ","
		<< m_g << ","
		<< m_h << ","
		<< m_i << ","
		<< m_j << ",";

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

void Quadric::fromDescription(const std::string& description, const std::vector<Material*>& materials)
{
	std::vector<std::string> list = Utility::split(description, ',');

	int i = 2;

	m_name = list[i++];

	m_a = std::stod(list[i++]);
	m_b = std::stod(list[i++]);
	m_c = std::stod(list[i++]);
	m_d = std::stod(list[i++]);
	m_e = std::stod(list[i++]);
	m_f = std::stod(list[i++]);
	m_g = std::stod(list[i++]);
	m_h = std::stod(list[i++]);
	m_i = std::stod(list[i++]);
	m_j = std::stod(list[i++]);

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

void Quadric::setParameters(double a, double b, double c, double d, double e,
                            double f, double g, double h, double i, double j)
{
    m_a = a;
    m_b = b;
    m_c = c;
    m_d = d;
    m_e = e;
    m_f = f;
    m_g = g;
    m_h = h;
    m_i = i;
    m_j = j;
}

std::unique_ptr<ComponentShape> Quadric::clone() const
{
    return std::make_unique<Quadric>(*this);
}

void Quadric::translate(const Vector& translation)
{
    double aa = m_a;
    double bb = m_b;
    double cc = m_c;
    double dd = m_d;
    double ee = m_e;
    double ff = m_f;
    double gg = m_g;
    double hh = m_h;
    double ii = m_i;
    double jj = m_j;

    m_g = (-2 * aa * translation.m_x) - (dd * translation.m_y) - (ee * translation.m_z) + gg;
    m_h = (-2 * bb * translation.m_y) - (dd * translation.m_x) - (ff * translation.m_z) + hh;
    m_i = (-2 * cc * translation.m_z) - (ee * translation.m_x) - (ff * translation.m_y) + ii;
    m_j = (aa * translation.m_x * translation.m_x) +
            (bb * translation.m_y * translation.m_y) +
            (cc * translation.m_z * translation.m_z) +
            (dd * translation.m_x * translation.m_y) +
            (ee * translation.m_x * translation.m_z) +
            (ff * translation.m_y * translation.m_z) -
            (gg * translation.m_x) -
            (hh * translation.m_y) -
            (ii * translation.m_z) +
            jj;
}

void Quadric::rotate(double degrees, const Vector& axis)
{
	double radians = degrees * (-Constants::PI / 180);
    double w = cos(radians / 2);
    double x = axis.m_x * sin(radians / 2);
    double y = axis.m_y * sin(radians / 2);
    double z = axis.m_z * sin(radians / 2);

    double x2 = 2.0f * x, y2 = 2.0f * y, z2 = 2.0f * z;
    double xy = x2 * y, xz = x2 * z;
    double yy = y2 * y, yw = y2 * w;
    double zw = z2 * w, zz = z2 * z;
    double xx = x2 * x, xw = x2 * w, yz = y2 * z;

    double k = 1 - yy - zz;
    double l = xy - zw;
    double m = xz + yw;
    double n = xy + zw;
    double o = 1 - xx - zz;
    double p = yz - xw;
    double q = xz - yw;
    double r = yz + xw;
    double s = 1 - xx - yy;

    double aa = m_a;
    double bb = m_b;
    double cc = m_c;
    double dd = m_d;
    double ee = m_e;
    double ff = m_f;
    double gg = m_g;
    double hh = m_h;
    double ii = m_i;

	m_a = (aa * k * k) + (bb * n * n) + (cc * q * q) + (dd * k * n) + (ee * k * q) + (ff * n * q);

    m_b = (aa * l * l) + (bb * o * o) + (cc * r * r) + (dd * l * o) + (ee * l * r) + (ff * o * r);

    m_c = (aa * m * m) + (bb * p * p) + (cc * s * s) + (dd * m * p) + (ee * m * s) + (ff * p * s);

    m_d = (2.0 * aa * k * l) + (2.0 * bb * n * o) + (2.0 * cc * q * r) +
            (dd * k * o) + (dd * l * n) + (ee * k * r) + (ee * l * q) + (ff * n * r) + (ff * o * q);

    m_e = (2.0 * aa * k * m) + (2.0 * bb * n * p) + (2.0 * cc * q * s) +
            (dd * k * p) + (dd * m * n) + (ee * k * s) + (ee * m * q) + (ff * n * s) + (ff * p * q);

    m_f = (2.0 * aa * l * m) + (2.0 * bb * o * p) + (2.0 * cc * r * s) +
            (dd * l * p) + (dd * m * o) + (ee * l * s) + (ee * m * r) + (ff * o * s) + (ff * p * r);

    m_g = (gg * k) + (hh * n) + (ii * q);

    m_h = (gg * l) + (hh * o) + (ii * r);

    m_i = (gg * m) + (hh * p) + (ii * s);
}

void Quadric::scale(const Vector& factors)
{
	m_a /= factors.m_x * factors.m_x;
    m_b /= factors.m_y * factors.m_y;
    m_c /= factors.m_z * factors.m_z;
    m_d /= factors.m_x * factors.m_y;
    m_e /= factors.m_x * factors.m_z;
    m_f /= factors.m_y * factors.m_z;
    m_g /= factors.m_x;
    m_h /= factors.m_y;
    m_i /= factors.m_z;
}
