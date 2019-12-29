#include "OrthoCamera.h"
#include "Utility.h"

#include <sstream>

std::string OrthoCamera::DESCRIPTION_LABEL = "OrthoCamera";

double OrthoCamera::getOrthoWidth() const
{
	return m_orthoWidth;
}

double OrthoCamera::getOrthoHeight() const
{
	return m_orthoHeight;
}

void OrthoCamera::fromDescription(const std::string& description)
{
	std::vector<std::string> list = Utility::split(description, ',');

	int i = 2;

	m_position = Vector(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_direction = Vector(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_up = Vector(std::stod(list[i]), std::stod(list[i + 1]), std::stod(list[i + 2]));
	i += 3;
	m_orthoWidth = std::stod(list[i++]);
	m_orthoHeight = std::stod(list[i++]);

	lookAt(m_position, m_position + m_direction, m_up, m_orthoWidth, m_orthoHeight);
}

void OrthoCamera::setOrthoWidth(double width)
{
	m_orthoWidth = width;
}

void OrthoCamera::setOrthoHeight(double height)
{
	m_orthoHeight = height;
}

void OrthoCamera::lookAt(const Vector& eye, const Vector& target, const Vector& sky, double orthoWidth, double orthoHeight)
{
	m_orthoWidth = orthoWidth;
	m_orthoHeight = orthoHeight;
	m_position = eye;
	m_direction = (target - eye).normalize();
	m_right = cross(m_direction, sky).normalize();
	m_up = cross(m_right, m_direction).normalize();
}

Ray OrthoCamera::getRay(int screenWidth, int screenHeight, double x, double y) const
{
	//0...1
	double ndcX = x / screenWidth;
	double ndcY = y / screenHeight;

	//-1...1
	double pixelScreenX = (2 * ndcX) - 1;
	double pixelScreenY = 1 - (2 * ndcY);

	double aspectRatio = static_cast<double>(screenWidth) / static_cast<double>(screenHeight);

	double pixelCameraX = pixelScreenX * (m_orthoWidth / 2) * aspectRatio;
	double pixelCameraY = pixelScreenY * (m_orthoHeight / 2);

	return Ray(m_position + pixelCameraX * m_right + pixelCameraY * m_up, m_direction);
}

std::unique_ptr<Camera> OrthoCamera::clone() const
{
	return std::make_unique<OrthoCamera>(*this);
}

std::string OrthoCamera::toDescription() const
{
	std::stringstream ss;

	ss << DESCRIPTION_LABEL << ","

		<< 11 << ","

		<< m_position.m_x << ","
		<< m_position.m_y << ","
		<< m_position.m_z << ","
		<< m_direction.m_x << ","
		<< m_direction.m_y << ","
		<< m_direction.m_z << ","
		<< m_up.m_x << ","
		<< m_up.m_y << ","
		<< m_up.m_z << ","
		<< m_orthoWidth << ","
		<< m_orthoHeight;

	return ss.str();
}