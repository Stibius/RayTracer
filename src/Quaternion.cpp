#include <cmath>

#include "Quaternion.h"
#include "Constants.h"

Quaternion::Quaternion(double degrees, const Vector& axis)
{
    double radians = degrees * (Constants::PI / 180);
    m_w = cos(radians / 2);
    m_x = axis.m_x * sin(radians / 2);
    m_y = axis.m_y * sin(radians / 2);
    m_z = axis.m_z * sin(radians / 2);
}

Matrix Quaternion::getMatrix() const
{
    double x2 = 2.0f * m_x, y2 = 2.0f * m_y, z2 = 2.0f * m_z;
    double xy = x2 * m_y, xz = x2 * m_z;
    double yy = y2 * m_y, yw = y2 * m_w;
    double zw = z2 * m_w, zz = z2 * m_z;
    double xx = x2 * m_x, xw = x2 * m_w, yz = y2 * m_z;

    Matrix matrix = Matrix(
                1 - yy - zz,     xy - zw,     xz + yw, 0,
                xy + zw, 1 - xx - zz,     yz - xw, 0,
                xz - yw,     yz + xw, 1 - xx - yy, 0,
                0,           0,           0, 1);

    return matrix;
}

Matrix Quaternion::getMatrix(double degrees, const Vector& axis)
{
	double radians = degrees * (Constants::PI / 180);
    double w = cos(radians / 2);
    double x = axis.m_x * sin(radians / 2);
    double y = axis.m_y * sin(radians / 2);
    double z = axis.m_z * sin(radians / 2);

    double x2 = 2.0f * x, y2 = 2.0f * y, z2 = 2.0f * z;
    double xy = x2 * y, xz = x2 * z;
    double yy = y2 * y, yw = y2 * w;
    double zw = z2 * w, zz = z2 * z;
    double xx = x2 * x, xw = x2 * w, yz = y2 * z;

    Matrix matrix = Matrix(
                1 - yy - zz,     xy - zw,     xz + yw, 0,
                xy + zw, 1 - xx - zz,     yz - xw, 0,
                xz - yw,     yz + xw, 1 - xx - yy, 0,
                0,           0,           0, 1);

    return matrix;
}

Quaternion Quaternion::operator*(const Quaternion& rhs)
{
	Quaternion result;

	result.m_w = (m_w * rhs.m_w) - (m_x * rhs.m_x) - (m_y * rhs.m_y) - (m_z * rhs.m_z);
	result.m_x = (m_w * rhs.m_x) + (m_x * rhs.m_w) + (m_y * rhs.m_z) - (m_z * rhs.m_y);
	result.m_y = (m_w * rhs.m_y) - (m_x * rhs.m_z) + (m_y * rhs.m_w) + (m_z * rhs.m_x);
	result.m_z = (m_w * rhs.m_z) + (m_x * rhs.m_y) - (m_y * rhs.m_x) + (m_z * rhs.m_w);

	return result;
}

Quaternion& Quaternion::operator*=(const Quaternion& rhs)
{
	*this = *this * rhs;

	return *this;
}
