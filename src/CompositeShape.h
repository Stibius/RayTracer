#pragma once

#include "ComponentShape.h"
#include "Plane.h"

class CompositeShape : public ComponentShape
{
public:

	static std::string DESCRIPTION_LABEL;

	CompositeShape() = default;

	CompositeShape(
		Operation operation,
		std::unique_ptr<ComponentShape> leftChild,
		std::unique_ptr<ComponentShape> rightChild,
		ComponentShape* parent,
		const Material* material,
		const std::string& name = "");

	CompositeShape(const std::string& expression, const std::vector<ComponentShape*>& components, const Material* material, const std::string& name = "");

	CompositeShape(const std::string& description, const std::vector<Material*>& materials);

	CompositeShape(const CompositeShape& other);

	CompositeShape(CompositeShape&& other) = default;

	CompositeShape& operator=(const CompositeShape& other);

	CompositeShape& operator=(CompositeShape&& other) = default;

	~CompositeShape() override = default;

	std::string getExpression() const override;

	std::string toDescription() const override;

	ComponentShape* getLeftChild() override;

	const ComponentShape* getLeftChild() const override;

	std::unique_ptr<ComponentShape>* getLeftChildUPtr() override;

	ComponentShape* getRightChild() override;

	const ComponentShape* getRightChild() const override;

	std::unique_ptr<ComponentShape>* getRightChildUPtr() override;

	bool hasChildren() const override;

	Intersection getIntersection(const Ray& ray) const override;

	std::vector<Intersection> getIntersections(const Ray& ray) const override;

	bool clipsPoint(const Vector& point) const override;

	std::unique_ptr<ComponentShape> clone() const override;

	bool fromExpression(const std::string& expression, const std::vector<ComponentShape*>& components) override;

	void fromDescription(const std::string& description, const std::vector<Material*>& materials) override;

	void setEnabled(bool enabled) override;

	bool setOperation(Operation operation) override;

	bool setLeftChild(std::unique_ptr<ComponentShape> component) override;

	bool setRightChild(std::unique_ptr<ComponentShape> component) override;

	void translate(const Vector& translation) override;

	void rotate(double degrees, const Vector& axis) override;

	void scale(const Vector& scaleFactors) override;

private:

	std::unique_ptr<ComponentShape> m_leftChild;
	std::unique_ptr<ComponentShape> m_rightChild;

	std::string getExpressionRecursive(const ComponentShape* root) const;

	std::unique_ptr<ComponentShape> componentFromDescription(const std::string& description, const std::vector<Material*>& materials) const;

	std::vector<Intersection> add(const std::vector<Intersection>& leftInts, const std::vector<Intersection>& rightInts) const;

	std::vector<Intersection> intersect(const std::vector<Intersection>& leftInts, const std::vector<Intersection>& rightInts) const;

	std::vector<Intersection> subtract(const std::vector<Intersection>& leftInts, const std::vector<Intersection>& rightInts) const;

	std::vector<Intersection> clip(const std::vector<Intersection>& intersections, const ComponentShape& component, const Ray& ray) const;

	std::vector<std::string> tokenize(const std::string& expression) const;

	std::vector<std::string> toPostfix(const std::vector<std::string>& tokens) const;

	bool fromExpression(const std::vector<std::string>& postfixTokens, const std::vector<ComponentShape*>& components);
};
