#include "PerspectiveCamera.h"
#include "Utility.h"

#include <sstream>

std::string PerspectiveCamera::DESCRIPTION_LABEL = "PerspectiveCamera";

double PerspectiveCamera::getFOV() const
{
	return m_fov * (180.0 / Constants::PI);
}

void PerspectiveCamera::fromDescription(const std::string& description)
{
	std::vector<std::string> list = Utility::split(description, ',');

	int i = 2;

	m_position = Vector(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_direction = Vector(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_up = Vector(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_fov = std::stod(list[i++]);

	lookAt(m_position, m_position + m_direction, m_up, m_fov * (180 / Constants::PI));
}

void PerspectiveCamera::setFOV(double fov)
{
	m_fov = fov * (Constants::PI / 180.0);
}

void PerspectiveCamera::lookAt(const Vector& eye, const Vector& target, const Vector& sky, double fov)
{
	m_position = eye;
	m_direction = (target - eye).normalize();
	m_right = cross(m_direction, sky).normalize();
	m_up = cross(m_right, m_direction).normalize();
	m_fov = fov * (Constants::PI / 180.0);
}

Ray PerspectiveCamera::getRay(int screenWidth, int screenHeight, double x, double y) const
{
	//0...1
	double ndcX = x / screenWidth;
	double ndcY = y / screenHeight;

	//-1...1
	double pixelScreenX = (2 * ndcX) - 1;
	double pixelScreenY = 1 - (2 * ndcY);

	double aspectRatio = static_cast<double>(screenWidth) / static_cast<double>(screenHeight);

	double pixelCameraX = pixelScreenX * aspectRatio * tan(m_fov / 2);
	double pixelCameraY = pixelScreenY * tan(m_fov / 2);

	return Ray(m_position, (m_direction + pixelCameraX * m_right + pixelCameraY * m_up).normalize());
}

std::unique_ptr<Camera> PerspectiveCamera::clone() const
{
	return std::make_unique<PerspectiveCamera>(*this);
}

std::string PerspectiveCamera::toDescription() const
{
	std::stringstream ss;

	ss << DESCRIPTION_LABEL << ","

		<< 10 << ","

		<< m_position.m_x << ","
		<< m_position.m_y << ","
		<< m_position.m_z << ","
		<< m_direction.m_x << ","
		<< m_direction.m_y << ","
		<< m_direction.m_z << ","
		<< m_up.m_x << ","
		<< m_up.m_y << ","
		<< m_up.m_z << ","
		<< m_fov;

	return ss.str();
}