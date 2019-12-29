#pragma once

#include "LeafShape.h"

class Triangle : public LeafShape
{
private:

    Vector m_v1, m_v2, m_v3;

    Vector m_n1, m_n2, m_n3;

public:

	static std::string DESCRIPTION_LABEL;

    Triangle() = default;

    Triangle(const Vector& v1, const Vector& v2, const Vector& v3, ComponentShape* parent, const Material* material, const std::string& name = "");

    Triangle(const Vector& v1, const Vector& v2, const Vector& v3, const Vector& n1, const Vector& n2, const Vector& n3, ComponentShape* parent, const Material* material, const std::string& name = "");

	Triangle(const std::string& description, const std::vector<Material*>& materials);
	
	Triangle(const Triangle& other) = default;

	Triangle(Triangle&& other) = default;

	Triangle& operator=(const Triangle& other) = default;

	Triangle& operator=(Triangle&& other) = default;

	~Triangle() override = default;

    Intersection getIntersection(const Ray& ray) const override;

    Intersection getIntersectionSmooth(const Ray& ray) const;

    std::vector<Intersection> getIntersections(const Ray& ray) const override;

    std::vector<Intersection> getIntersectionsSmooth(const Ray& ray) const;

	bool clipsPoint(const Vector& point) const override;

    const Vector& getV1() const;

    const Vector& getV2() const;

    const Vector& getV3() const;

    const Vector& getN1() const;

    const Vector& getN2() const;

    const Vector& getN3() const;

    std::unique_ptr<ComponentShape> clone() const override;

	std::string toDescription() const override;

	void setV1(const Vector& v1);

	void setV2(const Vector& v2);

	void setV3(const Vector& v3);

	void setN1(const Vector& n1);

	void setN2(const Vector& n2);

	void setN3(const Vector& n3);

	void fromDescription(const std::string& description, const std::vector<Material*>& materials) override;

    void translate(const Vector& translation) override;

    void rotate(double degrees, const Vector& axis) override;

    void scale(const Vector& scaleFactors) override;
};
