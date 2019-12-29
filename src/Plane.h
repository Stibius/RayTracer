#pragma once

#include "LeafShape.h"

class Plane : public LeafShape
{
private:

    Vector m_normal = Vector(0.0, 1.0, 0.0);
    Vector m_origin;
    double m_d;

public:

	static std::string DESCRIPTION_LABEL;

    Plane() = default;

    Plane(const Vector& normal, const Vector& origin, ComponentShape* parent, const Material* material, const std::string& name = "");

	Plane(const std::string& description, const std::vector<Material*>& materials);

	Plane(const Plane& other) = default;

	Plane(Plane&& other) = default;

	Plane& operator=(const Plane& other) = default;

	Plane& operator=(Plane&& other) = default;

	~Plane() override = default;

    Intersection getIntersection(const Ray& ray) const override;

    std::vector<Intersection> getIntersections(const Ray& ray) const override;

	bool clipsPoint(const Vector& point) const override;

    const Vector& getNormal() const;

    const Vector& getOrigin() const;

	std::string toDescription() const override;

	void fromDescription(const std::string& description, const std::vector<Material*>& materials) override;

    void setOrigin(const Vector& origin);

    void setNormal(const Vector& normal);

    Vector projectPoint(const Vector& point) const;

    std::unique_ptr<ComponentShape> clone() const override;

    void translate(const Vector& translation) override;

    void rotate(double degrees, const Vector& axis) override;

    void scale(const Vector& scaleFactors) override;
};
