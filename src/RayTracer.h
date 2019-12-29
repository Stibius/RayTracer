#pragma once

#include <vector>

#include <QObject>

#include "Image.h"
#include "Scene.h"
#include "Vector.h"
#include "Color.h"
#include "Camera.h"
#include "EntityDescriptionInterface.h"

class RayTracer : public QObject, public EntityDescriptionInterface
{
    Q_OBJECT

private:

	Scene* m_scene = nullptr;
	Camera* m_camera = nullptr;

    bool m_previewMode = false;
	Color m_backgroundColor;
    bool m_adaptiveSuperSampling = true;
    double m_superSamplingThreshold = 0.1;
    int m_subSamplingSize = 20;
    int m_recursion = 5;
    int m_shadowDist = 1;
    int m_reflectionDist = 1;
    int m_refractionDist = 1;
    bool m_stop = false;
    Image m_image;

    Color traceRay(const Ray& ray, int recursion) const;

    Intersection getIntersection(const Ray& ray) const;

    double fresnel(const Vector& incident, const Vector& normal, double n1, double n2, bool out) const;

public:

	static std::string DESCRIPTION_LABEL;

    RayTracer() = default;

    RayTracer(int width, int height, Scene* scene = nullptr, Camera* camera = nullptr);

	RayTracer(const RayTracer& other) = default;

	RayTracer(RayTracer&& other) = default;

	RayTracer& operator=(const RayTracer& other) = default;

	RayTracer& operator=(RayTracer&& other) = default;

	~RayTracer() = default;

    int getWidth() const;

    int getHeight() const;

	const Color& getBackgroundColor() const;

    bool getAdaptiveSuperSampling() const;

    double getSuperSamplingThreshold() const;

    bool getPreviewMode() const;

    int getSubSamplingSize() const;

    int getRecursion() const;

    int getShadowDist() const;

    int getReflectionDist() const;

    int getRefractionDist() const;

    const Image& getImage() const;

    const unsigned char* getPixmap() const;

	std::string toDescription() const override;

	void init(Scene* scene, Camera* camera);

	void set(const std::string& description);

    void setSize(int width, int height);

	void setBackgroundColor(const Color& color);

    void setAdaptiveSuperSampling(bool adaptiveSuperSampling);

    void setSuperSamplingThreshold(double threshold);

    void setPreviewMode(bool previewMode);

    void setSubSamplingSize(int subSamplingSize);

    void setRecursion(int recursion);

    void setShadowDist(int shadowDist);

    void setReflectionDist(int reflectionDist);

    void setRefractionDist(int refractionDist);

    int render();

    void cancelRendering();

signals:

    void lineFinished();

};
