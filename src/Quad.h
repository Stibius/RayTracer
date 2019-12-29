#pragma once

#include "LeafShape.h"
#include "Plane.h"

class Quad : public LeafShape
{

private:

    Plane m_plane;
    Vector m_dimensions;

public:

	static std::string DESCRIPTION_LABEL;

    Quad() = default;

    Quad(const Plane& plane, const Vector& dimensions, ComponentShape* parent, const Material* material, const std::string& name = "");

	Quad(const std::string& description, const std::vector<Material*>& materials);

	Quad(const Quad& other) = default;

	Quad(Quad&& other) = default;

	Quad& operator=(const Quad& other) = default;

	Quad& operator=(Quad&& other) = default;

	~Quad() override = default;

    Intersection getIntersection(const Ray& ray) const override;

    std::vector<Intersection> getIntersections(const Ray& ray) const override;

	bool clipsPoint(const Vector& point) const override;

    const Plane& getPlane() const;

    const Vector& getDimensions() const;

    std::unique_ptr<ComponentShape> clone() const override;

	std::string toDescription() const override;

	void fromDescription(const std::string& description, const std::vector<Material*>& materials) override;

	void setPlane(const Plane& plane);
	
	void setDimensions(const Vector& dimensions);

    void translate(const Vector& translation) override;

    void rotate(double degrees, const Vector& axis) override;

    void scale(const Vector& scaleFactors) override;
};
