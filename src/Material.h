#pragma once

#include "Color.h"
#include "Vector.h"
#include "NamedEntity.h"
#include "EntityDescriptionInterface.h"

class MaterialProperties
{
public:

    Color m_diffuse;
    Color m_specular;
    Color m_ambient;
    int m_shininess = 0;

    double m_reflectance = 0.0;
    double m_reflectionDistAngle = 0.0;
    double m_transparency = 0.0;
    double m_refractionIndex = 1.0;
    double m_refractionDistAngle = 0.0;

    MaterialProperties() = default;

    MaterialProperties(const Color& diffuse, const Color& specular, const Color& ambient, int shininess, double reflectance = 0.0, double reflectionDistAngle = 0.0, double transparency = 0.0, double refractionIndex = 1.0, double refractionDistAngle = 0.0);

	MaterialProperties(const MaterialProperties& other) = default;

	MaterialProperties(MaterialProperties&& other) = default;

	MaterialProperties& operator=(const MaterialProperties& other) = default;

	MaterialProperties& operator=(MaterialProperties&& other) = default;

	~MaterialProperties() = default;
};

class Material : public NamedEntity, public EntityDescriptionInterface1
{
public:

    explicit Material(const std::string& name = "");

	Material(const Material& other) = default;

	Material(Material&& other) = default;

	Material& operator=(const Material& other) = default;

	Material& operator=(Material&& other) = default;

	~Material() override = default;

    virtual const MaterialProperties& getProperties(const Vector& position = Vector()) const = 0;

    virtual std::unique_ptr<Material> clone() const = 0;
};

class SimpleMaterial : public Material
{
private:

	MaterialProperties m_properties = MaterialProperties(Color(1, 0.5, 0.3), Color(0.5, 0.5, 0.5), Color(0.1, 0.05, 0.03), 32);

public:

	static std::string DESCRIPTION_LABEL;

    SimpleMaterial() = default;

    explicit SimpleMaterial(const MaterialProperties& properties, const std::string& name = "");

	SimpleMaterial(const SimpleMaterial& other) = default;

	SimpleMaterial(SimpleMaterial&& other) = default;

	SimpleMaterial& operator=(const SimpleMaterial& other) = default;

	SimpleMaterial& operator=(SimpleMaterial&& other) = default;

	~SimpleMaterial() override = default;

    const MaterialProperties& getProperties(const Vector& position) const override;

    const MaterialProperties& getProperties() const;

	std::string toDescription() const override;

	void fromDescription(const std::string& description) override;

    void setProperties(const MaterialProperties& properties);

    std::unique_ptr<Material> clone() const override;
};

class CheckerMaterial : public Material
{
private:

    MaterialProperties m_properties1 = MaterialProperties(Color(0.0, 0.0, 0.0), Color(1.0, 1.0, 1.0), Color(0.0, 0.0, 0.0), 15);
    MaterialProperties m_properties2 = MaterialProperties(Color(1.0, 1.0, 1.0), Color(1.0, 1.0, 1.0), Color(0.25, 0.25, 0.25), 15);

    Vector m_tileSize = Vector(1.0, 1.0, 1.0);

public:

	static std::string DESCRIPTION_LABEL;

    CheckerMaterial() = default;

    CheckerMaterial(const MaterialProperties& properties1, const MaterialProperties& properties2, const Vector& tileSize = Vector(1.0, 1.0, 1.0), const std::string& name = "");

	CheckerMaterial(const CheckerMaterial& other) = default;

	CheckerMaterial(CheckerMaterial&& other) = default;

	CheckerMaterial& operator=(const CheckerMaterial& other) = default;

	CheckerMaterial& operator=(CheckerMaterial&& other) = default;

	~CheckerMaterial() override = default;
	
	const MaterialProperties& getProperties(const Vector& position) const override;

    const MaterialProperties& getProperties1() const;

    const MaterialProperties& getProperties2() const;

    const Vector& getTileSize() const;

	std::string toDescription() const override;

	void fromDescription(const std::string& description) override;

    void setProperties(const MaterialProperties& properties1, const MaterialProperties& properties2);

    void setTileSize(const Vector& tileSize);

    std::unique_ptr<Material> clone() const override;
};
