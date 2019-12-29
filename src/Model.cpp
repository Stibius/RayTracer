#include "Model.h"
#include "Utility.h"

#include <sstream>

std::string Model::DESCRIPTION_LABEL = "Model";

Model::Model(const std::vector<Mesh>& meshes, ComponentShape* parent, const Material* material, bool smooth, const std::string& name)
    : LeafShape(parent, material, name)
	, m_meshes(meshes)
{
    setSmooth(smooth);
}

Model::Model(const std::string& description, const std::vector<Material*>& materials)
{
	fromDescription(description, materials);
}

Intersection Model::getIntersection(const Ray& ray) const
{
    Intersection intersection;

    for (const Mesh& mesh : m_meshes)
    {
        Intersection i = mesh.getIntersection(ray);

        if (intersection.type == Intersection::IntersectionType::NONE || i.m_t < intersection.m_t)
        {
            intersection = i;
        }
    }

    if (intersection.type != Intersection::IntersectionType::NONE && m_material != nullptr)
    {
        intersection.m_material = m_material;
    }

    return intersection;
}

std::vector<Intersection> Model::getIntersections(const Ray& ray) const
{
    std::vector<Intersection> intersections;

    for (const Mesh& mesh : m_meshes)
    {
        std::vector<Intersection> i = mesh.getIntersections(ray);

        intersections.insert(intersections.end(), i.begin(), i.end());
    }

    for (Intersection& intersection : intersections)
    {
		if (m_material != nullptr)
		{
			intersection.m_material = m_material;
		}
    }

    std::sort(intersections.begin(), intersections.end());

    return intersections;
}

bool Model::clipsPoint(const Vector& point) const
{
	for (const Mesh& mesh : m_meshes)
	{
		if (mesh.clipsPoint(point))
		{
			return true;
		}
	}

	return false;
}

const std::vector<Mesh>& Model::getMeshes() const
{
    return m_meshes;
}

bool Model::isSmooth() const
{
    return m_smooth;
}

bool Model::isEmpty() const
{
    return m_meshes.empty();
}

std::string Model::toDescription() const
{
	std::string meshDescriptions;
	for (const Mesh& mesh : m_meshes)
	{
		meshDescriptions += mesh.toDescription() + ",";
	}

	size_t pos = 0;
	int count = 0;
	while ((pos = meshDescriptions.find(',', pos)) != std::string::npos)
	{
		pos++;
		count++;
	}

	std::stringstream ss;

	ss << DESCRIPTION_LABEL << ","

		<< 4 + count << ","

		<< m_name << ","
		<< m_smooth << ","

		<< meshDescriptions

        << m_enabled << ",";

	if (m_material == nullptr)
	{
		ss << NO_MATERIAL_LABEL;
	}
	else
	{
		ss << m_material->getName();
	}

	return ss.str();
}

void Model::fromDescription(const std::string& description, const std::vector<Material*>& materials)
{
	std::vector<std::string> list = Utility::split(description, ',');

	int i = 2;

	m_name = list[i++];
	m_smooth = std::stoi(list[i++]);

	m_meshes.clear();
	while (list[i] == "Mesh")
	{
		int count = std::stoi(list[i + 1]);
		std::string meshDescription;

		for (int j = i; j <= i + count + 1; ++j)
		{
			meshDescription += list[j];
			if (j != i + count + 1) meshDescription += ",";
		}

		Mesh mesh;
		mesh.fromDescription(meshDescription, materials);;

		m_meshes.push_back(mesh);

		i += count + 2;
	}

	m_enabled = std::stoi(list[i++]);

	m_material = nullptr;

	if (list[i] != NO_MATERIAL_LABEL)
	{
		for (const Material* material : materials)
		{
			if (material != nullptr && material->getName() == list[i])
			{
				m_material = material;
			}
		}
	}
}

void Model::setSmooth(bool smooth)
{
    m_smooth = smooth;

    for (Mesh& mesh : m_meshes)
    {
        mesh.setSmooth(smooth);
    }
}

void Model::setMeshes(const std::vector<Mesh>& meshes)
{
   m_meshes = meshes;
}

void Model::setEnabled(bool enabled)
{
	m_enabled = enabled;

	for (Mesh& mesh : m_meshes)
	{
		mesh.setEnabled(enabled);
	}
}

std::unique_ptr<ComponentShape> Model::clone() const
{
    return std::make_unique<Model>(*this);
}

void Model::translate(const Vector& translation)
{
    for (Mesh& mesh : m_meshes)
    {
        mesh.translate(translation);
    }
}

void Model::rotate(double degrees, const Vector& axis)
{
    for (Mesh& mesh : m_meshes)
    {
        mesh.rotate(degrees, axis);
    }
}

void Model::scale(const Vector& factors)
{
    for (Mesh& mesh : m_meshes)
    {
        mesh.scale(factors);
    }
}
