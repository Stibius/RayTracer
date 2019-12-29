#include "Application.h"
#include "PerspectiveCamera.h"

#include <QGraphicsView>

bool Application::m_initialized = false;
std::unique_ptr<QApplication> Application::m_qApplication;
std::unique_ptr<MainWindow> Application::m_mainWindow;
std::unique_ptr<SettingsWindow> Application::m_settingsWindow;
RayTracer Application::m_rayTracer;
Scene Application::m_scene;
ConcurrencyHandler Application::m_concurrencyHandler;
KeyEventHandler Application::m_keyEventHandler;
MouseEventHandler Application::m_mouseEventHandler;

void Application::init(int& argc, char* argv[])
{
	if (m_initialized == true)
	{
		return;
	}
	m_initialized = true;

	m_qApplication = std::make_unique<QApplication>(argc, argv);
	m_mainWindow = std::make_unique<MainWindow>();
	m_settingsWindow = std::make_unique<SettingsWindow>();

	m_rayTracer.init(&m_scene, m_scene.getCamera());
	m_concurrencyHandler.init(&m_rayTracer);
	m_keyEventHandler.init(m_scene.getCamera());
	m_mouseEventHandler.init(m_scene.getCamera());
	m_mainWindow->installEventFilter(&m_keyEventHandler);
	m_mainWindow->getGraphicsView()->viewport()->installEventFilter(&m_mouseEventHandler);

	QObject::connect(m_mainWindow.get(), &MainWindow::renderPreview, &m_concurrencyHandler, &ConcurrencyHandler::onRenderPreview);
	QObject::connect(m_mainWindow.get(), &MainWindow::render, &m_concurrencyHandler, &ConcurrencyHandler::onRender);
	QObject::connect(m_mainWindow.get(), &MainWindow::cancelRendering, &m_concurrencyHandler, &ConcurrencyHandler::onCancelRendering);

	QObject::connect(&m_concurrencyHandler, &ConcurrencyHandler::renderPreviewFinished, m_mainWindow.get(), &MainWindow::onRenderPreviewFinished);
	QObject::connect(&m_concurrencyHandler, &ConcurrencyHandler::renderFinished, m_mainWindow.get(), &MainWindow::onRenderFinished);

	QObject::connect(&m_keyEventHandler, &KeyEventHandler::cameraMoving, m_mainWindow.get(), &MainWindow::onCameraMoving);
	QObject::connect(&m_keyEventHandler, &KeyEventHandler::cameraMoving, m_settingsWindow.get(), &SettingsWindow::on_cameraPosChanged);
	QObject::connect(&m_keyEventHandler, &KeyEventHandler::cameraMovingStopped, m_mainWindow.get(), &MainWindow::onCameraMovingStopped);

	QObject::connect(&m_mouseEventHandler, &MouseEventHandler::cameraRotating, m_mainWindow.get(), &MainWindow::onCameraRotating);
	QObject::connect(&m_mouseEventHandler, &MouseEventHandler::cameraRotatingStopped, m_mainWindow.get(), &MainWindow::onCameraRotatingStopped);

	QObject::connect(&m_rayTracer, &RayTracer::lineFinished, m_mainWindow.get(), &MainWindow::onLineFinished);
}

int Application::run()
{
	if (m_initialized == false)
	{
		return -1;
	}

	m_mainWindow->showMaximized();

	return m_qApplication->exec();
}

void Application::cleanUp()
{
	if (m_initialized == false)
	{
		return;
	}
	m_initialized = false;

	m_qApplication = nullptr;
	m_mainWindow = nullptr;
}
