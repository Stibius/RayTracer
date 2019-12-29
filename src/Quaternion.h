#pragma once

#include "Matrix.h"
#include "Vector.h"

class Quaternion
{
public:

    explicit Quaternion(double degrees = 0.0, const Vector& axis = Vector());

	Quaternion(const Quaternion& other) = default;

	Quaternion(Quaternion&& other) = default;

	Quaternion& operator=(const Quaternion& other) = default;

	Quaternion& operator=(Quaternion&& other) = default;

	~Quaternion() = default;

    Matrix getMatrix() const;

    static Matrix getMatrix(double degrees, const Vector& axis);

	Quaternion operator*(const Quaternion& rhs);

	Quaternion& operator*=(const Quaternion& rhs);

private:

    double m_w;
    double m_x;
    double m_y;
    double m_z;
};

