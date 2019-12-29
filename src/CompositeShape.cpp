#include "CompositeShape.h"
#include "Triangle.h"
#include "Quadric.h"
#include "Quad.h"
#include "Plane.h"
#include "Mesh.h"
#include "Model.h"
#include "Utility.h"

#include <sstream>
#include <stack>

std::string CompositeShape::DESCRIPTION_LABEL = "CompositeShape";

CompositeShape::CompositeShape(
	Operation operation, 
	std::unique_ptr<ComponentShape> leftChild, 
	std::unique_ptr<ComponentShape> rightChild, 
	ComponentShape* parent,
	const Material* material, 
	const std::string& name)
	: ComponentShape(operation, parent, material, name)
	, m_leftChild(std::move(leftChild))
	, m_rightChild(std::move(rightChild))
{
}

CompositeShape::CompositeShape(const std::string& expression, const std::vector<ComponentShape*>& components, const Material* material, const std::string& name)
	: ComponentShape(Operation::NONE, nullptr, material, name)
{
	fromExpression(expression, components);
}

CompositeShape::CompositeShape(const std::string& description, const std::vector<Material*>& materials)
{
	fromDescription(description, materials);
}

CompositeShape::CompositeShape(const CompositeShape& other)
	: ComponentShape(other)
{
	if (other.m_leftChild != nullptr) m_leftChild = other.m_leftChild->clone();
	if (other.m_rightChild != nullptr) m_rightChild = other.m_rightChild->clone();
}

CompositeShape& CompositeShape::operator=(const CompositeShape& other)
{
	if (&other == this)
	{
		return *this;
	}

	ComponentShape::operator=(other);

	if (other.m_leftChild != nullptr)
	{
		m_leftChild = other.m_leftChild->clone();
	}
	else
	{
		m_leftChild = nullptr;
	}

	if (other.m_rightChild != nullptr)
	{
		m_rightChild = other.m_rightChild->clone();
	}
	else
	{
		m_rightChild = nullptr;
	}

	return *this;
}

std::string CompositeShape::getExpression() const
{
	return getExpressionRecursive(this);
}

std::string CompositeShape::toDescription() const
{
	std::string leftDescription;
	if (m_leftChild != nullptr)
	{
		leftDescription = m_leftChild->toDescription();
	}
	size_t pos = 0;
	int leftCount = 1;
	while ((pos = leftDescription.find(',', pos)) != std::string::npos)
	{
		pos++;
		leftCount++;
	}

	std::string rightDescription;
	if (m_rightChild != nullptr)
	{
		rightDescription = m_rightChild->toDescription();
	}
	pos = 0;
	int rightCount = 1;
	while ((pos = rightDescription.find(',', pos)) != std::string::npos)
	{
		pos++;
		rightCount++;
	}

	std::stringstream sst;

	sst << DESCRIPTION_LABEL << ","
	    << 4 + leftCount + rightCount << ","
		<< m_name << ","
		<< m_enabled << ",";

	if (m_material == nullptr)
	{
		sst << NO_MATERIAL_LABEL << ",";
	}
	else
	{
		sst << m_material->getName() << ",";
	}

	sst << static_cast<int>(m_operation) << ","
		<< leftDescription << ","
		<< rightDescription;

	return sst.str();
}

ComponentShape* CompositeShape::getLeftChild()
{
	return m_leftChild.get();
}

const ComponentShape* CompositeShape::getLeftChild() const
{
	return m_leftChild.get();
}

std::unique_ptr<ComponentShape>* CompositeShape::getLeftChildUPtr()
{
	return &m_leftChild;
}

ComponentShape* CompositeShape::getRightChild()
{
	return m_rightChild.get();
}

const ComponentShape* CompositeShape::getRightChild() const
{
	return m_rightChild.get();
}

std::unique_ptr<ComponentShape>* CompositeShape::getRightChildUPtr()
{
	return &m_rightChild;
}

bool CompositeShape::hasChildren() const
{
	return m_leftChild != nullptr || m_rightChild != nullptr;
}

Intersection CompositeShape::getIntersection(const Ray& ray) const
{
	std::vector<Intersection> intersections = getIntersections(ray);

	if (intersections.empty())
	{
		return Intersection();
	}
	else
	{
		return intersections[0];
	}
}

std::vector<Intersection> CompositeShape::getIntersections(const Ray& ray) const
{
	std::vector<Intersection> rightInts;
	if (m_rightChild != nullptr)
	{
		rightInts = m_rightChild->getIntersections(ray);
	}
	
	std::vector<Intersection> leftInts;
	if (m_leftChild != nullptr)
	{
		leftInts = m_leftChild->getIntersections(ray);
	}

	std::vector<Intersection> intersections;

	switch (m_operation)
	{
	case Operation::ADD:
		intersections = add(leftInts, rightInts);
		break;
	case Operation::INTERSECT:
		intersections = intersect(leftInts, rightInts);
		break;
	case Operation::SUBTRACT:
		intersections = subtract(leftInts, rightInts);
		break;
	case Operation::CLIP:
		intersections = clip(leftInts, *m_rightChild, ray);
		break;
	default:
		break;
	}

	if (m_material != nullptr)
	{
		for (Intersection& i : intersections)
		{
			i.m_material = m_material;
		}
	}

	return intersections;
}

bool CompositeShape::clipsPoint(const Vector& point) const
{
	switch (m_operation)
	{
	case Operation::ADD:
		return (m_leftChild != nullptr && m_leftChild->clipsPoint(point)) || (m_rightChild != nullptr && m_rightChild->clipsPoint(point));
	case Operation::INTERSECT:
		return (m_leftChild != nullptr && m_leftChild->clipsPoint(point)) && (m_rightChild != nullptr && m_rightChild->clipsPoint(point));
	case Operation::SUBTRACT:
	case Operation::CLIP:
		return (m_leftChild != nullptr && m_leftChild->clipsPoint(point)) && (m_rightChild == nullptr || !m_rightChild->clipsPoint(point));
	default:
		return false;
	}
}

std::unique_ptr<ComponentShape> CompositeShape::clone() const
{
	return std::make_unique<CompositeShape>(*this);
}

bool CompositeShape::fromExpression(const std::string& expression, const std::vector<ComponentShape*>& components)
{
	if (expression.find(',') != std::string::npos)
	{
		return false;
	}

	std::vector<std::string> tokens = tokenize(expression);

	std::vector<std::string> postfix = toPostfix(tokens);

	if (fromExpression(postfix, components))
	{
		setEnabled(m_enabled);
		return true;
	}
	else
	{
		return false;
	}
}

void CompositeShape::fromDescription(const std::string& description, const std::vector<Material*>& materials)
{
	std::vector<std::string> list = Utility::split(description, ',');

	int i = 1;
	int count = std::stoi(list[i++]);
	m_name = list[i++];
	m_enabled = std::stoi(list[i++]);
	m_material = nullptr;
	if (list[i] != NO_MATERIAL_LABEL)
	{
		for (const Material* material : materials)
		{
			if (material != nullptr && material->getName() == list[i])
			{
				m_material = material;
				break;
			}
		}
	}
	i++;
	m_operation = static_cast<Operation>(std::stoi(list[i++]));
	if (!list[i].empty())
	{
		int leftCount = std::stoi(list[i + 1]);
		std::string leftDescription;
		for (int j = i; j <= i + leftCount + 1; ++j)
		{
			leftDescription += list[j];
			if (j != i + leftCount + 1) leftDescription += ",";
		}
		m_leftChild = componentFromDescription(leftDescription, materials);
		if (m_leftChild != nullptr) m_leftChild->setParent(this);
		i += leftCount + 2;
	}
	else
	{
		m_leftChild = nullptr;
		i++;
	}
	if (!list[i].empty())
	{
		int rightCount = std::stoi(list[i + 1]);
		std::string rightDescription;
		for (int j = i; j <= i + rightCount + 1; ++j)
		{
			rightDescription += list[j];
			if (j != i + rightCount + 1) rightDescription += ",";
		}
		m_rightChild = componentFromDescription(rightDescription, materials);
		if (m_rightChild != nullptr) m_rightChild->setParent(this);
	}
	else
	{
		m_rightChild = nullptr;
	}
}

void CompositeShape::setEnabled(bool enabled)
{
	m_enabled = enabled;

	if (m_leftChild != nullptr)
	{
		m_leftChild->setEnabled(enabled);
	}

	if (m_rightChild != nullptr)
	{
		m_rightChild->setEnabled(enabled);
	}
}

bool CompositeShape::setOperation(Operation operation)
{
	m_operation = operation;

	return true;
}

bool CompositeShape::setLeftChild(std::unique_ptr<ComponentShape> component)
{
	m_leftChild = std::move(component);

	return true;
}

bool CompositeShape::setRightChild(std::unique_ptr<ComponentShape> component)
{
	m_rightChild = std::move(component);

	return true;
}

void CompositeShape::translate(const Vector& translation)
{
	if (m_leftChild != nullptr)
	{
		m_leftChild->translate(translation);
	}

	if (m_rightChild != nullptr)
	{
		m_rightChild->translate(translation);
	}
}

void CompositeShape::rotate(double degrees, const Vector& axis)
{
	if (m_leftChild != nullptr)
	{
		m_leftChild->rotate(degrees, axis);
	}

	if (m_rightChild != nullptr)
	{
		m_rightChild->rotate(degrees, axis);
	}
}

void CompositeShape::scale(const Vector& scaleFactors)
{
	if (m_leftChild != nullptr)
	{
		m_leftChild->scale(scaleFactors);
	}

	if (m_rightChild != nullptr)
	{
		m_rightChild->scale(scaleFactors);
	}
}

std::string CompositeShape::getExpressionRecursive(const ComponentShape* root) const
{
	std::string expression;

	if (root == nullptr)
	{
		return expression;
	}

	switch (m_operation)
	{
	case Operation::ADD:
		expression += "(" + root->getLeftChild()->getExpression();
		expression += " | ";
		expression += root->getRightChild()->getExpression() + ")";
		break;
	case Operation::INTERSECT:
		expression += "(" + root->getLeftChild()->getExpression();
		expression += " & ";
		expression += root->getRightChild()->getExpression() + ")";
		break;
	case Operation::SUBTRACT:
		expression += "(" + root->getLeftChild()->getExpression();
		expression += " - ";
		expression += root->getRightChild()->getExpression() + ")";
		break;
	case Operation::CLIP:
		expression += "(" + root->getLeftChild()->getExpression();
		expression += " / ";
		expression += root->getRightChild()->getExpression() + ")";
		break;
	default:
		break;
	}

	return expression;
}

std::unique_ptr<ComponentShape> CompositeShape::componentFromDescription(const std::string& description, const std::vector<Material*>& materials) const
{
	std::vector<std::string> list = Utility::split(description, ',');

	if (list[0] == Triangle::DESCRIPTION_LABEL)
	{
		return std::make_unique<Triangle>(description, materials);
	}
	else if (list[0] == Quadric::DESCRIPTION_LABEL)
	{
		return std::make_unique<Quadric>(description, materials);
	}
	else if (list[0] == Quad::DESCRIPTION_LABEL)
	{
		return std::make_unique<Quad>(description, materials);
	}
	else if (list[0] == Plane::DESCRIPTION_LABEL)
	{
		return std::make_unique<Plane>(description, materials);
	}
	else if (list[0] == Mesh::DESCRIPTION_LABEL)
	{
		return std::make_unique<Mesh>(description, materials);
	}
	else if (list[0] == Model::DESCRIPTION_LABEL)
	{
		return std::make_unique<Model>(description, materials);
	}
	else if (list[0] == CompositeShape::DESCRIPTION_LABEL)
	{
		return std::make_unique<CompositeShape>(description, materials);
	}
	else
	{
		return nullptr;
	}
}

std::vector<Intersection> CompositeShape::add(const std::vector<Intersection>& leftInts, const std::vector<Intersection>& rightInts) const
{
	if (leftInts.empty())
	{
		return rightInts;
	}
	else if (rightInts.empty())
	{
		return leftInts;
	}

	std::vector<Intersection> intersections;

	bool leftIn = leftInts[0].type == Intersection::IntersectionType::OUT;
	bool rightIn = rightInts[0].type == Intersection::IntersectionType::OUT;
	int leftIndex = 0;
	int rightIndex = 0;

	while (leftIndex < leftInts.size() || rightIndex < rightInts.size())
	{
		if (rightIndex >= rightInts.size() || (leftIndex < leftInts.size() && leftInts[leftIndex] < rightInts[rightIndex]))
		{
			if (!rightIn && leftInts[leftIndex].m_material != nullptr)
			{
				intersections.push_back(leftInts[leftIndex]);
			}

			leftIn = !leftIn;
			leftIndex++;
		}
		else
		{
			if (!leftIn && rightInts[rightIndex].m_material != nullptr)
			{
				intersections.push_back(rightInts[rightIndex]);
			}

			rightIn = !rightIn;
			rightIndex++;
		}
	}

	return intersections;
}

std::vector<Intersection> CompositeShape::intersect(const std::vector<Intersection>& leftInts, const std::vector<Intersection>& rightInts) const
{
	std::vector<Intersection> intersections;

	if (leftInts.empty() || rightInts.empty())
	{
		return intersections;
	}

	bool leftIn = leftInts[0].type == Intersection::IntersectionType::OUT;
	bool rightIn = rightInts[0].type == Intersection::IntersectionType::OUT;
	int leftIndex = 0;
	int rightIndex = 0;

	while (leftIndex < leftInts.size() || rightIndex < rightInts.size())
	{
		if (rightIndex >= rightInts.size() || (leftIndex < leftInts.size() && leftInts[leftIndex] < rightInts[rightIndex]))
		{
			if (rightIn && leftInts[leftIndex].m_material != nullptr)
			{
				intersections.push_back(leftInts[leftIndex]);
			}

			leftIn = !leftIn;
			leftIndex++;
		}
		else
		{
			if (leftIn && rightInts[rightIndex].m_material != nullptr)
			{
				intersections.push_back(rightInts[rightIndex]);
			}

			rightIn = !rightIn;
			rightIndex++;
		}
	}

	return intersections;
}

std::vector<Intersection> CompositeShape::subtract(const std::vector<Intersection>& leftInts, const std::vector<Intersection>& rightInts) const
{
	if (leftInts.empty() || rightInts.empty())
	{
		return leftInts;
	}

	std::vector<Intersection> intersections;

	bool leftIn = leftInts[0].type == Intersection::IntersectionType::OUT;
	bool rightIn = rightInts[0].type == Intersection::IntersectionType::OUT;
	int leftIndex = 0;
	int rightIndex = 0;

	while (leftIndex < leftInts.size() || rightIndex < rightInts.size())
	{
		if (leftIndex < leftInts.size() && rightIndex < rightInts.size() && leftInts[leftIndex].m_t == rightInts[rightIndex].m_t)
		{
			leftIn = !leftIn;
			leftIndex++;

			rightIn = !rightIn;
			rightIndex++;
		}
		else if (rightIndex >= rightInts.size() || (leftIndex < leftInts.size() && leftInts[leftIndex] < rightInts[rightIndex]))
		{
			if (!rightIn && leftInts[leftIndex].m_material != nullptr)
			{
				intersections.push_back(leftInts[leftIndex]);
			}

			leftIn = !leftIn;
			leftIndex++;
		}
		else
		{
			if (leftIn && rightInts[rightIndex].m_material != nullptr)
			{
				intersections.push_back(rightInts[rightIndex]);
				Intersection& intersection = intersections.back();
				if (intersection.type == Intersection::IntersectionType::IN)
				{
					intersection.type = Intersection::IntersectionType::OUT;
				}
				else
				{
					intersection.type = Intersection::IntersectionType::IN;
				}
			}

			rightIn = !rightIn;
			rightIndex++;
		}
	}

	return intersections;
}

std::vector<Intersection> CompositeShape::clip(const std::vector<Intersection>& intersections, const ComponentShape& component, const Ray& ray) const
{
	std::vector<Intersection> output = intersections;

	bool someAdded = false;
	bool someCut = false;

	for (auto it = output.begin(); it != output.end(); it++)
	{
		Vector point = ray.getPoint((*it).m_t);

		if (component.clipsPoint(point))
		{
			it = output.erase(it);
			it--;
			someCut = true;
		}
		else
		{
			someAdded = true;
		}
	}

	if (someAdded && someCut)
	{
		Intersection newIntersection = component.getIntersection(ray);

		if (m_material != nullptr)
		{
			newIntersection.m_material = m_material;
		}

		if (newIntersection.m_material != nullptr)
		{
			output.push_back(newIntersection);
		}
	}

	std::sort(output.begin(), output.end());

	return output;
}

std::vector<std::string> CompositeShape::tokenize(const std::string& expression) const
{
	std::string modifiedExpression = expression;
	modifiedExpression = Utility::replace(modifiedExpression, "(", " ( ");
	modifiedExpression = Utility::replace(modifiedExpression, ")", " ) ");
	modifiedExpression = Utility::replace(modifiedExpression, "&", " & ");
	modifiedExpression = Utility::replace(modifiedExpression, "|", " | ");
	modifiedExpression = Utility::replace(modifiedExpression, "-", " - ");
	modifiedExpression = Utility::replace(modifiedExpression, "/", " / ");

	std::vector<std::string> tokens;
	std::string token;
	for (char c : modifiedExpression)
	{
		if (c != ' ')
		{
			token += c;
		}
		else if (!token.empty())
		{
			if (token == m_name)
			{
				tokens.clear();
				return tokens;
			}
			tokens.push_back(token);
			token.clear();
		}
	}
	if (!token.empty())
	{
		tokens.push_back(token);
		token.clear();
	}

	return tokens;
}

std::vector<std::string> CompositeShape::toPostfix(const std::vector<std::string>& tokens) const
{
	std::stack<std::string> strStack;
	std::vector<std::string> postfix;

	postfix.reserve(tokens.size());

	for (const std::string& token : tokens)
	{
		if (token == "(")
		{
			strStack.push(token);
		}
		else if (token == ")")
		{
			while (!strStack.empty() && strStack.top() != "(")
			{
				postfix.push_back(strStack.top());
				strStack.pop();
			}
			if (strStack.empty() || strStack.top() != "(")
			{
				postfix.clear();
				return postfix;
			}
			else
			{
				strStack.pop();
			}
		}
		else if (token == "&" || token == "|" || token == "-" || token == "/")
		{
			while (!strStack.empty() && strStack.top() != "(")
			{
				postfix.push_back(strStack.top());
				strStack.pop();
			}
			strStack.push(token);
		}
		else
		{
			postfix.push_back(token);
		}
	}

	while (!strStack.empty())
	{
		if (strStack.top() != "(")
		{
			postfix.push_back(strStack.top());
			strStack.pop();
		}
		else
		{
			postfix.clear();
			return postfix;
		}
	}

	return postfix;
}

bool CompositeShape::fromExpression(const std::vector<std::string>& postfixTokens, const std::vector<ComponentShape*>& components)
{
	std::vector<ComponentShape*> unusedShapes = components;
	std::stack<std::unique_ptr<ComponentShape>> nodeStack;

	for (const std::string& token : postfixTokens)
	{
		if (token == "&")
		{
			if (nodeStack.empty()) return false;
			std::unique_ptr<ComponentShape> node2 = std::move(nodeStack.top());
			nodeStack.pop();
			if (nodeStack.empty())
			{
				return false;
			}
			std::unique_ptr<ComponentShape> node1 = std::move(nodeStack.top());
			nodeStack.pop();

			std::unique_ptr<ComponentShape> newNode = std::make_unique<CompositeShape>(Operation::INTERSECT, std::move(node1), std::move(node2), nullptr, nullptr);
			newNode->getLeftChild()->setParent(newNode.get());
			newNode->getRightChild()->setParent(newNode.get());

			nodeStack.push(std::move(newNode));
		}
		else if (token == "|")
		{
			if (nodeStack.empty()) return false;
			std::unique_ptr<ComponentShape> node2 = std::move(nodeStack.top());
			nodeStack.pop();
			if (nodeStack.empty())
			{
				return false;
			}
			std::unique_ptr<ComponentShape> node1 = std::move(nodeStack.top());
			nodeStack.pop();

			std::unique_ptr<ComponentShape> newNode = std::make_unique<CompositeShape>(Operation::ADD, std::move(node1), std::move(node2), nullptr, nullptr);
			newNode->getLeftChild()->setParent(newNode.get());
			newNode->getRightChild()->setParent(newNode.get());

			nodeStack.push(std::move(newNode));
		}
		else if (token == "-")
		{
			if (nodeStack.empty()) return false;
			std::unique_ptr<ComponentShape> node2 = std::move(nodeStack.top());
			nodeStack.pop();
			if (nodeStack.empty())
			{
				return false;
			}
			std::unique_ptr<ComponentShape> node1 = std::move(nodeStack.top());
			nodeStack.pop();

			std::unique_ptr<ComponentShape> newNode = std::make_unique<CompositeShape>(Operation::SUBTRACT, std::move(node1), std::move(node2), nullptr, nullptr);
			newNode->getLeftChild()->setParent(newNode.get());
			newNode->getRightChild()->setParent(newNode.get());

			nodeStack.push(std::move(newNode));
		}
		else if (token == "/")
		{
			if (nodeStack.empty()) return false;
			std::unique_ptr<ComponentShape> node2 = std::move(nodeStack.top());
			nodeStack.pop();
			if (nodeStack.empty())
			{
				return false;
			}
			std::unique_ptr<ComponentShape> node1 = std::move(nodeStack.top());
			nodeStack.pop();

			std::unique_ptr<ComponentShape> newNode = std::make_unique<CompositeShape>(Operation::CLIP, std::move(node1), std::move(node2), nullptr, nullptr);
			newNode->getLeftChild()->setParent(newNode.get());
			newNode->getRightChild()->setParent(newNode.get());

			nodeStack.push(std::move(newNode));
		}
		else
		{
			auto it = std::find_if(unusedShapes.begin(), unusedShapes.end(), [&token](const ComponentShape* unusedShape)
			{
				return unusedShape->getName() == token;
			});

			if (it != unusedShapes.end())
			{
				std::unique_ptr<ComponentShape> node = (*it)->clone();
				nodeStack.push(std::move(node));
				unusedShapes.erase(it);
			}
			else
			{
				return false;
			}
		}
	}

	if (nodeStack.empty())
	{
		return false;
	}

	std::unique_ptr<ComponentShape> node = std::move(nodeStack.top());
	nodeStack.pop();

	if (!nodeStack.empty())
	{
		return false;
	}

	node->setName(m_name);
	node->setMaterial(m_material);

	*this = *static_cast<CompositeShape*>(node.get());

	return true;
}
