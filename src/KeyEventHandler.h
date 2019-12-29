#pragma once

#include <QObject>
#include <QEvent>

#include "Camera.h"


class KeyEventHandler : public QObject
{
	Q_OBJECT

public:

	KeyEventHandler() = default;

	explicit KeyEventHandler(Camera* camera);

	KeyEventHandler(const KeyEventHandler& other) = default;

	KeyEventHandler(KeyEventHandler&& other) = default;

	KeyEventHandler& operator=(const KeyEventHandler& other) = default;

	KeyEventHandler& operator=(KeyEventHandler&& other) = default;

	~KeyEventHandler() override = default;

	void init(Camera* camera);

	bool keysPressed() const;

	bool isCameraMoving() const;

	bool eventFilter(QObject* obj, QEvent* event) override;

signals:

	void cameraMoving();

	void cameraMovingStopped();

private:

	Camera* m_camera = nullptr;

	bool key_W = false;
	bool key_S = false;
	bool key_A = false;
	bool key_D = false;
	bool key_Ctrl = false;
	bool key_Space = false;

	bool m_cameraMoving = false;
};

