#include "SettingsIO.h"
#include "PerspectiveCamera.h"
#include "OrthoCamera.h"
#include "Application.h"
#include "Quad.h"
#include "Quadric.h"
#include "Model.h"
#include "CompositeShape.h"
#include "Utility.h"

#include <sstream>
#include <fstream>

void SettingsIO::saveSettings(const std::string& filename)
{
	std::ofstream ofs(filename);

	std::vector<EntityDescriptionInterface*> sceneDescriptions = Application::m_scene.getDescriptions();

	ofs << Application::m_rayTracer.toDescription() << "\n";

	for (const EntityDescriptionInterface* description : sceneDescriptions)
	{
		ofs << description->toDescription() << "\n";
	}

	ofs.close();
}

bool SettingsIO::loadSettings(const std::string& filename)
{
	std::ifstream ifs(filename);

	if (!ifs.is_open())
	{
		return false;
	}

	Application::m_scene.clear();

	std::string line;

	while (ifs)
	{
		std::getline(ifs, line);

		std::string label = Utility::split(line, ',')[0];

		if (label == RayTracer::DESCRIPTION_LABEL)
		{
			Application::m_rayTracer.set(line);
		}
		else if (label == PerspectiveCamera::DESCRIPTION_LABEL)
		{
			PerspectiveCamera camera;
			camera.fromDescription(line);
			Application::m_scene.setCamera(camera);
			
			Application::m_rayTracer.init(&Application::m_scene, Application::m_scene.getCamera());
			Application::m_keyEventHandler.init(Application::m_scene.getCamera());
			Application::m_mouseEventHandler.init(Application::m_scene.getCamera());
		}
		else if (label == OrthoCamera::DESCRIPTION_LABEL)
		{
			OrthoCamera camera;
			camera.fromDescription(line);
			Application::m_scene.setCamera(camera);

			Application::m_rayTracer.init(&Application::m_scene, Application::m_scene.getCamera());
			Application::m_keyEventHandler.init(Application::m_scene.getCamera());
			Application::m_mouseEventHandler.init(Application::m_scene.getCamera());
		}
		else if (label == PointLight::DESCRIPTION_LABEL)
		{
			PointLight pointLight;
			pointLight.fromDescription(line);
			Application::m_scene.addLight(pointLight);
		}
		else if (label == SphereLight::DESCRIPTION_LABEL)
		{
			SphereLight sphereLight;
			sphereLight.fromDescription(line);
			Application::m_scene.addLight(sphereLight);
		}
		else if (label == SimpleMaterial::DESCRIPTION_LABEL)
		{
			SimpleMaterial simpleMaterial;
			simpleMaterial.fromDescription(line);
			Application::m_scene.addMaterial(simpleMaterial);
		}
		else if (label == CheckerMaterial::DESCRIPTION_LABEL)
		{
			CheckerMaterial checkerMaterial;
			checkerMaterial.fromDescription(line);
			Application::m_scene.addMaterial(checkerMaterial);
		}
		else if (label == Triangle::DESCRIPTION_LABEL)
		{
			Triangle triangle;
			triangle.fromDescription(line, Application::m_scene.getMaterials());
			Application::m_scene.addShape(triangle);
		}
		else if (label == Quadric::DESCRIPTION_LABEL)
		{
			Quadric quadric;
			quadric.fromDescription(line, Application::m_scene.getMaterials());
			Application::m_scene.addShape(quadric);
		}
		else if (label == Quad::DESCRIPTION_LABEL)
		{
			Quad quad;
			quad.fromDescription(line, Application::m_scene.getMaterials());
			Application::m_scene.addShape(quad);
		}
		else if (label == Plane::DESCRIPTION_LABEL)
		{
			Plane plane;
			plane.fromDescription(line, Application::m_scene.getMaterials());
			Application::m_scene.addShape(plane);
		}
		else if (label == Mesh::DESCRIPTION_LABEL)
		{
			Mesh mesh;
			mesh.fromDescription(line, Application::m_scene.getMaterials());
			Application::m_scene.addShape(mesh);
		}
		else if (label == Model::DESCRIPTION_LABEL)
		{
			Model model;
			model.fromDescription(line, Application::m_scene.getMaterials());
			Application::m_scene.addShape(model);
		}
		else if (label == CompositeShape::DESCRIPTION_LABEL)
		{
			CompositeShape composite;
			composite.fromDescription(line, Application::m_scene.getMaterials());
			Application::m_scene.addShape(composite);
		}
	}

	ifs.close();

	return true;
}
