
#include "MouseEventHandler.h"
#include "Application.h"

#include <QMouseEvent>

MouseEventHandler::MouseEventHandler(Camera* camera)
	: m_camera(camera)
{
}

void MouseEventHandler::init(Camera* camera)
{
	m_camera = camera;
}

bool MouseEventHandler::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress) 
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		if (mouseEvent->button() == Qt::LeftButton)
		{
			m_lastLeftMousePos = mouseEvent->pos();

			emit cameraRotating();

			return true;
		}
	}
	else if (event->type() == QEvent::MouseButtonRelease) 
	{
		emit cameraRotatingStopped();

		return true;
	}
	else if (event->type() == QEvent::MouseMove)
	{
		if (!Application::m_concurrencyHandler.isRendering())
		{
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			if (mouseEvent->buttons() & Qt::LeftButton)
			{
				QPointF delta = mouseEvent->pos() - m_lastLeftMousePos;
				m_lastLeftMousePos = mouseEvent->pos();
				m_camera->rotate(-delta.x(), Vector(0.0, 1.0, 0.0));
				m_camera->rotate(-delta.y(), m_camera->getRight());

				emit cameraRotating();
			}
		}
		
		return true;
	}

	return QObject::eventFilter(obj, event);
}

