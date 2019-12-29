#pragma once

#include <QObject>
#include <QPointF>
#include <QEvent>

#include "Camera.h"

class MouseEventHandler : public QObject
{
	Q_OBJECT

public:

	MouseEventHandler() = default;

	explicit MouseEventHandler(Camera* camera);

	MouseEventHandler(const MouseEventHandler& other) = default;

	MouseEventHandler(MouseEventHandler&& other) = default;

	MouseEventHandler& operator=(const MouseEventHandler& other) = default;

	MouseEventHandler& operator=(MouseEventHandler&& other) = default;

	~MouseEventHandler() override = default;

	void init(Camera* camera);

	bool eventFilter(QObject* obj, QEvent* event) override;

signals:

	void cameraRotating();

	void cameraRotatingStopped();

protected:

	Camera* m_camera = nullptr;
	QPointF m_lastLeftMousePos;

};

