#pragma once

class Vector
{
public:

	double m_x = 0.0; 
	double m_y = 0.0; 
	double m_z = 0.0;

    Vector() = default;

    Vector(double x, double y, double z);

	Vector(const Vector& other) = default;

	Vector(Vector&& other) = default;

	Vector& operator=(const Vector& other) = default;

	Vector& operator=(Vector&& other) = default;

	~Vector() = default;

    double length() const;

    Vector perpendicular();

    Vector& normalize();
};


Vector operator-(const Vector& rhs);


Vector operator+(const Vector& lhs, const Vector& rhs);

Vector& operator+=(Vector& lhs, const Vector& rhs);


Vector operator-(const Vector& lhs, const Vector& rhs);

Vector& operator-=(Vector& lhs, const Vector& rhs);


Vector operator*(const Vector& lhs, const Vector& rhs);

Vector operator*(const Vector& lhs, double rhs);

Vector operator*(double lhs, const Vector& rhs);

Vector& operator*=(Vector& lhs, const Vector& rhs);

Vector& operator*=(Vector& lhs, double rhs);


Vector operator/(const Vector& lhs, const Vector& rhs);

Vector operator/(const Vector& lhs, double rhs);

Vector& operator/=(Vector& lhs, const Vector& rhs);

Vector& operator/=(Vector& lhs, double rhs);


bool operator==(const Vector& lhs, const Vector& rhs);

bool operator!=(const Vector& lhs, const Vector& rhs);


double dot(const Vector& lhs, const Vector& rhs);

Vector cross(const Vector& lhs, const Vector& rhs);

Vector reflect(const Vector& incident, const Vector& normal);

Vector normalize(const Vector& v);

Vector perpendicular(const Vector& v);

Vector getNormal(const Vector& v1, const Vector& v2, const Vector& v3);

double distance(const Vector& v1, const Vector& v2);
