#pragma once

#include "Camera.h"

class OrthoCamera : public Camera
{
protected:

	double m_orthoWidth = 5.0;
	double m_orthoHeight = 5.0;

public:

	static std::string DESCRIPTION_LABEL;

	double getOrthoWidth() const;

	double getOrthoHeight() const;

	Ray getRay(int screenWidth, int screenHeight, double x, double y) const override;

	std::unique_ptr<Camera> clone() const override;

	std::string toDescription() const override;

	void fromDescription(const std::string& description) override;

	void setOrthoWidth(double orthoWidth);

	void setOrthoHeight(double orthoHeight);

	void lookAt(const Vector& eye, const Vector& target, const Vector& sky, double orthoWidth, double orthoHeight);
};
