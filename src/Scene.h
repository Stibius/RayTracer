#pragma once

#include <memory>
#include <vector>

#include "PerspectiveCamera.h"
#include "Light.h"
#include "ComponentShape.h"
#include "Material.h"

class Scene
{

private:

	std::unique_ptr<Camera> m_camera = std::make_unique<PerspectiveCamera>();

	std::vector<std::unique_ptr<Light>> m_lights;
	
	std::vector<std::unique_ptr<Material>> m_materials;
	
	std::vector<std::unique_ptr<ComponentShape>> m_shapes;
	
	void changeMaterialInShape(ComponentShape* shape, const Material* oldMaterial, const Material* newMaterial);

	ComponentShape* setShape(ComponentShape* rootShape, ComponentShape* oldShape, const ComponentShape& newShape);

	bool eraseShape(std::unique_ptr<ComponentShape>* rootShape, ComponentShape* shape);

public:

    Scene() = default;

	Scene(const Scene& other);

	Scene(Scene&& other) = default;

	Scene& operator=(const Scene& other);

	Scene& operator=(Scene&& other) = default;

	~Scene() = default;

    Light* addLight(const Light& light);

	Material* addMaterial(const Material& material);

	ComponentShape* addShape(const ComponentShape& shape);

	void setCamera(const Camera& camera);

	Light* setLight(Light* oldLight, const Light& newLight);

	Material* setMaterial(Material* oldMaterial, const Material& newMaterial);

	ComponentShape* setShape(ComponentShape* oldShape, const ComponentShape& newShape);

	bool eraseLight(Light* light);

	bool eraseMaterial(Material* material);

	bool eraseShape(ComponentShape* shape);

	void clear();

	std::vector<EntityDescriptionInterface*> getDescriptions();

	Camera* getCamera();

	std::vector<Light*> getLights();

	std::vector<Material*> getMaterials();

	std::vector<ComponentShape*> getShapes();

};
