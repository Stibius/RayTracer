#include "ComponentShape.h"

std::string ComponentShape::NO_MATERIAL_LABEL = "None";

void ComponentShape::getAllChildren(ComponentShape* root, std::vector<ComponentShape*>& children) const
{
	if (root == nullptr)
	{
		return;
	}

	ComponentShape* leftChild = root->getLeftChild();
	if (leftChild != nullptr)
	{
		children.push_back(leftChild);
		getAllChildren(leftChild, children);
	}

	ComponentShape* rightChild = root->getRightChild();
	if (rightChild != nullptr)
	{
		children.push_back(rightChild);
		getAllChildren(rightChild, children);
	}
}

ComponentShape::ComponentShape(
	Operation operation,
	ComponentShape* parent,
	const Material* material,
	const std::string& name)
	: NamedEntity(name)
	, m_operation(operation)
	, m_parent(parent)
	, m_material(material)
{
	setEnabled(true);
}

ComponentShape::ComponentShape(const ComponentShape& other)
	: NamedEntity(other)
	, m_material(other.m_material)
	, m_enabled(other.m_enabled)
	, m_operation(other.m_operation)
	, m_parent(nullptr)
{
}

ComponentShape& ComponentShape::operator=(const ComponentShape& other)
{
	if (&other == this)
	{
		return *this;
	}

	NamedEntity::operator=(other);

	m_material = other.m_material;
	m_enabled = other.m_enabled;
	m_operation = other.m_operation;
	m_parent = nullptr;

	return *this;
}

const Material* ComponentShape::getMaterial() const
{
	return m_material;
}

bool ComponentShape::isEnabled() const
{
	return m_enabled;
}

std::string ComponentShape::getExpression() const
{
	return m_name;
}

void ComponentShape::setMaterial(const Material* material)
{
	m_material = material;
}

void ComponentShape::setEnabled(bool enabled)
{
	m_enabled = enabled;
}

void ComponentShape::setParent(ComponentShape* component)
{
	m_parent = component;
}

bool ComponentShape::setOperation(Operation operation)
{
	return false;
}

bool ComponentShape::setLeftChild(std::unique_ptr<ComponentShape> component)
{
	return false;
}

bool ComponentShape::setRightChild(std::unique_ptr<ComponentShape> component)
{
	return false;
}

ComponentShape::Operation ComponentShape::getOperation() const
{
	return m_operation;
}

ComponentShape* ComponentShape::getParent() 
{
	return m_parent;
}

const ComponentShape* ComponentShape::getParent() const
{
	return m_parent;
}

ComponentShape* ComponentShape::getLeftChild()
{
	return nullptr;
}

const ComponentShape* ComponentShape::getLeftChild() const
{
	return nullptr;
}

std::unique_ptr<ComponentShape>* ComponentShape::getLeftChildUPtr()
{
	return nullptr;
}

ComponentShape* ComponentShape::getRightChild()
{
	return nullptr;
}

const ComponentShape* ComponentShape::getRightChild() const
{
	return nullptr;
}

std::unique_ptr<ComponentShape>* ComponentShape::getRightChildUPtr()
{
	return nullptr;
}

std::vector<ComponentShape*> ComponentShape::getAllChildren() 
{
	std::vector<ComponentShape*> children;
	
	getAllChildren(this, children);

	return children;
}

bool ComponentShape::hasChildren() const
{
	return false;
}

bool ComponentShape::fromExpression(const std::string& expression, const std::vector<ComponentShape*>& components)
{
	return false;
}
