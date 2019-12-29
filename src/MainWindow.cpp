
#include <QFileDialog>
#include <QMessageBox>

#include "MainWindow.h"
#include "SettingsIO.h"
#include "Application.h"

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->statusBar->addWidget(&m_statusLabel);

    ui->graphicsView->setScene(&m_graphicsScene);
    m_pixmapItem = m_graphicsScene.addPixmap(QPixmap::fromImage(m_image));
    ui->graphicsView->viewport()->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

const QGraphicsView* MainWindow::getGraphicsView() const
{
	return ui->graphicsView;
}

void MainWindow::showEvent(QShowEvent* event)
{
    static bool first = true;
    if (first)
    {
        first = false;
    }
    else
    {
        return;
    }

	emit cancelRendering();

    Application::m_rayTracer.setSize(ui->graphicsView->width(), ui->graphicsView->height());
    m_image = QImage(Application::m_rayTracer.getWidth(), Application::m_rayTracer.getHeight(), QImage::Format_RGB32);
    m_pixmapItem->setPixmap(QPixmap::fromImage(m_image));

	ui->actionSaveImage->setEnabled(false);

    m_statusLabel.setText("Rendering...");

	emit render();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	emit cancelRendering();

    Application::m_settingsWindow->close();

    event->accept();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	static bool first = true;
	if (first)
	{
		first = false;
		return;
	}

	emit cancelRendering();

	QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);

	Application::m_rayTracer.setSize(resizeEvent->size().width(), resizeEvent->size().height());
	m_image = QImage(Application::m_rayTracer.getWidth(), Application::m_rayTracer.getHeight(), QImage::Format_RGB32);
	m_pixmapItem->setPixmap(QPixmap::fromImage(m_image));

	ui->actionSaveImage->setEnabled(false);

	m_statusLabel.setText("Rendering...");

	emit render();
}

void MainWindow::on_actionQuit_triggered()
{
	emit cancelRendering();

    this->close();
}

void MainWindow::onCameraMoving()
{
	ui->actionSaveImage->setEnabled(false);

	m_statusLabel.setText("Moving...");

	//Application::m_settingsWindow->loadCameraSettings();

	emit renderPreview();
}

void MainWindow::onCameraMovingStopped()
{
	if (QApplication::mouseButtons() == Qt::NoButton)
	{
		m_statusLabel.setText("Rendering...");

		emit render();
	}
}

void MainWindow::onCameraRotating()
{
	ui->actionSaveImage->setEnabled(false);

	m_statusLabel.setText("Moving...");

	emit renderPreview();
}

void MainWindow::onCameraRotatingStopped()
{
	if (!Application::m_keyEventHandler.isCameraMoving())
	{
		m_statusLabel.setText("Rendering...");

		emit render();
	}
}

void MainWindow::onLineFinished()
{
    m_pixmapItem->setPixmap(QPixmap::fromImage(m_image));
}

void MainWindow::onRenderFinished(int microseconds)
{
	const unsigned char* imageData = Application::m_rayTracer.getPixmap();
	m_image = QImage(imageData, Application::m_rayTracer.getWidth(), Application::m_rayTracer.getHeight(), QImage::Format_RGB32);
	m_pixmapItem->setPixmap(QPixmap::fromImage(m_image));

	ui->actionSaveImage->setEnabled(true);

	m_statusLabel.setText("Rendering finished in " + QString::number(microseconds / 1000000.0, 'f', 2) + " s!");
}

void MainWindow::onRenderPreviewFinished()
{
	const unsigned char* imageData = Application::m_rayTracer.getPixmap();
	m_image = QImage(imageData, Application::m_rayTracer.getWidth(), Application::m_rayTracer.getHeight(), QImage::Format_RGB32);
	m_pixmapItem->setPixmap(QPixmap::fromImage(m_image));
}

void MainWindow::onRedraw()
{
	ui->actionSaveImage->setEnabled(false);

	emit render();
}

void MainWindow::on_actionSettings_triggered()
{
	Application::m_settingsWindow->show();
	Application::m_settingsWindow->raise();
}

void MainWindow::on_actionSaveScene_triggered()
{
	QString selectedFilter = "CSV files (*.csv)";

    QString fileName = QFileDialog::getSaveFileName(
		this, 
		"Save Scene", 
		"", 
		"All files (*.*);;"
		"CSV files (*.csv);;"
		"Text files (*.txt)",
		&selectedFilter);

	SettingsIO::saveSettings(fileName.toStdString());
}

void MainWindow::on_actionLoadScene_triggered()
{
	QString selectedFilter = "CSV files (*.csv)";

    QString fileName = QFileDialog::getOpenFileName(
		this, 
		"Load Scene", 
		"", 
		"All files (*.*);;"
		"CSV files (*.csv);;"
		"Text files (*.txt)",
		&selectedFilter);

	emit cancelRendering();

	SettingsIO::loadSettings(fileName.toStdString());
	
	Application::m_settingsWindow->loadSettings();

	ui->actionSaveImage->setEnabled(false);
	
	m_statusLabel.setText("Rendering...");

	emit render();
}

void MainWindow::on_actionSaveImage_triggered()
{
	QString selectedFilter = "PNG (*.png)";

	QString filename = QFileDialog::getSaveFileName(
		this,
		"Save Grid",
		QDir::currentPath(),
		"All files (*.*);;"
		"bitmap image (*.bmp);; "
		"JPEG (*.jpg;*.jpeg);;"
		"PNG (*.png);;"
		"GIF (*.gif);;"
		"TIF (*.tif;*.tiff)",
		&selectedFilter);


	if (!filename.isEmpty())
	{
		if (!m_image.save(filename))
		{
			QMessageBox::critical(this, "Error!", "Unable to save to file\n" + filename);
		}
	}
}
