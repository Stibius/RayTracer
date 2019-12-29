
#include "Camera.h"
#include "Matrix.h"
#include "Quaternion.h"

const Vector& Camera::getPosition() const
{
    return m_position;
}

const Vector& Camera::getDirection() const
{
    return m_direction;
}

const Vector& Camera::getRight() const
{
    return m_right;
}

const Vector &Camera::getUp() const
{
    return m_up;
}

double Camera::getStepSize() const
{
	return m_stepSize;
}

double Camera::getRotationCoef() const
{
	return m_rotationCoef;
}

void Camera::setPosition(const Vector& position)
{
    m_position = position;
}

void Camera::setDirection(const Vector& direction)
{
	m_direction = direction;
}

void Camera::setRight(const Vector& right)
{
	m_right = right;
}

void Camera::setUp(const Vector& up)
{
	m_up = up;
}

void Camera::setStepSize(double speed)
{
	m_stepSize = speed;
}

void Camera::setRotationCoef(double speed)
{
	m_rotationCoef = speed;
}

void Camera::moveForward()
{
	translate(m_direction * m_stepSize);
}

void Camera::moveBackward()
{
	translate(-m_direction * m_stepSize);
}

void Camera::moveRight()
{
	translate(m_right * m_stepSize);
}

void Camera::moveLeft()
{
	translate(-m_right * m_stepSize);
}

void Camera::moveUp()
{
	translate(Vector(0.0f, m_stepSize, 0.0f));
}

void Camera::moveDown()
{
	translate(Vector(0.0f, -m_stepSize, 0.0f));
}

void Camera::translate(const Vector& translation)
{
    m_position += translation;
}

void Camera::rotate(double degrees, const Vector& axis)
{
    Matrix rotation = Quaternion::getMatrix(degrees * m_rotationCoef, axis);
    m_direction = normalize(rotation * m_direction);
    m_right = normalize(rotation * m_right);
    m_up = normalize(rotation * m_up);
}

