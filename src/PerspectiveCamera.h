#pragma once

#include "Camera.h"
#include "Constants.h"

class PerspectiveCamera : public Camera
{
protected:

	double m_fov = 30.0 * (Constants::PI / 180.0);

public:

	static std::string DESCRIPTION_LABEL;

	double getFOV() const;

	Ray getRay(int screenWidth, int screenHeight, double x, double y) const override;

	std::unique_ptr<Camera> clone() const override;

	std::string toDescription() const override;

	void fromDescription(const std::string& description) override;

	void setFOV(double fov);

	void lookAt(const Vector& eye, const Vector& target, const Vector& sky, double fov);
};
