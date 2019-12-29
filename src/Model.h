#pragma once

#include <string>
#include <vector>

#include "Mesh.h"

class Model : public LeafShape
{
private:

    std::vector<Mesh> m_meshes;
    bool m_smooth = false;

public:

	static std::string DESCRIPTION_LABEL;

    Model() = default;

    Model(const std::vector<Mesh>& meshes, ComponentShape* parent, const Material* material, bool smooth, const std::string& name = "");

	Model(const std::string& description, const std::vector<Material*>& materials);

	Model(const Model& other) = default;

	Model(Model&& other) = default;

	Model& operator=(const Model& other) = default;

	Model& operator=(Model&& other) = default;

	~Model() override = default;

    Intersection getIntersection(const Ray& ray) const override;

    std::vector<Intersection> getIntersections(const Ray& ray) const override;

	bool clipsPoint(const Vector& point) const override;

    const std::vector<Mesh>& getMeshes() const;

    bool isSmooth() const;

    bool isEmpty() const;

	std::string toDescription() const override;

	void fromDescription(const std::string& description, const std::vector<Material*>& materials) override;

    void setSmooth(bool smooth);

    void setMeshes(const std::vector<Mesh>& meshes);

	void setEnabled(bool enabled) override;

    std::unique_ptr<ComponentShape> clone() const override;

    void translate(const Vector& translation) override;

    void rotate(double degrees, const Vector& axis) override;

    void scale(const Vector& scaleFactors) override;
};
