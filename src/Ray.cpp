
#include <cmath>
#include <chrono>

#include "Ray.h"
#include "Constants.h"
#include "Quaternion.h"
#include "Matrix.h"

std::default_random_engine Ray::m_randomEngine(std::chrono::system_clock::now().time_since_epoch().count());

Ray::Ray(const Vector& start, const Vector& direction)
    : m_origin(start)
	, m_direction(direction)
{

}

const Vector& Ray::getOrigin() const
{
    return m_origin;
}

const Vector& Ray::getDirection() const
{
    return m_direction;
}

Vector Ray::getPoint(double t) const
{
    return m_origin + t * m_direction;
}

void Ray::setOrigin(const Vector& start)
{
    m_origin = start;
}

void Ray::setDirection(const Vector& direction)
{
    m_direction = direction;
}

void Ray::shiftOrigin(double shiftBy)
{
    m_origin += shiftBy * m_direction;
}

Ray Ray::reflect(const Vector& point, const Vector& normal) const
{
    return Ray(point, m_direction - 2 * dot(normal, m_direction) * normal);
}

Ray Ray::refract(const Vector& point, const Vector& normal, double n2, bool out) const
{
    double c1 = -dot(normal, m_direction);
    double n;

    if (out)
    {
        n = n2 / 1;
    }
    else
    {
        n = 1 / n2;
    }

	double k = 1 - n * n * (1 - c1 * c1);

    if (k >= 0.0)
    {
        return Ray(point, n * m_direction + (n * c1 - sqrt(k)) * normal);
    }
    else
    {
        return Ray(point, Vector(0, 0, 0));
    }
}

Ray Ray::distribute(double degrees) const
{
    if (degrees == 0.0)
    {
        return *this;
    }
    else
    {
        double radius = tan((degrees / 2) * (Constants::PI / 180));
        Vector position = getPoint(1.0);
        const Vector& point = getOrigin();

		std::uniform_int_distribution<int> distribution1(1, 100);
		double rad = distribution1(m_randomEngine) * 0.01 * radius;
		Vector perp = normalize(perpendicular(position - point));
		double ratio = (2 * Constants::PI * rad) / (2 * Constants::PI * radius);
		std::uniform_int_distribution<int> distribution2(0, static_cast<int>(360 * ratio) - 1);
		int pt = distribution2(m_randomEngine);
		double angle = pt * (360 / ratio);
		Matrix mat = Quaternion::getMatrix(angle, Vector(position - point));
		perp = normalize(mat * perp);

        Vector endPoint = position + (rad * perp);

        return Ray(point, endPoint - point);
    }
}
