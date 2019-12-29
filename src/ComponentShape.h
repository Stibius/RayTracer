#pragma once

#include "EntityDescriptionInterface.h"
#include "Intersection.h"
#include "Material.h"
#include "NamedEntity.h"
#include "Ray.h"

class ComponentShape : public NamedEntity, public EntityDescriptionInterface2
{
private:

	void getAllChildren(ComponentShape* root, std::vector<ComponentShape*>& children) const;

protected:

	static std::string NO_MATERIAL_LABEL;

public:

	enum class Operation
	{
		ADD,
		INTERSECT,
		SUBTRACT,
		CLIP,
		NONE
	};

	ComponentShape() = default;

	ComponentShape(
		Operation operation, 
		ComponentShape* parent, 
		const Material* material, 
		const std::string& name = "");

	ComponentShape(const ComponentShape& other);

	ComponentShape(ComponentShape&& other) = default;

	ComponentShape& operator=(const ComponentShape& other);

	ComponentShape& operator=(ComponentShape&& other) = default;

	~ComponentShape() override = default;

	const Material* getMaterial() const;

	bool isEnabled() const;

	virtual std::string getExpression() const;

	virtual Intersection getIntersection(const Ray& ray) const = 0;

	virtual std::vector<Intersection> getIntersections(const Ray& ray) const = 0;

	virtual bool clipsPoint(const Vector& point) const = 0;

	virtual std::unique_ptr<ComponentShape> clone() const = 0;

	Operation getOperation() const;

	ComponentShape* getParent();

	const ComponentShape* getParent() const;

	virtual ComponentShape* getLeftChild();

	virtual const ComponentShape* getLeftChild() const;

	virtual std::unique_ptr<ComponentShape>* getLeftChildUPtr();

	virtual ComponentShape* getRightChild();

	virtual const ComponentShape* getRightChild() const;

	virtual std::unique_ptr<ComponentShape>* getRightChildUPtr();

	std::vector<ComponentShape*> getAllChildren();

	virtual bool hasChildren() const;

	virtual bool fromExpression(const std::string& expression, const std::vector<ComponentShape*>& components);

	void setMaterial(const Material* material);

	virtual void setEnabled(bool enabled);

	void setParent(ComponentShape* component);

	virtual bool setOperation(Operation operation);

	virtual bool setLeftChild(std::unique_ptr<ComponentShape> component);

	virtual bool setRightChild(std::unique_ptr<ComponentShape> component);

	virtual void translate(const Vector& translation) = 0;

	virtual void rotate(double degrees, const Vector& axis) = 0;

	virtual void scale(const Vector& scaleFactors) = 0;

protected:

	Operation m_operation = Operation::NONE;
	ComponentShape* m_parent = nullptr;

	bool m_enabled = true;

	const Material* m_material = nullptr;
};
