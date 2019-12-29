
#include <algorithm>
#include <chrono>
#include <sstream>

#include "RayTracer.h"
#include "Utility.h"

std::string RayTracer::DESCRIPTION_LABEL = "RayTracer";

Color RayTracer::traceRay(const Ray& ray, int recursion) const
{
    Intersection intersection = getIntersection(ray);

    if (intersection.type != Intersection::IntersectionType::NONE && intersection.m_material != nullptr)
    {
        Color color = Color(0.0, 0.0, 0.0);

        Vector point = ray.getPoint(intersection.m_t);
        const Vector& normal = intersection.m_normal;

        const MaterialProperties& materialProperties = intersection.m_material->getProperties(point);

        //ambient
        color += materialProperties.m_ambient;

        //lights
		std::vector<Light*> lights = m_scene->getLights();
        for (const Light* light : lights)
        {
            if (!light->isEnabled())
            {
                continue;
            }

            const Color& lightColor = light->getColor();

            Color colorSum;
            int shadowRayCount = 1;

            if (m_previewMode == false && dynamic_cast<const SphereLight*>(light) != nullptr && m_shadowDist > 1)
            {
                shadowRayCount = m_shadowDist;
            }

            for (int j = 0; j < shadowRayCount; ++j)
            {
                Ray shadowRay = light->getShadowRay(point, shadowRayCount > 1);
                shadowRay.shiftOrigin();

                if (dot(shadowRay.getDirection(), normal) < 0)
                {
                    continue;
                }

                Intersection shadowIntersection = getIntersection(shadowRay);

                if (shadowIntersection.m_t == 0 || shadowIntersection.m_t > 1)
                {
                    //diffuse
                    Vector lightDir = shadowRay.getDirection();
                    lightDir.normalize();
                    colorSum += lightColor * (fabs(dot(lightDir, normal)) * materialProperties.m_diffuse);

                    //specular
                    if (materialProperties.m_shininess != 0)
                    {
                        Vector reflectionDir = reflect(-lightDir, normal);
                        Vector viewDir = -ray.getDirection();
                        viewDir.normalize();
						double specular = dot(viewDir, reflectionDir);
                        if (specular > 0)
                        {
                            colorSum += lightColor * pow(specular, materialProperties.m_shininess) * materialProperties.m_specular;
                        }
                    }
                }
            }

            color += colorSum / shadowRayCount;
        }

        //reflected ray
        if (m_previewMode == false && materialProperties.m_reflectance > 0 && recursion > 0)
        {
            Ray reflectedRay = ray.reflect(point, normal);
            reflectedRay.shiftOrigin();

            if (m_reflectionDist == 1)
            {
                Color reflectedColor = traceRay(reflectedRay, recursion - 1);
                color += reflectedColor * materialProperties.m_reflectance;
            }
            else
            {
                Color colorSum;

                for (int i = 0; i < m_reflectionDist; ++i)
                {
                    Ray distRay = reflectedRay.distribute(materialProperties.m_reflectionDistAngle);
                    if (dot(distRay.getDirection(), normal) < 0)
                    {
                        i--;
                        continue;
                    }
                    Color reflectedColor = traceRay(distRay, recursion - 1);
                    colorSum += reflectedColor * materialProperties.m_reflectance;
                }

                color += colorSum / m_reflectionDist;
            }
        }

        //refracted ray
        if (m_previewMode == false && materialProperties.m_transparency > 0 && recursion > 0)
        {
            double n2 = intersection.m_material->getProperties().m_refractionIndex;
            Ray refractedRay;
            if (intersection.type == Intersection::IntersectionType::OUT)
            {
                refractedRay = ray.refract(point, normal, n2, true);
            }
            else
            {
                refractedRay = ray.refract(point, normal, n2, false);
            }

            const Vector& direction = refractedRay.getDirection();

            if (direction.m_x != 0 || direction.m_y != 0 || direction.m_z != 0)
            {
                refractedRay.shiftOrigin();

                if (m_refractionDist == 1)
                {
                    Color refractedColor = traceRay(refractedRay, recursion - 1);
                    color += refractedColor * materialProperties.m_transparency;
                }
                else
                {
                    Color colorSum;

                    for (int i = 0; i < m_refractionDist; ++i)
                    {
                        Ray distRay = refractedRay.distribute(materialProperties.m_refractionDistAngle);
                        if (dot(distRay.getDirection(), normal) < 0)
                        {
                            i--;
                            continue;
                        }
                        Color refractedColor = traceRay(distRay, recursion - 1);
                        colorSum += refractedColor * materialProperties.m_transparency;
                    }

                    color += colorSum / m_refractionDist;
                }
            }
        }

        return color;

    }
    else
    {
	    return m_backgroundColor;
    }
}

Intersection RayTracer::getIntersection(const Ray& ray) const
{
    Intersection intersection(0.0);

	std::vector<ComponentShape*> shapes = m_scene->getShapes();
    for (const ComponentShape* shape : shapes)
    {
        if (!shape->isEnabled())
        {
            continue;
        }

        Intersection intersection2 = shape->getIntersection(ray);

        if (intersection.m_t == 0.0)
        {
            if (intersection2.m_t > 0.0 && intersection2.m_material != nullptr) intersection = intersection2;
        }
        else if (intersection2.m_t > 0.0 && intersection2.m_t < intersection.m_t && intersection2.m_material != nullptr)
        {
            intersection = intersection2;
        }
    }

    return intersection;
}

double RayTracer::fresnel(const Vector& incident, const Vector& normal, double n1, double n2, bool out) const
{
    double cos1;
    double n;

    if (out)
    {
        cos1 = dot(-normal, incident);
        n = n2 / 1;
    }
    else
    {
        cos1 = dot(normal, incident);
        n = 1 / n2;
    }

	double sin2 = n * sqrt(std::max(0.0, 1 - cos1 * cos1));

    if (sin2 >= 1)
    {
        return 1;
    }
    else
    {
        double cos2 = sqrt(std::max(0.0, 1 - sin2 * sin2));
        cos1 = fabs(cos1);
        double Rs = ((n2 * cos1) - (n1 * cos2)) / ((n2 * cos1) + (n1 * cos2));
        double Rp = ((n1 * cos1) - (n2 * cos2)) / ((n1 * cos1) + (n2 * cos2));
        return (Rs * Rs + Rp * Rp) / 2;
    }
}

RayTracer::RayTracer(int width, int height, Scene* scene, Camera* camera)
	: m_image(Image(width, height))
	, m_scene(scene)
	, m_camera(camera)
    , m_adaptiveSuperSampling(true)
    , m_superSamplingThreshold(0.1)
    , m_previewMode(false)
    , m_subSamplingSize(20)
    , m_recursion(5)
    , m_shadowDist(1)
    , m_reflectionDist(1)
    , m_refractionDist(1)
    , m_stop(false)
{
    
}

const Color& RayTracer::getBackgroundColor() const
{
	return m_backgroundColor;
}

int RayTracer::getWidth() const
{
    return m_image.getWidth();
}

int RayTracer::getHeight() const
{
    return m_image.getHeight();
}

bool RayTracer::getAdaptiveSuperSampling() const
{
    return m_adaptiveSuperSampling;
}

double RayTracer::getSuperSamplingThreshold() const
{
    return m_superSamplingThreshold;
}

bool RayTracer::getPreviewMode() const
{
    return m_previewMode;
}

int RayTracer::getSubSamplingSize() const
{
    return m_subSamplingSize;
}

int RayTracer::getRecursion() const
{
    return m_recursion;
}

int RayTracer::getShadowDist() const
{
    return m_shadowDist;
}

int RayTracer::getReflectionDist() const
{
    return m_reflectionDist;
}

int RayTracer::getRefractionDist() const
{
    return m_refractionDist;
}

const Image& RayTracer::getImage() const
{
    return m_image;
}

const unsigned char* RayTracer::getPixmap() const
{
    return m_image.getPixmap();
}

std::string RayTracer::toDescription() const
{
	std::stringstream ss;

	ss << DESCRIPTION_LABEL << ","
		<< 10 << ","
		<< m_backgroundColor.m_red << ","
		<< m_backgroundColor.m_green << ","
		<< m_backgroundColor.m_blue << ","
		<< m_adaptiveSuperSampling << ","
		<< m_superSamplingThreshold << ","
		<< m_subSamplingSize << ","
		<< m_recursion << ","
		<< m_shadowDist << ","
		<< m_reflectionDist << ","
		<< m_refractionDist;

	return ss.str();
}

void RayTracer::init(Scene* scene, Camera* camera)
{
	m_scene = scene;
	m_camera = camera;
}

void RayTracer::set(const std::string& description)
{
	std::vector<std::string> list = Utility::split(description, ',');

	int i = 2;

	m_backgroundColor = Color(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_adaptiveSuperSampling = std::stoi(list[i++]);
	m_superSamplingThreshold = std::stod(list[i++]);
	m_subSamplingSize = std::stoi(list[i++]);
	m_recursion = std::stoi(list[i++]);
	m_shadowDist = std::stoi(list[i++]);
	m_reflectionDist = std::stoi(list[i++]);
	m_refractionDist = std::stoi(list[i++]);
}

void RayTracer::setSize(int width, int height)
{
    m_image = Image(width, height);
}

void RayTracer::setBackgroundColor(const Color& color)
{
	m_backgroundColor = color;
}

void RayTracer::setAdaptiveSuperSampling(bool adaptiveSuperSampling)
{
    m_adaptiveSuperSampling = adaptiveSuperSampling;
}

void RayTracer::setSuperSamplingThreshold(double threshold)
{
    m_superSamplingThreshold = threshold;
}

void RayTracer::setPreviewMode(bool subSampling)
{
    m_previewMode = subSampling;
}

void RayTracer::setSubSamplingSize(int subSamplingSize)
{
    m_subSamplingSize = subSamplingSize;
}

void RayTracer::setRecursion(int recursion)
{
    m_recursion = recursion;
}

void RayTracer::setShadowDist(int shadowDist)
{
    m_shadowDist = shadowDist;
}

void RayTracer::setReflectionDist(int reflectionDist)
{
    m_reflectionDist = reflectionDist;
}

void RayTracer::setRefractionDist(int refractionDist)
{
    m_refractionDist = refractionDist;
}

int RayTracer::render()
{
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    m_stop = false;

    int width = m_image.getWidth();
    int height = m_image.getHeight();

    int stepSize = 1;
    if (m_previewMode)
    {
        stepSize = m_subSamplingSize;
    }

    for (int y = 0; y < height; y += stepSize)
    {
        for (int x = 0; x < width; x += stepSize)
        {
            if (m_stop)
            {
				std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

				std::chrono::duration<int64_t, std::micro> elapsed = std::chrono::duration_cast<std::chrono::duration<int64_t, std::micro>>(t2 - t1);

				return elapsed.count();
            }

            double xx = x + 0.5 * stepSize;
            if (xx >= width) xx = x + (0.5 * (width - x));
            double yy = y + 0.5 * stepSize;
            if (yy >= height) yy = y + (0.5 * (height - y));
            Ray ray = m_camera->getRay(width, height, xx, yy);
            Color color = traceRay(ray, m_recursion);

            if (m_previewMode == false && m_adaptiveSuperSampling == true && x > 0 && y > 0)
            {
                const Color& leftColor = m_image.getPixel(x - 1, y);
				const Color& topColor = m_image.getPixel(x, y - 1);

                if (distance(color, leftColor) > m_superSamplingThreshold || distance(color, topColor) > m_superSamplingThreshold)
                {
                    Color color2;

                    ray = m_camera->getRay(width, height, x - 0.25, y + 0.25);
                    color2 = traceRay(ray, m_recursion);
                    color.accumulate(color2, 0.6);

                    ray = m_camera->getRay(width, height, x + 0.25, y + 0.25);
                    color2 = traceRay(ray, m_recursion);
                    color.accumulate(color2, 0.6);

                    ray = m_camera->getRay(width, height, x - 0.25, y - 0.25);
                    color2 = traceRay(ray, m_recursion);
                    color.accumulate(color2, 0.6);

                    ray = m_camera->getRay(width, height, x + 0.25, y - 0.25);
                    color2 = traceRay(ray, m_recursion);
                    color.accumulate(color2, 0.6);

                    color /= 1 + 4*0.6;
                }
            }

            for (int yy = y; yy < y + stepSize; ++yy)
            {
                if (yy >= height) break;

                for (int xx = x; xx < x + stepSize; ++xx)
                {
                    if (xx >= width) break;

                    m_image.putPixel(xx, yy, color);
                }
            }

            if (y + stepSize < height)
            {
                m_image.putPixel(x, y + stepSize, Color(1, 0, 0));
            }
        }

        emit lineFinished();
    }

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	std::chrono::duration<int64_t, std::micro> elapsed = std::chrono::duration_cast<std::chrono::duration<int64_t, std::micro>>(t2 - t1);

	return elapsed.count();
}

void RayTracer::cancelRendering()
{
    m_stop = true;
}
