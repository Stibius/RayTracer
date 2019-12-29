#include "KeyEventHandler.h"

#include <QKeyEvent>

KeyEventHandler::KeyEventHandler(Camera* camera)
	: m_camera(camera)
{
}

void KeyEventHandler::init(Camera* camera)
{
	m_camera = camera;
}

bool KeyEventHandler::keysPressed() const
{
	return key_W || key_S || key_A || key_D || key_Ctrl || key_Space;
}

bool KeyEventHandler::isCameraMoving() const
{
	return m_cameraMoving;
}

bool KeyEventHandler::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

		switch (keyEvent->key())
		{
		case Qt::Key_W:
			m_camera->moveForward();
			key_W = true;
			m_cameraMoving = true;
			emit cameraMoving();
			return true;
		case Qt::Key_S:
			m_camera->moveBackward();
			key_S = true;
			m_cameraMoving = true;
			emit cameraMoving();
			return true;
		case Qt::Key_A:
			m_camera->moveLeft();
			key_A = true;
			m_cameraMoving = true;
			emit cameraMoving();
			return true;
		case Qt::Key_D:
			m_camera->moveRight();
			key_D = true;
			m_cameraMoving = true;
			emit cameraMoving();
			return true;
		case Qt::Key_Space:
			m_camera->moveUp();
			key_Space = true;
			m_cameraMoving = true;
			emit cameraMoving();
			return true;
		case Qt::Key_Control:
			m_camera->moveDown();
			key_Ctrl = true;
			m_cameraMoving = true;
			emit cameraMoving();
			return true;
		default:
			break;
		}
	}
	else if (event->type() == QEvent::KeyRelease)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

		switch (keyEvent->key())
		{
		case Qt::Key_W:
			key_W = false;
			break;
		case Qt::Key_S:
			key_S = false;
			break;
		case Qt::Key_A:
			key_A = false;
			break;
		case Qt::Key_D:
			key_D = false;
			break;
		case Qt::Key_Space:
			key_Space = false;
			break;
		case Qt::Key_Control:
			key_Ctrl = false;
			break;
		default:
			return QObject::eventFilter(obj, event);
		}

		if (!key_W && !key_S && !key_A && !key_D && !key_Ctrl && !key_Space)
		{
			m_cameraMoving = false;
			emit cameraMovingStopped();
		}

		return true;
	}

	return QObject::eventFilter(obj, event);
}
