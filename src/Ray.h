#pragma once

#include "Vector.h"

#include <random>

class Ray 
{
private:

    Vector m_origin;
    Vector m_direction;

	static std::default_random_engine m_randomEngine;

public:

    Ray() = default;

    Ray(const Vector& origin, const Vector& direction);

	Ray(const Ray& other) = default;

	Ray(Ray&& other) = default;

	Ray& operator=(const Ray& other) = default;

	Ray& operator=(Ray&& other) = default;

	~Ray() = default;

    const Vector& getOrigin() const;

    const Vector& getDirection() const;

    Vector getPoint(double t) const;

    void setOrigin(const Vector& origin);

    void setDirection(const Vector& direction);

	void shiftOrigin(double shiftBy = 1e-6);

    Ray reflect(const Vector& point, const Vector& normal) const;

    Ray refract(const Vector& point, const Vector& normal, double n2, bool out) const;

    Ray distribute(double degrees) const;
};
