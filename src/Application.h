#pragma once

#include <QApplication>

#include "MainWindow.h"
#include "SettingsWindow.h"
#include "RayTracer.h"
#include "Scene.h"
#include "Camera.h"
#include "ConcurrencyHandler.h"
#include "KeyEventHandler.h"
#include "MouseEventHandler.h"

class Application
{

public:

	static bool m_initialized;
	static std::unique_ptr<QApplication> m_qApplication;
	static std::unique_ptr<MainWindow> m_mainWindow;
	static std::unique_ptr<SettingsWindow> m_settingsWindow;
	static RayTracer m_rayTracer;
	static Scene m_scene;
	static ConcurrencyHandler m_concurrencyHandler;
	static KeyEventHandler m_keyEventHandler;
	static MouseEventHandler m_mouseEventHandler;

	static void init(int& argc, char* argv[]);

	static int run();

	static void cleanUp();
};
