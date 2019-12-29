#pragma once

#include <QMainWindow>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicspixmapItem>
#include <QShowEvent>
#include <QResizeEvent>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget* parent = nullptr);

	MainWindow(const MainWindow& other) = delete;

	MainWindow(MainWindow&& other) = delete;

	MainWindow& operator=(const MainWindow& other) = delete;

	MainWindow& operator=(MainWindow&& other) = delete;

    ~MainWindow() override;

	const QGraphicsView* getGraphicsView() const;

protected:

    void showEvent(QShowEvent* event) override;

    void closeEvent (QCloseEvent* event) override;

	void resizeEvent(QResizeEvent* event) override;

private slots:

    void on_actionQuit_triggered();

    void on_actionSettings_triggered();

    void on_actionSaveScene_triggered();

    void on_actionLoadScene_triggered();

	void on_actionSaveImage_triggered();

private:

    Ui::MainWindow* ui;

    QGraphicsScene m_graphicsScene;
    QImage m_image;
    QGraphicsPixmapItem* m_pixmapItem;
    QLabel m_statusLabel;

public slots:

	void onCameraMoving();

	void onCameraMovingStopped();

	void onCameraRotating();

	void onCameraRotatingStopped();

    void onLineFinished();

	void onRenderPreviewFinished();

	void onRenderFinished(int microseconds);

    void onRedraw();

signals:

	void renderPreview();

	void render();

	void cancelRendering();
};
