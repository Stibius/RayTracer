#pragma once

#include "Triangle.h"
#include "Quad.h"

class Mesh : public LeafShape
{
private:

	std::vector<Triangle> m_triangles;
    std::vector<Quad> m_boundingBox;
	bool m_smooth = false;

    void setBoundingBox();

public:

	static std::string DESCRIPTION_LABEL;

    Mesh() = default;

	Mesh(const std::vector<Triangle>& triangles, ComponentShape* parent, const Material* material, bool smooth, const std::string& name = "");

	Mesh(const std::string& description, const std::vector<Material*>& materials);

	Mesh(const Mesh& other) = default;

	Mesh(Mesh&& other) = default;

	Mesh& operator=(const Mesh& other) = default;

	Mesh& operator=(Mesh&& other) = default;

	~Mesh() override = default;
	
	Intersection getIntersection(const Ray& ray) const override;

    std::vector<Intersection> getIntersections(const Ray& ray) const override;

	bool clipsPoint(const Vector& point) const override;

    const std::vector<Triangle>& getTriangles() const;

	bool isSmooth() const;

	std::string toDescription() const override;

	void fromDescription(const std::string& description, const std::vector<Material*>& materials) override;

    void setTriangles(const std::vector<Triangle>& triangles);

    void setSmooth(bool smooth);

	void setEnabled(bool enabled) override;

    std::unique_ptr<ComponentShape> clone() const override;

    void translate(const Vector& translation) override;

    void rotate(double degrees, const Vector& axis) override;

    void scale(const Vector& scaleFactors) override;
};
