#include <cmath>

#include "Vector.h"

Vector::Vector(double x, double y, double z)
    : m_x(x)
	, m_y(y)
	, m_z(z)
{

}

double Vector::length() const
{
    return sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
}

Vector Vector::perpendicular()
{
    if (m_y != 0.0)
    {
        return Vector(1.0,  -m_x / m_y, 0.0);
    }
    else if (m_x != 0.0)
    {
        return Vector(-m_y / m_x, 1.0, 0.0);
    }
    else if (m_z != 0.0)
    {
        return Vector(1.0, 0.0, -m_x / m_z);
    }
    else
    {
        return *this;
    }
}

Vector& Vector::normalize()
{
    double len = length();

    m_x /= len;
    m_y /= len;
    m_z /= len;

    return *this;
}

Vector operator-(const Vector& rhs)
{
	return Vector(-rhs.m_x, -rhs.m_y, -rhs.m_z);
}

Vector operator+(const Vector& lhs, const Vector& rhs)
{
	return Vector(lhs.m_x + rhs.m_x, lhs.m_y + rhs.m_y, lhs.m_z + rhs.m_z);
}

Vector& operator+=(Vector& lhs, const Vector& rhs)
{
	lhs.m_x += rhs.m_x;
	lhs.m_y += rhs.m_y;
	lhs.m_z += rhs.m_z;

	return lhs;
}

Vector operator-(const Vector& lhs, const Vector& rhs)
{
	return Vector(lhs.m_x - rhs.m_x, lhs.m_y - rhs.m_y, lhs.m_z - rhs.m_z);
}

Vector& operator-=(Vector& lhs, const Vector& rhs)
{
	lhs.m_x -= rhs.m_x;
	lhs.m_y -= rhs.m_y;
	lhs.m_z -= rhs.m_z;

	return lhs;
}

Vector operator*(const Vector& lhs, const Vector& rhs)
{
	return Vector(lhs.m_x * rhs.m_x, lhs.m_y * rhs.m_y, lhs.m_z * rhs.m_z);
}

Vector operator*(const Vector& lhs, double rhs)
{
	return Vector(lhs.m_x * rhs, lhs.m_y * rhs, lhs.m_z * rhs);
}

Vector operator*(double lhs, const Vector& rhs)
{
	return Vector(lhs * rhs.m_x, lhs * rhs.m_y, lhs * rhs.m_z);
}

Vector& operator*=(Vector& lhs, const Vector& rhs)
{
	lhs.m_x *= rhs.m_x;
	lhs.m_y *= rhs.m_y;
	lhs.m_z *= rhs.m_z;

	return lhs;
}

Vector& operator*=(Vector& lhs, double rhs)
{
	lhs.m_x *= rhs;
	lhs.m_y *= rhs;
	lhs.m_z *= rhs;

	return lhs;
}

Vector operator/(const Vector& lhs, const Vector& rhs)
{
	return Vector(lhs.m_x / rhs.m_x, lhs.m_y / rhs.m_y, lhs.m_z / rhs.m_z);
}

Vector operator/(const Vector& lhs, double rhs)
{
	return Vector(lhs.m_x / rhs, lhs.m_y / rhs, lhs.m_z / rhs);
}

Vector& operator/=(Vector& lhs, const Vector& rhs)
{
	lhs.m_x /= rhs.m_x;
	lhs.m_y /= rhs.m_y;
	lhs.m_z /= rhs.m_z;

	return lhs;
}

Vector& operator/=(Vector& lhs, double rhs)
{
	lhs.m_x /= rhs;
	lhs.m_y /= rhs;
	lhs.m_z /= rhs;

	return lhs;
}

bool operator==(const Vector& lhs, const Vector& rhs)
{
	return (lhs.m_x == rhs.m_x && lhs.m_y == rhs.m_y && lhs.m_z == rhs.m_z);
}

bool operator!=(const Vector& lhs, const Vector& rhs)
{
	return !(lhs == rhs);
}

double dot(const Vector& lhs, const Vector& rhs)
{
    return lhs.m_x * rhs.m_x + lhs.m_y * rhs.m_y + lhs.m_z * rhs.m_z;
}

Vector cross(const Vector& lhs, const Vector& rhs)
{
    return Vector(lhs.m_y * rhs.m_z - lhs.m_z * rhs.m_y, lhs.m_z * rhs.m_x - lhs.m_x * rhs.m_z, lhs.m_x * rhs.m_y - lhs.m_y * rhs.m_x);
}

Vector reflect(const Vector& incident, const Vector& normal)
{
    return incident - 2 * dot(normal, incident) * normal;
}

Vector normalize(const Vector& v)
{
    double length = v.length();

    return Vector(v.m_x / length, v.m_y / length, v.m_z / length);
}

Vector getNormal(const Vector& v1, const Vector& v2, const Vector& v3)
{
    return cross(v2 - v1, v3 - v1).normalize();
}

double distance(const Vector& v1, const Vector& v2)
{
    return sqrt(pow(v1.m_x - v2.m_x, 2) + pow(v1.m_y - v2.m_y, 2) + pow(v1.m_z - v2.m_z, 2));
}

Vector perpendicular(const Vector& v)
{
    if (v.m_y != 0.0)
    {
        return Vector(1.0,  -v.m_x / v.m_y, 0.0);
    }
    else if (v.m_x != 0.0)
    {
        return Vector(-v.m_y / v.m_x, 1.0, 0.0);
    }
    else if (v.m_z != 0.0)
    {
        return Vector(1.0, 0.0, -v.m_x / v.m_z);
    }
    else
    {
        return v;
    }
}
