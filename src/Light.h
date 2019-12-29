#pragma once

#include "Color.h"
#include "Vector.h"
#include "Ray.h"
#include "NamedEntity.h"
#include "EntityDescriptionInterface.h"

#include <random>

class Light : public NamedEntity, public EntityDescriptionInterface1
{
protected:

    bool m_enabled = true;
    Color m_color;
    Vector m_position;

public:

    Light() = default;

    Light(const Vector& position, const Color& color, const std::string& name = "");

	Light(const Light& other) = default;

	Light(Light&& other) = default;

	Light& operator=(const Light& other) = default;

	Light& operator=(Light&& other) = default;

	~Light() override = default;

    const Vector& getPosition() const;

    const Color& getColor() const;

    bool isEnabled() const;

    virtual Ray getShadowRay(const Vector& point, bool distribution = false) const = 0;

	virtual std::unique_ptr<Light> clone() const = 0;

    void setPosition(const Vector& position);

    void setColor(const Color& color);

    void setEnabled(bool enabled);

};

class PointLight : public Light
{
public:

	static std::string DESCRIPTION_LABEL;

    PointLight() = default;

    PointLight(const Vector& position, const Color& color, const std::string& name = "");

	PointLight(const PointLight& other) = default;

	PointLight(PointLight&& other) = default;

	PointLight& operator=(const PointLight& other) = default;

	PointLight& operator=(PointLight&& other) = default;

	~PointLight() override = default;

    Ray getShadowRay(const Vector& point, bool distribution = false) const override;

    std::unique_ptr<Light> clone() const override;

	std::string toDescription() const override;

	void fromDescription(const std::string& description) override;
};

class SphereLight : public Light
{
private:

    double m_radius = 1.0;

	static std::default_random_engine m_randomEngine;

public:

	static std::string DESCRIPTION_LABEL;

    SphereLight() = default;

    SphereLight(const Vector& position, const Color& color, double radius, const std::string& name = "");

	SphereLight(const SphereLight& other) = default;

	SphereLight(SphereLight&& other) = default;

	SphereLight& operator=(const SphereLight& other) = default;

	SphereLight& operator=(SphereLight&& other) = default;

	~SphereLight() override = default;

    double getRadius() const;

    Ray getShadowRay(const Vector& point, bool distribution = false) const override;

    std::unique_ptr<Light> clone() const override;

	std::string toDescription() const override;

	void setRadius(double radius);

	void fromDescription(const std::string& description) override;
};


