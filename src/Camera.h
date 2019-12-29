#pragma once

#include "Vector.h"
#include "Ray.h"
#include "EntityDescriptionInterface.h"

class Camera : public EntityDescriptionInterface1
{
protected:

    Vector m_position = Vector(0, 0, 10);
    Vector m_direction = Vector(0, 0, -1);
    Vector m_right = Vector(1, 0, 0);
    Vector m_up = Vector(0, 1, 0);
	double m_stepSize = 1.0;
	double m_rotationCoef = 1.0;

public:

    const Vector& getPosition() const;

    const Vector& getDirection() const;

    const Vector& getRight() const;

    const Vector& getUp() const;

	double getStepSize() const;

	double getRotationCoef() const;

	virtual Ray getRay(int screenWidth, int screenHeight, double x, double y) const = 0;

	virtual std::unique_ptr<Camera> clone() const = 0;

	void setPosition(const Vector& position);

	void setDirection(const Vector& direction);

	void setRight(const Vector& right);

	void setUp(const Vector& up);

	void setStepSize(double speed);

	void setRotationCoef(double speed);

	void moveForward();

	void moveBackward();

	void moveRight();

	void moveLeft();

	void moveUp();

	void moveDown();

    void translate(const Vector& translation);

    void rotate(double degrees, const Vector& axis);
};
