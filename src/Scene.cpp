#include "Scene.h"

void Scene::changeMaterialInShape(ComponentShape* shape, const Material* oldMaterial, const Material* newMaterial)
{
	if (shape == nullptr)
	{
		return;
	}

	if (shape->getMaterial() == oldMaterial)
	{
		shape->setMaterial(newMaterial);
	}

	changeMaterialInShape(shape->getLeftChild(), oldMaterial, newMaterial);
	changeMaterialInShape(shape->getRightChild(), oldMaterial, newMaterial);
}

ComponentShape* Scene::setShape(ComponentShape* rootShape, ComponentShape* oldShape, const ComponentShape& newShape)
{
	if (rootShape == nullptr)
	{
		return nullptr;
	}

	ComponentShape* leftChild = rootShape->getLeftChild();
	if (leftChild != nullptr)
	{
		if (leftChild == oldShape)
		{
			std::unique_ptr<ComponentShape> newChild = newShape.clone();
			newChild->setParent(rootShape);
			rootShape->setLeftChild(std::move(newChild));
			return rootShape->getLeftChild();
		}

		ComponentShape* newChild = setShape(leftChild, oldShape, newShape);
		if (newChild != nullptr)
		{
			return newChild;
		}
	}

	ComponentShape* rightChild = rootShape->getRightChild();
	if (rightChild != nullptr)
	{
		if (rightChild == oldShape)
		{
			std::unique_ptr<ComponentShape> newChild = newShape.clone();
			newChild->setParent(rootShape);
			rootShape->setRightChild(std::move(newChild));
			return rootShape->getRightChild();
		}

		ComponentShape* newChild = setShape(rightChild, oldShape, newShape);
		if (newChild != nullptr)
		{
			return newChild;
		}
	}

	return nullptr;
}

bool Scene::eraseShape(std::unique_ptr<ComponentShape>* rootShape, ComponentShape* shape)
{
	if (rootShape == nullptr || *rootShape == nullptr)
	{
		return false;
	}

	std::unique_ptr<ComponentShape>* leftChild = (*rootShape)->getLeftChildUPtr();
	if (leftChild != nullptr && *leftChild != nullptr)
	{
		if (leftChild->get() == shape)
		{
			*rootShape = std::move(*(*rootShape)->getRightChildUPtr());
			return true;
		}

		if (eraseShape(leftChild, shape))
		{
			return true;
		}
	}

	std::unique_ptr<ComponentShape>* rightChild = (*rootShape)->getRightChildUPtr();
	if (rightChild != nullptr && *rightChild != nullptr)
	{
		if (rightChild->get() == shape)
		{
			*rootShape = std::move(*(*rootShape)->getLeftChildUPtr());
			return true;
		}

		if (eraseShape(rightChild, shape))
		{
			return true;
		}
	}

	return false;
}

Scene::Scene(const Scene& other)
{
	if (other.m_camera != nullptr)
	{
		m_camera = other.m_camera->clone();
	}

	m_lights.clear();
	m_lights.reserve(other.m_lights.size());
	for (const std::unique_ptr<Light>& light : other.m_lights)
	{
		m_lights.push_back(light->clone());
	}

	m_materials.clear();
	m_materials.reserve(other.m_materials.size());
	for (const std::unique_ptr<Material>& material : other.m_materials)
	{
		m_materials.push_back(material->clone());
	}

	m_shapes.clear();
	m_shapes.reserve(other.m_shapes.size());
	for (const std::unique_ptr<ComponentShape>& shape : other.m_shapes)
	{
		m_shapes.push_back(shape->clone());
	}
}

Scene &Scene::operator=(const Scene& other)
{
	if (&other == this)
	{
		return *this;
	}

	if (other.m_camera != nullptr)
	{
		m_camera = other.m_camera->clone();
	}
	else
	{
		m_camera = nullptr;
	}

	m_lights.clear();
	m_lights.reserve(other.m_lights.size());
	for (const std::unique_ptr<Light>& light : other.m_lights)
	{
		m_lights.push_back(light->clone());
	}

	m_materials.clear();
	m_materials.reserve(other.m_materials.size());
	for (const std::unique_ptr<Material>& material : other.m_materials)
	{
		m_materials.push_back(material->clone());
	}

	m_shapes.clear();
	m_shapes.reserve(other.m_shapes.size());
	for (const std::unique_ptr<ComponentShape>& shape : other.m_shapes)
	{
		m_shapes.push_back(shape->clone());
	}

	return *this;
}

Light* Scene::addLight(const Light& light)
{
	m_lights.push_back(light.clone());
	
	return m_lights.back().get();
}

Material* Scene::addMaterial(const Material& material)
{
	m_materials.push_back(material.clone());

	return m_materials.back().get();
}

ComponentShape* Scene::addShape(const ComponentShape& shape)
{
	m_shapes.push_back(shape.clone());

	return m_shapes.back().get();
}

void Scene::setCamera(const Camera& camera)
{
	m_camera = camera.clone();
}

Light* Scene::setLight(Light* oldLight, const Light& newLight)
{
	for (std::unique_ptr<Light>& light : m_lights)
	{
		if (light.get() == oldLight)
		{
			light = newLight.clone();

			return light.get();
		}
	}
	
	return nullptr;
}

Material* Scene::setMaterial(Material* oldMaterial, const Material& newMaterial)
{
	for (std::unique_ptr<Material>& material : m_materials)
	{
		if (material.get() == oldMaterial)
		{
			material = newMaterial.clone();

			for (std::unique_ptr<ComponentShape>& shape : m_shapes)
			{
				changeMaterialInShape(shape.get(), oldMaterial, material.get());
			}

			return material.get();
		}
	}

	return nullptr;
}

ComponentShape* Scene::setShape(ComponentShape* oldShape, const ComponentShape& newShape)
{
	for (std::unique_ptr<ComponentShape>& shape : m_shapes)
	{
		if (shape.get() == oldShape)
		{
			shape = newShape.clone();
			return shape.get();
		}
		else
		{
			ComponentShape* newShapePtr = setShape(shape.get(), oldShape, newShape);
			if (newShapePtr != nullptr)
			{
				return newShapePtr;
			}
		}
	}

	return nullptr;
}

bool Scene::eraseLight(Light* light)
{
	for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
	{
		if (it->get() == light)
		{
			m_lights.erase(it);

			return true;
		}
	}
	
	return false;
}

bool Scene::eraseMaterial(Material* material)
{
	for (auto it = m_materials.begin(); it != m_materials.end(); ++it)
	{
		if (it->get() == material)
		{
			m_materials.erase(it);

			for (std::unique_ptr<ComponentShape>& shape : m_shapes)
			{
				changeMaterialInShape(shape.get(), it->get(), nullptr);
			}

			return true;
		}
	}
	
	return false;
}

bool Scene::eraseShape(ComponentShape* shape)
{
	for (auto it = m_shapes.begin(); it != m_shapes.end(); ++it)
	{
		if (it->get() == shape)
		{
			m_shapes.erase(it);
			return true;
		}
		else
		{
			if (eraseShape(&*it, shape))
			{
				return true;
			}
		}
	}

	return false;
}

void Scene::clear()
{
	m_lights.clear();

	m_materials.clear();

	m_shapes.clear();
}

std::vector<EntityDescriptionInterface*> Scene::getDescriptions() 
{
	std::vector<EntityDescriptionInterface*> descriptions;

	descriptions.reserve(m_lights.size() + m_materials.size() + m_shapes.size() + 1);
	
	descriptions.push_back(m_camera.get());

	for (std::unique_ptr<Light>& light : m_lights)
	{
		descriptions.push_back(light.get());
	}

	for (std::unique_ptr<Material>& material : m_materials)
	{
		descriptions.push_back(material.get());
	}

	for (std::unique_ptr<ComponentShape>& shape : m_shapes)
	{
		descriptions.push_back(shape.get());
	}

	return descriptions;
}

Camera* Scene::getCamera()
{
	return m_camera.get();
}

std::vector<Light*> Scene::getLights() 
{
	std::vector<Light*> lights;

	lights.reserve(m_lights.size());
	for (const std::unique_ptr<Light>& light : m_lights)
	{
		lights.push_back(light.get());
	}

	return lights;
}

std::vector<Material*> Scene::getMaterials() 
{
	std::vector<Material*> materials;

	materials.reserve(m_materials.size());
	for (const std::unique_ptr<Material>& material : m_materials)
	{
		materials.push_back(material.get());
	}

	return materials;
}

std::vector<ComponentShape*> Scene::getShapes()
{
	std::vector<ComponentShape*> shapes;

	shapes.reserve(m_shapes.size());
	for (const std::unique_ptr<ComponentShape>& shape : m_shapes)
	{
		shapes.push_back(shape.get());
    }

	return shapes;
}
