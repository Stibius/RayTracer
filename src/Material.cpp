
#include <cmath>
#include <sstream>

#include "Material.h"
#include "Utility.h"

std::string SimpleMaterial::DESCRIPTION_LABEL = "SimpleMaterial";
std::string CheckerMaterial::DESCRIPTION_LABEL = "CheckerMaterial";

MaterialProperties::MaterialProperties(const Color& diffuse, const Color& specular, const Color& ambient, int shininess, double reflectance, double reflectionDistAngle, double transparency, double refractionIndex, double refractionDistAngle)
    : m_ambient(ambient)
	, m_diffuse(diffuse)
	, m_specular(specular)
	, m_shininess(shininess)
	, m_reflectance(reflectance)
	, m_reflectionDistAngle(reflectionDistAngle)
	, m_transparency(transparency)
	, m_refractionIndex(refractionIndex)
	, m_refractionDistAngle(refractionDistAngle)
{

}

Material::Material(const std::string& name)
    : NamedEntity(name)
{

}

SimpleMaterial::SimpleMaterial(const MaterialProperties& properties, const std::string& name)
    : Material(name)
	, m_properties(properties)
{

}

const MaterialProperties& SimpleMaterial::getProperties(const Vector& position) const
{
    return m_properties;
}

const MaterialProperties& SimpleMaterial::getProperties() const
{
    return m_properties;
}

std::string SimpleMaterial::toDescription() const
{
	std::stringstream ss;

	ss << DESCRIPTION_LABEL << ","

		<< 16 << ","

		<< m_name << ","
		<< m_properties.m_diffuse.m_red << ","
	    << m_properties.m_diffuse.m_green << ","
	    << m_properties.m_diffuse.m_blue << ","
	    << m_properties.m_specular.m_red << ","
	    << m_properties.m_specular.m_green << ","
	    << m_properties.m_specular.m_blue << ","
	    << m_properties.m_ambient.m_red << ","
	    << m_properties.m_ambient.m_green << ","
	    << m_properties.m_ambient.m_blue << ","
	    << m_properties.m_shininess << ","
	    << m_properties.m_reflectance << ","
	    << m_properties.m_reflectionDistAngle << ","
	    << m_properties.m_transparency << ","
	    << m_properties.m_refractionIndex << ","
	    << m_properties.m_refractionDistAngle;

	return ss.str();
}

void SimpleMaterial::fromDescription(const std::string& description)
{
	std::vector<std::string> list = Utility::split(description, ',');

	int i = 2;

	m_name = list[i++];
	m_properties.m_diffuse = Color(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_properties.m_specular = Color(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_properties.m_ambient = Color(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_properties.m_shininess = std::stoi(list[i++]);
	m_properties.m_reflectance = std::stod(list[i++]);
	m_properties.m_reflectionDistAngle = std::stod(list[i++]);
	m_properties.m_transparency = std::stod(list[i++]);
	m_properties.m_refractionIndex = std::stod(list[i++]);
	m_properties.m_refractionDistAngle = std::stod(list[i]);
}

void SimpleMaterial::setProperties(const MaterialProperties& properties)
{
    m_properties = properties;
}

std::unique_ptr<Material> SimpleMaterial::clone() const
{
    return std::make_unique<SimpleMaterial>(*this);
}

CheckerMaterial::CheckerMaterial(const MaterialProperties& , const MaterialProperties& properties2, const Vector& tileSize, const std::string& name)
    : Material(name)
	, m_properties1(m_properties1)
	, m_properties2(properties2)
	, m_tileSize(tileSize)
{

}

const MaterialProperties& CheckerMaterial::getProperties(const Vector& position) const
{
    return ((int(floor(position.m_x * (1 / m_tileSize.m_x) + 1e-5)) ^
             int(floor(position.m_y * (1 / m_tileSize.m_y) + 1e-5)) ^
             int(floor(position.m_z * (1 / m_tileSize.m_z) + 1e-5))) & 1) ? m_properties1 : m_properties2;
}

const MaterialProperties& CheckerMaterial::getProperties1() const
{
    return m_properties1;
}

const MaterialProperties& CheckerMaterial::getProperties2() const
{
    return m_properties2;
}

const Vector& CheckerMaterial::getTileSize() const
{
    return m_tileSize;
}

std::string CheckerMaterial::toDescription() const
{
	std::stringstream ss;

	ss << DESCRIPTION_LABEL << ","

		<< 34 << ","

		<< m_name << ","

		<< m_tileSize.m_x << ","
		<< m_tileSize.m_y << ","
		<< m_tileSize.m_z << ","

		<< m_properties1.m_diffuse.m_red << ","
		<< m_properties1.m_diffuse.m_green << ","
		<< m_properties1.m_diffuse.m_blue << ","
		<< m_properties1.m_specular.m_red << ","
		<< m_properties1.m_specular.m_green << ","
		<< m_properties1.m_specular.m_blue << ","
		<< m_properties1.m_ambient.m_red << ","
		<< m_properties1.m_ambient.m_green << ","
		<< m_properties1.m_ambient.m_blue << ","
		<< m_properties1.m_shininess << ","
		<< m_properties1.m_reflectance << ","
		<< m_properties1.m_reflectionDistAngle << ","
		<< m_properties1.m_transparency << ","
		<< m_properties1.m_refractionIndex << ","
		<< m_properties1.m_refractionDistAngle << ","

		<< m_properties2.m_diffuse.m_red << ","
		<< m_properties2.m_diffuse.m_green << ","
		<< m_properties2.m_diffuse.m_blue << ","
		<< m_properties2.m_specular.m_red << ","
		<< m_properties2.m_specular.m_green << ","
		<< m_properties2.m_specular.m_blue << ","
		<< m_properties2.m_ambient.m_red << ","
		<< m_properties2.m_ambient.m_green << ","
		<< m_properties2.m_ambient.m_blue << ","
		<< m_properties2.m_shininess << ","
		<< m_properties2.m_reflectance << ","
		<< m_properties2.m_reflectionDistAngle << ","
		<< m_properties2.m_transparency << ","
		<< m_properties2.m_refractionIndex << ","
		<< m_properties2.m_refractionDistAngle;

	return ss.str();
}

void CheckerMaterial::fromDescription(const std::string& description)
{
	std::vector<std::string> list = Utility::split(description, ',');

	int i = 2;

	m_name = list[i++];

	m_tileSize = Vector(std::stod(list[i++]), std::stod(list[i++]), std::stod(list[i++]));

	m_properties1.m_diffuse = Color(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_properties1.m_specular = Color(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_properties1.m_ambient = Color(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_properties1.m_shininess = std::stoi(list[i++]);
	m_properties1.m_reflectance = std::stod(list[i++]);
	m_properties1.m_reflectionDistAngle = std::stod(list[i++]);
	m_properties1.m_transparency = std::stod(list[i++]);
	m_properties1.m_refractionIndex = std::stod(list[i++]);
	m_properties1.m_refractionDistAngle = std::stod(list[i++]);

	m_properties2.m_diffuse = Color(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_properties2.m_specular = Color(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_properties2.m_ambient = Color(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_properties2.m_shininess = std::stoi(list[i++]);
	m_properties2.m_reflectance = std::stod(list[i++]);
	m_properties2.m_reflectionDistAngle = std::stod(list[i++]);
	m_properties2.m_transparency = std::stod(list[i++]);
	m_properties2.m_refractionIndex = std::stod(list[i++]);
	m_properties2.m_refractionDistAngle = std::stod(list[i]);
}

void CheckerMaterial::setProperties(const MaterialProperties& properties1, const MaterialProperties& properties2)
{
    m_properties1 = properties1;
    m_properties2 = properties2;
}

void CheckerMaterial::setTileSize(const Vector& tileSize)
{
    m_tileSize = tileSize;
}

std::unique_ptr<Material> CheckerMaterial::clone() const
{
    return std::make_unique<CheckerMaterial>(*this);
}
