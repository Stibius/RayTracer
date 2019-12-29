
#include <chrono>
#include <sstream>

#include "Light.h"
#include "Quaternion.h"
#include "Constants.h"
#include "Utility.h"

std::string PointLight::DESCRIPTION_LABEL = "PointLight";
std::string SphereLight::DESCRIPTION_LABEL = "SphereLight";

std::default_random_engine SphereLight::m_randomEngine(std::chrono::system_clock::now().time_since_epoch().count());

Light::Light(const Vector& position, const Color& color, const std::string& name)
    : NamedEntity(name)
	, m_position(position)
	, m_color(color)
	, m_enabled(true)
{

}

const Vector& Light::getPosition() const
{
    return m_position;
}

const Color& Light::getColor() const
{
    return m_color;
}

bool Light::isEnabled() const
{
    return m_enabled;
}

void Light::setPosition(const Vector& position)
{
    m_position = position;
}

void Light::setColor(const Color& color)
{
    m_color = color;
}

void Light::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

PointLight::PointLight(const Vector& position, const Color& color, const std::string& name)
    : Light(position, color, name)
{

}

Ray PointLight::getShadowRay(const Vector& point, bool distribution) const
{
    return Ray(point, getPosition() - point);
}

std::unique_ptr<Light> PointLight::clone() const
{
    return std::make_unique<PointLight>(*this);
}

std::string PointLight::toDescription() const
{
	std::stringstream ss;

	ss << DESCRIPTION_LABEL << ","

		<< 8 << ","

		<< m_name << ","
		<< m_position.m_x << ","
		<< m_position.m_y << ","
		<< m_position.m_z << ","
		<< m_color.m_red << ","
		<< m_color.m_green << ","
		<< m_color.m_blue << ","
		<< m_enabled;

	return ss.str();
}

void PointLight::fromDescription(const std::string& description)
{
	std::vector<std::string> list = Utility::split(description, ',');

	int i = 2;

	m_name = list[i++];
	m_position = Vector(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_color = Color(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_enabled = std::stoi(list[i]);
}

SphereLight::SphereLight(const Vector& position, const Color& color, double radius, const std::string& name)
    : Light(position, color, name)
	, m_radius(radius)
{
    
}

double SphereLight::getRadius() const
{
    return m_radius;
}

void SphereLight::setRadius(double radius)
{
	m_radius = radius;
}

Ray SphereLight::getShadowRay(const Vector& point, bool distribution) const
{
    if (distribution)
    {
		std::uniform_int_distribution<int> distribution1(1, 100);
        double rad = distribution1(m_randomEngine) * 0.01 * m_radius;
        Vector perp = normalize(perpendicular(getPosition() - point));
        double ratio = (2 * Constants::PI * rad) / (2 * Constants::PI * m_radius);
		std::uniform_int_distribution<int> distribution2(0, static_cast<int>(360 * ratio));
        int pt = distribution2(m_randomEngine);
        double angle = pt * (360 / ratio);
        Matrix mat = Quaternion::getMatrix(angle, Vector(getPosition() - point));
        perp = normalize(mat * perp);

        Vector lightPoint = getPosition() + (rad * perp);

        return Ray(point, lightPoint - point);
    }
    else
    {
        return Ray(point, getPosition() - point);
    }
}

std::unique_ptr<Light> SphereLight::clone() const
{
	return std::make_unique<SphereLight>(*this);
}

std::string SphereLight::toDescription() const
{
	std::stringstream ss;

	ss << DESCRIPTION_LABEL << ","

		<< 9 << ","

		<< m_name << ","
		<< m_radius << ","
		<< m_position.m_x << ","
		<< m_position.m_y << ","
		<< m_position.m_z << ","
		<< m_color.m_red << ","
		<< m_color.m_green << ","
		<< m_color.m_blue << ","
		<< m_enabled;

	return ss.str();
}

void SphereLight::fromDescription(const std::string& description)
{
	std::vector<std::string> list = Utility::split(description, ',');

	int i = 2;

	m_name = list[i++];
	m_radius = std::stod(list[i++]);
	m_position = Vector(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_color = Color(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_enabled = std::stoi(list[i]);
}
