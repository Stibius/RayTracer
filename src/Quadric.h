#pragma once

#include "LeafShape.h"

class Quadric : public LeafShape
{
private:

	double m_a = 1.0;
	double m_b = 1.0;
	double m_c = 1.0;
	double m_d = 0.0;
	double m_e = 0.0;
	double m_f = 0.0;
	double m_g = 0.0;
	double m_h = 0.0;
	double m_i = 0.0;
	double m_j = -1.0;

public:

	static std::string DESCRIPTION_LABEL;

    Quadric() = default;

    Quadric(double a, double b, double c, double d, double e,
            double f, double g, double h, double i, double j,
		    ComponentShape* parent, const Material* material, const std::string& name = "");

	Quadric(const std::string& description, const std::vector<Material*>& materials);

	Quadric(const Quadric& other) = default;

	Quadric(Quadric&& other) = default;

	Quadric& operator=(const Quadric& other) = default;

	Quadric& operator=(Quadric&& other) = default;

	~Quadric() override = default;

    Intersection getIntersection(const Ray& ray) const override;

    std::vector<Intersection> getIntersections(const Ray& ray) const override;

	bool clipsPoint(const Vector& point) const override;

    double getA() const;

    double getB() const;

    double getC() const;

    double getD() const;

    double getE() const;

    double getF() const;

    double getG() const;

    double getH() const;

    double getI() const;

    double getJ() const;

	std::string toDescription() const override;

	void fromDescription(const std::string& description, const std::vector<Material*>& materials) override;

    void setParameters(double a, double b, double c, double d, double e,
                       double f, double g, double h, double i, double j);

    std::unique_ptr<ComponentShape> clone() const override;

    void translate(const Vector& translation) override;

    void rotate(double degrees, const Vector& axis) override;

    void scale(const Vector& scaleFactors) override;
};
